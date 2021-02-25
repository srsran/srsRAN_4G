/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
  srslte::get_background_workers().push_task([&task_sched, &state]() {
    task_sched.notify_background_task_result([&state]() { state = task_result::external; });
  });
  TESTASSERT(state == task_result::null);
  task_sched.run_next_task(); // runs notification
  TESTASSERT(state == task_result::external);

  return SRSLTE_SUCCESS;
}

int test_task_scheduler_with_pool()
{
  srslte::task_scheduler task_sched{5, 2};
  task_result            state = task_result::null;

  srslte::get_background_workers().push_task([&task_sched, &state]() {
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
