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

#include "srslte/common/task_scheduler.h"
#include "srslte/common/test_common.h"

enum class task_result { null, internal, external, timer };

int test_task_scheduler_no_pool()
{
  srslte::task_scheduler task_sched{5, 0};
  task_result            state = task_result::null;

  // TEST: deferring task does not run the task until the next tic
  task_sched.defer_task([&state]() { state = task_result::internal; });
  TESTASSERT(state == task_result::null);
  task_sched.run_pending_tasks();
  TESTASSERT(state == task_result::internal);

  // TEST: check delaying of task
  state   = task_result::null;
  int dur = 5;
  task_sched.defer_callback(dur, [&state]() { state = task_result::timer; });
  for (int i = 0; i < dur; ++i) {
    TESTASSERT(state == task_result::null);
    task_sched.tic();
    task_sched.run_pending_tasks();
  }
  TESTASSERT(state == task_result::timer);

  // TEST: background task is run, despite there are no pool workers
  state = task_result::null;
  task_sched.enqueue_background_task([&task_sched, &state](uint32_t worker_id) {
    task_sched.notify_background_task_result([&state]() { state = task_result::external; });
  });
  TESTASSERT(state == task_result::null);
  task_sched.run_next_task(); // runs background task
  TESTASSERT(state == task_result::null);
  task_sched.run_next_task(); // runs notification
  TESTASSERT(state == task_result::external);

  return SRSLTE_SUCCESS;
}

int test_task_scheduler_with_pool()
{
  srslte::task_scheduler task_sched{5, 2};
  task_result            state = task_result::null;

  task_sched.enqueue_background_task([&task_sched, &state](uint32_t worker_id) {
    task_sched.notify_background_task_result([&state]() { state = task_result::external; });
  });
  TESTASSERT(state == task_result::null);
  task_sched.run_next_task(); // waits and runs notification
  TESTASSERT(state == task_result::external);

  return SRSLTE_SUCCESS;
}

int main()
{
  TESTASSERT(test_task_scheduler_no_pool() == SRSLTE_SUCCESS);
  TESTASSERT(test_task_scheduler_with_pool() == SRSLTE_SUCCESS);
}
