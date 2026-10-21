// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

/******************************************************************************
 *  File:         resample_arb.h
 *
 *  Description:  Arbitrary rate resampler using a polyphase filter bank
 *                implementation.
 *
 *  Reference:    Multirate Signal Processing for Communication Systems
 *                fredric j. harris
 *****************************************************************************/

#ifndef SRSRAN_RESAMPLE_ARB_H
#define SRSRAN_RESAMPLE_ARB_H

#include <complex.h>
#include <stdbool.h>
#include <stdint.h>

#include "srsran/config.h"

#define SRSRAN_RESAMPLE_ARB_N_35 35
#define SRSRAN_RESAMPLE_ARB_N 32 // Polyphase filter rows
#define SRSRAN_RESAMPLE_ARB_M 8  // Polyphase filter columns

typedef struct SRSRAN_API {
  float rate; // Resample rate
  float step; // Step increment through filter
  float acc;  // Index into filter
  bool  interpolate;
  cf_t  reg[SRSRAN_RESAMPLE_ARB_M]; // Our window of samples

} srsran_resample_arb_t;

SRSRAN_API void srsran_resample_arb_init(srsran_resample_arb_t* q, float rate, bool interpolate);

SRSRAN_API int srsran_resample_arb_compute(srsran_resample_arb_t* q, cf_t* input, cf_t* output, int n_in);

#endif // SRSRAN_RESAMPLE_ARB_
