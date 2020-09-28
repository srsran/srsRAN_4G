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

#include "srslte/common/standard_streams.h"
#include <cstdarg>
#include <cstdio>

using namespace srslte;

void srslte::console(const char* str, ...)
{
  std::va_list args;
  va_start(args, str);
  char buffer[1024];
  std::vsnprintf(buffer, sizeof(buffer) - 1, str, args);
  std::fputs(buffer, stdout);
  std::fflush(stdout);
  va_end(args);
}

void srslte::console_stderr(const char* str, ...)
{
  std::va_list args;
  va_start(args, str);
  char buffer[1024];
  std::vsnprintf(buffer, sizeof(buffer) - 1, str, args);
  std::fputs(buffer, stderr);
  std::fflush(stderr);
  va_end(args);
}
