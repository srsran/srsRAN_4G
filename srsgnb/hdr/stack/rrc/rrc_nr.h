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

#ifndef SRSENB_RRC_NR_H
#define SRSENB_RRC_NR_H

#include "srsenb/hdr/stack/enb_stack_base.h"
#include "srsenb/hdr/stack/rrc/rrc_config_common.h"
#include "srsenb/hdr/stack/rrc/rrc_metrics.h"
#include "srsgnb/hdr/stack/rrc/rrc_nr_config.h"
#include "srsran/asn1/rrc_nr.h"
#include "srsran/common/block_queue.h"
#include "srsran/common/buffer_pool.h"
#include "srsran/common/common.h"
#include "srsran/common/task_scheduler.h"
#include "srsran/common/threads.h"
#include "srsran/common/timeout.h"
#include "srsran/interfaces/enb_pdcp_interfaces.h"
#include "srsran/interfaces/enb_rlc_interfaces.h"
#include "srsran/interfaces/enb_x2_interfaces.h"
#include "srsran/interfaces/gnb_interfaces.h"
#include "srsran/interfaces/gnb_mac_interfaces.h"
#include "srsran/interfaces/gnb_ngap_interfaces.h"
#include "srsran/interfaces/gnb_rrc_nr_interfaces.h"
#include <map>
#include <queue>

namespace srsenb {

class enb_bearer_manager;
class du_config_manager;

enum class rrc_nr_state_t { RRC_IDLE, RRC_INACTIVE, RRC_CONNECTED };

class rrc_nr final : public rrc_interface_pdcp_nr,
                     public rrc_interface_mac_nr,
                     public rrc_interface_rlc_nr,
                     public rrc_interface_ngap_nr,
                     public rrc_nr_interface_rrc
{
public:
  explicit rrc_nr(srsran::task_sched_handle task_sched_);
  ~rrc_nr();

  int32_t init(const rrc_nr_cfg_t&         cfg,
               phy_interface_stack_nr*     phy,
               mac_interface_rrc_nr*       mac,
               rlc_interface_rrc*          rlc,
               pdcp_interface_rrc*         pdcp,
               ngap_interface_rrc_nr*      ngap_,
               gtpu_interface_rrc*         gtpu_,
               enb_bearer_manager&         bearer_mapper_,
               rrc_eutra_interface_rrc_nr* rrc_eutra_);

  void stop();

  void get_metrics(srsenb::rrc_metrics_t& m);

  void    config_phy();
  void    config_mac();
  int32_t generate_sibs();
  int     read_pdu_bcch_bch(const uint32_t tti, srsran::byte_buffer_t& buffer) final;
  int     read_pdu_bcch_dlsch(uint32_t sib_index, srsran::byte_buffer_t& buffer) final;

  /// User management
  int  add_user(uint16_t rnti, uint32_t pcell_cc_idx) final;
  void rem_user(uint16_t rnti);
  int  update_user(uint16_t new_rnti, uint16_t old_rnti) final;
  void set_activity_user(uint16_t rnti) final;
  int  rrc_release(uint16_t rnti);

  // RLC interface
  // TODO
  void        read_pdu_pcch(uint8_t* payload, uint32_t payload_size) final {}
  void        max_retx_attempted(uint16_t rnti) final {}
  void        protocol_failure(uint16_t rnti) final {}
  const char* get_rb_name(uint32_t lcid) final { return "invalid"; }

  // PDCP interface
  void write_pdu(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t pdu) final;
  void notify_pdcp_integrity_error(uint16_t rnti, uint32_t lcid) final;

  // Interface for EUTRA RRC
  void sgnb_addition_request(uint16_t rnti, const sgnb_addition_req_params_t& params) final;
  void sgnb_reconfiguration_complete(uint16_t rnti, const asn1::dyn_octstring& reconfig_response) final;
  void sgnb_release_request(uint16_t nr_rnti) final;

  // Interfaces for NGAP
  int  ue_set_security_cfg_key(uint16_t rnti, const asn1::fixed_bitstring<256, false, true>& key) final;
  int  ue_set_bitrates(uint16_t rnti, const asn1::ngap::ue_aggregate_maximum_bit_rate_s& rates) final;
  int  ue_set_security_cfg_capabilities(uint16_t rnti, const asn1::ngap::ue_security_cap_s& caps) final;
  int  start_security_mode_procedure(uint16_t rnti, srsran::unique_byte_buffer_t nas_pdu) final;
  int  establish_rrc_bearer(uint16_t                rnti,
                            uint16_t                pdu_session_id,
                            srsran::const_byte_span nas_pdu,
                            uint32_t                lcid,
                            uint32_t                five_qi) final;
  int  release_bearers(uint16_t rnti) final;
  void release_user(uint16_t rnti) final;
  void write_dl_info(uint16_t rnti, srsran::unique_byte_buffer_t sdu) final;
  int  set_aggregate_max_bitrate(uint16_t rnti, const asn1::ngap::ue_aggregate_maximum_bit_rate_s& rates) final;
  int  allocate_lcid(uint16_t rnti) final;

  // logging
  typedef enum { Rx = 0, Tx } direction_t;
  template <class T>
  void log_rrc_message(const char*             source,
                       const direction_t       dir,
                       srsran::const_byte_span pdu,
                       const T&                msg,
                       const char*             msg_type);

  class ue;

private:
  static constexpr uint32_t UE_PSCELL_CC_IDX = 0; // first NR cell is always Primary Secondary Cell for UE
  rrc_nr_cfg_t              cfg              = {};

  // interfaces
  phy_interface_stack_nr*     phy           = nullptr;
  mac_interface_rrc_nr*       mac           = nullptr;
  rlc_interface_rrc*          rlc           = nullptr;
  pdcp_interface_rrc*         pdcp          = nullptr;
  ngap_interface_rrc_nr*      ngap          = nullptr;
  gtpu_interface_rrc*         gtpu          = nullptr;
  rrc_eutra_interface_rrc_nr* rrc_eutra     = nullptr;
  enb_bearer_manager*         bearer_mapper = nullptr;

  // args
  srsran::task_sched_handle task_sched;

  // derived
  uint32_t              slot_dur_ms = 0;
  srslog::basic_logger& logger;

  // vars
  std::unique_ptr<du_config_manager> du_cfg;
  struct cell_ctxt_t {
    asn1::rrc_nr::sys_info_ies_s::sib_type_and_info_l_    sibs;
    std::vector<srsran::unique_byte_buffer_t>             sib_buffer;
    std::unique_ptr<const asn1::rrc_nr::cell_group_cfg_s> master_cell_group;
    srsran::phy_cfg_nr_t                                  default_phy_ue_cfg_nr;
  };
  std::unique_ptr<cell_ctxt_t>     cell_ctxt;
  rnti_map_t<std::unique_ptr<ue> > users;
  bool                             running = false;

  /// Private Methods
  void handle_pdu(uint16_t rnti, uint32_t lcid, srsran::const_byte_span pdu);
  void handle_ul_ccch(uint16_t rnti, srsran::const_byte_span pdu);
  void handle_ul_dcch(uint16_t rnti, uint32_t lcid, srsran::const_byte_span pdu);

  // TS 38.331, 5.3.3 - RRC connection establishment
  void handle_rrc_setup_request(uint16_t rnti, const asn1::rrc_nr::rrc_setup_request_s& msg);

  // TS 38.331, 5.3.7 RRC connection reestablishment
  void handle_rrc_reest_request(uint16_t rnti, const asn1::rrc_nr::rrc_reest_request_s& msg);

  /// This gets called by rrc_nr::sgnb_addition_request and WILL NOT TRIGGER the RX MSG3 activity timer
  int add_user(uint16_t rnti, uint32_t pcell_cc_idx, bool start_msg3_timer);

  // Helper to create PDU from RRC message
  template <class T>
  srsran::unique_byte_buffer_t pack_into_pdu(const T& msg, const char* context_name = nullptr)
  {
    context_name = context_name == nullptr ? __FUNCTION__ : context_name;
    // Allocate a new PDU buffer and pack the
    srsran::unique_byte_buffer_t pdu = srsran::make_byte_buffer();
    if (pdu == nullptr) {
      logger.error("Couldn't allocate PDU in %s.", context_name);
      return nullptr;
    }
    asn1::bit_ref bref(pdu->msg, pdu->get_tailroom());
    if (msg.pack(bref) == asn1::SRSASN_ERROR_ENCODE_FAIL) {
      logger.error("Failed to pack message in %s. Discarding it.", context_name);
      return nullptr;
    }
    pdu->N_bytes = bref.distance_bytes();
    return pdu;
  }
  void log_rx_pdu_fail(uint16_t                rnti,
                       uint32_t                lcid,
                       srsran::const_byte_span pdu,
                       const char*             cause_str,
                       bool                    log_hex = true);
};

} // namespace srsenb

#endif // SRSENB_RRC_NR_H
