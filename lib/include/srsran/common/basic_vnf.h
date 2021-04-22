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

#ifndef SRSRAN_BASIC_VNF_H
#define SRSRAN_BASIC_VNF_H

#include "basic_vnf_api.h"
#include "common.h"
#include "srsran/common/threads.h"
#include "srsran/interfaces/gnb_interfaces.h"
#include "srsran/interfaces/ue_nr_interfaces.h"
#include "srsran/srslog/srslog.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <random>
#include <strings.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

namespace srsran {

class srsran_basic_vnf : public thread
{
public:
  srsran_basic_vnf(const vnf_args_t& args_, stack_interface_phy_nr* stack_);
  ~srsran_basic_vnf();

  bool stop();

  int dl_config_request(const srsenb::phy_interface_stack_nr::dl_config_request_t& request);
  int tx_request(const srsenb::phy_interface_stack_nr::tx_request_t& request);
  int tx_request(const srsue::phy_interface_stack_nr::tx_request_t& request);

private:
  void run_thread();

  // handlers
  int handle_msg(const uint8_t* buffer, const uint32_t len);
  int handle_sf_ind(basic_vnf_api::sf_ind_msg_t* msg);
  int handle_dl_ind(basic_vnf_api::dl_ind_msg_t* msg);
  int handle_ul_ind(basic_vnf_api::ul_ind_msg_t* msg);
  int handle_rx_data_ind(basic_vnf_api::rx_data_ind_msg_t* msg);

  // senders
  int send_dl_config_request();

  // helpers
  uint32_t calc_full_msg_len(const basic_vnf_api::tx_request_msg_t& msg);

  srslog::basic_logger&           logger      = srslog::fetch_basic_logger("VNF", false);
  srsenb::stack_interface_phy_nr* m_gnb_stack = nullptr;
  srsue::stack_interface_phy_nr*  m_ue_stack  = nullptr;

  std::unique_ptr<basic_vnf_api::tx_request_msg_t> m_tx_req_msg;

  bool running = false;

  vnf_args_t m_args = {};

  int                sockfd   = 0;
  struct sockaddr_in servaddr = {}, client_addr = {};

  uint32_t last_sf_indication_time = 0;
};

} // namespace srsran

#endif // SRSRAN_BASIC_VNF_H
