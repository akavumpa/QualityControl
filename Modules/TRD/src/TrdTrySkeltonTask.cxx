#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>

#include "QualityControl/QcInfoLogger.h"
#include "TRD/TrdTrySkeltonTask.h"
#include <Framework/InputRecordWalker.h>
#include <Framework/DataRefUtils.h>

#include "DataFormatsTRD/Tracklet64.h"
#include "QualityControl/MonitorObject.h"
#include "DataFormatsTRD/TriggerRecord.h"

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

  // histTrackletsTF = std::make_unique<TH1F>("nTrackletsTF", "Number of TRD Tracklets per Timeframe", 2000, 0, 200000); // pp
  histTrackletsTF = std::make_unique<TH1F>("nTrackletsTF", "Number of TRD Tracklets per Timeframe", 20000, 0, 2000000); // Pb-Pb

  histTrackletsEvent = std::make_unique<TH1F>("nTrackletsEVENT", "Number of TRD Tracklets per Event", 5000, 0, 5000);

  histQ0 = std::make_unique<TH1F>("Q0", "Q0 per TRD Tracklet", 256, -0.5, 255.5);
  histQ1 = std::make_unique<TH1F>("Q1", "Q1 per TRD Tracklet", 256, -0.5, 255.5);
  histQ2 = std::make_unique<TH1F>("Q2", "Q2 per TRD Tracklet", 256, -0.5, 255.5);

  histChamber = std::make_unique<TH1F>("Chamber", "Tracklets per TRD Chamber", 540, 0, 540);
  histPadRow = std::make_unique<TH1F>("PadRow", "Tracklets per PadRow", 16, 0, 16);
  histPadRowVsDet = std::make_unique<TH2F>("PadRowVsDet", "PadRow vs Detector;Detector ID;PadRow", 540, 0, 540, 16, 0, 16); // 540 chambers and 16 padrows

  // 540 chambers × 16 MCMs each = 8640 total MCMs
  histMCM = std::make_unique<TH1F>("MCM",
                                   "Tracklets per global MCM;Global MCM ID;Entries",
                                   8640, 0, 8640);

  histMCMOccupancy = std::make_unique<TH1F>("MCMTrackletPerMCM",
                                            "Number of tracklets per MCM;Tracklets per MCM;Count of MCMs",
                                            16, -0.5, 15.5);

  getObjectsManager()->startPublishing(histPadRowVsDet.get(), PublicationPolicy::Forever);
  getObjectsManager()->startPublishing(histMCMOccupancy.get(), PublicationPolicy::Forever);

  getObjectsManager()->startPublishing(histTrackletsTF.get(), PublicationPolicy::Forever);
  getObjectsManager()->startPublishing(histTrackletsEvent.get(), PublicationPolicy::Forever);
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
  histTrackletsEvent->Reset();
  histQ0->Reset();
  histQ1->Reset();
  histQ2->Reset();
  histChamber->Reset();
  histPadRow->Reset();
  histMCM->Reset();
  histPadRowVsDet->Reset();
  histMCMOccupancy->Reset();
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
  auto trigRec = ctx.inputs().get<gsl::span<o2::trd::TriggerRecord>>("triggers");

  // 1. Tracklets per timeframe (simple count)
  int nTF = tracklets.size();
  histTrackletsTF->Fill(nTF);

  // 2. Tracklets per event
  for (auto& tr : trigRec) {
    int start = tr.getFirstTracklet();
    int n = tr.getNumberOfTracklets();
    histTrackletsEvent->Fill(n);
  }

  // One entry per MCM (global MCM ID 0–8639)
  static constexpr int kNMCMTot = 540 * 16;
  std::array<int, kNMCMTot> mcmCounts{};
  mcmCounts.fill(0);

  // Loop over tracklets
  for (const auto& trk : tracklets) {
    // Filling Q values
    histQ0->Fill(trk.getQ0());
    histQ1->Fill(trk.getQ1());
    histQ2->Fill(trk.getQ2());

    histChamber->Fill(trk.getDetector());                      // ChamberIDs (0-539)
    histPadRow->Fill(trk.getPadRow());                         // PadRow (0-15)
    histPadRowVsDet->Fill(trk.getDetector(), trk.getPadRow()); // Fill PadRow vs Detector

    int det = trk.getDetector(); // 0–539
    int locMCM = trk.getMCM();   // 0–15

    // Safety check
    if (det < 0 || det >= 540) {
      LOG(warn) << "Invalid detector ID: " << det;
      continue;
    }
    if (locMCM < 0 || locMCM >= 16) {
      LOG(warn) << "Invalid local MCM ID: " << locMCM;
      continue;
    }

    // Global MCM index
    int globalMCM = det * 16 + locMCM; // 0–8639
    // Fill histograms
    histMCM->Fill(globalMCM);
    mcmCounts[globalMCM]++;

    // LOG(info) << "Tracklet: detector=" << trk.getDetector() << " Padrow" << trk.getPadRow();
    // // << "  chamber=" << trk.getChamber()
    // // << "  stack=" << trk.getStack()
    // // << "  layer=" << trk.getLayer()
    // // << "  mcm=" << trk.getMCM();
  }

  // Fill histogram for "how many MCMs have N tracklets"
  int maxOcc = 0;
  for (int mcm = 0; mcm < kNMCMTot; mcm++) {
    if (mcmCounts[mcm] > 0) {
      histMCMOccupancy->Fill(mcmCounts[mcm]);
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
  if (histTrackletsEvent)
    histTrackletsEvent->Reset();
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
  if (histPadRowVsDet)
    histPadRowVsDet->Reset();
  if (histMCMOccupancy)
    histMCMOccupancy->Reset();
}

} // namespace o2::quality_control_modules::trd
