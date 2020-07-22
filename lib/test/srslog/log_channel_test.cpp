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

#include "srslte/srslog/log_channel.h"
#include "srslte/srslog/sink.h"
#include "testing_helpers.h"

using namespace srslog;

namespace {

/// A Dummy implementation of a sink.
class sink_dummy : public sink
{
public:
  detail::error_string write(detail::memory_buffer buffer) override
  {
    return {};
  }

  detail::error_string flush() override { return {}; }
};

/// A Dummy implementation of the log backend.
class backend_dummy : public detail::log_backend
{
public:
  void start() override {}

  void push(detail::log_entry&& entry) override {}
};

} // namespace

static bool when_log_channel_is_created_then_id_matches_expected_value()
{
  backend_dummy backend;
  sink_dummy s;
  log_channel log("id", s, backend);

  ASSERT_EQ(log.id(), "id");

  return true;
}

static bool when_log_channel_is_disabled_then_enabled_returns_false()
{
  backend_dummy backend;
  sink_dummy s;
  log_channel log("id", s, backend);

  log.set_enabled(false);

  ASSERT_EQ(log.enabled(), false);

  return true;
}

static bool when_log_channel_is_enabled_then_enabled_returns_true()
{
  backend_dummy backend;
  sink_dummy s;
  log_channel log("id", s, backend);

  log.set_enabled(true);

  ASSERT_EQ(log.enabled(), true);

  return true;
}

namespace {

/// A Spy implementation of a log backend. Tests can query if the push method
/// has been invoked and the last received log entry.
class backend_spy : public detail::log_backend
{
public:
  void start() override {}

  void push(detail::log_entry&& entry) override
  {
    e = std::move(entry);
    ++count;
  }

  unsigned push_invocation_count() const { return count; }

  const detail::log_entry& last_entry() const { return e; }

private:
  unsigned count = 0;
  detail::log_entry e;
};

} // namespace

static bool
when_logging_in_log_channel_then_log_entry_is_pushed_into_the_backend()
{
  backend_spy backend;
  sink_dummy s;
  log_channel log("id", s, backend);

  std::string fmtstring = "Arg1: %u Arg2: %s";
  log(fmtstring, 42, "Hello");

  ASSERT_EQ(backend.push_invocation_count(), 1);
  ASSERT_NE(backend.last_entry().s, nullptr);
  ASSERT_EQ(backend.last_entry().fmtstring, fmtstring);

  return true;
}

static bool when_logging_in_disabled_log_channel_then_log_entry_is_ignored()
{
  backend_spy backend;
  sink_dummy s;
  log_channel log("id", s, backend);

  log.set_enabled(false);
  std::string fmtstring = "Arg1: %u Arg2: %s";
  log(fmtstring, 42, "Hello");

  ASSERT_EQ(backend.push_invocation_count(), 0);

  return true;
}

int main()
{
  TEST_FUNCTION(when_log_channel_is_created_then_id_matches_expected_value);
  TEST_FUNCTION(when_log_channel_is_disabled_then_enabled_returns_false);
  TEST_FUNCTION(when_log_channel_is_enabled_then_enabled_returns_true);
  TEST_FUNCTION(
      when_logging_in_log_channel_then_log_entry_is_pushed_into_the_backend);
  TEST_FUNCTION(when_logging_in_disabled_log_channel_then_log_entry_is_ignored);

  return 0;
}
