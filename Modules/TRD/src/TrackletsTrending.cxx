#include "TRD/TrackletsTrending.h"

#include "QualityControl/QcInfoLogger.h"
#include "QualityControl/DatabaseInterface.h"
#include "QualityControl/ReductorTObject.h"
#include "QualityControl/RootClassFactory.h"
#include "QualityControl/ObjectMetadataKeys.h"

#include <TDatime.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TTree.h>

using namespace o2::quality_control;
using namespace o2::quality_control::repository;
using namespace o2::quality_control::postprocessing;

void TrackletsTrending::configure(const boost::property_tree::ptree& config)
{
  mConfig = TrendingTaskConfigTRD(getID(), config);
}

void TrackletsTrending::initialize(Trigger, framework::ServiceRegistryRef)
{
  mTrend = std::make_unique<TTree>(getName().c_str(), getName().c_str());

  mTrend->Branch("runNumber", &mMetaData.runNumber);
  mTrend->Branch("ntreeentries", &ntreeentries);
  mTrend->Branch("time", &mTime);

  for (const auto& source : mConfig.dataSources) {
    auto reductor = std::unique_ptr<Reductor>(
      core::root_class_factory::create<Reductor>(
        source.moduleName,
        source.reductorName
      )
    );

    mTrend->Branch(
      source.name.c_str(),
      reductor->getBranchAddress(),
      reductor->getBranchLeafList()
    );

    mReductors.emplace(source.name, std::move(reductor));
  }
}

void TrackletsTrending::update(Trigger t, framework::ServiceRegistryRef services)
{
  auto& qcdb = services.get<DatabaseInterface>();
  trendValues(t, qcdb);
  generatePlots(qcdb);
}

void TrackletsTrending::finalize(Trigger, framework::ServiceRegistryRef services)
{
  auto& qcdb = services.get<DatabaseInterface>();

  auto mo = std::make_shared<core::MonitorObject>(
    mTrend.get(),
    getName(),
    "o2::quality_control::postprocessing::TrackletsTrending",
    mConfig.detectorName
  );
  mo->setIsOwner(false);
  qcdb.storeMO(mo);

  generatePlots(qcdb);
}

void TrackletsTrending::trendValues(const Trigger& t, DatabaseInterface& qcdb)
{
  mTime = t.timestamp;
  mMetaData.runNumber = t.activity.mId;

  bool firstSource = true;

  for (auto& dataSource : mConfig.dataSources) {
    if (dataSource.type != "repository") {
      ILOG(Error, Support) << "Unsupported data source type: "
                           << dataSource.type << ENDM;
      continue;
    }

    auto mo = qcdb.retrieveMO(dataSource.path, dataSource.name, t.timestamp);
    if (!mo) {
      ILOG(Warning, Devel)
        << "Could not retrieve MO " << dataSource.name << ENDM;
      continue;
    }

    if (firstSource) {
      auto meta = mo->getMetadataMap();
      auto it = meta.find(metadata_keys::runNumber);
      if (it != meta.end()) {
        mMetaData.runNumber = std::stoll(it->second);
      }
      ntreeentries = mTrend->GetEntries() + 1;
      mRunList.push_back(std::to_string(mMetaData.runNumber));
      firstSource = false;
    }

    auto* reductor =
      dynamic_cast<ReductorTObject*>(mReductors[dataSource.name].get());

    if (reductor) {
      reductor->update(mo->getObject());
    }
  }

  mTrend->Fill();
}

void TrackletsTrending::generatePlots(DatabaseInterface& qcdb)
{
  if (!mTrend || mTrend->GetEntries() == 0) {
    return;
  }

  for (const auto& plot : mConfig.plots) {

    auto& canvas = mCanvases[plot.name];
    if (!canvas) {
      canvas = std::make_unique<TCanvas>(
        plot.name.c_str(),
        plot.title.c_str(),
        800, 600
      );
    }

    canvas->Clear();
    canvas->cd();

    mTrend->Draw(
      plot.varexp.c_str(),
      plot.selection.c_str(),
      plot.option.c_str()
    );

    auto* histo =
      dynamic_cast<TH1*>(canvas->GetPrimitive("htemp"));

    if (!histo) {
      ILOG(Warning, Devel)
        << "No histogram for plot " << plot.name << ENDM;
      continue;
    }

    histo->SetTitle(plot.title.c_str());

    if (plot.varexp.find(":time") != std::string::npos) {
      histo->GetXaxis()->SetTimeDisplay(1);
      histo->GetXaxis()->SetTimeFormat("%Y-%m-%d %H:%M");
      histo->GetXaxis()->SetNdivisions(505);
    }

    if (plot.varexp.find(":runNumber") != std::string::npos) {
      for (size_t i = 0; i < mRunList.size(); ++i) {
        histo->GetXaxis()->SetBinLabel(i + 1, mRunList[i].c_str());
      }
    }

    canvas->Modified();
    canvas->Update();

    auto mo = std::make_shared<core::MonitorObject>(
      canvas.get(),
      plot.name,
      "o2::quality_control::postprocessing::TrackletsTrending",
      mConfig.detectorName
    );
    mo->setIsOwner(false);
    qcdb.storeMO(mo);
  }
}



/* void TrackletsTrending::generatePlots(repository::DatabaseInterface& qcdb)
{
    if (mTrend->GetEntries() < 1) return;

    std::vector<std::unique_ptr<TCanvas>> mCanvasTRD;
    int mPlot = 0;

    for (const auto& plot : mConfig.plots) {
        mCanvasTRD.push_back(std::make_unique<TCanvas>(plot.name.c_str(), plot.title.c_str()));
        mCanvasTRD[mPlot]->cd();
        mTrend->Draw(plot.varexp.c_str(), plot.selection.c_str(), plot.option.c_str());

        TH1* histo = dynamic_cast<TH1*>(mCanvasTRD[mPlot]->GetPrimitive("htemp"));
        if (histo) {
            histo->SetTitle(plot.title.c_str());

            if (plot.varexp.find(":time") != std::string::npos) {
                histo->GetXaxis()->SetTimeDisplay(1);
                histo->GetXaxis()->SetNdivisions(505);
                histo->GetXaxis()->SetTimeOffset(0.0);
                histo->GetXaxis()->SetTimeFormat("%Y-%m-%d %H:%M");
            } else if (plot.varexp.find(":runNumber") != std::string::npos) {
                histo->GetXaxis()->SetNdivisions(505);
                for (int ir = 0; ir < (int)runlist.size(); ir++)
                    histo->GetXaxis()->SetBinLabel(ir + 1, runlist[ir].c_str());
            }

            histo->BufferEmpty();
            mCanvasTRD[mPlot]->Update();
        }

        mPlots[plot.name] = mCanvasTRD[mPlot].get();
        // auto mo_trd = std::make_shared<MonitorObject>(mCanvasTRD[mPlot].get(), mConfig.taskName, "o2::quality_control_modules::trd::TrackletsTrending", mConfig.detectorName);
        auto mo_trd = std::make_shared<o2::quality_control::core::MonitorObject>(
            mCanvasTRD[mPlot].get(),
            mConfig.taskName,
            "o2::quality_control_modules::trd::TrackletsTrending",
            mConfig.detectorName
        );

        mo_trd->setIsOwner(false);
        qcdb.storeMO(mo_trd);

        ++mPlot;
    }
} */
