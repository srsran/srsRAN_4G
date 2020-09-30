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

#include "src/srslog/log_backend_impl.h"
#include "srslte/srslog/sink.h"
#include "testing_helpers.h"

using namespace srslog;

static bool when_backend_is_started_then_is_started_returns_true()
{
  log_backend_impl backend;

  ASSERT_EQ(backend.is_running(), false);
  backend.start();
  ASSERT_EQ(backend.is_running(), true);

  return true;
}

static bool when_backend_is_started_and_stopped_then_is_started_returns_false()
{
  log_backend_impl backend;

  backend.start();
  backend.stop();

  ASSERT_EQ(backend.is_running(), false);

  return true;
}

namespace {

/// A Spy implementation of a log sink. Tests can query the last received memory
/// buffer by the sink and the number of invocations to the write method.
class sink_spy : public sink
{
public:
  detail::error_string write(detail::memory_buffer buffer) override
  {
    ++count;
    str.insert(0, buffer.data(), buffer.size());
    return {};
  }

  detail::error_string flush() override { return {}; }

  unsigned write_invocation_count() const { return count; }

  const std::string& received_buffer() const { return str; }

private:
  unsigned count = 0;
  std::string str;
};

} // namespace

static bool when_backend_is_not_started_then_pushed_log_entries_are_ignored()
{
  sink_spy spy;
  log_backend_impl backend;

  detail::log_entry entry = {&spy};
  backend.push(std::move(entry));

  ASSERT_EQ(spy.write_invocation_count(), 0);

  return true;
}

/// Builds a basic log entry.
static detail::log_entry build_log_entry(sink* s)
{
  using tp_ty = std::chrono::time_point<std::chrono::high_resolution_clock>;
  tp_ty tp;

  fmt::dynamic_format_arg_store<fmt::printf_context> store;
  store.push_back(88);

  return {s, tp, {0, false}, "Text %d", std::move(store), "", '\0'};
}

static bool when_backend_is_started_then_pushed_log_entries_are_sent_to_sink()
{
  sink_spy spy;

  log_backend_impl backend;
  backend.start();

  backend.push(build_log_entry(&spy));

  // Stop the backend to ensure the entry has been processed.
  backend.stop();

  ASSERT_EQ(spy.write_invocation_count(), 1);
  ASSERT_NE(spy.received_buffer().find("Text 88"), std::string::npos);

  return true;
}

namespace {

/// A Configurable Stub implementation of an object to be invoked by the
/// backend. Upon invocation, instances of this class return an error that can
/// be configured dynamically.
class sink_error_stub : public sink
{
public:
  explicit sink_error_stub(std::string err) : err(std::move(err)) {}

  detail::error_string write(detail::memory_buffer buffer) override
  {
    return err;
  }

  detail::error_string flush() override { return err; }

private:
  std::string err;
};

} // namespace

static bool when_sink_write_fails_then_error_handler_is_invoked()
{
  std::string error_str("error");
  sink_error_stub s(error_str);

  bool valid_err = false;
  unsigned count = 0;
  // valid_err checks that the input error matches the expected string.
  // The count variable counts the number of calls.
  auto handler = [&count, &valid_err, error_str](const std::string& error) {
    valid_err = (error == error_str);
    ++count;
  };

  log_backend_impl backend;
  backend.set_error_handler(handler);
  backend.start();

  backend.push(build_log_entry(&s));

  // Stop the backend to ensure the entry has been processed.
  backend.stop();

  ASSERT_EQ(valid_err, true);
  ASSERT_EQ(count, 1);

  return true;
}

static bool when_handler_is_set_after_start_then_handler_is_not_used()
{
  sink_error_stub s("test");

  unsigned count = 0;
  // The count variable counts the number of calls.
  auto handler = [&count](const std::string& error) { ++count; };

  log_backend_impl backend;
  // We want to remove output to stderr by the default handler.
  backend.set_error_handler([](const std::string&) {});
  backend.start();
  backend.set_error_handler(handler);

  backend.push(build_log_entry(&s));

  // Stop the backend to ensure the entry has been processed.
  backend.stop();

  ASSERT_EQ(count, 0);

  return true;
}

static bool when_empty_handler_is_used_then_backend_does_not_crash()
{
  sink_error_stub s("test");

  log_backend_impl backend;
  // We want to remove output to stderr by the default handler.
  backend.set_error_handler({});
  backend.start();

  backend.push(build_log_entry(&s));

  // Stop the backend to ensure the entry has been processed.
  backend.stop();

  return true;
}

int main()
{
  TEST_FUNCTION(when_backend_is_started_then_is_started_returns_true);
  TEST_FUNCTION(
      when_backend_is_started_and_stopped_then_is_started_returns_false);
  TEST_FUNCTION(
      when_backend_is_not_started_then_pushed_log_entries_are_ignored);
  TEST_FUNCTION(
      when_backend_is_started_then_pushed_log_entries_are_sent_to_sink);
  TEST_FUNCTION(when_sink_write_fails_then_error_handler_is_invoked);
  TEST_FUNCTION(when_handler_is_set_after_start_then_handler_is_not_used);
  TEST_FUNCTION(when_empty_handler_is_used_then_backend_does_not_crash);

  return 0;
}
