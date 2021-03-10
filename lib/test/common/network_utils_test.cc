/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

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
  auto& logger = srslog::fetch_basic_logger("S1AP", false);

  int counter = 0;

  srslte::socket_handler_t       server_socket, client_socket, client_socket2;
  srslte::rx_multisocket_handler sockhandler("RXSOCKETS", logger);
  int                            server_port = 36412;
  const char*                    server_addr = "127.0.100.1";
  using namespace srslte::net_utils;

  TESTASSERT(sctp_init_server(&server_socket, socket_type::seqpacket, server_addr, server_port));
  logger.info("Listening from fd=%d", server_socket.fd());

  TESTASSERT(sctp_init_client(&client_socket, socket_type::seqpacket, "127.0.0.1"));
  TESTASSERT(sctp_init_client(&client_socket2, socket_type::seqpacket, "127.0.0.2"));
  TESTASSERT(client_socket.connect_to(server_addr, server_port));
  TESTASSERT(client_socket2.connect_to(server_addr, server_port));

  // register server Rx handler
  auto pdu_handler =
      [&logger,
       &counter](srslte::unique_byte_buffer_t pdu, const sockaddr_in& from, const sctp_sndrcvinfo& sri, int flags) {
        if (pdu->N_bytes > 0) {
          logger.info(pdu->msg, pdu->N_bytes, "Received msg from %s:", get_ip(from).c_str());
          counter++;
        }
      };
  sockhandler.add_socket_sctp_pdu_handler(server_socket.fd(), pdu_handler);

  uint8_t     buf[128]        = {};
  int32_t     nof_counts      = 5;
  sockaddr_in server_addrin   = server_socket.get_addr_in();
  socklen_t   socklen         = sizeof(server_addrin);
  const int   NONUE_STREAM_ID = 0;
  for (int32_t i = 0; i < nof_counts; ++i) {
    buf[i] = i;
    // Round-robin between clients
    srslte::socket_handler_t* chosen = &client_socket;
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
