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

#include "srslte/srslog/event_trace.h"
#include "srslte/srslog/log_channel.h"
#include "test_dummies.h"
#include "testing_helpers.h"

using namespace srslog;

namespace {

/// A Spy implementation of a log backend. Tests can query if the push method
/// has been invoked.
class backend_spy : public detail::log_backend
{
public:
  void start() override {}

  void push(detail::log_entry&& entry) override { ++count; }

  bool is_running() const override { return true; }

  void reset() { count = 0; }

  unsigned push_invocation_count() const { return count; }

private:
  unsigned count = 0;
};

} // namespace

static bool
when_tracing_with_duration_event_then_two_events_are_generated(backend_spy& spy)
{
  trace_duration_begin("a", "b");
  ASSERT_EQ(spy.push_invocation_count(), 1);

  trace_duration_end("a", "b");
  ASSERT_EQ(spy.push_invocation_count(), 2);

  return true;
}

static bool
when_tracing_with_complete_event_then_one_event_is_generated(backend_spy& spy)
{
  {
    trace_complete_event("a", "b");
  }
  ASSERT_EQ(spy.push_invocation_count(), 1);

  return true;
}

int main()
{
  test_dummies::sink_dummy s;
  backend_spy backend;
  log_channel c("test", s, backend);

  // Inject our spy into the framework.
  event_trace_init(c);

  TEST_FUNCTION(when_tracing_with_duration_event_then_two_events_are_generated,
                backend);
  backend.reset();
  TEST_FUNCTION(when_tracing_with_complete_event_then_one_event_is_generated,
                backend);

  return 0;
}
