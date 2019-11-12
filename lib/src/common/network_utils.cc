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

#include "srslte/common/network_utils.h"

#include <netinet/sctp.h>
#include <sys/types.h>

#define rxSockError(fmt, ...) log_h->error("%s: " fmt, name.c_str(), ##__VA_ARGS__)
#define rxSockInfo(fmt, ...) log_h->info("%s: " fmt, name.c_str(), ##__VA_ARGS__)
#define rxSockDebug(fmt, ...) log_h->debug("%s: " fmt, name.c_str(), ##__VA_ARGS__)

namespace srslte {

sctp_socket::sctp_socket()
{
  bzero(&addr_in, sizeof(addr_in));
  bzero(&dest_addr, sizeof(dest_addr));
}

sctp_socket::sctp_socket(sctp_socket&& other) noexcept
{
  sockfd = other.sockfd;
  memcpy(&addr_in, &other.addr_in, sizeof(addr_in));
  // reset other without calling close
  other.sockfd = -1;
  bzero(&other.addr_in, sizeof(other.addr_in));
}

sctp_socket::~sctp_socket()
{
  reset();
}

sctp_socket& sctp_socket::operator=(sctp_socket&& other) noexcept
{
  if (this == &other) {
    return *this;
  }
  sockfd = other.sockfd;
  memcpy(&addr_in, &other.addr_in, sizeof(addr_in));
  other.sockfd = -1;
  bzero(&other.addr_in, sizeof(other.addr_in));
  return *this;
}

void sctp_socket::reset()
{
  if (sockfd >= 0) {
    close(sockfd);
  }
  bzero(&addr_in, sizeof(addr_in));
  bzero(&dest_addr, sizeof(dest_addr));
}

int sctp_socket::listen_addr(const char* bind_addr_str, int port)
{
  if (sockfd < 0) {
    if (create_socket()) {
      return -1;
    }
  }

  // Sets the data_io_event to be able to use sendrecv_info
  // Subscribes to the SCTP_SHUTDOWN event, to handle graceful shutdown
  struct sctp_event_subscribe evnts;
  bzero(&evnts, sizeof(evnts));
  evnts.sctp_data_io_event  = 1;
  evnts.sctp_shutdown_event = 1;
  if (setsockopt(sockfd, IPPROTO_SCTP, SCTP_EVENTS, &evnts, sizeof(evnts))) {
    perror("setsockopt");
    reset();
    return -1;
  }

  // bind addr
  if (bind_addr(bind_addr_str, port)) {
    reset();
    return -1;
  }

  // Listen for connections
  if (listen(sockfd, SOMAXCONN)) {
    perror("listen");
    reset();
    return -1;
  }

  return 0;
}

int sctp_socket::connect_addr(const char* bind_addr_str, const char* dest_addr_str, int dest_port)
{
  if (sockfd < 0) {
    if (bind_addr(bind_addr_str, 0)) {
      return -1;
    }
  }

  dest_addr.sin_family = AF_INET;
  dest_addr.sin_port   = htons(dest_port);
  if (inet_pton(AF_INET, dest_addr_str, &(dest_addr.sin_addr)) != 1) {
    perror("inet_pton()");
    return -1;
  }
  if (connect(sockfd, (struct sockaddr*)&dest_addr, sizeof(dest_addr)) == -1) {
    perror("connect()");
    return -1;
  }

  return 0;
}

int sctp_socket::read(void*                   buf,
                      ssize_t                 nbytes,
                      struct sockaddr_in*     from,
                      socklen_t               fromlen,
                      struct sctp_sndrcvinfo* sinfo,
                      int                     msg_flags)
{
  int rd_sz = sctp_recvmsg(sockfd, buf, nbytes, (struct sockaddr*)from, &fromlen, sinfo, &msg_flags);
  if (rd_sz <= 0) {
    perror("sctp read");
  }
  return rd_sz;
}

int sctp_socket::send(void* buf, ssize_t nbytes, uint32_t ppid, uint32_t stream_id)
{
  return sctp_sendmsg(
      sockfd, buf, nbytes, (struct sockaddr*)&dest_addr, sizeof(dest_addr), htonl(ppid), 0, stream_id, 0, 0);
}

sctp_socket::operator rx_sctp_socket_ref()
{
  return rx_sctp_socket_ref(this);
}

// Private Methods

int sctp_socket::bind_addr(const char* bind_addr_str, int port)
{
  if (sockfd < 0) {
    if (create_socket()) {
      return -1;
    }
  }

  addr_in.sin_family = AF_INET;
  if (inet_pton(AF_INET, bind_addr_str, &(addr_in.sin_addr)) != 1) {
    perror("inet_pton");
    return -1;
  }
  addr_in.sin_port = (port != 0) ? htons(port) : 0;
  if (bind(sockfd, (struct sockaddr*)&addr_in, sizeof(addr_in))) {
    perror("bind()");
    return -1;
  }
  return 0;
}

int sctp_socket::create_socket()
{
  sockfd = socket(AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP);
  if (sockfd == -1) {
    perror("Could not create SCTP socket\n");
    return -1;
  }
  return 0;
}

/***************************************************************
 *                 Rx Multisocket Handler
 **************************************************************/

rx_multisocket_handler::rx_multisocket_handler(std::string name_, srslte::log* log_) :
  thread(name_),
  name(std::move(name_)),
  log_h(log_)
{
  // register control pipe fd
  if (pipe(pipefd) == -1) {
    rxSockInfo("Failed to open control pipe\n");
    return;
  }
  start(THREAD_PRIO);
}

rx_multisocket_handler::~rx_multisocket_handler()
{
  if (running) {
    std::lock_guard<std::mutex> lock(socket_mutex);
    ctrl_cmd_t                  msg{};
    msg.cmd = ctrl_cmd_t::cmd_id_t::EXIT;
    rxSockDebug("Closing socket handler\n");
    if (write(pipefd[1], &msg, sizeof(msg)) != sizeof(msg)) {
      rxSockError("while writing to control pipe\n");
    }
  }

  // close thread
  wait_thread_finish();

  close(pipefd[0]);
  close(pipefd[1]);

  // close all sockets
  for (auto& handler_pair : active_sctp_sockets) {
    if (close(handler_pair.first) == -1) {
      rxSockError("Failed to close socket fd=%d\n", handler_pair.first);
    }
  }

  rxSockDebug("closed.\n");
}

bool rx_multisocket_handler::register_sctp_socket(rx_sctp_socket_ref sock, callback_t recv_handler_)
{
  std::lock_guard<std::mutex> lock(socket_mutex);
  if (sock.fd() < 0) {
    rxSockError("Provided SCTP socket must be already open\n");
    return false;
  }
  if (active_sctp_sockets.count(sock.fd()) > 0) {
    rxSockError("Tried to register fd=%d, but this fd already exists\n", sock.fd());
    return false;
  }

  active_sctp_sockets.insert(std::make_pair(sock.fd(), sctp_handler_t{std::move(recv_handler_), sock}));

  // this unlocks the reading thread to add new connections
  ctrl_cmd_t msg;
  msg.cmd    = ctrl_cmd_t::cmd_id_t::NEW_FD;
  msg.new_fd = sock.fd();
  if (write(pipefd[1], &msg, sizeof(msg)) != sizeof(msg)) {
    rxSockError("while writing to control pipe\n");
  }

  rxSockDebug("socket fd=%d has been registered.\n", sock.fd());
  return true;
}

void rx_multisocket_handler::run_thread()
{
  running = true;
  fd_set total_fd_set, read_fd_set;
  FD_ZERO(&total_fd_set);
  int max_fd = 0;

  FD_SET(pipefd[0], &total_fd_set);
  max_fd = std::max(pipefd[0], max_fd);

  while (running) {
    memcpy(&read_fd_set, &total_fd_set, sizeof(total_fd_set));
    int n = select(max_fd + 1, &read_fd_set, nullptr, nullptr, nullptr);

    // handle select return
    if (n == -1) {
      rxSockError("Error from select()");
      continue;
    }
    if (n == 0) {
      rxSockDebug("No data from select.\n");
      continue;
    }

    std::lock_guard<std::mutex> lock(socket_mutex);

    // call read callback for all SCTP connections
    for (auto& handler_pair : active_sctp_sockets) {
      if (not FD_ISSET(handler_pair.first, &read_fd_set)) {
        continue;
      }
      handler_pair.second.callback(handler_pair.second.sctp_ptr);
    }

    // TODO: For TCP and UDP

    // handle ctrl messages
    if (FD_ISSET(pipefd[0], &read_fd_set)) {
      ctrl_cmd_t msg;
      ssize_t    nrd = read(pipefd[0], &msg, sizeof(msg));
      if (nrd <= 0) {
        rxSockError("Unable to read control message.\n");
        continue;
      }
      switch (msg.cmd) {
        case ctrl_cmd_t::cmd_id_t::EXIT:
          running = false;
          return;
        case ctrl_cmd_t::cmd_id_t::NEW_FD:
          if (msg.new_fd >= 0) {
            FD_SET(msg.new_fd, &total_fd_set);
            max_fd = std::max(max_fd, msg.new_fd);
          } else {
            rxSockError("added fd is not valid\n");
          }
          break;
        default:
          rxSockError("ctrl message command %d is not valid\n", (int)msg.cmd);
      }
    }
  }
}

} // namespace srslte
