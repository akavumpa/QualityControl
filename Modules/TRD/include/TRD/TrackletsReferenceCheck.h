#ifndef QC_MODULE_TRD_TRACKLETSREFERENCECHECK_H
#define QC_MODULE_TRD_TRACKLETSREFERENCECHECK_H

#include "QualityControl/CheckInterface.h"
#include "QualityControl/MonitorObject.h"
#include "QualityControl/Quality.h"

#include <map>
#include <memory>
#include <string>

namespace o2::quality_control_modules::trd
{

class TrackletsReferenceCheck
  : public o2::quality_control::checker::CheckInterface
{
 public:
  TrackletsReferenceCheck() = default;
  ~TrackletsReferenceCheck() override = default;

  void configure() override;

  o2::quality_control::core::Quality check(
    std::map<std::string,
             std::shared_ptr<o2::quality_control::core::MonitorObject>>* moMap)
    override;

  void beautify(
    std::shared_ptr<o2::quality_control::core::MonitorObject> mo,
    o2::quality_control::core::Quality checkResult) override;

  void reset() override;

 private:
  std::string mQ0ReferenceFile = "Reference_Q0.root";
  std::string mQ1ReferenceFile = "Reference_Q1.root";
  std::string mQ2ReferenceFile = "Reference_Q2.root";
  std::string mPulseHeightReferenceFile = "Reference_PulseHeight.root";

  double mTolerance = 0.10;

  // Provisional limits for the fraction of bins outside the corridor.
  // These values must be validated before being used for production QC.
  double mMediumFraction = 0.05;
  double mBadFraction = 0.20;

  ClassDefOverride(TrackletsReferenceCheck, 1);
};

} // namespace o2::quality_control_modules::trd

#endif
