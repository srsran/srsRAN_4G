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

#ifndef SRSLTE_RRC_UE_H
#define SRSLTE_RRC_UE_H

#include "rrc.h"

namespace srsenb {

class rrc::ue
{
public:
  class rrc_mobility;

  ue(rrc* outer_rrc, uint16_t rnti, const sched_interface::ue_cfg_t& ue_cfg);
  ~ue();
  bool is_connected();
  bool is_idle();

  typedef enum {
    MSG3_RX_TIMEOUT = 0,    ///< Msg3 has its own timeout to quickly remove fake UEs from random PRACHs
    UE_INACTIVITY_TIMEOUT,  ///< UE inactivity timeout
    nulltype
  } activity_timeout_type_t;
  std::string to_string(const activity_timeout_type_t& type);
  void        set_activity_timeout(const activity_timeout_type_t type);
  void        set_activity();
  void        activity_timer_expired();

  rrc_state_t get_state();

  void send_connection_setup();
  void send_connection_reest(uint8_t ncc);
  void send_connection_reject();
  void send_connection_release();
  void send_connection_reest_rej();
  void send_connection_reconf(srslte::unique_byte_buffer_t sdu);
  void send_connection_reconf_new_bearer();
  void send_connection_reconf_upd(srslte::unique_byte_buffer_t pdu);
  void send_security_mode_command();
  void send_ue_cap_enquiry();
  void parse_ul_dcch(uint32_t lcid, srslte::unique_byte_buffer_t pdu);

  void handle_rrc_con_req(asn1::rrc::rrc_conn_request_s* msg);
  void handle_rrc_con_reest_req(asn1::rrc::rrc_conn_reest_request_r8_ies_s* msg);
  void handle_rrc_con_setup_complete(asn1::rrc::rrc_conn_setup_complete_s* msg, srslte::unique_byte_buffer_t pdu);
  void handle_rrc_con_reest_req(asn1::rrc::rrc_conn_reest_request_s* msg);
  void handle_rrc_con_reest_complete(asn1::rrc::rrc_conn_reest_complete_s* msg, srslte::unique_byte_buffer_t pdu);
  void handle_rrc_reconf_complete(asn1::rrc::rrc_conn_recfg_complete_s* msg, srslte::unique_byte_buffer_t pdu);
  void handle_security_mode_complete(asn1::rrc::security_mode_complete_s* msg);
  void handle_security_mode_failure(asn1::rrc::security_mode_fail_s* msg);
  bool handle_ue_cap_info(asn1::rrc::ue_cap_info_s* msg);
  void handle_ue_init_ctxt_setup_req(const asn1::s1ap::init_context_setup_request_s& msg);
  bool handle_ue_ctxt_mod_req(const asn1::s1ap::ue_context_mod_request_s& msg);

  void set_bitrates(const asn1::s1ap::ue_aggregate_maximum_bitrate_s& rates);

  bool setup_erabs(const asn1::s1ap::erab_to_be_setup_list_ctxt_su_req_l& e);
  bool setup_erabs(const asn1::s1ap::erab_to_be_setup_list_bearer_su_req_l& e);
  bool release_erabs();

  // handover
  void handle_ho_preparation_complete(bool is_success, srslte::unique_byte_buffer_t container);

  void notify_s1ap_ue_ctxt_setup_complete();
  void notify_s1ap_ue_erab_setup_response(const asn1::s1ap::erab_to_be_setup_list_bearer_su_req_l& e);

  // Getters for PUCCH resources
  int  get_cqi(uint16_t* pmi_idx, uint16_t* n_pucch, uint32_t ue_cc_idx);
  int  get_ri(uint32_t m_ri, uint16_t* ri_idx);
  bool is_allocated() const;

  void send_dl_ccch(asn1::rrc::dl_ccch_msg_s* dl_ccch_msg);
  bool send_dl_dcch(const asn1::rrc::dl_dcch_msg_s* dl_dcch_msg,
                    srslte::unique_byte_buffer_t    pdu = srslte::unique_byte_buffer_t());

  uint16_t rnti   = 0;
  rrc*     parent = nullptr;

  bool                          connect_notified = false;
  std::unique_ptr<rrc_mobility> mobility_handler;

  bool is_csfb = false;

private:
  // args
  srslte::byte_buffer_pool*           pool = nullptr;
  srslte::timer_handler::unique_timer activity_timer;

  // cached for ease of context transfer
  asn1::rrc::rrc_conn_recfg_s last_rrc_conn_recfg;

  asn1::rrc::establishment_cause_e establishment_cause;

  // S-TMSI for this UE
  bool     has_tmsi = false;
  uint32_t m_tmsi   = 0;
  uint8_t  mmec     = 0;

  // state
  uint32_t                                     rlf_cnt              = 0;
  uint8_t                                      transaction_id       = 0;
  rrc_state_t                                  state                = RRC_STATE_IDLE;
  uint16_t                                     old_reest_rnti       = SRSLTE_INVALID_RNTI;
  std::map<uint16_t, srslte::pdcp_lte_state_t> old_reest_pdcp_state = {};

  asn1::s1ap::ue_aggregate_maximum_bitrate_s bitrates;
  bool                                       eutra_capabilities_unpacked = false;
  asn1::rrc::ue_eutra_cap_s                  eutra_capabilities;
  srslte::rrc_ue_capabilities_t              ue_capabilities;

  const static uint32_t UE_PCELL_CC_IDX = 0;

  cell_ctxt_dedicated_list cell_ded_list;
  bearer_cfg_handler       bearer_list;
  security_cfg_handler     ue_security_cfg;

  class mac_controller;
  std::unique_ptr<mac_controller> mac_ctrl;

  ///< Helper to fill RR config dedicated struct for RRR Connection Setup/Reestablish
  void fill_rrc_setup_rr_config_dedicated(asn1::rrc::rr_cfg_ded_s* rr_cfg);

  ///< Helper to access a cell cfg based on ue_cc_idx
  cell_info_common* get_ue_cc_cfg(uint32_t ue_cc_idx);

  ///< Helper to fill SCell struct for RRR Connection Reconfig
  int fill_scell_to_addmod_list(asn1::rrc::rrc_conn_recfg_r8_ies_s* conn_reconf);

  ///< UE's Physical layer dedicated configuration
  phy_interface_rrc_lte::phy_rrc_cfg_list_t phy_rrc_dedicated_list = {};

  /**
   * Setups the PCell physical layer common configuration of the UE from the SIB2 message. This methods is designed to
   * be called from the constructor.
   *
   * @param config ASN1 Common SIB struct carrying the common physical layer parameters
   */
  void apply_setup_phy_common(const asn1::rrc::rr_cfg_common_sib_s& config, bool update_phy);

  /**
   * Setups the PCell physical layer dedicated configuration of the UE. This method shall be called from the
   * connection setup only.
   *
   * @param phys_cfg_ded ASN1 Physical layer configuration dedicated
   */
  void apply_setup_phy_config_dedicated(const asn1::rrc::phys_cfg_ded_s& phys_cfg_ded);

  /**
   * Reconfigures the PCell and SCell physical layer dedicated configuration of the UE. This method shall be called
   * from the connection reconfiguration. `apply_setup_phy_config` shall not be called before/after. It automatically
   * parses the PCell and SCell reconfiguration.
   *
   * @param reconfig_r8 ASN1 reconfiguration message
   */
  void apply_reconf_phy_config(const asn1::rrc::rrc_conn_recfg_r8_ies_s& reconfig_r8, bool update_phy);

  /**
   * Reconfigures PDCP bearers
   * @param srbs_to_add SRBs to add
   */
  void apply_pdcp_srb_updates();
  void apply_pdcp_drb_updates();
  void apply_rlc_rb_updates();
}; // class ue

} // namespace srsenb

#endif // SRSLTE_RRC_UE_H
