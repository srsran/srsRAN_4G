/**
 * Copyright 2013-2022 Software Radio Systems Limited
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
#ifndef SRSEPC_S1AP_CTX_MNGMT_PROC_H
#define SRSEPC_S1AP_CTX_MNGMT_PROC_H

#include "mme_gtpc.h"
#include "s1ap_common.h"
#include "srsran/asn1/s1ap.h"
#include "srsran/common/buffer_pool.h"
#include "srsran/common/common.h"

namespace srsepc {

class s1ap;

class s1ap_ctx_mngmt_proc
{
public:
  static s1ap_ctx_mngmt_proc* m_instance;
  static s1ap_ctx_mngmt_proc* get_instance(void);
  static void                 cleanup(void);

  void init(void);

  bool send_initial_context_setup_request(nas* nas_ctx, uint16_t erab_to_setup);
  bool handle_initial_context_setup_response(const asn1::s1ap::init_context_setup_resp_s& in_ctxt_resp);
  bool handle_ue_context_release_request(const asn1::s1ap::ue_context_release_request_s& ue_rel,
                                         struct sctp_sndrcvinfo*                         enb_sri);
  bool send_ue_context_release_command(nas* nas_ctx);
  bool handle_ue_context_release_complete(const asn1::s1ap::ue_context_release_complete_s& rel_comp);

private:
  s1ap_ctx_mngmt_proc();
  virtual ~s1ap_ctx_mngmt_proc();

  s1ap*                 m_s1ap;
  srslog::basic_logger& m_logger = srslog::fetch_basic_logger("S1AP");

  s1ap_args_t m_s1ap_args;

  mme_gtpc* m_mme_gtpc;
};

} // namespace srsepc
#endif // SRSEPC_S1AP_CTX_MNGMT_PROC_H
