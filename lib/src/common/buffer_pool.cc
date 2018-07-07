/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */


#include <pthread.h>
#include "srslte/common/buffer_pool.h"
#include <stdio.h>
#include <string>

namespace srslte{

byte_buffer_pool *byte_buffer_pool::instance = NULL;
pthread_mutex_t instance_mutex = PTHREAD_MUTEX_INITIALIZER;

byte_buffer_pool* byte_buffer_pool::get_instance(int capacity)
{
  pthread_mutex_lock(&instance_mutex);
  if(NULL == instance) {
    instance = new byte_buffer_pool(capacity);
  }
  pthread_mutex_unlock(&instance_mutex);
  return instance;
}

void byte_buffer_pool::cleanup(void)
{
  pthread_mutex_lock(&instance_mutex);
  if(NULL != instance)
  {
    delete instance;
    instance = NULL;
  }
  pthread_mutex_unlock(&instance_mutex);
}
  
} // namespace srsue
