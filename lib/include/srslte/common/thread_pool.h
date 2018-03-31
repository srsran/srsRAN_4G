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

/******************************************************************************
 *  File:         thread_pool.h
 *  Description:  Implements a pool of threads. Pending tasks to execute are 
 *                identified by a pointer. 
 *  Reference:
 *****************************************************************************/

#ifndef SRSLTE_THREAD_POOL_H
#define SRSLTE_THREAD_POOL_H

#include <stdint.h>
#include <string>
#include <vector>
#include <stack>

#include "srslte/common/threads.h"

namespace srslte {

class thread_pool
{
public:
  
  class worker : public thread
  {
  public:
    void setup(uint32_t id, thread_pool *parent, uint32_t prio=0, uint32_t mask = 255);
    void stop();
    uint32_t get_id();
    void release();
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
  void    init_worker(uint32_t id, worker*, uint32_t prio = 0, uint32_t mask = 255);              
  void    stop();
  worker* wait_worker();              
  worker* wait_worker(uint32_t tti);              
  worker* wait_worker_nb(uint32_t tti);
  void    start_worker(worker*);              
  void    start_worker(uint32_t id);              
  worker* get_worker(uint32_t id);
  uint32_t get_nof_workers();
  

private:

  bool find_finished_worker(uint32_t tti, uint32_t *id);
  
  typedef enum {
    IDLE, 
    START_WORK,
    WORKER_READY,
    WORKING
  }worker_status;
  
  std::vector<worker*> workers; 
  uint32_t nof_workers;
  uint32_t max_workers; 
  bool running;
  pthread_cond_t cvar_queue;
  pthread_mutex_t mutex_queue;
  std::vector<worker_status> status;
  std::vector<pthread_cond_t> cvar;
  std::vector<pthread_mutex_t> mutex;
  std::stack<worker*> available_workers;
};
}
  
#endif // SRSLTE_THREAD_POOL_H
