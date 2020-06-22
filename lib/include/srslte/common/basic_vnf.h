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

#ifndef SRSLTE_BASIC_VNF_H
#define SRSLTE_BASIC_VNF_H

#include "basic_vnf_api.h"
#include "common.h"
#include "srslte/common/logmap.h"
#include "srslte/common/threads.h"
#include "srslte/interfaces/gnb_interfaces.h"
#include "srslte/interfaces/ue_nr_interfaces.h"
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

namespace srslte {

class srslte_basic_vnf : public thread
{
public:
  srslte_basic_vnf(const vnf_args_t& args_, srslte::logger* logger_, stack_interface_phy_nr* stack_);
  ~srslte_basic_vnf();

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

  srslte::logger*                 m_logger = nullptr;
  srslte::log_ref                 log_h;
  srsenb::stack_interface_phy_nr* m_gnb_stack = nullptr;
  srsue::stack_interface_phy_nr*  m_ue_stack  = nullptr;
  srslte::byte_buffer_pool*       m_pool      = nullptr;

  std::unique_ptr<basic_vnf_api::tx_request_msg_t> m_tx_req_msg;

  bool running = false;

  vnf_args_t m_args = {};

  int                sockfd   = 0;
  struct sockaddr_in servaddr = {}, client_addr = {};

  uint32_t last_sf_indication_time = 0;
};

} // namespace srslte

#endif // SRSLTE_BASIC_VNF_H
