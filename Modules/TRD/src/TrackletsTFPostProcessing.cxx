#include "TRD/TrackletsTFPostProcessing.h"
#include "QualityControl/QcInfoLogger.h"
#include "QualityControl/DatabaseInterface.h"
#include "QualityControl/MonitorObject.h"
#include "TRD/TrackletsTFCheck.h"

#include <TH1.h>

using namespace o2::quality_control::repository;
using namespace o2::quality_control::core;

namespace o2::quality_control::postprocessing
{

void TrackletsTFPostProcessing::configure(
  const boost::property_tree::ptree&)
{
}

void TrackletsTFPostProcessing::initialize(
  Trigger,
  framework::ServiceRegistryRef)
{
  ILOG(Info, Support)
    << "TrackletsTFPostProcessing initialized"
    << ENDM;

  const auto& defaults = mCustomParameters.getAllDefaults();

  ILOG(Info, Support)
    << "PostProcessing received "
    << defaults.size()
    << " default custom parameters"
    << ENDM;

  for (const auto& [key, value] : defaults) {
    ILOG(Info, Support)
      << key << " = " << value
      << ENDM;
  }
}

void TrackletsTFPostProcessing::update(
  Trigger t,
  framework::ServiceRegistryRef services)
{
  auto& qcdb = services.get<DatabaseInterface>();
  retrieveObjects(t, qcdb);
}

void TrackletsTFPostProcessing::finalize(
  Trigger,
  framework::ServiceRegistryRef)
{
}

void TrackletsTFPostProcessing::retrieveObjects(
  const Trigger& t,
  repository::DatabaseInterface& qcdb)
{
  std::vector<std::pair<std::string, std::string>> monitorObjectNames = {
    { "Tracklets", "TrackletQ0" },
    { "Tracklets", "TrackletQ1" },
    { "Tracklets", "TrackletQ2" },
    { "Tracklets", "trackletsperHC2D" },
    { "Tracklets", "trackletspertimeframe" },
    { "Tracklets", "trackletspereventPbPb" },

    { "PulseHeight", "mPulseHeight" },
    { "PulseHeight", "mPulseHeightpro" }
  };

  // const long long ts = 1708707000000;
  mMonitorObjects.clear();

  //============================================
  ILOG(Info, Support)
    << "Timestamp : " << t.timestamp
    << ENDM;

  ILOG(Info, Support)
    << "Run : " << t.activity.mId
    << ENDM;

  ILOG(Info, Support)
    << "Pass : " << t.activity.mPassName
    << ENDM;

  ILOG(Info, Support)
    << "Period : " << t.activity.mPeriodName
    << ENDM;

  ILOG(Info, Support)
    << "Provenance : " << t.activity.mProvenance
    << ENDM;
  //===============================================

  for (auto const& [folder, name] : monitorObjectNames) {

    //   auto mo =
    //     qcdb.retrieveMO(
    //       "TRD/MO/" + folder,
    //       name,
    //       ts);
    //   // t.timestamp);

    auto mo =
      qcdb.retrieveMO(
        "TRD/MO/" + folder,
        name,
        t.timestamp,
        t.activity);

    if (!mo) {
      ILOG(Warning, Support)
        << "Could not retrieve "
        << name
        << ENDM;
      continue;
    }

    mMonitorObjects.emplace(mo->getFullName(), mo);

    auto h =
      dynamic_cast<TH1*>(mo->getObject());

    if (!h) {
      ILOG(Warning, Support)
        << name
        << " is not a TH1"
        << ENDM;
      continue;
    }

    ILOG(Info, Support)
      << name
      << " entries = "
      << h->GetEntries()
      << " mean = "
      << h->GetMean()
      << ENDM;
  }

  ILOG(Info, Support)
    << "Retrieved "
    << mMonitorObjects.size()
    << " MonitorObjects from QCDB"
    << ENDM;

  o2::quality_control_modules::trd::TrackletsTFCheck checker;

  checker.setCustomParameters(mCustomParameters);

  auto quality = checker.check(&mMonitorObjects);

  for (auto& [name, mo] : mMonitorObjects) {
    checker.beautify(mo, quality);
  }

  ILOG(Info, Support)
    << "TrackletsTFCheck finished. Quality = "
    << quality.getName()
    << ENDM;
}

} // namespace o2::quality_control::postprocessing