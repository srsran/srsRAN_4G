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

#include "srslte/srslog/sink.h"
#include "srslte/srslog/srslog.h"
#include "testing_helpers.h"

using namespace srslog;

static constexpr char test_id1[] = "Test1";
static constexpr char test_id2[] = "Test2";

//:TODO: older compilers may not have defined this C++11 trait.
#if (defined(__clang__) && (__clang_major__ >= 5)) ||                          \
    (defined(__GNUG__) && (__GNUC__ >= 5))
static_assert(std::is_trivially_copyable<detail::memory_buffer>::value,
              "Expected to be trivially copyable");
#endif

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

static bool
when_id_with_sharps_is_passed_then_channel_is_fetched_with_clean_id()
{
  log_channel& channel1 = fetch_log_channel("a1#a");
  log_channel& channel2 = fetch_log_channel("a2#a", fetch_stdout_sink(), {});

  ASSERT_EQ(channel1.id(), "a1a");
  ASSERT_EQ(channel2.id(), "a2a");

  return true;
}

static constexpr char logger_id[] = "TestLogger";
static constexpr char basic_logger_id1[] = "BasicTestLogger";
static constexpr char basic_logger_id2[] = "BasicTestLogger2";

static bool when_fetching_logger_then_logger_instance_is_returned()
{
  log_channel& error = fetch_log_channel("logger.error");
  log_channel& warning = fetch_log_channel("logger.warning");
  log_channel& info = fetch_log_channel("logger.info");
  log_channel& debug = fetch_log_channel("logger.debug");

  auto& logger =
      fetch_logger<basic_logger>(logger_id, error, warning, info, debug);

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

static bool when_invalid_id_with_valid_type_is_passed_then_no_logger_is_found()
{
  ASSERT_EQ(find_logger<basic_logger>("invalid"), nullptr);

  return true;
}

/// Dummy logger type
enum class dummy_levels { error, LAST };
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
  fetch_file_sink(file_name);
  sink* s = find_sink(file_name);

  ASSERT_NE(s, nullptr);

  return true;
}

static bool when_invalid_id_is_passed_then_no_sink_is_found()
{
  ASSERT_EQ(find_sink("invalid"), nullptr);

  return true;
}

static bool when_no_installed_default_sink_then_stdout_sink_is_used()
{
  sink& default_sink = get_default_sink();

  ASSERT_EQ(&default_sink, &fetch_stdout_sink());

  return true;
}

static bool
when_setting_stderr_as_default_then_get_default_returns_stderr_sink()
{
  set_default_sink(fetch_stderr_sink());
  sink& default_sink = get_default_sink();

  ASSERT_EQ(&default_sink, &fetch_stderr_sink());

  return true;
}

int main()
{
  TEST_FUNCTION(when_fetching_channel_then_channel_instance_is_returned);
  TEST_FUNCTION(when_valid_id_is_passed_then_channel_is_found);
  TEST_FUNCTION(when_non_existent_id_is_passed_then_nothing_is_found);
  TEST_FUNCTION(
      when_id_with_sharps_is_passed_then_channel_is_fetched_with_clean_id);
  TEST_FUNCTION(when_fetching_logger_then_logger_instance_is_returned);
  TEST_FUNCTION(
      when_fetching_basic_logger_then_basic_logger_instance_is_returned);
  TEST_FUNCTION(when_valid_id_and_type_is_passed_then_logger_is_found);
  TEST_FUNCTION(
      when_invalid_id_with_valid_type_is_passed_then_no_logger_is_found);
  TEST_FUNCTION(when_invalid_id_and_type_is_passed_then_no_logger_is_found);
  TEST_FUNCTION(
      when_valid_id_with_invalid_type_is_passed_then_no_logger_is_found);
  TEST_FUNCTION(when_file_sink_is_fetched_then_sink_instance_is_returned);
  TEST_FUNCTION(when_invalid_id_is_passed_then_no_sink_is_found);
  TEST_FUNCTION(when_no_installed_default_sink_then_stdout_sink_is_used);
  TEST_FUNCTION(
      when_setting_stderr_as_default_then_get_default_returns_stderr_sink);

  return 0;
}
