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

#ifndef SRSENB_RRC_MOBILITY_H
#define SRSENB_RRC_MOBILITY_H

#include "rrc.h"
#include "rrc_ue.h"
#include "srsran/adt/fsm.h"
#include <map>

namespace srsenb {

enum class ho_interface_t { S1, X2, intra_enb };

class rrc::ue::rrc_mobility : public srsran::fsm_t<rrc::ue::rrc_mobility>
{
public:
  // public events
  struct user_crnti_upd_ev {
    uint16_t crnti;
    uint16_t temp_crnti;
  };
  struct ho_cancel_ev {
    asn1::s1ap::cause_c cause;

    ho_cancel_ev(const asn1::s1ap::cause_c& cause_) : cause(cause_) {}
  };

  rrc_mobility(srsenb::rrc::ue* outer_ue);

  bool fill_conn_recfg_no_ho_cmd(asn1::rrc::rrc_conn_recfg_r8_ies_s* conn_recfg);
  void handle_ue_meas_report(const asn1::rrc::meas_report_s& msg, srsran::unique_byte_buffer_t pdu);
  void handle_ho_preparation_complete(rrc::ho_prep_result          result,
                                      const asn1::s1ap::ho_cmd_s&  msg,
                                      srsran::unique_byte_buffer_t container);
  bool is_ho_running() const { return not is_in_state<idle_st>(); }

  // S1-Handover
  bool start_s1_tenb_ho(const asn1::s1ap::ho_request_s&                                   msg,
                        const asn1::s1ap::sourceenb_to_targetenb_transparent_container_s& container,
                        asn1::s1ap::cause_c&                                              cause);

private:
  // helper methods
  bool update_ue_var_meas_cfg(uint32_t               src_earfcn,
                              const enb_cell_common& target_pcell,
                              asn1::rrc::meas_cfg_s* diff_meas_cfg);

  // Handover from source cell
  bool
  start_ho_preparation(uint32_t target_eci, uint16_t target_tac, uint8_t measobj_id, bool fwd_direct_path_available);

  // Handover to target cell
  void fill_mobility_reconf_common(asn1::rrc::dl_dcch_msg_s& msg,
                                   const enb_cell_common&    target_cell,
                                   uint32_t                  src_dl_earfcn,
                                   uint32_t                  src_pci);
  bool apply_ho_prep_cfg(const asn1::rrc::ho_prep_info_r8_ies_s& ho_prep,
                         const asn1::s1ap::ho_request_s&         ho_req_msg,
                         std::vector<asn1::s1ap::erab_item_s>&   erabs_failed_to_setup,
                         asn1::s1ap::cause_c&                    cause);

  rrc::ue*              rrc_ue  = nullptr;
  rrc*                  rrc_enb = nullptr;
  srslog::basic_logger& logger;

  // vars
  asn1::rrc::meas_cfg_s                current_meas_cfg;
  asn1::rrc::rrc_conn_recfg_complete_s pending_recfg_complete;
  asn1::s1ap::cause_c                  failure_cause;

  // events
  struct ho_meas_report_ev {
    uint32_t                                target_eci      = 0;
    uint16_t                                target_tac      = 0;
    const asn1::rrc::meas_obj_to_add_mod_s* meas_obj        = nullptr;
    bool                                    direct_fwd_path = false;
  };
  struct ho_req_rx_ev {
    const asn1::s1ap::ho_request_s*                                   ho_req_msg;
    const asn1::s1ap::sourceenb_to_targetenb_transparent_container_s* transparent_container;
  };
  struct ho_failure_ev {
    asn1::s1ap::cause_c cause;
    ho_failure_ev(const asn1::s1ap::cause_c& cause_) : cause(cause_) {}
  };
  using recfg_complete_ev  = asn1::rrc::rrc_conn_recfg_complete_s;
  using status_transfer_ev = asn1::s1ap::bearers_subject_to_status_transfer_list_l;

  // states
  struct idle_st {};
  struct intraenb_ho_st {
    const enb_cell_common* target_cell     = nullptr;
    const enb_cell_common* source_cell     = nullptr;
    uint16_t               last_temp_crnti = SRSRAN_INVALID_RNTI;

    void enter(rrc_mobility* f, const ho_meas_report_ev& meas_report);
  };
  struct s1_target_ho_st {
    asn1::s1ap::cause_c   failure_cause;
    std::vector<uint32_t> pending_tunnels;
  };
  struct wait_recfg_comp {};
  struct s1_source_ho_st : public subfsm_t<s1_source_ho_st> {
    struct ho_cmd_msg {
      const asn1::s1ap::ho_cmd_s*       s1ap_ho_cmd;
      const asn1::rrc::ho_cmd_r8_ies_s* ho_cmd;
    };
    ho_meas_report_ev report;

    void enter(rrc_mobility* f, const ho_meas_report_ev& ev);

    struct wait_ho_cmd {};
    struct status_transfer_st {};

    explicit s1_source_ho_st(rrc_mobility* parent_);

  private:
    void                handle_ho_cmd(wait_ho_cmd& s, const ho_cmd_msg& ho_cmd);
    void                handle_ho_cancel(const ho_cancel_ev& ev);
    asn1::s1ap::cause_c start_enb_status_transfer(const asn1::s1ap::ho_cmd_s& s1ap_ho_cmd);

    rrc*                  rrc_enb;
    rrc::ue*              rrc_ue;
    srslog::basic_logger& logger;

  protected:
    using fsm = s1_source_ho_st;
    state_list<wait_ho_cmd, status_transfer_st> states{this};
    // clang-format off
    using transitions = transition_table<
    //           Start                 Target                   Event       Action                 Guard
    //      +-------------------+------------------+---------------------+-----------------------+---------------------+
    to_state<                     idle_st,            srsran::failure_ev                                               >,
    to_state<                     idle_st,            ho_cancel_ev,        &fsm::handle_ho_cancel                      >,
         row< wait_ho_cmd,        status_transfer_st, ho_cmd_msg,          &fsm::handle_ho_cmd                         >
    //      +-------------------+------------------+---------------------+-----------------------+---------------------+
    >;
    // clang-format on
  };

  // FSM guards
  bool needs_s1_ho(idle_st& s, const ho_meas_report_ev& meas_report);
  bool needs_intraenb_ho(idle_st& s, const ho_meas_report_ev& meas_report);

  // FSM transition handlers
  void handle_crnti_ce(intraenb_ho_st& s, const user_crnti_upd_ev& ev);
  void handle_recfg_complete(intraenb_ho_st& s, const recfg_complete_ev& ev);
  void handle_ho_requested(idle_st& s, const ho_req_rx_ev& ho_req);
  void handle_ho_failure(const ho_failure_ev& ev);
  void handle_status_transfer(s1_target_ho_st& s, const status_transfer_ev& ev);
  void defer_recfg_complete(s1_target_ho_st& s, const recfg_complete_ev& ev);
  void handle_recfg_complete(wait_recfg_comp& s, const recfg_complete_ev& ev);

protected:
  // states
  state_list<idle_st, intraenb_ho_st, s1_target_ho_st, wait_recfg_comp, s1_source_ho_st> states{this,
                                                                                                idle_st{},
                                                                                                intraenb_ho_st{},
                                                                                                s1_target_ho_st{},
                                                                                                wait_recfg_comp{},
                                                                                                s1_source_ho_st{this}};

  // transitions
  using fsm = rrc_mobility;
  // clang-format off
  using transitions = transition_table<
  //      Start         Target              Event                Action                      Guard
  // +----------------+-------------------+---------------------+----------------------------+-------------------------+
  row< idle_st,         s1_source_ho_st,   ho_meas_report_ev,   nullptr,                      &fsm::needs_s1_ho        >,
  row< idle_st,         intraenb_ho_st,    ho_meas_report_ev,   nullptr,                      &fsm::needs_intraenb_ho  >,
  row< idle_st,         s1_target_ho_st,   ho_req_rx_ev,        &fsm::handle_ho_requested                              >,
  // +----------------+-------------------+---------------------+----------------------------+-------------------------+
  upd< intraenb_ho_st,                     user_crnti_upd_ev,   &fsm::handle_crnti_ce                                  >,
  row< intraenb_ho_st,  idle_st,           recfg_complete_ev,   &fsm::handle_recfg_complete                            >,
  // +----------------+-------------------+---------------------+----------------------------+-------------------------+
  row< s1_target_ho_st, wait_recfg_comp,   status_transfer_ev,  &fsm::handle_status_transfer                           >,
  to_state<             idle_st,           ho_failure_ev,       &fsm::handle_ho_failure                                >,
  upd< s1_target_ho_st,                    recfg_complete_ev,   &fsm::defer_recfg_complete                             >,
  row< wait_recfg_comp, idle_st,           recfg_complete_ev,   &fsm::handle_recfg_complete                            >
  // +----------------+-------------------+---------------------+----------------------------+-------------------------+
  >;
  // clang-format on
};

} // namespace srsenb
#endif // SRSENB_RRC_MOBILITY_H
