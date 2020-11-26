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
 *  File:         resample_arb.h
 *
 *  Description:  Arbitrary rate resampler using a polyphase filter bank
 *                implementation.
 *
 *  Reference:    Multirate Signal Processing for Communication Systems
 *                fredric j. harris
 *****************************************************************************/

#ifndef SRSLTE_RESAMPLE_ARB_H
#define SRSLTE_RESAMPLE_ARB_H

#include <complex.h>
#include <stdbool.h>
#include <stdint.h>

#include "srslte/config.h"

#define SRSLTE_RESAMPLE_ARB_N_35 35
#define SRSLTE_RESAMPLE_ARB_N 32 // Polyphase filter rows
#define SRSLTE_RESAMPLE_ARB_M 8  // Polyphase filter columns

typedef struct SRSLTE_API {
  float rate; // Resample rate
  float step; // Step increment through filter
  float acc;  // Index into filter
  bool  interpolate;
  cf_t  reg[SRSLTE_RESAMPLE_ARB_M]; // Our window of samples

} srslte_resample_arb_t;

SRSLTE_API void srslte_resample_arb_init(srslte_resample_arb_t* q, float rate, bool interpolate);

SRSLTE_API int srslte_resample_arb_compute(srslte_resample_arb_t* q, cf_t* input, cf_t* output, int n_in);

#endif // SRSLTE_RESAMPLE_ARB_
