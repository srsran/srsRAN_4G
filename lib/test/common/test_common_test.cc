/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

#include "srslte/common/test_common.h"

int test_nullsink_log()
{
  // Description: Test nullsink_log that only stores the last log message in a local std::string that can be checked
  //              This logger is useful to confirm that a certain action produced an expected error/warning,
  //              without contaminating the console/log file
  srslte::nullsink_log null_logger{"TEST"};

  TESTASSERT(srslte::scoped_tester_log::get_instance() == &null_logger);

  TESTASSERT(null_logger.error_counter == 0);
  TESTASSERT(null_logger.last_log_level == srslte::LOG_LEVEL_NONE);
  TESTASSERT(null_logger.last_log_msg.empty());
  null_logger.error("ERROR MESSAGE"); // This message should not be seen in the console
  TESTASSERT(null_logger.error_counter == 1);
  TESTASSERT(null_logger.last_log_level == srslte::LOG_LEVEL_ERROR);
  TESTASSERT(null_logger.last_log_msg == "ERROR MESSAGE");

  return SRSLTE_SUCCESS;
}

int test_log_scoping()
{
  // Description: Test whether we can use different global TEST loggers in different scopes
  //              on scope exit the previous logger should be recovered
  //              This behavior is useful for the cases we have one generic logger for all tests, but in a specific test
  //              we want to use a different one
  srslte::nullsink_log logger1("TEST1");
  TESTASSERT(srslte::scoped_tester_log::get_instance() == &logger1);

  logger1.error("message1");
  logger1.error("message2");
  TESTASSERT(logger1.last_log_msg == "message2");

  {
    // the global test log should be overwriten here, and used by TESTASSERT macro
    srslte::nullsink_log logger2("TEST2");
    TESTASSERT(srslte::scoped_tester_log::get_instance() == &logger2);
    TESTASSERT(logger2.error_counter == 0);
    logger2.error("error message in logger2\n");
    TESTASSERT(logger2.last_log_msg == "error message in logger2\n");
    TESTASSERT(logger2.error_counter == 1);
  }
  // the last logger should be recovered

  TESTASSERT(srslte::scoped_tester_log::get_instance() == &logger1);
  TESTASSERT(logger1.error_counter == 2);
  return 0;
}

int main()
{
  TESTASSERT(test_nullsink_log() == 0);
  TESTASSERT(test_log_scoping() == 0);
  return 0;
}
