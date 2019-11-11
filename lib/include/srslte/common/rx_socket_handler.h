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

class rx_socket_handler final : public thread
{
public:
  struct sctp_packet_t {
    ssize_t                      rd_sz = 0;
    srslte::unique_byte_buffer_t buf;
    struct sockaddr_in           from;
    socklen_t                    fromlen = sizeof(from);
    struct sctp_sndrcvinfo       sinfo;
    int                          msg_flags = 0;
    sctp_packet_t() : from{}, sinfo{} {}
  };
  using sctp_callback_t = std::function<void(sctp_packet_t&&)>;

  rx_socket_handler(std::string name_, srslte::log* log_) : thread(name_), log_h(log_), name(std::move(name_))
  {
    pool = byte_buffer_pool::get_instance();

    // register control pipe fd
    if (pipe(pipefd) == -1) {
      log_h->error("%s: Failed to open control pipe\n", name.c_str());
      return;
    }
    start(THREAD_PRIO);
  }
  rx_socket_handler(rx_socket_handler&&)      = delete;
  rx_socket_handler(const rx_socket_handler&) = delete;
  rx_socket_handler& operator=(const rx_socket_handler&) = delete;
  rx_socket_handler& operator=(const rx_socket_handler&&) = delete;

  ~rx_socket_handler()
  {
    if (running) {
      std::lock_guard<std::mutex> lock(socket_mutex);
      ctrl_msg_t                  msg{};
      msg.cmd = ctrl_msg_t::cmd_t::EXIT;
      log_h->debug("%s: Closing socket handler\n", name.c_str());
      if (write(pipefd[1], &msg, sizeof(msg)) != sizeof(msg)) {
        log_h->error("%s: while writing to control pipe\n", name.c_str());
      }
    }

    // close thread
    wait_thread_finish();

    close(pipefd[0]);
    close(pipefd[1]);

    // close all sockets
    for (auto& handler_pair : active_sctp_sockets) {
      if (close(handler_pair.first) == -1) {
        log_h->error("Failed to close socket fd=%d\n", handler_pair.first);
      }
    }

    log_h->debug("%s: closed.\n", name.c_str());
  }

  void register_sctp_socket(int fd_, sctp_callback_t recv_handler_)
  {
    std::lock_guard<std::mutex> lock(socket_mutex);
    if (active_sctp_sockets.count(fd_) > 0) {
      log_h->error("SOCKET: Tried to register fd=%d, but this fd already exists\n", fd_);
      return;
    }
    if (fd_ < 0) {
      log_h->error("%s: Provided fd=%d can\'t be negative\n", name.c_str(), fd_);
      return;
    }

    active_sctp_sockets.insert(std::make_pair(fd_, std::move(recv_handler_)));

    ctrl_msg_t msg;
    msg.cmd    = ctrl_msg_t::cmd_t::NEW_FD;
    msg.new_fd = fd_;
    if (write(pipefd[1], &msg, sizeof(msg)) != sizeof(msg)) {
      log_h->error("%s: while writing to control pipe\n", name.c_str());
    }

    log_h->debug("%s: socket fd=%d has been registered.\n", name.c_str(), fd_);
  }

  void run_thread() override
  {
    srslte::unique_byte_buffer_t pdu = srslte::allocate_unique_buffer(*pool, true);
    const uint32_t               sz  = pdu->get_tailroom();
    running                          = true;
    fd_set total_fd_set, read_fd_set;
    FD_ZERO(&total_fd_set);
    int max_fd = 0;

    FD_SET(pipefd[0], &total_fd_set);
    max_fd = std::max(pipefd[0], max_fd);

    while (running) {
      memcpy(&read_fd_set, &total_fd_set, sizeof(total_fd_set));
      int n = select(max_fd + 1, &read_fd_set, nullptr, nullptr, nullptr);

      std::lock_guard<std::mutex> lock(socket_mutex);

      // handle select return
      if (n == -1) {
        log_h->error("%s: Error from select", name.c_str());
        continue;
      }
      if (n == 0) {
        log_h->debug("%s: No data from select.\n", name.c_str());
        continue;
      }

      // handle sctp messages
      for (auto& handler_pair : active_sctp_sockets) {
        if (not FD_ISSET(handler_pair.first, &read_fd_set)) {
          continue;
        }
        sctp_packet_t packet;
        packet.rd_sz = sctp_recvmsg(handler_pair.first,
                                    pdu->msg,
                                    sz,
                                    (struct sockaddr*)&packet.from,
                                    &packet.fromlen,
                                    &packet.sinfo,
                                    &packet.msg_flags);
        if (packet.rd_sz > 0) {
          pdu->N_bytes = static_cast<uint32_t>(packet.rd_sz);
          packet.buf   = std::move(pdu);
          pdu          = srslte::allocate_unique_buffer(*pool, true);
          handler_pair.second(std::move(packet));
        } else {
          log_h->error("%s: Unable to read from sctp socket fd=%d\n", name.c_str(), handler_pair.first);
          perror(name.c_str());
        }
      }

      // TODO: For UDP as well

      // handle ctrl messages
      if (FD_ISSET(pipefd[0], &read_fd_set)) {
        ctrl_msg_t msg;
        ssize_t    nrd = read(pipefd[0], &msg, sizeof(msg));
        if (nrd < 0) {
          log_h->error("%s: unable to read control message.\n", name.c_str());
          continue;
        }
        switch (msg.cmd) {
          case ctrl_msg_t::cmd_t::EXIT:
            running = false;
            return;
          case ctrl_msg_t::cmd_t::NEW_FD:
            if (msg.new_fd >= 0) {
              FD_SET(msg.new_fd, &total_fd_set);
              max_fd = std::max(max_fd, msg.new_fd);
            } else {
              log_h->error("%s: added fd is not valid\n", name.c_str());
            }
            break;
          default:
            log_h->error("%s: ctrl message command %d is not valid\n", name.c_str(), (int)msg.cmd);
        }
      }
    }
  }

private:
  const static int               THREAD_PRIO = 65;
  std::string                    name;
  srslte::log*                   log_h = nullptr;
  srslte::byte_buffer_pool*      pool  = nullptr;
  std::mutex                     socket_mutex;
  std::map<int, sctp_callback_t> active_sctp_sockets;
  bool                           running   = false;
  int                            pipefd[2] = {};
  struct ctrl_msg_t {
    enum class cmd_t { EXIT, NEW_FD };
    cmd_t cmd    = cmd_t::EXIT;
    int   new_fd = -1;
  };
};

} // namespace srslte

#endif // SRSLTE_RX_SOCKET_HANDLER_H
