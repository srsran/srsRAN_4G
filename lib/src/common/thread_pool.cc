/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#include "srsran/common/thread_pool.h"
#include "srsran/srslog/srslog.h"
#include <assert.h>
#include <chrono>
#include <stdio.h>

#define DEBUG 0
#define debug_thread(fmt, ...)                                                                                         \
  do {                                                                                                                 \
    if (DEBUG)                                                                                                         \
      printf(fmt, __VA_ARGS__);                                                                                        \
  } while (0)

namespace srsran {

thread_pool::worker::worker() : thread("THREAD_POOL_WORKER") {}

void thread_pool::worker::setup(uint32_t id, thread_pool* parent, uint32_t prio, uint32_t mask)
{
  my_id     = id;
  my_parent = parent;

  if (mask == 255) {
    start(prio);
  } else {
    start_cpu_mask(prio, mask);
  }
}

void thread_pool::worker::run_thread()
{
  set_name(my_parent->get_id() + std::string("WORKER") + std::to_string(my_id));
  while (running.load(std::memory_order_relaxed)) {
    wait_to_start();
    if (running.load(std::memory_order_relaxed)) {
      work_imp();
      finished();
    }
  }
}

void thread_pool::worker::stop()
{
  running = false;
}

uint32_t thread_pool::worker::get_id()
{
  return my_id;
}

thread_pool::thread_pool(uint32_t max_workers_, std::string id_) :
  workers(max_workers_), max_workers(max_workers_), status(max_workers_), cvar_worker(max_workers_), id(id_)
{
  for (uint32_t i = 0; i < max_workers; i++) {
    workers[i] = NULL;
    status[i]  = IDLE;
  }
  running     = true;
  nof_workers = 0;
}

void thread_pool::init_worker(uint32_t id, worker* obj, uint32_t prio, uint32_t mask)
{
  std::lock_guard<std::mutex> lock(mutex_queue);
  if (id < max_workers) {
    if (id >= nof_workers) {
      nof_workers = id + 1;
    }
    workers[id] = obj;
    obj->setup(id, this, prio, mask);
    cvar_queue.notify_all();
  }
}

void thread_pool::stop()
{
  {
    std::lock_guard<std::mutex> lock(mutex_queue);

    /* Stop any thread waiting for available worker */
    running = false;

    /* Now stop all workers */
    for (uint32_t i = 0; i < nof_workers; i++) {
      if (workers[i]) {
        debug_thread("stop(): stopping %d\n", i);
        workers[i]->stop();
        status[i] = STOP;
        cvar_worker[i].notify_all();
        cvar_queue.notify_all();
      }
    }
  }

  for (uint32_t i = 0; i < nof_workers; i++) {
    debug_thread("stop(): waiting %d\n", i);
    workers[i]->wait_thread_finish();
    debug_thread("stop(): done %d\n", i);
  }
}

void thread_pool::worker::release()
{
  finished();
}

void thread_pool::worker::wait_to_start()
{
  std::unique_lock<std::mutex> lock(my_parent->mutex_queue);

  debug_thread("wait_to_start() id=%d, status=%d, enter\n", my_id, my_parent->status[my_id]);

  while (my_parent->status[my_id] != START_WORK && my_parent->status[my_id] != STOP) {
    my_parent->cvar_worker[my_id].wait(lock);
  }
  if (my_parent->status[my_id] != STOP) {
    my_parent->status[my_id] = WORKING;
  }

  debug_thread("wait_to_start() id=%d, status=%d, exit\n", my_id, my_parent->status[my_id]);
}

void thread_pool::worker::finished()
{
  std::lock_guard<std::mutex> lock(my_parent->mutex_queue);
  if (my_parent->status[my_id] != STOP) {
    my_parent->status[my_id] = IDLE;
    my_parent->cvar_worker[my_id].notify_all();
    my_parent->cvar_queue.notify_all();
  }
}

bool thread_pool::worker::is_stopped() const
{
  std::lock_guard<std::mutex> lock(my_parent->mutex_queue);
  return my_parent->status[my_id] == STOP;
}

bool thread_pool::find_finished_worker(uint32_t tti, uint32_t* id)
{
  for (uint32_t i = 0; i < nof_workers; i++) {
    if (status[i] == IDLE) {
      *id = i;
      return true;
    }
  }
  return false;
}

thread_pool::worker* thread_pool::wait_worker_id(uint32_t id)
{
  std::unique_lock<std::mutex> lock(mutex_queue);

  debug_thread("wait_worker_id() - enter - id=%d, state0=%d, state1=%d\n", id, status[0], status[1]);

  thread_pool::worker* ret = nullptr;

  while (status[id] != IDLE && running) {
    cvar_queue.wait(lock);
  }
  if (running) {
    ret        = workers[id];
    status[id] = WORKER_READY;
  }
  debug_thread("wait_worker_id() - exit - id=%d\n", id);
  return ret;
}

thread_pool::worker* thread_pool::wait_worker(uint32_t tti)
{
  std::unique_lock<std::mutex> lock(mutex_queue);

  debug_thread("wait_worker() - enter - tti=%d, state0=%d, state1=%d\n", tti, status[0], status[1]);

  thread_pool::worker* ret = nullptr;
  uint32_t             id  = 0;

  while (!find_finished_worker(tti, &id) && running) {
    cvar_queue.wait(lock);
  }
  if (running) {
    ret        = workers[id];
    status[id] = WORKER_READY;
  }
  debug_thread("wait_worker() - exit - id=%d\n", id);
  return ret;
}

thread_pool::worker* thread_pool::wait_worker_nb(uint32_t tti)
{
  std::unique_lock<std::mutex> lock(mutex_queue);

  debug_thread("wait_worker_nb() - enter - tti=%d, state0=%d, state1=%d\n", tti, status[0], status[1]);

  thread_pool::worker* ret = nullptr;
  uint32_t             id  = 0;

  if (find_finished_worker(tti, &id) && running) {
    ret        = workers[id];
    status[id] = WORKER_READY;
  }

  debug_thread("wait_worker_nb() - exit - id=%d\n", id);
  return ret;
}

void thread_pool::start_worker(uint32_t id)
{
  std::unique_lock<std::mutex> lock(mutex_queue);
  if (id < nof_workers) {
    debug_thread("start_worker() id=%d, status=%d\n", id, status[id]);
    if (status[id] != STOP) {
      status[id] = START_WORK;
      cvar_worker[id].notify_all();
      cvar_queue.notify_all();
    }
  }
}

void thread_pool::start_worker(worker* x)
{
  for (uint32_t i = 0; i < nof_workers; i++) {
    if (x == workers[i]) {
      start_worker(i);
      return;
    }
  }
}

thread_pool::worker* thread_pool::get_worker(uint32_t id)
{
  if (id < nof_workers) {
    return workers[id];
  }
  return NULL;
}

uint32_t thread_pool::get_nof_workers()
{
  return nof_workers;
}

std::string thread_pool::get_id()
{
  return id;
}

/**************************************************************************
 *  task_thread_pool - uses a queue to enqueue callables, that start
 *  once a worker is available
 *************************************************************************/

task_thread_pool::task_thread_pool(uint32_t nof_workers, bool start_deferred, int32_t prio_, uint32_t mask_) :
  logger(srslog::fetch_basic_logger("POOL")), pending_tasks(max_task_num), workers(std::max(1u, nof_workers))
{
  if (not start_deferred) {
    start(prio_, mask_);
  }
}

task_thread_pool::~task_thread_pool()
{
  stop();
}

void task_thread_pool::set_nof_workers(uint32_t nof_workers)
{
  std::lock_guard<std::mutex> lock(queue_mutex);
  if (workers.size() > nof_workers) {
    logger.error("Reducing the number of workers dynamically not supported");
    return;
  }
  uint32_t old_size = workers.size();
  workers.resize(nof_workers);
  if (running) {
    for (uint32_t i = old_size; i < nof_workers; ++i) {
      workers[i].reset(new worker_t(this, i));
    }
  }
}

void task_thread_pool::start(int32_t prio_, uint32_t mask_)
{
  std::lock_guard<std::mutex> lock(queue_mutex);
  if (running) {
    logger.error("Starting thread pool that has already started");
    return;
  }
  prio    = prio_;
  mask    = mask_;
  running = true;
  for (uint32_t i = 0; i < workers.size(); ++i) {
    workers[i].reset(new worker_t(this, i));
  }
}

void task_thread_pool::stop()
{
  std::unique_lock<std::mutex> lock(queue_mutex);
  if (running) {
    running              = false;
    bool workers_running = false;
    for (std::unique_ptr<worker_t>& w : workers) {
      if (w->is_running()) {
        workers_running = true;
        break;
      }
    }
    lock.unlock();
    if (workers_running) {
      cv_empty.notify_all();
    }
    for (std::unique_ptr<worker_t>& w : workers) {
      w->stop();
    }
  }
}

void task_thread_pool::push_task(task_t&& task)
{
  {
    std::lock_guard<std::mutex> lock(queue_mutex);
    if (pending_tasks.full()) {
      logger.error("Cannot push anymore tasks into the queue, maximum size is %u", uint32_t(max_task_num));
      return;
    }
    pending_tasks.push(std::move(task));
  }
  cv_empty.notify_one();
}

uint32_t task_thread_pool::nof_pending_tasks() const
{
  std::lock_guard<std::mutex> lock(queue_mutex);
  return pending_tasks.size();
}

task_thread_pool::worker_t::worker_t(srsran::task_thread_pool* parent_, uint32_t my_id) :
  parent(parent_), thread(std::string("TASKWORKER") + std::to_string(my_id)), id_(my_id), running(true)
{
  if (parent->mask == 255) {
    start(parent->prio);
  } else {
    start_cpu_mask(parent->prio, parent->mask);
  }
}

void task_thread_pool::worker_t::stop()
{
  wait_thread_finish();
}

bool task_thread_pool::worker_t::wait_task(task_t* task)
{
  std::unique_lock<std::mutex> lock(parent->queue_mutex);
  while (parent->running and parent->pending_tasks.empty()) {
    parent->cv_empty.wait(lock);
  }
  if (not parent->running) {
    return false;
  }
  if (task) {
    *task = std::move(parent->pending_tasks.top());
  }
  parent->pending_tasks.pop();
  return true;
}

void task_thread_pool::worker_t::run_thread()
{
  // main loop
  task_t task;
  while (wait_task(&task)) {
    task();
  }

  // on exit, notify pool class
  std::unique_lock<std::mutex> lock(parent->queue_mutex);
  running = false;
}

task_worker::task_worker(std::string thread_name_,
                         uint32_t    queue_size,
                         bool        start_deferred,
                         int32_t     prio_,
                         uint32_t    mask_) :
  thread(std::move(thread_name_)),
  prio(prio_),
  mask(mask_),
  pending_tasks(queue_size),
  logger(srslog::fetch_basic_logger("POOL"))
{
  if (not start_deferred) {
    start(prio_, mask_);
  }
}

task_worker::~task_worker()
{
  stop();
}

void task_worker::stop()
{
  if (not pending_tasks.is_stopped()) {
    pending_tasks.stop();
    wait_thread_finish();
  }
}

void task_worker::start(int32_t prio_, uint32_t mask_)
{
  prio = prio_;
  mask = mask_;

  if (mask == 255) {
    thread::start(prio);
  } else {
    thread::start_cpu_mask(prio, mask);
  }
}

void task_worker::push_task(task_t&& task)
{
  auto ret = pending_tasks.try_push(std::move(task));
  if (ret.is_error()) {
    logger.error("Cannot push anymore tasks into the worker queue. maximum size is %u",
                 uint32_t(pending_tasks.max_size()));
    return;
  }
}

uint32_t task_worker::nof_pending_tasks() const
{
  return pending_tasks.size();
}

void task_worker::run_thread()
{
  while (true) {
    bool   success;
    task_t task = pending_tasks.pop_blocking(&success);
    if (not success) {
      break;
    }
    task();
  }
  logger.info("Task worker %s finished.", thread::get_name().c_str());
}

// Global thread pool for long, low-priority tasks
task_thread_pool& get_background_workers()
{
  static task_thread_pool background_workers;
  return background_workers;
}

} // namespace srsran
