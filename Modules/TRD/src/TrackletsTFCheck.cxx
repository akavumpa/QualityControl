#include "TRD/TrackletsTFCheck.h"

#include "QualityControl/Quality.h"
#include "QualityControl/MonitorObject.h"

#include <TH1.h>

using namespace o2::quality_control;

namespace o2::quality_control_modules::trd
{

void TrackletsTFCheck::configure()
{
  // Nothing for now
  // Thresholds can later be read from JSON via checkParameters
}

Quality TrackletsTFCheck::check(
  std::map<std::string, std::shared_ptr<MonitorObject>>* moMap)
{
  auto it = moMap->find("nTrackletsTF");
  if (it == moMap->end()) {
    return Quality::Null;
  }

  auto* h = dynamic_cast<TH1*>(it->second->getObject());
  if (!h) {
    return Quality::Null;
  }

  double mean = h->GetMean();

  if (mean < 1e4) {
    return Quality::Bad;
  } else if (mean < 5e4) {
    return Quality::Medium;
  }

  return Quality::Good;
}

void TrackletsTFCheck::beautify(std::shared_ptr<MonitorObject> mo,
                                Quality checkResult)
{
  auto* h = dynamic_cast<TH1*>(mo->getObject());
  if (!h) {
    return;
  }

  if (!mMessage) {
    mMessage = std::make_shared<TPaveText>(0.15, 0.75, 0.85, 0.88, "NDC");
    mMessage->SetFillColor(0);
    mMessage->SetBorderSize(0);
  }

  mMessage->Clear();

  if (checkResult == Quality::Good) {
    mMessage->AddText("Tracklets/TF: OK");
  } else if (checkResult == Quality::Medium) {
    mMessage->AddText("Tracklets/TF: WARNING");
  } else if (checkResult == Quality::Bad) {
    mMessage->AddText("Tracklets/TF: BAD");
  }

  mo->addOrReplaceDecoration(mMessage);
}

void TrackletsTFCheck::reset()
{
  mMessage.reset();
}

} // namespace o2::quality_control_modules::trd
