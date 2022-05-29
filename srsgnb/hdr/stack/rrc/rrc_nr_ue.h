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

#ifndef SRSRAN_RRC_NR_UE_H
#define SRSRAN_RRC_NR_UE_H

#include "rrc_nr.h"
#include "rrc_nr_security_context.h"

namespace srsenb {

class rrc_nr::ue
{
public:
  enum activity_timeout_type_t {
    MSG3_RX_TIMEOUT = 0,   ///< Msg3 has its own timeout to quickly remove fake UEs from random PRACHs
    UE_INACTIVITY_TIMEOUT, ///< (currently unused) UE inactivity timeout (usually bigger than reestablishment timeout)
    MSG5_RX_TIMEOUT,       ///< (currently unused) for receiving RRCConnectionSetupComplete/RRCReestablishmentComplete
    nulltype
  };

  /// @param [in] start_msg3_timer: indicates whether the UE is created as part of a RACH process
  ue(rrc_nr* parent_, uint16_t rnti_, uint32_t pcell_cc_idx, bool start_msg3_timer = true);
  ~ue();

  int  handle_sgnb_addition_request(uint16_t eutra_rnti, const sgnb_addition_req_params_t& params);
  void crnti_ce_received();

  // getters
  bool     is_connected() { return state == rrc_nr_state_t::RRC_CONNECTED; }
  bool     is_idle() { return state == rrc_nr_state_t::RRC_IDLE; }
  bool     is_inactive() { return state == rrc_nr_state_t::RRC_INACTIVE; }
  bool     is_endc() { return endc; }
  uint16_t get_eutra_rnti() { return eutra_rnti; }
  void     get_metrics(rrc_ue_metrics_t& ue_metrics) { ue_metrics = {}; /*TODO fill RRC metrics*/ };

  // setters
  void set_security_key(const asn1::fixed_bitstring<256, false, true>& key) { sec_ctx.set_security_key(key); }
  void set_security_capabilities(const asn1::ngap::ue_security_cap_s& caps) { sec_ctx.set_security_capabilities(caps); }

  void deactivate_bearers();

  /// methods to handle activity timer
  std::string to_string(const activity_timeout_type_t& type);
  void        set_activity_timeout(activity_timeout_type_t type);
  void        set_activity(bool enabled = true);
  void        activity_timer_expired(const activity_timeout_type_t type);

  /** TS 38.331 - 5.3.3 RRC connection establishment */
  void handle_rrc_setup_request(const asn1::rrc_nr::rrc_setup_request_s& msg);
  void handle_rrc_setup_complete(const asn1::rrc_nr::rrc_setup_complete_s& msg);

  /** TS 38.331 - 5.3.4 Initial AS security activation */
  void handle_security_mode_complete(const asn1::rrc_nr::security_mode_complete_s& msg);

  /** TS 38.331 - 5.3.5 RRC reconfiguration */
  void handle_rrc_reconfiguration_complete(const asn1::rrc_nr::rrc_recfg_complete_s& msg);

  /** TS 38.331 - 5.3.7 RRC connection reestablishment */
  void handle_rrc_reestablishment_request(const asn1::rrc_nr::rrc_reest_request_s& msg);
  void handle_rrc_reestablishment_complete(const asn1::rrc_nr::rrc_reest_complete_s& msg);

  /** TS 38.331 - 5.3.8 Connection Release */
  void send_rrc_release();

  /* TS 38.331 - 5.6.1 UE capability transfer */
  void handle_ue_capability_information(const asn1::rrc_nr::ue_cap_info_s& msg);

  /** TS 38.331 - 5.7.1 DL information transfer */
  void send_dl_information_transfer(srsran::unique_byte_buffer_t sdu);

  /** TS 38.331 - 5.7.2 UL information transfer */
  void handle_ul_information_transfer(const asn1::rrc_nr::ul_info_transfer_s& msg);

  // NGAP interface
  void establish_eps_bearer(uint32_t pdu_session_id, srsran::const_byte_span nas_pdu, uint32_t lcid, uint32_t five_qi);

  /* TS 38.331 - 5.3.4 Initial AS security activation */
  void send_security_mode_command(srsran::unique_byte_buffer_t nas_pdu);

  /* TS 38.331 - 5.3.5 RRC reconfiguration */
  void send_rrc_reconfiguration();

  /* TS 38.331 - 5.6.1 UE capability transfer */
  int send_ue_capability_enquiry();

private:
  int send_dl_ccch(const asn1::rrc_nr::dl_ccch_msg_s& dl_ccch_msg);
  int send_dl_dcch(srsran::nr_srb srb, const asn1::rrc_nr::dl_dcch_msg_s& dl_dcch_msg);

  /** TS 38.331 - 5.3.3 RRC connection establishment */
  void send_rrc_setup();
  void send_rrc_reject(uint8_t reject_wait_time_secs);

  /** TS 38.331 - 5.3.7 RRC connection reestablishment */
  void send_connection_reest(uint8_t ncc);

  /// Update PDCP bearers based on ASN1 structs passed to the UE
  int update_pdcp_bearers(const asn1::rrc_nr::radio_bearer_cfg_s& radio_bearer_diff,
                          const asn1::rrc_nr::cell_group_cfg_s&   cell_group_diff);

  /// Update RLC bearers based on ASN1 structs passed to the UE
  int update_rlc_bearers(const asn1::rrc_nr::cell_group_cfg_s& cell_group_diff);

  /// Update MAC based on ASN1 message
  int update_mac(const asn1::rrc_nr::cell_group_cfg_s& cell_group_config, bool is_config_complete);

  /// Update AS security config on active RB
  int update_as_security(uint32_t lcid, bool enable_integrity, bool enable_ciphering);

  int pack_rrc_reconfiguration(asn1::dyn_octstring& packed_rrc_reconfig);
  int pack_secondary_cell_group_cfg(asn1::dyn_octstring& packed_secondary_cell_config);

  int pack_secondary_cell_group_mac_cfg(asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack);

  int pack_secondary_cell_group_sp_cell_cfg(asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack);

  int pack_sp_cell_cfg_ded(asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack);

  int pack_sp_cell_cfg_ded_init_dl_bwp(asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack);
  int pack_sp_cell_cfg_ded_init_dl_bwp_radio_link_monitoring(asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack);

  int pack_sp_cell_cfg_ded_ul_cfg(asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack);
  int pack_sp_cell_cfg_ded_ul_cfg_init_ul_bwp(asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack);
  int pack_sp_cell_cfg_ded_ul_cfg_init_ul_bwp_pucch_cfg(asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack);
  int pack_sp_cell_cfg_ded_ul_cfg_init_ul_bwp_pusch_cfg(asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack);

  int pack_sp_cell_cfg_ded_pdcch_serving_cell_cfg(asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack);

  int pack_recfg_with_sync(asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack);
  int pack_recfg_with_sync_sp_cell_cfg_common(asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack);
  int pack_recfg_with_sync_sp_cell_cfg_common_dl_cfg_common(asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack);
  int pack_recfg_with_sync_sp_cell_cfg_common_dl_cfg_common_phy_cell_group_cfg(
      asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack);

  int pack_recfg_with_sync_sp_cell_cfg_common_dl_cfg_init_dl_bwp(asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack);
  int pack_recfg_with_sync_sp_cell_cfg_common_dl_cfg_init_dl_bwp_pdsch_cfg_common(
      asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack);

  int pack_recfg_with_sync_sp_cell_cfg_common_ul_cfg_common(asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack);
  int pack_recfg_with_sync_sp_cell_cfg_common_ul_cfg_common_init_ul_bwp(
      asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack);
  int pack_recfg_with_sync_sp_cell_cfg_common_ul_cfg_common_init_ul_bwp_pusch_cfg_common(
      asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack);

  int pack_nr_radio_bearer_config(asn1::dyn_octstring& packed_nr_bearer_config);

  int add_drb(uint32_t five_qi);

  bool init_pucch();

  // logging helpers
  template <class M>
  void log_rrc_message(srsran::nr_srb          srb,
                       const direction_t       dir,
                       srsran::const_byte_span pdu,
                       const M&                msg,
                       const char*             msg_type);
  template <class M>
  void log_rrc_container(const direction_t dir, srsran::const_byte_span pdu, const M& msg, const char* msg_type);

  // args
  rrc_nr*               parent = nullptr;
  srslog::basic_logger& logger;
  uint16_t              rnti = SRSRAN_INVALID_RNTI;

  // state
  rrc_nr_state_t       state          = rrc_nr_state_t::RRC_IDLE;
  uint8_t              transaction_id = 0;
  srsran::unique_timer activity_timer; /// for basic DL/UL activity timeout

  // RRC configs for UEs
  asn1::rrc_nr::cell_group_cfg_s            cell_group_cfg, next_cell_group_cfg;
  asn1::rrc_nr::radio_bearer_cfg_s          radio_bearer_cfg, next_radio_bearer_cfg;
  std::vector<srsran::unique_byte_buffer_t> nas_pdu_queue;

  // MAC controller
  sched_nr_interface::ue_cfg_t uecfg{};

  const uint32_t drb1_lcid = 4;
  uint32_t       drb1_five_qi = 0; /// selected by 5GC

  // Security helper
  srsgnb::nr_security_context sec_ctx;

  // SA specific variables
  struct ctxt_t {
    uint64_t                               setup_ue_id = -1;
    asn1::rrc_nr::establishment_cause_opts connection_cause;
  } ctxt;

  // NSA specific variables
  bool     endc       = false;
  uint16_t eutra_rnti = SRSRAN_INVALID_RNTI;
};

} // namespace srsenb

#endif // SRSRAN_RRC_NR_UE_H
