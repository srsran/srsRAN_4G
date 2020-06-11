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

#ifndef SRSLTE_BASIC_PNF_H
#define SRSLTE_BASIC_PNF_H

#include "basic_vnf_api.h"
#include "common.h"
#include <arpa/inet.h>
#include <atomic>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <mutex>
#include <netinet/in.h>
#include <poll.h>
#include <random>
#include <strings.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

#define RAND_SEED (12384)
#define RX_TIMEOUT_MS (500)

#define MIN_TB_LEN (100) // MAX_TB_LEN defined in api.h

#define PING_REQUEST_PDU 1

namespace srslte {

struct pnf_metrics_t {
  uint32_t avg_rtt_us;
  uint32_t num_timing_errors;
  uint32_t num_pdus;
  uint32_t tb_size;
};

class srslte_basic_pnf
{
public:
  srslte_basic_pnf(const std::string& type_,
                   const std::string& vnf_p5_addr,
                   const uint16_t&    vnf_p5_port,
                   const uint32_t&    sf_interval,
                   const int32_t&     num_sf_,
                   const uint32_t&    tb_len_) :
    running(false),
    type(type_),
    tti(100), ///< Random start TTI
    vnf_addr(vnf_p5_addr),
    vnf_port(vnf_p5_port),
    sf_interval_us(sf_interval),
    num_sf(num_sf_),
    tb_len(tb_len_),
    rand_gen(RAND_SEED),
    rand_dist(MIN_TB_LEN, MAX_TB_LEN){};

  ~srslte_basic_pnf() { stop(); };

  bool start()
  {
    // create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
      perror("socket");
      return false;
    }

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

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(vnf_addr.c_str());
    servaddr.sin_port        = htons(vnf_port);

    // start main thread
    running = true;

    if (type == "gnb") {
      rx_thread = std::unique_ptr<std::thread>(new std::thread(&srslte_basic_pnf::rx_thread_function, this));
      tx_thread = std::unique_ptr<std::thread>(new std::thread(&srslte_basic_pnf::tx_thread_function, this));
    } else {
      tx_thread = std::unique_ptr<std::thread>(new std::thread(&srslte_basic_pnf::tx_thread_function_ue, this));
    }

    return true;
  };

  bool stop()
  {
    running = false;

    if (rx_thread) {
      if (rx_thread->joinable()) {
        rx_thread->join();
      }
    }

    if (tx_thread) {
      if (tx_thread->joinable()) {
        tx_thread->join();
      }
    }

    return true;
  };

  pnf_metrics_t get_metrics()
  {
    pnf_metrics_t tmp = metrics;
    metrics           = {};
    return tmp;
  }

private:
  void rx_thread_function()
  {
    pthread_setname_np(pthread_self(), rx_thread_name.c_str());

    // set_rt_prio();

    struct pollfd fd;
    fd.fd     = sockfd;
    fd.events = POLLIN;

    const uint32_t max_basic_api_pdu = sizeof(basic_vnf_api::dl_conf_msg_t) + 32; // larger than biggest message
    std::unique_ptr<std::array<uint8_t, max_basic_api_pdu> > rx_buffer =
        std::unique_ptr<std::array<uint8_t, max_basic_api_pdu> >(new std::array<uint8_t, max_basic_api_pdu>);

    while (running) {
      // receive response
      int ret = poll(&fd, 1, RX_TIMEOUT_MS);
      switch (ret) {
        case -1:
          printf("Error occured.\n");
          running = false;
          break;
        case 0:
          // Timeout
          printf("Error: Didn't receive response after %dms\n", RX_TIMEOUT_MS);
          running = false;
          break;
        default:
          int recv_ret = recv(sockfd, rx_buffer->data(), rx_buffer->size(), 0);
          handle_msg(rx_buffer->data(), recv_ret);
          break;
      }

      std::lock_guard<std::mutex> lock(mutex);
      auto                        rtt =
          std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - tti_start_time)
              .count();

      // FIXME: add averaging
      metrics.avg_rtt_us = rtt;
    }
  };

  void tx_thread_function()
  {
    pthread_setname_np(pthread_self(), tx_thread_name.c_str());

    // set_rt_prio();

    struct pollfd fd;
    fd.fd     = sockfd;
    fd.events = POLLIN;

    const uint32_t max_basic_api_pdu = sizeof(basic_vnf_api::dl_conf_msg_t) + 32; // larger than biggest message
    std::unique_ptr<std::array<uint8_t, max_basic_api_pdu> > rx_buffer =
        std::unique_ptr<std::array<uint8_t, max_basic_api_pdu> >(new std::array<uint8_t, max_basic_api_pdu>);

    int32_t sf_counter = 0;
    while (running && (num_sf > 0 ? sf_counter < num_sf : true)) {
      {
        std::lock_guard<std::mutex> lock(mutex);

        // Increase TTI
        tti = (tti + 1) % 10240;

        // Take time before sending the SF indication
        tti_start_time = std::chrono::steady_clock::now();

        // Send request
        send_sf_ind(tti);

        // provide UL data every 2nd TTI
        if (tti % 2 == 0) {
          send_rx_data_ind(tti);
        }

        sf_counter++;
      }

      std::this_thread::sleep_for(std::chrono::microseconds(sf_interval_us));
    }

    printf("Leaving Tx thread after %d subframes\n", sf_counter);
  };

  void tx_thread_function_ue()
  {
    pthread_setname_np(pthread_self(), tx_thread_name.c_str());

    // set_rt_prio();

    struct pollfd fd;
    fd.fd     = sockfd;
    fd.events = POLLIN;

    const uint32_t max_basic_api_pdu = sizeof(basic_vnf_api::dl_conf_msg_t) + 32; // larger than biggest message
    std::unique_ptr<std::array<uint8_t, max_basic_api_pdu> > rx_buffer =
        std::unique_ptr<std::array<uint8_t, max_basic_api_pdu> >(new std::array<uint8_t, max_basic_api_pdu>);

    int32_t sf_counter = 0;
    while (running && (num_sf > 0 ? sf_counter < num_sf : true)) {
      {
        std::lock_guard<std::mutex> lock(mutex);

        // Increase TTI
        tti = (tti + 1) % 10240;

        // Take time before sending the SF indication
        tti_start_time = std::chrono::steady_clock::now();

        // Send SF indication
        send_sf_ind(tti);

        // provide DL grant every even TTI, and UL grant every odd
        if (tti % 2 == 0) {
          send_dl_ind(tti);
        } else {
          send_ul_ind(tti);
        }

        sf_counter++;
      }

      std::this_thread::sleep_for(std::chrono::microseconds(sf_interval_us));
    }

    printf("Leaving Tx thread after %d subframes\n", sf_counter);
  };

  void send_sf_ind(uint32_t tti_)
  {
    basic_vnf_api::sf_ind_msg_t sf_ind;
    bzero(&sf_ind, sizeof(sf_ind));
    sf_ind.header.type    = basic_vnf_api::SF_IND;
    sf_ind.header.msg_len = sizeof(sf_ind) - sizeof(basic_vnf_api::msg_header_t);
    sf_ind.tti            = tti_;
    sf_ind.t1             = 0;
    sf_ind.tb_len         = tb_len > 0 ? tb_len : rand_dist(rand_gen);

    int n = 0;
    if ((n = sendto(sockfd, &sf_ind, sizeof(sf_ind), 0, (struct sockaddr*)&servaddr, sizeof(servaddr))) < 0) {
      printf("sendto failed, ret=%d\n", n);
    }
  }

  int handle_msg(const uint8_t* buffer, const uint32_t len)
  {
    basic_vnf_api::msg_header_t* header = (basic_vnf_api::msg_header_t*)buffer;

    // printf("Received %s (%d B) in TTI\n", msg_type_text[header->type], len);

    switch (header->type) {
      case basic_vnf_api::SF_IND:
        printf("Error: %s not handled by VNF\n", basic_vnf_api::msg_type_text[header->type]);
        break;
      case basic_vnf_api::DL_CONFIG:
        handle_dl_config((basic_vnf_api::dl_conf_msg_t*)header);
        break;
      case basic_vnf_api::TX_REQUEST:
        handle_tx_request((basic_vnf_api::tx_request_msg_t*)header);
        break;
      default:
        printf("Unknown msg type.\n");
        break;
    }
    return 0;
  }

  int handle_dl_config(basic_vnf_api::dl_conf_msg_t* msg)
  {
    // printf("Received DL config for TTI=%d\n", msg->tti);

    if (msg->tti != tti) {
      metrics.num_timing_errors++;
      // printf("Received DL config for TTI=%d but current TTI is %d\n", msg->tti, tti.load());
      return -1;
    }

    return 0;
  }

  int handle_tx_request(basic_vnf_api::tx_request_msg_t* msg)
  {
    // printf("Received TX request config for TTI=%d\n", msg->tti);

    if (msg->tti != tti) {
      metrics.num_timing_errors++;
      // printf("Received TX request for TTI=%d but current TTI is %d\n", msg->tti, tti.load());
      return -1;
    }

    for (uint32_t i = 0; i < msg->nof_pdus; ++i) {
      metrics.tb_size += msg->pdus[i].length;
    }

    metrics.num_pdus += msg->nof_pdus;

    return 0;
  }

  void send_rx_data_ind(const uint32_t tti)
  {
    // MAC PDU for UL-SCH with IPv6 router solicitation
    static uint8_t tv[] = {0x04, 0x38, 0x00, 0x80, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x08, 0x3a, 0xff, 0xfe,
                           0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x21, 0x44, 0x4b, 0x0f, 0x2c, 0x33, 0x98, 0xf2,
                           0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x02, 0x85, 0x00, 0x4b, 0xbe, 0x00, 0x00, 0x00, 0x00, 0x2f, 0x7f, 0x00, 0x00, 0x3f, 0x00};

    basic_vnf_api::rx_data_ind_msg_t rx_ind = {};

    rx_ind.header.type    = basic_vnf_api::RX_DATA_IND;
    rx_ind.header.msg_len = sizeof(rx_ind) - sizeof(basic_vnf_api::msg_header_t);
    rx_ind.sfn            = tti;
    rx_ind.t1             = 0;

    rx_ind.nof_pdus       = 1;
    rx_ind.pdus[0].type   = basic_vnf_api::PUSCH;
    rx_ind.pdus[0].length = tb_len > 0 ? tb_len : rand_dist(rand_gen);

    if (rx_ind.pdus[0].length >= sizeof(tv)) {
      // copy TV
      memcpy(rx_ind.pdus[0].data, tv, sizeof(tv));
      // set remaining bytes to zero
      memset(rx_ind.pdus[0].data + sizeof(tv), 0xaa, rx_ind.pdus[0].length - sizeof(tv));
    } else {
      // just fill with dummy bytes
      memset(rx_ind.pdus[0].data, 0xab, rx_ind.pdus[0].length);
    }

    int n = 0;
    if ((n = sendto(sockfd, &rx_ind, sizeof(rx_ind), 0, (struct sockaddr*)&servaddr, sizeof(servaddr))) < 0) {
      printf("sendto failed, ret=%d\n", n);
    }
  }

  void send_dl_ind(uint32_t tti)
  {
#if PING_REQUEST_PDU
    static uint8_t tv[] = {
        0x04, 0x5c, 0x00, 0x80, 0x00, 0x00, 0x45, 0x00, 0x00, 0x54, 0x15, 0x02, 0x40, 0x00, 0x40, 0x01, 0xa2, 0x52,
        0xc0, 0xa8, 0x01, 0x01, 0xc0, 0xa8, 0x01, 0x03, 0x08, 0x00, 0x26, 0x40, 0x5e, 0x8f, 0x00, 0xb3, 0x04, 0x55,
        0xc4, 0x5d, 0x00, 0x00, 0x00, 0x00, 0xe4, 0xf7, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x11, 0x12, 0x13,
        0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25,
        0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x4f, 0x7f, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
#else
    // MAC PDU with a single LCID with padding only
    static uint8_t tv[] = {
        0x01,
        0x08,
        0x11,
        0x22,
        0x33,
        0x44,
        0x55,
        0x66,
        0x77,
        0x88,
        0x3f,
    };
#endif // PING_REQUEST_PDU

    basic_vnf_api::dl_ind_msg_t dl_ind = {};

    dl_ind.header.type    = basic_vnf_api::DL_IND;
    dl_ind.header.msg_len = sizeof(dl_ind) - sizeof(basic_vnf_api::msg_header_t);
    dl_ind.tti            = tti;
    dl_ind.t1             = 0;

    dl_ind.nof_pdus       = 1;
    dl_ind.pdus[0].type   = basic_vnf_api::PDSCH;
    dl_ind.pdus[0].length = tb_len > 0 ? tb_len : rand_dist(rand_gen);

    if (dl_ind.pdus[0].length >= sizeof(tv)) {
      // copy TV
      memcpy(dl_ind.pdus[0].data, tv, sizeof(tv));
      // set remaining bytes to zero
      memset(dl_ind.pdus[0].data + sizeof(tv), 0xaa, dl_ind.pdus[0].length - sizeof(tv));
    } else {
      // just fill with dummy bytes
      memset(dl_ind.pdus[0].data, 0xab, dl_ind.pdus[0].length);
    }

    int n = 0;
    if ((n = sendto(sockfd, &dl_ind, sizeof(dl_ind), 0, (struct sockaddr*)&servaddr, sizeof(servaddr))) < 0) {
      printf("sendto failed, ret=%d\n", n);
    }
  }

  void send_ul_ind(uint32_t tti_)
  {
    basic_vnf_api::ul_ind_msg_t ul_ind = {};

    ul_ind.header.type    = basic_vnf_api::UL_IND;
    ul_ind.header.msg_len = sizeof(ul_ind) - sizeof(basic_vnf_api::msg_header_t);
    ul_ind.tti            = tti_;
    ul_ind.t1             = 0;

    ul_ind.pdus.type   = basic_vnf_api::PUSCH;
    ul_ind.pdus.length = tb_len > 0 ? tb_len : rand_dist(rand_gen);

    int n = 0;
    if ((n = sendto(sockfd, &ul_ind, sizeof(ul_ind), 0, (struct sockaddr*)&servaddr, sizeof(servaddr))) < 0) {
      printf("sendto failed, ret=%d\n", n);
    }
  }

  std::unique_ptr<std::thread> tx_thread, rx_thread;
  std::string                  tx_thread_name = "TX_PNF", rx_thread_name = "RX_PNF";
  bool                         running = false;

  std::mutex                            mutex;
  std::atomic<std::uint32_t>            tti;
  std::chrono::steady_clock::time_point tti_start_time;

  std::string type;

  std::string vnf_addr;
  uint16_t    vnf_port = 3333;

  uint32_t sf_interval_us = 1000;
  int32_t  num_sf         = -1;
  uint32_t tb_len         = 100;

  pnf_metrics_t metrics = {};

  int                sockfd   = 0;
  struct sockaddr_in servaddr = {};

  // For random number generation
  std::mt19937                            rand_gen;
  std::uniform_int_distribution<uint16_t> rand_dist;
};

} // namespace srslte

#endif // SRSLTE_BASIC_PNF_H
