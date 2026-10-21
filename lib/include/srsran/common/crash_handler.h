// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

/**
 * @file crash_handler.h
 * @brief Common handler to catch segfaults and write backtrace to file.
 */

#ifndef SRSRAN_CRASH_HANDLER_H
#define SRSRAN_CRASH_HANDLER_H

#include "srsran/config.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void srsran_debug_handle_crash(int argc, char** argv);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // SRSRAN_CRASH_HANDLER_H
