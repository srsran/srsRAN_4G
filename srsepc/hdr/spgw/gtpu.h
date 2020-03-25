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

#ifndef SRSEPC_GTPU_H
#define SRSEPC_GTPU_H

#include "srsepc/hdr/spgw/spgw.h"
#include "srslte/asn1/gtpc.h"
#include "srslte/common/buffer_pool.h"
#include "srslte/common/logmap.h"
#include "srslte/interfaces/epc_interfaces.h"
#include <cstddef>
#include <queue>

namespace srsepc {

class spgw::gtpu : public gtpu_interface_gtpc
{
public:
  gtpu();
  virtual ~gtpu();
  int  init(spgw_args_t* args, spgw* spgw, gtpc_interface_gtpu* gtpc, srslte::log_ref gtpu_log);
  void stop();

  int init_sgi(spgw_args_t* args);
  int init_s1u(spgw_args_t* args);
  int get_sgi();
  int get_s1u();

  void handle_sgi_pdu(srslte::byte_buffer_t* msg);
  void handle_s1u_pdu(srslte::byte_buffer_t* msg);
  void send_s1u_pdu(srslte::gtp_fteid_t enb_fteid, srslte::byte_buffer_t* msg);

  virtual in_addr_t get_s1u_addr();

  virtual bool modify_gtpu_tunnel(in_addr_t ue_ipv4, srslte::gtp_fteid_t dw_user_fteid, uint32_t up_ctr_fteid);
  virtual bool delete_gtpu_tunnel(in_addr_t ue_ipv4);
  virtual bool delete_gtpc_tunnel(in_addr_t ue_ipv4);
  virtual void send_all_queued_packets(srslte::gtp_fteid_t                 dw_user_fteid,
                                       std::queue<srslte::byte_buffer_t*>& pkt_queue);

  spgw*                m_spgw;
  gtpc_interface_gtpu* m_gtpc;

  bool m_sgi_up;
  int  m_sgi;

  bool        m_s1u_up;
  int         m_s1u;
  sockaddr_in m_s1u_addr;

  std::map<in_addr_t, srslte::gtp_fteid_t> m_ip_to_usr_teid; // Map IP to User-plane TEID for downlink traffic
  std::map<in_addr_t, uint32_t>            m_ip_to_ctr_teid; // IP to control TEID map. Important to check if
                                                             // UE is attached without an active user-plane
                                                             // for downlink notifications.

  srslte::log_ref m_gtpu_log;

private:
  srslte::byte_buffer_pool* m_pool;
};

inline int spgw::gtpu::get_sgi()
{
  return m_sgi;
}

inline int spgw::gtpu::get_s1u()
{
  return m_s1u;
}

inline in_addr_t spgw::gtpu::get_s1u_addr()
{
  return m_s1u_addr.sin_addr.s_addr;
}

} // namespace srsepc
#endif // SRSEPC_GTPU_H
