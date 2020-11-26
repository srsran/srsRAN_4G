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

std::unique_ptr<byte_buffer_pool> byte_buffer_pool::instance;
static pthread_mutex_t instance_mutex = PTHREAD_MUTEX_INITIALIZER;

byte_buffer_pool* byte_buffer_pool::get_instance(int capacity)
{
  pthread_mutex_lock(&instance_mutex);
  if (!instance) {
    instance = std::unique_ptr<byte_buffer_pool>(new byte_buffer_pool(capacity));
  }
  pthread_mutex_unlock(&instance_mutex);
  return instance.get();
}

void byte_buffer_pool::cleanup()
{
  pthread_mutex_lock(&instance_mutex);
  if (instance) {
    instance.reset();
  }
  pthread_mutex_unlock(&instance_mutex);
}

} // namespace srslte
