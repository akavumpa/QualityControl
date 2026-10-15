#include "TRD/TrackletsReferenceCheck.h"

#include "QualityControl/QcInfoLogger.h"

#include <TFile.h>
#include <TH1.h>
#include <TGraph.h>
#include <TList.h>
#include <TProfile.h>
#include <TAxis.h>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <string>

using namespace o2::quality_control::core;

namespace o2::quality_control_modules::trd
{

namespace
{

// Resolve a relative reference filename against the user's home directory.
std::string resolvePath(const std::string& filename)
{
  if (!filename.empty() && filename[0] == '/') {
    return filename;
  }

  const char* home = std::getenv("HOME");
  if (!home) {
    return filename;
  }

  return std::string(home) + "/" + filename;
}

// Find a current MonitorObject by a distinctive part of its name.
TH1* findHistogram(
  std::map<std::string, std::shared_ptr<MonitorObject>>* moMap,
  const std::string& name)
{
  for (auto& [key, mo] : *moMap) {
    if (key.find(name) != std::string::npos && mo && mo->getObject()) {
      return dynamic_cast<TH1*>(mo->getObject());
    }
  }

  return nullptr;
}

// Compare compatible histograms bin by bin.
Quality compareHistograms(
  TH1* current,
  TH1* reference,
  double tolerance,
  double mediumFraction,
  double badFraction,
  const std::string& name)
{
  if (!current || !reference) {
    ILOG(Error, Ops) << name << ": current or reference histogram is missing"
                     << ENDM;
    return Quality::Bad;
  }

  if (current->GetNbinsX() != reference->GetNbinsX() ||
      std::abs(current->GetXaxis()->GetXmin() -
               reference->GetXaxis()->GetXmin()) > 1.e-9 ||
      std::abs(current->GetXaxis()->GetXmax() -
               reference->GetXaxis()->GetXmax()) > 1.e-9) {
    ILOG(Error, Ops) << name << ": incompatible histogram binning" << ENDM;
    return Quality::Bad;
  }

  int outside = 0;
  const int total = reference->GetNbinsX();

  for (int bin = 1; bin <= total; ++bin) {
    const double ref = reference->GetBinContent(bin);
    const double value = current->GetBinContent(bin);

    // Reference values are expected to be non-negative.
    if (ref < 0.0) {
      ILOG(Error, Ops) << name << ": negative reference bin " << bin << ENDM;
      return Quality::Bad;
    }

    // For a zero reference bin, require the current bin to be zero too.
    if (ref == 0.0) {
      if (value != 0.0) {
        ++outside;
      }
      continue;
    }

    const double lower = (1.0 - tolerance) * ref;
    const double upper = (1.0 + tolerance) * ref;

    if (value < lower || value > upper) {
      ++outside;
    }
  }

  const double fraction =
    total > 0 ? static_cast<double>(outside) / total : 1.0;

  ILOG(Info, Ops) << name << ": " << outside << "/" << total
                  << " bins outside corridor; fraction = " << fraction
                  << ENDM;

  if (fraction > badFraction) {
    return Quality::Bad;
  }

  if (fraction > mediumFraction) {
    return Quality::Medium;
  }

  return Quality::Good;
}

// Draw the reference and the upper/lower corridor as overlays.
void drawReferenceCorridor(
  TH1* current,
  TH1* reference,
  double tolerance)
{
  if (!current || !reference ||
      current->GetNbinsX() != reference->GetNbinsX()) {
    return;
  }

  const int n = reference->GetNbinsX();

  auto* refGraph = new TGraph(n);
  auto* lowGraph = new TGraph(n);
  auto* highGraph = new TGraph(n);

  refGraph->SetName("ReferenceHistogram");
  lowGraph->SetName("ReferenceCorridorLower");
  highGraph->SetName("ReferenceCorridorUpper");

  refGraph->SetLineColor(kBlue + 1);
  refGraph->SetLineStyle(2);
  refGraph->SetLineWidth(2);

  lowGraph->SetLineColor(kMagenta + 1);
  highGraph->SetLineColor(kMagenta + 1);
  lowGraph->SetLineStyle(3);
  highGraph->SetLineStyle(3);
  lowGraph->SetLineWidth(2);
  highGraph->SetLineWidth(2);

  for (int bin = 1; bin <= n; ++bin) {
    const double x = reference->GetXaxis()->GetBinCenter(bin);
    const double ref = reference->GetBinContent(bin);

    refGraph->SetPoint(bin - 1, x, ref);
    lowGraph->SetPoint(bin - 1, x, (1.0 - tolerance) * ref);
    highGraph->SetPoint(bin - 1, x, (1.0 + tolerance) * ref);
  }

  // Keep overlays attached to the current histogram for drawing.
  current->GetListOfFunctions()->Add(refGraph, "L SAME");
  current->GetListOfFunctions()->Add(lowGraph, "L SAME");
  current->GetListOfFunctions()->Add(highGraph, "L SAME");
}

} // namespace

void TrackletsReferenceCheck::configure()
{
  auto getString = [&](const std::string& key, std::string& value) {
    if (mCustomParameters.count(key)) {
      value = mCustomParameters.at(key);
    }
  };

  auto getDouble = [&](const std::string& key, double& value) {
    if (mCustomParameters.count(key)) {
      value = std::stod(mCustomParameters.at(key));
    }
  };

  getString("mQ0ReferenceFile", mQ0ReferenceFile);
  getString("mQ1ReferenceFile", mQ1ReferenceFile);
  getString("mQ2ReferenceFile", mQ2ReferenceFile);
  getString("mPulseHeightReferenceFile", mPulseHeightReferenceFile);

  getDouble("mTolerance", mTolerance);
  getDouble("mMediumFraction", mMediumFraction);
  getDouble("mBadFraction", mBadFraction);

  if (mTolerance < 0.0 || mTolerance >= 1.0 ||
      mMediumFraction < 0.0 ||
      mBadFraction < mMediumFraction ||
      mBadFraction > 1.0) {
    ILOG(Error, Ops) << "Invalid TrackletsReferenceCheck parameters" << ENDM;
    mTolerance = 0.10;
    mMediumFraction = 0.05;
    mBadFraction = 0.20;
  }

  ILOG(Info, Ops) << "TrackletsReferenceCheck configured: tolerance = "
                  << mTolerance << ENDM;
}

Quality TrackletsReferenceCheck::check(
  std::map<std::string, std::shared_ptr<MonitorObject>>* moMap)
{
  if (!moMap) {
    return Quality::Bad;
  }

  Quality overall = Quality::Good;

  auto updateOverall = [&](Quality q) {
    if (q == Quality::Bad) {
      overall = Quality::Bad;
    } else if (q == Quality::Medium && overall == Quality::Good) {
      overall = Quality::Medium;
    }
  };

  auto checkOne = [&](const std::string& moName,
                      const std::string& referenceFile) {
    TH1* current = findHistogram(moMap, moName);

    const std::string path = resolvePath(referenceFile);
    TFile file(path.c_str(), "READ");

    if (file.IsZombie()) {
      ILOG(Error, Ops) << "Cannot open reference file: " << path << ENDM;
      updateOverall(Quality::Bad);
      return;
    }

    auto* reference = dynamic_cast<TH1*>(file.Get("ccdb_object"));

    if (!reference) {
      ILOG(Error, Ops) << "ccdb_object missing in " << path << ENDM;
      updateOverall(Quality::Bad);
      return;
    }

    Quality result = compareHistograms(
      current, reference, mTolerance,
      mMediumFraction, mBadFraction, moName);

    updateOverall(result);
  };

  checkOne("TrackletQ0", mQ0ReferenceFile);
  checkOne("TrackletQ1", mQ1ReferenceFile);
  checkOne("TrackletQ2", mQ2ReferenceFile);
  checkOne("mPulseHeightpro", mPulseHeightReferenceFile);

  ILOG(Info, Ops) << "TrackletsReferenceCheck overall quality: "
                  << overall.getName() << ENDM;

  return overall;
}

void TrackletsReferenceCheck::beautify(
  std::shared_ptr<MonitorObject> mo,
  Quality checkResult)
{
  if (!mo || !mo->getObject()) {
    return;
  }

  auto* current = dynamic_cast<TH1*>(mo->getObject());
  if (!current) {
    return;
  }

  const std::string name = mo->getFullName();
  std::string referenceFile;

  if (name.find("TrackletQ0") != std::string::npos) {
    referenceFile = mQ0ReferenceFile;
  } else if (name.find("TrackletQ1") != std::string::npos) {
    referenceFile = mQ1ReferenceFile;
  } else if (name.find("TrackletQ2") != std::string::npos) {
    referenceFile = mQ2ReferenceFile;
  } else if (name.find("mPulseHeightpro") != std::string::npos) {
    referenceFile = mPulseHeightReferenceFile;
  } else {
    return;
  }

  const std::string path = resolvePath(referenceFile);
  TFile file(path.c_str(), "READ");

  if (!file.IsZombie()) {
    auto* reference = dynamic_cast<TH1*>(file.Get("ccdb_object"));
    if (reference) {
      drawReferenceCorridor(current, reference, mTolerance);
    }
  }

  // Indicate the overall checker result on the current histogram.
  // if (checkResult == Quality::Good) {
  //   current->SetLineColor(kGreen + 2);
  // } else if (checkResult == Quality::Medium) {
  //   current->SetLineColor(kOrange + 7);
  // } else {
  //   current->SetLineColor(kRed);
  // }
}

void TrackletsReferenceCheck::reset()
{
}

} // namespace o2::quality_control_modules::trd
