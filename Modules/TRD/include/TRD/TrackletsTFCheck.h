#ifndef QC_MODULE_TRD_TRACKLETSTFCHECK_H
#define QC_MODULE_TRD_TRACKLETSTFCHECK_H

#include <map>
#include <memory>

#include <TH1F.h>
#include <TPaveText.h>

#include "QualityControl/CheckInterface.h"
#include "QualityControl/MonitorObject.h"

namespace o2::quality_control_modules::trd
{

class TrackletsTFCheck final
  : public o2::quality_control::checker::CheckInterface
{
 public:
  TrackletsTFCheck() = default;
  ~TrackletsTFCheck() override = default;

  /// Called once at startup
  void configure() override;

  /// Main check method
  o2::quality_control::core::Quality
  check(std::map<std::string,
       std::shared_ptr<o2::quality_control::core::MonitorObject>>* moMap) override;

  /// Optional: decorate histogram
  void beautify(std::shared_ptr<o2::quality_control::core::MonitorObject> mo,
                o2::quality_control::core::Quality checkResult =
                  o2::quality_control::core::Quality::Null) override;

  /// Reset internal state
  void reset() override;

 private:
  // thresholds (can later come from JSON)
  float mLowerThresholdTF = 0.f;
  float mUpperThresholdTF = 1e9;

  std::shared_ptr<TPaveText> mMessage;

  ClassDefOverride(TrackletsTFCheck, 1);
};

} // namespace o2::quality_control_modules::trd

#endif // QC_MODULE_TRD_TRACKLETSTFCHECK_H
