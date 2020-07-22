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

static const char* const test_id = "Test";

//:FIXME: older compilers may not have defined this C++11 trait.
#if (defined(__clang__) && (__clang_major__ >= 5)) ||                          \
    (defined(__GNUG__) && (__GNUC__ >= 5))
static_assert(std::is_trivially_copyable<detail::memory_buffer>::value,
              "Expected to be trivially copyable");
#endif

namespace {

/// A Dummy implementation of a sink.
class sink_dummy : public sink
{
public:
  detail::error_string write(detail::memory_buffer buffer) override
  {
    return {};
  }

  detail::error_string flush() override { return {}; }
};

} // namespace

static bool when_no_channel_exists_then_channel_is_created()
{
  sink_dummy s;
  log_channel* channel = create_log_channel(test_id, s);

  ASSERT_NE(channel, nullptr);
  ASSERT_EQ(channel->id(), test_id);

  return true;
}

static bool when_channel_already_exists_then_nullptr_is_returned()
{
  sink_dummy s;
  log_channel* channel = create_log_channel(test_id, s);

  ASSERT_EQ(channel, nullptr);

  return true;
}

static bool when_valid_id_is_passed_then_channel_is_found()
{
  log_channel* c = find_log_channel(test_id);

  ASSERT_NE(c, nullptr);
  ASSERT_EQ(c->id(), test_id);

  return true;
}

static bool when_invalid_id_is_passed_then_nothing_is_found()
{
  ASSERT_EQ(find_log_channel("invalid"), nullptr);

  return true;
}

static const char* const logger_id = "TestLogger";

static bool when_no_logger_exists_then_logger_is_created()
{
  sink_dummy s;
  log_channel& error = *create_log_channel("logger#error", s);
  log_channel& warning = *create_log_channel("logger#warning", s);
  log_channel& info = *create_log_channel("logger#info", s);
  basic_logger* logger =
      create_logger<basic_logger>(logger_id, error, warning, info);

  ASSERT_NE(logger, nullptr);
  ASSERT_EQ(logger->id(), logger_id);

  return true;
}

static bool when_logger_already_exists_then_nullptr_is_returned()
{
  log_channel& error = *find_log_channel("logger#error");
  log_channel& warning = *find_log_channel("logger#warning");
  log_channel& info = *find_log_channel("logger#info");
  basic_logger* logger =
      create_logger<basic_logger>(logger_id, error, warning, info);

  ASSERT_EQ(logger, nullptr);

  return true;
}

static bool when_valid_id_and_type_is_passed_then_logger_is_found()
{
  basic_logger* l = find_logger<basic_logger>(logger_id);

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

static bool when_no_sink_exists_then_sink_is_created()
{
  sink* s = create_stdout_sink();

  ASSERT_NE(s, nullptr);

  return true;
}

static bool when_sink_already_exists_then_nullptr_is_returned()
{
  sink* s = create_stdout_sink();

  ASSERT_EQ(s, nullptr);

  return true;
}

static bool when_valid_id_is_passed_then_sink_is_found()
{
  sink* s = find_sink("stdout");

  ASSERT_NE(s, nullptr);

  return true;
}

static bool when_invalid_id_is_passed_then_no_sink_is_found()
{
  ASSERT_EQ(find_sink("invalid"), nullptr);

  return true;
}

int main()
{
  TEST_FUNCTION(when_no_channel_exists_then_channel_is_created);
  TEST_FUNCTION(when_channel_already_exists_then_nullptr_is_returned);
  TEST_FUNCTION(when_valid_id_is_passed_then_channel_is_found);
  TEST_FUNCTION(when_invalid_id_is_passed_then_nothing_is_found);
  TEST_FUNCTION(when_no_logger_exists_then_logger_is_created);
  TEST_FUNCTION(when_logger_already_exists_then_nullptr_is_returned);
  TEST_FUNCTION(when_valid_id_and_type_is_passed_then_logger_is_found);
  TEST_FUNCTION(
      when_invalid_id_with_valid_type_is_passed_then_no_logger_is_found);
  TEST_FUNCTION(when_invalid_id_and_type_is_passed_then_no_logger_is_found);
  TEST_FUNCTION(
      when_valid_id_with_invalid_type_is_passed_then_no_logger_is_found);
  TEST_FUNCTION(when_no_sink_exists_then_sink_is_created);
  TEST_FUNCTION(when_sink_already_exists_then_nullptr_is_returned);
  TEST_FUNCTION(when_valid_id_is_passed_then_sink_is_found);
  TEST_FUNCTION(when_invalid_id_is_passed_then_no_sink_is_found);

  return 0;
}
