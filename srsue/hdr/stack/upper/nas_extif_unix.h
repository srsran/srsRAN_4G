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

#ifndef SRSUE_NAS_EXTIF_UNIX_H
#define SRSUE_NAS_EXTIF_UNIX_H

#include <boost/asio.hpp>

#include "srslte/common/block_queue.h"
#include "srslte/common/common.h"
#include "srslte/common/log.h"

#include "srsue/hdr/stack/upper/nas_extif.h"

namespace srsue {

// UNIX domain socket server
class nas_extif_unix : public nas_extif_base
{
public:
  nas_extif_unix(srslte::log* log_, recv_cb_t cb_, const std::string& sock_path_);

  void close(void);
  int  write(const srslte::byte_buffer_t& pdu);

protected:
  void run_thread(void);
  void stop(void);

private:
  std::unique_ptr<boost::asio::local::stream_protocol::acceptor> acc;
  std::unique_ptr<boost::asio::local::stream_protocol::socket>   sock;
  boost::asio::io_context                                        io_ctx;
  std::string                                                    sock_path;

  srslte::block_queue<srslte::byte_buffer_t> tx_queue;
  bool                                       has_connection;
  uint8_t                                    buf[1024];

  void handle_write(void);
  void handle_read(void);
  void accept_conn(void);
};

} // namespace srsue

#endif // SRSUE_NAS_EXTIF_UNIX_H
