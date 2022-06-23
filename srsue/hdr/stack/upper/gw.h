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

#ifndef SRSUE_GW_H
#define SRSUE_GW_H

#include "gw_metrics.h"
#include "srsran/common/buffer_pool.h"
#include "srsran/common/common.h"
#include "srsran/common/interfaces_common.h"
#include "srsran/common/threads.h"
#include "srsran/interfaces/ue_gw_interfaces.h"
#include "srsran/srslog/srslog.h"
#include "tft_packet_filter.h"
#include <atomic>
#include <mutex>
#include <net/if.h>
#include <netinet/in.h>

namespace srsue {

class stack_interface_gw;

struct gw_args_t {
  struct log_args_t {
    std::string gw_level;
    int         gw_hex_limit;
  } log;
  std::string netns;
  std::string tun_dev_name;
  std::string tun_dev_netmask;
  std::string tun_dev_name_ims;
};

class gw : public gw_interface_stack, public srsran::thread
{
public:
  gw(srslog::basic_logger& logger_);
  ~gw();
  int  init(const gw_args_t& args_, stack_interface_gw* stack);
  void stop();

  void get_metrics(gw_metrics_t& m, const uint32_t nof_tti);

  // PDCP interface
  void write_pdu(uint32_t lcid, srsran::unique_byte_buffer_t pdu);
  void write_pdu_mch(uint32_t lcid, srsran::unique_byte_buffer_t pdu);

  // NAS interface
  int  setup_if_addr(uint32_t                eps_bearer_id,
                     srsran::srsran_apn_type srsran_apn_type,
                     uint8_t                 pdn_type,
                     uint32_t                ip_addr,
                     uint8_t*                ipv6_if_addr,
                     char*                   err_str);
  void setup_route(uint32_t pcscf_addr, srsran::srsran_apn_type srsran_apn_type);
  void setup_route_v6(uint8_t pcscf_addr[16], srsran::srsran_apn_type srsran_apn_type);
  void send_router_solicitation(srsran::srsran_apn_type srsran_apn_type);
  int  update_ipv6_prefix(int lcid, struct in6_addr* prefix, uint8_t* prefix_len);
  int  deactivate_eps_bearer(const uint32_t eps_bearer_id);
  int  apply_traffic_flow_template(const uint8_t& eps_bearer_id, const LIBLTE_MME_TRAFFIC_FLOW_TEMPLATE_STRUCT* tft);
  void set_test_loop_mode(const test_loop_mode_state_t mode, const uint32_t ip_pdu_delay_ms);
  void start_pcap(srsran::gw_pcap* pcap_);

  // RRC interface
  void add_mch_port(uint32_t lcid, uint32_t port);
  bool is_running();

private:
  static const int GW_THREAD_PRIO = -1;

  stack_interface_gw* stack = nullptr;

  gw_args_t args = {};

  std::atomic<bool> running    = {false};
  std::atomic<bool> run_enable = {false};
  int32_t           netns_fd   = 0;
  int32_t           tun_fd_inet = 0;
  int32_t           tun_fd_ims  = 0;
  struct ifreq      ifr_inet    = {};
  struct ifreq      ifr_ims     = {};
  int32_t           sock_inet   = 0;
  int32_t           sock_ims    = 0;
  std::atomic<bool> if_up_inet  = {false};
  std::atomic<bool> if_up_ims   = {false};

  static const int NOT_ASSIGNED          = -1;
  int32_t          default_eps_bearer_id_inet = NOT_ASSIGNED;
  int32_t          default_eps_bearer_id_ims  = NOT_ASSIGNED;
  std::mutex       gw_mutex;

  srslog::basic_logger& logger;
  srsran::gw_pcap*      pcap = nullptr;

  uint32_t current_ip_addr_inet = 0;
  uint32_t current_ip_addr_ims  = 0;
  uint8_t  current_if_id_ims[8];
  uint8_t  current_if_id_inet[8];

  uint32_t                                       ul_tput_bytes     = 0;
  uint32_t                                       dl_tput_bytes     = 0;
  uint32_t                                       ims_ul_tput_bytes = 0;
  uint32_t                                       ims_dl_tput_bytes = 0;
  std::chrono::high_resolution_clock::time_point metrics_tp; // stores time when last metrics have been taken

  void          run_thread();
  int           init_if(srsran::srsran_apn_type srsran_apn_type, char* err_str);
  int           setup_if_addr4(uint32_t ip_addr, srsran::srsran_apn_type srsran_apn_type, char* err_str);
  int           setup_if_addr6(uint8_t* ipv6_if_id, srsran::srsran_apn_type srsran_apn_type, char* err_str);
  bool          find_ipv6_addr(srsran::srsran_apn_type srsran_apn_type, struct in6_addr* in6_out);
  void          del_ipv6_addr(srsran::srsran_apn_type srsran_apn_type, struct in6_addr* in6p);
  struct ifreq& fetch_right_ifr(srsran::srsran_apn_type srsran_apn_type);
  std::string   fetch_interface_name(srsran::srsran_apn_type srsran_apn_type);
  int32_t&      fetch_right_tun_fd(srsran::srsran_apn_type srsran_apn_type);

  // MBSFN
  int                mbsfn_sock_fd                   = 0;  // Sink UDP socket file descriptor
  struct sockaddr_in mbsfn_sock_addr                 = {}; // Target address
  uint32_t           mbsfn_ports[SRSRAN_N_MCH_LCIDS] = {}; // Target ports for MBSFN data

  // IMS Interface
  int                ims_sock_fd                   = 0;  // Sink UDP socket file descriptor
  struct sockaddr_in ims_sock_addr                 = {}; // Target address
  uint16_t           ims_ports[SRSRAN_N_MCH_LCIDS] = {}; // Target ports for MBSFN data

  // TFT
  tft_pdu_matcher tft_matcher;
};

} // namespace srsue

#endif // SRSUE_GW_H
