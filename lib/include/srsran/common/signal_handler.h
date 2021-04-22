/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

/**
 * @file signal_handler.h
 * @brief Common signal handling methods for all srsRAN applications.
 */

#ifndef SRSRAN_SIGNAL_HANDLER_H
#define SRSRAN_SIGNAL_HANDLER_H

#include "srsran/srslog/sink.h"
#include "srsran/srslog/srslog.h"
#include <signal.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define SRSRAN_TERM_TIMEOUT_S (5)

// static vars required by signal handling
static srslog::sink* log_sink = nullptr;
static bool          running  = true;

static void srsran_signal_handler(int signal)
{
  switch (signal) {
    case SIGALRM:
      fprintf(stderr, "Couldn't stop after %ds. Forcing exit.\n", SRSRAN_TERM_TIMEOUT_S);
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
