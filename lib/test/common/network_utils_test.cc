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

#include "srslte/common/log_filter.h"
#include "srslte/common/network_utils.h"
#include <iostream>

#define TESTASSERT(cond)                                                                                               \
  do {                                                                                                                 \
    if (!(cond)) {                                                                                                     \
      std::cout << "[" << __FUNCTION__ << "][Line " << __LINE__ << "]: FAIL at " << (#cond) << std::endl;              \
      return -1;                                                                                                       \
    }                                                                                                                  \
  } while (0)

int test_socket_handler()
{
  srslte::log_filter log("S1AP");
  log.set_level(srslte::LOG_LEVEL_DEBUG);
  log.set_hex_limit(128);

  int                       counter = 0;
  srslte::byte_buffer_pool* pool    = srslte::byte_buffer_pool::get_instance();

  srslte::sctp_socket_t          server_sock, client_sock, client_sock2;
  srslte::rx_multisocket_handler sockhandler("RXSOCKETS", &log);

  TESTASSERT(server_sock.listen_addr("127.0.100.1", 36412) == 0);
  log.info("Listening from fd=%d\n", server_sock.fd());

  TESTASSERT(client_sock.connect_addr("127.0.0.1", "127.0.100.1", 36412) == 0);
  TESTASSERT(client_sock2.connect_addr("127.0.0.2", "127.0.100.1", 36412) == 0);

  // register server Rx handler
  sockhandler.register_socket(server_sock, [pool, &log, &counter](const srslte::sctp_socket_t& sock) {
    srslte::unique_byte_buffer_t pdu = srslte::allocate_unique_buffer(*pool, true);
    srslte::net_addr_t           addr;
    int                          rd_sz = sock.read(pdu->msg, pdu->get_tailroom(), &addr);
    if (rd_sz > 0) {
      pdu->N_bytes = rd_sz;
      log.info_hex(pdu->msg, pdu->N_bytes, "Received msg from %s:", addr.ip().c_str());
      counter++;
    }
  });

  int       PPID            = 18;
  const int NONUE_STREAM_ID = 0;

  uint8_t buf[128]   = {};
  int32_t nof_counts = 5;
  for (int32_t i = 0; i < nof_counts; ++i) {
    buf[i] = i;
    // Round-robin between clients
    srslte::sctp_socket_t* chosen = &client_sock;
    if (i % 2 == 1) {
      chosen = &client_sock2;
    }
    // send packet
    ssize_t n_sent = chosen->send(buf, i + 1, PPID, NONUE_STREAM_ID);
    TESTASSERT(n_sent >= 0);
    usleep(1000);
    log.info("Message %d sent.\n", i);
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
  TESTASSERT(test_socket_handler() == 0);
  return 0;
}
