/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#ifndef SRSENB_RRC_NR_H
#define SRSENB_RRC_NR_H

#include "rrc_config_common.h"
#include "rrc_metrics.h"
#include "srsenb/hdr/stack/enb_stack_base.h"
#include "srsenb/hdr/stack/rrc/nr/rrc_config_nr.h"
#include "srsran/asn1/rrc_nr.h"
#include "srsran/common/block_queue.h"
#include "srsran/common/buffer_pool.h"
#include "srsran/common/common.h"
#include "srsran/common/task_scheduler.h"
#include "srsran/common/threads.h"
#include "srsran/common/timeout.h"
#include "srsran/interfaces/enb_pdcp_interfaces.h"
#include "srsran/interfaces/enb_rlc_interfaces.h"
#include "srsran/interfaces/enb_rrc_interfaces.h"
#include "srsran/interfaces/enb_x2_interfaces.h"
#include "srsran/interfaces/gnb_interfaces.h"
#include "srsran/interfaces/gnb_mac_interfaces.h"
#include "srsran/interfaces/gnb_ngap_interfaces.h"
#include "srsran/interfaces/gnb_rrc_nr_interfaces.h"
#include <map>
#include <queue>

namespace srsenb {

enum class rrc_nr_state_t { RRC_IDLE, RRC_INACTIVE, RRC_CONNECTED };

class rrc_nr final : public rrc_interface_pdcp_nr,
                     public rrc_interface_mac_nr,
                     public rrc_interface_rlc_nr,
                     public rrc_interface_ngap_nr,
                     public rrc_nr_interface_rrc
{
public:
  explicit rrc_nr(srsran::task_sched_handle task_sched_);

  int32_t init(const rrc_nr_cfg_t&         cfg,
               phy_interface_stack_nr*     phy,
               mac_interface_rrc_nr*       mac,
               rlc_interface_rrc*          rlc,
               pdcp_interface_rrc*         pdcp,
               ngap_interface_rrc_nr*      ngap_,
               gtpu_interface_rrc_nr*      gtpu,
               rrc_eutra_interface_rrc_nr* rrc_eutra_);

  void stop();

  void get_metrics(srsenb::rrc_metrics_t& m);

  rrc_nr_cfg_t update_default_cfg(const rrc_nr_cfg_t& rrc_cfg);
  void         config_phy();
  void         config_mac();
  int32_t      generate_sibs();
  int          read_pdu_bcch_bch(const uint32_t tti, srsran::unique_byte_buffer_t& buffer) final;
  int          read_pdu_bcch_dlsch(uint32_t sib_index, srsran::unique_byte_buffer_t& buffer) final;

  /// User manegement
  int  add_user(uint16_t rnti, const sched_nr_ue_cfg_t& uecfg);
  void rem_user(uint16_t rnti);
  int  update_user(uint16_t new_rnti, uint16_t old_rnti);
  void set_activity_user(uint16_t rnti);

  // RLC interface
  // TODO
  void        read_pdu_pcch(uint8_t* payload, uint32_t payload_size) {}
  void        max_retx_attempted(uint16_t rnti) {}
  void        protocol_failure(uint16_t rnti) {}
  const char* get_rb_name(uint32_t lcid) { return "invalid"; }

  // PDCP interface
  void write_pdu(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t pdu) final;
  void notify_pdcp_integrity_error(uint16_t rnti, uint32_t lcid) final;

  // Interface for EUTRA RRC
  void sgnb_addition_request(uint16_t rnti, const sgnb_addition_req_params_t& params);
  void sgnb_reconfiguration_complete(uint16_t rnti, const asn1::dyn_octstring& reconfig_response) final;
  void sgnb_release_request(uint16_t nr_rnti) final;

  // Interfaces for NGAP
  int  ue_set_security_cfg_key(uint16_t rnti, const asn1::fixed_bitstring<256, false, true>& key);
  int  ue_set_bitrates(uint16_t rnti, const asn1::ngap_nr::ue_aggregate_maximum_bit_rate_s& rates);
  int  ue_set_security_cfg_capabilities(uint16_t rnti, const asn1::ngap_nr::ue_security_cap_s& caps);
  int  start_security_mode_procedure(uint16_t rnti);
  int  establish_rrc_bearer(uint16_t rnti, uint16_t pdu_session_id, srsran::const_byte_span nas_pdu, uint32_t lcid);
  int  release_bearers(uint16_t rnti);
  void write_dl_info(uint16_t rnti, srsran::unique_byte_buffer_t sdu);
  int  set_aggregate_max_bitrate(uint16_t rnti, const asn1::ngap_nr::ue_aggregate_maximum_bit_rate_s& rates);
  int  allocate_lcid(uint16_t rnti);

  class ue
  {
  public:
    enum activity_timeout_type_t {
      MSG3_RX_TIMEOUT = 0,   ///< Msg3 has its own timeout to quickly remove fake UEs from random PRACHs
      UE_INACTIVITY_TIMEOUT, ///< (currently unused) UE inactivity timeout (usually bigger than reestablishment timeout)
      MSG5_RX_TIMEOUT,       ///< (currently unused) for receiving RRCConnectionSetupComplete/RRCReestablishmentComplete
      nulltype
    };

    /// @param [in] start_msg3_timer: indicates whether the UE is created as part of a RACH process
    ue(rrc_nr* parent_, uint16_t rnti_, const sched_nr_ue_cfg_t& uecfg, bool start_msg3_timer = true);

    void send_connection_setup();
    void send_dl_ccch(asn1::rrc_nr::dl_ccch_msg_s* dl_dcch_msg);

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

    int  pack_rrc_reconfiguration();
    void deactivate_bearers();

    /// methods to handle activity timer
    std::string to_string(const activity_timeout_type_t& type);
    void        set_activity_timeout(activity_timeout_type_t type);
    void        set_activity(bool enabled = true);
    void        activity_timer_expired(const activity_timeout_type_t type);

  private:
    rrc_nr*  parent = nullptr;
    uint16_t rnti   = SRSRAN_INVALID_RNTI;
    /// for basic DL/UL activity timeout
    srsran::unique_timer activity_timer;

    int pack_rrc_reconfiguration(asn1::dyn_octstring& packed_rrc_reconfig);
    int pack_secondary_cell_group_cfg(asn1::dyn_octstring& packed_secondary_cell_config);

    int pack_secondary_cell_group_rlc_cfg(asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack);
    int pack_secondary_cell_group_mac_cfg(asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack);

    int pack_secondary_cell_group_sp_cell_cfg(asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack);

    int pack_sp_cell_cfg_ded(asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack);

    int pack_sp_cell_cfg_ded_init_dl_bwp(asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack);
    int pack_sp_cell_cfg_ded_init_dl_bwp_pdsch_cfg(asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack);
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

    int add_drb();

    // state
    rrc_nr_state_t state          = rrc_nr_state_t::RRC_IDLE;
    uint8_t        transaction_id = 0;

    // RRC configs for UEs
    asn1::rrc_nr::cell_group_cfg_s   cell_group_cfg;
    asn1::rrc_nr::radio_bearer_cfg_s radio_bearer_cfg;

    // MAC controller
    sched_nr_interface::ue_cfg_t uecfg{};

    const uint32_t drb1_lcid = 4;

    // NSA specific variables
    bool     endc       = false;
    uint16_t eutra_rnti = SRSRAN_INVALID_RNTI;
  };

private:
  static constexpr uint32_t UE_PSCELL_CC_IDX = 0; // first NR cell is always Primary Secondary Cell for UE
  rrc_nr_cfg_t              cfg              = {};

  // interfaces
  phy_interface_stack_nr*     phy       = nullptr;
  mac_interface_rrc_nr*       mac       = nullptr;
  rlc_interface_rrc*          rlc       = nullptr;
  pdcp_interface_rrc*         pdcp      = nullptr;
  gtpu_interface_rrc_nr*      gtpu      = nullptr;
  ngap_interface_rrc_nr*      ngap      = nullptr;
  rrc_eutra_interface_rrc_nr* rrc_eutra = nullptr;

  // args
  srsran::task_sched_handle task_sched;

  // derived
  uint32_t                    slot_dur_ms = 0;
  srslog::basic_logger&       logger;
  asn1::rrc_nr::sp_cell_cfg_s base_sp_cell_cfg;

  // vars
  std::map<uint16_t, std::unique_ptr<ue> >  users;
  bool                                      running = false;
  std::vector<srsran::unique_byte_buffer_t> sib_buffer;
  srsran::unique_byte_buffer_t              mib_buffer = nullptr;

  uint32_t nof_si_messages = 0;

  /// Private Methods
  void handle_pdu(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t pdu);
  /// This gets called by rrc_nr::sgnb_addition_request and WILL NOT TRIGGER the RX MSG3 activity timer
  int add_user(uint16_t rnti, const sched_nr_ue_cfg_t& uecfg, bool start_msg3_timer);

  // logging
  typedef enum { Rx = 0, Tx } direction_t;
  template <class T>
  void log_rrc_message(const std::string& source, direction_t dir, const srsran::byte_buffer_t* pdu, const T& msg);
};

} // namespace srsenb

#endif // SRSENB_RRC_NR_H
