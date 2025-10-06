// // Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// // See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
// // All rights not expressly granted are reserved.
// //
// // This software is distributed under the terms of the GNU General Public
// // License v3 (GPL Version 3), copied verbatim in the file "COPYING".
// //
// // In applying this license CERN does not waive the privileges and immunities
// // granted to it by virtue of its status as an Intergovernmental Organization
// // or submit itself to any jurisdiction.

// ///
// /// \file   TrdTrySkeltonTask.cxx
// /// \author My Name
// ///

// #include <TCanvas.h>
// #include <TH1.h>

// #include "QualityControl/QcInfoLogger.h"
// #include "TRD/TrdTrySkeltonTask.h"
// #include <Framework/InputRecordWalker.h>
// #include <Framework/DataRefUtils.h>

// namespace o2::quality_control_modules::trd
// {

// TrdTrySkeltonTask::~TrdTrySkeltonTask()
// {
// }

// void TrdTrySkeltonTask::initialize(o2::framework::InitContext& /*ctx*/)
// {
//   // THUS FUNCTION BODY IS AN EXAMPLE. PLEASE REMOVE EVERYTHING YOU DO NOT NEED.

//   // This is how logs are created. QcInfoLogger is used. In production, FairMQ logs will go to InfoLogger as well.
//   ILOG(Debug, Devel) << "initialize TrdTrySkeltonTask" << ENDM;
//   ILOG(Debug, Support) << "A debug targeted for support" << ENDM;
//   ILOG(Info, Ops) << "An Info log targeted for operators" << ENDM;

//   ILOG(Info, Ops) << "Initializing histograms" << ENDM;

//   // This creates and registers a histogram for publication at the end of each cycle, until the end of the task lifetime
//   // mHistogramA = std::make_unique<TH1F>("example", "example", 20, 0, 30000);
//   mHistogramA = std::make_unique<TH1F>("nTracklets", "Number of TRD Tracklets per Event", 100, 0, 1000);
//   mHistogramB = std::make_unique<TH1F>("nTracks", "Number of TRD Tracks per Event", 100, 0, 200);
//   mHistogramC = std::make_unique<TH1F>("adcSum", "ADC Sum of TRD Tracklets", 256, 0, 1024);
//   mHistogramD = std::make_unique<TH1F>("trkletsPerTrack", "Number of Tracklets per TRD Track", 10, 0, 10);

//   getObjectsManager()->startPublishing(mHistogramA.get(), PublicationPolicy::Forever);
//   getObjectsManager()->startPublishing(mHistogramB.get(), PublicationPolicy::Forever);
//   getObjectsManager()->startPublishing(mHistogramC.get(), PublicationPolicy::Forever);
//   getObjectsManager()->startPublishing(mHistogramD.get(), PublicationPolicy::Forever);

//   try {
//     getObjectsManager()->addMetadata(mHistogramA->GetName(), "custom", "34");
//   } catch (...) {

//     ILOG(Warning, Support) << "Metadata could not be added to " << mHistogramA->GetName() << ENDM;
//   }
// }

// void TrdTrySkeltonTask::startOfActivity(const Activity& activity)
// {
//   ILOG(Debug, Devel) << "startOfActivity " << activity.mId << ENDM;

//   // We clean any histograms that could have been filled in previous runs.
//   mHistogramA->Reset();

//   // This creates and registers a histogram for publication at the end of each cycle until and including the end of run.
//   // Typically you may create and register a histogram here if you require Activity information for binning or decoration.
//   // Since ROOT does not respond well to having two histograms with the same name in the memory,
//   // we invoke the reset() to first delete the object that could remained from a previous run.

//   // mHistogramB.reset();
//   // mHistogramB = std::make_unique<TH1F>("example2", "example2", 256, 0, 255);
//   // getObjectsManager()->startPublishing(mHistogramB.get(), PublicationPolicy::ThroughStop);

//   // // Example of retrieving a custom parameter based on the run type + beam type available in Activity
//   // std::string parameter;
//   // // first we try for the current activity. It returns an optional.
//   // if (auto param = mCustomParameters.atOptional("myOwnKey", activity)) {
//   //   parameter = param.value(); // we got a value
//   // } else {
//   //   // we did not get a value. We ask for the "default" runtype and beamtype and we specify a default return value.
//   //   parameter = mCustomParameters.atOrDefaultValue("myOwnKey", "some default");
//   // }
// }

// void TrdTrySkeltonTask::startOfCycle()
// {
//   // THUS FUNCTION BODY IS AN EXAMPLE. PLEASE REMOVE EVERYTHING YOU DO NOT NEED.
//   ILOG(Debug, Devel) << "startOfCycle" << ENDM;
// }

// void TrdTrySkeltonTask::monitorData(o2::framework::ProcessingContext& ctx)
// {
//   // THIS FUNCTION BODY IS AN EXAMPLE. PLEASE REMOVE EVERYTHING YOU DO NOT NEED.

//   // In this function you can access data inputs specified in the JSON config file.
//   // Typically, you should have asked for data inputs as a direct data source or as a data sampling policy.
//   // In both cases you probably used a query such as:
//   //   "query": "data:TST/RAWDATA/0"
//   // which follows the format <binding>:<dataOrigin>/<dataDescription>[/<subSpecification]
//   // Usually inputs should be accessed by their bindings:
//   auto randomData = ctx.inputs().get("data");
//   // If you know what type the data is, you can specify it as such:
//   // auto channels = ctx.inputs().get<gsl::span<o2::fdd::ChannelData>>("channels");
//   // auto digits = ctx.inputs().get<gsl::span<o2::fdd::Digit>>("digits");

//   // In our case we will access the header and the payload as a raw array of bytes
//   const auto* header = o2::framework::DataRefUtils::getHeader<header::DataHeader*>(randomData);
//   auto payloadSize = header->payloadSize;

//   // as an example, let's fill the histogram A with payload sizes and histogram B with the value in the first byte
//   mHistogramA->Fill(payloadSize);

//   if (payloadSize > 0) {
//     auto firstByte = static_cast<uint8_t>(randomData.payload[0]);
//     mHistogramB->Fill(firstByte);
//   }

//   // One can find some examples of using InputRecord at:
//   // https://github.com/AliceO2Group/AliceO2/blob/dev/Framework/Core/README.md#using-inputs---the-inputrecord-api
//   //
//   // One can also iterate over all inputs in a loop:
//   for (const framework::DataRef& input : framework::InputRecordWalker(ctx.inputs())) {
//     // do something with input
//   }
//   // To know how to access CCDB objects, please refer to:
//   // https://github.com/AliceO2Group/QualityControl/blob/master/doc/Advanced.md#accessing-objects-in-ccdb
//   // for GRP objects in particular:
//   // https://github.com/AliceO2Group/QualityControl/blob/master/doc/Advanced.md#access-grp-objects-with-grp-geom-helper
// }

// void TrdTrySkeltonTask::endOfCycle()
// {
//   // THIS FUNCTION BODY IS AN EXAMPLE. PLEASE REMOVE EVERYTHING YOU DO NOT NEED.
//   ILOG(Debug, Devel) << "endOfCycle" << ENDM;
// }

// void TrdTrySkeltonTask::endOfActivity(const Activity& /*activity*/)
// {
//   // THIS FUNCTION BODY IS AN EXAMPLE. PLEASE REMOVE EVERYTHING YOU DO NOT NEED.
//   ILOG(Debug, Devel) << "endOfActivity" << ENDM;
// }

// void TrdTrySkeltonTask::reset()
// {
//   // THIS FUNCTION BODY IS AN EXAMPLE. PLEASE REMOVE EVERYTHING YOU DO NOT NEED.

//   // Clean all the monitor objects here.
//   ILOG(Debug, Devel) << "Resetting the histograms" << ENDM;
//   if (mHistogramA) {
//     mHistogramA->Reset();
//   }
//   if (mHistogramB) {
//     mHistogramB->Reset();
//   }
// }

// } // namespace o2::quality_control_modules::trd

// Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
// All rights not expressly granted are reserved.
//
// This software is distributed under the terms of the GNU General Public
// License v3 (GPL Version 3), copied verbatim in the file "COPYING".
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

///
/// \file   TrdTrySkeltonTask.cxx
/// \author My Name
///

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
  mHistogramB = std::make_unique<TH1F>("nTracks", "Number of TRD Tracks per Event", 100, 0, 200);
  mHistogramC = std::make_unique<TH1F>("adcSum", "ADC Sum of TRD Tracklets", 256, 0, 1024);
  mHistogramD = std::make_unique<TH1F>("trkletsPerTrack", "Number of Tracklets per TRD Track", 10, 0, 10);

  getObjectsManager()->startPublishing(mHistogramA.get(), PublicationPolicy::Forever);
  getObjectsManager()->startPublishing(mHistogramB.get(), PublicationPolicy::Forever);
  getObjectsManager()->startPublishing(mHistogramC.get(), PublicationPolicy::Forever);
  getObjectsManager()->startPublishing(mHistogramD.get(), PublicationPolicy::Forever);

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
  auto tracklets = ctx.inputs().get<gsl::span<o2::trd::Tracklet64>>("tracklets");

  // Get TRD tracks
  auto tracks = ctx.inputs().get<gsl::span<o2::trd::TrackTRD>>("tracks");

  // Count and fill histograms
  int nTracklets = tracklets.size();
  int nTracks = tracks.size();
  mHistogramA->Fill(nTracklets); // Histogram A: number of tracklets
  mHistogramB->Fill(nTracks);

  for (const auto& trk : tracks) {
    int nTrkl = trk.getNtracklets(); // or size of trk.getTracklets()
    mHistogramD->Fill(nTrkl);        // tracklets per track
  }

  // // One can find some examples of using InputRecord at:
  // // https://github.com/AliceO2Group/AliceO2/blob/dev/Framework/Core/README.md#using-inputs---the-inputrecord-api
  // //
  // // One can also iterate over all inputs in a loop:

  // for (const framework::DataRef& input : framework::InputRecordWalker(ctx.inputs())) {
  //   // do something with input
  // }

  // // To know how to access CCDB objects, please refer to:
  // // https://github.com/AliceO2Group/QualityControl/blob/master/doc/Advanced.md#accessing-objects-in-ccdb
  // // for GRP objects in particular:
  // // https://github.com/AliceO2Group/QualityControl/blob/master/doc/Advanced.md#access-grp-objects-with-grp-geom-helper
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
  if (mHistogramB)
    mHistogramB->Reset();
  if (mHistogramC)
    mHistogramC->Reset();
  if (mHistogramD)
    mHistogramD->Reset();
}

} // namespace o2::quality_control_modules::trdtryskelton
