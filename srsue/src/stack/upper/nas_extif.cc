/*
 * Copyright 2020 Software Radio Systems Limited
 * Author: Vadim Yanitskiy <axilirator@gmail.com>
 * Sponsored by Positive Technologies
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

#include "srslte/common/common.h"
#include "srslte/common/log.h"

#include "srsue/hdr/stack/upper/nas_extif_unix.h"

using namespace boost::asio::local;

namespace srsue {

nas_extif_unix::nas_extif_unix(srslte::log* log_, recv_cb_t cb_, const std::string& sock_path_) :
  nas_extif_base(log_, cb_), sock_path(sock_path_), has_connection(false)
{
  nas_log->info("Init external NAS interface at '%s'\n", sock_path.c_str());

  // Remove previous binding if present
  unlink(sock_path.c_str());

  // Set up a UNIX domain socket
  stream_protocol::endpoint  ep(sock_path);
  stream_protocol::socket*   sock_ = new stream_protocol::socket(io_ctx);
  stream_protocol::acceptor* acc_  = new stream_protocol::acceptor(io_ctx, ep);

  // Move ownership to this->acc
  std::unique_ptr<stream_protocol::acceptor> acc_ptr(acc_);
  acc = std::move(acc_ptr);

  // Move ownership to this->sock
  std::unique_ptr<stream_protocol::socket> sock_ptr(sock_);
  sock = std::move(sock_ptr);

  // Welcome the first connection
  accept_conn();

  nas_log->info("Starting the server...\n");
  start(IFACE_THREAD_PRIO);
};

void nas_extif_unix::handle_write(void)
{
  srslte::byte_buffer_t pdu;

  if (not tx_queue.try_pop(&pdu))
    return;

  boost::asio::async_write(*sock, boost::asio::buffer(pdu.msg, pdu.N_bytes),
      [this](boost::system::error_code ec, std::size_t len)
      {
        if (!ec) {
          nas_log->info("Tx %zu bytes to external NAS interface\n", len);
        } else {
          nas_log->warning("External NAS write() handler got error (ec=%d)\n", ec.value());
        }

        // Keep writing unless the queue is empty
        handle_write();
      });
}

void nas_extif_unix::handle_read(void)
{
  sock->async_read_some(boost::asio::buffer(buf),
      [this](boost::system::error_code ec, std::size_t len)
      {
        if (!ec) {
          nas_log->info("Rx %zu bytes from external NAS interface\n", len);

          // Invoke the Rx callback
          srslte::byte_buffer_t pdu;
          pdu.append_bytes(buf, len);
          recv_cb(pdu);

          // Keep reading
          handle_read();
        } else {
          nas_log->info("External NAS interface has lost connection\n");
          has_connection = false;
          sock->release();
        }
      });
}

void nas_extif_unix::accept_conn(void)
{
  acc->async_accept(
      [this](boost::system::error_code ec, stream_protocol::socket sock_)
      {
        if (!ec) {
          if (!has_connection) {
            nas_log->info("Accepted connection on external NAS interface\n");
            *sock = std::move(sock_);
            has_connection = true;
            handle_read();
          } else {
            nas_log->warning("NAS interface already has an active connection, rejecting...\n");
            boost::asio::write(sock_, boost::asio::buffer("REJECT"));
          }
        } else {
          nas_log->warning("External NAS connection handler got error (ec=%d)\n", ec.value());
        }

        // Keep waiting for a new connection
        accept_conn();
      });
}

void nas_extif_unix::run_thread(void)
{
  // This is a blocking call
  io_ctx.run();
}

void nas_extif_unix::stop(void)
{
  io_ctx.stop();
  wait_thread_finish();
}

void nas_extif_unix::close(void)
{
  stop();
}

int nas_extif_unix::write(const srslte::byte_buffer_t& pdu)
{
  if (!has_connection) {
    nas_log->error("External NAS entity is not connected\n");
    return -EIO;
  }

  tx_queue.push(pdu);
  handle_write();
  return 0;
}

} // namespace srsue
