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

/**********************************************************************************************
 *  File:         timestamp.h
 *
 *  Description:  A simple timestamp struct with separate variables for full and frac seconds.
 *                Separate variables are used to avoid loss of precision in our frac seconds.
 *                Only positive timestamps are supported.
 *
 *  Reference:
 *********************************************************************************************/

#ifndef SRSLTE_TIMESTAMP_H
#define SRSLTE_TIMESTAMP_H

#include "srslte/config.h"
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

typedef struct SRSLTE_API {
  time_t full_secs;
  double frac_secs;
} srslte_timestamp_t;

#ifdef __cplusplus
extern "C" {
#endif

SRSLTE_API int srslte_timestamp_init(srslte_timestamp_t* t, time_t full_secs, double frac_secs);

SRSLTE_API void srslte_timestamp_init_uint64(srslte_timestamp_t* ts_time, uint64_t ts_count, double base_srate);

SRSLTE_API int srslte_timestamp_copy(srslte_timestamp_t* dest, srslte_timestamp_t* src);

SRSLTE_API int srslte_timestamp_compare(const srslte_timestamp_t* a, const srslte_timestamp_t* b);

SRSLTE_API int srslte_timestamp_add(srslte_timestamp_t* t, time_t full_secs, double frac_secs);

SRSLTE_API int srslte_timestamp_sub(srslte_timestamp_t* t, time_t full_secs, double frac_secs);

SRSLTE_API double srslte_timestamp_real(const srslte_timestamp_t* t);

SRSLTE_API bool srslte_timestamp_iszero(const srslte_timestamp_t* t);

SRSLTE_API uint32_t srslte_timestamp_uint32(srslte_timestamp_t* t);

SRSLTE_API uint64_t srslte_timestamp_uint64(const srslte_timestamp_t* t, double srate);

#ifdef __cplusplus
}
#endif

#endif // SRSLTE_TIMESTAMP_H
