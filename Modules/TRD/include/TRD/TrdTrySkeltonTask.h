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
/// \file   TrdTrySkeltonTask.h
/// \author My Name
///

#ifndef QC_MODULE_TRD_TRDTRYSKELTONTASK_H
#define QC_MODULE_TRD_TRDTRYSKELTONTASK_H

#include "QualityControl/TaskInterface.h"
#include <memory>

class TH1F;

using namespace o2::quality_control::core;

namespace o2::quality_control_modules::trd
{

/// \brief Example Quality Control Task
/// \author My Name
class TrdTrySkeltonTask final : public TaskInterface
{
 public:
  /// \brief Constructor
  TrdTrySkeltonTask() = default;
  /// Destructor
  ~TrdTrySkeltonTask() override;

  // Definition of the methods for the template method pattern
  void initialize(o2::framework::InitContext& ctx) override;
  void startOfActivity(const Activity& activity) override;
  void startOfCycle() override;
  void monitorData(o2::framework::ProcessingContext& ctx) override;
  void endOfCycle() override;
  void endOfActivity(const Activity& activity) override;
  void reset() override;

 private:
  std::unique_ptr<TH1F> histTracklet;  // number of tracklets per event
  std::unique_ptr<TH1F> histQ0;        // Q0 per tracklet
  std::unique_ptr<TH1F> histQ1;        // Q1 per tracklet
  std::unique_ptr<TH1F> histQ2;        // Q2 per tracklet
  std::unique_ptr<TH1F> histChamber;   // tracklets per chamber
  std::unique_ptr<TH1F> histPadRow;    // tracklets per padrow
  std::unique_ptr<TH1F> histMCM;       // tracklets per MCM (non-zero)
};

} // namespace o2::quality_control_modules::trd

#endif // QC_MODULE_TRD_TRDTRYSKELTONTASK_H
