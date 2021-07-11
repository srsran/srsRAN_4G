/**
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSRAN_TASK_SCHEDULER_H
#define SRSRAN_TASK_SCHEDULER_H

#include "block_queue.h"
#include "interfaces_common.h"
#include "multiqueue.h"
#include "thread_pool.h"
#include "timers.h"

namespace srsran {

class task_scheduler
{
public:
  explicit task_scheduler(uint32_t default_extern_tasks_size = 512, uint32_t nof_timers_prealloc = 100) :
    external_tasks{default_extern_tasks_size}, timers{nof_timers_prealloc}, internal_tasks(512)
  {
    background_queue = external_tasks.add_queue();
  }
  task_scheduler(const task_scheduler&) = delete;
  task_scheduler(task_scheduler&&)      = delete;
  task_scheduler& operator=(const task_scheduler&) = delete;
  task_scheduler& operator=(task_scheduler&&) = delete;

  void stop() { external_tasks.stop(); }

  srsran::unique_timer get_unique_timer() { return timers.get_unique_timer(); }

  //! Creates new queue for tasks coming from external thread
  srsran::task_queue_handle make_task_queue() { return external_tasks.add_queue(); }
  srsran::task_queue_handle make_task_queue(uint32_t qsize) { return external_tasks.add_queue(qsize); }

  //! Delays a task processing by duration_ms
  template <typename F>
  void defer_callback(uint32_t duration_ms, F&& func)
  {
    timers.defer_callback(duration_ms, std::forward<F>(func));
  }

  //! Enqueues internal task to be run in next tic
  void defer_task(srsran::move_task_t func)
  {
    if (not internal_tasks.try_push(std::move(func))) {
      srslog::fetch_basic_logger("COMN", false).warning("Couldn't add new internal task");
    }
  }

  //! Defer the handling of the result of a background task to next tic
  void notify_background_task_result(srsran::move_task_t task)
  {
    // run the notification in next tic
    background_queue.push(std::move(task));
  }

  //! Updates timers, and run any pending internal tasks.
  //  CAUTION: Should be called in main thread
  void tic() { timers.step_all(); }

  //! Processes the next task in the multiqueue.
  //  CAUTION: This is a blocking call
  bool run_next_task()
  {
    srsran::move_task_t task{};
    if (external_tasks.wait_pop(&task)) {
      task();
      run_all_internal_tasks();
      return true;
    }
    run_all_internal_tasks();
    return false;
  }

  //! Processes the next task in the multiqueue if it exists.
  void run_pending_tasks()
  {
    run_all_internal_tasks();
    srsran::move_task_t task{};
    while (external_tasks.try_pop(&task)) {
      task();
      run_all_internal_tasks();
    }
  }

  srsran::timer_handler* get_timer_handler() { return &timers; }

private:
  // Perform pending stack deferred tasks
  void run_all_internal_tasks()
  {
    srsran::move_task_t task{};
    while (internal_tasks.try_pop(task)) {
      task();
    }
  }

  srsran::task_multiqueue   external_tasks;
  srsran::task_queue_handle background_queue; ///< Queue for handling the outcomes of tasks run in the background
  srsran::timer_handler     timers;
  srsran::dyn_blocking_queue<srsran::move_task_t>
      internal_tasks; ///< enqueues stack tasks from within main thread. Avoids locking
};

//! Task scheduler handle given to classes/functions running within the main control thread
class task_sched_handle
{
public:
  task_sched_handle(task_scheduler* sched_) : sched(sched_) {}

  srsran::unique_timer get_unique_timer() { return sched->get_unique_timer(); }
  void                 notify_background_task_result(srsran::move_task_t task)
  {
    sched->notify_background_task_result(std::move(task));
  }
  template <typename F>
  void defer_callback(uint32_t duration_ms, F&& func)
  {
    sched->defer_callback(duration_ms, std::forward<F>(func));
  }
  void                      defer_task(srsran::move_task_t func) { sched->defer_task(std::move(func)); }
  srsran::task_queue_handle make_task_queue() { return sched->make_task_queue(); }

private:
  task_scheduler* sched;
};

//! Task scheduler handle given to classes/functions running outside of main control thread
class ext_task_sched_handle
{
public:
  ext_task_sched_handle(task_scheduler* sched_) : sched(sched_) {}

  srsran::unique_timer get_unique_timer() { return sched->get_unique_timer(); }
  void                 notify_background_task_result(srsran::move_task_t task)
  {
    sched->notify_background_task_result(std::move(task));
  }
  srsran::task_queue_handle make_task_queue() { return sched->make_task_queue(); }
  template <typename F>
  void defer_callback(uint32_t duration_ms, F&& func)
  {
    sched->defer_callback(duration_ms, std::forward<F>(func));
  }

private:
  task_scheduler* sched;
};

} // namespace srsran

#endif // SRSRAN_TASK_SCHEDULER_H
