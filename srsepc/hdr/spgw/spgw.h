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

#ifndef SPGW_H
#define SPGW_H

#include <cstddef>
#include "srslte/common/log.h"
#include "srslte/common/logger_file.h"
#include "srslte/common/log_filter.h"
#include "srslte/common/buffer_pool.h"
#include "srslte/common/threads.h"
#include "srslte/asn1/gtpc.h"

namespace srsepc{

class mme_gtpc;

const uint16_t GTPU_RX_PORT = 2152;

typedef struct {
  std::string gtpu_bind_addr;
  std::string sgi_if_addr;
} spgw_args_t;


typedef struct {
  uint64_t imsi;
  in_addr_t ue_ipv4;
  struct srslte::gtpc_f_teid_ie up_ctrl_fteid;
  struct srslte::gtpc_f_teid_ie up_user_fteid;
  struct srslte::gtpc_f_teid_ie dw_ctrl_fteid;
  struct srslte::gtpc_f_teid_ie dw_user_fteid;
} spgw_ue_ctx;

class spgw:
  public thread
{
public:
  static spgw* get_instance(void);
  static void cleanup(void);
  int init(spgw_args_t* args, srslte::log_filter *spgw_log);
  void stop();
  void run_thread();

  void handle_create_session_request(struct srslte::gtpc_create_session_request *cs_req, struct srslte::gtpc_pdu *cs_resp_pdu);

private:

  spgw();
  virtual ~spgw();
  static spgw *m_instance;

  srslte::error_t init_sgi_if(spgw_args_t *args);
  srslte::error_t init_s1u(spgw_args_t *args);
  uint64_t get_new_ctrl_teid();
  uint64_t get_new_user_teid();
  in_addr_t get_new_ue_ipv4();

  bool m_running;
  srslte::byte_buffer_pool *m_pool;
  mme_gtpc *m_mme_gtpc;


  bool m_sgi_up;
  int m_sgi_if;
  int m_sgi_sock;

  bool m_s1u_up;
  int m_s1u;

  uint64_t m_next_ctrl_teid;
  uint64_t m_next_user_teid;

  std::map<uint32_t,spgw_ue_ctx*> m_teid_to_ue_ctx;      //Map control TEID to ue_ctx. Usefull to get reply ctrl TEID
  std::map<in_addr_t,gtpc_f_teid_ie> m_ip_to_teid;          //Map IP to User-plane TEID for downlink traffic

  /*Logs*/
  srslte::log_filter  *m_spgw_log;

};

} // namespace srsepc

#endif // SGW_H
