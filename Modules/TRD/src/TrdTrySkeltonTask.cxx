#include <TCanvas.h>
#include <TH1.h>

#include "QualityControl/QcInfoLogger.h"
#include "TRD/TrdTrySkeltonTask.h"
#include <Framework/InputRecordWalker.h>
#include <Framework/DataRefUtils.h>

#include "DataFormatsTRD/Tracklet64.h"
#include "QualityControl/MonitorObject.h"
// #include "DataFormatsTRD/TrackTRD.h"

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

  ILOG(Info, Ops) << "Initializing TRD skelton histograms" << ENDM;

  // This creates and registers a histogram for publication at the end of each cycle, until the end of the task lifetime
  histTrackletsTF = std::make_unique<TH1F>("nTrackletsTF", "Number of TRD Tracklets per Timeframe", 2000, 0, 200000);
  // histTrackletsEvent = std::make_unique<TH1F>("nTrackletsEVENT", "Number of TRD Tracklets per Event", 5000, 0, 5000);

  histQ0 = std::make_unique<TH1F>("Q0", "per TRD Tracklet Q0", 256, -0.5, 255.5);
  histQ1 = std::make_unique<TH1F>("Q1", "per TRD Tracklet Q1", 256, -0.5, 255.5);
  histQ2 = std::make_unique<TH1F>("Q2", "per TRD Tracklet Q2", 256, -0.5, 255.5);

  histChamber = std::make_unique<TH1F>("Chamber", "Tracklets per TRD Chamber", 540, 0, 540);

  histPadRow = std::make_unique<TH1F>("PadRow", "Tracklets per PadRow", 16, 0, 16);

  histMCM = std::make_unique<TH1F>("MCM", "Tracklets per MCM (ignore 0)", 160, 0, 160);

  getObjectsManager()->startPublishing(histTrackletsTF.get(), PublicationPolicy::Forever);
  // getObjectsManager()->startPublishing(histTrackletsEvent.get(), PublicationPolicy::Forever);
  getObjectsManager()->startPublishing(histQ0.get(), PublicationPolicy::Forever);
  getObjectsManager()->startPublishing(histQ1.get(), PublicationPolicy::Forever);
  getObjectsManager()->startPublishing(histQ2.get(), PublicationPolicy::Forever);
  getObjectsManager()->startPublishing(histChamber.get(), PublicationPolicy::Forever);
  getObjectsManager()->startPublishing(histPadRow.get(), PublicationPolicy::Forever);
  getObjectsManager()->startPublishing(histMCM.get(), PublicationPolicy::Forever);

  try {
    getObjectsManager()->addMetadata(histTrackletsTF->GetName(), "custom", "34");
  } catch (...) {
    ILOG(Warning, Support) << "Metadata could not be added to " << histTrackletsTF->GetName() << ENDM;
  }
}

void TrdTrySkeltonTask::startOfActivity(const Activity& activity)
{
  ILOG(Debug, Devel) << "startOfActivity " << activity.mId << ENDM;

  // We clean anyhists that could have been filled in previous runs.
  histTrackletsTF->Reset();
  // histTrackletsEvent->Reset();
  histQ0->Reset();
  histQ1->Reset();
  histQ2->Reset();
  histChamber->Reset();
  histPadRow->Reset();
  histMCM->Reset();
}

void TrdTrySkeltonTask::startOfCycle()
{
  // THUS FUNCTION BODY IS AN EXAMPLE. PLEASE REMOVE EVERYTHING YOU DO NOT NEED.
  ILOG(Debug, Devel) << "startOfCycle" << ENDM;
}

void TrdTrySkeltonTask::monitorData(o2::framework::ProcessingContext& ctx)
{
  // Get TRD tracklets
  auto tracklets = ctx.inputs().get<gsl::span<o2::trd::Tracklet64>>("tracklets");
  // auto trigRec = ctx.inputs().get<gsl::span<o2::trd::Tracklet64>>("trackletsTR");

  // // Count and fillhists
  // int nTracklets = tracklets.size();
  // histTracklet->Fill(nTracklets); // hist A: number of tracklets

  // 1. Tracklets per timeframe (simple count)
  int nTF = tracklets.size();
  histTrackletsTF->Fill(nTF);

  // // 2. Tracklets per event
  // for (auto& tr : trigRec) {
  //   int start = tr.getFirstEntry();
  //   int n = tr.getNumberOfObjects();
  //   histTrackletsEvent->Fill(n);
  // }

  // Loop over tracklets
  for (const auto& trk : tracklets) {
    // Filling Q values
    histQ0->Fill(trk.getQ0());
    histQ1->Fill(trk.getQ1());
    histQ2->Fill(trk.getQ2());
    // ChamberIDs (0-539)
    histChamber->Fill(trk.getDetector());
    // PadRow (0-15)
    histPadRow->Fill(trk.getPadRow());
    // MCM (Zero ignored)
    int mcm = trk.getMCM();
    if (mcm > 0) {
      histMCM->Fill(mcm);
    }
  }
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

  // ✔ 200,000 bins for timeframe is correct ?
  // Typical TF has ~50 events × 2k tracklets/event = 100k tracklets.

  // ✔ Per-event histogram should go to around 5000
  // Highest tracklet multiplicity per event ≈ 2500–3000.

  // Clean all the monitor objects here.
  ILOG(Debug, Devel)
    << "Resetting thehists" << ENDM;
  // if (histTracklet)
  //   histTracklet->Reset();
  if (histTrackletsTF)
    histTrackletsTF->Reset();
  // if (histTrackletsEvent)
  //   histTrackletsEvent->Reset();
  if (histQ0)
    histQ0->Reset();
  if (histQ1)
    histQ1->Reset();
  if (histQ2)
    histQ2->Reset();
  if (histChamber)
    histChamber->Reset();
  if (histPadRow)
    histPadRow->Reset();
  if (histMCM)
    histMCM->Reset();
}

} // namespace o2::quality_control_modules::trd
