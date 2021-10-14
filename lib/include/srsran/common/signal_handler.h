/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/**
 * @file signal_handler.h
 * @brief Common signal handling methods for all srsRAN applications.
 */

#ifndef SRSRAN_SIGNAL_HANDLER_H
#define SRSRAN_SIGNAL_HANDLER_H

#include "srsran/common/emergency_handlers.h"
#include "srsran/srslog/sink.h"
#include "srsran/srslog/srslog.h"
#include <signal.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define SRSRAN_TERM_TIMEOUT_S (5)

// static vars required by signal handling
static std::atomic<bool> running = {true};

static void srsran_signal_handler(int signal)
{
  switch (signal) {
    case SIGALRM:
      fprintf(stderr, "Couldn't stop after %ds. Forcing exit.\n", SRSRAN_TERM_TIMEOUT_S);
      execute_emergency_cleanup_handlers();
      raise(SIGKILL);
    default:
      // all other registered signals try to stop the app gracefully
      if (running) {
        running = false;
        fprintf(stdout, "Stopping ..\n");
        alarm(SRSRAN_TERM_TIMEOUT_S);
      } else {
        // already waiting for alarm to go off ..
      }
      break;
  }
}

void srsran_register_signal_handler()
{
  signal(SIGINT, srsran_signal_handler);
  signal(SIGTERM, srsran_signal_handler);
  signal(SIGHUP, srsran_signal_handler);
  signal(SIGALRM, srsran_signal_handler);
}

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // SRSRAN_SIGNAL_HANDLER_H
