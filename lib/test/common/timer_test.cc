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

#include "srsran/common/timers.h"
#include "srsran/support/srsran_test.h"
#include <iostream>
#include <random>
#include <srsran/common/tti_sync_cv.h>
#include <thread>

using namespace srsran;

static_assert(timer_handler::max_timer_duration() == 1073741823, "Invalid max duration");

void timers_test1()
{
  timer_handler timers;
  uint32_t      dur = 5;

  {
    // TEST: default ctor places unique_timer in correct state
    timer_handler::unique_timer t = timers.get_unique_timer();
    TESTASSERT(not t.is_running() and not t.is_expired());
    TESTASSERT(t.id() == 0);
    timer_handler::unique_timer t2 = timers.get_unique_timer();
    TESTASSERT(not t2.is_running() and not t2.is_expired());
    TESTASSERT(t2.id() == 1);
    TESTASSERT(timers.nof_timers() == 2);

    // TEST: Run multiple times with the same duration
    bool callback_called = false;
    t.set(dur, [&callback_called](int tid) { callback_called = true; });
    for (uint32_t runs = 0; runs < 3; ++runs) {
      callback_called = false;
      TESTASSERT(not t.is_running());
      t.run();
      TESTASSERT(t.is_running() and not t.is_expired());
      for (uint32_t i = 0; i < dur - 1; ++i) {
        timers.step_all();
        TESTASSERT(t.is_running() and not t.is_expired());
      }
      timers.step_all();
      TESTASSERT(not t.is_running() and t.is_expired());
      TESTASSERT(callback_called);
    }

    // TEST: interrupt a timer. check if callback was called
    callback_called = false;
    t.run();
    timers.step_all();
    TESTASSERT(t.is_running());
    t.stop();
    TESTASSERT(not t.is_running());
    for (uint32_t i = 0; i < dur; ++i) {
      timers.step_all();
      TESTASSERT(not t.is_running());
    }
    TESTASSERT(not callback_called);

    // TEST: call timer::run() when it is already running. Check if duration gets extended.
    callback_called = false;
    t.run();
    timers.step_all();
    TESTASSERT(t.is_running());
    t.run(); // re-run
    for (uint32_t i = 0; i < dur - 1; ++i) {
      timers.step_all();
      TESTASSERT(t.is_running());
    }
    timers.step_all();
    TESTASSERT(not t.is_running());
    TESTASSERT(callback_called);

    // TEST: ordering of timers is respected
    timer_handler::unique_timer t3 = timers.get_unique_timer();
    TESTASSERT(t3.id() == 2);
    int  first_id = -1, last_id = -1;
    auto callback = [&first_id, &last_id](int id) {
      if (first_id < 0) {
        printf("First timer id=%d\n", id);
        first_id = id;
      }
      last_id = id;
    };
    t.set(4, callback);
    t2.set(2, callback);
    t3.set(6, callback);
    t.run();
    t2.run();
    t3.run();
    for (uint32_t i = 0; i < 5; ++i) {
      timers.step_all();
      TESTASSERT(i >= 3 or t.is_running());
      TESTASSERT(i >= 1 or t2.is_running());
      TESTASSERT(t3.is_running());
    }
    timers.step_all();
    TESTASSERT(t.is_expired() and t2.is_expired() and t3.is_expired());
    TESTASSERT(first_id == 1);
    TESTASSERT(last_id == 2);
  }
  // TEST: timer dtor is called and removes "timer" from "timers"
  TESTASSERT(timers.nof_timers() == 0);
}

/**
 * Description:
 * - calling stop() early, forbids the timer from getting expired
 * - calling stop() after timer has expired should be a noop
 */
void timers_test2()
{
  timer_handler timers;
  uint32_t      duration = 2;

  auto utimer  = timers.get_unique_timer();
  auto utimer2 = timers.get_unique_timer();
  utimer.set(duration);
  utimer2.set(duration);

  // TEST 1: call utimer.stop() early and check if timer expires
  utimer.run();
  utimer2.run();
  TESTASSERT(utimer.is_running() and not utimer.is_expired());
  utimer.stop();
  TESTASSERT(not utimer.is_running() and not utimer.is_expired());

  for (uint32_t i = 0; i < 5; ++i) {
    timers.step_all();
  }
  TESTASSERT(not utimer.is_expired());
  TESTASSERT(utimer2.is_expired());

  // TEST 2: call utimer.stop() after it expires and assert it is still expired
  utimer2.stop();
  TESTASSERT(utimer2.is_expired());
}

/**
 * Description:
 * - setting a new duration while the timer is already running should not stop timer, and should extend timeout
 */
void timers_test3()
{
  timer_handler timers;
  uint32_t      duration = 5;

  auto utimer = timers.get_unique_timer();
  utimer.set(duration);
  utimer.run();

  for (uint32_t i = 0; i < 2 * duration + 1; ++i) {
    timers.step_all();
    if ((i % 2) == 0) {
      // extends lifetime
      utimer.set(duration);
    }
    TESTASSERT(utimer.is_running());
  }
  for (uint32_t i = 0; i < duration - 1; ++i) {
    timers.step_all();
    TESTASSERT(utimer.is_running());
  }
  timers.step_all();
  TESTASSERT(not utimer.is_running() and utimer.is_expired());
}

struct timers_test4_ctxt {
  std::vector<unique_timer> timers;
  srsran::tti_sync_cv       tti_sync1;
  srsran::tti_sync_cv       tti_sync2;
  const uint32_t            duration = 1000;
};

static void timers2_test4_thread(timers_test4_ctxt* ctx)
{
  std::random_device                    rd;
  std::mt19937                          mt19937(rd());
  std::uniform_real_distribution<float> real_dist(0.0f, 1.0f);
  for (uint32_t d = 0; d < ctx->duration; d++) {
    // make random events
    for (uint32_t i = 1; i < ctx->timers.size(); i++) {
      // ensure the getters always return reasonable values
      TESTASSERT(ctx->timers[i].time_elapsed() <= ctx->duration);

      if (0.1f > real_dist(mt19937)) {
        ctx->timers[i].run();
      }
      if (0.1f > real_dist(mt19937)) {
        ctx->timers[i].stop();
      }
      if (0.1f > real_dist(mt19937)) {
        ctx->timers[i].set(static_cast<uint32_t>(ctx->duration * real_dist(mt19937)));
        ctx->timers[i].run();
      }
    }

    // Send finished to main thread
    ctx->tti_sync1.increase();

    // Wait to main thread to check results
    ctx->tti_sync2.wait();
  }
}

void timers_test4()
{
  timer_handler                         timers;
  timers_test4_ctxt                     ctx;
  uint32_t                              nof_timers = 32;
  std::mt19937                          mt19937(4);
  std::uniform_real_distribution<float> real_dist(0.0f, 1.0f);

  // Generate all timers and start them
  for (uint32_t i = 0; i < nof_timers; i++) {
    ctx.timers.push_back(timers.get_unique_timer());
    ctx.timers[i].set(ctx.duration);
    ctx.timers[i].run();
  }

  /* ========== multithreaded region begin =========== */

  // Create side thread
  std::thread thread(timers2_test4_thread, &ctx);

  for (uint32_t d = 0; d < ctx.duration; d++) {
    // make random events
    for (uint32_t i = 1; i < nof_timers; i++) {
      // ensure the getters always return reasonable values
      TESTASSERT(ctx.timers[i].time_elapsed() <= ctx.duration);

      if (0.1f > real_dist(mt19937)) {
        ctx.timers[i].run(); // restart run
      }
      if (0.1f > real_dist(mt19937)) {
        ctx.timers[i].stop(); // stop run
      }
      if (0.1f > real_dist(mt19937)) {
        ctx.timers[i].set(static_cast<uint32_t>(ctx.duration * real_dist(mt19937)));
        ctx.timers[i].run(); // start run with new duration
      }
    }

    // first timer does not get updated, so it shall keep running
    TESTASSERT(ctx.timers[0].is_running());

    // Increment time
    timers.step_all();

    // wait second thread to finish events
    ctx.tti_sync1.wait();

    // assert no timer got wrong values
    for (uint32_t i = 0; i < nof_timers; i++) {
      if (ctx.timers[i].is_running()) {
        TESTASSERT(ctx.timers[i].time_elapsed() <= ctx.timers[i].duration());
        TESTASSERT(ctx.timers[i].duration() <= ctx.duration);
      }
    }

    // Start new TTI
    ctx.tti_sync2.increase();
  }

  // Finish asynchronous thread
  thread.join();

  /* ========== multithreaded region end =========== */

  // First timer should have expired
  TESTASSERT(ctx.timers[0].is_expired());
  TESTASSERT(not ctx.timers[0].is_running());

  // Run for the maximum period
  for (uint32_t d = 0; d < ctx.duration; d++) {
    timers.step_all();
  }

  // No timer should be running
  for (uint32_t i = 0; i < nof_timers; i++) {
    TESTASSERT(not ctx.timers[i].is_running());
  }
}

/**
 * Description: Delaying a callback using the timer_handler
 */
void timers_test5()
{
  timer_handler timers;
  TESTASSERT(timers.nof_timers() == 0);
  TESTASSERT(timers.nof_running_timers() == 0);

  std::vector<int> vals;

  // TTI 0: Add a unique_timer of duration=5
  timer_handler::unique_timer t = timers.get_unique_timer();
  TESTASSERT(timers.nof_timers() == 1);
  t.set(5, [&vals](uint32_t tid) { vals.push_back(1); });
  t.run();
  TESTASSERT(timers.nof_running_timers() == 1);
  timers.step_all();

  // TTI 1: Add two delayed callbacks, with duration=2 and 6
  {
    // ensure captures by value are ok
    std::string string = "test string";
    timers.defer_callback(2, [&vals, string]() {
      vals.push_back(2);
      srsran_assert(string == "test string", "string was not captured correctly");
    });
  }
  timers.defer_callback(6, [&vals]() { vals.push_back(3); });
  TESTASSERT(timers.nof_timers() == 3);
  TESTASSERT(timers.nof_running_timers() == 3);
  timers.step_all();
  timers.step_all();

  // TTI 3: First callback should have been triggered by now
  TESTASSERT(timers.nof_running_timers() == 2);
  TESTASSERT(timers.nof_timers() == 2);
  TESTASSERT(vals.size() == 1);
  TESTASSERT(vals[0] == 2);
  timers.step_all();
  timers.step_all();

  // TTI 5: Unique timer should have been triggered by now
  TESTASSERT(timers.nof_running_timers() == 1);
  TESTASSERT(timers.nof_timers() == 2);
  TESTASSERT(vals.size() == 2);
  TESTASSERT(vals[1] == 1);
  timers.step_all();
  timers.step_all();

  // TTI 7: Second callback should have been triggered by now
  TESTASSERT(timers.nof_running_timers() == 0);
  TESTASSERT(timers.nof_timers() == 1);
  TESTASSERT(vals.size() == 3);
  TESTASSERT(vals[2] == 3);
}

/**
 * Description: Check if erasure of a running timer is safe
 */
void timers_test6()
{
  timer_handler timers;

  std::vector<int> vals;

  // Event: Add a timer that gets erased 1 tti after, and before expiring.
  {
    timer_handler::unique_timer t = timers.get_unique_timer();
    t.set(2, [&vals](uint32_t tid) { vals.push_back(1); });
    t.run();
    TESTASSERT(timers.nof_running_timers() == 1 and t.duration() == 2 and t.is_running());
    timers.step_all();
  }
  TESTASSERT(timers.nof_running_timers() == 0);
  TESTASSERT(timers.nof_timers() == 0);

  // TEST: The timer callback should not have been called
  timers.step_all();
  TESTASSERT(vals.empty());

  // Event: Add a timer that gets erased right after, and add another timer with same timeout
  {
    timer_handler::unique_timer t = timers.get_unique_timer();
    t.set(2, [&vals](uint32_t tid) { vals.push_back(2); });
    t.run();
    TESTASSERT(timers.nof_running_timers() == 1 and t.is_running());
    timers.step_all();
    TESTASSERT(t.time_elapsed() == 1);
  }
  timer_handler::unique_timer t = timers.get_unique_timer();
  t.set(1, [&vals](uint32_t tid) { vals.push_back(3); });
  t.run();
  TESTASSERT(timers.nof_running_timers() == 1);

  // TEST: The second timer's callback should be the one being called, and should be called only once
  timers.step_all();
  TESTASSERT(vals.size() == 1 and vals[0] == 3);
}

/**
 * Tests specific to timer_handler wheel-based implementation:
 * - check if timer update is safe when its new updated wheel position matches the previous wheel position
 * - multime timers can exist in the same wheel position
 */
void timers_test7()
{
  timer_handler timers;
  size_t        wheel_size = timer_handler::get_wheel_size();

  unique_timer t = timers.get_unique_timer();
  t.set(2);
  t.run();

  timers.step_all();
  TESTASSERT(not t.is_expired() and t.is_running());

  // should fall in same wheel position as previous timer run
  t.set(1 + wheel_size);
  for (size_t i = 0; i < wheel_size; ++i) {
    timers.step_all();
    TESTASSERT(not t.is_expired() and t.is_running());
  }
  timers.step_all();
  TESTASSERT(t.is_expired() and not t.is_running());

  // the three timers will all fall in the same wheel position. However, only t and t3 should trigger
  unique_timer t2 = timers.get_unique_timer();
  unique_timer t3 = timers.get_unique_timer();
  t.set(5);
  t2.set(5 + wheel_size);
  t3.set(5);
  t.run();
  t2.run();
  t3.run();
  TESTASSERT(timers.nof_running_timers() == 3 and timers.nof_timers() == 3);
  for (size_t i = 0; i < 5; ++i) {
    TESTASSERT(not t.is_expired() and t.is_running());
    TESTASSERT(not t2.is_expired() and t2.is_running());
    TESTASSERT(not t3.is_expired() and t3.is_running());
    timers.step_all();
  }
  TESTASSERT(t.is_expired() and not t.is_running());
  TESTASSERT(not t2.is_expired() and t2.is_running());
  TESTASSERT(t3.is_expired() and not t3.is_running());
  TESTASSERT(timers.nof_running_timers() == 1 and timers.nof_timers() == 3);
}

int main()
{
  timers_test1();
  timers_test2();
  timers_test3();
  timers_test4();
  timers_test5();
  timers_test6();
  timers_test7();
  printf("Success\n");
  return 0;
}
