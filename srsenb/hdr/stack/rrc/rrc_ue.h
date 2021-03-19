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

#ifndef SRSRAN_RRC_UE_H
#define SRSRAN_RRC_UE_H

#include "mac_controller.h"
#include "rrc.h"
#include "srsran/adt/mem_pool.h"
#include "srsran/interfaces/enb_phy_interfaces.h"
#include "srsran/interfaces/pdcp_interface_types.h"

namespace srsenb {

class rrc::ue
{
public:
  class rrc_mobility;

  ue(rrc* outer_rrc, uint16_t rnti, const sched_interface::ue_cfg_t& ue_cfg);
  ~ue();
  int  init();
  bool is_connected();
  bool is_idle();

  typedef enum {
    MSG3_RX_TIMEOUT = 0,    ///< Msg3 has its own timeout to quickly remove fake UEs from random PRACHs
    UE_INACTIVITY_TIMEOUT,  ///< UE inactivity timeout (usually bigger than reestablishment timeout)
    UE_REESTABLISH_TIMEOUT, ///< Maximum timeout in which UE reestablishment is expected
    nulltype
  } activity_timeout_type_t;
  std::string to_string(const activity_timeout_type_t& type);
  void        set_activity_timeout(const activity_timeout_type_t type);
  void        set_activity();
  void        activity_timer_expired();
  void        max_retx_reached();

  rrc_state_t get_state();
  void        get_metrics(rrc_ue_metrics_t& ue_metrics) const;

  ///< Helper to access a cell cfg based on ue_cc_idx
  enb_cell_common* get_ue_cc_cfg(uint32_t ue_cc_idx);

  void send_connection_setup();
  void send_connection_reest(uint8_t ncc);
  void send_connection_reject();
  void send_connection_release();
  void send_connection_reest_rej();
  void send_connection_reconf(srsran::unique_byte_buffer_t           sdu             = {},
                              bool                                   phy_cfg_updated = true,
                              const asn1::unbounded_octstring<true>* nas_pdu         = nullptr);
  void send_security_mode_command();
  void send_ue_cap_enquiry();
  void send_ue_info_req();

  void parse_ul_dcch(uint32_t lcid, srsran::unique_byte_buffer_t pdu);

  /// List of results for a connection request.
  enum class conn_request_result_t { success, error_mme_not_connected, error_unknown_rnti };

  /// Possible causes for the RRC to transition to the idle state.
  enum class rrc_idle_transition_cause { release, timeout };

  void handle_rrc_con_req(asn1::rrc::rrc_conn_request_s* msg);
  void handle_rrc_con_setup_complete(asn1::rrc::rrc_conn_setup_complete_s* msg, srsran::unique_byte_buffer_t pdu);
  void handle_rrc_con_reest_req(asn1::rrc::rrc_conn_reest_request_s* msg);
  void handle_rrc_con_reest_complete(asn1::rrc::rrc_conn_reest_complete_s* msg, srsran::unique_byte_buffer_t pdu);
  void handle_rrc_reconf_complete(asn1::rrc::rrc_conn_recfg_complete_s* msg, srsran::unique_byte_buffer_t pdu);
  void handle_security_mode_complete(asn1::rrc::security_mode_complete_s* msg);
  void handle_security_mode_failure(asn1::rrc::security_mode_fail_s* msg);
  bool handle_ue_cap_info(asn1::rrc::ue_cap_info_s* msg);
  void handle_ue_init_ctxt_setup_req(const asn1::s1ap::init_context_setup_request_s& msg);
  bool handle_ue_ctxt_mod_req(const asn1::s1ap::ue_context_mod_request_s& msg);
  void handle_ue_info_resp(const asn1::rrc::ue_info_resp_r9_s& msg, srsran::unique_byte_buffer_t pdu);

  void set_bitrates(const asn1::s1ap::ue_aggregate_maximum_bitrate_s& rates);

  bool setup_erabs(const asn1::s1ap::erab_to_be_setup_list_ctxt_su_req_l& e);
  bool setup_erabs(const asn1::s1ap::erab_to_be_setup_list_bearer_su_req_l& e);
  bool release_erabs();
  bool release_erab(uint32_t erab_id);
  bool modify_erab(uint16_t                                   erab_id,
                   const asn1::s1ap::erab_level_qos_params_s& qos_params,
                   const asn1::unbounded_octstring<true>*     nas_pdu);

  void notify_s1ap_ue_ctxt_setup_complete();
  void notify_s1ap_ue_erab_setup_response(const asn1::s1ap::erab_to_be_setup_list_bearer_su_req_l& e);

  // Getters for PUCCH resources
  int  get_cqi(uint16_t* pmi_idx, uint16_t* n_pucch, uint32_t ue_cc_idx);
  int  get_ri(uint32_t m_ri, uint16_t* ri_idx);
  bool is_allocated() const;
  bool is_crnti_set() const { return mac_ctrl.is_crnti_set(); }

  void send_dl_ccch(asn1::rrc::dl_ccch_msg_s* dl_ccch_msg);
  bool send_dl_dcch(const asn1::rrc::dl_dcch_msg_s* dl_dcch_msg,
                    srsran::unique_byte_buffer_t    pdu = srsran::unique_byte_buffer_t());

  void save_ul_message(srsran::unique_byte_buffer_t pdu) { last_ul_msg = std::move(pdu); }

  uint16_t rnti   = 0;
  rrc*     parent = nullptr;

  bool                          connect_notified = false;
  std::unique_ptr<rrc_mobility> mobility_handler;

  bool is_csfb = false;

  void* operator new(size_t sz);
  void* operator new[](size_t sz) = delete;
  void  operator delete(void* ptr)noexcept;
  void  operator delete[](void* ptr) = delete;

  using ue_pool_t = srsran::background_allocator_obj_pool<ue, 16, 4>;
  static ue_pool_t* get_ue_pool();

private:
  // args
  srsran::timer_handler::unique_timer activity_timer;

  /// cached ASN1 fields for RRC config update checking, and ease of context transfer during HO
  ue_var_cfg_t current_ue_cfg;

  asn1::rrc::establishment_cause_e establishment_cause;

  // S-TMSI for this UE
  bool     has_tmsi = false;
  uint32_t m_tmsi   = 0;
  uint8_t  mmec     = 0;

  // state
  uint32_t                                     rlf_cnt              = 0;
  uint8_t                                      transaction_id       = 0;
  rrc_state_t                                  state                = RRC_STATE_IDLE;
  uint16_t                                     old_reest_rnti       = SRSRAN_INVALID_RNTI;
  std::map<uint16_t, srsran::pdcp_lte_state_t> old_reest_pdcp_state = {};
  bool                                         rlf_info_pending     = false;

  asn1::s1ap::ue_aggregate_maximum_bitrate_s bitrates;
  bool                                       eutra_capabilities_unpacked = false;
  asn1::rrc::ue_eutra_cap_s                  eutra_capabilities;
  srsran::rrc_ue_capabilities_t              ue_capabilities;

  const static uint32_t UE_PCELL_CC_IDX = 0;

  ue_cell_ded_list     ue_cell_list;
  bearer_cfg_handler   bearer_list;
  security_cfg_handler ue_security_cfg;

  /// Cached message of the last uplinl message.
  srsran::unique_byte_buffer_t last_ul_msg;

  // controllers
  mac_controller mac_ctrl;

  /// Helper to fill cell_ded_list with SCells provided in the eNB config
  void update_scells();

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
  void apply_pdcp_srb_updates(const asn1::rrc::rr_cfg_ded_s& pending_rr_cfg);
  void apply_pdcp_drb_updates(const asn1::rrc::rr_cfg_ded_s& pending_rr_cfg);
  void apply_rlc_rb_updates(const asn1::rrc::rr_cfg_ded_s& pending_rr_cfg);
}; // class ue

} // namespace srsenb

#endif // SRSRAN_RRC_UE_H
