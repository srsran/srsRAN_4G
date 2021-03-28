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

#include "srsran/common/standard_streams.h"
#include <cstdarg>
#include <cstdio>

using namespace srsran;

void srsran::console(const char* str, ...)
{
  std::va_list args;
  va_start(args, str);
  char buffer[1024];
  std::vsnprintf(buffer, sizeof(buffer) - 1, str, args);
  std::fputs(buffer, stdout);
  std::fflush(stdout);
  va_end(args);
}

void srsran::console_stderr(const char* str, ...)
{
  std::va_list args;
  va_start(args, str);
  char buffer[1024];
  std::vsnprintf(buffer, sizeof(buffer) - 1, str, args);
  std::fputs(buffer, stderr);
  std::fflush(stderr);
  va_end(args);
}
