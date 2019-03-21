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

/******************************************************************************
 * File:        spgw.h
 * Description: Top-level SP-GW class. Creates and links all
 *              interfaces and helpers.
 *****************************************************************************/

#ifndef SRSEPC_SPGW_H
#define SRSEPC_SPGW_H

#include "srslte/asn1/gtpc.h"
#include "srslte/common/buffer_pool.h"
#include "srslte/common/log.h"
#include "srslte/common/log_filter.h"
#include "srslte/common/logger_file.h"
#include "srslte/common/threads.h"
#include <cstddef>

namespace srsepc {

class mme_gtpc;

const uint16_t GTPU_RX_PORT = 2152;

typedef struct {
  std::string gtpu_bind_addr;
  std::string sgi_if_addr;
  std::string sgi_if_name;
} spgw_args_t;

typedef struct spgw_tunnel_ctx {
  uint64_t            imsi;
  in_addr_t           ue_ipv4;
  uint8_t             ebi;
  srslte::gtp_fteid_t up_ctrl_fteid;
  srslte::gtp_fteid_t up_user_fteid;
  srslte::gtp_fteid_t dw_ctrl_fteid;
  srslte::gtp_fteid_t dw_user_fteid;
} spgw_tunnel_ctx_t;

class spgw : public thread
{
  class gtpc;
  class gtpu;

public:
  static spgw* get_instance(void);
  static void  cleanup(void);
  int init(spgw_args_t* args, srslte::log_filter* gtpu_log, srslte::log_filter* gtpc_log, srslte::log_filter* spgw_log);
  void stop();
  void run_thread();

  void handle_sgi_pdu(srslte::byte_buffer_t* msg);
  void handle_s1u_pdu(srslte::byte_buffer_t* msg);
  void handle_s11_pdu(srslte::gtpc_pdu* pdu, srslte::gtpc_pdu* reply_pdu);

private:
  spgw();
  virtual ~spgw();
  static spgw* m_instance;

  srslte::error_t init_sgi_if(spgw_args_t* args);
  srslte::error_t init_s1u(spgw_args_t* args);
  srslte::error_t init_ue_ip(spgw_args_t* args);

  uint64_t  get_new_ctrl_teid();
  uint64_t  get_new_user_teid();
  in_addr_t get_new_ue_ipv4();

  spgw_tunnel_ctx_t* create_gtp_ctx(struct srslte::gtpc_create_session_request* cs_req);
  bool               delete_gtp_ctx(uint32_t ctrl_teid);

  bool                      m_running;
  srslte::byte_buffer_pool* m_pool;
  mme_gtpc*                 m_mme_gtpc;

  // data-plane/control-plane mutex
  pthread_mutex_t m_mutex;

  // GTP-C and GTP-U handlers
  gtpc* m_gtpc;
  gtpu* m_gtpu;

  // Logs
  srslte::log_filter* m_spgw_log;
};

} // namespace srsepc
#endif // SRSEPC_SPGW_H
