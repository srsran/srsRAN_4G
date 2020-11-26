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

/******************************************************************************
 *  File:         backtrace.h
 *
 *  Description:  print backtrace in runtime.
 *
 *  Reference:
 *****************************************************************************/

#ifndef SRSLTE_BACKTRACE_H
#define SRSLTE_BACKTRACE_H

#include "srslte/config.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

SRSLTE_API void srslte_backtrace_print(FILE* f);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // SRSLTE_BACKTRACE_H
