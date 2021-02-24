/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#include "srsue/hdr/stack/mac_nr/mux_nr.h"
#include "srslte/common/buffer_pool.h"

namespace srsue {

mux_nr::mux_nr(srslog::basic_logger& logger_) : logger(logger_)
{
  msg3_buff = srslte::make_byte_buffer();
};

void mux_nr::init() {}

void mux_nr::msg3_flush()
{
  msg3_buff->clear();
  msg3_state = msg3_state_t::none;
}

void mux_nr::msg3_prepare()
{
  msg3_state = msg3_state_t::pending;
}

void mux_nr::msg3_transmitted()
{
  msg3_state = msg3_state_t::transmitted;
}

bool mux_nr::msg3_is_transmitted()
{
  return msg3_state == msg3_state_t::transmitted;
}

bool mux_nr::msg3_is_pending()
{
  return msg3_state == msg3_state_t::pending;
}

bool mux_nr::msg3_is_empty()
{
  return msg3_buff->N_bytes == 0;
}

} // namespace srsue
