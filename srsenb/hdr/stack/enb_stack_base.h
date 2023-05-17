/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#ifndef SRSRAN_ENB_STACK_BASE_H
#define SRSRAN_ENB_STACK_BASE_H

#include "srsran/interfaces/enb_interfaces.h"
#include "srsran/interfaces/enb_mac_interfaces.h"
#include "srsran/interfaces/enb_s1ap_interfaces.h"
#include "srsue/hdr/stack/upper/gw.h"
#include <string>

namespace srsenb {

typedef struct {
  bool        enable;
  std::string filename;
} pcap_args_t;

typedef struct {
  bool        enable;
  std::string client_ip;
  std::string bind_ip;
  uint16_t    client_port;
  uint16_t    bind_port;
} pcap_net_args_t;

typedef struct {
  bool        enable;
  std::string m1u_multiaddr;
  std::string m1u_if_addr;
  uint16_t    mcs;
} embms_args_t;

typedef struct {
  std::string mac_level;
  std::string rlc_level;
  std::string pdcp_level;
  std::string rrc_level;
  std::string gtpu_level;
  std::string s1ap_level;
  std::string stack_level;

  int mac_hex_limit;
  int rlc_hex_limit;
  int pdcp_hex_limit;
  int rrc_hex_limit;
  int gtpu_hex_limit;
  int s1ap_hex_limit;
  int stack_hex_limit;
} stack_log_args_t;

typedef struct {
  uint32_t         sync_queue_size; // Max allowed difference between PHY and Stack clocks (in TTI)
  uint32_t         gtpu_indirect_tunnel_timeout_msec;
  mac_args_t       mac;
  s1ap_args_t      s1ap;
  pcap_args_t      mac_pcap;
  pcap_net_args_t  mac_pcap_net;
  pcap_args_t      s1ap_pcap;
  stack_log_args_t log;
  embms_args_t     embms;
} stack_args_t;

struct stack_metrics_t;

class enb_stack_base
{
public:
  virtual ~enb_stack_base() = default;

  virtual std::string get_type() = 0;

  virtual void stop() = 0;

  virtual void toggle_padding() = 0;
  // eNB metrics interface
  virtual bool get_metrics(stack_metrics_t* metrics) = 0;

  virtual void tti_clock() = 0;
};

} // namespace srsenb

#endif // SRSRAN_ENB_STACK_BASE_H
