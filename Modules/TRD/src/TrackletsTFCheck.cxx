#include "TRD/TrackletsTFCheck.h"

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
  auto it = moMap->find("nTrackletsTF");
  if (it == moMap->end()) {
    return Quality::Null;
  }

  auto* h = dynamic_cast<TH1*>(it->second->getObject());
  if (!h) {
    return Quality::Null;
  }

  double mean = h->GetMean();

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
