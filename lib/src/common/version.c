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

#include "srsran/version.h"

char* srsran_get_version()
{
  return SRSRAN_VERSION_STRING;
}

int srsran_get_version_major()
{
  return SRSRAN_VERSION_MAJOR;
}
int srsran_get_version_minor()
{
  return SRSRAN_VERSION_MINOR;
}
int srsran_get_version_patch()
{
  return SRSRAN_VERSION_PATCH;
}

int srsran_check_version(int major, int minor, int patch)
{
  return (SRSRAN_VERSION >= SRSRAN_VERSION_ENCODE(major, minor, patch));
}
