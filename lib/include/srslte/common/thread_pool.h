/*
 * Copyright 2013-2020 Software Radio Systems Limited
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

/******************************************************************************
 *  File:         thread_pool.h
 *  Description:  Implements a pool of threads. Pending tasks to execute are
 *                identified by a pointer.
 *  Reference:
 *****************************************************************************/

#ifndef SRSLTE_THREAD_POOL_H
#define SRSLTE_THREAD_POOL_H

#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <stack>
#include <stdint.h>
#include <string>
#include <vector>

#include "srslte/common/threads.h"

namespace srslte {

class thread_pool
{
public:
  class worker : public thread
  {
  public:
    worker();
    ~worker() = default;
    void     setup(uint32_t id, thread_pool* parent, uint32_t prio = 0, uint32_t mask = 255);
    uint32_t get_id();
    void     release();

  protected:
    virtual void work_imp() = 0;

  private:
    uint32_t     my_id     = 0;
    thread_pool* my_parent = nullptr;

    void run_thread();
    void wait_to_start();
    void finished();
  };

  thread_pool(uint32_t nof_workers);
  void     init_worker(uint32_t id, worker*, uint32_t prio = 0, uint32_t mask = 255);
  void     stop();
  worker*  wait_worker_id(uint32_t id);
  worker*  wait_worker(uint32_t tti);
  worker*  wait_worker_nb(uint32_t tti);
  void     start_worker(worker*);
  void     start_worker(uint32_t id);
  worker*  get_worker(uint32_t id);
  uint32_t get_nof_workers();

private:
  bool find_finished_worker(uint32_t tti, uint32_t* id);

  typedef enum { STOP, IDLE, START_WORK, WORKER_READY, WORKING } worker_status;

  std::vector<worker*>                 workers     = {};
  uint32_t                             nof_workers = 0;
  uint32_t                             max_workers = 0;
  bool                                 running     = false;
  std::condition_variable              cvar_queue  = {};
  std::mutex                           mutex_queue = {};
  std::vector<worker_status>           status      = {};
  std::vector<std::condition_variable> cvar_worker = {};
};

class task_thread_pool
{
  using task_t = std::function<void(uint32_t worker_id)>;

public:
  explicit task_thread_pool(uint32_t nof_workers);
  ~task_thread_pool();
  void start(int32_t prio = -1, uint32_t mask = 255);
  void stop();

  void     push_task(const task_t& task);
  void     push_task(task_t&& task);
  uint32_t nof_pending_tasks();

private:
  class worker_t : public thread
  {
  public:
    explicit worker_t(task_thread_pool* parent_, uint32_t id);
    void     stop();
    void     setup(int32_t prio, uint32_t mask);
    bool     is_running() const { return running; }
    uint32_t id() const { return id_; }

    void run_thread() override;

  private:
    bool wait_task(task_t* task);

    task_thread_pool* parent  = nullptr;
    uint32_t          id_     = 0;
    bool              running = false;
  };

  std::queue<task_t>      pending_tasks;
  std::vector<worker_t>   workers;
  std::mutex              queue_mutex;
  std::condition_variable cv_empty;
  bool                    running;
};

} // namespace srslte

#endif // SRSLTE_THREAD_POOL_H
