#include "TRD/TrackletsTFPostProcessing.h"

#include "QualityControl/DatabaseInterface.h"
#include "QualityControl/MonitorObject.h"

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
  framework::ServiceRegistryRef)
{
  auto& qcdb =
    t.services().get<repository::DatabaseInterface>();

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
  std::vector<std::string> names = {
    "TrackletQ0",
    "TrackletQ1",
    "TrackletQ2",
    "trackletsperHC2D",
    "trackletspertimeframe",
    "trackletspereventPbPb"
  };

  for (auto const& name : names) {

    auto mo =
      qcdb.retrieveMO(
        "TRD/MO/Tracklets",
        name,
        t.timestamp);

    if (!mo) {
      ILOG(Warning, Support)
        << "Could not retrieve "
        << name
        << ENDM;
      continue;
    }

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
}

} // namespace o2::quality_control::postprocessing