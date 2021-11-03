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

#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "srsran/common/crash_handler.h"

#if HAVE_BACKWARD
#include "srsran/common/backward.hpp"
using namespace backward;
void srsran_debug_handle_crash(int argc, char** argv)
{
  backward::SignalHandling sh;
}
#else // HAVE_BACKWARD
#include "srsran/common/backtrace.h"
#include "srsran/version.h"

static int    bt_argc;
static char** bt_argv;

static void crash_handler(int sig)
{
  FILE*     f = stderr;
  time_t    lnTime;
  struct tm stTime;
  char      strdate[32];

  time(&lnTime);
  gmtime_r(&lnTime, &stTime);

  strftime(strdate, sizeof(strdate), "%d/%m/%Y %H:%M:%S", &stTime);

  fprintf(f, "--- command='");
  for (int i = 0; i < bt_argc; i++) {
    fprintf(f, "%s%s", (i == 0) ? "" : " ", bt_argv[i]);
  }
  fprintf(f, "' version=%s signal=%d date='%s' ---\n", SRSRAN_VERSION_STRING, sig, strdate);

  srsran_backtrace_print(f);

  fprintf(f, "srsRAN crashed. Please send this backtrace to the developers ...\n");

  fprintf(f, "---  exiting  ---\n");
  exit(1);
}

void srsran_debug_handle_crash(int argc, char** argv)
{
  bt_argc = argc;
  bt_argv = argv;

  signal(SIGSEGV, crash_handler);
  signal(SIGABRT, crash_handler);
  signal(SIGILL, crash_handler);
  signal(SIGFPE, crash_handler);
  signal(SIGPIPE, crash_handler);
}

#endif // HAVE_BACKWARD