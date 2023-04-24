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

#include "src/srslog/formatters/json_formatter.h"
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
  fmt::dynamic_format_arg_store<fmt::printf_context> store;
  fmt::memory_buffer                                 buffer;
  json_formatter{}.format(build_log_entry_metadata(&store), buffer);
  std::string result   = fmt::to_string(buffer);
  std::string expected = "{\n"
                         "  \"log_entry\": \"Text 88\"\n"
                         "}\n";

  ASSERT_EQ(result, expected);

  return true;
}

static bool when_fully_filled_log_entry_with_hex_dump_then_everything_is_formatted()
{
  fmt::dynamic_format_arg_store<fmt::printf_context> store;
  auto                                               entry = build_log_entry_metadata(&store);
  entry.hex_dump.resize(12);
  std::iota(entry.hex_dump.begin(), entry.hex_dump.end(), 0);

  fmt::memory_buffer buffer;
  json_formatter{}.format(std::move(entry), buffer);
  std::string result   = fmt::to_string(buffer);
  std::string expected = "{\n"
                         "  \"log_entry\": \"Text 88\",\n"
                         "  \"hex_dump\": \"00 01 02 03 04 05 06 07 08 09 0a 0b\"\n"
                         "}\n";

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

using basic_ctx_t = srslog::build_context_type<myset1, myset2>;
} // namespace

static bool when_log_entry_with_only_basic_context_is_passed_then_context_is_formatted()
{
  auto entry      = build_log_entry_metadata(nullptr);
  entry.fmtstring = nullptr;
  basic_ctx_t ctx("UL Context");

  ctx.get<myset1>().write<snr_t>(-55.1);
  ctx.get<myset1>().write<pwr_t>(-10);
  ctx.get<myset1>().write<cfreq_t>(1500);
  ctx.get<myset2>().write<thr_t>(150.01);
  ctx.get<myset2>().write<ip_addr_t>("192.168.1.0");

  fmt::memory_buffer buffer;
  json_formatter{}.format_ctx(ctx, std::move(entry), buffer);
  std::string result   = fmt::to_string(buffer);
  std::string expected = "{\n"
                         "  \"RF\": {\n"
                         "    \"SNR\": -55.1,\n"
                         "    \"PWR\": -10,\n"
                         "    \"CenterFreq\": 1500\n"
                         "  },\n"
                         "  \"Network\": {\n"
                         "    \"Throughput\": 150.01,\n"
                         "    \"Address\": \"192.168.1.0\"\n"
                         "  }\n"
                         "}\n";

  ASSERT_EQ(result, expected);

  return true;
}

static bool when_log_entry_with_message_and_basic_context_is_passed_then_context_is_formatted()
{
  fmt::dynamic_format_arg_store<fmt::printf_context> store;
  auto                                               entry = build_log_entry_metadata(&store);
  basic_ctx_t                                        ctx("UL Context");

  ctx.get<myset1>().write<snr_t>(-55.1);
  ctx.get<myset1>().write<pwr_t>(-10);
  ctx.get<myset1>().write<cfreq_t>(1500);
  ctx.get<myset2>().write<thr_t>(150.01);
  ctx.get<myset2>().write<ip_addr_t>("192.168.1.0");

  fmt::memory_buffer buffer;
  json_formatter{}.format_ctx(ctx, std::move(entry), buffer);
  std::string result   = fmt::to_string(buffer);
  std::string expected = "{\n"
                         "  \"log_entry\": \"Text 88\",\n"
                         "  \"RF\": {\n"
                         "    \"SNR\": -55.1,\n"
                         "    \"PWR\": -10,\n"
                         "    \"CenterFreq\": 1500\n"
                         "  },\n"
                         "  \"Network\": {\n"
                         "    \"Throughput\": 150.01,\n"
                         "    \"Address\": \"192.168.1.0\"\n"
                         "  }\n"
                         "}\n";

  ASSERT_EQ(result, expected);

  return true;
}

namespace {
DECLARE_METRIC("bearer_id", bearer_id_t, unsigned, "");
DECLARE_METRIC("qci", qci_t, unsigned, "");
DECLARE_METRIC_SET("bearer_container", bearer_set, bearer_id_t, qci_t);

DECLARE_METRIC("ue_rnti", ue_rnti_t, unsigned, "");
DECLARE_METRIC("dl_cqi", dl_cqi_t, unsigned, "");
DECLARE_METRIC_LIST("bearer_list", bearer_list_t, std::vector<bearer_set>);
DECLARE_METRIC_SET("ue_container", ue_set, ue_rnti_t, dl_cqi_t, bearer_list_t);

DECLARE_METRIC("type", entry_type_t, std::string, "");
DECLARE_METRIC("sector_id", sector_id_t, unsigned, "");
DECLARE_METRIC_LIST("ue_list", ue_list_t, std::vector<ue_set>);
DECLARE_METRIC_SET("sector_metrics", sector_set, entry_type_t, sector_id_t, ue_list_t);

DECLARE_METRIC_LIST("sector_list", sector_list_t, std::vector<sector_set>);

using complex_ctx_t = srslog::build_context_type<sector_list_t>;
} // namespace

static bool when_log_entry_with_only_complex_context_is_passed_then_context_is_formatted()
{
  complex_ctx_t ctx("UL Context");
  auto          entry = build_log_entry_metadata(nullptr);
  entry.fmtstring     = nullptr;

  ctx.get<sector_list_t>().emplace_back();
  ctx.at<sector_list_t>(0).get<ue_list_t>().emplace_back();
  ctx.at<sector_list_t>(0).get<ue_list_t>().emplace_back();

  ctx.at<sector_list_t>(0).at<ue_list_t>(0).get<bearer_list_t>().emplace_back();
  ctx.at<sector_list_t>(0).at<ue_list_t>(0).get<bearer_list_t>().emplace_back();

  ctx.at<sector_list_t>(0).at<ue_list_t>(1).get<bearer_list_t>().emplace_back();
  ctx.at<sector_list_t>(0).at<ue_list_t>(1).get<bearer_list_t>().emplace_back();

  fmt::memory_buffer buffer;
  json_formatter{}.format_ctx(ctx, std::move(entry), buffer);
  std::string result   = fmt::to_string(buffer);
  std::string expected = "{\n"
                         "  \"sector_list\": [\n"
                         "    {\n"
                         "      \"sector_metrics\": {\n"
                         "        \"type\": \"\",\n"
                         "        \"sector_id\": 0,\n"
                         "        \"ue_list\": [\n"
                         "          {\n"
                         "            \"ue_container\": {\n"
                         "              \"ue_rnti\": 0,\n"
                         "              \"dl_cqi\": 0,\n"
                         "              \"bearer_list\": [\n"
                         "                {\n"
                         "                  \"bearer_container\": {\n"
                         "                    \"bearer_id\": 0,\n"
                         "                    \"qci\": 0\n"
                         "                  }\n"
                         "                },\n"
                         "                {\n"
                         "                  \"bearer_container\": {\n"
                         "                    \"bearer_id\": 0,\n"
                         "                    \"qci\": 0\n"
                         "                  }\n"
                         "                }\n"
                         "              ]\n"
                         "            }\n"
                         "          },\n"
                         "          {\n"
                         "            \"ue_container\": {\n"
                         "              \"ue_rnti\": 0,\n"
                         "              \"dl_cqi\": 0,\n"
                         "              \"bearer_list\": [\n"
                         "                {\n"
                         "                  \"bearer_container\": {\n"
                         "                    \"bearer_id\": 0,\n"
                         "                    \"qci\": 0\n"
                         "                  }\n"
                         "                },\n"
                         "                {\n"
                         "                  \"bearer_container\": {\n"
                         "                    \"bearer_id\": 0,\n"
                         "                    \"qci\": 0\n"
                         "                  }\n"
                         "                }\n"
                         "              ]\n"
                         "            }\n"
                         "          }\n"
                         "        ]\n"
                         "      }\n"
                         "    }\n"
                         "  ]\n"
                         "}\n";

  ASSERT_EQ(result, expected);

  return true;
}

namespace {
DECLARE_METRIC("list_metric2", list_metric2, unsigned, "");
DECLARE_METRIC_SET("metric_list_set", metric_list_set, list_metric2);
DECLARE_METRIC_LIST("metrics_list", metrics_list, std::vector<metric_list_set>);
DECLARE_METRIC("list_metric3", list_metric3, unsigned, "");
DECLARE_METRIC("list_metric4", list_metric4, unsigned, "");
using list_ctx_t = srslog::build_context_type<list_metric3, list_metric4, metrics_list>;
}; // namespace

static bool when_context_with_empty_list_is_passed_then_list_object_is_empty()
{
  list_ctx_t ctx("UL Context");
  auto       entry = build_log_entry_metadata(nullptr);
  entry.fmtstring  = nullptr;

  fmt::memory_buffer buffer;
  json_formatter{}.format_ctx(ctx, std::move(entry), buffer);
  std::string result   = fmt::to_string(buffer);
  std::string expected = "{\n"
                         "  \"list_metric3\": 0,\n"
                         "  \"list_metric4\": 0,\n"
                         "  \"metrics_list\": [\n"
                         "  ]\n"
                         "}\n";

  ASSERT_EQ(result, expected);

  return true;
}

int main()
{
  TEST_FUNCTION(when_fully_filled_log_entry_then_everything_is_formatted);
  TEST_FUNCTION(when_fully_filled_log_entry_with_hex_dump_then_everything_is_formatted);
  TEST_FUNCTION(when_log_entry_with_only_basic_context_is_passed_then_context_is_formatted);
  TEST_FUNCTION(when_log_entry_with_message_and_basic_context_is_passed_then_context_is_formatted);
  TEST_FUNCTION(when_log_entry_with_only_complex_context_is_passed_then_context_is_formatted);
  TEST_FUNCTION(when_context_with_empty_list_is_passed_then_list_object_is_empty);

  return 0;
}
