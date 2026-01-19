#ifndef QC_MODULE_TRD_TRACKLETSTFCHECK_H
#define QC_MODULE_TRD_TRACKLETSTFCHECK_H

#include "QualityControl/CheckInterface.h"
#include "QualityControl/MonitorObject.h"
#include "QualityControl/Quality.h"

#include <TH1.h>
#include <TPaveText.h>
#include <map>
#include <memory>
#include <string>

namespace o2::quality_control_modules::trd
{

class TrackletsTFCheck : public o2::quality_control::checker::CheckInterface
{
 public:
  TrackletsTFCheck() = default;
  ~TrackletsTFCheck() override = default;

  void configure() override;
  o2::quality_control::core::Quality
  check(std::map<std::string,
        std::shared_ptr<o2::quality_control::core::MonitorObject>>* moMap) override;
  void beautify(std::shared_ptr<o2::quality_control::core::MonitorObject>,
                o2::quality_control::core::Quality) override;
  void reset() override;

 private:
  float mLowerThresholdTF = 1e4;
  float mUpperThresholdTF = 5e4;

  ClassDefOverride(TrackletsTFCheck, 1);
};

} // namespace o2::quality_control_modules::trd

#endif
