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
