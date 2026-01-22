#include "TRD/TrackletsTFCheck.h"

#include "QualityControl/QcInfoLogger.h"
#include "QualityControl/Quality.h"
#include "QualityControl/MonitorObject.h"

#include <TH1.h>


using namespace o2::quality_control::core;

namespace o2::quality_control_modules::trd
{

void TrackletsTFCheck::configure()
{
  // nothing for now
}

Quality TrackletsTFCheck::check(
  std::map<std::string, std::shared_ptr<MonitorObject>>* moMap)
{
  // 1. Look for the histogram
  auto it = moMap->find("nTrackletsTF");
  if (it == moMap->end()) {
    // DO NOT return Null
    return Quality::Bad;
  }

  // 2. Check it is a histogram
  auto* h = dynamic_cast<TH1*>(it->second->getObject());
  if (!h || h->GetEntries() == 0) {
    // Startup / empty cycles
    return Quality::Medium;
  }

  // 3. Compute mean tracklets per TF
  double mean = h->GetMean();

  ILOG(Info, Ops)
  << "TrackletsTFCheck DEBUG: mean=" << mean
  << " lower=" << mLowerThresholdTF
  << " upper=" << mUpperThresholdTF
  << ENDM;

  // 4. Apply thresholds
  if (mean < mLowerThresholdTF) {
    return Quality::Bad;
  }
  if (mean < mUpperThresholdTF) {
    return Quality::Medium;
  }

  return Quality::Good;
}

void TrackletsTFCheck::beautify(std::shared_ptr<MonitorObject>,
                                Quality)
{
  // DO NOTHING (important!)
}

void TrackletsTFCheck::reset()
{
  // nothing
}

} // namespace o2::quality_control_modules::trd
