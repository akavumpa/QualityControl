#include "QualityControl/CheckInterface.h"
#include "QualityControl/Quality.h"
#include <TH1.h>

using namespace o2::quality_control;

namespace o2::quality_control_modules::trd
{

class TrackletsTFCheck final : public CheckInterface
{
 public:
  Quality check(std::map<std::string, std::shared_ptr<MonitorObject>>* moMap) override
  {
    auto it = moMap->find("nTrackletsTF");
    if (it == moMap->end()) {
      return Quality::Null;
    }

    auto* h = dynamic_cast<TH1*>(it->second->getObject());
    if (!h) {
      return Quality::Null;
    }

    // Mean number of tracklets per TF
    double mean = h->GetMean();

    // --- simple sanity thresholds ---
    if (mean < 1e4) {
      return Quality::Bad;
    } else if (mean < 5e4) {
      return Quality::Medium;
    }

    return Quality::Good;
  }

  std::string getName() override { return "TrackletsTFCheck"; }
};

} // namespace o2::quality_control_modules::trd
