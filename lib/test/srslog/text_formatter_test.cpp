/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#include "src/srslog/formatters/text_formatter.h"
#include "srslte/srslog/detail/log_entry_metadata.h"
#include "testing_helpers.h"
#include <numeric>

using namespace srslog;

/// Helper to build a log entry.
static detail::log_entry_metadata build_log_entry_metadata()
{
  // Create a time point 50000us from epoch.
  using tp_ty = std::chrono::time_point<std::chrono::high_resolution_clock>;
  tp_ty tp(std::chrono::microseconds(50000));

  fmt::dynamic_format_arg_store<fmt::printf_context> store;
  store.push_back(88);

  return {tp, {10, true}, "Text %d", std::move(store), "ABC", 'Z'};
}

static bool when_fully_filled_log_entry_then_everything_is_formatted()
{
  fmt::memory_buffer buffer;
  text_formatter{}.format(build_log_entry_metadata(), buffer);
  std::string result = fmt::to_string(buffer);
  std::string expected = "00:00:00.050000 [ABC ] [Z] [   10] Text 88\n";

  ASSERT_EQ(result, expected);

  return true;
}

static bool when_log_entry_without_name_is_passed_then_name_is_not_formatted()
{
  auto entry = build_log_entry_metadata();
  entry.log_name = "";

  fmt::memory_buffer buffer;
  text_formatter{}.format(std::move(entry), buffer);
  std::string result = fmt::to_string(buffer);
  std::string expected = "00:00:00.050000 [Z] [   10] Text 88\n";

  ASSERT_EQ(result, expected);

  return true;
}

static bool when_log_entry_without_tag_is_passed_then_tag_is_not_formatted()
{
  auto entry = build_log_entry_metadata();
  entry.log_tag = '\0';

  fmt::memory_buffer buffer;
  text_formatter{}.format(std::move(entry), buffer);
  std::string result = fmt::to_string(buffer);
  std::string expected = "00:00:00.050000 [ABC ] [   10] Text 88\n";

  ASSERT_EQ(result, expected);

  return true;
}

static bool
when_log_entry_without_context_is_passed_then_context_is_not_formatted()
{
  auto entry = build_log_entry_metadata();
  entry.context.enabled = false;

  fmt::memory_buffer buffer;
  text_formatter{}.format(std::move(entry), buffer);
  std::string result = fmt::to_string(buffer);
  std::string expected = "00:00:00.050000 [ABC ] [Z] Text 88\n";

  ASSERT_EQ(result, expected);

  return true;
}

static bool when_log_entry_with_hex_dump_is_passed_then_hex_dump_is_formatted()
{
  auto entry = build_log_entry_metadata();
  entry.hex_dump.resize(20);
  std::iota(entry.hex_dump.begin(), entry.hex_dump.end(), 0);

  fmt::memory_buffer buffer;
  text_formatter{}.format(std::move(entry), buffer);
  std::string result = fmt::to_string(buffer);
  std::string expected =
      "00:00:00.050000 [ABC ] [Z] [   10] Text 88\n"
      "    0000: 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f\n"
      "    0010: 10 11 12 13\n";

  ASSERT_EQ(result, expected);

  return true;
}

namespace {
DECLARE_METRIC("SNR", snr_t, float, "dB");
DECLARE_METRIC("PWR", pwr_t, int, "dBm");
DECLARE_METRIC("CenterFreq", cfreq_t, unsigned, "MHz");
DECLARE_METRIC_SET("RF", myset1, snr_t, pwr_t, cfreq_t);

DECLARE_METRIC("Throughput", thr_t, float, "MB/s");
DECLARE_METRIC("Address", ip_addr_t, std::string, "");
DECLARE_METRIC_SET("Network", myset2, thr_t, ip_addr_t);

using ctx_t = srslog::build_context_type<myset1, myset2>;
} // namespace

static bool
when_log_entry_with_only_context_is_passed_then_context_is_formatted()
{
  auto entry = build_log_entry_metadata();
  entry.fmtstring = "";
  ctx_t ctx("UL Context");

  ctx.get<myset1>().write<snr_t>(-55.1);
  ctx.get<myset1>().write<pwr_t>(-10);
  ctx.get<myset1>().write<cfreq_t>(1500);
  ctx.get<myset2>().write<thr_t>(150.01);
  ctx.get<myset2>().write<ip_addr_t>("192.168.1.0");

  fmt::memory_buffer buffer;
  text_formatter{}.format_ctx(ctx, std::move(entry), buffer);
  std::string result = fmt::to_string(buffer);
  std::string expected =
      "00:00:00.050000 [ABC ] [Z] [   10] Context dump for \"UL Context\"\n"
      "  RF\n"
      "    SNR: -55.1 dB\n"
      "    PWR: -10 dBm\n"
      "    CenterFreq: 1500 MHz\n"
      "  Network\n"
      "    Throughput: 150.01 MB/s\n"
      "    Address: 192.168.1.0\n";

  ASSERT_EQ(result, expected);

  return true;
}

static bool
when_log_entry_with_context_and_message_is_passed_then_context_is_formatted()
{
  auto entry = build_log_entry_metadata();
  ctx_t ctx("UL Context");

  ctx.get<myset1>().write<snr_t>(-55.1);
  ctx.get<myset1>().write<pwr_t>(-10);
  ctx.get<myset1>().write<cfreq_t>(1500);
  ctx.get<myset2>().write<thr_t>(150.01);
  ctx.get<myset2>().write<ip_addr_t>("192.168.1.0");

  fmt::memory_buffer buffer;
  text_formatter{}.format_ctx(ctx, std::move(entry), buffer);
  std::string result = fmt::to_string(buffer);
  std::string expected =
      "00:00:00.050000 [ABC ] [Z] [   10] [[RF_SNR: -55.1 dB, RF_PWR: -10 dBm, "
      "RF_CenterFreq: 1500 MHz] [Network_Throughput: 150.01 MB/s, "
      "Network_Address: 192.168.1.0]]: Text 88\n";

  ASSERT_EQ(result, expected);

  return true;
}

int main()
{
  TEST_FUNCTION(when_fully_filled_log_entry_then_everything_is_formatted);
  TEST_FUNCTION(
      when_log_entry_without_name_is_passed_then_name_is_not_formatted);
  TEST_FUNCTION(when_log_entry_without_tag_is_passed_then_tag_is_not_formatted);
  TEST_FUNCTION(
      when_log_entry_without_context_is_passed_then_context_is_not_formatted);
  TEST_FUNCTION(
      when_log_entry_with_hex_dump_is_passed_then_hex_dump_is_formatted);
  TEST_FUNCTION(
      when_log_entry_with_only_context_is_passed_then_context_is_formatted);
  TEST_FUNCTION(
      when_log_entry_with_context_and_message_is_passed_then_context_is_formatted);

  return 0;
}
