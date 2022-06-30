/**
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */
#ifndef SRSRAN_RRC_MEAS_H_
#define SRSRAN_RRC_MEAS_H_

#include "srsran/asn1/rrc.h"
#include "srsran/asn1/rrc_utils.h"
#include "srsran/common/common.h"
#include "srsran/interfaces/ue_interfaces.h"
#include "srsue/hdr/stack/rrc/rrc.h"

#include <map>
#include <set>

namespace srsue {

using namespace asn1::rrc;

typedef std::vector<phy_cell_t> cell_triggered_t;

// TODO make this agnostic with srsenb
meas_obj_to_add_mod_s* find_meas_obj_map(std::map<uint32_t, meas_obj_to_add_mod_s>& l, uint32_t earfcn);

// RRC Measurements class
class rrc::rrc_meas
{
public:
  rrc_meas() : meas_cfg(&meas_report_list), meas_report_list(&meas_cfg), logger(srslog::fetch_basic_logger("RRC")) {}
  void init(rrc* rrc_ptr);
  void reset();
  bool parse_meas_config(const rrc_conn_recfg_r8_ies_s* meas_config, bool is_ho_reest = false, uint32_t src_earfcn = 0);
  void ho_reest_actions(const uint32_t src_earfcn, const uint32_t dst_earfcn);
  void run_tti();
  void update_phy();
  float rsrp_filter(const float new_value, const float avg_value);
  float rsrq_filter(const float new_value, const float avg_value);

private:
  typedef struct {
    float rsrp;
    float rsrq;
  } phy_quant_t;

  typedef enum { eutra, inter_rat } report_type_t;

  class var_meas_cfg;

  class var_meas_report_list
  {
  public:
    var_meas_report_list(var_meas_cfg* meas_cfg_) : meas_cfg(meas_cfg_), logger(srslog::fetch_basic_logger("RRC")) {}
    void init(rrc* rrc);
    void generate_report(const uint32_t measId);
    void remove_all_varmeas_reports();
    void remove_varmeas_report(const uint32_t measId);
    bool is_timer_expired(const uint32_t measId);
    void set_measId(const uint32_t            measId,
                    const uint32_t            carrier_freq,
                    const report_cfg_eutra_s& report_cfg,
                    const cell_triggered_t&   cell_triggered_list);

    void set_measId(const uint32_t                measId,
                    const uint32_t                carrier_freq,
                    const report_cfg_inter_rat_s& report_cfg,
                    const cell_triggered_t&       cell_triggered_list);

    void             upd_measId(const uint32_t measId, const cell_triggered_t& cell_triggered_list);
    cell_triggered_t get_measId_cells(const uint32_t measId);

  private:
    void generate_report_eutra(meas_results_s* report, const uint32_t measId);
    void generate_report_interrat(meas_results_s* report, const uint32_t measId);
    class var_meas_report
    {
    public:
      report_type_t                       report_type         = eutra;
      uint32_t                            carrier_freq        = 0;
      uint8_t                             nof_reports_sent    = 0;
      cell_triggered_t                    cell_triggered_list = {};
      report_cfg_eutra_s                  report_cfg_eutra    = {};
      report_cfg_inter_rat_s              report_cfg_inter    = {};
      srsran::timer_handler::unique_timer periodic_timer      = {};
    };
    var_meas_cfg*                       meas_cfg = nullptr;
    srslog::basic_logger&               logger;
    rrc*                                rrc_ptr = nullptr;
    std::map<uint32_t, var_meas_report> varMeasReportList;
  };

  // The UE variable VarMeasConfig includes the accumulated configuration of the measurements to be performed by the
  // UE, covering intra-frequency, inter-frequency and inter-RAT mobility related measurements.
  class var_meas_cfg
  {
  public:
    var_meas_cfg(var_meas_report_list* meas_report_) :
      meas_report(meas_report_), logger(srslog::fetch_basic_logger("RRC"))
    {}
    void                             init(rrc* rrc);
    void                             reset();
    phy_quant_t                      get_filter_a();
    void                             remove_measId(const uint32_t measId);
    std::list<meas_obj_to_add_mod_s> get_active_objects();
    void                             ho_reest_finish(const uint32_t src_earfcn, const uint32_t dst_earfcn);
    bool parse_meas_config(const meas_cfg_s* meas_config, bool is_ho_reest, uint32_t src_earfcn);
    void eval_triggers();
    void report_triggers();

  private:
    void remove_varmeas_report(const uint32_t meas_id);

    void measObject_removal(const meas_obj_to_rem_list_l& list);
    void measObject_addmod(const meas_obj_to_add_mod_list_l& list);
    void reportConfig_removal(const report_cfg_to_rem_list_l& list);
    void reportConfig_addmod(const report_cfg_to_add_mod_list_l& list);
    void measId_removal(const meas_id_to_rem_list_l& list);
    void measId_addmod(const meas_id_to_add_mod_list_l& list);
    void quantity_config(const quant_cfg_s& cfg);
    void log_debug_trigger_value_eutra(const eutra_event_s::event_id_c_& e);
    void log_debug_trigger_value_interrat(const report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_& e);
    static bool is_rsrp(report_cfg_eutra_s::trigger_quant_opts::options q);

    // Helpers
    void measObject_addmod_eutra(const meas_obj_to_add_mod_s& l);
    void measObject_addmod_nr_r15(const meas_obj_to_add_mod_s& l);

    void reportConfig_addmod_eutra(const report_cfg_to_add_mod_s& l);
    void reportConfig_addmod_interrat(const report_cfg_to_add_mod_s& l);
    bool reportConfig_addmod_to_reportConfigList(const report_cfg_to_add_mod_s& l);

    void eval_triggers_eutra(uint32_t            meas_id,
                             report_cfg_eutra_s& report_cfg,
                             meas_obj_eutra_s&   meas_obj,
                             meas_cell_eutra*    serv_cell,
                             float               Ofs,
                             float               Ocs);
    void report_triggers_eutra(uint32_t meas_id, report_cfg_eutra_s& report_cfg, meas_obj_eutra_s& meas_obj);
    void report_triggers_eutra_check_new(int32_t meas_id, report_cfg_eutra_s& report_cfg, meas_obj_eutra_s& meas_obj);
    void report_triggers_eutra_check_leaving(int32_t meas_id, report_cfg_eutra_s& report_cfg);
    void report_triggers_eutra_removing_trigger(int32_t meas_id);
    void eval_triggers_interrat_nr(uint32_t meas_id, report_cfg_inter_rat_s& report_cfg, meas_obj_nr_r15_s& meas_obj);
    void report_triggers_interrat_nr(uint32_t meas_id, report_cfg_inter_rat_s& report_cfg, meas_obj_nr_r15_s& meas_obj);
    void report_triggers_interrat_check_new(int32_t                 meas_id,
                                            report_cfg_inter_rat_s& report_cfg,
                                            meas_obj_nr_r15_s&      meas_obj);
    void report_triggers_interrat_check_leaving(int32_t meas_id, report_cfg_inter_rat_s& report_cfg);
    void report_triggers_interrat_removing_trigger(int32_t meas_id);

    class cell_trigger_state
    {
    public:
      void event_condition(const bool enter, const bool exit);
      bool is_enter_equal(const uint32_t nof_tti);
      bool is_exit_equal(const uint32_t nof_tti);

    private:
      uint32_t nof_tti_enter = 0;
      uint32_t nof_tti_exit  = 0;
    };

    // varMeasConfig data
    std::map<uint32_t, meas_id_to_add_mod_s>    measIdList;       // Uses MeasId as key
    std::map<uint32_t, meas_obj_to_add_mod_s>   measObjectsList;  // Uses MeasObjectId as key
    std::map<uint32_t, report_cfg_to_add_mod_s> reportConfigList; // Uses ReportConfigId as key

    phy_quant_t filter_a = {1.0, 1.0}; // disable filtering until quantityConfig is received (see Sec. 5.5.3.2 Note 2)
    float       s_measure_value = 0.0;

    // trigger counters. First key is measId, second key is cell id (pci)
    // It is safe to use [] operator in this double-map because all members are uint32_t
    std::map<uint32_t, std::map<uint32_t, cell_trigger_state> > trigger_state;

    std::map<uint32_t, std::map<uint32_t, cell_trigger_state> > trigger_state_nr;

    var_meas_report_list* meas_report = nullptr;
    srslog::basic_logger& logger;
    rrc*                  rrc_ptr = nullptr;
  };

  std::mutex            meas_cfg_mutex;
  var_meas_cfg          meas_cfg;
  var_meas_report_list  meas_report_list;
  srslog::basic_logger& logger;
  rrc*                  rrc_ptr = nullptr;

  // Static functions
  static uint8_t value_to_range_nr(const asn1::rrc::thres_nr_r15_c::types_opts::options type, const float value);
  static float   range_to_value_nr(const asn1::rrc::thres_nr_r15_c::types_opts::options type, const uint8_t range);
  static uint8_t offset_val(const meas_obj_eutra_s& meas_obj);
  static asn1::dyn_array<cells_to_add_mod_s>::iterator find_pci_in_meas_obj(meas_obj_eutra_s& meas_obj, uint32_t pci);
};

} // namespace srsue

#endif // SRSRAN_SRSUE_HDR_STACK_RRC_RRC_MEAS_H_
