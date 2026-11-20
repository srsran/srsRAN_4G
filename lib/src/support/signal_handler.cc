// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: AGPL-3.0-or-later

#include "srsran/support/signal_handler.h"
#include "srsran/support/emergency_handlers.h"
#include <atomic>
#include <csignal>
#include <cstdio>
#include <unistd.h>

#ifndef SRSRAN_TERM_TIMEOUT_S
#define SRSRAN_TERM_TIMEOUT_S 5
#endif

/// Handler called after the user interrupts the program.
static std::atomic<srsran_signal_hanlder> user_handler;

/// Set once the forced exit has started.
static volatile sig_atomic_t forcing_exit = 0;

/// Time given to the emergency cleanup handlers before the process is killed anyway.
static const unsigned FORCED_EXIT_CLEANUP_TIMEOUT_S = 1;

/// Async-signal-safe print of a string literal.
template <size_t N>
static void signal_safe_print(int fd, const char (&msg)[N])
{
  ssize_t ret = ::write(fd, msg, N - 1);
  (void)ret;
}

#define SRSRAN_STR_(x) #x
#define SRSRAN_STR(x) SRSRAN_STR_(x)

static void srsran_signal_handler(int signal)
{
  switch (signal) {
    case SIGALRM:
      if (forcing_exit) {
        // The emergency cleanup did not finish in time, e.g. it blocks on a lock held by the interrupted thread
        // (malloc, stdio, ...) as it runs from a signal handler. Kill the process without further delay.
        raise(SIGKILL);
      }
      forcing_exit = 1;
      signal_safe_print(STDERR_FILENO, "Couldn't stop after " SRSRAN_STR(SRSRAN_TERM_TIMEOUT_S) "s. Forcing exit.\n");
      // SIGALRM is installed with SA_NODEFER, so this alarm interrupts a stuck cleanup and re-enters the handler.
      alarm(FORCED_EXIT_CLEANUP_TIMEOUT_S);
      execute_emergency_cleanup_handlers();
      raise(SIGKILL);
      break;
    default:
      // all other registered signals try to stop the app gracefully
      // Call the user handler if present and remove it so that further signals are treated by the default handler.
      if (auto handler = user_handler.exchange(nullptr)) {
        handler();
      } else {
        return;
      }
      signal_safe_print(STDOUT_FILENO, "Stopping ..\n");
      alarm(SRSRAN_TERM_TIMEOUT_S);
      break;
  }
}

void srsran_register_signal_handler(srsran_signal_hanlder handler)
{
  user_handler.store(handler);

  std::signal(SIGINT, srsran_signal_handler);
  std::signal(SIGTERM, srsran_signal_handler);
  std::signal(SIGHUP, srsran_signal_handler);

  // Allow SIGALRM to interrupt its own handler, see the forced exit above.
  struct sigaction alrm_action = {};
  alrm_action.sa_handler       = srsran_signal_handler;
  alrm_action.sa_flags         = SA_RESTART | SA_NODEFER;
  sigemptyset(&alrm_action.sa_mask);
  sigaction(SIGALRM, &alrm_action, nullptr);
}
