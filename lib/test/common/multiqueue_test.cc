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

#include "srsran/adt/move_callback.h"
#include "srsran/common/multiqueue.h"
#include "srsran/common/test_common.h"
#include "srsran/common/thread_pool.h"
#include <iostream>
#include <map>
#include <random>
#include <thread>
#include <unistd.h>

using namespace srsran;

int test_multiqueue()
{
  std::cout << "\n======= TEST multiqueue test: start =======\n";

  int number = 2;

  multiqueue_handler<int> multiqueue;
  TESTASSERT(multiqueue.nof_queues() == 0);

  // test push/pop and size for one queue
  queue_handle<int> qid1 = multiqueue.add_queue();
  TESTASSERT(qid1.active());
  TESTASSERT(qid1.size() == 0 and qid1.empty());
  TESTASSERT(multiqueue.nof_queues() == 1);
  TESTASSERT(qid1.try_push(5).has_value());
  TESTASSERT(qid1.try_push(number));
  TESTASSERT(qid1.size() == 2 and not qid1.empty());
  TESTASSERT(multiqueue.wait_pop(&number));
  TESTASSERT(number == 5);
  TESTASSERT(multiqueue.wait_pop(&number));
  TESTASSERT(number == 2 and qid1.empty());

  // test push/pop and size for two queues
  queue_handle<int> qid2 = multiqueue.add_queue();
  TESTASSERT(qid2.active());
  TESTASSERT(multiqueue.nof_queues() == 2 and qid1.active());
  TESTASSERT(qid2.try_push(3).has_value());
  TESTASSERT(qid2.size() == 1 and not qid2.empty());
  TESTASSERT(qid1.empty());

  // check if erasing a queue breaks anything
  qid1.reset();
  TESTASSERT(multiqueue.nof_queues() == 1 and not qid1.active());
  qid1 = multiqueue.add_queue();
  TESTASSERT(qid1.empty() and qid1.active());
  TESTASSERT(qid2.size() == 1 and not qid2.empty());
  multiqueue.wait_pop(&number);

  // check round-robin
  for (int i = 0; i < 10; ++i) {
    TESTASSERT(qid1.try_push(i));
  }
  for (int i = 20; i < 35; ++i) {
    TESTASSERT(qid2.try_push(i));
  }
  TESTASSERT(qid1.size() == 10);
  TESTASSERT(qid2.size() == 15);
  TESTASSERT(multiqueue.wait_pop(&number) and number == 0);
  TESTASSERT(multiqueue.wait_pop(&number) and number == 20);
  TESTASSERT(multiqueue.wait_pop(&number) and number == 1);
  TESTASSERT(multiqueue.wait_pop(&number) and number == 21);
  TESTASSERT(qid1.size() == 8);
  TESTASSERT(qid2.size() == 13);
  for (int i = 0; i < 8 * 2; ++i) {
    multiqueue.wait_pop(&number);
  }
  TESTASSERT(qid1.size() == 0);
  TESTASSERT(qid2.size() == 5);
  TESTASSERT(multiqueue.wait_pop(&number) and number == 30);

  // remove existing queues
  qid1.reset();
  qid2.reset();
  TESTASSERT(multiqueue.nof_queues() == 0);

  // check that adding a queue of different capacity works
  {
    qid1 = multiqueue.add_queue();
    qid2 = multiqueue.add_queue();

    // remove first queue again
    qid1.reset();
    TESTASSERT(multiqueue.nof_queues() == 1);

    // add queue with non-default capacity
    auto qid3 = multiqueue.add_queue(10);
    TESTASSERT(qid3.capacity() == 10);

    // make sure neither a new queue index is returned
    TESTASSERT(qid1 != qid3);
    TESTASSERT(qid2 != qid3);
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
  auto                    qid1 = multiqueue.add_queue();
  std::atomic<bool>       t1_running         = {true};
  auto                    push_blocking_func = [&t1_running](queue_handle<int>* qid, int start_value, int nof_pushes) {
    for (int i = 0; i < nof_pushes; ++i) {
      qid->push(start_value + i);
      std::cout << "t1: pushed item " << i << std::endl;
    }
    std::cout << "t1: pushed all items\n";
    t1_running = false;
  };

  std::thread t1(push_blocking_func, &qid1, start_number, nof_pushes);

  // Wait for queue to fill
  while ((int)qid1.size() != capacity) {
    usleep(1000);
    TESTASSERT(t1_running);
  }

  for (int i = 0; i < nof_pushes; ++i) {
    TESTASSERT(multiqueue.wait_pop(&number));
    TESTASSERT(number == start_number + i);
    std::cout << "main: popped item " << i << "\n";
  }
  std::cout << "main: popped all items\n";

  // wait for thread to finish
  while (t1_running) {
    usleep(1000);
  }
  TESTASSERT(qid1.size() == 0);

  multiqueue.stop();
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
  auto                    qid1 = multiqueue.add_queue();
  auto push_blocking_func      = [](queue_handle<int>* qid, int start_value, int nof_pushes, bool* is_running) {
    for (int i = 0; i < nof_pushes; ++i) {
      qid->push(start_value + i);
    }
    std::cout << "t1: pushed all items\n";
    *is_running = false;
  };

  bool        t1_running = true;
  std::thread t1(push_blocking_func, &qid1, start_number, nof_pushes, &t1_running);

  // Wait for queue to fill
  while ((int)qid1.size() != capacity) {
    usleep(1000);
    TESTASSERT(t1_running);
  }

  multiqueue.stop();
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
  auto                    qid1              = multiqueue.add_queue();
  auto                    pop_blocking_func = [&multiqueue](bool* success) {
    int  number = 0;
    bool ret    = multiqueue.wait_pop(&number);
    *success    = not ret;
  };

  bool        t1_success = false;
  std::thread t1(pop_blocking_func, &t1_success);

  TESTASSERT(not t1_success);
  usleep(1000);
  TESTASSERT(not t1_success);
  TESTASSERT((int)qid1.size() == 0);

  // Should be able to unlock all
  multiqueue.stop();
  TESTASSERT(multiqueue.nof_queues() == 0);
  TESTASSERT(not qid1.active());
  t1.join();
  TESTASSERT(t1_success);

  std::cout << "outcome: Success\n";
  std::cout << "===================================================\n";

  return 0;
}

int test_multiqueue_threading4()
{
  std::cout << "\n===== TEST multiqueue threading test 4: start =====\n";
  // Description: the consumer will block on popping, but the pushing from different producers
  //              should be sufficient to awake it when necessary

  int                     capacity = 4;
  multiqueue_handler<int> multiqueue(capacity);
  auto                    qid1 = multiqueue.add_queue();
  auto                    qid2 = multiqueue.add_queue();
  auto                    qid3 = multiqueue.add_queue();
  auto                    qid4 = multiqueue.add_queue();
  std::mutex              mutex;
  int                     last_number       = -1;
  auto                    pop_blocking_func = [&multiqueue, &last_number, &mutex](bool* success) {
    int number = 0;
    while (multiqueue.wait_pop(&number)) {
      std::lock_guard<std::mutex> lock(mutex);
      last_number = std::max(last_number, number);
    }
    *success = true;
  };

  bool        t1_success = false;
  std::thread t1(pop_blocking_func, &t1_success);

  std::random_device              rd;
  std::mt19937                    gen(rd());
  std::uniform_int_distribution<> dist{0, 2};
  for (int i = 0; i < 10000; ++i) {
    int qidx = dist(gen);
    switch (qidx) {
      case 0:
        qid1.push(i);
        break;
      case 1:
        qid2.push(i);
        break;
      case 2:
        qid4.push(i);
        break;
      default:
        break;
    }
    if (i % 20 == 0) {
      int                          count = 0;
      std::unique_lock<std::mutex> lock(mutex);
      while (last_number != i) {
        lock.unlock();
        usleep(100);
        count++;
        TESTASSERT(count < 100000);
        lock.lock();
      }
    }
  }

  // Should be able to unlock all
  multiqueue.stop();
  TESTASSERT(multiqueue.nof_queues() == 0);
  TESTASSERT(not qid1.active());
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

  uint32_t                       nof_workers = 4, nof_runs = 10000;
  std::mutex                     count_mutex;
  std::map<std::thread::id, int> count_worker;

  task_thread_pool thread_pool(nof_workers);

  auto task = [&count_worker, &count_mutex]() {
    std::lock_guard<std::mutex> lock(count_mutex);
    count_worker[std::this_thread::get_id()]++;
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
  for (auto& w : count_worker) {
    if (w.second < 10) {
      std::cout << "WARNING: the number of tasks " << w.second << " assigned to worker " << w.first << " is too low";
    }
    total_count += w.second;
    std::cout << "worker " << w.first << ": " << w.second << " runs\n";
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

  uint32_t             nof_workers = 4;
  std::atomic<uint8_t> workers_started{0};
  uint8_t              workers_finished = 0;
  std::mutex           mut;

  task_thread_pool thread_pool(nof_workers);
  thread_pool.start();

  auto task = [&workers_started, &workers_finished, &mut]() {
    {
      std::lock_guard<std::mutex> lock(mut);
      workers_started++;
    }
    std::this_thread::sleep_for(std::chrono::seconds{1});
    std::lock_guard<std::mutex> lock(mut);
    std::cout << "worker has finished\n";
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

  srsran::move_callback<void()> t{l0};
  srsran::move_callback<void()> t2{[v]() mutable { v = 2; }};
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
  srsran::move_callback<void()> t4{std::bind([&v](C& c) { v = *c.val; }, std::move(c))};
  {
    decltype(t4) t5;
    t5 = std::move(t4);
    t5();
    TESTASSERT(v == 5);
  }

  D                             d;
  srsran::move_callback<void()> t6 = [&v, d]() { v = d.big_val[0]; };
  {
    srsran::move_callback<void()> t7;
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
  auto l3 = [](const srsran::move_callback<void()>& task) { task(); };
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
  TESTASSERT(test_multiqueue_threading4() == 0);

  TESTASSERT(test_task_thread_pool() == 0);
  TESTASSERT(test_task_thread_pool2() == 0);
  TESTASSERT(test_task_thread_pool3() == 0);

  TESTASSERT(test_inplace_task() == 0);
}
