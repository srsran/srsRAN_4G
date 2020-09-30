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

#include "src/srslog/formatter.h"
#include "testing_helpers.h"
#include <numeric>

using namespace srslog;

/// Helper to build a log entry.
static detail::log_entry build_log_entry()
{
  // Create a time point 50000us from epoch.
  using tp_ty = std::chrono::time_point<std::chrono::high_resolution_clock>;
  tp_ty tp(std::chrono::microseconds(50000));

  fmt::dynamic_format_arg_store<fmt::printf_context> store;
  store.push_back(88);

  return {nullptr, tp, {10, true}, "Text %d", std::move(store), "ABC", 'Z'};
}

static bool when_fully_filled_log_entry_then_result_everything_is_formatted()
{
  std::string result = format_log_entry_to_text(build_log_entry());
  std::string expected = "00:00:00.050000 [ABC ] [Z] [   10] Text 88\n";

  ASSERT_EQ(result, expected);

  return true;
}

static bool when_log_entry_without_name_is_passed_then_name_is_not_formatted()
{
  auto entry = build_log_entry();
  entry.log_name = "";

  std::string result = format_log_entry_to_text(std::move(entry));
  std::string expected = "00:00:00.050000 [Z] [   10] Text 88\n";

  ASSERT_EQ(result, expected);

  return true;
}

static bool when_log_entry_without_tag_is_passed_then_tag_is_not_formatted()
{
  auto entry = build_log_entry();
  entry.log_tag = '\0';

  std::string result = format_log_entry_to_text(std::move(entry));
  std::string expected = "00:00:00.050000 [ABC ] [   10] Text 88\n";

  ASSERT_EQ(result, expected);

  return true;
}

static bool
when_log_entry_without_context_is_passed_then_context_is_not_formatted()
{
  auto entry = build_log_entry();
  entry.context.enabled = false;

  std::string result = format_log_entry_to_text(std::move(entry));
  std::string expected = "00:00:00.050000 [ABC ] [Z] Text 88\n";

  ASSERT_EQ(result, expected);

  return true;
}

static bool when_log_entry_with_hex_dump_is_passed_then_hex_dump_is_formatted()
{
  auto entry = build_log_entry();
  entry.hex_dump.resize(20);
  std::iota(entry.hex_dump.begin(), entry.hex_dump.end(), 0);

  std::string result = format_log_entry_to_text(std::move(entry));
  std::string expected =
      "00:00:00.050000 [ABC ] [Z] [   10] Text 88\n"
      "    0000: 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f\n"
      "    0010: 10 11 12 13\n";

  ASSERT_EQ(result, expected);

  return true;
}

int main()
{
  TEST_FUNCTION(
      when_fully_filled_log_entry_then_result_everything_is_formatted);
  TEST_FUNCTION(
      when_log_entry_without_name_is_passed_then_name_is_not_formatted);
  TEST_FUNCTION(when_log_entry_without_tag_is_passed_then_tag_is_not_formatted);
  TEST_FUNCTION(
      when_log_entry_without_context_is_passed_then_context_is_not_formatted);
  TEST_FUNCTION(
      when_log_entry_with_hex_dump_is_passed_then_hex_dump_is_formatted);

  return 0;
}
