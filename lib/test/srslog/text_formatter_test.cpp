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

#include "src/srslog/formatters/text_formatter.h"
#include "srsran/srslog/detail/log_entry_metadata.h"
#include "testing_helpers.h"
#include <numeric>

using namespace srslog;

/// Helper to build a log entry.
static detail::log_entry_metadata build_log_entry_metadata(fmt::dynamic_format_arg_store<fmt::printf_context>* store)
{
  // Create a time point 50000us from epoch.
  using tp_ty = std::chrono::time_point<std::chrono::high_resolution_clock>;
  tp_ty tp(std::chrono::microseconds(50000));

  if (store) {
    store->push_back(88);
  }

  return {tp, {10, true}, "Text %d", store, "ABC", 'Z'};
}

static bool when_fully_filled_log_entry_then_everything_is_formatted()
{
  fmt::memory_buffer                                 buffer;
  fmt::dynamic_format_arg_store<fmt::printf_context> store;
  text_formatter{}.format(build_log_entry_metadata(&store), buffer);
  std::string result   = fmt::to_string(buffer);
  std::string expected = "1970-01-01T00:00:00.050000 [ABC    ] [Z] [   10] Text 88\n";

  ASSERT_EQ(result, expected);

  return true;
}

static bool when_log_entry_without_name_is_passed_then_name_is_not_formatted()
{
  fmt::dynamic_format_arg_store<fmt::printf_context> store;
  auto                                               entry = build_log_entry_metadata(&store);
  entry.log_name                                           = "";

  fmt::memory_buffer buffer;
  text_formatter{}.format(std::move(entry), buffer);
  std::string result   = fmt::to_string(buffer);
  std::string expected = "1970-01-01T00:00:00.050000 [Z] [   10] Text 88\n";

  ASSERT_EQ(result, expected);

  return true;
}

static bool when_log_entry_without_tag_is_passed_then_tag_is_not_formatted()
{
  fmt::dynamic_format_arg_store<fmt::printf_context> store;
  auto                                               entry = build_log_entry_metadata(&store);
  entry.log_tag                                            = '\0';

  fmt::memory_buffer buffer;
  text_formatter{}.format(std::move(entry), buffer);
  std::string result   = fmt::to_string(buffer);
  std::string expected = "1970-01-01T00:00:00.050000 [ABC    ] [   10] Text 88\n";

  ASSERT_EQ(result, expected);

  return true;
}

static bool when_log_entry_without_context_is_passed_then_context_is_not_formatted()
{
  fmt::dynamic_format_arg_store<fmt::printf_context> store;
  auto                                               entry = build_log_entry_metadata(&store);
  entry.context.enabled                                    = false;

  fmt::memory_buffer buffer;
  text_formatter{}.format(std::move(entry), buffer);
  std::string result   = fmt::to_string(buffer);
  std::string expected = "1970-01-01T00:00:00.050000 [ABC    ] [Z] Text 88\n";

  ASSERT_EQ(result, expected);

  return true;
}

static bool when_log_entry_with_hex_dump_is_passed_then_hex_dump_is_formatted()
{
  fmt::dynamic_format_arg_store<fmt::printf_context> store;
  auto                                               entry = build_log_entry_metadata(&store);
  entry.hex_dump.resize(20);
  std::iota(entry.hex_dump.begin(), entry.hex_dump.end(), 0);

  fmt::memory_buffer buffer;
  text_formatter{}.format(std::move(entry), buffer);
  std::string result   = fmt::to_string(buffer);
  std::string expected = "1970-01-01T00:00:00.050000 [ABC    ] [Z] [   10] Text 88\n"
                         "    0000: 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f\n"
                         "    0010: 10 11 12 13\n";

  ASSERT_EQ(result, expected);

  return true;
}

namespace {
DECLARE_METRIC("SNR", snr_t, float, "dB");
DECLARE_METRIC("PWR", pwr_t, int, "dBm");
DECLARE_METRIC_SET("RF", rf_set, snr_t, pwr_t);

DECLARE_METRIC("Throughput", thr_t, float, "MB/s");
DECLARE_METRIC("Address", ip_addr_t, std::string, "");
DECLARE_METRIC_LIST("Antennas", antenna_list_t, std::vector<rf_set>);
DECLARE_METRIC_SET("ue_container", ue_set, thr_t, ip_addr_t, antenna_list_t);

DECLARE_METRIC("type", entry_type_t, std::string, "");
DECLARE_METRIC("sector_id", sector_id_t, unsigned, "");
DECLARE_METRIC_LIST("ue_list", ue_list_t, std::vector<ue_set>);
DECLARE_METRIC_SET("sector_metrics", sector_set, entry_type_t, sector_id_t, ue_list_t);

DECLARE_METRIC_LIST("sector_list", sector_list_t, std::vector<sector_set>);

using complex_ctx_t = srslog::build_context_type<sector_list_t>;
} // namespace

/// Builds an instance of a complex context object filled in with some random
/// data.
static complex_ctx_t build_complex_context()
{
  complex_ctx_t ctx("Complex Context");

  ctx.get<sector_list_t>().emplace_back();
  ctx.at<sector_list_t>(0).write<entry_type_t>("event");
  ctx.at<sector_list_t>(0).write<sector_id_t>(1);

  ctx.at<sector_list_t>(0).get<ue_list_t>().emplace_back();
  ctx.at<sector_list_t>(0).at<ue_list_t>(0).write<thr_t>(1.2);
  ctx.at<sector_list_t>(0).at<ue_list_t>(0).write<ip_addr_t>("10.20.30.40");

  ctx.at<sector_list_t>(0).get<ue_list_t>().emplace_back();
  ctx.at<sector_list_t>(0).at<ue_list_t>(1).write<thr_t>(10.2);
  ctx.at<sector_list_t>(0).at<ue_list_t>(1).write<ip_addr_t>("10.20.30.41");

  ctx.at<sector_list_t>(0).at<ue_list_t>(0).get<antenna_list_t>().emplace_back();
  ctx.at<sector_list_t>(0).at<ue_list_t>(0).at<antenna_list_t>(0).write<snr_t>(5.1);
  ctx.at<sector_list_t>(0).at<ue_list_t>(0).at<antenna_list_t>(0).write<pwr_t>(-11.5);

  ctx.at<sector_list_t>(0).at<ue_list_t>(0).get<antenna_list_t>().emplace_back();
  ctx.at<sector_list_t>(0).at<ue_list_t>(0).at<antenna_list_t>(1).write<snr_t>(10.1);
  ctx.at<sector_list_t>(0).at<ue_list_t>(0).at<antenna_list_t>(1).write<pwr_t>(-20.5);

  ctx.at<sector_list_t>(0).at<ue_list_t>(1).get<antenna_list_t>().emplace_back();
  ctx.at<sector_list_t>(0).at<ue_list_t>(1).at<antenna_list_t>(0).write<snr_t>(20.1);
  ctx.at<sector_list_t>(0).at<ue_list_t>(1).at<antenna_list_t>(0).write<pwr_t>(-30.5);
  ctx.at<sector_list_t>(0).at<ue_list_t>(1).get<antenna_list_t>().emplace_back();
  ctx.at<sector_list_t>(0).at<ue_list_t>(1).at<antenna_list_t>(1).write<snr_t>(30.1);
  ctx.at<sector_list_t>(0).at<ue_list_t>(1).at<antenna_list_t>(1).write<pwr_t>(-40.5);

  return ctx;
}

static bool when_log_entry_with_only_context_is_passed_then_context_is_formatted()
{
  auto ctx        = build_complex_context();
  auto entry      = build_log_entry_metadata(nullptr);
  entry.fmtstring = nullptr;

  fmt::memory_buffer buffer;
  text_formatter{}.format_ctx(ctx, std::move(entry), buffer);
  std::string result   = fmt::to_string(buffer);
  std::string expected = "1970-01-01T00:00:00.050000 [ABC    ] [Z] [   10] Context dump for "
                         "\"Complex Context\"\n"
                         "  > List: sector_list\n"
                         "    > Set: sector_metrics\n"
                         "      type: event\n"
                         "      sector_id: 1\n"
                         "      > List: ue_list\n"
                         "        > Set: ue_container\n"
                         "          Throughput: 1.2 MB/s\n"
                         "          Address: 10.20.30.40\n"
                         "          > List: Antennas\n"
                         "            > Set: RF\n"
                         "              SNR: 5.1 dB\n"
                         "              PWR: -11 dBm\n"
                         "            > Set: RF\n"
                         "              SNR: 10.1 dB\n"
                         "              PWR: -20 dBm\n"
                         "        > Set: ue_container\n"
                         "          Throughput: 10.2 MB/s\n"
                         "          Address: 10.20.30.41\n"
                         "          > List: Antennas\n"
                         "            > Set: RF\n"
                         "              SNR: 20.1 dB\n"
                         "              PWR: -30 dBm\n"
                         "            > Set: RF\n"
                         "              SNR: 30.1 dB\n"
                         "              PWR: -40 dBm\n";

  ASSERT_EQ(result, expected);

  return true;
}

static bool when_log_entry_with_context_and_message_is_passed_then_context_is_formatted()
{
  fmt::dynamic_format_arg_store<fmt::printf_context> store;
  auto                                               entry = build_log_entry_metadata(&store);
  auto                                               ctx   = build_complex_context();

  fmt::memory_buffer buffer;
  text_formatter{}.format_ctx(ctx, std::move(entry), buffer);
  std::string result   = fmt::to_string(buffer);
  std::string expected = "1970-01-01T00:00:00.050000 [ABC    ] [Z] [   10] [[sector_metrics_type: event, "
                         "sector_metrics_sector_id: 1, [ue_container_Throughput: 1.2 MB/s, "
                         "ue_container_Address: 10.20.30.40, [RF_SNR: 5.1 dB, RF_PWR: -11 "
                         "dBm][RF_SNR: 10.1 dB, RF_PWR: -20 dBm]][ue_container_Throughput: 10.2 "
                         "MB/s, ue_container_Address: 10.20.30.41, [RF_SNR: 20.1 dB, RF_PWR: -30 "
                         "dBm][RF_SNR: 30.1 dB, RF_PWR: -40 dBm]]]]: Text 88\n";

  ASSERT_EQ(result, expected);

  return true;
}

int main()
{
  TEST_FUNCTION(when_fully_filled_log_entry_then_everything_is_formatted);
  TEST_FUNCTION(when_log_entry_without_name_is_passed_then_name_is_not_formatted);
  TEST_FUNCTION(when_log_entry_without_tag_is_passed_then_tag_is_not_formatted);
  TEST_FUNCTION(when_log_entry_without_context_is_passed_then_context_is_not_formatted);
  TEST_FUNCTION(when_log_entry_with_hex_dump_is_passed_then_hex_dump_is_formatted);
  TEST_FUNCTION(when_log_entry_with_only_context_is_passed_then_context_is_formatted);
  TEST_FUNCTION(when_log_entry_with_context_and_message_is_passed_then_context_is_formatted);

  return 0;
}
