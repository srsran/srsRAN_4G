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
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>

void srsran_backtrace_print(FILE* f)
{
  void* array[128] = {};
  int   size       = 0;

  if (f) {
    // Get all stack traces
    size = backtrace(array, 128);

    char** symbols = backtrace_symbols(array, size);

    for (int i = 1; i < size; i++) {
      fprintf(f, "\t%s\n", symbols[i]);
    }
    free(symbols);
  }
}
