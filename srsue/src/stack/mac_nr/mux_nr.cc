/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
