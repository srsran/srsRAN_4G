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

#ifndef SRSLTE_RX_SOCKET_HANDLER_H
#define SRSLTE_RX_SOCKET_HANDLER_H

#include "srslte/common/buffer_pool.h"
#include "srslte/common/logmap.h"
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

namespace net_utils {

enum class addr_family { ipv4 = AF_INET, ipv6 = AF_INET6 };
enum class socket_type : int { none = -1, datagram = SOCK_DGRAM, stream = SOCK_STREAM, seqpacket = SOCK_SEQPACKET };
enum class protocol_type : int { NONE = -1, SCTP = IPPROTO_SCTP, TCP = IPPROTO_TCP, UDP = IPPROTO_UDP };
enum class ppid_values : uint32_t { S1AP = 18 };
const char* protocol_to_string(protocol_type p);

// Convenience addr functions
bool                   set_sockaddr(sockaddr_in* addr, const char* ip_str, int port);
bool                   set_sockaddr(sockaddr_in6* addr, const char* ip_str, int port);
std::string            get_ip(const sockaddr_in& addr);
std::string            get_ip(const sockaddr_in6& addr);
int                    get_port(const sockaddr_in& addr);
net_utils::socket_type get_addr_family(int fd);

// Convenience socket functions
int  open_socket(net_utils::addr_family ip, net_utils::socket_type socket_type, net_utils::protocol_type protocol);
bool bind_addr(int fd, const sockaddr_in& addr_in);
bool bind_addr(int fd, const char* bind_addr_str, int port, sockaddr_in* addr_result = nullptr);
bool connect_to(int fd, const char* dest_addr_str, int dest_port, sockaddr_in* dest_sockaddr = nullptr);

} // namespace net_utils

/**
 * Description: Net socket class with convenience methods for connecting, binding, and opening socket
 */
class socket_handler_t
{
public:
  socket_handler_t()                        = default;
  socket_handler_t(const socket_handler_t&) = delete;
  socket_handler_t(socket_handler_t&& other) noexcept;
  ~socket_handler_t();
  socket_handler_t& operator=(const socket_handler_t&) = delete;
  socket_handler_t& operator                           =(socket_handler_t&&) noexcept;

  void close();
  void reset();

  bool                   is_init() const { return sockfd >= 0; }
  int                    fd() const { return sockfd; }
  const sockaddr_in&     get_addr_in() const { return addr; }
  std::string            get_ip() const { return net_utils::get_ip(addr); }
  net_utils::socket_type get_family() const { return net_utils::get_addr_family(sockfd); }

  bool bind_addr(const char* bind_addr_str, int port);
  bool connect_to(const char* dest_addr_str, int dest_port, sockaddr_in* dest_sockaddr = nullptr);
  bool open_socket(net_utils::addr_family ip, net_utils::socket_type socket_type, net_utils::protocol_type protocol);

protected:
  sockaddr_in addr   = {};
  int         sockfd = -1;
};

namespace net_utils {

bool sctp_init_client(socket_handler_t* socket, net_utils::socket_type socktype, const char* bind_addr_str);
bool sctp_init_server(socket_handler_t* socket, net_utils::socket_type socktype, const char* bind_addr_str, int port);

// TODO: for TCP and UDP
bool tcp_make_server(socket_handler_t* socket, const char* bind_addr_str, int port, int nof_connections = 1);
int  tcp_accept(socket_handler_t* socket, sockaddr_in* destaddr);
int  tcp_read(int remotefd, void* buf, size_t nbytes);
int  tcp_send(int remotefd, const void* buf, size_t nbytes);

} // namespace net_utils

/****************************
 * Rx multisocket handler
 ***************************/

/**
 * Description - Instantiates a thread that will block waiting for IO from multiple sockets, via a select
 *               The user can register their own (socket fd, data handler) in this class via the
 *               add_socket_handler(fd, task) API or its other variants
 */
class rx_multisocket_handler final : public thread
{
public:
  // polymorphic callback to handle the socket recv
  class recv_task
  {
  public:
    virtual ~recv_task()            = default;
    virtual bool operator()(int fd) = 0; // returns false, if socket needs to be removed
  };
  using task_callback_t     = std::unique_ptr<recv_task>;
  using recvfrom_callback_t = std::function<void(srslte::unique_byte_buffer_t, const sockaddr_in&)>;
  using sctp_recv_callback_t =
      std::function<void(srslte::unique_byte_buffer_t, const sockaddr_in&, const sctp_sndrcvinfo&, int)>;

  rx_multisocket_handler(std::string name_, srslte::log_ref log_, int thread_prio = 65);
  rx_multisocket_handler(rx_multisocket_handler&&)      = delete;
  rx_multisocket_handler(const rx_multisocket_handler&) = delete;
  rx_multisocket_handler& operator=(const rx_multisocket_handler&) = delete;
  rx_multisocket_handler& operator=(rx_multisocket_handler&&) = delete;
  ~rx_multisocket_handler();

  void stop();
  bool remove_socket(int fd);
  bool add_socket_handler(int fd, task_callback_t handler);
  // convenience methods for recv using buffer pool
  bool add_socket_pdu_handler(int fd, recvfrom_callback_t pdu_task);
  bool add_socket_sctp_pdu_handler(int fd, sctp_recv_callback_t task);

  void run_thread() override;

private:
  // used to unlock select
  struct ctrl_cmd_t {
    enum class cmd_id_t { EXIT, NEW_FD, RM_FD };
    cmd_id_t cmd    = cmd_id_t::EXIT;
    int      new_fd = -1;
  };
  bool remove_socket_unprotected(int fd, fd_set* total_fd_set, int* max_fd);

  // args
  std::string               name;
  srslte::log_ref           log_h;
  srslte::byte_buffer_pool* pool = nullptr;

  // state
  std::mutex                     socket_mutex;
  std::map<int, task_callback_t> active_sockets;
  bool                           running   = false;
  int                            pipefd[2] = {};
};

} // namespace srslte

#endif // SRSLTE_RX_SOCKET_HANDLER_H
