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

/**
 * @file signal_handler.h
 * @brief Common signal handling methods for all srsLTE applications.
 */

#ifndef SRSLTE_SIGNAL_HANDLER_H
#define SRSLTE_SIGNAL_HANDLER_H

#include "srslte/srslog/sink.h"
#include <signal.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define SRSLTE_TERM_TIMEOUT_S (5)

// static vars required by signal handling
static srslog::sink*       log_sink = nullptr;
static bool                running = true;

static void srslte_signal_handler(int signal)
{
  switch (signal) {
    case SIGALRM:
      fprintf(stderr, "Couldn't stop after %ds. Forcing exit.\n", SRSLTE_TERM_TIMEOUT_S);
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
