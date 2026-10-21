// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom
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
