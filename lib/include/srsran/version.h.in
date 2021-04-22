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

#ifndef SRSRAN_VERSION_H_IN
#define SRSRAN_VERSION_H_IN

// the configured options and settings for SRSRAN
#define SRSRAN_VERSION_MAJOR @SRSRAN_VERSION_MAJOR@
#define SRSRAN_VERSION_MINOR @SRSRAN_VERSION_MINOR@
#define SRSRAN_VERSION_PATCH @SRSRAN_VERSION_PATCH@
#define SRSRAN_VERSION_STRING "@SRSRAN_VERSION_STRING@"

#define SRSRAN_VERSION_ENCODE(major, minor, patch) ( \
    ((major) * 10000)                                \
  + ((minor) *   100)                                \
  + ((patch) *     1))

#define SRSRAN_VERSION SRSRAN_VERSION_ENCODE( \
  SRSRAN_VERSION_MAJOR,                       \
  SRSRAN_VERSION_MINOR,                       \
  SRSRAN_VERSION_PATCH)

#define SRSRAN_VERSION_CHECK(major,minor,patch)    \
  (SRSRAN_VERSION >= SRSRAN_VERSION_ENCODE(major,minor,patch))

#include "srsran/config.h"

SRSRAN_API char* srsran_get_version();
SRSRAN_API int   srsran_get_version_major();
SRSRAN_API int   srsran_get_version_minor();
SRSRAN_API int   srsran_get_version_patch();
SRSRAN_API int   srsran_check_version(int major, int minor, int patch);

#endif // VERSION_
