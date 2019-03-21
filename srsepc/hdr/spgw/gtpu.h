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

#ifndef SRSEPC_GTPU_H
#define SRSEPC_GTPU_H

#include "srsepc/hdr/spgw/spgw.h"
#include "srslte/asn1/gtpc.h"
#include "srslte/common/buffer_pool.h"
#include "srslte/interfaces/epc_interfaces.h"
#include <cstddef>

namespace srsepc {

class spgw::gtpu : public gtpu_interface_gtpc {
 public:
  gtpu();
  virtual ~gtpu();
  int init(spgw_args_t *args, spgw *spgw, srslte::log_filter *gtpu_log);
  void stop();

  srslte::error_t init_sgi(spgw_args_t *args);
  srslte::error_t init_s1u(spgw_args_t *args);
  int get_sgi();
  int get_s1u();

  void handle_sgi_pdu(srslte::byte_buffer_t *msg);
  void handle_s1u_pdu(srslte::byte_buffer_t *msg);
  void send_s1u_pdu(srslte::gtp_fteid_t enb_fteid, srslte::byte_buffer_t *msg);

  virtual in_addr_t get_s1u_addr();

  virtual bool modify_gtpu_tunnel(in_addr_t ue_ipv4, srslte::gtp_fteid_t dw_user_fteid, uint32_t up_ctr_fteid);
  virtual bool delete_gtpu_tunnel(in_addr_t ue_ipv4);

  std::string gtpu_ntoa(uint32_t addr);

  spgw *m_spgw;

  bool m_sgi_up;
  int m_sgi;

  bool m_s1u_up;
  int m_s1u;
  sockaddr_in m_s1u_addr;

  std::map<in_addr_t, srslte::gtpc_f_teid_ie> m_ip_to_usr_teid;  // Map IP to User-plane TEID for downlink traffic

  srslte::log_filter *m_gtpu_log;

private:
  srslte::byte_buffer_pool *m_pool;  
};

inline int spgw::gtpu::get_sgi(){
  return m_sgi;
} 

inline int spgw::gtpu::get_s1u(){
  return m_s1u;
} 

inline in_addr_t spgw::gtpu::get_s1u_addr()
{
  return m_s1u_addr.sin_addr.s_addr;
}

// Helper function to return a string from IPv4 address for easy printing
inline std::string spgw::gtpu::gtpu_ntoa(uint32_t addr){
  char tmp_str[INET_ADDRSTRLEN+1];
  bzero(tmp_str, sizeof(tmp_str));
  struct in_addr tmp_addr;
  tmp_addr.s_addr = addr;
  const char* tmp_ptr = inet_ntop(AF_INET, &tmp_addr, tmp_str, INET_ADDRSTRLEN);
  if(tmp_ptr == NULL){
    return std::string("Invalid IPv4 address");
  }
  return std::string(tmp_str);
} 

}// namespace srsepc
#endif // SRSEPC_GTPU_H
