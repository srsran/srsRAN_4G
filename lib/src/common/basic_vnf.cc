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

#include "srslte/common/basic_vnf.h"
#include "srslte/interfaces/ue_nr_interfaces.h"
#include <algorithm>
#include <chrono>
#include <poll.h>

#define RAND_SEED (12314)
#define RX_TIMEOUT_MS (1000)

namespace srslte {

struct srslte_pnf_info_t {
  // TODO: fill when needed
};

struct srslte_vnf_info_t {};

srslte_basic_vnf::srslte_basic_vnf(const vnf_args_t& args_, srslte::logger* logger_, stack_interface_phy_nr* stack_) :
  m_args(args_),
  m_logger(logger_),
  thread("BASIC_VNF_P7"),
  m_tx_req_msg(new basic_vnf_api::tx_request_msg_t),
  log_h("VNF"),
  m_pool(srslte::byte_buffer_pool::get_instance())
{
  log_h->set_level(m_args.log_level);
  log_h->set_hex_limit(m_args.log_hex_limit);

  if (m_args.type == "gnb" || m_args.type == "ue") {
    if (m_args.type == "gnb") {
      m_gnb_stack = (srsenb::stack_interface_phy_nr*)stack_;
    } else {
      m_ue_stack = (srsue::stack_interface_phy_nr*)stack_;
    }

    log_h->info("Initializing VNF for gNB\n");
    start();
  } else {
    log_h->error("Unknown VNF type. Exiting\n.");
  }
}

srslte_basic_vnf::~srslte_basic_vnf()
{
  stop();
}

void srslte_basic_vnf::run_thread()
{
  // Bind to UDP socket
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    perror("socket");
    return;
  }

  // Make sockets reusable
  int enable = 1;
#if defined(SO_REUSEADDR)
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
    perror("setsockopt(SO_REUSEADDR) failed");
  }
#endif
#if defined(SO_REUSEPORT)
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int)) < 0) {
    perror("setsockopt(SO_REUSEPORT) failed");
  }
#endif

  servaddr.sin_family      = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port        = htons(m_args.bind_port);

  if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(struct sockaddr_in))) {
    perror("bind");
    return;
  }

  struct pollfd fd;
  fd.fd     = sockfd;
  fd.events = POLLIN;

  const uint32_t max_basic_api_pdu = sizeof(basic_vnf_api::dl_ind_msg_t) + 32; // larger than biggest message
  std::unique_ptr<std::array<uint8_t, max_basic_api_pdu> > rx_buffer =
      std::unique_ptr<std::array<uint8_t, max_basic_api_pdu> >(new std::array<uint8_t, max_basic_api_pdu>);

  running = true;

  log_h->info("Started VNF handler listening on %s:%d\n", m_args.bind_addr.c_str(), m_args.bind_port);

  while (running) {
    int ret = poll(&fd, 1, RX_TIMEOUT_MS);
    switch (ret) {
      case -1:
        printf("Error occured.\n");
        break;
      case 0:
        // Timeout
        break;
      default:

        socklen_t len = sizeof(client_addr);
        ret = recvfrom(sockfd, rx_buffer->data(), rx_buffer->size(), MSG_WAITALL, (struct sockaddr*)&client_addr, &len);

        handle_msg(rx_buffer->data(), ret);
        break;
    }
  }
  log_h->info("VNF thread stopped\n");
}

int srslte_basic_vnf::handle_msg(const uint8_t* buffer, const uint32_t len)
{
  basic_vnf_api::msg_header_t* header = (basic_vnf_api::msg_header_t*)buffer;

  log_h->info("Received %s (%d B)\n", basic_vnf_api::msg_type_text[header->type], len);

  switch (header->type) {
    case basic_vnf_api::SF_IND:
      handle_sf_ind((basic_vnf_api::sf_ind_msg_t*)header);
      break;
    case basic_vnf_api::DL_CONFIG:
      printf("Error: %s not handled by VNF\n", basic_vnf_api::msg_type_text[header->type]);
      break;
    case basic_vnf_api::DL_IND:
      handle_dl_ind((basic_vnf_api::dl_ind_msg_t*)header);
      break;
    case basic_vnf_api::UL_IND:
      handle_ul_ind((basic_vnf_api::ul_ind_msg_t*)header);
      break;
    case basic_vnf_api::RX_DATA_IND:
      handle_rx_data_ind((basic_vnf_api::rx_data_ind_msg_t*)header);
      break;
    default:
      printf("Unknown msg type.\n");
      break;
  }
  return 0;
}

int srslte_basic_vnf::handle_sf_ind(basic_vnf_api::sf_ind_msg_t* msg)
{
  int ret = SRSLTE_SUCCESS;
  log_h->info("Received %s for TTI=%d\n", basic_vnf_api::msg_type_text[msg->header.type], msg->tti);

  // store Rx timestamp
  last_sf_indication_time = msg->t1;

  if (m_gnb_stack != nullptr) {
    m_gnb_stack->sf_indication(msg->tti);
  } else if (m_ue_stack != nullptr) {
    m_ue_stack->sf_indication(msg->tti);
  } else {
    ret = SRSLTE_ERROR;
  }

  return ret;
}

int srslte_basic_vnf::handle_dl_ind(basic_vnf_api::dl_ind_msg_t* msg)
{
  int ret = SRSLTE_ERROR;
  log_h->info("Received %s for TTI=%d\n", basic_vnf_api::msg_type_text[msg->header.type], msg->tti);

  uint32_t cc_idx = 0;

  // fill DL struct
  srsue::stack_interface_phy_nr::mac_nr_grant_dl_t dl_grant = {};
  dl_grant.tti                                              = msg->tti;

  if (msg->nof_pdus > SRSLTE_MAX_TB) {
    log_h->error("Too many TBs (%d > %d)\n", msg->nof_pdus, SRSLTE_MAX_TB);
    goto exit;
  }

  for (uint32_t i = 0; i < msg->nof_pdus; ++i) {
    dl_grant.tb[i] = srslte::allocate_unique_buffer(*m_pool);
    if (dl_grant.tb[i]->get_tailroom() >= msg->pdus[i].length) {
      memcpy(dl_grant.tb[i]->msg, msg->pdus[i].data, msg->pdus[i].length);
      dl_grant.tb[i]->N_bytes = msg->pdus[i].length;
      if (msg->pdus[i].type == basic_vnf_api::PDSCH) {
        m_ue_stack->tb_decoded(cc_idx, dl_grant);
      }
    } else {
      log_h->error("TB too big to fit into buffer (%d > %d)\n", msg->pdus[i].length, dl_grant.tb[i]->get_tailroom());
    }
  }

  ret = SRSLTE_SUCCESS;

exit:

  return ret;
}

int srslte_basic_vnf::handle_ul_ind(basic_vnf_api::ul_ind_msg_t* msg)
{
  log_h->info("Received %s for TTI=%d\n", basic_vnf_api::msg_type_text[msg->header.type], msg->tti);

  if (msg->pdus.type != basic_vnf_api::PUSCH) {
    log_h->error("Received UL indication for wrong PDU type\n");
    return SRSLTE_ERROR;
  }

  uint32_t cc_idx = 0;

  // fill DL struct
  srsue::stack_interface_phy_nr::mac_nr_grant_ul_t ul_grant = {};
  ul_grant.tti                                              = msg->tti;
  ul_grant.tbs                                              = msg->pdus.length;
  ul_grant.rnti                                             = msg->rnti;
  m_ue_stack->new_grant_ul(cc_idx, ul_grant);

  return SRSLTE_SUCCESS;
}

int srslte_basic_vnf::handle_rx_data_ind(basic_vnf_api::rx_data_ind_msg_t* msg)
{
  log_h->info("Received %s for TTI=%d\n", basic_vnf_api::msg_type_text[msg->header.type], msg->sfn);

  if (msg->nof_pdus != 1 || msg->pdus[0].type != basic_vnf_api::PUSCH) {
    log_h->error("Received UL indication for wrong PDU type\n");
    return SRSLTE_ERROR;
  }

  // fill struct
  srsenb::stack_interface_phy_nr::rx_data_ind_t rx_data = {};
  rx_data.tti                                           = msg->sfn;
  rx_data.tb                                            = srslte::allocate_unique_buffer(*m_pool);
  if (rx_data.tb->get_tailroom() >= msg->pdus[0].length) {
    // copy actual data
    memcpy(rx_data.tb->msg, msg->pdus[0].data, msg->pdus[0].length);
    rx_data.tb->N_bytes = msg->pdus[0].length;
    if (msg->pdus[0].type == basic_vnf_api::PUSCH) {
      m_gnb_stack->rx_data_indication(rx_data);
    }
  }

  return SRSLTE_SUCCESS;
}

int srslte_basic_vnf::dl_config_request(const srsenb::phy_interface_stack_nr::dl_config_request_t& request)
{
  // Generate DL Config
  basic_vnf_api::dl_conf_msg_t dl_conf = {};
  dl_conf.header.type                  = basic_vnf_api::DL_CONFIG;
  dl_conf.header.msg_len               = sizeof(dl_conf) - sizeof(basic_vnf_api::msg_header_t);

  dl_conf.t1      = last_sf_indication_time; // play back the time
  dl_conf.t2      = 0xaa;                    // FIXME: add timestamp
  dl_conf.tti     = request.tti;
  dl_conf.beam_id = request.beam_id;

  // Send entire struct
  uint32_t len = sizeof(dl_conf);

  // Send it to PNF
  log_h->info("Sending %s (%d B)\n", basic_vnf_api::msg_type_text[dl_conf.header.type], len);
  int n = 0;
  if ((n = sendto(sockfd, &dl_conf, len, MSG_CONFIRM, (struct sockaddr*)&client_addr, sizeof(client_addr))) < 0) {
    log_h->error("sendto failed, ret=%d\n", n);
  }

  return 0;
}

/// Tx request from UE, i.e. UL transmission
int srslte_basic_vnf::tx_request(const srsue::phy_interface_stack_nr::tx_request_t& request)
{
  // Generate Tx request
  m_tx_req_msg->header.type    = basic_vnf_api::TX_REQUEST;
  m_tx_req_msg->header.msg_len = 0; // set further down

  m_tx_req_msg->tti = request.tti;

  m_tx_req_msg->nof_pdus       = 1;
  m_tx_req_msg->pdus[0].index  = 0;
  m_tx_req_msg->pdus[0].type   = basic_vnf_api::PUSCH;
  m_tx_req_msg->pdus[0].length = request.tb_len;

  if (request.tb_len <= MAX_PDU_SIZE) {
    // copy data from TB0
    memcpy(m_tx_req_msg->pdus[0].data, request.data, request.tb_len);
  } else {
    log_h->error("Trying to send %d B PDU. Maximum size is %d B\n", request.tb_len, MAX_PDU_SIZE);
  }

  // calculate actual length of
  uint32_t len = calc_full_msg_len(*m_tx_req_msg.get());

  // update msg header length field
  m_tx_req_msg->header.msg_len = len - sizeof(basic_vnf_api::msg_header_t);

  // Send it to PNF
  log_h->info("Sending %s (%d B)\n", basic_vnf_api::msg_type_text[m_tx_req_msg->header.type], len);
  int n = 0;
  if ((n = sendto(sockfd, m_tx_req_msg.get(), len, MSG_CONFIRM, (struct sockaddr*)&client_addr, sizeof(client_addr))) <
      0) {
    log_h->error("sendto failed, ret=%d\n", n);
  }

  return 0;
}

int srslte_basic_vnf::tx_request(const srsenb::phy_interface_stack_nr::tx_request_t& request)
{
  if (request.nof_pdus > MAX_NUM_PDUS) {
    log_h->error("Trying to send %d PDUs but only %d supported\n", request.nof_pdus, MAX_NUM_PDUS);
    return SRSLTE_ERROR;
  }
  if (request.nof_pdus == 0) {
    return SRSLTE_SUCCESS;
  }

  // Generate Tx request
  m_tx_req_msg->header.type    = basic_vnf_api::TX_REQUEST;
  m_tx_req_msg->header.msg_len = 0; // set further down

  m_tx_req_msg->nof_pdus = request.nof_pdus;
  m_tx_req_msg->tti      = request.tti;

  for (uint32_t i = 0; i < m_tx_req_msg->nof_pdus; ++i) {
    if (request.pdus[i].length <= MAX_PDU_SIZE) {
      m_tx_req_msg->pdus[i].index  = i;
      m_tx_req_msg->pdus[i].type   = request.pdus[i].pbch.mib_present ? basic_vnf_api::MAC_PBCH : basic_vnf_api::PDSCH;
      m_tx_req_msg->pdus[i].length = request.pdus[i].length;
      // copy data from TB0
      memcpy(m_tx_req_msg->pdus[i].data, request.pdus[i].data[0], m_tx_req_msg->pdus[i].length);
    } else {
      log_h->error("Trying to send %d B PDU. Maximum size is %d B\n", request.pdus[i].length, MAX_PDU_SIZE);
    }
  }

  // calculate actual length of message
  uint32_t len = calc_full_msg_len(*m_tx_req_msg.get());

  // update msg header length field
  m_tx_req_msg->header.msg_len = len - sizeof(basic_vnf_api::msg_header_t);

  // Send it to PNF
  log_h->info("Sending %s (%d B)\n", basic_vnf_api::msg_type_text[m_tx_req_msg->header.type], len);
  if (log_h->get_level() == LOG_LEVEL_DEBUG) {
    for (uint32_t i = 0; i < m_tx_req_msg->nof_pdus; ++i) {
      log_h->debug_hex(m_tx_req_msg->pdus[i].data,
                       m_tx_req_msg->pdus[i].length,
                       "Sending PDU %s:%d (%d bytes)\n",
                       basic_vnf_api::msg_type_text[m_tx_req_msg->header.type],
                       m_tx_req_msg->pdus[i].index,
                       m_tx_req_msg->pdus[i].length);
    }
  }
  int n = 0;
  if ((n = sendto(sockfd, m_tx_req_msg.get(), len, MSG_CONFIRM, (struct sockaddr*)&client_addr, sizeof(client_addr))) <
      0) {
    log_h->error("sendto failed, ret=%d\n", n);
  }

  return 0;
}

uint32_t srslte_basic_vnf::calc_full_msg_len(const basic_vnf_api::tx_request_msg_t& msg)
{
  // start with mandatory part
  uint32_t len = sizeof(basic_vnf_api::msg_header_t) + 3 * sizeof(uint32_t);

  // add all PDUs
  for (uint32_t i = 0; i < msg.nof_pdus; ++i) {
    len += 2 * sizeof(uint16_t) + sizeof(basic_vnf_api::pdu_type_t) + msg.pdus[i].length;
  }

  return len;
}

bool srslte_basic_vnf::stop()
{
  if (running) {
    running = false;
    wait_thread_finish();
  }

  return true;
}

} // namespace srslte
