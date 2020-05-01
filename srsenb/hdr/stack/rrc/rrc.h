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

#ifndef SRSENB_RRC_H
#define SRSENB_RRC_H

#include "rrc_cell_cfg.h"
#include "rrc_metrics.h"
#include "srsenb/hdr/stack/upper/common_enb.h"
#include "srslte/common/block_queue.h"
#include "srslte/common/buffer_pool.h"
#include "srslte/common/common.h"
#include "srslte/common/logmap.h"
#include "srslte/common/stack_procedure.h"
#include "srslte/common/timeout.h"
#include "srslte/interfaces/enb_interfaces.h"
#include <map>
#include <queue>

namespace srsenb {

static const char rrc_state_text[RRC_STATE_N_ITEMS][100] = {"IDLE",
                                                            "WAIT FOR CON SETUP COMPLETE",
                                                            "WAIT FOR SECURITY MODE COMPLETE",
                                                            "WAIT FOR UE CAPABILITIY INFORMATION",
                                                            "WAIT FOR CON RECONF COMPLETE",
                                                            "RRC CONNECTED",
                                                            "RELEASE REQUEST"};

class rrc final : public rrc_interface_pdcp,
                  public rrc_interface_mac,
                  public rrc_interface_rlc,
                  public rrc_interface_s1ap
{
public:
  rrc();
  ~rrc();

  void init(const rrc_cfg_t&       cfg_,
            phy_interface_rrc_lte* phy,
            mac_interface_rrc*     mac,
            rlc_interface_rrc*     rlc,
            pdcp_interface_rrc*    pdcp,
            s1ap_interface_rrc*    s1ap,
            gtpu_interface_rrc*    gtpu,
            srslte::timer_handler* timers_);

  void stop();
  void get_metrics(rrc_metrics_t& m);
  void tti_clock();

  // rrc_interface_mac
  void     rl_failure(uint16_t rnti) override;
  void     add_user(uint16_t rnti, const sched_interface::ue_cfg_t& init_ue_cfg) override;
  void     upd_user(uint16_t new_rnti, uint16_t old_rnti) override;
  void     set_activity_user(uint16_t rnti) override;
  bool     is_paging_opportunity(uint32_t tti, uint32_t* payload_len) override;
  uint8_t* read_pdu_bcch_dlsch(const uint8_t cc_idx, const uint32_t sib_index) override;

  // rrc_interface_rlc
  void read_pdu_pcch(uint8_t* payload, uint32_t buffer_size) override;
  void max_retx_attempted(uint16_t rnti) override;

  // rrc_interface_s1ap
  void write_dl_info(uint16_t rnti, srslte::unique_byte_buffer_t sdu) override;
  void release_complete(uint16_t rnti) override;
  bool setup_ue_ctxt(uint16_t rnti, const asn1::s1ap::init_context_setup_request_s& msg) override;
  bool modify_ue_ctxt(uint16_t rnti, const asn1::s1ap::ue_context_mod_request_s& msg) override;
  bool setup_ue_erabs(uint16_t rnti, const asn1::s1ap::erab_setup_request_s& msg) override;
  bool release_erabs(uint32_t rnti) override;
  void add_paging_id(uint32_t ueid, const asn1::s1ap::ue_paging_id_c& UEPagingID) override;
  void ho_preparation_complete(uint16_t rnti, bool is_success, srslte::unique_byte_buffer_t rrc_container) override;

  // rrc_interface_pdcp
  void write_pdu(uint16_t rnti, uint32_t lcid, srslte::unique_byte_buffer_t pdu) override;

  uint32_t get_nof_users();

  // logging
  typedef enum { Rx = 0, Tx } direction_t;
  template <class T>
  void log_rrc_message(const std::string&           source,
                       direction_t                  dir,
                       const srslte::byte_buffer_t* pdu,
                       const T&                     msg,
                       const std::string&           msg_type);

private:
  class ue
  {
  public:
    class rrc_mobility;

    ue(rrc* outer_rrc, uint16_t rnti, const sched_interface::ue_cfg_t& ue_cfg);
    ~ue();
    bool is_connected();
    bool is_idle();

    typedef enum {
      MSG3_RX_TIMEOUT = 0,    ///< Msg3 has its own timeout to quickly remove fake UEs from random PRACHs
      UE_RESPONSE_RX_TIMEOUT, ///< General purpose timeout for responses to eNB requests
      UE_INACTIVITY_TIMEOUT,  ///< UE inactivity timeout
      nulltype
    } activity_timeout_type_t;
    std::string to_string(const activity_timeout_type_t& type);
    void        set_activity_timeout(const activity_timeout_type_t type);
    void        set_activity();
    void        activity_timer_expired();

    uint32_t rl_failure();

    rrc_state_t get_state();

    void send_connection_setup(bool is_setup = true);
    void send_connection_reest();
    void send_connection_reject();
    void send_connection_release();
    void send_connection_reest_rej();
    void send_connection_reconf(srslte::unique_byte_buffer_t sdu);
    void send_connection_reconf_new_bearer(const asn1::s1ap::erab_to_be_setup_list_bearer_su_req_l& e);
    void send_connection_reconf_upd(srslte::unique_byte_buffer_t pdu);
    void send_security_mode_command();
    void send_ue_cap_enquiry();
    void parse_ul_dcch(uint32_t lcid, srslte::unique_byte_buffer_t pdu);

    void handle_rrc_con_req(asn1::rrc::rrc_conn_request_s* msg);
    void handle_rrc_con_reest_req(asn1::rrc::rrc_conn_reest_request_r8_ies_s* msg);
    void handle_rrc_con_setup_complete(asn1::rrc::rrc_conn_setup_complete_s* msg, srslte::unique_byte_buffer_t pdu);
    void handle_rrc_reconf_complete(asn1::rrc::rrc_conn_recfg_complete_s* msg, srslte::unique_byte_buffer_t pdu);
    void handle_security_mode_complete(asn1::rrc::security_mode_complete_s* msg);
    void handle_security_mode_failure(asn1::rrc::security_mode_fail_s* msg);
    bool handle_ue_cap_info(asn1::rrc::ue_cap_info_s* msg);

    void set_bitrates(const asn1::s1ap::ue_aggregate_maximum_bitrate_s& rates);
    void set_security_capabilities(const asn1::s1ap::ue_security_cap_s& caps);
    void set_security_key(const asn1::fixed_bitstring<256, false, true>& key);

    bool setup_erabs(const asn1::s1ap::erab_to_be_setup_list_ctxt_su_req_l& e);
    bool setup_erabs(const asn1::s1ap::erab_to_be_setup_list_bearer_su_req_l& e);
    void setup_erab(uint8_t                                            id,
                    const asn1::s1ap::erab_level_qos_params_s&         qos,
                    const asn1::bounded_bitstring<1, 160, true, true>& addr,
                    uint32_t                                           teid_out,
                    const asn1::unbounded_octstring<true>*             nas_pdu);
    bool release_erabs();

    // handover
    void handle_ho_preparation_complete(bool is_success, srslte::unique_byte_buffer_t container);

    void notify_s1ap_ue_ctxt_setup_complete();
    void notify_s1ap_ue_erab_setup_response(const asn1::s1ap::erab_to_be_setup_list_bearer_su_req_l& e);

    // Getters for PUCCH resources
    int  get_cqi(uint16_t* pmi_idx, uint16_t* n_pucch, uint32_t ue_cc_idx);
    int  get_ri(uint32_t m_ri, uint16_t* ri_idx);
    bool is_allocated() const;

    bool select_security_algorithms();
    void send_dl_ccch(asn1::rrc::dl_ccch_msg_s* dl_ccch_msg);
    void send_dl_dcch(asn1::rrc::dl_dcch_msg_s*    dl_dcch_msg,
                      srslte::unique_byte_buffer_t pdu = srslte::unique_byte_buffer_t());

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
    asn1::rrc::rrc_conn_recfg_s         last_rrc_conn_recfg;
    asn1::rrc::security_algorithm_cfg_s last_security_mode_cmd;

    asn1::rrc::establishment_cause_e establishment_cause;

    // S-TMSI for this UE
    bool     has_tmsi = false;
    uint32_t m_tmsi   = 0;
    uint8_t  mmec     = 0;

    // state
    sched_interface::ue_cfg_t current_sched_ue_cfg = {};
    uint32_t                  rlf_cnt              = 0;
    uint8_t                   transaction_id       = 0;
    rrc_state_t               state                = RRC_STATE_IDLE;

    std::map<uint32_t, asn1::rrc::srb_to_add_mod_s> srbs;
    std::map<uint32_t, asn1::rrc::drb_to_add_mod_s> drbs;

    uint8_t                      k_enb[32]; // Provided by MME
    srslte::as_security_config_t sec_cfg = {};

    asn1::s1ap::ue_aggregate_maximum_bitrate_s bitrates;
    asn1::s1ap::ue_security_cap_s              security_capabilities;
    bool                                       eutra_capabilities_unpacked = false;
    asn1::rrc::ue_eutra_cap_s                  eutra_capabilities;
    srslte::rrc_ue_capabilities_t              ue_capabilities;

    struct erab_t {
      uint8_t                                     id;
      asn1::s1ap::erab_level_qos_params_s         qos_params;
      asn1::bounded_bitstring<1, 160, true, true> address;
      uint32_t                                    teid_out;
      uint32_t                                    teid_in;
    };
    std::map<uint8_t, erab_t> erabs;

    std::map<uint8_t, srslte::unique_byte_buffer_t> erab_info_list;

    const static uint32_t UE_PCELL_CC_IDX = 0;

    cell_ctxt_dedicated_list cell_ded_list;

    int get_drbid_config(asn1::rrc::drb_to_add_mod_s* drb, int drbid);

    ///< Helper to access a cell cfg based on ue_cc_idx
    cell_info_common* get_ue_cc_cfg(uint32_t ue_cc_idx);

    ///< Helper to fill SCell struct for RRR Connection Reconfig
    int fill_scell_to_addmod_list(asn1::rrc::rrc_conn_recfg_r8_ies_s* conn_reconf);

    ///< UE's Physical layer dedicated configuration
    phy_interface_rrc_lte::phy_rrc_dedicated_list_t phy_rrc_dedicated_list = {};

    /**
     * Setups the PCell physical layer common configuration of the UE from the SIB2 message. This methods is designed to
     * be called from the constructor.
     *
     * @param config ASN1 Common SIB struct carrying the common physical layer parameters
     */
    void apply_setup_phy_common(const asn1::rrc::rr_cfg_common_sib_s& config);

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
    void apply_reconf_phy_config(const asn1::rrc::rrc_conn_recfg_r8_ies_s& reconfig_r8);
  }; // class ue

  // args
  srslte::timer_handler*    timers = nullptr;
  srslte::byte_buffer_pool* pool   = nullptr;
  phy_interface_rrc_lte*    phy    = nullptr;
  mac_interface_rrc*        mac    = nullptr;
  rlc_interface_rrc*        rlc    = nullptr;
  pdcp_interface_rrc*       pdcp   = nullptr;
  gtpu_interface_rrc*       gtpu   = nullptr;
  s1ap_interface_rrc*       s1ap   = nullptr;
  srslte::log_ref           rrc_log;

  // derived params
  std::unique_ptr<cell_info_common_list> cell_common_list;

  // state
  std::unique_ptr<freq_res_common_list>          pucch_res_list;
  std::map<uint16_t, std::unique_ptr<ue> >       users; // NOTE: has to have fixed addr
  std::map<uint32_t, asn1::rrc::paging_record_s> pending_paging;

  void     process_release_complete(uint16_t rnti);
  void     process_rl_failure(uint16_t rnti);
  void     rem_user(uint16_t rnti);
  uint32_t generate_sibs();
  void     configure_mbsfn_sibs(asn1::rrc::sib_type2_s* sib2, asn1::rrc::sib_type13_r9_s* sib13);

  void config_mac();
  void parse_ul_dcch(uint16_t rnti, uint32_t lcid, srslte::unique_byte_buffer_t pdu);
  void parse_ul_ccch(uint16_t rnti, srslte::unique_byte_buffer_t pdu);
  void configure_security(uint16_t rnti, uint32_t lcid, srslte::as_security_config_t sec_cfg);
  void enable_integrity(uint16_t rnti, uint32_t lcid);
  void enable_encryption(uint16_t rnti, uint32_t lcid);

  srslte::byte_buffer_t byte_buf_paging;

  typedef struct {
    uint16_t                     rnti;
    uint32_t                     lcid;
    srslte::unique_byte_buffer_t pdu;
  } rrc_pdu;

  const static uint32_t LCID_EXIT     = 0xffff0000;
  const static uint32_t LCID_REM_USER = 0xffff0001;
  const static uint32_t LCID_REL_USER = 0xffff0002;
  const static uint32_t LCID_RLF_USER = 0xffff0003;
  const static uint32_t LCID_ACT_USER = 0xffff0004;

  bool                         running         = false;
  static const int             RRC_THREAD_PRIO = 65;
  srslte::block_queue<rrc_pdu> rx_pdu_queue;

  asn1::rrc::mcch_msg_s  mcch;
  bool                   enable_mbms     = false;
  rrc_cfg_t              cfg             = {};
  uint32_t               nof_si_messages = 0;
  asn1::rrc::sib_type7_s sib7;

  class enb_mobility_handler;
  std::unique_ptr<enb_mobility_handler> enb_mobility_cfg;

  void rem_user_thread(uint16_t rnti);

  std::mutex paging_mutex;
};

} // namespace srsenb

#endif // SRSENB_RRC_H
