/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#include "srsran/srslog/log_channel.h"
#include "test_dummies.h"
#include "testing_helpers.h"

using namespace srslog;

static bool when_log_channel_is_created_then_id_matches_expected_value()
{
  test_dummies::backend_dummy backend;
  test_dummies::sink_dummy    s;
  log_channel                 log("id", s, backend);

  ASSERT_EQ(log.id(), "id");

  return true;
}

static bool when_log_channel_is_disabled_then_enabled_returns_false()
{
  test_dummies::backend_dummy backend;
  test_dummies::sink_dummy    s;
  log_channel                 log("id", s, backend);

  log.set_enabled(false);

  ASSERT_EQ(log.enabled(), false);

  return true;
}

static bool when_log_channel_is_enabled_then_enabled_returns_true()
{
  test_dummies::backend_dummy backend;
  test_dummies::sink_dummy    s;
  log_channel                 log("id", s, backend);

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
  void start(srslog::backend_priority priority) override {}

  bool push(detail::log_entry&& entry) override
  {
    e = std::move(entry);
    ++count;
    return true;
  }

  bool is_running() const override { return true; }

  fmt::dynamic_format_arg_store<fmt::printf_context>* alloc_arg_store() override { return &store; }

  unsigned push_invocation_count() const { return count; }

  const detail::log_entry& last_entry() const { return e; }

private:
  unsigned                                           count = 0;
  detail::log_entry                                  e;
  fmt::dynamic_format_arg_store<fmt::printf_context> store;
};

} // namespace

static bool when_logging_in_log_channel_then_log_entry_is_pushed_into_the_backend()
{
  backend_spy              backend;
  test_dummies::sink_dummy s;
  log_channel              log("id", s, backend);

  log("test", 42, "Hello");

  ASSERT_EQ(backend.push_invocation_count(), 1);

  return true;
}

static bool when_logging_in_disabled_log_channel_then_log_entry_is_ignored()
{
  backend_spy              backend;
  test_dummies::sink_dummy s;
  log_channel              log("id", s, backend);

  log.set_enabled(false);
  log("test", 42, "Hello");

  ASSERT_EQ(backend.push_invocation_count(), 0);

  return true;
}

static bool when_logging_then_filled_in_log_entry_is_pushed_into_the_backend()
{
  backend_spy              backend;
  test_dummies::sink_dummy s;

  std::string name = "name";
  char        tag  = 'A';

  log_channel log("id", s, backend, {name, tag, true});

  uint32_t ctx = 10;

  log.set_context(ctx);
  log("test", 42, "Hello");

  ASSERT_EQ(backend.push_invocation_count(), 1);

  const detail::log_entry& entry = backend.last_entry();
  ASSERT_EQ(&s, entry.s);
  ASSERT_NE(entry.format_func, nullptr);
  ASSERT_NE(entry.metadata.tp.time_since_epoch().count(), 0);
  ASSERT_EQ(entry.metadata.context.value, ctx);
  ASSERT_EQ(entry.metadata.context.enabled, true);
  ASSERT_EQ(entry.metadata.fmtstring, std::string("test"));
  ASSERT_EQ(entry.metadata.log_name, name);
  ASSERT_EQ(entry.metadata.log_tag, tag);
  ASSERT_EQ(entry.metadata.hex_dump.empty(), true);

  return true;
}

static bool when_logging_with_hex_dump_then_filled_in_log_entry_is_pushed_into_the_backend()
{
  backend_spy              backend;
  test_dummies::sink_dummy s;

  std::string name = "name";
  char        tag  = 'A';

  log_channel log("id", s, backend, {name, tag, true});

  uint32_t ctx = 4;

  log.set_context(ctx);
  log.set_hex_dump_max_size(4);
  uint8_t hex[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
  log(hex, sizeof(hex), "test", 42, "Hello");

  ASSERT_EQ(backend.push_invocation_count(), 1);

  const detail::log_entry& entry = backend.last_entry();
  ASSERT_EQ(&s, entry.s);
  ASSERT_NE(entry.format_func, nullptr);
  ASSERT_NE(entry.metadata.tp.time_since_epoch().count(), 0);
  ASSERT_EQ(entry.metadata.context.value, ctx);
  ASSERT_EQ(entry.metadata.context.enabled, true);
  ASSERT_EQ(entry.metadata.fmtstring, std::string("test"));
  ASSERT_EQ(entry.metadata.log_name, name);
  ASSERT_EQ(entry.metadata.log_tag, tag);
  ASSERT_EQ(entry.metadata.hex_dump.size(), 4);
  ASSERT_EQ(std::equal(entry.metadata.hex_dump.begin(), entry.metadata.hex_dump.end(), std::begin(hex)), true);

  return true;
}

static bool when_hex_array_length_is_less_than_hex_log_max_size_then_array_length_is_used()
{
  backend_spy              backend;
  test_dummies::sink_dummy s;

  std::string name = "name";

  log_channel log("id", s, backend);

  log.set_hex_dump_max_size(10);
  uint8_t hex[] = {0, 1, 2};
  log(hex, sizeof(hex), "test", 42, "Hello");

  ASSERT_EQ(backend.push_invocation_count(), 1);

  const detail::log_entry& entry = backend.last_entry();
  ASSERT_EQ(entry.metadata.hex_dump.size(), 3);
  ASSERT_EQ(std::equal(entry.metadata.hex_dump.begin(), entry.metadata.hex_dump.end(), std::begin(hex)), true);

  return true;
}

namespace {

DECLARE_METRIC("SNR", snr_t, int, "dB");
DECLARE_METRIC_SET("myset", my_set, snr_t);
using my_ctx = srslog::build_context_type<my_set>;

} // namespace

static bool when_logging_with_context_then_filled_in_log_entry_is_pushed_into_the_backend()
{
  backend_spy              backend;
  test_dummies::sink_dummy s;

  std::string name = "name";
  char        tag  = 'A';

  log_channel log("id", s, backend, {name, tag, true});

  uint32_t ctx_value = 4;
  log.set_context(ctx_value);

  my_ctx ctx("myctx");
  log(ctx);

  ASSERT_EQ(backend.push_invocation_count(), 1);

  const detail::log_entry& entry = backend.last_entry();
  ASSERT_EQ(&s, entry.s);
  ASSERT_NE(entry.format_func, nullptr);
  ASSERT_NE(entry.metadata.tp.time_since_epoch().count(), 0);
  ASSERT_EQ(entry.metadata.context.value, ctx_value);
  ASSERT_EQ(entry.metadata.context.enabled, true);
  ASSERT_EQ(entry.metadata.log_name, name);
  ASSERT_EQ(entry.metadata.log_tag, tag);
  ASSERT_EQ(entry.metadata.hex_dump.empty(), true);

  return true;
}

static bool when_logging_with_context_and_message_then_filled_in_log_entry_is_pushed_into_the_backend()
{
  backend_spy              backend;
  test_dummies::sink_dummy s;

  std::string name = "name";
  char        tag  = 'A';

  log_channel log("id", s, backend, {name, tag, true});

  uint32_t ctx_value = 4;
  log.set_context(ctx_value);

  my_ctx ctx("myctx");
  log(ctx, "test", 10, 3.3);

  ASSERT_EQ(backend.push_invocation_count(), 1);

  const detail::log_entry& entry = backend.last_entry();
  ASSERT_EQ(&s, entry.s);
  ASSERT_NE(entry.format_func, nullptr);
  ASSERT_NE(entry.metadata.tp.time_since_epoch().count(), 0);
  ASSERT_EQ(entry.metadata.context.value, ctx_value);
  ASSERT_EQ(entry.metadata.context.enabled, true);
  ASSERT_EQ(entry.metadata.fmtstring, std::string("test"));
  ASSERT_EQ(entry.metadata.log_name, name);
  ASSERT_EQ(entry.metadata.log_tag, tag);
  ASSERT_EQ(entry.metadata.hex_dump.empty(), true);

  return true;
}

int main()
{
  TEST_FUNCTION(when_log_channel_is_created_then_id_matches_expected_value);
  TEST_FUNCTION(when_log_channel_is_disabled_then_enabled_returns_false);
  TEST_FUNCTION(when_log_channel_is_enabled_then_enabled_returns_true);
  TEST_FUNCTION(when_logging_in_log_channel_then_log_entry_is_pushed_into_the_backend);
  TEST_FUNCTION(when_logging_in_disabled_log_channel_then_log_entry_is_ignored);
  TEST_FUNCTION(when_logging_then_filled_in_log_entry_is_pushed_into_the_backend);
  TEST_FUNCTION(when_logging_with_hex_dump_then_filled_in_log_entry_is_pushed_into_the_backend);
  TEST_FUNCTION(when_hex_array_length_is_less_than_hex_log_max_size_then_array_length_is_used);
  TEST_FUNCTION(when_logging_with_context_then_filled_in_log_entry_is_pushed_into_the_backend);
  TEST_FUNCTION(when_logging_with_context_and_message_then_filled_in_log_entry_is_pushed_into_the_backend);

  return 0;
}
