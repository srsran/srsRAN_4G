/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSENB_RRC_ENDC_H
#define SRSENB_RRC_ENDC_H

#include "rrc.h"
#include "rrc_ue.h"
#include "srsran/adt/fsm.h"
#include <map>

namespace srsenb {

/**
 * @brief This procedure handles the secondary node (SgNB) addition for
 * EUTRA-NR Dual connectivity (ENDC)
 *
 */

class rrc::ue::rrc_endc : public srsran::fsm_t<rrc::ue::rrc_endc>
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

  rrc_endc(srsenb::rrc::ue* outer_ue);

  bool fill_conn_recfg(asn1::rrc::rrc_conn_recfg_r8_ies_s* conn_recfg);
  void handle_eutra_capabilities(const asn1::rrc::ue_eutra_cap_s& eutra_caps);
  void handle_ue_meas_report(const asn1::rrc::meas_report_s& msg);
  void handle_sgnb_addition_ack(const asn1::dyn_octstring& nr_secondary_cell_group_cfg_r15,
                                const asn1::dyn_octstring& nr_radio_bearer_cfg1_r15);
  void handle_sgnb_addition_reject();
  void handle_sgnb_addition_complete();
  bool is_endc_supported();

private:
  // Send SgNB addition request to gNB
  bool start_sgnb_addition();

  bool is_endc_activation_running() const { return not is_in_state<idle_st>(); }

  rrc::ue*              rrc_ue  = nullptr;
  rrc*                  rrc_enb = nullptr;
  srslog::basic_logger& logger;

  // vars
  bool                                 endc_supported = false;
  asn1::rrc::rrc_conn_recfg_complete_s pending_recfg_complete;

  // temporary storage for NR reconfiguration
  asn1::dyn_octstring nr_secondary_cell_group_cfg_r15;
  asn1::dyn_octstring nr_radio_bearer_cfg1_r15;

  // events
  struct sgnb_add_req_sent_ev {};
  struct sgnb_add_req_ack_ev {};
  struct sgnb_add_req_reject_ev {};
  struct rrc_recfg_sent_ev {};
  struct prach_nr_received_ev {};

  using recfg_complete_ev  = asn1::rrc::rrc_conn_recfg_complete_s;
  using status_transfer_ev = asn1::s1ap::bearers_subject_to_status_transfer_list_l;

  // states
  struct idle_st {};
  struct wait_sgnb_add_req_resp {};
  struct prepare_recfg {};
  struct wait_recfg_comp {};
  struct wait_prach_nr {};

  // FSM guards

  // FSM transition handlers
  void handle_recfg_complete(wait_recfg_comp& s, const recfg_complete_ev& ev);
  void handle_sgnb_addition_request_sent(const sgnb_add_req_sent_ev& ev);

protected:
  // states
  state_list<idle_st, wait_sgnb_add_req_resp, prepare_recfg, wait_recfg_comp, wait_prach_nr>
      states{this, idle_st{}, wait_sgnb_add_req_resp{}, prepare_recfg{}, wait_recfg_comp{}, wait_prach_nr{}};

  // transitions
  using fsm = rrc_endc;
  // clang-format off
  using transitions = transition_table<
  //      Start                Target                  Event                   Action                      Guard
  // +-----------------------+-----------------------+------------------------+----------------------------+-------------------------+
  row< idle_st,                wait_sgnb_add_req_resp, sgnb_add_req_sent_ev,   nullptr                                                >,
  // +-----------------------+-----------------------+------------------------+----------------------------+-------------------------+
  row< wait_sgnb_add_req_resp, prepare_recfg,          sgnb_add_req_ack_ev                                                            >,
  row< wait_sgnb_add_req_resp, idle_st,                sgnb_add_req_reject_ev                                                         >,
  row< prepare_recfg,          wait_recfg_comp,        rrc_recfg_sent_ev                                                              >,
  row< wait_recfg_comp,        idle_st,                recfg_complete_ev,      &fsm::handle_recfg_complete                            >
  // +-----------------------+-----------------------+------------------------+----------------------------+-------------------------+
  >;
  // clang-format on
};

} // namespace srsenb

#endif // SRSENB_RRC_ENDC_H
