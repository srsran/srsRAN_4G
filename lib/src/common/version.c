// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

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
