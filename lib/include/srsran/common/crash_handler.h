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
