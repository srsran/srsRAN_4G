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
  ~rrc_nr();

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

  void    config_phy();
  void    config_mac();
  int32_t generate_sibs();
  int     read_pdu_bcch_bch(const uint32_t tti, srsran::byte_buffer_t& buffer) final;
  int     read_pdu_bcch_dlsch(uint32_t sib_index, srsran::byte_buffer_t& buffer) final;

  /// User manegement
  int  add_user(uint16_t rnti, const sched_nr_ue_cfg_t& uecfg) final;
  void rem_user(uint16_t rnti);
  int  update_user(uint16_t new_rnti, uint16_t old_rnti) final;
  void set_activity_user(uint16_t rnti) final;

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
  int  ue_set_bitrates(uint16_t rnti, const asn1::ngap_nr::ue_aggregate_maximum_bit_rate_s& rates) final;
  int  ue_set_security_cfg_capabilities(uint16_t rnti, const asn1::ngap_nr::ue_security_cap_s& caps) final;
  int  start_security_mode_procedure(uint16_t rnti) final;
  int  establish_rrc_bearer(uint16_t                rnti,
                            uint16_t                pdu_session_id,
                            srsran::const_byte_span nas_pdu,
                            uint32_t                lcid) final;
  int  release_bearers(uint16_t rnti) final;
  void write_dl_info(uint16_t rnti, srsran::unique_byte_buffer_t sdu) final;
  int  set_aggregate_max_bitrate(uint16_t rnti, const asn1::ngap_nr::ue_aggregate_maximum_bit_rate_s& rates) final;
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
  struct cell_ctxt_t {
    asn1::rrc_nr::mib_s                                   mib;
    asn1::rrc_nr::sib1_s                                  sib1;
    asn1::rrc_nr::sys_info_ies_s::sib_type_and_info_l_    sibs;
    srsran::unique_byte_buffer_t                          mib_buffer = nullptr;
    std::vector<srsran::unique_byte_buffer_t>             sib_buffer;
    std::unique_ptr<const asn1::rrc_nr::cell_group_cfg_s> master_cell_group;
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

  /// This gets called by rrc_nr::sgnb_addition_request and WILL NOT TRIGGER the RX MSG3 activity timer
  int add_user(uint16_t rnti, const sched_nr_ue_cfg_t& uecfg, bool start_msg3_timer);

  // Helper to create PDU from RRC message
  template <class T>
  srsran::unique_byte_buffer_t pack_into_pdu(const T& msg)
  {
    // Allocate a new PDU buffer and pack the
    srsran::unique_byte_buffer_t pdu = srsran::make_byte_buffer();
    if (pdu == nullptr) {
      logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
      return nullptr;
    }
    asn1::bit_ref bref(pdu->msg, pdu->get_tailroom());
    if (msg.pack(bref) == asn1::SRSASN_ERROR_ENCODE_FAIL) {
      logger.error("Failed to pack message. Discarding it.");
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
