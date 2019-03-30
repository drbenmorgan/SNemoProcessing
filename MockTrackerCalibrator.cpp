#include "bayeux/dpp/base_module.h"
#include "bayeux/datatools/service_manager.h"
#include "bayeux/geomtools/geometry_service.h"
#include "bayeux/geomtools/manager.h"
#include "bayeux/mctools/simulated_data.h"
#include "falaise/property_reader.h"
#include "falaise/snemo/processing/services.h"
#include "falaise/snemo/datamodels/data_model.h"
#include "falaise/snemo/datamodels/mock_raw_tracker_hit.h"
#include "falaise/snemo/datamodels/calibrated_data.h"


#include <iostream>

namespace  {
using namespace falaise::properties;
struct CalibratorConfig {
  CalibratorConfig() = default;
  explicit CalibratorConfig(const datatools::properties& p) :
    SD_label(getValueOrDefault(p, "SD_label", snemo::datamodel::data_info::default_simulated_data_label())),
    CD_label(getValueOrDefault(p, "CD_label", snemo::datamodel::data_info::default_calibrated_data_label())),
    Geo_label(getValueOrDefault(p, "Geo_label", snemo::processing::service_info::default_geometry_service_label())),
    hit_category(getValueOrDefault(p, "hit_category", std::string("gg"))),
    random_id(getValueOrDefault(p, "random.id", std::string("mt19937"))),
    random_seed(getValueOrDefault(p, "random.seed", 12345)) {
  }

  std::string SD_label; // inbox
  std::string CD_label; // outbox
  std::string Geo_label; //name of geo service
  std::string hit_category; // ;
  std::string random_id;
  int random_seed;
};

} // namespace


class MockTrackerCalibrator : public dpp::base_module {
 public:
  using CalibratedData = snemo::datamodel::calibrated_data;
  using RawTrackerHit = snemo::datamodel::mock_raw_tracker_hit;
  using RawTrackerHitCollection = std::list<RawTrackerHit>;

 public:
  MockTrackerCalibrator() = default;
  ~MockTrackerCalibrator() {
    this->reset();
  }

  void initialize(const datatools::properties& config,
                  datatools::service_manager& services,
                  dpp::module_handle_dict_type&) override {
    config_ = CalibratorConfig(config);
    geoManager_ = &(services.get<geomtools::geometry_service>(config_.Geo_label).get_geom_manager());
    this->_set_initialized(true);
  }

  dpp::base_module::process_status process(datatools::things& event) override {
    const auto& simData = event.get<mctools::simulated_data>(config_.SD_label);
    RawTrackerHitCollection rawHits = this->mockupRawTrackerHits(simData);
    // oddity of datatools::things operation (putting rather than creating data)
    //  NB: could also use module name (this->get_name() as output label).
    event.add<CalibratedData>(config_.CD_label) = std::move(this->makeCalibration(rawHits));
    return PROCESS_OK;
  }

  void reset() override {
    this->_set_initialized(false);
  }

 private:
  //! Create mocked raw tracker hits from MC output
  RawTrackerHitCollection mockupRawTrackerHits(mctools::simulated_data const& simHits) {
    using MCGeigerHitCollection = mctools::simulated_data::hit_handle_collection_type;
    // No tracker info, nothing to do...
    if (!simHits.has_step_hits(config_.hit_category)) {
      return RawTrackerHitCollection {};
    }

    MCGeigerHitCollection simGeigerHits = simHits.get_step_hits_dict().at(config_.hit_category);
    RawTrackerHitCollection output;

    // For each step hit...
    for (const auto& hit : simGeigerHits) {
      // 1. Find drift distance...
      //    - continue if < "anode_efficiency"
      // 2. Do Anode TDC
      // 3. Do Cathode TDCs
      //
      // Insert into output collection...
      // 1. ... directly if GID not used before
      // 2. ... else, update existing hit if new drift time less than existing
      // one
      //
    }

    return output;
  }

  CalibratedData makeCalibration(RawTrackerHitCollection const& input) {
    CalibratedData x;
    x.grab_properties().store("foo", "bar");
    return x;
  }


 private:
  const geomtools::manager* geoManager_ = nullptr;
  CalibratorConfig config_;

  DPP_MODULE_REGISTRATION_INTERFACE(MockTrackerCalibrator);
};

DPP_MODULE_REGISTRATION_IMPLEMENT(MockTrackerCalibrator, "MockTrackerCalibrator");

