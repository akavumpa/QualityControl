#include <TCanvas.h>
#include <TH1.h>

#include "QualityControl/QcInfoLogger.h"
#include "TRD/TrdTrySkeltonTask.h"
#include <Framework/InputRecordWalker.h>
#include <Framework/DataRefUtils.h>

#include "DataFormatsTRD/Tracklet64.h"
#include "QualityControl/MonitorObject.h"
#include "DataFormatsTRD/TrackTRD.h"

namespace o2::quality_control_modules::trd
{

TrdTrySkeltonTask::~TrdTrySkeltonTask()
{
}

void TrdTrySkeltonTask::initialize(o2::framework::InitContext& /*ctx*/)
{
  // This is how logs are created. QcInfoLogger is used. In production, FairMQ logs will go to InfoLogger as well.
  ILOG(Debug, Devel) << "initialize TrdTrySkeltonTask" << ENDM;
  ILOG(Debug, Support) << "A debug targeted for support" << ENDM;
  ILOG(Info, Ops) << "An Info log targeted for operators" << ENDM;

  ILOG(Info, Ops) << "Initializing histograms" << ENDM;

  // This creates and registers a histogram for publication at the end of each cycle, until the end of the task lifetime
  mHistogramA = std::make_unique<TH1F>("nTracklets", "Number of TRD Tracklets per Event", 100, 0, 1000);
  // mHistogramB = std::make_unique<TH1F>("nTracks", "Number of TRD Tracks per Event", 100, 0, 200);
  // mHistogramC = std::make_unique<TH1F>("adcSum", "ADC Sum of TRD Tracklets", 256, 0, 1024);
  // mHistogramD = std::make_unique<TH1F>("trkletsPerTrack", "Number of Tracklets per TRD Track", 10, 0, 10);

  getObjectsManager()->startPublishing(mHistogramA.get(), PublicationPolicy::Forever);
  // getObjectsManager()->startPublishing(mHistogramB.get(), PublicationPolicy::Forever);
  // getObjectsManager()->startPublishing(mHistogramC.get(), PublicationPolicy::Forever);
  // getObjectsManager()->startPublishing(mHistogramD.get(), PublicationPolicy::Forever);

  try {
    getObjectsManager()->addMetadata(mHistogramA->GetName(), "custom", "34");
  } catch (...) {
    ILOG(Warning, Support) << "Metadata could not be added to " << mHistogramA->GetName() << ENDM;
  }
}

void TrdTrySkeltonTask::startOfActivity(const Activity& activity)
{
  ILOG(Debug, Devel) << "startOfActivity " << activity.mId << ENDM;

  // We clean any histograms that could have been filled in previous runs.
  mHistogramA->Reset();
}

void TrdTrySkeltonTask::startOfCycle()
{
  // THUS FUNCTION BODY IS AN EXAMPLE. PLEASE REMOVE EVERYTHING YOU DO NOT NEED.
  ILOG(Debug, Devel) << "startOfCycle" << ENDM;
}

void TrdTrySkeltonTask::monitorData(o2::framework::ProcessingContext& ctx)
{
  // Get TRD tracklets
  if (ctx.inputs().contains("tracklets")) {
    auto tracklets = ctx.inputs().get<gsl::span<o2::trd::Tracklet64>>("tracklets");
    LOG(INFO) << "Number of tracklets: " << tracklets.size();
  } else {
    LOG(WARNING) << "No tracklets found in this input!";
  }

  // // Get TRD tracks
  // auto tracks = ctx.inputs().get<gsl::span<o2::trd::TrackTRD>>("tracks");

  // Count and fill histograms
  int nTracklets = tracklets.size();
  // int nTracks = tracks.size();
  mHistogramA->Fill(nTracklets); // Histogram A: number of tracklets
  // mHistogramB->Fill(nTracks);

  // for (const auto& trk : tracks) {
  //   int nTrkl = trk.getNtracklets(); // or size of trk.getTracklets()
  //   mHistogramD->Fill(nTrkl);        // tracklets per track
  // }
}

void TrdTrySkeltonTask::endOfCycle()
{
  // THIS FUNCTION BODY IS AN EXAMPLE. PLEASE REMOVE EVERYTHING YOU DO NOT NEED.
  ILOG(Debug, Devel) << "endOfCycle" << ENDM;
}

void TrdTrySkeltonTask::endOfActivity(const Activity& /*activity*/)
{
  // THIS FUNCTION BODY IS AN EXAMPLE. PLEASE REMOVE EVERYTHING YOU DO NOT NEED.
  ILOG(Debug, Devel) << "endOfActivity" << ENDM;
}

void TrdTrySkeltonTask::reset()
{
  // THIS FUNCTION BODY IS AN EXAMPLE. PLEASE REMOVE EVERYTHING YOU DO NOT NEED.

  // Clean all the monitor objects here.
  ILOG(Debug, Devel) << "Resetting the histograms" << ENDM;
  if (mHistogramA)
    mHistogramA->Reset();
  // if (mHistogramB)
  //   mHistogramB->Reset();
  // if (mHistogramC)
  //   mHistogramC->Reset();
  // if (mHistogramD)
  //   mHistogramD->Reset();
}

} // namespace o2::quality_control_modules::trd
