/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

/******************************************************************************
 * File:        mbms-gw.h
 * Description: Top-level MBMS-GW class. Creates and links all
 *              interfaces and helpers.
 *****************************************************************************/

#ifndef MBMS_GW_H
#define MBMS_GW_H

#include "srsran/asn1/gtpc.h"
#include "srsran/common/buffer_pool.h"
#include "srsran/common/threads.h"
#include "srsran/srslog/srslog.h"
#include "srsran/srsran.h"
#include <cstddef>

namespace srsepc {

const uint16_t GTPU_RX_PORT = 2152;

typedef struct {
  std::string name;
  std::string sgi_mb_if_name;
  std::string sgi_mb_if_addr;
  std::string sgi_mb_if_mask;
  std::string m1u_multi_addr;
  std::string m1u_multi_if;
  int         m1u_multi_ttl;
} mbms_gw_args_t;

struct pseudo_hdr {
  uint32_t src_addr;
  uint32_t dst_addr;
  uint8_t  placeholder;
  uint8_t  protocol;
  uint16_t udp_len;
};

class mbms_gw : public srsran::thread
{
public:
  static mbms_gw* get_instance(void);
  static void     cleanup(void);
  int             init(mbms_gw_args_t* args);
  void            stop();
  void            run_thread();

private:
  /* Methods */
  mbms_gw();
  virtual ~mbms_gw();
  static mbms_gw* m_instance;

  int      init_sgi_mb_if(mbms_gw_args_t* args);
  int      init_m1_u(mbms_gw_args_t* args);
  void     handle_sgi_md_pdu(srsran::byte_buffer_t* msg);
  uint16_t in_cksum(uint16_t* iphdr, int count);

  /* Members */
  bool                  m_running;
  srslog::basic_logger& m_logger = srslog::fetch_basic_logger("MBMS");

  bool m_sgi_mb_up;
  int  m_sgi_mb_if;

  bool               m_m1u_up;
  int                m_m1u;
  struct sockaddr_in m_m1u_multi_addr;
};

} // namespace srsepc

#endif // SGW_H
