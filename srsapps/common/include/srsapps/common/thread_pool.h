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


#include <stdint.h>
#include <string>
#include <vector>
#include <stack>

#include "srsapps/common/threads.h"

/******************************************************************************
 *  File:         thread_pool.h
 *
 *  Description:  Implements a pool of threads. Pending tasks to execute are 
 *                identified by a pointer. 
 *
 *  Reference:
 *****************************************************************************/

#ifndef THREAD_POOL_H
#define THREAD_POOL_H

namespace srslte {

class thread_pool
{
public:
  
  class worker : public thread
  {
  public:
    void setup(uint32_t id, thread_pool *parent, uint32_t prio=0);
    void stop();
    uint32_t get_id();
  protected: 
    virtual void work_imp() = 0;
  private: 
    uint32_t my_id; 
    thread_pool *my_parent;
    bool running; 
    void run_thread();  
    void wait_to_start();
    void finished();
  };
    
  
  thread_pool(uint32_t nof_workers);  
  void    init_worker(uint32_t id, worker*, uint32_t prio = 0);              
  void    stop();
  worker* wait_worker();              
  void    start_worker(worker*);              
  void    start_worker(uint32_t id);              
  worker* get_worker(uint32_t id);
  uint32_t get_nof_workers();
  

private:
  std::vector<worker*> workers; 
  std::vector<bool> begin;
  std::stack<worker*> available_workers;
  pthread_cond_t cvar_start, cvar_stop;
  pthread_mutex_t mutex_start, mutex_stop;
  uint32_t nof_workers; 
  bool running;
};
}
  
#endif