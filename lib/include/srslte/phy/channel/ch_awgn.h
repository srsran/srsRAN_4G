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

/**********************************************************************************************
 *  File:         ch_awgn.h
 *
 *  Description:  Additive white gaussian noise channel object
 *
 *  Reference:
 *********************************************************************************************/

#include <stdint.h>
#include "srslte/config.h"

#ifndef SRSLTE_CH_AWGN_H
#define SRSLTE_CH_AWGN_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * The srsLTE channel AWGN implements an efficient Box-Muller Method accelerated with SIMD.
 */
typedef struct {
  float*   table_cos;
  float*   table_log;
  uint32_t rand_state;
  float    std_dev;
} srslte_channel_awgn_t;

/**
 * Initialization function of the channel AWGN object
 *
 * @param q AWGN channel object
 * @param seed random generator seed
 */
SRSLTE_API int srslte_channel_awgn_init(srslte_channel_awgn_t* q, uint32_t seed);

/**
 * Sets the noise level N0 in decibels full scale (dBfs)
 *
 * @param q AWGN channel object
 * @param n0_dBfs noise level
 */
SRSLTE_API int srslte_channel_awgn_set_n0(srslte_channel_awgn_t* q, float n0_dBfs);

/**
 * Runs the complex AWGN channel
 *
 * @param q AWGN channel object
 * @param in complex input array
 * @param out complex output array
 * @param length number of samples
 */
SRSLTE_API void srslte_channel_awgn_run_c(srslte_channel_awgn_t* q, const cf_t* in, cf_t* out, uint32_t length);

/**
 * Runs the real AWGN channel
 *
 * @param q AWGN channel object
 * @param in real input array
 * @param out real output array
 * @param length number of samples
 */
SRSLTE_API void srslte_channel_awgn_run_f(srslte_channel_awgn_t* q, const float* in, float* out, uint32_t length);

/**
 * Free AWGN channel generator data
 *
 * @param q AWGN channel object
 */
SRSLTE_API void srslte_channel_awgn_free(srslte_channel_awgn_t* q);

SRSLTE_API void srslte_ch_awgn_c(const cf_t* input, cf_t* output, float variance, uint32_t len);

SRSLTE_API void srslte_ch_awgn_f(const float* x, float* y, float variance, uint32_t len);

SRSLTE_API float srslte_ch_awgn_get_variance(float ebno_db, float rate);

#ifdef __cplusplus
}
#endif

#endif // SRSLTE_CH_AWGN_H
