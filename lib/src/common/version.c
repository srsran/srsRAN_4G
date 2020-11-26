/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
