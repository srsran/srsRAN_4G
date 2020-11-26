/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srslte/common/test_common.h"

using srslte::nullsink_log;
using srslte::scoped_log;

int test_nullsink_log()
{
  // Description: Test nullsink_log that only stores the last log message in a local std::string that can be checked
  //              This logger is useful to confirm that a certain action produced an expected error/warning,
  //              without contaminating the console/log file, and to check what error message was stored
  scoped_log<nullsink_log> null_log("TEST");

  TESTASSERT(srslte::logmap::get("TEST").get() == null_log.get());
  TESTASSERT(null_log->error_counter == 0);
  TESTASSERT(null_log->last_log_level == srslte::LOG_LEVEL_NONE);
  TESTASSERT(null_log->last_log_msg.empty());
  null_log->error("ERROR MESSAGE"); // This message should not be seen in the console
  TESTASSERT(null_log->error_counter == 1);
  TESTASSERT(null_log->last_log_level == srslte::LOG_LEVEL_ERROR);
  TESTASSERT(null_log->last_log_msg == "ERROR MESSAGE");

  return SRSLTE_SUCCESS;
}

int test_log_scoping()
{
  // Description: Test whether we can use different global TEST loggers in different scopes
  //              on scope exit the previous logger should be recovered
  //              This behavior is useful for the cases we have one generic logger for all tests, but in a specific test
  //              we want to use a different one
  scoped_log<nullsink_log> log1("TEST");
  TESTASSERT(srslte::logmap::get("TEST").get() == log1.get());

  log1->error("message1");
  log1->error("message2");
  TESTASSERT(log1->last_log_msg == "message2");
  TESTASSERT(log1->error_counter == 2);

  {
    // the global test log should be overwriten here, and used by TESTASSERT macro
    scoped_log<nullsink_log> log2("TEST");
    TESTASSERT(srslte::logmap::get("TEST").get() == log2.get());
    TESTASSERT(log2->error_counter == 0);
    log2->error("error message in logger2\n");
    TESTASSERT(log2->last_log_msg == "error message in logger2\n");
    TESTASSERT(log2->error_counter == 1);
  }
  // the last logger should be recovered

  TESTASSERT(srslte::logmap::get("TEST").get() == log1.get());
  TESTASSERT(log1->error_counter == 2);
  return 0;
}

int main()
{
  TESTASSERT(test_nullsink_log() == 0);
  TESTASSERT(test_log_scoping() == 0);
  return 0;
}
