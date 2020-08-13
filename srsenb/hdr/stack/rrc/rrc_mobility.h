/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSENB_RRC_MOBILITY_H
#define SRSENB_RRC_MOBILITY_H

#include "rrc.h"
#include "rrc_ue.h"
#include "srslte/common/fsm.h"
#include "srslte/common/logmap.h"
#include <map>

namespace srsenb {

/**
 * This class is responsible for storing the UE Measurement Configuration at the eNB side.
 * Has the same fields as asn1::rrc::var_meas_cfg but stored in data structs that are easier to handle
 */
class var_meas_cfg_t
{
public:
  using meas_cell_t  = asn1::rrc::cells_to_add_mod_s;
  using meas_id_t    = asn1::rrc::meas_id_to_add_mod_s;
  using meas_obj_t   = asn1::rrc::meas_obj_to_add_mod_s;
  using report_cfg_t = asn1::rrc::report_cfg_to_add_mod_s;

  var_meas_cfg_t() : rrc_log(srslte::logmap::get("RRC")) {}
  std::tuple<bool, meas_obj_t*, meas_cell_t*> add_cell_cfg(const meas_cell_cfg_t& cellcfg);
  report_cfg_t*                               add_report_cfg(const asn1::rrc::report_cfg_eutra_s& reportcfg);
  meas_id_t*                                  add_measid_cfg(uint8_t measobjid, uint8_t repid);
  asn1::rrc::quant_cfg_s*                     add_quant_cfg(const asn1::rrc::quant_cfg_eutra_s& quantcfg);

  bool compute_diff_meas_cfg(const var_meas_cfg_t& target_cfg, asn1::rrc::meas_cfg_s* meas_cfg) const;
  void compute_diff_meas_objs(const var_meas_cfg_t& target_cfg, asn1::rrc::meas_cfg_s* meas_cfg) const;
  void compute_diff_cells(const asn1::rrc::meas_obj_eutra_s& target_it,
                          const asn1::rrc::meas_obj_eutra_s& src_it,
                          asn1::rrc::meas_obj_to_add_mod_s*  added_obj) const;
  void compute_diff_report_cfgs(const var_meas_cfg_t& target_cfg, asn1::rrc::meas_cfg_s* meas_cfg) const;
  void compute_diff_meas_ids(const var_meas_cfg_t& target_cfg, asn1::rrc::meas_cfg_s* meas_cfg) const;
  void compute_diff_quant_cfg(const var_meas_cfg_t& target_cfg, asn1::rrc::meas_cfg_s* meas_cfg_msg) const;

  // getters
  const asn1::rrc::meas_obj_to_add_mod_list_l&   meas_objs() const { return var_meas.meas_obj_list; }
  const asn1::rrc::report_cfg_to_add_mod_list_l& rep_cfgs() const { return var_meas.report_cfg_list; }
  const asn1::rrc::meas_id_to_add_mod_list_l&    meas_ids() const { return var_meas.meas_id_list; }
  asn1::rrc::meas_obj_to_add_mod_list_l&         meas_objs() { return var_meas.meas_obj_list; }
  asn1::rrc::report_cfg_to_add_mod_list_l&       rep_cfgs() { return var_meas.report_cfg_list; }
  asn1::rrc::meas_id_to_add_mod_list_l&          meas_ids() { return var_meas.meas_id_list; }

  static var_meas_cfg_t make(const asn1::rrc::meas_cfg_s& meas_cfg);

private:
  asn1::rrc::var_meas_cfg_s var_meas;
  srslte::log_ref           rrc_log;
};

enum class ho_interface_t { S1, X2, intra_enb };

class rrc::enb_mobility_handler
{
public:
  explicit enb_mobility_handler(rrc* rrc_);

  //! Variable used to store the MeasConfig expected for each cell.
  // Note: Made const to forbid silent updates and enable comparison based on addr
  std::vector<std::shared_ptr<const var_meas_cfg_t> > cell_meas_cfg_list;

  rrc*       get_rrc() { return rrc_ptr; }
  const rrc* get_rrc() const { return rrc_ptr; }

  uint16_t start_ho_ue_resource_alloc(const asn1::s1ap::ho_request_s&                                   msg,
                                      const asn1::s1ap::sourceenb_to_targetenb_transparent_container_s& container,
                                      srslte::byte_buffer_t&                                            ho_cmd,
                                      std::vector<asn1::fixed_octstring<4, true> >&                     admitted_erabs);

private:
  // args
  rrc*             rrc_ptr = nullptr;
  const rrc_cfg_t* cfg     = nullptr;
};

class rrc::ue::rrc_mobility : public srslte::fsm_t<rrc::ue::rrc_mobility>
{
public:
  // public events
  struct user_crnti_upd_ev {
    uint16_t crnti;
    uint16_t temp_crnti;
  };

  explicit rrc_mobility(srsenb::rrc::ue* outer_ue);
  bool fill_conn_recfg_msg(asn1::rrc::rrc_conn_recfg_r8_ies_s* conn_recfg);
  void handle_ue_meas_report(const asn1::rrc::meas_report_s& msg);
  void handle_ho_preparation_complete(bool is_success, srslte::unique_byte_buffer_t container);
  bool is_ho_running() const { return not is_in_state<idle_st>(); }

  // S1-Handover
  bool start_s1_tenb_ho(const asn1::s1ap::ho_request_s&                                   msg,
                        const asn1::s1ap::sourceenb_to_targetenb_transparent_container_s& container,
                        srslte::byte_buffer_t&                                            ho_cmd,
                        std::vector<asn1::fixed_octstring<4, true> >&                     admitted_erabs);

private:
  // Handover from source cell
  bool start_ho_preparation(uint32_t target_eci, uint8_t measobj_id, bool fwd_direct_path_available);
  bool start_enb_status_transfer();

  // Handover to target cell
  bool update_ue_var_meas_cfg(const asn1::rrc::meas_cfg_s& source_meas_cfg,
                              uint32_t                     target_enb_cc_idx,
                              asn1::rrc::meas_cfg_s*       diff_meas_cfg);
  bool update_ue_var_meas_cfg(const var_meas_cfg_t&  source_var_meas_cfg,
                              uint32_t               target_enb_cc_idx,
                              asn1::rrc::meas_cfg_s* diff_meas_cfg);
  void fill_mobility_reconf_common(asn1::rrc::dl_dcch_msg_s& msg, const cell_info_common& target_cell);

  rrc::ue*                   rrc_ue  = nullptr;
  rrc*                       rrc_enb = nullptr;
  rrc::enb_mobility_handler* cfg     = nullptr;
  srslte::byte_buffer_pool*  pool    = nullptr;
  srslte::log_ref            rrc_log;

  // vars
  std::shared_ptr<const var_meas_cfg_t> ue_var_meas;

  // events
  struct ho_meas_report_ev {
    uint32_t                                target_eci = 0;
    const asn1::rrc::cells_to_add_mod_s*    meas_cell  = nullptr;
    const asn1::rrc::meas_obj_to_add_mod_s* meas_obj   = nullptr;
  };
  struct ho_req_rx_ev {
    asn1::rrc::rrc_conn_recfg_r8_ies_s                                ho_cmd;
    const asn1::s1ap::ho_request_s*                                   ho_req_msg;
    const asn1::rrc::ho_prep_info_r8_ies_s*                           ho_prep_r8;
    const asn1::s1ap::sourceenb_to_targetenb_transparent_container_s* transparent_container;
  };
  using unsuccessful_outcome_ev = std::false_type;
  using recfg_complete_ev       = asn1::rrc::rrc_conn_recfg_complete_s;

  // states
  struct idle_st {};
  struct intraenb_ho_st {
    const cell_info_common*    target_cell      = nullptr;
    const cell_ctxt_dedicated* source_cell_ctxt = nullptr;
    uint16_t                   last_temp_crnti  = SRSLTE_INVALID_RNTI;

    void enter(rrc_mobility* f, const ho_meas_report_ev& meas_report);
  };
  struct s1_target_ho_st {
    void enter(rrc_mobility* f, const ho_req_rx_ev& ho_req);
  };
  struct s1_source_ho_st : public subfsm_t<s1_source_ho_st> {
    ho_meas_report_ev report;

    struct wait_ho_req_ack_st {
      void enter(s1_source_ho_st* f, const ho_meas_report_ev& ev);
    };
    struct status_transfer_st {
      void enter(s1_source_ho_st* f);
    };

    explicit s1_source_ho_st(rrc_mobility* parent_) : base_t(parent_) {}

  private:
    bool send_ho_cmd(wait_ho_req_ack_st& s, const srslte::unique_byte_buffer_t& container);

  protected:
    using fsm = s1_source_ho_st;
    state_list<wait_ho_req_ack_st, status_transfer_st> states{this};
    // clang-format off
    using transitions = transition_table<
    //                 Start                 Target                   Event               Action    Guard
    //            +-------------------+------------------+------------------------------+---------+---------------------+
    from_any_state<                     idle_st,            srslte::failure_ev                                          >,
               row< wait_ho_req_ack_st, status_transfer_st, srslte::unique_byte_buffer_t, nullptr, &fsm::send_ho_cmd    >,
               row< wait_ho_req_ack_st, idle_st           , srslte::unique_byte_buffer_t                                >
    //            +-------------------+------------------+------------------------------+---------+---------------------+
    >;
    // clang-format on
  };

  // FSM guards
  bool needs_s1_ho(idle_st& s, const ho_meas_report_ev& meas_report);
  bool needs_intraenb_ho(idle_st& s, const ho_meas_report_ev& meas_report);

  // FSM transition handlers
  void handle_crnti_ce(intraenb_ho_st& s, const user_crnti_upd_ev& ev);
  void handle_recfg_complete(s1_target_ho_st& s, const recfg_complete_ev& ev);
  void handle_recfg_complete(intraenb_ho_st& s, const recfg_complete_ev& ev);

protected:
  // states
  state_list<idle_st, intraenb_ho_st, s1_target_ho_st, s1_source_ho_st> states{this,
                                                                               idle_st{},
                                                                               intraenb_ho_st{},
                                                                               s1_target_ho_st{},
                                                                               s1_source_ho_st{this}};

  // transitions
  using fsm = rrc_mobility;
  // clang-format off
  using transitions = transition_table<
  //      Start         Target             Event                Action                      Guard
  // +----------------+----------------+--------------------+---------------------------+-------------------------+
  row< idle_st,         s1_source_ho_st, ho_meas_report_ev,  nullptr,                     &fsm::needs_s1_ho       >,
  row< idle_st,         intraenb_ho_st,  ho_meas_report_ev,  nullptr,                     &fsm::needs_intraenb_ho >,
  row< idle_st,         s1_target_ho_st, ho_req_rx_ev                                                             >,
  // +----------------+----------------+--------------------+---------------------------+-------------------------+
  upd< intraenb_ho_st,                   user_crnti_upd_ev, &fsm::handle_crnti_ce                                 >,
  row< intraenb_ho_st,  idle_st,         recfg_complete_ev, &fsm::handle_recfg_complete                           >,
  // +----------------+----------------+--------------------+---------------------------+-------------------------+
  row< s1_target_ho_st, idle_st,         recfg_complete_ev, &fsm::handle_recfg_complete                           >
  // +----------------+----------------+--------------------+---------------------------+-------------------------+
  >;
  // clang-format on
};

} // namespace srsenb
#endif // SRSENB_RRC_MOBILITY_H
