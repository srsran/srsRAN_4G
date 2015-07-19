/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
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


#include <pthread.h>
#include <stdint.h>
#include <string>
#include <queue>
#include "srsapps/common/threads.h"

/******************************************************************************
 *  File:         task_dispatcher.h
 *
 *  Description:  Implements a pool of threads. Pending tasks to execute are 
 *                identified by a pointer. 
 *
 *  Reference:
 *****************************************************************************/

#ifndef THREAD_POOL_H
#define THREAD_POOL_H

namespace srslte {
  
class task_dispatcher : public thread
{
public:
  task_dispatcher(uint32_t max_pending_tasks);
  ~task_dispatcher();
  void push_task(uint32_t task_code);
  virtual void run_task(uint32_t task_code) = 0; 
private:
  std::queue<uint32_t> pending_tasks;    
  void run_thread();  
  pthread_mutex_t mutex; 
  pthread_cond_t cvar; 
  bool running;
};
}
  
#endif