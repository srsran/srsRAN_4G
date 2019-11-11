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
#include "srslte/common/rx_socket_handler.h"
#include <iostream>

#define TESTASSERT(cond)                                                                                               \
  do {                                                                                                                 \
    if (!(cond)) {                                                                                                     \
      std::cout << "[" << __FUNCTION__ << "][Line " << __LINE__ << "]: FAIL at " << (#cond) << std::endl;              \
      return -1;                                                                                                       \
    }                                                                                                                  \
  } while (0)

struct sockaddr_in local_addr;
struct sockaddr_in listen_addr;

int socket_listen(srslte::log* log_h, const char* listen_addr_str, int listen_port)
{
  /*This function sets up the SCTP socket for eNBs to connect to*/
  int                         sock_fd, err;
  struct sctp_event_subscribe evnts;

  sock_fd = socket(AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP);
  if (sock_fd == -1) {
    log_h->error("Could not create SCTP socket\n");
    return -1;
  }

  // Sets the data_io_event to be able to use sendrecv_info
  // Subscribes to the SCTP_SHUTDOWN event, to handle graceful shutdown
  bzero(&evnts, sizeof(evnts));
  evnts.sctp_data_io_event  = 1;
  evnts.sctp_shutdown_event = 1;
  if (setsockopt(sock_fd, IPPROTO_SCTP, SCTP_EVENTS, &evnts, sizeof(evnts))) {
    close(sock_fd);
    log_h->console("Subscribing to sctp_data_io_events failed\n");
    return -1;
  }

  // S1-MME bind
  bzero(&listen_addr, sizeof(listen_addr));
  listen_addr.sin_family = AF_INET;
  inet_pton(AF_INET, listen_addr_str, &(listen_addr.sin_addr));
  listen_addr.sin_port = htons(listen_port);
  err                  = bind(sock_fd, (struct sockaddr*)&listen_addr, sizeof(listen_addr));
  if (err != 0) {
    close(sock_fd);
    log_h->error("Error binding SCTP socket\n");
    return -1;
  }

  // Listen for connections
  err = listen(sock_fd, SOMAXCONN);
  if (err != 0) {
    close(sock_fd);
    log_h->error("Error in SCTP socket listen\n");
    return -1;
  }

  return sock_fd;
}

int create_fd(srslte::log* log_h, const char* bind_addr_str = "127.0.0.1")
{
  int       socket_fd   = -1;
  const int ADDR_FAMILY = AF_INET;

  if ((socket_fd = socket(ADDR_FAMILY, SOCK_STREAM, IPPROTO_SCTP)) == -1) {
    log_h->error("Failed to create S1AP socket\n");
    perror("socket()");
    goto exit_fail;
  }

  // Bind to the local address
  memset(&local_addr, 0, sizeof(struct sockaddr_in));
  local_addr.sin_family = ADDR_FAMILY;
  local_addr.sin_port   = 0; // Any local port will do
  if (inet_pton(AF_INET, bind_addr_str, &(local_addr.sin_addr)) != 1) {
    log_h->error("Error converting IP address (%s) to sockaddr_in structure\n", bind_addr_str);
    goto exit_fail;
  }
  if (bind(socket_fd, (struct sockaddr*)&local_addr, sizeof(local_addr)) != 0) {
    log_h->error("Failed to bind on S1-C address %s: %s errno %d\n", bind_addr_str, strerror(errno), errno);
    perror("bind()");
    goto exit_fail;
  }

  if (connect(socket_fd, (struct sockaddr*)&listen_addr, sizeof(listen_addr)) == -1) {
    log_h->error("Failed to establish socket connection to Remote\n");
    goto exit_fail;
  }

  log_h->info("Connected to remote\n");
  return socket_fd;

exit_fail:
  if (socket_fd >= 0) {
    close(socket_fd);
    socket_fd = -1;
  }
  return -1;
}

int test_socket_handler()
{
  srslte::log_filter log("S1AP");
  log.set_level(srslte::LOG_LEVEL_DEBUG);
  log.set_hex_limit(128);

  int                       counter = 0;
  srslte::byte_buffer_pool* pool    = srslte::byte_buffer_pool::get_instance();
  srslte::rx_socket_handler sockhandler("RXSOCKETS", &log);

  int listen_fd = socket_listen(&log, "127.0.100.1", 36412);
  log.info("Listening from %d\n", listen_fd);

  int fd = create_fd(&log, "127.0.0.1");
  TESTASSERT(fd >= 0);

  int       PPID            = 18;
  const int NONUE_STREAM_ID = 0;

  sockhandler.register_sctp_socket(listen_fd, [&counter, &log](srslte::rx_socket_handler::sctp_packet_t&& packet) {
    log.info("Received %ld bytes\n", packet.rd_sz);
    if (packet.buf != nullptr) {
      log.info_hex(packet.buf->msg, packet.buf->N_bytes, "Received msg:");
      counter++;
    }
  });

  srslte::unique_byte_buffer_t buf = srslte::allocate_unique_buffer(*pool, true);
  for (uint32_t i = 0; i < 5; ++i) {
    buf->N_bytes   = i + 1;
    buf->msg[i]    = i;
    ssize_t n_sent = sctp_sendmsg(fd,
                                  buf->msg,
                                  buf->N_bytes,
                                  (struct sockaddr*)&listen_addr,
                                  sizeof(struct sockaddr_in),
                                  htonl(PPID),
                                  0,
                                  NONUE_STREAM_ID,
                                  0,
                                  0);
    TESTASSERT(n_sent == buf->N_bytes);
    usleep(1000);
    log.info("Message %d sent.\n", i);
  }

  sleep(1);

  return 0;
}

int main()
{
  TESTASSERT(test_socket_handler() == 0);
  return 0;
}
