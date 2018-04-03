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


typedef struct spgw_tunnel_ctx {
  uint64_t imsi;
  in_addr_t ue_ipv4;
  uint8_t ebi;
  struct srslte::gtpc_f_teid_ie up_ctrl_fteid;
  struct srslte::gtpc_f_teid_ie up_user_fteid;
  struct srslte::gtpc_f_teid_ie dw_ctrl_fteid;
  struct srslte::gtpc_f_teid_ie dw_user_fteid;
} spgw_tunnel_ctx_t;

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
  void handle_modify_bearer_request(struct srslte::gtpc_pdu *mb_req_pdu, struct srslte::gtpc_pdu *mb_resp_pdu);
  void handle_delete_session_request(struct srslte::gtpc_pdu *del_req_pdu, struct srslte::gtpc_pdu *del_resp_pdu);
  void handle_release_access_bearers_request(struct srslte::gtpc_pdu *rel_req_pdu, struct srslte::gtpc_pdu *rel_resp_pdu);

  void handle_sgi_pdu(srslte::byte_buffer_t *msg);
  void handle_s1u_pdu(srslte::byte_buffer_t *msg);

private:

  spgw();
  virtual ~spgw();
  static spgw *m_instance;

  srslte::error_t init_sgi_if(spgw_args_t *args);
  srslte::error_t init_s1u(spgw_args_t *args);
  srslte::error_t init_ue_ip(spgw_args_t *args);

  uint64_t get_new_ctrl_teid();
  uint64_t get_new_user_teid();
  in_addr_t get_new_ue_ipv4();

  spgw_tunnel_ctx_t* create_gtp_ctx(struct srslte::gtpc_create_session_request *cs_req);
  bool delete_gtp_ctx(uint32_t ctrl_teid);


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

  sockaddr_in m_s1u_addr;

  pthread_mutex_t m_mutex;

  std::map<uint64_t,uint32_t> m_imsi_to_ctr_teid;                   //IMSI to control TEID map. Important to check if UE is previously connected
  std::map<uint32_t,spgw_tunnel_ctx*> m_teid_to_tunnel_ctx;         //Map control TEID to tunnel ctx. Usefull to get reply ctrl TEID, UE IP, etc.
  std::map<in_addr_t,srslte::gtpc_f_teid_ie> m_ip_to_teid;          //Map IP to User-plane TEID for downlink traffic

  uint32_t m_h_next_ue_ip;

  /*Time*/
  struct timeval m_t_last_dl;
  struct timeval m_t_last_ul;

  /*Logs*/
  srslte::log_filter  *m_spgw_log;

};

} // namespace srsepc

#endif // SRSEPC_SPGW_H
