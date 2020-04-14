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

#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "srslte/common/backtrace.h"
#include "srslte/common/crash_handler.h"
#include "srslte/version.h"

const static char crash_file_name[] = "./srsLTE.backtrace.crash";
static int        bt_argc;
static char**     bt_argv;

static void crash_handler(int sig)
{
  FILE* f = fopen(crash_file_name, "a");
  if (!f) {
    printf("srsLTE crashed... we could not save backtrace in '%s'...\n", crash_file_name);
  } else {
    time_t     lnTime;
    struct tm  stTime;
    char       strdate[32];

    time(&lnTime);
    gmtime_r(&lnTime, &stTime);

    strftime(strdate, sizeof(strdate), "%d/%m/%Y %H:%M:%S", &stTime);

    fprintf(f, "--- command='");
    for (int i = 0; i < bt_argc; i++) {
      fprintf(f, "%s%s", (i == 0) ? "" : " ", bt_argv[i]);
    }
    fprintf(f, "' version=%s signal=%d date='%s' ---\n", SRSLTE_VERSION_STRING, sig, strdate);

    srslte_backtrace_print(f);
    fprintf(f, "\n");

    printf("srsLTE crashed... backtrace saved in '%s'...\n", crash_file_name);
    fclose(f);
  }
  printf("---  exiting  ---\n");
  exit(1);
}

void srslte_debug_handle_crash(int argc, char** argv)
{
  bt_argc = argc;
  bt_argv = argv;

  signal(SIGSEGV, crash_handler);
  signal(SIGABRT, crash_handler);
  signal(SIGILL, crash_handler);
  signal(SIGFPE, crash_handler);
  signal(SIGPIPE, crash_handler);
}
