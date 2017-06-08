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


#include "srslte/common/task_dispatcher.h"
#include <queue>

namespace srslte {
  
task_dispatcher::task_dispatcher(uint32_t max_pending_tasks)
{
  pthread_cond_init(&cvar, NULL);
  pthread_mutex_init(&mutex, NULL);
}

task_dispatcher::~task_dispatcher()
{
  running = false; 
  pthread_cond_signal(&cvar);
  wait_thread_finish();
  pthread_cond_destroy(&cvar);
  pthread_mutex_destroy(&mutex);
}

void task_dispatcher::push_task(uint32_t task_code)
{
  pthread_mutex_lock(&mutex);
  pending_tasks.push(task_code);
  pthread_cond_signal(&cvar);
  pthread_mutex_unlock(&mutex);
}

void task_dispatcher::run_thread()
{
  running = true; 
  while(running) {
    uint32_t task = 0;
    pthread_mutex_lock(&mutex);    
    while(pending_tasks.empty()) {
      pthread_cond_wait(&cvar, &mutex);
    }    
    task = (uint32_t) pending_tasks.front();
    pending_tasks.pop();
    pthread_mutex_unlock(&mutex);
    if (running) {
      run_task(task);
    }
  }
}


}