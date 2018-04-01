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
#ifndef SRSEPC_S1AP_CTX_MNGMT_PROC_H
#define SRSEPC_S1AP_CTX_MNGMT_PROC_H

#include "srslte/asn1/liblte_s1ap.h"
#include "srslte/common/common.h"
#include "s1ap_common.h"
#include "srslte/common/log_filter.h"
#include "mme_gtpc.h"
#include "srslte/common/buffer_pool.h"

namespace srsepc{

class s1ap;

class s1ap_ctx_mngmt_proc
{
public:

  static s1ap_ctx_mngmt_proc *m_instance;
  static s1ap_ctx_mngmt_proc* get_instance(void);
  static void cleanup(void);

  void init(void);

  //bool send_initial_context_setup_request(uint32_t mme_ue_s1ap_id, struct srslte::gtpc_create_session_response *cs_resp, struct srslte::gtpc_f_teid_ie sgw_ctrl_fteid);
  bool send_initial_context_setup_request(ue_emm_ctx_t *emm_ctx, ue_ecm_ctx_t *ecm_ctx, erab_ctx_t *erab_ctx);
  bool handle_initial_context_setup_response(LIBLTE_S1AP_MESSAGE_INITIALCONTEXTSETUPRESPONSE_STRUCT *in_ctxt_resp);
  bool handle_ue_context_release_request(LIBLTE_S1AP_MESSAGE_UECONTEXTRELEASEREQUEST_STRUCT *ue_rel, struct sctp_sndrcvinfo *enb_sri, srslte::byte_buffer_t *reply_buffer, bool *reply_flag);
  bool send_ue_context_release_command(ue_ecm_ctx_t *ecm_ctx, srslte::byte_buffer_t *reply_buffer);
  bool handle_ue_context_release_complete(LIBLTE_S1AP_MESSAGE_UECONTEXTRELEASECOMPLETE_STRUCT *rel_comp);

private:
  s1ap_ctx_mngmt_proc();
  virtual ~s1ap_ctx_mngmt_proc();

  s1ap* m_s1ap;
  s1ap_nas_transport* m_s1ap_nas_transport;
  srslte::log_filter *m_s1ap_log;

  s1ap_args_t m_s1ap_args;

  mme_gtpc* m_mme_gtpc;
  srslte::byte_buffer_pool      *m_pool;
};

} //namespace srsepc

#endif // SRSEPC_S1AP_CTX_MNGMT_PROC_H
