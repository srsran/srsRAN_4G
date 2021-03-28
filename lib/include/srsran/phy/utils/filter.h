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
 *  File:         debug.h
 *
 *  Description:  Debug output utilities.
 *
 *  Reference:
 *****************************************************************************/

#ifndef SRSRAN_FILTER_H
#define SRSRAN_FILTER_H

#include "srsran/config.h"
#include "srsran/phy/utils/vector.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
typedef struct SRSRAN_API {
  cf_t*  filter_input;
  cf_t*  downsampled_input;
  cf_t*  filter_output;
  bool   is_decimator;
  int    factor;
  int    num_taps;
  float* taps;

} srsran_filt_cc_t;

void srsran_filt_decim_cc_init(srsran_filt_cc_t* q, int factor, int order);

void srsran_filt_decim_cc_free(srsran_filt_cc_t* q);

void srsran_filt_decim_cc_execute(srsran_filt_cc_t* q, cf_t* input, cf_t* downsampled_input, cf_t* output, int size);

void srsran_downsample_cc(cf_t* input, cf_t* output, int M, int size);
#endif // SRSRAN_FILTER_H