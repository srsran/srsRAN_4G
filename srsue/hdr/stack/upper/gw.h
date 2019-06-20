/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

#ifndef SRSUE_GW_H
#define SRSUE_GW_H

#include "gw_metrics.h"
#include "srslte/common/buffer_pool.h"
#include "srslte/common/common.h"
#include "srslte/common/interfaces_common.h"
#include "srslte/common/log.h"
#include "srslte/common/log_filter.h"
#include "srslte/common/threads.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "tft_packet_filter.h"
#include <mutex>
#include <net/if.h>

namespace srsue {

struct gw_args_t {
  struct log_args_t {
    std::string gw_level;
    int         gw_hex_limit;
  } log;
  std::string tun_dev_name;
  std::string tun_dev_netmask;
};

class gw : public gw_interface_stack, public thread
{
public:
  gw();
  int  init(const gw_args_t& args_, srslte::logger* logger_, stack_interface_gw* stack);
  void stop();

  void get_metrics(gw_metrics_t &m);

  // PDCP interface
  void write_pdu(uint32_t lcid, srslte::unique_byte_buffer_t pdu);
  void write_pdu_mch(uint32_t lcid, srslte::unique_byte_buffer_t pdu);

  // NAS interface
  int setup_if_addr(uint32_t lcid, uint8_t pdn_type, uint32_t ip_addr, uint8_t* ipv6_if_addr, char* err_str);
  int apply_traffic_flow_template(const uint8_t&                                 eps_bearer_id,
                                  const uint8_t&                                 lcid,
                                  const LIBLTE_MME_TRAFFIC_FLOW_TEMPLATE_STRUCT* tft);

  // RRC interface
  void add_mch_port(uint32_t lcid, uint32_t port);

private:
  static const int GW_THREAD_PRIO = 7;

  stack_interface_gw*       stack;
  srslte::byte_buffer_pool* pool;
  srslte::logger*           logger;

  gw_args_t args;

  bool                running;
  bool                run_enable;
  int32_t             tun_fd;
  struct ifreq        ifr;
  int32_t             sock;
  bool                if_up;
  uint32_t            default_lcid = 0;

  srslte::log_filter log;

  uint32_t            current_ip_addr;
  uint8_t             current_if_id[8];

  long                ul_tput_bytes;
  long                dl_tput_bytes;
  struct timeval      metrics_time[3];

  void run_thread();
  int  init_if(char* err_str);
  int  setup_if_addr4(uint32_t ip_addr, char* err_str);
  int  setup_if_addr6(uint8_t* ipv6_if_id, char* err_str);
  bool find_ipv6_addr(struct in6_addr* in6_out);
  void del_ipv6_addr(struct in6_addr* in6p);

  // MBSFN
  int      mbsfn_sock_fd;                   // Sink UDP socket file descriptor
  struct   sockaddr_in mbsfn_sock_addr;     // Target address
  uint32_t mbsfn_ports[SRSLTE_N_MCH_LCIDS]; // Target ports for MBSFN data

  // TFT
  std::mutex                                      tft_mutex;
  typedef std::map<uint16_t, tft_packet_filter_t> tft_filter_map_t;
  tft_filter_map_t                                tft_filter_map;

  uint8_t check_tft_filter_match(const srslte::unique_byte_buffer_t& pdu);
};

} // namespace srsue


#endif // SRSUE_GW_H
