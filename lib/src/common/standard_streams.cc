// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

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
