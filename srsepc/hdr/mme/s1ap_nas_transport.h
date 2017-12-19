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
#ifndef S1AP_NAS_TRANSPORT_H
#define S1AP_NAS_TRANSPORT_H

#include "srslte/asn1/liblte_s1ap.h"
#include "srslte/common/buffer_pool.h"
#include "mme/s1ap_common.h"
#include "srslte/asn1/gtpc.h"

namespace srsepc{

class s1ap_nas_transport
{
public:

  static s1ap_nas_transport* m_instance;
  static s1ap_nas_transport* get_instance(void);
  static void cleanup(void);
  void init(void);

  void set_log(srslte::log *s1ap_logger);
  bool unpack_initial_ue_message(LIBLTE_S1AP_MESSAGE_INITIALUEMESSAGE_STRUCT *init_ue, LIBLTE_MME_ATTACH_REQUEST_MSG_STRUCT *attach_req, LIBLTE_MME_PDN_CONNECTIVITY_REQUEST_MSG_STRUCT *pdn_con_req);
  bool pack_authentication_request(srslte::byte_buffer_t *reply_msg, uint32_t enb_ue_s1ap_id, uint32_t next_mme_ue_s1ap_id, uint8_t *autn,uint8_t *rand);
  bool pack_authentication_reject(srslte::byte_buffer_t *reply_msg, uint32_t enb_ue_s1ap_id, uint32_t mme_ue_s1ap_id);

  bool unpack_authentication_response(LIBLTE_S1AP_MESSAGE_UPLINKNASTRANSPORT_STRUCT *ul_xport, LIBLTE_MME_AUTHENTICATION_RESPONSE_MSG_STRUCT *auth_resp);
  bool pack_security_mode_command(srslte::byte_buffer_t *reply_msg, ue_ctx_t *ue_ctx);
  bool pack_attach_accept(ue_ctx_t *ue_ctx, LIBLTE_S1AP_E_RABTOBESETUPITEMCTXTSUREQ_STRUCT *erab_ctxt, struct srslte::gtpc_pdn_address_allocation_ie *paa, srslte::byte_buffer_t *nas_buffer);

  void log_unhandled_attach_request_ies(const LIBLTE_MME_ATTACH_REQUEST_MSG_STRUCT *attach_req);
  void log_unhandled_pdn_con_request_ies(const LIBLTE_MME_PDN_CONNECTIVITY_REQUEST_MSG_STRUCT *pdn_con_req);
  void log_unhandled_initial_ue_message_ies(LIBLTE_S1AP_MESSAGE_INITIALUEMESSAGE_STRUCT *init_ue);


private:
  s1ap_nas_transport();
  virtual ~s1ap_nas_transport();

  srslte::log *m_s1ap_log;
  srslte::byte_buffer_pool *m_pool;

  s1ap* m_parent;

};

} //namespace srsepc

#endif //S1AP_NAS_TRANSPORT
