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

#include "srslte/version.h"

char* srslte_get_version()
{
  return SRSLTE_VERSION_STRING;
}

int srslte_get_version_major()
{
  return SRSLTE_VERSION_MAJOR;
}
int srslte_get_version_minor()
{
  return SRSLTE_VERSION_MINOR;
}
int srslte_get_version_patch()
{
  return SRSLTE_VERSION_PATCH;
}

int srslte_check_version(int major, int minor, int patch)
{
  return (SRSLTE_VERSION >= SRSLTE_VERSION_ENCODE(major, minor, patch));
}
