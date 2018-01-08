/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
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
#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>

#include "srslte/phy/utils/debug.h"
#include "srslte/version.h"

int srslte_verbose = 0;
int handler_registered = 0;

void get_time_interval(struct timeval * tdata) {

  tdata[0].tv_sec = tdata[2].tv_sec - tdata[1].tv_sec;
  tdata[0].tv_usec = tdata[2].tv_usec - tdata[1].tv_usec;
  if (tdata[0].tv_usec < 0) {
    tdata[0].tv_sec--;
    tdata[0].tv_usec += 1000000;
  }
}

const static char crash_file_name[] = "./srsLTE.backtrace.crash";
static int bt_argc;
static char **bt_argv;

static void crash_handler(int sig) {
  void *array[128];
  int size;

  /* Get all stack traces */
  size = backtrace(array, 128);

  FILE *f = fopen(crash_file_name, "a");
  if (!f) {
    printf("srsLTE crashed... we could not save backtrace in '%s'...\n", crash_file_name);
  } else {
    char **symbols = backtrace_symbols(array, size);

    time_t lnTime;
    struct tm *stTime;
    char strdate[32];

    time(&lnTime);
    stTime = localtime(&lnTime);

    strftime(strdate, 32, "%d/%m/%Y %H:%M:%S", stTime);

    fprintf(f, "--- command='");
    for (int i = 0; i < bt_argc; i++) {
      fprintf(f, "%s%s", (i == 0) ? "" : " ", bt_argv[i]);
    }
    fprintf(f, "' version=%s signal=%d date='%s' ---\n", SRSLTE_VERSION_STRING, sig, strdate);

    for (int i = 0; i < size; i++) {
      fprintf(f, "\t%s\n", symbols[i]);
    }
    fprintf(f, "\n");

    printf("srsLTE crashed... backtrace saved in '%s'...\n", crash_file_name);
    fclose(f);
  }
  printf("---  exiting  ---\n");
  exit(1);
}

void srslte_debug_handle_crash(int argc, char **argv) {
  bt_argc = argc;
  bt_argv = argv;

  signal(SIGSEGV, crash_handler);
  signal(SIGABRT, crash_handler);
  signal(SIGILL, crash_handler);
  signal(SIGFPE, crash_handler);
}
