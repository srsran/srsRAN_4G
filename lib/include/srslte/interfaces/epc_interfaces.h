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

#ifndef SRSLTE_EPC_INTERFACES_H
#define SRSLTE_EPC_INTERFACES_H

#include "srslte/asn1/gtpc_ies.h"
#include "srslte/common/common.h"
#include <netinet/sctp.h>
#include <queue>

namespace srsepc {

class nas;

enum nas_timer_type {
  T_3413,
};

/******************
 * MME Interfaces *
 ******************/
class gtpc_interface_nas // NAS -> GTP-C
{
public:
  virtual bool send_create_session_request(uint64_t imsi)                                                         = 0;
  virtual bool send_modify_bearer_request(uint64_t imsi, uint16_t erab_to_modify, srslte::gtp_fteid_t* enb_fteid) = 0;
  virtual bool send_delete_session_request(uint64_t imsi)                                                         = 0;
  virtual bool send_downlink_data_notification_failure_indication(uint64_t                      imsi,
                                                                  enum srslte::gtpc_cause_value cause)            = 0;
};

class s1ap_interface_gtpc // GTP-C -> S1AP
{
public:
  virtual bool send_initial_context_setup_request(uint64_t imsi, uint16_t erab_to_setup) = 0;
};

class s1ap_interface_nas // NAS -> S1AP
{
public:
  virtual uint32_t allocate_m_tmsi(uint64_t imsi)                                            = 0;
  virtual uint32_t get_next_mme_ue_s1ap_id()                                                 = 0;
  virtual bool     add_nas_ctx_to_imsi_map(nas* nas_ctx)                                     = 0;
  virtual bool     add_nas_ctx_to_mme_ue_s1ap_id_map(nas* nas_ctx)                           = 0;
  virtual bool     add_ue_to_enb_set(int32_t enb_assoc, uint32_t mme_ue_s1ap_id)             = 0;
  virtual bool     release_ue_ecm_ctx(uint32_t mme_ue_s1ap_id)                               = 0;
  virtual bool     delete_ue_ctx(uint64_t imsi)                                              = 0;
  virtual uint64_t find_imsi_from_m_tmsi(uint32_t m_tmsi)                                    = 0;
  virtual nas*     find_nas_ctx_from_imsi(uint64_t imsi)                                     = 0;
  virtual bool     send_initial_context_setup_request(uint64_t imsi, uint16_t erab_to_setup) = 0;
  virtual bool     send_ue_context_release_command(uint32_t mme_ue_s1ap_id)                  = 0;
  virtual bool     send_downlink_nas_transport(uint32_t               enb_ue_s1ap_id,
                                               uint32_t               mme_ue_s1ap_id,
                                               srslte::byte_buffer_t* nas_msg,
                                               struct sctp_sndrcvinfo enb_sri)               = 0;
};

class hss_interface_nas // NAS -> HSS
{
public:
  virtual bool gen_auth_info_answer(uint64_t imsi, uint8_t* k_asme, uint8_t* autn, uint8_t* rand, uint8_t* xres) = 0;
  virtual bool gen_update_loc_answer(uint64_t imsi, uint8_t* qci)                                                = 0;
  virtual bool resync_sqn(uint64_t imsi, uint8_t* auts)                                                          = 0;
};

class mme_interface_nas // NAS -> MME
{
public:
  virtual bool add_nas_timer(int timer_fd, enum nas_timer_type type, uint64_t imsi) = 0;
  virtual bool is_nas_timer_running(enum nas_timer_type type, uint64_t imsi)        = 0;
  virtual bool remove_nas_timer(enum nas_timer_type type, uint64_t imsi)            = 0;
};

class s1ap_interface_mme // MME -> S1AP
{
public:
  virtual bool expire_nas_timer(enum nas_timer_type type, uint64_t imsi) = 0;
};

/*******************
 * SPGW Interfaces *
 *******************/
class gtpu_interface_gtpc // GTP-C -> GTP-U
{
public:
  virtual in_addr_t get_s1u_addr() = 0;

  virtual bool modify_gtpu_tunnel(in_addr_t ue_ipv4, srslte::gtpc_f_teid_ie dw_user_fteid, uint32_t up_ctrl_teid) = 0;
  virtual bool delete_gtpu_tunnel(in_addr_t ue_ipv4)                                                              = 0;
  virtual bool delete_gtpc_tunnel(in_addr_t ue_ipv4)                                                              = 0;
  virtual void send_all_queued_packets(srslte::gtp_fteid_t                 dw_user_fteid,
                                       std::queue<srslte::byte_buffer_t*>& pkt_queue)                             = 0;
};

class gtpc_interface_gtpu // GTP-U -> GTP-C
{
public:
  virtual bool queue_downlink_packet(uint32_t spgw_ctr_teid, srslte::byte_buffer_t* msg) = 0;
  virtual bool send_downlink_data_notification(uint32_t spgw_ctr_teid)                   = 0;
};

} // namespace srsepc
#endif // SRSLTE_EPC_INTERFACES_H
