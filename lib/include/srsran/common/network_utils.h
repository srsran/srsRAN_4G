/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#ifndef SRSRAN_RX_SOCKET_HANDLER_H
#define SRSRAN_RX_SOCKET_HANDLER_H

#include "srsran/common/buffer_pool.h"
#include "srsran/common/multiqueue.h"
#include "srsran/common/threads.h"

#include <arpa/inet.h>
#include <map>
#include <mutex>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <sys/socket.h>
#include <sys/types.h>

namespace srsran {

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
class unique_socket
{
public:
  unique_socket()                     = default;
  unique_socket(const unique_socket&) = delete;
  unique_socket(unique_socket&& other) noexcept;
  ~unique_socket();
  unique_socket& operator=(const unique_socket&) = delete;
  unique_socket& operator                        =(unique_socket&&) noexcept;

  void close();

  bool                   is_open() const { return sockfd >= 0; }
  int                    fd() const { return sockfd; }
  const sockaddr_in&     get_addr_in() const { return addr; }
  std::string            get_ip() const { return net_utils::get_ip(addr); }
  net_utils::socket_type get_family() const { return net_utils::get_addr_family(sockfd); }

  bool open_socket(net_utils::addr_family ip, net_utils::socket_type socket_type, net_utils::protocol_type protocol);
  bool bind_addr(const char* bind_addr_str, int port);
  bool connect_to(const char* dest_addr_str, int dest_port, sockaddr_in* dest_sockaddr = nullptr);
  bool start_listen();
  bool reuse_addr();
  bool sctp_subscribe_to_events();
  bool sctp_set_rto_opts(int rto_max);
  bool sctp_set_init_msg_opts(int max_init_attempts, int max_init_timeo);
  int  get_socket() const { return sockfd; };

protected:
  sockaddr_in addr   = {};
  int         sockfd = -1;
};

namespace net_utils {

bool sctp_init_socket(unique_socket* socket, net_utils::socket_type socktype, const char* bind_addr_str, int bind_port);

} // namespace net_utils

/****************************
 * Rx multisocket handler
 ***************************/

class socket_manager_itf
{
public:
  /// Callback called when socket fd (passed as argument) has data
  using recv_callback_t = srsran::move_callback<bool(int)>;

  explicit socket_manager_itf(srslog::basic_logger& logger_) : logger(logger_) {}
  socket_manager_itf(socket_manager_itf&&)      = delete;
  socket_manager_itf(const socket_manager_itf&) = delete;
  socket_manager_itf& operator=(const socket_manager_itf&) = delete;
  socket_manager_itf& operator=(socket_manager_itf&&) = delete;
  virtual ~socket_manager_itf()                       = default;

  /// Register (fd, callback). callback is called within socket thread when fd has data.
  virtual bool add_socket_handler(int fd, recv_callback_t handler) = 0;

  /// remove registered socket fd
  virtual bool remove_socket(int fd) = 0;

protected:
  srslog::basic_logger& logger;
};

/**
 * Description - Instantiates a thread that will block waiting for IO from multiple sockets, via a select
 *               The user can register their own (socket fd, data handler) in this class via the
 *               add_socket_handler(fd, task) API or its other variants
 */
class socket_manager final : public thread, public socket_manager_itf
{
  using recv_callback_t = socket_manager_itf::recv_callback_t;

public:
  socket_manager();
  ~socket_manager() final;

  void stop();
  bool remove_socket_nonblocking(int fd, bool signal_completion = false);
  bool remove_socket(int fd) final;
  bool add_socket_handler(int fd, recv_callback_t handler) final;

  void run_thread() override;

private:
  const int thread_prio = 65;

  // used to unlock select
  struct ctrl_cmd_t {
    enum class cmd_id_t { EXIT, NEW_FD, RM_FD };
    cmd_id_t cmd;
    int      new_fd;
    bool     signal_rm_complete;
    ctrl_cmd_t() { bzero(this, sizeof(ctrl_cmd_t)); }
  };
  std::map<int, recv_callback_t>::iterator remove_socket_unprotected(int fd, fd_set* total_fd_set, int* max_fd);

  // state
  std::mutex                     socket_mutex;
  std::map<int, recv_callback_t> active_sockets;
  std::atomic<bool>              running   = {false};
  int                            pipefd[2] = {-1, -1};
  std::vector<int>               rem_fd_tmp_list;
  std::condition_variable        rem_cvar;
};

/// Function signature for SDU byte buffers received from SCTP socket
using sctp_recv_callback_t =
    srsran::move_callback<void(srsran::unique_byte_buffer_t, const sockaddr_in&, const sctp_sndrcvinfo&, int)>;

/// Function signature for SDU byte buffers received from any sockaddr_in-based socket
using recvfrom_callback_t = srsran::move_callback<void(srsran::unique_byte_buffer_t, const sockaddr_in&)>;

/**
 * Helper function that creates a callback that is called when a SCTP socket has data, and does the following tasks:
 * 1. receive SDU byte buffer from SCTP socket and associated metadata - sockaddr_in, sctp_sndrcvinfo, flags
 * 2. dispatches the received SDU+metadata+rx_callback into the "queue"
 * 3. potentially on a separate thread, the SDU+metadata+callback are popped from the queue, and callback is called with
 * the SDU+metadata as arguments
 * @param logger logger used by recv_callback_t to log any failure/reception of an SDU
 * @param queue queue to which the SDU+metadata+callback are going to be dispatched
 * @param rx_callback callback that is run when a new SDU arrives, from the thread that calls queue.pop()
 * @return callback void(int) that can be registered in socket_manager
 */
socket_manager_itf::recv_callback_t
make_sctp_sdu_handler(srslog::basic_logger& logger, srsran::task_queue_handle& queue, sctp_recv_callback_t rx_callback);

/**
 * Similar to make_sctp_sdu_handler, but for any sockaddr_in-based socket type
 */
socket_manager_itf::recv_callback_t
make_sdu_handler(srslog::basic_logger& logger, srsran::task_queue_handle& queue, recvfrom_callback_t rx_callback);

inline socket_manager& get_rx_io_manager()
{
  static socket_manager io;
  return io;
}

} // namespace srsran

#endif // SRSRAN_RX_SOCKET_HANDLER_H
