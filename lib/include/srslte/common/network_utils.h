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

#ifndef SRSLTE_RX_SOCKET_HANDLER_H
#define SRSLTE_RX_SOCKET_HANDLER_H

#include "srslte/common/buffer_pool.h"
#include "srslte/common/threads.h"

#include <functional>
#include <map>
#include <mutex>
#include <netinet/sctp.h>
#include <queue>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h> // for the pipe

namespace srslte {

class rx_sctp_socket_ref;

/**
 * @brief handles the lifetime of a SCTP socket and provides convenience methods for listening/connecting, and read/send
 */
class sctp_socket
{
public:
  sctp_socket();
  sctp_socket(sctp_socket&&) noexcept;
  sctp_socket(const sctp_socket&) = delete;
  ~sctp_socket();
  sctp_socket& operator=(sctp_socket&&) noexcept;
  sctp_socket& operator=(const sctp_socket&) = delete;

  void reset();
  int  listen_addr(const char* bind_addr_str, int port);
  int  connect_addr(const char* bind_addr_str, const char* dest_addr_str, int dest_port);

  int read(void*                   buf,
           ssize_t                 nbytes,
           struct sockaddr_in*     from      = nullptr,
           socklen_t               fromlen   = sizeof(sockaddr_in),
           struct sctp_sndrcvinfo* sinfo     = nullptr,
           int                     msg_flags = 0);
  int send(void* buf, ssize_t nbytes, uint32_t ppid, uint32_t stream_id);

  const struct sockaddr_in& get_sockaddr_in() const { return addr_in; }
  int                       fd() const { return sockfd; }
                            operator rx_sctp_socket_ref(); ///< cast to rx_sctp_socket_ref is safe

private:
  int create_socket();
  int bind_addr(const char* bind_addr_str, int port = 0);

  int                sockfd = -1;
  struct sockaddr_in addr_in;
  struct sockaddr_in dest_addr;
};

/**
 * @brief The rx_sctp_socket_ref class is a safe inteface/handler for receiving SCTP packets
 *        it basically forbids the user from trying to reset the socket while it is still
 *        registered to the rx_multisocket_handler for instance.
 */
class rx_sctp_socket_ref
{
public:
  rx_sctp_socket_ref(sctp_socket* sock_) : sock(sock_) {}
  int read(void*                   buf,
           ssize_t                 nbytes,
           struct sockaddr_in*     from      = nullptr,
           socklen_t               fromlen   = sizeof(sockaddr_in),
           struct sctp_sndrcvinfo* sinfo     = nullptr,
           int                     msg_flags = 0)
  {
    return sock->read(buf, nbytes, from, fromlen, sinfo, msg_flags);
  }
  int fd() const { return sock->fd(); }

private:
  sctp_socket* sock = nullptr;
};

class rx_multisocket_handler final : public thread
{
public:
  using callback_t = std::function<void(rx_sctp_socket_ref)>;

  rx_multisocket_handler(std::string name_, srslte::log* log_);
  rx_multisocket_handler(rx_multisocket_handler&&)      = delete;
  rx_multisocket_handler(const rx_multisocket_handler&) = delete;
  rx_multisocket_handler& operator=(const rx_multisocket_handler&) = delete;
  rx_multisocket_handler& operator=(const rx_multisocket_handler&&) = delete;
  ~rx_multisocket_handler();

  bool register_sctp_socket(rx_sctp_socket_ref sock, callback_t recv_handler_);

  void run_thread() override;

private:
  const static int THREAD_PRIO = 65;
  // used to unlock select
  struct ctrl_cmd_t {
    enum class cmd_id_t { EXIT, NEW_FD };
    cmd_id_t cmd    = cmd_id_t::EXIT;
    int      new_fd = -1;
  };
  struct sctp_handler_t {
    callback_t         callback;
    rx_sctp_socket_ref sctp_ptr;
  };
  // args
  std::string  name;
  srslte::log* log_h = nullptr;

  // state
  std::mutex                    socket_mutex;
  std::map<int, sctp_handler_t> active_sctp_sockets;
  bool                          running   = false;
  int                           pipefd[2] = {};
};

} // namespace srslte

#endif // SRSLTE_RX_SOCKET_HANDLER_H
