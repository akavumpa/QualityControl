#ifndef QC_MODULE_TRD_TRACKLETSTFCHECK_H
#define QC_MODULE_TRD_TRACKLETSTFCHECK_H

#include <TH1F.h>
#include "QualityControl/CheckInterface.h"
#include "QualityControl/MonitorObject.h"

namespace o2::quality_control_modules::trd
{

class TrackletsTFCheck : public o2::quality_control::checker::CheckInterface
{
 public:
  TrackletsTFCheck() = default;
  ~TrackletsTFCheck() override = default;

  void configure() override {}
  Quality check(std::map<std::string, std::shared_ptr<MonitorObject>>* moMap) override;
  void beautify(std::shared_ptr<MonitorObject>, Quality) override {}
  void reset() override {}

  ClassDefOverride(TrackletsTFCheck, 1);
};

} // namespace o2::quality_control_modules::trd

#endif
