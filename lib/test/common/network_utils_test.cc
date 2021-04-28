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

#include "srsran/common/network_utils.h"
#include "srsran/common/task_scheduler.h"
#include "srsran/common/test_common.h"
#include <atomic>
#include <iostream>

struct rx_thread_tester {
  srsran::task_scheduler    task_sched;
  srsran::task_queue_handle task_queue;
  std::atomic<bool>         stop_token;
  std::thread               t;

  rx_thread_tester() :
    task_queue(task_sched.make_task_queue()),
    t([this]() {
      stop_token.store(false);
      while (not stop_token.load(std::memory_order_relaxed)) {
        task_sched.run_pending_tasks();
        std::this_thread::sleep_for(std::chrono::microseconds(100));
      }
    }),
    stop_token(false)
  {}
  ~rx_thread_tester()
  {
    stop_token.store(true, std::memory_order_relaxed);
    t.join();
  }
};

int test_socket_handler()
{
  auto& logger = srslog::fetch_basic_logger("S1AP", false);

  int counter = 0;

  srsran::unique_socket  server_socket, client_socket, client_socket2;
  srsran::socket_manager sockhandler;
  int                    server_port = 36412;
  const char*            server_addr = "127.0.100.1";
  using namespace srsran::net_utils;

  TESTASSERT(sctp_init_server(&server_socket, socket_type::seqpacket, server_addr, server_port));
  logger.info("Listening from fd=%d", server_socket.fd());

  TESTASSERT(sctp_init_client(&client_socket, socket_type::seqpacket, "127.0.0.1"));
  TESTASSERT(sctp_init_client(&client_socket2, socket_type::seqpacket, "127.0.0.2"));
  TESTASSERT(client_socket.connect_to(server_addr, server_port));
  TESTASSERT(client_socket2.connect_to(server_addr, server_port));

  // register server Rx handler
  auto pdu_handler =
      [&logger,
       &counter](srsran::unique_byte_buffer_t pdu, const sockaddr_in& from, const sctp_sndrcvinfo& sri, int flags) {
        if (pdu->N_bytes > 0) {
          logger.info(pdu->msg, pdu->N_bytes, "Received msg from %s:", get_ip(from).c_str());
          counter++;
        }
      };
  rx_thread_tester rx_tester;
  sockhandler.add_socket_handler(server_socket.fd(),
                                 srsran::make_sctp_sdu_handler(logger, rx_tester.task_queue, pdu_handler));

  uint8_t     buf[128]        = {};
  int32_t     nof_counts      = 5;
  sockaddr_in server_addrin   = server_socket.get_addr_in();
  socklen_t   socklen         = sizeof(server_addrin);
  const int   NONUE_STREAM_ID = 0;
  for (int32_t i = 0; i < nof_counts; ++i) {
    buf[i] = i;
    // Round-robin between clients
    srsran::unique_socket* chosen = &client_socket;
    if (i % 2 == 1) {
      chosen = &client_socket2;
    }
    // send packet
    ssize_t n_sent = sctp_sendmsg(chosen->fd(),
                                  buf,
                                  i + 1,
                                  (struct sockaddr*)&server_addrin,
                                  socklen,
                                  (uint32_t)ppid_values::S1AP,
                                  0,
                                  NONUE_STREAM_ID,
                                  0,
                                  0);
    TESTASSERT(n_sent >= 0);
    usleep(1000);
    logger.info("Message %d sent.", i);
  }

  uint32_t time_elapsed = 0;
  while (counter != nof_counts) {
    usleep(100);
    time_elapsed += 100;
    if (time_elapsed > 3000000) {
      // too much time has passed
      return -1;
    }
  }

  return 0;
}

int main()
{
  auto& logger = srslog::fetch_basic_logger("S1AP", false);
  logger.set_level(srslog::basic_levels::debug);
  logger.set_hex_dump_max_size(128);

  srslog::init();

  TESTASSERT(test_socket_handler() == 0);

  return 0;
}
