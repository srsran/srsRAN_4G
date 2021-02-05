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

#include "srslte/common/byte_buffer.h"
#include "srslte/common/buffer_pool.h"

namespace srslte {

void byte_buffer_deleter::operator()(byte_buffer_t* buf) const
{
  if (buf) {
    pool->deallocate(buf);
  }
}

} // namespace srslte