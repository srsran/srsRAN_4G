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
