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

#ifndef SRSENB_RRC_H
#define SRSENB_RRC_H

#include "rrc_bearer_cfg.h"
#include "rrc_cell_cfg.h"
#include "rrc_metrics.h"
#include "srsenb/hdr/common/common_enb.h"
#include "srsenb/hdr/common/rnti_pool.h"
#include "srsran/adt/circular_buffer.h"
#include "srsran/common/bearer_manager.h"
#include "srsran/common/buffer_pool.h"
#include "srsran/common/common.h"
#include "srsran/common/stack_procedure.h"
#include "srsran/common/task_scheduler.h"
#include "srsran/common/timeout.h"
#include "srsran/interfaces/enb_rrc_interface_mac.h"
#include "srsran/interfaces/enb_rrc_interface_pdcp.h"
#include "srsran/interfaces/enb_rrc_interface_rlc.h"
#include "srsran/interfaces/enb_rrc_interface_s1ap.h"
#include "srsran/interfaces/enb_x2_interfaces.h"
#include "srsran/srslog/srslog.h"
#include <map>

namespace srsenb {

class s1ap_interface_rrc;
class pdcp_interface_rrc;
class rlc_interface_rrc;
class mac_interface_rrc;
class phy_interface_rrc_lte;

class paging_manager;

class rrc final : public rrc_interface_pdcp,
                  public rrc_interface_mac,
                  public rrc_interface_rlc,
                  public rrc_interface_s1ap,
                  public rrc_eutra_interface_rrc_nr
{
public:
  explicit rrc(srsran::task_sched_handle task_sched_, enb_bearer_manager& manager_);
  ~rrc();

  int32_t init(const rrc_cfg_t&       cfg_,
               phy_interface_rrc_lte* phy,
               mac_interface_rrc*     mac,
               rlc_interface_rrc*     rlc,
               pdcp_interface_rrc*    pdcp,
               s1ap_interface_rrc*    s1ap,
               gtpu_interface_rrc*    gtpu);

  int32_t init(const rrc_cfg_t&       cfg_,
               phy_interface_rrc_lte* phy,
               mac_interface_rrc*     mac,
               rlc_interface_rrc*     rlc,
               pdcp_interface_rrc*    pdcp,
               s1ap_interface_rrc*    s1ap,
               gtpu_interface_rrc*    gtpu,
               rrc_nr_interface_rrc*  rrc_nr);

  void stop();
  void get_metrics(rrc_metrics_t& m);
  void tti_clock();

  // rrc_interface_mac
  int      add_user(uint16_t rnti, const sched_interface::ue_cfg_t& init_ue_cfg) override;
  void     upd_user(uint16_t new_rnti, uint16_t old_rnti) override;
  void     set_activity_user(uint16_t rnti) override;
  void     set_radiolink_dl_state(uint16_t rnti, bool crc_res) override;
  void     set_radiolink_ul_state(uint16_t rnti, bool crc_res) override;
  bool     is_paging_opportunity(uint32_t tti, uint32_t* payload_len) override;
  uint8_t* read_pdu_bcch_dlsch(const uint8_t cc_idx, const uint32_t sib_index) override;
  uint8_t*
  read_pdu_bcch_dlsch(const uint8_t cc_idx, const uint32_t sib_index, const uint32_t sib_segment_index) override;

  // rrc_interface_rlc
  void read_pdu_pcch(uint32_t tti_tx_dl, uint8_t* payload, uint32_t buffer_size) override;
  void max_retx_attempted(uint16_t rnti) override;
  void protocol_failure(uint16_t rnti) override;

  // rrc_interface_s1ap
  void     write_dl_info(uint16_t rnti, srsran::unique_byte_buffer_t sdu) override;
  void     release_ue(uint16_t rnti) override;
  bool     setup_ue_ctxt(uint16_t rnti, const asn1::s1ap::init_context_setup_request_s& msg) override;
  bool     modify_ue_ctxt(uint16_t rnti, const asn1::s1ap::ue_context_mod_request_s& msg) override;
  bool     has_erab(uint16_t rnti, uint32_t erab_id) const override;
  int      get_erab_addr_in(uint16_t rnti, uint16_t erab_id, transp_addr_t& addr_in, uint32_t& teid_in) const override;
  void     set_aggregate_max_bitrate(uint16_t rnti, const asn1::s1ap::ue_aggregate_maximum_bitrate_s& bitrate) override;
  int      setup_erab(uint16_t                                           rnti,
                      uint16_t                                           erab_id,
                      const asn1::s1ap::erab_level_qos_params_s&         qos_params,
                      srsran::const_span<uint8_t>                        nas_pdu,
                      const asn1::bounded_bitstring<1, 160, true, true>& addr,
                      uint32_t                                           gtpu_teid_out,
                      asn1::s1ap::cause_c&                               cause) override;
  int      modify_erab(uint16_t                                   rnti,
                       uint16_t                                   erab_id,
                       const asn1::s1ap::erab_level_qos_params_s& qos_params,
                       srsran::const_span<uint8_t>                nas_pdu,
                       asn1::s1ap::cause_c&                       cause) override;
  bool     release_erabs(uint32_t rnti) override;
  int      release_erab(uint16_t rnti, uint16_t erab_id) override;
  void     add_paging_id(uint32_t ueid, const asn1::s1ap::ue_paging_id_c& ue_paging_id) override;
  void     ho_preparation_complete(uint16_t                     rnti,
                                   rrc::ho_prep_result          result,
                                   const asn1::s1ap::ho_cmd_s&  msg,
                                   srsran::unique_byte_buffer_t rrc_container) override;
  uint16_t start_ho_ue_resource_alloc(const asn1::s1ap::ho_request_s&                                   msg,
                                      const asn1::s1ap::sourceenb_to_targetenb_transparent_container_s& container,
                                      asn1::s1ap::cause_c& failure_cause) override;
  void     set_erab_status(uint16_t rnti, const asn1::s1ap::bearers_subject_to_status_transfer_list_l& erabs) override;

  int notify_ue_erab_updates(uint16_t rnti, srsran::const_byte_span nas_pdu) override;

  // rrc_eutra_interface_rrc_nr
  void sgnb_addition_ack(uint16_t eutra_rnti, const sgnb_addition_ack_params_t params) override;
  void sgnb_addition_reject(uint16_t eutra_rnti) override;
  void sgnb_addition_complete(uint16_t eutra_rnti, uint16_t nr_rnti) override;
  void sgnb_inactivity_timeout(uint16_t eutra_rnti) override;
  void sgnb_release_ack(uint16_t eutra_rnti) override;

  // rrc_interface_pdcp
  void write_pdu(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t pdu) override;
  void notify_pdcp_integrity_error(uint16_t rnti, uint32_t lcid) override;

  uint32_t get_nof_users();

  // logging
  enum direction_t { Rx = 0, Tx, toS1AP, fromS1AP };
  template <class T>
  void log_rrc_message(const direction_t       dir,
                       uint16_t                rnti,
                       uint32_t                lcid,
                       srsran::const_byte_span pdu,
                       const T&                msg,
                       const char*             msg_type)
  {
    log_rxtx_pdu_impl(dir, rnti, lcid, pdu, msg_type);
    if (logger.debug.enabled()) {
      asn1::json_writer json_writer;
      msg.to_json(json_writer);
      logger.debug("Content:\n%s", json_writer.to_string().c_str());
    }
  }
  template <class T>
  void log_broadcast_rrc_message(uint16_t rnti, srsran::const_byte_span pdu, const T& msg, const char* msg_type)
  {
    log_rrc_message(Tx, rnti, -1, pdu, msg, msg_type);
  }

  class ue;

private:
  // args
  srsran::task_sched_handle task_sched;
  enb_bearer_manager&       bearer_manager;
  phy_interface_rrc_lte*    phy    = nullptr;
  mac_interface_rrc*        mac    = nullptr;
  rlc_interface_rrc*        rlc    = nullptr;
  pdcp_interface_rrc*       pdcp   = nullptr;
  gtpu_interface_rrc*       gtpu   = nullptr;
  s1ap_interface_rrc*       s1ap   = nullptr;
  rrc_nr_interface_rrc*     rrc_nr = nullptr;
  srslog::basic_logger&     logger;

  // derived params
  std::unique_ptr<enb_cell_common_list> cell_common_list;

  // state
  std::unique_ptr<freq_res_common_list>    cell_res_list;
  std::map<uint16_t, unique_rnti_ptr<ue> > users; // NOTE: has to have fixed addr
  std::unique_ptr<paging_manager>          pending_paging;

  void     process_release_complete(uint16_t rnti);
  void     rem_user(uint16_t rnti);
  uint32_t generate_sibs();
  void     configure_mbsfn_sibs();
  int      pack_mcch();

  void config_mac();
  void parse_ul_dcch(ue& ue, uint32_t lcid, srsran::unique_byte_buffer_t pdu);
  void parse_ul_ccch(ue& ue, srsran::unique_byte_buffer_t pdu);
  void send_rrc_connection_reject(uint16_t rnti);

  const static int mcch_payload_len                      = 3000;
  int              current_mcch_length                   = 0;
  uint8_t          mcch_payload_buffer[mcch_payload_len] = {};
  struct rrc_pdu {
    uint16_t                     rnti;
    uint32_t                     lcid;
    uint32_t                     arg;
    srsran::unique_byte_buffer_t pdu;
  };
  void log_rx_pdu_fail(uint16_t rnti, uint32_t lcid, srsran::const_byte_span pdu, const char* cause);
  void
  log_rxtx_pdu_impl(direction_t dir, uint16_t rnti, uint32_t lcid, srsran::const_byte_span pdu, const char* msg_type);

  const static uint32_t LCID_EXIT       = 0xffff0000;
  const static uint32_t LCID_REM_USER   = 0xffff0001;
  const static uint32_t LCID_REL_USER   = 0xffff0002;
  const static uint32_t LCID_ACT_USER   = 0xffff0004;
  const static uint32_t LCID_RLC_RTX    = 0xffff0005;
  const static uint32_t LCID_RADLINK_DL = 0xffff0006;
  const static uint32_t LCID_RADLINK_UL = 0xffff0007;
  const static uint32_t LCID_PROT_FAIL  = 0xffff0008;

  bool                                running = false;
  srsran::dyn_blocking_queue<rrc_pdu> rx_pdu_queue;

  asn1::rrc::mcch_msg_s  mcch;
  bool                   enable_mbms     = false;
  rrc_cfg_t              cfg             = {};
  uint32_t               nof_si_messages = 0;
  asn1::rrc::sib_type7_s sib7;

  void rem_user_thread(uint16_t rnti);
};

} // namespace srsenb

#endif // SRSENB_RRC_H
