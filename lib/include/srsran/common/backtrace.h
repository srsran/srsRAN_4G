// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

/******************************************************************************
 *  File:         backtrace.h
 *
 *  Description:  print backtrace in runtime.
 *
 *  Reference:
 *****************************************************************************/

#ifndef SRSRAN_BACKTRACE_H
#define SRSRAN_BACKTRACE_H

#include "srsran/config.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

SRSRAN_API void srsran_backtrace_print(FILE* f);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // SRSRAN_BACKTRACE_H
