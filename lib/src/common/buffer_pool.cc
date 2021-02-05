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

#include "srslte/common/buffer_pool.h"
#include <pthread.h>
#include <stdio.h>
#include <string>

namespace srslte {

byte_buffer_pool* byte_buffer_pool::get_instance(int capacity)
{
  static std::unique_ptr<byte_buffer_pool> instance(new byte_buffer_pool(capacity));
  return instance.get();
}

} // namespace srslte
