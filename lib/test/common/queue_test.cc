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

#include "srslte/adt/move_callback.h"
#include "srslte/common/multiqueue.h"
#include "srslte/common/thread_pool.h"
#include <iostream>
#include <thread>
#include <unistd.h>

#define TESTASSERT(cond)                                                                                               \
  {                                                                                                                    \
    if (!(cond)) {                                                                                                     \
      std::cout << "[" << __FUNCTION__ << "][Line " << __LINE__ << "]: FAIL at " << (#cond) << std::endl;              \
      return -1;                                                                                                       \
    }                                                                                                                  \
  }

using namespace srslte;

int test_multiqueue()
{
  std::cout << "\n======= TEST multiqueue test: start =======\n";

  int number = 2;

  multiqueue_handler<int> multiqueue;
  TESTASSERT(multiqueue.nof_queues() == 0)

  // test push/pop and size for one queue
  int qid1 = multiqueue.add_queue();
  TESTASSERT(qid1 == 0 and multiqueue.is_queue_active(qid1))
  TESTASSERT(multiqueue.size(qid1) == 0 and multiqueue.empty(qid1))
  TESTASSERT(multiqueue.nof_queues() == 1)
  TESTASSERT(multiqueue.try_push(qid1, 5).first)
  TESTASSERT(multiqueue.try_push(qid1, number))
  TESTASSERT(multiqueue.size(qid1) == 2 and not multiqueue.empty(qid1))
  TESTASSERT(multiqueue.wait_pop(&number) == qid1)
  TESTASSERT(number == 5)
  TESTASSERT(multiqueue.wait_pop(&number) == qid1)
  TESTASSERT(number == 2 and multiqueue.empty(qid1) and multiqueue.size(qid1) == 0)

  // test push/pop and size for two queues
  int qid2 = multiqueue.add_queue();
  TESTASSERT(qid2 == 1)
  TESTASSERT(multiqueue.nof_queues() == 2 and multiqueue.is_queue_active(qid1))
  TESTASSERT(multiqueue.try_push(qid2, 3).first)
  TESTASSERT(multiqueue.size(qid2) == 1 and not multiqueue.empty(qid2))
  TESTASSERT(multiqueue.empty(qid1) and multiqueue.size(qid1) == 0)

  // check if erasing a queue breaks anything
  multiqueue.erase_queue(qid1);
  TESTASSERT(multiqueue.nof_queues() == 1 and not multiqueue.is_queue_active(qid1))
  qid1 = multiqueue.add_queue();
  TESTASSERT(qid1 == 0)
  TESTASSERT(multiqueue.empty(qid1) and multiqueue.is_queue_active(qid1))
  multiqueue.wait_pop(&number);

  // check round-robin
  for (int i = 0; i < 10; ++i) {
    TESTASSERT(multiqueue.try_push(qid1, i))
  }
  for (int i = 20; i < 35; ++i) {
    TESTASSERT(multiqueue.try_push(qid2, i))
  }
  TESTASSERT(multiqueue.size(qid1) == 10)
  TESTASSERT(multiqueue.size(qid2) == 15)
  TESTASSERT(multiqueue.wait_pop(&number) == qid1 and number == 0)
  TESTASSERT(multiqueue.wait_pop(&number) == qid2 and number == 20)
  TESTASSERT(multiqueue.wait_pop(&number) == qid1 and number == 1)
  TESTASSERT(multiqueue.wait_pop(&number) == qid2 and number == 21)
  TESTASSERT(multiqueue.size(qid1) == 8)
  TESTASSERT(multiqueue.size(qid2) == 13)
  for (int i = 0; i < 8 * 2; ++i) {
    multiqueue.wait_pop(&number);
  }
  TESTASSERT(multiqueue.size(qid1) == 0)
  TESTASSERT(multiqueue.size(qid2) == 5)
  TESTASSERT(multiqueue.wait_pop(&number) == qid2 and number == 30)

  // remove existing queues
  multiqueue.erase_queue(qid1);
  multiqueue.erase_queue(qid2);
  TESTASSERT(multiqueue.nof_queues() == 0)

  // check that adding a queue of different capacity works
  {
    int qid1 = multiqueue.add_queue();
    int qid2 = multiqueue.add_queue();

    // remove first queue again
    multiqueue.erase_queue(qid1);
    TESTASSERT(multiqueue.nof_queues() == 1)

    // add queue with non-default capacity
    int qid3 = multiqueue.add_queue(10);

    // make sure neither a new queue index is returned
    TESTASSERT(qid1 != qid3)
    TESTASSERT(qid2 != qid3)
  }

  std::cout << "outcome: Success\n";
  std::cout << "===========================================\n";

  return 0;
}

int test_multiqueue_threading()
{
  std::cout << "\n===== TEST multiqueue threading test: start =====\n";

  int                     capacity = 4, number = 0, start_number = 2, nof_pushes = capacity + 1;
  multiqueue_handler<int> multiqueue(capacity);
  int                     qid1 = multiqueue.add_queue();
  auto push_blocking_func      = [&multiqueue](int qid, int start_value, int nof_pushes, bool* is_running) {
    for (int i = 0; i < nof_pushes; ++i) {
      multiqueue.push(qid, start_value + i);
      std::cout << "t1: pushed item " << i << std::endl;
    }
    std::cout << "t1: pushed all items\n";
    *is_running = false;
  };

  bool        t1_running = true;
  std::thread t1(push_blocking_func, qid1, start_number, nof_pushes, &t1_running);

  // Wait for queue to fill
  while ((int)multiqueue.size(qid1) != capacity) {
    usleep(1000);
    TESTASSERT(t1_running)
  }

  for (int i = 0; i < nof_pushes; ++i) {
    TESTASSERT(multiqueue.wait_pop(&number) == qid1)
    TESTASSERT(number == start_number + i)
    std::cout << "main: popped item " << i << "\n";
  }
  std::cout << "main: popped all items\n";

  // wait for thread to finish
  while (t1_running) {
    usleep(1000);
  }
  TESTASSERT(multiqueue.size(qid1) == 0)

  multiqueue.reset();
  t1.join();

  std::cout << "outcome: Success\n";
  std::cout << "==================================================\n";

  return 0;
}

int test_multiqueue_threading2()
{
  std::cout << "\n===== TEST multiqueue threading test 2: start =====\n";
  // Description: push items until blocking in thread t1. Unblocks in main thread by calling multiqueue.reset()

  int                     capacity = 4, start_number = 2, nof_pushes = capacity + 1;
  multiqueue_handler<int> multiqueue(capacity);
  int                     qid1 = multiqueue.add_queue();
  auto push_blocking_func      = [&multiqueue](int qid, int start_value, int nof_pushes, bool* is_running) {
    for (int i = 0; i < nof_pushes; ++i) {
      multiqueue.push(qid, start_value + i);
    }
    std::cout << "t1: pushed all items\n";
    *is_running = false;
  };

  bool        t1_running = true;
  std::thread t1(push_blocking_func, qid1, start_number, nof_pushes, &t1_running);

  // Wait for queue to fill
  while ((int)multiqueue.size(qid1) != capacity) {
    usleep(1000);
    TESTASSERT(t1_running)
  }

  multiqueue.reset();
  t1.join();

  std::cout << "outcome: Success\n";
  std::cout << "===================================================\n";

  return 0;
}

int test_multiqueue_threading3()
{
  std::cout << "\n===== TEST multiqueue threading test 3: start =====\n";
  // pop will block in a separate thread, but multiqueue.reset() will unlock it

  int                     capacity = 4;
  multiqueue_handler<int> multiqueue(capacity);
  int                     qid1              = multiqueue.add_queue();
  auto                    pop_blocking_func = [&multiqueue](int qid, bool* success) {
    int number = 0;
    int id     = multiqueue.wait_pop(&number);
    *success   = id < 0;
  };

  bool        t1_success = false;
  std::thread t1(pop_blocking_func, qid1, &t1_success);

  TESTASSERT(not t1_success)
  usleep(1000);
  TESTASSERT(not t1_success)
  TESTASSERT((int)multiqueue.size(qid1) == 0)

  // Should be able to unlock all
  multiqueue.reset();
  t1.join();
  TESTASSERT(t1_success);

  std::cout << "outcome: Success\n";
  std::cout << "===================================================\n";

  return 0;
}

int test_task_thread_pool()
{
  std::cout << "\n====== TEST task thread pool test 1: start ======\n";
  // Description: check whether the tasks are successfully distributed between workers

  uint32_t                nof_workers = 4, nof_runs = 10000;
  std::vector<int>        count_worker(nof_workers, 0);
  std::vector<std::mutex> count_mutex(nof_workers);

  task_thread_pool thread_pool(nof_workers);
  thread_pool.start();

  auto task = [&count_worker, &count_mutex](uint32_t worker_id) {
    std::lock_guard<std::mutex> lock(count_mutex[worker_id]);
    //    std::cout << "hello world from worker " << worker_id << std::endl;
    count_worker[worker_id]++;
  };

  for (uint32_t i = 0; i < nof_runs; ++i) {
    thread_pool.push_task(task);
  }

  // wait for all tasks to be successfully processed
  while (thread_pool.nof_pending_tasks() > 0) {
    usleep(100);
  }

  thread_pool.stop();

  uint32_t total_count = 0;
  for (uint32_t i = 0; i < nof_workers; ++i) {
    if (count_worker[i] < 10) {
      printf("WARNING: the number of tasks %d assigned to worker %d is too low\n", count_worker[i], i);
    }
    total_count += count_worker[i];
    printf("worker %d: %d runs\n", i, count_worker[i]);
  }
  if (total_count != nof_runs) {
    printf("Number of task runs=%d does not match total=%d\n", total_count, nof_runs);
    return -1;
  }

  std::cout << "outcome: Success\n";
  std::cout << "===================================================\n";
  return 0;
}

int test_task_thread_pool2()
{
  std::cout << "\n====== TEST task thread pool test 2: start ======\n";
  // Description: push a very long task to all workers, and call thread_pool.stop() to check if it waits for the tasks
  //              to be completed, and does not get stuck.

  uint32_t   nof_workers     = 4;
  uint8_t    workers_started = 0, workers_finished = 0;
  std::mutex mut;

  task_thread_pool thread_pool(nof_workers);
  thread_pool.start();

  auto task = [&workers_started, &workers_finished, &mut](uint32_t worker_id) {
    {
      std::lock_guard<std::mutex> lock(mut);
      workers_started++;
    }
    sleep(1);
    std::lock_guard<std::mutex> lock(mut);
    std::cout << "worker " << worker_id << " has finished\n";
    workers_finished++;
  };

  for (uint32_t i = 0; i < nof_workers; ++i) {
    thread_pool.push_task(task);
  }

  while (workers_started != nof_workers) {
    usleep(10);
  }

  std::cout << "stopping thread pool...\n";
  thread_pool.stop();
  std::cout << "thread pool stopped.\n";

  TESTASSERT(workers_finished == nof_workers);

  std::cout << "outcome: Success\n";
  std::cout << "===================================================\n";
  return 0;
}

int test_task_thread_pool3()
{
  std::cout << "\n====== TEST task thread pool test 3: start ======\n";
  // Description: create many workers and shut down the pool before all of them started yet. Should exit cleanly

  uint32_t nof_workers = 100;

  task_thread_pool thread_pool(nof_workers);
  thread_pool.start();

  std::cout << "outcome: Success\n";
  std::cout << "===================================================\n";
  return 0;
}

struct C {
  std::unique_ptr<int> val{new int{5}};
};
struct D {
  std::array<int, 64> big_val;
  D() { big_val[0] = 6; }
};

int test_inplace_task()
{
  std::cout << "\n======= TEST inplace task: start =======\n";
  int v = 0;

  auto l0 = [&v]() { v = 1; };

  srslte::move_callback<void()> t{l0};
  srslte::move_callback<void()> t2{[v]() mutable { v = 2; }};
  // sanity static checks
  static_assert(task_details::is_move_callback<std::decay<decltype(t)>::type>::value, "failed check\n");
  static_assert(
      std::is_base_of<std::false_type, task_details::is_move_callback<std::decay<decltype(l0)>::type> >::value,
      "failed check\n");

  t();
  t2();
  TESTASSERT(v == 1);
  v              = 2;
  decltype(t) t3 = std::move(t);
  t3();
  TESTASSERT(v == 1);

  C                             c;
  srslte::move_callback<void()> t4{std::bind([&v](C& c) { v = *c.val; }, std::move(c))};
  {
    decltype(t4) t5;
    t5 = std::move(t4);
    t5();
    TESTASSERT(v == 5);
  }

  D                             d;
  srslte::move_callback<void()> t6 = [&v, d]() { v = d.big_val[0]; };
  {
    srslte::move_callback<void()> t7;
    t6();
    TESTASSERT(v == 6);
    v  = 0;
    t7 = std::move(t6);
    t7();
    TESTASSERT(v == 6);
  }

  auto l1 = std::bind([&v](C& c) { v = *c.val; }, C{});
  auto l2 = [&v, d]() { v = d.big_val[0]; };
  t       = std::move(l1);
  t2      = l2;
  v       = 0;
  t();
  TESTASSERT(v == 5);
  t2();
  TESTASSERT(v == 6);
  TESTASSERT(t.is_in_small_buffer() and not t2.is_in_small_buffer());
  std::swap(t, t2);
  TESTASSERT(t2.is_in_small_buffer() and not t.is_in_small_buffer());
  v = 0;
  t();
  TESTASSERT(v == 6);
  t2();
  TESTASSERT(v == 5);

  // TEST: task works in const contexts
  t       = l2;
  auto l3 = [](const srslte::move_callback<void()>& task) { task(); };
  v       = 0;
  l3(t);
  TESTASSERT(v == 6);

  std::cout << "outcome: Success\n";
  std::cout << "========================================\n";
  return 0;
}

int main()
{
  TESTASSERT(test_multiqueue() == 0);
  TESTASSERT(test_multiqueue_threading() == 0);
  TESTASSERT(test_multiqueue_threading2() == 0);
  TESTASSERT(test_multiqueue_threading3() == 0);

  TESTASSERT(test_task_thread_pool() == 0);
  TESTASSERT(test_task_thread_pool2() == 0);
  TESTASSERT(test_task_thread_pool3() == 0);

  TESTASSERT(test_inplace_task() == 0);
}
