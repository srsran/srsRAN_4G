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
#include <net/if.h>

namespace srsue {

struct gw_args_t {
  struct log_args_t {
    std::string gw_level;
    int         gw_hex_limit;
  } log;
  std::string netns;
  std::string tun_dev_name;
  std::string tun_dev_netmask;
};

class gw : public gw_interface_stack, public srslte::thread
{
public:
  gw();
  int  init(const gw_args_t& args_, srslte::logger* logger_, stack_interface_gw* stack);
  void stop();

  void get_metrics(gw_metrics_t& m);

  // PDCP interface
  void write_pdu(uint32_t lcid, srslte::unique_byte_buffer_t pdu);
  void write_pdu_mch(uint32_t lcid, srslte::unique_byte_buffer_t pdu);

  // NAS interface
  int setup_if_addr(uint32_t lcid, uint8_t pdn_type, uint32_t ip_addr, uint8_t* ipv6_if_addr, char* err_str);
  int apply_traffic_flow_template(const uint8_t&                                 eps_bearer_id,
                                  const uint8_t&                                 lcid,
                                  const LIBLTE_MME_TRAFFIC_FLOW_TEMPLATE_STRUCT* tft);
  void set_test_loop_mode(const test_loop_mode_state_t mode, const uint32_t ip_pdu_delay_ms);

  // RRC interface
  void add_mch_port(uint32_t lcid, uint32_t port);

private:
  static const int GW_THREAD_PRIO = -1;

  stack_interface_gw*       stack  = nullptr;
  srslte::byte_buffer_pool* pool   = nullptr;
  srslte::logger*           logger = nullptr;

  gw_args_t args = {};

  bool         running      = false;
  bool         run_enable   = false;
  int32_t      netns_fd     = 0;
  int32_t      tun_fd       = 0;
  struct ifreq ifr          = {};
  int32_t      sock         = 0;
  bool         if_up        = false;
  uint32_t     default_lcid = 0;

  srslte::log_filter log;

  uint32_t current_ip_addr = 0;
  uint8_t  current_if_id[8];

  long           ul_tput_bytes = 0;
  long           dl_tput_bytes = 0;
  struct timeval metrics_time[3];

  void run_thread();
  int  init_if(char* err_str);
  int  setup_if_addr4(uint32_t ip_addr, char* err_str);
  int  setup_if_addr6(uint8_t* ipv6_if_id, char* err_str);
  bool find_ipv6_addr(struct in6_addr* in6_out);
  void del_ipv6_addr(struct in6_addr* in6p);

  // MBSFN
  int                mbsfn_sock_fd                   = 0;  // Sink UDP socket file descriptor
  struct sockaddr_in mbsfn_sock_addr                 = {}; // Target address
  uint32_t           mbsfn_ports[SRSLTE_N_MCH_LCIDS] = {}; // Target ports for MBSFN data

  // TFT
  tft_pdu_matcher    tft_matcher;
};

} // namespace srsue

#endif // SRSUE_GW_H
