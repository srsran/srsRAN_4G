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

#include "srsran/srslog/logger.h"
#include "test_dummies.h"
#include "testing_helpers.h"

using namespace srslog;

static constexpr char logger_id[] = "TestLogger";

namespace {

/// Definition of a three level logger
enum class test_logger_levels { none, error, warning, info, LAST };
struct test_logger_channels {
  log_channel& error;
  log_channel& warning;
  log_channel& info;
};
using test_logger = build_logger_type<test_logger_channels, test_logger_levels>;

} // namespace

static bool when_logger_is_created_then_id_matches_expected_value()
{
  test_dummies::backend_dummy backend;
  test_dummies::sink_dummy    s;
  log_channel                 error("err", s, backend);
  log_channel                 warning("warning", s, backend);
  log_channel                 info("info", s, backend);

  test_logger logger(logger_id, error, warning, info);

  ASSERT_EQ(logger.id(), logger_id);

  return true;
}

static bool when_level_is_set_to_error_then_info_and_warning_is_disabled()
{
  test_dummies::backend_dummy backend;
  test_dummies::sink_dummy    s;
  log_channel                 error("err", s, backend);
  log_channel                 warning("warning", s, backend);
  log_channel                 info("info", s, backend);

  test_logger logger(logger_id, error, warning, info);
  logger.set_level(test_logger_levels::error);

  ASSERT_EQ(logger.error.enabled(), true);
  ASSERT_EQ(logger.warning.enabled(), false);
  ASSERT_EQ(logger.info.enabled(), false);

  return true;
}

static bool when_level_is_set_to_warning_then_info_is_disabled()
{
  test_dummies::backend_dummy backend;
  test_dummies::sink_dummy    s;
  log_channel                 error("err", s, backend);
  log_channel                 warning("warning", s, backend);
  log_channel                 info("info", s, backend);

  test_logger logger(logger_id, error, warning, info);
  logger.set_level(test_logger_levels::warning);

  ASSERT_EQ(logger.error.enabled(), true);
  ASSERT_EQ(logger.warning.enabled(), true);
  ASSERT_EQ(logger.info.enabled(), false);

  return true;
}

static bool when_level_is_set_to_info_then_all_are_enabled()
{
  test_dummies::backend_dummy backend;
  test_dummies::sink_dummy    s;
  log_channel                 error("err", s, backend);
  log_channel                 warning("warning", s, backend);
  log_channel                 info("info", s, backend);

  test_logger logger(logger_id, error, warning, info);
  logger.set_level(test_logger_levels::info);

  ASSERT_EQ(logger.error.enabled(), true);
  ASSERT_EQ(logger.warning.enabled(), true);
  ASSERT_EQ(logger.info.enabled(), true);

  return true;
}

static bool when_level_is_set_to_none_then_all_are_disabled()
{
  test_dummies::backend_dummy backend;
  test_dummies::sink_dummy    s;
  log_channel                 error("err", s, backend);
  log_channel                 warning("warning", s, backend);
  log_channel                 info("info", s, backend);

  test_logger logger(logger_id, error, warning, info);
  logger.set_level(test_logger_levels::none);

  ASSERT_EQ(logger.error.enabled(), false);
  ASSERT_EQ(logger.warning.enabled(), false);
  ASSERT_EQ(logger.info.enabled(), false);

  return true;
}

int main()
{
  TEST_FUNCTION(when_logger_is_created_then_id_matches_expected_value);
  TEST_FUNCTION(when_level_is_set_to_error_then_info_and_warning_is_disabled);
  TEST_FUNCTION(when_level_is_set_to_warning_then_info_is_disabled);
  TEST_FUNCTION(when_level_is_set_to_info_then_all_are_enabled);
  TEST_FUNCTION(when_level_is_set_to_none_then_all_are_disabled);

  return 0;
}
