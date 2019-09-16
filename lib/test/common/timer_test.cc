/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

#include "srslte/common/timers.h"
#include <iostream>

#define TESTASSERT(cond)                                                                                               \
  {                                                                                                                    \
    if (!(cond)) {                                                                                                     \
      std::cout << "[" << __FUNCTION__ << "][Line " << __LINE__ << "]: FAIL at " << (#cond) << std::endl;              \
      return -1;                                                                                                       \
    }                                                                                                                  \
  }

using namespace srslte;

int timers2_test()
{
  timers2  timers;
  uint32_t dur = 5;

  timers2::unique_timer t = timers.get_unique_timer();
  TESTASSERT(not t.is_running() and not t.is_expired())
  TESTASSERT(t.id() == 0)
  timers2::unique_timer t2 = timers.get_unique_timer();
  TESTASSERT(not t2.is_running() and not t2.is_expired())
  TESTASSERT(t2.id() == 1)

  // TEST: Run multiple times with the same duration
  bool callback_called = false;
  t.set(dur, [&callback_called](int) { callback_called = true; });
  for (uint32_t runs = 0; runs < 3; ++runs) {
    callback_called = false;
    TESTASSERT(not t.is_running())
    t.run();
    TESTASSERT(t.is_running() and not t.is_expired())
    for (uint32_t i = 0; i < dur; ++i) {
      timers.step_all();
      TESTASSERT(t.is_running() and not t.is_expired())
    }
    timers.step_all();
    TESTASSERT(not t.is_running() and t.is_expired())
    TESTASSERT(callback_called)
  }

  // TEST: interrupt a timer. check if callback was called
  callback_called = false;
  t.run();
  timers.step_all();
  TESTASSERT(t.is_running())
  t.stop();
  TESTASSERT(not t.is_running())
  for (uint32_t i = 0; i < dur; ++i) {
    timers.step_all();
    TESTASSERT(not t.is_running())
  }
  TESTASSERT(not callback_called)

  // TEST: call timer::run() when it is already running. Check if duration gets extended.
  callback_called = false;
  t.run();
  timers.step_all();
  TESTASSERT(t.is_running())
  t.run(); // re-run
  for (uint32_t i = 0; i < dur; ++i) {
    timers.step_all();
    TESTASSERT(t.is_running())
  }
  timers.step_all();
  TESTASSERT(not t.is_running())
  TESTASSERT(callback_called)

  printf("Success\n");

  return SRSLTE_SUCCESS;
}

int main()
{
  TESTASSERT(timers2_test() == SRSLTE_SUCCESS)

  return 0;
}
