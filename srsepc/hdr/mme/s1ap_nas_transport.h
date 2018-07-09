/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2017 Software Radio Systems Limited
 *
 * \section LICENSE
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
#ifndef SRSEPC_S1AP_NAS_TRANSPORT_H
#define SRSEPC_S1AP_NAS_TRANSPORT_H

#include "srslte/asn1/liblte_s1ap.h"
#include "srslte/common/buffer_pool.h"
#include "s1ap_common.h"
#include "srslte/asn1/gtpc.h"
#include "srsepc/hdr/hss/hss.h"
#include "mme_gtpc.h"

namespace srsepc{

class s1ap_nas_transport
{
public:

  static s1ap_nas_transport* m_instance;
  static s1ap_nas_transport* get_instance(void);
  static void cleanup(void);
  void init(hss_interface_s1ap * hss_);

  bool handle_initial_ue_message(LIBLTE_S1AP_MESSAGE_INITIALUEMESSAGE_STRUCT *init_ue, struct sctp_sndrcvinfo *enb_sri, srslte::byte_buffer_t *reply_buffer, bool *reply_flag);
  bool handle_uplink_nas_transport(LIBLTE_S1AP_MESSAGE_UPLINKNASTRANSPORT_STRUCT *ul_xport, struct sctp_sndrcvinfo *enb_sri, srslte::byte_buffer_t *reply_buffer, bool *reply_flag);

  bool pack_attach_accept(ue_emm_ctx_t *ue_emm_ctx, ue_ecm_ctx_t *ue_ecm_ctx, LIBLTE_S1AP_E_RABTOBESETUPITEMCTXTSUREQ_STRUCT *erab_ctxt, struct srslte::gtpc_pdn_address_allocation_ie *paa, srslte::byte_buffer_t *nas_buffer);

private:
  s1ap_nas_transport();
  virtual ~s1ap_nas_transport();

  srslte::log *m_s1ap_log;
  srslte::byte_buffer_pool *m_pool;

  s1ap* m_s1ap;
  hss_interface_s1ap*  m_hss;
  mme_gtpc* m_mme_gtpc;

    //Initial UE messages
  bool handle_nas_attach_request( uint32_t enb_ue_s1ap_id,
                                  srslte::byte_buffer_t *nas_msg,
                                  srslte::byte_buffer_t *reply_buffer,
                                  bool* reply_flag,
                                  struct sctp_sndrcvinfo *enb_sri);
  bool handle_nas_imsi_attach_request(uint32_t enb_ue_s1ap_id,
                                      const LIBLTE_MME_ATTACH_REQUEST_MSG_STRUCT &attach_req,
                                      const LIBLTE_MME_PDN_CONNECTIVITY_REQUEST_MSG_STRUCT &pdn_con_req,
                                      srslte::byte_buffer_t *reply_buffer,
                                      bool* reply_flag,
                                      struct sctp_sndrcvinfo *enb_sri);
  bool handle_nas_guti_attach_request(uint32_t enb_ue_s1ap_id,
                                      const LIBLTE_MME_ATTACH_REQUEST_MSG_STRUCT &attach_req,
                                      const LIBLTE_MME_PDN_CONNECTIVITY_REQUEST_MSG_STRUCT &pdn_con_req,
                                      srslte::byte_buffer_t *nas_msg,
                                      srslte::byte_buffer_t *reply_buffer,
                                      bool* reply_flag,
                                      struct sctp_sndrcvinfo *enb_sri);

  bool handle_nas_service_request(uint32_t m_tmsi,
                                  uint32_t enb_ue_s1ap_id,
                                  srslte::byte_buffer_t *nas_msg,
                                  srslte::byte_buffer_t *reply_buffer,
                                  bool* reply_flag,
                                  struct sctp_sndrcvinfo *enb_sri);

  bool handle_nas_detach_request(uint32_t m_tmsi,
                                 uint32_t enb_ue_s1ap_id,
                                 srslte::byte_buffer_t *nas_msg,
                                 srslte::byte_buffer_t *reply_buffer,
                                 bool* reply_flag,
                                 struct sctp_sndrcvinfo *enb_sri);

  bool handle_nas_tracking_area_update_request( uint32_t m_tmsi,
                                                uint32_t enb_ue_s1ap_id,
                                                srslte::byte_buffer_t *nas_msg,
                                                srslte::byte_buffer_t *reply_buffer,
                                                bool* reply_flag,
                                                struct sctp_sndrcvinfo *enb_sri);

  bool handle_nas_authentication_response(srslte::byte_buffer_t *nas_msg, ue_ctx_t *ue_ctx, srslte::byte_buffer_t *reply_buffer, bool* reply_flag);
  bool handle_nas_security_mode_complete(srslte::byte_buffer_t *nas_msg, ue_ctx_t *ue_ctx, srslte::byte_buffer_t *reply_buffer, bool *reply_flag);
  bool handle_nas_attach_complete(srslte::byte_buffer_t *nas_msg, ue_ctx_t *ue_ctx, srslte::byte_buffer_t *reply_buffer, bool *reply_flag);
  bool handle_esm_information_response(srslte::byte_buffer_t *nas_msg, ue_ctx_t* ue_ctx, srslte::byte_buffer_t *reply_msg, bool *reply_flag);
  bool handle_identity_response(srslte::byte_buffer_t *nas_msg, ue_ctx_t* ue_ctx, srslte::byte_buffer_t *reply_msg, bool *reply_flag);
  bool handle_tracking_area_update_request(srslte::byte_buffer_t *nas_msg, ue_ctx_t* ue_ctx, srslte::byte_buffer_t *reply_msg, bool *reply_flag);
  bool handle_authentication_failure(srslte::byte_buffer_t *nas_msg, ue_ctx_t* ue_ctx,  srslte::byte_buffer_t *reply_buffer, bool *reply_flag);
  bool handle_nas_detach_request(srslte::byte_buffer_t *nas_msg, ue_ctx_t* ue_ctx, srslte::byte_buffer_t *reply_msg, bool *reply_flag);

  bool integrity_check(ue_emm_ctx_t *emm_ctx, srslte::byte_buffer_t *pdu);
  bool short_integrity_check(ue_emm_ctx_t *emm_ctx, srslte::byte_buffer_t *pdu);

  bool pack_authentication_request(srslte::byte_buffer_t *reply_msg, uint32_t enb_ue_s1ap_id, uint32_t next_mme_ue_s1ap_id, uint8_t eksi, uint8_t *autn, uint8_t *rand);
  bool pack_authentication_reject(srslte::byte_buffer_t *reply_msg, uint32_t enb_ue_s1ap_id, uint32_t mme_ue_s1ap_id);
  bool unpack_authentication_response(LIBLTE_S1AP_MESSAGE_UPLINKNASTRANSPORT_STRUCT *ul_xport, LIBLTE_MME_AUTHENTICATION_RESPONSE_MSG_STRUCT *auth_resp);

  bool pack_security_mode_command(srslte::byte_buffer_t *reply_msg, ue_emm_ctx_t *ue_emm_ctx, ue_ecm_ctx_t *ue_ecm_ctx);
  bool pack_esm_information_request(srslte::byte_buffer_t *reply_msg, ue_emm_ctx_t *ue_emm_ctx, ue_ecm_ctx_t *ue_ecm_ctx);

  bool pack_identity_request(srslte::byte_buffer_t *reply_msg, uint32_t enb_ue_s1ap_id, uint32_t mme_ue_s1ap_id);

  bool pack_emm_information(ue_ctx_t* ue_ctx, srslte::byte_buffer_t *reply_msg);
  bool pack_service_reject(srslte::byte_buffer_t *reply_msg, uint8_t emm_cause, uint32_t enb_ue_s1ap_id);

  void log_unhandled_attach_request_ies(const LIBLTE_MME_ATTACH_REQUEST_MSG_STRUCT *attach_req);
  void log_unhandled_pdn_con_request_ies(const LIBLTE_MME_PDN_CONNECTIVITY_REQUEST_MSG_STRUCT *pdn_con_req);
  void log_unhandled_initial_ue_message_ies(LIBLTE_S1AP_MESSAGE_INITIALUEMESSAGE_STRUCT *init_ue);
};
} //namespace srsepc
#endif // SRSEPC_S1AP_NAS_TRANSPORT_H
