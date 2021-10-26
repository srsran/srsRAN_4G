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

#include "srsran/support/signal_handler.h"
#include "srsran/support/emergency_handlers.h"
#include <atomic>
#include <csignal>
#include <cstdio>
#include <unistd.h>

#ifndef SRSRAN_TERM_TIMEOUT_S
#define SRSRAN_TERM_TIMEOUT_S (5)
#endif

/// Handler called after the user interrupts the program.
static std::atomic<srsran_signal_hanlder> user_handler;

static void srsran_signal_handler(int signal)
{
  switch (signal) {
    case SIGALRM:
      fprintf(stderr, "Couldn't stop after %ds. Forcing exit.\n", SRSRAN_TERM_TIMEOUT_S);
      execute_emergency_cleanup_handlers();
      raise(SIGKILL);
    default:
      // all other registered signals try to stop the app gracefully
      // Call the user handler if present and remove it so that further signals are treated by the default handler.
      if (auto handler = user_handler.exchange(nullptr)) {
        handler();
      } else {
        return;
      }
      fprintf(stdout, "Stopping ..\n");
      alarm(SRSRAN_TERM_TIMEOUT_S);
      break;
  }
}

void srsran_register_signal_handler(srsran_signal_hanlder handler)
{
  user_handler.store(handler);

  signal(SIGINT, srsran_signal_handler);
  signal(SIGTERM, srsran_signal_handler);
  signal(SIGHUP, srsran_signal_handler);
  signal(SIGALRM, srsran_signal_handler);
}
