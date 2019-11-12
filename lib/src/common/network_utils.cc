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

base_socket_t::base_socket_t(base_socket_t&& other) noexcept
{
  sockfd = other.sockfd;
  memcpy(&addr_in, &other.addr_in, sizeof(addr_in));
  other.sockfd = 0;
  bzero(&other.addr_in, sizeof(other.addr_in));
}
base_socket_t::~base_socket_t()
{
  if (sockfd >= 0) {
    close(sockfd);
  }
}
base_socket_t& base_socket_t::operator=(base_socket_t&& other) noexcept
{
  if (this == &other) {
    return *this;
  }
  memcpy(&addr_in, &other.addr_in, sizeof(addr_in));
  sockfd = other.sockfd;
  bzero(&other.addr_in, sizeof(other.addr_in));
  other.sockfd = 0;
  return *this;
}

void base_socket_t::reset_()
{
  if (sockfd >= 0) {
    close(sockfd);
  }
  addr_in = {};
}

int base_socket_t::bind_addr(const char* bind_addr_str, int port)
{
  if (sockfd < 0) {
    if (create_socket() != 0) {
      return -1;
    }
  }

  addr_in.sin_family = AF_INET;
  addr_in.sin_port   = (port != 0) ? htons(port) : 0;
  if (inet_pton(AF_INET, bind_addr_str, &(addr_in.sin_addr)) != 1) {
    perror("inet_pton");
    return -1;
  }

  if (bind(sockfd, (struct sockaddr*)&addr_in, sizeof(addr_in)) != 0) {
    perror("bind()");
    return -1;
  }
  return 0;
}

int base_socket_t::connect_to(struct sockaddr_in* dest_addr, const char* dest_addr_str, int dest_port)
{
  dest_addr->sin_family = AF_INET;
  dest_addr->sin_port   = htons(dest_port);
  if (inet_pton(AF_INET, dest_addr_str, &(dest_addr->sin_addr)) != 1) {
    perror("inet_pton()");
    return -1;
  }
  if (connect(sockfd, (struct sockaddr*)dest_addr, sizeof(*dest_addr)) == -1) {
    perror("connect()");
    return -1;
  }
  return 0;
}

/***********************************************************************
 *                          SCTP socket
 **********************************************************************/

void sctp_socket_t::reset()
{
  reset_();
  dest_addr = {};
}

int sctp_socket_t::listen_addr(const char* bind_addr_str, int port)
{
  if (sockfd < 0 and create_socket() != 0) {
    reset();
    return SRSLTE_ERROR;
  }

  // Sets the data_io_event to be able to use sendrecv_info
  // Subscribes to the SCTP_SHUTDOWN event, to handle graceful shutdown
  struct sctp_event_subscribe evnts = {};
  evnts.sctp_data_io_event          = 1;
  evnts.sctp_shutdown_event         = 1;
  if (setsockopt(sockfd, IPPROTO_SCTP, SCTP_EVENTS, &evnts, sizeof(evnts)) != 0) {
    perror("setsockopt");
    reset();
    return SRSLTE_ERROR;
  }

  // bind addr
  if (bind_addr(bind_addr_str, port) != 0) {
    reset();
    return SRSLTE_ERROR;
  }

  // Listen for connections
  if (listen(sockfd, SOMAXCONN) != 0) {
    perror("listen");
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

int sctp_socket_t::connect_addr(const char* bind_addr_str, const char* dest_addr_str, int dest_port)
{
  if (sockfd < 0 and bind_addr(bind_addr_str, 0) != 0) {
    reset();
    return SRSLTE_ERROR;
  }

  if (connect_to(&dest_addr, dest_addr_str, dest_port) != 0) {
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

int sctp_socket_t::read_from(void*                   buf,
                             size_t                  nbytes,
                             struct sockaddr_in*     from,
                             socklen_t*              fromlen,
                             struct sctp_sndrcvinfo* sinfo,
                             int                     msg_flags) const
{
  if (fromlen != nullptr) {
    *fromlen = sizeof(*from);
  }
  return sctp_recvmsg(sockfd, buf, nbytes, (struct sockaddr*)from, fromlen, sinfo, &msg_flags);
}

int sctp_socket_t::send(void* buf, size_t nbytes, uint32_t ppid, uint32_t stream_id) const
{
  return sctp_sendmsg(
      sockfd, buf, nbytes, (struct sockaddr*)&dest_addr, sizeof(dest_addr), htonl(ppid), 0, stream_id, 0, 0);
}

// Private Methods

int sctp_socket_t::create_socket()
{
  sockfd = socket(AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP);
  if (sockfd == -1) {
    perror("Could not create SCTP socket\n");
    return -1;
  }
  return 0;
}

/***************************************************************
 *                        TCP Socket
 **************************************************************/

void tcp_socket_t::reset()
{
  reset_();
  dest_addr = {};
  if (connfd >= 0) {
    connfd = -1;
  }
}

int tcp_socket_t::listen_addr(const char* bind_addr_str, int port)
{
  if (sockfd < 0 and bind_addr(bind_addr_str, port) != 0) {
    reset();
    return -1;
  }

  // Listen for connections
  if (listen(sockfd, 1) != 0) {
    perror("listen");
    return -1;
  }

  return 0;
}

int tcp_socket_t::accept_connection()
{
  socklen_t clilen = sizeof(dest_addr);
  connfd           = accept(sockfd, (struct sockaddr*)&dest_addr, &clilen);
  if (connfd < 0) {
    perror("accept");
    return -1;
  }
  return 0;
}

int tcp_socket_t::connect_addr(const char* bind_addr_str, const char* dest_addr_str, int dest_port)
{
  if (sockfd < 0 and bind_addr(bind_addr_str, 0) != 0) {
    return -1;
  }
  return connect_to(&dest_addr, dest_addr_str, dest_port);
}

int tcp_socket_t::create_socket()
{
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    perror("Could not create TCP socket\n");
    return SRSLTE_ERROR;
  }
  return SRSLTE_SUCCESS;
}

int tcp_socket_t::read(void* buf, size_t nbytes) const
{
  int n = ::read(connfd, buf, nbytes);
  if (n == 0) {
    return 0;
  }
  if (n == -1) {
    perror("read");
  }
  return n;
}

int tcp_socket_t::send(void* buf, size_t nbytes) const
{
  // Loop until all bytes are sent
  char* ptr = (char*)buf;
  while (nbytes > 0) {
    ssize_t i = ::send(connfd, ptr, nbytes, 0);
    if (i < 1) {
      perror("Error calling send()\n");
      return SRSLTE_ERROR;
    }
    ptr += i;
    nbytes -= i;
  }
  return SRSLTE_SUCCESS;
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

  rxSockDebug("closed.\n");
}

bool rx_multisocket_handler::register_socket_(std::pair<const int, std::function<void()> >&& elem)
{
  int                         fd = elem.first;
  std::lock_guard<std::mutex> lock(socket_mutex);
  if (fd < 0) {
    rxSockError("Provided SCTP socket must be already open\n");
    return false;
  }
  if (active_sockets.count(fd) > 0) {
    rxSockError("Tried to register fd=%d, but this fd already exists\n", fd);
    return false;
  }

  active_sockets.insert(std::move(elem));

  // this unlocks the reading thread to add new connections
  ctrl_cmd_t msg;
  msg.cmd    = ctrl_cmd_t::cmd_id_t::NEW_FD;
  msg.new_fd = fd;
  if (write(pipefd[1], &msg, sizeof(msg)) != sizeof(msg)) {
    rxSockError("while writing to control pipe\n");
  }

  rxSockDebug("socket fd=%d has been registered.\n", fd);
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

    // call read callback for all SCTP/TCP/UDP connections
    for (auto& handler_pair : active_sockets) {
      if (not FD_ISSET(handler_pair.first, &read_fd_set)) {
        continue;
      }
      handler_pair.second();
    }

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
