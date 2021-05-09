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

#include "srsran/srslog/event_trace.h"
#include "srsran/srslog/log_channel.h"
#include "test_dummies.h"
#include "testing_helpers.h"
#include <thread>

using namespace srslog;

namespace {

/// A Spy implementation of a log backend. Tests can query if the push method
/// has been invoked.
class backend_spy : public detail::log_backend
{
public:
  void start(srslog::backend_priority priority) override {}

  bool push(detail::log_entry&& entry) override
  {
    ++count;
    return true;
  }

  fmt::dynamic_format_arg_store<fmt::printf_context>* alloc_arg_store() override { return &store; }

  bool is_running() const override { return true; }

  void reset() { count = 0; }

  unsigned push_invocation_count() const { return count; }

private:
  unsigned                                           count = 0;
  fmt::dynamic_format_arg_store<fmt::printf_context> store;
};

} // namespace

static bool when_tracing_with_duration_event_then_two_events_are_generated(backend_spy& spy)
{
  trace_duration_begin("a", "b");
  ASSERT_EQ(spy.push_invocation_count(), 1);

  trace_duration_end("a", "b");
  ASSERT_EQ(spy.push_invocation_count(), 2);

  return true;
}

static bool when_tracing_with_complete_event_then_one_event_is_generated(backend_spy& spy)
{
  {
    trace_complete_event("a", "b");
  }
  ASSERT_EQ(spy.push_invocation_count(), 1);

  return true;
}

static bool when_tracing_with_under_threshold_complete_event_then_no_event_is_generated(backend_spy& spy)
{
  {
    trace_threshold_complete_event("a", "b", std::chrono::microseconds(100000));
  }
  ASSERT_EQ(spy.push_invocation_count(), 0);

  return true;
}

static bool when_tracing_with_above_threshold_complete_event_then_one_event_is_generated(backend_spy& spy)
{
  {
    trace_threshold_complete_event("a", "b", std::chrono::microseconds(10));
    std::this_thread::sleep_for(std::chrono::microseconds(1000));
  }
  ASSERT_EQ(spy.push_invocation_count(), 1);

  return true;
}

int main()
{
  test_dummies::sink_dummy s;
  backend_spy              backend;
  log_channel              c("test", s, backend);

  // Inject our spy into the framework.
  event_trace_init(c);

  TEST_FUNCTION(when_tracing_with_duration_event_then_two_events_are_generated, backend);
  backend.reset();
  TEST_FUNCTION(when_tracing_with_complete_event_then_one_event_is_generated, backend);
  backend.reset();
  TEST_FUNCTION(when_tracing_with_under_threshold_complete_event_then_no_event_is_generated, backend);
  backend.reset();
  TEST_FUNCTION(when_tracing_with_above_threshold_complete_event_then_one_event_is_generated, backend);

  return 0;
}
