#ifndef QUALITYCONTROL_TRACKLETSTFPOSTPROCESSING_H
#define QUALITYCONTROL_TRACKLETSTFPOSTPROCESSING_H

#include "QualityControl/PostProcessingInterface.h"

namespace o2::quality_control::repository
{
class DatabaseInterface;
}

namespace o2::quality_control::postprocessing
{

class TrackletsTFPostProcessing final : public PostProcessingInterface
{
 public:
  TrackletsTFPostProcessing() = default;
  ~TrackletsTFPostProcessing() override = default;

  void configure(const boost::property_tree::ptree& config) override;
  void initialize(Trigger, framework::ServiceRegistryRef) override;
  void update(Trigger, framework::ServiceRegistryRef) override;
  void finalize(Trigger, framework::ServiceRegistryRef) override;

 private:
  void retrieveObjects(const Trigger& t,
                       repository::DatabaseInterface& qcdb);
};

} // namespace o2::quality_control::postprocessing

#endif