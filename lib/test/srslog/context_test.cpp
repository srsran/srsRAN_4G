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

#include "srsran/srslog/context.h"
#include "testing_helpers.h"
#include <vector>

using namespace srslog;

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

/// Builds a context for testing.
static ctx_t build_context()
{
  ctx_t ctx("Ctx");
  return ctx;
}

static bool when_context_is_built_then_context_name_is_valid()
{
  ctx_t ctx = build_context();

  ASSERT_EQ(ctx.name(), "Ctx");

  return true;
}

static bool when_context_is_built_then_metric_sets_names_are_valid()
{
  ctx_t ctx = build_context();

  ASSERT_EQ(ctx.get<myset1>().name(), std::string("RF"));
  ASSERT_EQ(ctx.get<myset2>().name(), std::string("Network"));

  return true;
}

static bool when_context_is_built_then_metric_names_are_valid()
{
  ctx_t ctx = build_context();

  ASSERT_EQ(ctx.get<myset1>().get<snr_t>().name(), std::string("SNR"));
  ASSERT_EQ(ctx.get<myset1>().get<pwr_t>().name(), std::string("PWR"));

  return true;
}

static bool when_context_is_built_then_metric_units_are_valid()
{
  ctx_t ctx = build_context();

  ASSERT_EQ(ctx.get<myset1>().get<snr_t>().units(), std::string("dB"));
  ASSERT_EQ(ctx.get<myset1>().get<pwr_t>().units(), std::string("dBm"));

  return true;
}

static bool when_metric_is_set_through_context_then_value_is_stored()
{
  ctx_t ctx   = build_context();
  float value = 10;

  ctx.get<myset1>().write<snr_t>(value);

  ASSERT_EQ(ctx.get<myset1>().read<snr_t>(), value);

  return true;
}

namespace {
DECLARE_METRIC("metric1", m1_t, float, "");
DECLARE_METRIC_SET("test_set_t", test_set_t, m1_t);
DECLARE_METRIC_LIST("vector", vector_metrics, std::vector<test_set_t>);
using ctx2_t = srslog::build_context_type<vector_metrics>;
} // namespace

static bool when_context_with_list_is_set_value_is_retrieved_correctly()
{
  ctx2_t ctx("test");
  float  val = 2;

  ctx.get<vector_metrics>().emplace_back();
  ctx.at<vector_metrics>(0).write<m1_t>(val);

  ASSERT_EQ(ctx.at<vector_metrics>(0).read<m1_t>(), val);

  return true;
}

int main()
{
  TEST_FUNCTION(when_context_is_built_then_context_name_is_valid);
  TEST_FUNCTION(when_context_is_built_then_metric_sets_names_are_valid);
  TEST_FUNCTION(when_context_is_built_then_metric_names_are_valid);
  TEST_FUNCTION(when_context_is_built_then_metric_units_are_valid);
  TEST_FUNCTION(when_metric_is_set_through_context_then_value_is_stored);
  TEST_FUNCTION(when_context_with_list_is_set_value_is_retrieved_correctly);

  return 0;
}
