/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#include "phy_controller.h"
#include "srsran/interfaces/ue_nas_interfaces.h"
#include "srsran/srslog/srslog.h"
#include "srsue/hdr/stack/rrc/rrc.h"
#include <map>
#include <string>

#ifndef SRSRAN_RRC_PROCEDURES_H
#define SRSRAN_RRC_PROCEDURES_H

namespace srsue {

/********************************
 *         Procedures
 *******************************/

class rrc::cell_search_proc
{
public:
  enum class state_t { phy_cell_search, si_acquire, wait_measurement, phy_cell_select };

  explicit cell_search_proc(rrc* parent_);
  srsran::proc_outcome_t init();
  srsran::proc_outcome_t step();
  srsran::proc_outcome_t step_si_acquire();
  srsran::proc_outcome_t react(const phy_controller::cell_srch_res& event);
  srsran::proc_outcome_t react(const bool& event);
  srsran::proc_outcome_t step_wait_measurement();

  rrc_interface_phy_lte::cell_search_ret_t get_result() const { return search_result.cs_ret; }
  static const char*                       name() { return "Cell Search"; }

private:
  srsran::proc_outcome_t handle_cell_found(const phy_cell_t& new_cell);

  // conts
  rrc* rrc_ptr;

  // state vars
  phy_controller::cell_srch_res search_result;
  srsran::proc_future_t<void>   si_acquire_fut;
  state_t                       state;
};

/****************************************************************
 * TS 36.331 Sec 5.2.3 - Acquisition of an SI message procedure
 ***************************************************************/
class rrc::si_acquire_proc
{
public:
  const static int SIB_SEARCH_TIMEOUT_MS = 1000;
  struct si_acq_timer_expired {
    uint32_t timer_id;
  };
  struct sib_received_ev {};

  explicit si_acquire_proc(rrc* parent_);
  srsran::proc_outcome_t init(uint32_t sib_index_);
  srsran::proc_outcome_t step() { return srsran::proc_outcome_t::yield; }
  static const char*     name() { return "SI Acquire"; }
  srsran::proc_outcome_t react(si_acq_timer_expired ev);
  srsran::proc_outcome_t react(sib_received_ev ev);
  void                   then(const srsran::proc_state_t& result);

private:
  void start_si_acquire();

  // conts
  rrc*                  rrc_ptr;
  srslog::basic_logger& logger;

  // state
  srsran::timer_handler::unique_timer si_acq_timeout, si_acq_retry_timer;
  uint32_t                            period = 0, sched_index = 0;
  uint32_t                            sib_index = 0;
};

class rrc::serving_cell_config_proc
{
public:
  explicit serving_cell_config_proc(rrc* parent_);
  srsran::proc_outcome_t init(const std::vector<uint32_t>& required_sibs_);
  srsran::proc_outcome_t step();
  static const char*     name() { return "Serving Cell Configuration"; }

private:
  rrc*                  rrc_ptr;
  srslog::basic_logger& logger;

  srsran::proc_outcome_t launch_sib_acquire();

  // proc args
  std::vector<uint32_t> required_sibs;

  // state variables
  uint32_t                    req_idx = 0;
  srsran::proc_future_t<void> si_acquire_fut;
};

class rrc::cell_selection_proc
{
public:
  using cell_selection_complete_ev = srsran::proc_result_t<cs_result_t>;

  explicit cell_selection_proc(rrc* parent_);
  srsran::proc_outcome_t init(std::vector<uint32_t> required_sibs_ = {});
  srsran::proc_outcome_t step();
  cs_result_t            get_result() const { return cs_result; }
  static const char*     name() { return "Cell Selection"; }
  srsran::proc_outcome_t react(const bool& event);
  void                   then(const srsran::proc_result_t<cs_result_t>& proc_result) const;

private:
  srsran::proc_outcome_t start_next_cell_selection();
  srsran::proc_outcome_t step_cell_search();
  srsran::proc_outcome_t step_cell_config();
  bool                   is_serv_cell_suitable() const;
  bool                   is_sib_acq_required() const;
  srsran::proc_outcome_t set_proc_complete();
  srsran::proc_outcome_t start_phy_cell_selection(const meas_cell_eutra& cell);
  srsran::proc_outcome_t start_sib_acquisition();

  // consts
  rrc*                             rrc_ptr;
  meas_cell_list<meas_cell_eutra>* meas_cells;

  // state variables
  enum class search_state_t { cell_selection, serv_cell_camp, cell_config, cell_search };
  cs_result_t                                                     cs_result;
  search_state_t                                                  state;
  uint32_t                                                        neigh_index;
  bool                                                            serv_cell_select_attempted = false;
  srsran::proc_future_t<rrc_interface_phy_lte::cell_search_ret_t> cell_search_fut;
  srsran::proc_future_t<void>                                     serv_cell_cfg_fut;
  bool                                                            discard_serving = false, cell_search_called = false;
  std::vector<uint32_t>                                           required_sibs = {};
  phy_cell_t                                                      init_serv_cell;
};

class rrc::plmn_search_proc
{
public:
  explicit plmn_search_proc(rrc* parent_);
  srsran::proc_outcome_t init();
  srsran::proc_outcome_t step();
  void                   then(const srsran::proc_state_t& result) const;
  static const char*     name() { return "PLMN Search"; }

private:
  // consts
  rrc*                  rrc_ptr;
  srslog::basic_logger& logger;

  // state variables
  nas_interface_rrc::found_plmn_t                                 found_plmns[nas_interface_rrc::MAX_FOUND_PLMNS];
  int                                                             nof_plmns = 0;
  srsran::proc_future_t<rrc_interface_phy_lte::cell_search_ret_t> cell_search_fut;
};

class rrc::connection_request_proc
{
public:
  explicit connection_request_proc(rrc* parent_);
  srsran::proc_outcome_t init(srsran::establishment_cause_t cause_, srsran::unique_byte_buffer_t dedicated_info_nas_);
  srsran::proc_outcome_t step();
  void                   then(const srsran::proc_state_t& result);
  srsran::proc_outcome_t react(const cell_selection_proc::cell_selection_complete_ev& e);
  static const char*     name() { return "Connection Request"; }

private:
  // const
  rrc*                  rrc_ptr;
  srslog::basic_logger& logger;
  // args
  srsran::establishment_cause_t cause;
  srsran::unique_byte_buffer_t  dedicated_info_nas;

  // state variables
  enum class state_t { cell_selection, config_serving_cell, wait_t300 } state;
  cs_result_t                 cs_ret;
  srsran::proc_future_t<void> serv_cfg_fut;
};

class rrc::connection_setup_proc
{
public:
  explicit connection_setup_proc(rrc* parent_);
  srsran::proc_outcome_t init(const asn1::rrc::rr_cfg_ded_s* cnfg_, srsran::unique_byte_buffer_t dedicated_info_nas_);
  srsran::proc_outcome_t step() { return srsran::proc_outcome_t::yield; }
  void                   then(const srsran::proc_state_t& result);
  srsran::proc_outcome_t react(const bool& config_complete);
  static const char*     name() { return "Connection Setup"; }

private:
  // const
  rrc*                  rrc_ptr;
  srslog::basic_logger& logger;
  // args
  srsran::unique_byte_buffer_t dedicated_info_nas;
};

class rrc::connection_reconf_no_ho_proc
{
public:
  explicit connection_reconf_no_ho_proc(rrc* parent_);
  srsran::proc_outcome_t init(const asn1::rrc::rrc_conn_recfg_s& recfg_);
  srsran::proc_outcome_t step() { return srsran::proc_outcome_t::yield; }
  static const char*     name() { return "Connection Reconfiguration"; }
  srsran::proc_outcome_t react(const bool& config_complete);
  void                   then(const srsran::proc_state_t& result);

private:
  // const
  rrc* rrc_ptr;
  // args
  bool                               has_5g_nr_reconfig = false;
  asn1::rrc::rrc_conn_recfg_r8_ies_s rx_recfg;
};

class rrc::process_pcch_proc
{
public:
  struct paging_complete {
    bool outcome;
  };

  explicit process_pcch_proc(rrc* parent_);
  srsran::proc_outcome_t init(const asn1::rrc::paging_s& paging_);
  srsran::proc_outcome_t step();
  srsran::proc_outcome_t react(paging_complete e);
  static const char*     name() { return "Process PCCH"; }

private:
  // args
  rrc*                  rrc_ptr;
  srslog::basic_logger& logger;
  asn1::rrc::paging_s   paging;

  // vars
  uint32_t paging_idx = 0;
  enum class state_t { next_record, nas_paging, serv_cell_cfg } state;
  srsran::proc_future_t<void> serv_cfg_fut;
};

class rrc::go_idle_proc
{
public:
  explicit go_idle_proc(rrc* rrc_);
  srsran::proc_outcome_t init();
  srsran::proc_outcome_t step();
  srsran::proc_outcome_t react(bool timeout);
  static const char*     name() { return "Go Idle"; }
  void                   then(const srsran::proc_state_t& result);

private:
  static const uint32_t rlc_flush_timeout_ms = 60; // TS 36.331 Sec 5.3.8.3

  rrc*                                rrc_ptr;
  srsran::timer_handler::unique_timer rlc_flush_timer;
};

class rrc::cell_reselection_proc
{
public:
  /// Timer duration to restart Cell Reselection Procedure
  const static uint32_t cell_reselection_periodicity_ms = 20, cell_reselection_periodicity_long_ms = 1000;

  cell_reselection_proc(rrc* rrc_);
  srsran::proc_outcome_t init();
  srsran::proc_outcome_t step();
  static const char*     name() { return "Cell Reselection"; }
  void                   then(const srsran::proc_state_t& result);

private:
  rrc* rrc_ptr;

  srsran::timer_handler::unique_timer reselection_timer;
  srsran::proc_future_t<cs_result_t>  cell_selection_fut;
  cs_result_t                         cell_sel_result = cs_result_t::no_cell;
};

class rrc::connection_reest_proc
{
public:
  struct t311_expiry {};
  struct serv_cell_cfg_completed {};
  struct t301_expiry {};

  explicit connection_reest_proc(rrc* rrc_);
  // 5.3.7.2 - Initiation
  srsran::proc_outcome_t init(asn1::rrc::reest_cause_e cause);
  // 5.3.7.3 Actions following cell selection while T311 is running
  // && 5.3.7.4 - Actions related to transmission of RRCConnectionReestablishmentRequest message
  srsran::proc_outcome_t react(const cell_selection_proc::cell_selection_complete_ev& e);
  // 5.3.7.5 - Reception of the RRCConnectionReestablishment by the UE
  srsran::proc_outcome_t react(const asn1::rrc::rrc_conn_reest_s& reest_msg);
  // 5.3.7.6 - T311 expiry
  srsran::proc_outcome_t react(const t311_expiry& ev);
  // 5.3.7.7 - T301 expiry or selected cell no longer suitable
  srsran::proc_outcome_t react(const t301_expiry& ev);
  // detects if cell is no longer suitable (part of 5.3.7.7)
  srsran::proc_outcome_t step();
  // 5.3.7.8 - Reception of RRCConnectionReestablishmentReject by the UE
  srsran::proc_outcome_t react(const asn1::rrc::rrc_conn_reest_reject_s& reest_msg);
  static const char*     name() { return "Connection re-establishment"; }
  uint32_t               get_source_earfcn() const { return reest_source_freq; }

private:
  enum class state_t { wait_cell_selection, wait_reest_msg } state;

  rrc*                     rrc_ptr     = nullptr;
  asn1::rrc::reest_cause_e reest_cause = asn1::rrc::reest_cause_e::nulltype;
  uint16_t                 reest_rnti = 0, reest_source_pci = 0;
  uint32_t                 reest_source_freq = 0, reest_cellid = 0;

  bool                   passes_cell_criteria() const;
  srsran::proc_outcome_t cell_criteria();
  srsran::proc_outcome_t start_cell_selection();
};

class rrc::ho_proc
{
public:
  struct t304_expiry {};
  struct ra_completed_ev {
    bool success;
  };

  explicit ho_proc(rrc* rrc_);
  srsran::proc_outcome_t init(const asn1::rrc::rrc_conn_recfg_s& rrc_reconf);
  srsran::proc_outcome_t react(const bool& ev);
  srsran::proc_outcome_t react(t304_expiry ev);
  srsran::proc_outcome_t react(ra_completed_ev ev);
  srsran::proc_outcome_t step() { return srsran::proc_outcome_t::yield; }
  void                   then(const srsran::proc_state_t& result);
  static const char*     name() { return "Handover"; }

  phy_cell_t ho_src_cell;
  uint16_t   ho_src_rnti = 0;

private:
  rrc* rrc_ptr = nullptr;

  // args
  asn1::rrc::rrc_conn_recfg_r8_ies_s recfg_r8;

  // state
  phy_cell_t target_cell;

  // helper to revert security config of source cell
  void reset_security_config();
};

} // namespace srsue

#endif // SRSRAN_RRC_PROCEDURES_H
