#include "TRD/TrackletsTFCheck.h"
#include "QualityControl/QcInfoLogger.h"
#include "QualityControl/Quality.h"
#include "QualityControl/MonitorObject.h"
#include <TH1.h>

using namespace o2::quality_control::core;

namespace o2::quality_control_modules::trd
{

// ---------- Helper Functions ----------

Quality checkMean(TH1* h, double low, double high, const std::string& name)
{
  if (!h || h->GetEntries() == 0) {
    ILOG(Warning, Ops) << name << " empty" << ENDM;
    return Quality::Bad;
  }

  double mean = h->GetMean();
  ILOG(Info, Ops) << name << " mean=" << mean << ENDM;

  if (mean < low)
    return Quality::Bad;
  if (mean <= high)
    return Quality::Good;
  return Quality::Medium;
}

Quality checkEntries(TH1* h, double minEntries, const std::string& name)
{
  if (!h)
    return Quality::Bad;

  if (h->GetEntries() < minEntries) {
    ILOG(Warning, Ops) << name << " low statistics" << ENDM;
    return Quality::Bad;
  }
  return Quality::Good;
}

Quality checkEmptyBins(TH1* h, double maxFrac, const std::string& name)
{
  if (!h)
    return Quality::Bad;

  int empty = 0;
  int total = h->GetNbinsX();

  for (int i = 1; i <= total; ++i)
    if (h->GetBinContent(i) == 0)
      empty++;

  double frac = (double)empty / total;

  ILOG(Info, Ops) << name << " empty fraction=" << frac << ENDM;

  if (frac > maxFrac)
    return Quality::Bad;
  return Quality::Good;
}

//------------Variables------------
void TrackletsTFCheck::configure()
{
  mTFMeanLow = std::stof(getParameter("mTFMeanLow"));
  mTFMeanHigh = std::stof(getParameter("mTFMeanHigh"));

  mEventMeanLow = std::stof(getParameter("mEventMeanLow"));
  mEventMeanHigh = std::stof(getParameter("mEventMeanHigh"));

  mQEntriesMin = std::stof(getParameter("mQEntriesMin"));
  mQMeanLow = std::stof(getParameter("mQMeanLow"));
  mQMeanHigh = std::stof(getParameter("mQMeanHigh"));

  mChamberMaxEmptyFrac = std::stof(getParameter("mChamberMaxEmptyFrac"));
  mPadRowMaxEmptyFrac = std::stof(getParameter("mPadRowMaxEmptyFrac"));

  mMCMLoadLow = std::stof(getParameter("mMCMLoadLow"));
  mMCMLoadHigh = std::stof(getParameter("mMCMLoadHigh"));

  ILOG(Info, Ops)
    << "TrackletsTFCheck configured: "
    << "TFMeanLow=" << mTFMeanLow
    << " TFMeanHigh=" << mTFMeanHigh
    << " QMeanLow=" << mQMeanLow
    << " QMeanHigh=" << mQMeanHigh
    << ENDM;
}

// ---------- MAIN CHECK ----------

Quality TrackletsTFCheck::check(
  std::map<std::string, std::shared_ptr<MonitorObject>>* moMap)
{
  Quality finalQ = Quality::Good;

  auto worst = [&](Quality q) {
    if (q == Quality::Bad)
      finalQ = Quality::Bad;
    else if (q == Quality::Medium && finalQ == Quality::Good)
      finalQ = Quality::Medium;
  };

  auto getH = [&](const std::string& name) -> TH1* {
    auto it = moMap->find(name);
    if (it == moMap->end()) {
      ILOG(Error, Ops) << "Missing MO: " << name << ENDM;
      finalQ = Quality::Bad;
      return nullptr;
    }
    return dynamic_cast<TH1*>(it->second->getObject());
  };

  // ---------- Global Activity ----------
  worst(checkMean(getH("TrdTrySkelton/nTrackletsTF"), mTFMeanLow, mTFMeanHigh, "nTrackletsTF"));
  worst(checkMean(getH("TrdTrySkelton/nTrackletsEVENT"), mEventMeanLow, mEventMeanHigh, "nTrackletsEVENT"));

  // ---------- Signal Quality ----------
  worst(checkEntries(getH("TrdTrySkelton/Q0"), mQEntriesMin, "Q0"));
  worst(checkEntries(getH("TrdTrySkelton/Q1"), mQEntriesMin, "Q1"));
  worst(checkEntries(getH("TrdTrySkelton/Q2"), mQEntriesMin, "Q2"));

  worst(checkMean(getH("TrdTrySkelton/Q0"), mQMeanLow, mQMeanHigh, "Q0 mean"));
  worst(checkMean(getH("TrdTrySkelton/Q1"), mQMeanLow, mQMeanHigh, "Q1 mean"));
  worst(checkMean(getH("TrdTrySkelton/Q2"), mQMeanLow, mQMeanHigh, "Q2 mean"));

  // ---------- Geometry Coverage ----------
  worst(checkEmptyBins(getH("TrdTrySkelton/Chamber"), mChamberMaxEmptyFrac, "Chamber"));
  worst(checkEmptyBins(getH("TrdTrySkelton/PadRow"), mPadRowMaxEmptyFrac, "PadRow"));

  // ---------- Electronics Load ----------
  worst(checkMean(getH("TrdTrySkelton/MCMTrackletPerMCM"), mMCMLoadLow, mMCMLoadHigh, "MCMTrackletPerMCM"));

  return finalQ;
}

void TrackletsTFCheck::beautify(std::shared_ptr<MonitorObject>, Quality) {}
void TrackletsTFCheck::reset() {}

} // namespace o2::quality_control_modules::trd
