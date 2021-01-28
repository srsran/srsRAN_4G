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

/**
 * @file signal_handler.h
 * @brief Common signal handling methods for all srsLTE applications.
 */

#ifndef SRSLTE_SIGNAL_HANDLER_H
#define SRSLTE_SIGNAL_HANDLER_H

#include "srslte/srslog/sink.h"
#include "srslte/srslog/srslog.h"
#include <signal.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define SRSLTE_TERM_TIMEOUT_S (5)

// static vars required by signal handling
static srslog::sink* log_sink = nullptr;
static bool          running  = true;

static void srslte_signal_handler(int signal)
{
  switch (signal) {
    case SIGALRM:
      fprintf(stderr, "Couldn't stop after %ds. Forcing exit.\n", SRSLTE_TERM_TIMEOUT_S);
      srslog::flush();
      //:TODO: refactor the sighandler, should not depend on log utilities
      if (log_sink) {
        log_sink->flush();
      }
      raise(SIGKILL);
    default:
      // all other registered signals try to stop the app gracefully
      if (running) {
        running = false;
        fprintf(stdout, "Stopping ..\n");
        alarm(SRSLTE_TERM_TIMEOUT_S);
      } else {
        // already waiting for alarm to go off ..
      }
      break;
  }
}

void srslte_register_signal_handler()
{
  signal(SIGINT, srslte_signal_handler);
  signal(SIGTERM, srslte_signal_handler);
  signal(SIGHUP, srslte_signal_handler);
  signal(SIGALRM, srslte_signal_handler);
}

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // SRSLTE_SIGNAL_HANDLER_H
