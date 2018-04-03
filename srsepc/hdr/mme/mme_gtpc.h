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
#ifndef SRSEPC_MME_GTPC_H
#define SRSEPC_MME_GTPC_H

#include "srslte/common/log.h"
#include "srslte/common/log_filter.h"
#include "srslte/common/buffer_pool.h"
#include "srslte/asn1/gtpc.h"
#include "s1ap_common.h"
namespace srsepc
{

class spgw;
class s1ap;

class mme_gtpc
{
public:

  typedef struct gtpc_ctx{
    srslte::gtp_fteid_t mme_ctr_fteid;
    srslte::gtp_fteid_t sgw_ctr_fteid;
  }gtpc_ctx_t;
  static mme_gtpc* get_instance(void);
  static void cleanup(void);

  bool init(srslte::log_filter *mme_gtpc_log);

  uint32_t get_new_ctrl_teid();
  void send_create_session_request(uint64_t imsi);
  void handle_create_session_response(srslte::gtpc_pdu *cs_resp_pdu);
  void send_modify_bearer_request(uint64_t imsi, erab_ctx_t *bearer_ctx);
  void handle_modify_bearer_response(srslte::gtpc_pdu *mb_resp_pdu);
  void send_release_access_bearers_request(uint64_t imsi);
  void send_delete_session_request(uint64_t imsi);

private:

  mme_gtpc();
  virtual ~mme_gtpc();
  static mme_gtpc *m_instance;

  srslte::log_filter *m_mme_gtpc_log;
  srslte::byte_buffer_pool *m_pool;

  s1ap* m_s1ap;
  spgw* m_spgw;
  in_addr_t m_mme_gtpc_ip;

  uint32_t m_next_ctrl_teid;
  std::map<uint32_t,uint64_t> m_mme_ctr_teid_to_imsi;
  std::map<uint64_t,struct gtpc_ctx> m_imsi_to_gtpc_ctx;

};

}
#endif // SRSEPC_MME_GTPC_H
