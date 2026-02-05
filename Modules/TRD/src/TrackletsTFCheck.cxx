#include "TRD/TrackletsTFCheck.h"
#include "QualityControl/QcInfoLogger.h"
#include "QualityControl/Quality.h"
#include "QualityControl/MonitorObject.h"
#include <TH1.h>

using namespace o2::quality_control::core;

namespace o2::quality_control_modules::trd
{

// ---------- Helper Functions ----------

Quality checkMean(TH1* h, double low, double high, const std::string& name) // Function that evaluates histogram mean
{
  if (!h || h->GetEntries() == 0) {
    ILOG(Warning, Ops) << name << " empty" << ENDM;
    return Quality::Bad;
  } // Detector probably dead / data missing

  double mean = h->GetMean();
  ILOG(Info, Ops) << name << " mean=" << mean << ENDM;

  if (mean < low)
    return Quality::Bad;
  if (mean <= high)
    return Quality::Good;
  return Quality::Medium;
}
// Low → detector off
// Normal range → Good
// Too high → suspicious noise

Quality checkEntries(TH1* h, double minEntries, const std::string& name) // Used to verify histogram has enough statistics
{
  if (!h)
    return Quality::Bad;

  if (h->GetEntries() < minEntries) {
    ILOG(Warning, Ops) << name << " low statistics" << ENDM;
    return Quality::Bad;
  }
  return Quality::Good;
}
// If entries < threshold → Bad
// Else → Good
// Used for Q0/Q1/Q2 decoding sanity

Quality checkEmptyBins(TH1* h, double maxFrac, const std::string& name) // // Detects dead detector regions
{
  if (!h)
    return Quality::Bad;

  int empty = 0;
  int total = h->GetNbinsX();
  // check for bincontent of each bins of a histogram: frac = #emptyBins/#totalBins
  for (int i = 1; i <= total; ++i)
    if (h->GetBinContent(i) == 0)
      empty++;

  double frac = (double)empty / total;

  ILOG(Info, Ops) << name << " empty fraction=" << frac << ENDM;

  if (frac > maxFrac)
    return Quality::Bad; // If too many empty → detector coverage problem
  return Quality::Good;
}

Quality checkEmptyBins2D(TH2* h, double maxFrac, const std::string& name) // // Detects dead detector regions
{
  if (!h)
    return Quality::Bad;

  int empty = 0;
  int nx = h->GetNbinsX();
  int ny = h->GetNbinsY();
  int total = nx * ny;
  // check for bincontent of each bins of a histogram: frac = #emptyBins/#totalBins
  for (int ix = 1; ix <= nx; ++ix) {
    for (int iy = 1; iy <= ny; ++iy) {
      if (h->GetBinContent(ix, iy) == 0) {
        empty++;
      }
    }
  }
  double frac = (double)empty / total;

  ILOG(Info, Ops) << name << " empty fraction=" << frac << ENDM;

  if (frac > maxFrac)
    return Quality::Bad; // If too many empty → detector coverage problem
  return Quality::Good;
}

//------------Variables------------
void TrackletsTFCheck::configure() // Reads thresholds from JSON
{
  auto get = [&](const std::string& key, float& var) {
    if (mCustomParameters.count(key)) {
      var = std::stof(mCustomParameters.at(key));
    } else {
      ILOG(Warning, Ops) << "Parameter " << key << " not provided, using default " << var << ENDM;
    }
  };

  get("mTFMeanLow", mTFMeanLow);
  get("mTFMeanHigh", mTFMeanHigh);

  get("mEventMeanLow", mEventMeanLow);
  get("mEventMeanHigh", mEventMeanHigh);

  get("mQEntriesMin", mQEntriesMin);
  get("mQMeanLow", mQMeanLow);
  get("mQMeanHigh", mQMeanHigh);

  get("mChamberMaxEmptyFrac", mChamberMaxEmptyFrac);

  ILOG(Info, Ops)
    << "TrackletsTFCheck configured with TFMeanLow=" << mTFMeanLow
    << " TFMeanHigh=" << mTFMeanHigh
    << ENDM;
}

// ---------- MAIN CHECK ----------

Quality TrackletsTFCheck::check(
  std::map<std::string, std::shared_ptr<MonitorObject>>* moMap)
{
  Quality finalQ = Quality::Good;
  // Function to keep worst result
  auto worst = [&](Quality q) {
    if (q == Quality::Bad)
      finalQ = Quality::Bad;
    else if (q == Quality::Medium && finalQ == Quality::Good)
      finalQ = Quality::Medium;
  };
  // If any check returns Bad → final = Bad
  // Else if Medium appears → final = Medium

  // Looks up histogram in MO map
  auto getH = [&](const std::string& name) -> TH1* {
    auto it = moMap->find(name);
    if (it == moMap->end()) {
      ILOG(Error, Ops) << "Missing MO: " << name << ENDM;
      finalQ = Quality::Bad;
      return nullptr;
    }
    return dynamic_cast<TH1*>(it->second->getObject());
  };

  // ---------- Global Detector Activity ----------
  // Is TRD producing tracklets?
  worst(checkMean(getH("Tracklets/trackletspertimeframe"), mTFMeanLow, mTFMeanHigh, "trackletspertimeframe"));
  worst(checkMean(getH("Tracklets/trackletspereventPbPb"), mEventMeanLow, mEventMeanHigh, "trackletspereventPbPb"));
  // worst(checkMean(getH("TrdTrySkelton/nTrackletsTF"), mTFMeanLow, mTFMeanHigh, "nTrackletsTF"));
  // worst(checkMean(getH("TrdTrySkelton/nTrackletsEVENT"), mEventMeanLow, mEventMeanHigh, "nTrackletsEVENT"));

  // // ---------- Signal Quality ----------
  // // Are ADC histograms filled?
  // worst(checkEntries(getH("TrdTrySkelton/Q0"), mQEntriesMin, "Q0"));
  // worst(checkEntries(getH("TrdTrySkelton/Q1"), mQEntriesMin, "Q1"));
  // worst(checkEntries(getH("TrdTrySkelton/Q2"), mQEntriesMin, "Q2"));
  worst(checkEntries(getH("Tracklets/TrackletQ0"), mQEntriesMin, "TrackletQ0"));
  worst(checkEntries(getH("Tracklets/TrackletQ1"), mQEntriesMin, "TrackletQ1"));
  worst(checkEntries(getH("Tracklets/TrackletQ2"), mQEntriesMin, "TrackletQ2"));
  // // Are charge values in normal range?
  // worst(checkMean(getH("TrdTrySkelton/Q0"), mQMeanLow, mQMeanHigh, "Q0 mean"));
  // worst(checkMean(getH("TrdTrySkelton/Q1"), mQMeanLow, mQMeanHigh, "Q1 mean"));
  // worst(checkMean(getH("TrdTrySkelton/Q2"), mQMeanLow, mQMeanHigh, "Q2 mean"));
  worst(checkMean(getH("Tracklets/TrackletQ0"), mQMeanLow, mQMeanHigh, "Q0 mean"));
  worst(checkMean(getH("Tracklets/TrackletQ1"), mQMeanLow, mQMeanHigh, "Q1 mean"));
  worst(checkMean(getH("Tracklets/TrackletQ2"), mQMeanLow, mQMeanHigh, "Q2 mean"));

  // // ---------- Geometry Coverage ----------
  // // Detect dead modules/stacks
  // worst(checkEmptyBins(getH("TrdTrySkelton/Chamber"), mChamberMaxEmptyFrac, "Chamber"));
  // worst(checkEmptyBins(getH("TrdTrySkelton/PadRow"), mPadRowMaxEmptyFrac, "PadRow"));
  worst(checkEmptyBins2D(dynamic_cast<TH2*>(getH("Tracklets/trackletsperHC2D")), mChamberMaxEmptyFrac, "trackletsperHC2D"));

  // // ---------- Electronics Load ----------
  // // Detect noisy MCMs
  // worst(checkMean(getH("TrdTrySkelton/MCMTrackletPerMCM"), mMCMLoadLow, mMCMLoadHigh, "MCMTrackletPerMCM"));

  return finalQ;
}

void TrackletsTFCheck::beautify(std::shared_ptr<MonitorObject>, Quality) {}
void TrackletsTFCheck::reset() {}

} // namespace o2::quality_control_modules::trd
