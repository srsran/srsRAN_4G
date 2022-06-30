/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#include "srsran/srslog/srslog.h"
#include "test_dummies.h"
#include "testing_helpers.h"

using namespace srslog;

static constexpr char test_id1[] = "Test1";
static constexpr char test_id2[] = "Test2";

//: TODO: older compilers may not have defined this C++11 trait.
#if (defined(__clang__) && (__clang_major__ >= 5)) || (defined(__GNUG__) && (__GNUC__ >= 5))
static_assert(std::is_trivially_copyable<detail::memory_buffer>::value, "Expected to be trivially copyable");
#endif

static bool when_srslog_is_created_then_stdout_and_stderr_sinks_exist()
{
  ASSERT_NE(find_sink("stdout"), nullptr);
  ASSERT_NE(find_sink("stderr"), nullptr);

  return true;
}

static bool when_no_installed_default_formatter_then_a_formatter_is_returned()
{
  ASSERT_NE(get_default_log_formatter(), nullptr);

  return true;
}

static bool when_no_installed_default_sink_then_stdout_sink_is_used()
{
  sink& default_sink = get_default_sink();

  ASSERT_EQ(&default_sink, find_sink("stdout"));

  return true;
}

static bool when_fetching_channel_then_channel_instance_is_returned()
{
  log_channel& channel1 = fetch_log_channel(test_id1);
  log_channel& channel2 = fetch_log_channel(test_id2, fetch_stdout_sink(), {});

  ASSERT_EQ(channel1.id(), test_id1);
  ASSERT_EQ(channel2.id(), test_id2);

  return true;
}

static bool when_valid_id_is_passed_then_channel_is_found()
{
  log_channel* channel1 = find_log_channel(test_id1);
  log_channel* channel2 = find_log_channel(test_id2);

  ASSERT_NE(channel1, nullptr);
  ASSERT_EQ(channel1->id(), test_id1);

  ASSERT_NE(channel2, nullptr);
  ASSERT_EQ(channel2->id(), test_id2);

  return true;
}

static bool when_non_existent_id_is_passed_then_nothing_is_found()
{
  ASSERT_EQ(find_log_channel("non_existent_channel"), nullptr);

  return true;
}

static bool when_id_with_sharps_is_passed_then_channel_is_fetched_with_clean_id()
{
  log_channel& channel1 = fetch_log_channel("a1#a");
  log_channel& channel2 = fetch_log_channel("a2#a", fetch_stdout_sink(), {});

  ASSERT_EQ(channel1.id(), "a1a");
  ASSERT_EQ(channel2.id(), "a2a");

  return true;
}

static bool when_channel_already_exists_then_fetch_returns_channel()
{
  log_channel& channel1 = fetch_log_channel("b1");
  log_channel& channel2 = fetch_log_channel("b2", fetch_stdout_sink(), {});

  ASSERT_EQ(&channel1, &fetch_log_channel("b1"));
  ASSERT_EQ(&channel2, &fetch_log_channel("b2"));

  return true;
}

static constexpr char logger_id[]        = "TestLogger";
static constexpr char basic_logger_id1[] = "BasicTestLogger";
static constexpr char basic_logger_id2[] = "BasicTestLogger2";

static bool when_fetching_logger_then_logger_instance_is_returned()
{
  log_channel& error   = fetch_log_channel("logger.error");
  log_channel& warning = fetch_log_channel("logger.warning");
  log_channel& info    = fetch_log_channel("logger.info");
  log_channel& debug   = fetch_log_channel("logger.debug");

  auto& logger = fetch_logger<basic_logger>(logger_id, error, warning, info, debug);

  ASSERT_EQ(logger.id(), logger_id);

  return true;
}

static bool when_fetching_basic_logger_then_basic_logger_instance_is_returned()
{
  basic_logger& logger1 = fetch_basic_logger(basic_logger_id1);
  basic_logger& logger2 = fetch_basic_logger(basic_logger_id2, fetch_stdout_sink());

  ASSERT_EQ(logger1.id(), basic_logger_id1);
  ASSERT_EQ(logger2.id(), basic_logger_id2);

  return true;
}

static bool when_valid_id_and_type_is_passed_then_logger_is_found()
{
  auto* l = find_logger<basic_logger>(logger_id);

  ASSERT_NE(l, nullptr);
  ASSERT_EQ(l->id(), logger_id);

  return true;
}

static bool when_basic_logger_already_exists_then_fetch_returns_logger()
{
  basic_logger& logger1 = fetch_basic_logger("b1");
  basic_logger& logger2 = fetch_basic_logger("b2", fetch_stdout_sink());

  ASSERT_EQ(&logger1, &fetch_basic_logger("b1"));
  ASSERT_EQ(&logger2, &fetch_basic_logger("b2"));

  return true;
}

static bool when_invalid_id_with_valid_type_is_passed_then_no_logger_is_found()
{
  ASSERT_EQ(find_logger<basic_logger>("invalid"), nullptr);

  return true;
}

/// Dummy logger type
enum class dummy_levels { none, error, LAST };
struct dummy_logger_channels {
  log_channel& error;
};
using dummy_logger = build_logger_type<dummy_logger_channels, dummy_levels>;

static bool when_invalid_id_and_type_is_passed_then_no_logger_is_found()
{
  ASSERT_EQ(find_logger<dummy_logger>("invalid"), nullptr);

  return true;
}

static bool when_valid_id_with_invalid_type_is_passed_then_no_logger_is_found()
{
  ASSERT_EQ(find_logger<dummy_logger>(logger_id), nullptr);

  return true;
}

static constexpr char file_name[] = "file_fetch_test.txt";

static bool when_file_sink_is_fetched_then_sink_instance_is_returned()
{
  sink& s1 = fetch_file_sink(file_name);
  sink* s2 = find_sink(file_name);

  ASSERT_EQ(&s1, s2);
  ASSERT_NE(s2, nullptr);

  return true;
}

static bool when_invalid_id_is_passed_then_no_sink_is_found()
{
  ASSERT_EQ(find_sink("invalid"), nullptr);

  return true;
}

static bool when_setting_stderr_as_default_then_get_default_returns_stderr_sink()
{
  set_default_sink(fetch_stderr_sink());
  sink& default_sink = get_default_sink();

  ASSERT_EQ(&default_sink, &fetch_stderr_sink());

  return true;
}

static bool when_creating_non_default_stdout_and_stderr_sinks_then_they_are_created()
{
  auto& out1 = fetch_stdout_sink("stdout1");
  auto& err1 = fetch_stderr_sink("stderr1");

  ASSERT_EQ(&out1, find_sink("stdout1"));
  ASSERT_EQ(&err1, find_sink("stderr1"));

  return true;
}

static constexpr char custom_sink_name[] = "custom_sink";

static bool when_setting_custom_sink_first_time_then_insertion_succeeds()
{
  std::unique_ptr<sink> custom(new test_dummies::sink_dummy);
  sink*                 s = custom.get();

  bool ret = install_custom_sink(custom_sink_name, std::move(custom));
  ASSERT_EQ(ret, true);

  sink* found_sink = find_sink(custom_sink_name);
  ASSERT_NE(found_sink, nullptr);
  ASSERT_EQ(found_sink, s);

  return true;
}

static bool when_setting_custom_sink_twice_then_insertion_fails()
{
  std::unique_ptr<sink> custom(new test_dummies::sink_dummy);
  sink*                 s = custom.get();

  bool ret = install_custom_sink(custom_sink_name, std::move(custom));
  ASSERT_EQ(ret, false);

  sink* found_sink = find_sink(custom_sink_name);
  ASSERT_NE(found_sink, nullptr);
  ASSERT_NE(found_sink, s);

  return true;
}

static bool when_default_formatter_is_installed_then_getter_returns_same_formatter()
{
  auto f = std::unique_ptr<log_formatter>(new test_dummies::log_formatter_dummy);
  set_default_log_formatter(std::move(f));

  // Dummy formatter does not implement the clone method, thus the unique
  // pointer will be nullptr.
  auto f2 = get_default_log_formatter();
  ASSERT_EQ(f2.get(), nullptr);

  return true;
}

int main()
{
  TEST_FUNCTION(when_srslog_is_created_then_stdout_and_stderr_sinks_exist);
  TEST_FUNCTION(when_no_installed_default_formatter_then_a_formatter_is_returned);
  TEST_FUNCTION(when_no_installed_default_sink_then_stdout_sink_is_used);
  TEST_FUNCTION(when_fetching_channel_then_channel_instance_is_returned);
  TEST_FUNCTION(when_valid_id_is_passed_then_channel_is_found);
  TEST_FUNCTION(when_non_existent_id_is_passed_then_nothing_is_found);
  TEST_FUNCTION(when_id_with_sharps_is_passed_then_channel_is_fetched_with_clean_id);
  TEST_FUNCTION(when_channel_already_exists_then_fetch_returns_channel);
  TEST_FUNCTION(when_fetching_logger_then_logger_instance_is_returned);
  TEST_FUNCTION(when_fetching_basic_logger_then_basic_logger_instance_is_returned);
  TEST_FUNCTION(when_valid_id_and_type_is_passed_then_logger_is_found);
  TEST_FUNCTION(when_basic_logger_already_exists_then_fetch_returns_logger);
  TEST_FUNCTION(when_invalid_id_with_valid_type_is_passed_then_no_logger_is_found);
  TEST_FUNCTION(when_invalid_id_and_type_is_passed_then_no_logger_is_found);
  TEST_FUNCTION(when_valid_id_with_invalid_type_is_passed_then_no_logger_is_found);
  TEST_FUNCTION(when_file_sink_is_fetched_then_sink_instance_is_returned);
  TEST_FUNCTION(when_invalid_id_is_passed_then_no_sink_is_found);
  TEST_FUNCTION(when_setting_stderr_as_default_then_get_default_returns_stderr_sink);
  TEST_FUNCTION(when_creating_non_default_stdout_and_stderr_sinks_then_they_are_created);
  TEST_FUNCTION(when_setting_custom_sink_first_time_then_insertion_succeeds);
  TEST_FUNCTION(when_setting_custom_sink_twice_then_insertion_fails);
  TEST_FUNCTION(when_default_formatter_is_installed_then_getter_returns_same_formatter);

  return 0;
}
