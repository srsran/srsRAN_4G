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

#ifndef SRSUE_NAS_EXTIF_H
#define SRSUE_NAS_EXTIF_H

#include "srslte/common/common.h"
#include "srslte/common/log.h"
#include "srslte/common/threads.h"

using srslte::byte_buffer_t;

namespace srsue {

// Abstract class for an external interface
class nas_extif_base : public thread
{
public:
  using recv_cb_t = std::function<void(const srslte::byte_buffer_t&)>;

  nas_extif_base(srslte::log* log_, recv_cb_t recv_cb_) :
    nas_log(log_), recv_cb(std::move(recv_cb_)), thread("EXTIF"){};

  // Interface for nas_ext
  virtual void close(void)                             = 0;
  virtual int  write(const srslte::byte_buffer_t& pdu) = 0;

protected:
  static const int IFACE_THREAD_PRIO = 65;
  virtual void     run_thread()      = 0;
  virtual void     stop()            = 0;
  bool             running           = false;

  srslte::log* nas_log = nullptr;
  recv_cb_t    recv_cb;
};

} // namespace srsue

#endif // SRSUE_NAS_EXTIF_H
