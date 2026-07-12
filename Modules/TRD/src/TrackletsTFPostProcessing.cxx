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
  std::vector<std::string> monitorObjectNames = {
    "TrackletQ0",
    "TrackletQ1",
    "TrackletQ2",
    "trackletsperHC2D",
    "trackletspertimeframe",
    "trackletspereventPbPb"
  };

  const long long ts = 1708707000000;
  mMonitorObjects.clear();

  for (auto const& name : monitorObjectNames) {
    auto mo =
      qcdb.retrieveMO(
        "TRD/MO/Tracklets",
        name,
        ts);
    // t.timestamp);

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

  checker.configure();

  auto quality = checker.check(&mMonitorObjects);

  ILOG(Info, Support)
    << "TrackletsTFCheck finished."
    << ENDM;
}

} // namespace o2::quality_control::postprocessing