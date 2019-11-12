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

class rx_sctp_socket_ref_t;

/**
 * Description: Class created for code reuse by different sockets
 */
class base_socket_t
{
public:
  base_socket_t()                     = default;
  base_socket_t(const base_socket_t&) = delete;
  base_socket_t(base_socket_t&& other) noexcept;
  virtual ~base_socket_t();
  base_socket_t& operator=(const base_socket_t&) = delete;
  base_socket_t& operator                        =(base_socket_t&&) noexcept;

  bool is_init() const { return sockfd >= 0; }
  int  fd() const { return sockfd; }

  // generic read/write interface
  virtual int read(void* buf, size_t nbytes) const = 0;
  virtual int send(void* buf, size_t nbytes) const = 0;

protected:
  void        reset_();
  int         bind_addr(const char* bind_addr_str, int port);
  virtual int create_socket() = 0;
  int         connect_to(struct sockaddr_in* dest_addr, const char* dest_addr_str, int dest_port);

  int                sockfd  = -1;
  struct sockaddr_in addr_in = {};
};

/**
 * Description: handles the lifetime of a SCTP socket and provides convenience methods for listening/connecting, and
 * read/send
 */
class sctp_socket_t final : public base_socket_t
{
public:
  void reset();
  int  listen_addr(const char* bind_addr_str, int port);
  int  connect_addr(const char* bind_addr_str, const char* dest_addr_str, int dest_port);

  int read_from(void*                   buf,
                size_t                  nbytes,
                struct sockaddr_in*     from      = nullptr,
                socklen_t*              fromlen   = nullptr,
                struct sctp_sndrcvinfo* sinfo     = nullptr,
                int                     msg_flags = 0) const;
  int send(void* buf, size_t nbytes, uint32_t ppid, uint32_t stream_id) const;

  int read(void* buf, size_t nbytes) const override { return read_from(buf, nbytes, nullptr, nullptr, nullptr, 0); }
  int send(void* buf, size_t nbytes) const override
  {
    printf("SCTP interface send is invalid\n");
    return -1;
  }

private:
  int create_socket() override;

  struct sockaddr_in dest_addr = {};
};

class tcp_socket_t final : public base_socket_t
{
public:
  void reset();
  int  listen_addr(const char* bind_addr_str, int port);
  int  accept_connection();
  int  connect_addr(const char* bind_addr_str, const char* dest_addr_str, int dest_port);

  int read(void* buf, size_t nbytes) const override;
  int send(void* buf, size_t nbytes) const override;

private:
  int create_socket() override;

  struct sockaddr_in dest_addr = {};
  int                connfd    = -1;
};

class rx_multisocket_handler final : public thread
{
public:
  using sctp_callback_t = std::function<void(const sctp_socket_t&)>;
  using tcp_callback_t  = std::function<void(const tcp_socket_t&)>;

  rx_multisocket_handler(std::string name_, srslte::log* log_);
  rx_multisocket_handler(rx_multisocket_handler&&)      = delete;
  rx_multisocket_handler(const rx_multisocket_handler&) = delete;
  rx_multisocket_handler& operator=(const rx_multisocket_handler&) = delete;
  rx_multisocket_handler& operator=(const rx_multisocket_handler&&) = delete;
  ~rx_multisocket_handler();

  template <typename Sock, typename Handler>
  bool register_socket(const Sock& s, Handler&& handler)
  {
    auto func = [&s, handler]() { handler(s); };
    return register_socket_(std::pair<const int, std::function<void()> >(s.fd(), func));
  }
  //  bool register_sctp_socket(const sctp_socket_t& sock, const sctp_callback_t& recv_handler_);
  //  bool register_tcp_socket(const tcp_socket_t& sock, const tcp_callback_t& recv_handler_);

  void run_thread() override;

private:
  const static int THREAD_PRIO = 65;
  // used to unlock select
  struct ctrl_cmd_t {
    enum class cmd_id_t { EXIT, NEW_FD };
    cmd_id_t cmd    = cmd_id_t::EXIT;
    int      new_fd = -1;
  };

  bool register_socket_(std::pair<const int, std::function<void()> >&& elem);

  // args
  std::string  name;
  srslte::log* log_h = nullptr;

  // state
  std::mutex                            socket_mutex;
  std::map<int, std::function<void()> > active_sockets;
  bool                                  running   = false;
  int                                   pipefd[2] = {};
};

} // namespace srslte

#endif // SRSLTE_RX_SOCKET_HANDLER_H
