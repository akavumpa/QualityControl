#ifndef QC_MODULE_TRD_TRACKLETSTFCHECK_H
#define QC_MODULE_TRD_TRACKLETSTFCHECK_H

#include "QualityControl/CheckInterface.h"
#include "QualityControl/MonitorObject.h"
#include "QualityControl/Quality.h"

#include <TH1.h>
#include <TH2.h>
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
  //   float mLowerThresholdTF = 1e4;
  //   float mUpperThresholdTF = 5e5;
  // ---------- Global Activity ----------
  float mTFMeanLow = 1e4;
  float mTFMeanHigh = 5e5;

  float mEventMeanLow = 100;
  float mEventMeanHigh = 10e3;

  // ---------- Signal Quality (ADC) ----------
  float mQEntriesMin = 1000;
  float mQMeanLow = 20;
  float mQMeanHigh = 50;

  // ---------- Geometry Coverage ----------
  float mChamberMaxEmptyFrac = 0.3;
  float mPadRowMaxEmptyFrac = 0.4;

  // ---------- Electronics Load ----------
  float mMCMLoadLow = 1.0;
  float mMCMLoadHigh = 3.0;

  // ------------- PulseHeight ----------
  // ---------- Pulse Height ----------
  float mDriftRegionStart = 7;
  float mDriftRegionEnd = 20;

  float mPeakRegionStart = 1;
  float mPeakRegionEnd = 5;

  float mPulseHeightRatio = 1.1;

  // // ---------- Helper functions ----------
  // void checkGlobalActivity(
  //   std::map<std::string,
  //            std::shared_ptr<o2::quality_control::core::MonitorObject>>* moMap,
  //   o2::quality_control::core::Quality& finalQ);

  // void checkCharge(
  //   std::map<std::string,
  //            std::shared_ptr<o2::quality_control::core::MonitorObject>>* moMap,
  //   o2::quality_control::core::Quality& finalQ);

  // void checkGeometry(
  //   std::map<std::string,
  //            std::shared_ptr<o2::quality_control::core::MonitorObject>>* moMap,
  //   o2::quality_control::core::Quality& finalQ);

  // void checkPulseHeight(
  //   std::map<std::string,
  //            std::shared_ptr<o2::quality_control::core::MonitorObject>>* moMap,
  //   o2::quality_control::core::Quality& finalQ);

  // ---------- Individual quality checks ----------

  o2::quality_control::core::Quality checkGlobalActivity(
    std::map<std::string,
             std::shared_ptr<o2::quality_control::core::MonitorObject>>* moMap);

  o2::quality_control::core::Quality checkCharge(
    std::map<std::string,
             std::shared_ptr<o2::quality_control::core::MonitorObject>>* moMap);

  o2::quality_control::core::Quality checkGeometry(
    std::map<std::string,
             std::shared_ptr<o2::quality_control::core::MonitorObject>>* moMap);

  o2::quality_control::core::Quality checkPulseHeight(
    std::map<std::string,
             std::shared_ptr<o2::quality_control::core::MonitorObject>>* moMap);

  // ---------- Individual quality results ----------
  o2::quality_control::core::Quality mGlobalActivityQuality =
    o2::quality_control::core::Quality::Good;

  o2::quality_control::core::Quality mChargeQuality =
    o2::quality_control::core::Quality::Good;

  o2::quality_control::core::Quality mGeometryQuality =
    o2::quality_control::core::Quality::Good;

  o2::quality_control::core::Quality mPulseHeightQuality =
    o2::quality_control::core::Quality::Good;

  ClassDefOverride(TrackletsTFCheck, 1);
};

} // namespace o2::quality_control_modules::trd

#endif
