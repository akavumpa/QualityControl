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
  Quality q;
  q.setName("TrackletsTFCheck");

  auto it = moMap->find("nTrackletsTF");
  if (it == moMap->end()) {
    q.setLevel(QualityLevel::Bad);
    return q;
  }

  auto* h = dynamic_cast<TH1*>(it->second->getObject());
  if (!h || h->GetEntries() == 0) {
    q.setLevel(QualityLevel::Medium);
    return q;
  }

  double mean = h->GetMean();

  if (mean < mLowerThresholdTF) {
    q.setLevel(QualityLevel::Bad);
  } else if (mean < mUpperThresholdTF) {
    q.setLevel(QualityLevel::Medium);
  } else {
    q.setLevel(QualityLevel::Good);
  }

  return q;
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
