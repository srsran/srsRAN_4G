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
 *  File:         debug.h
 *
 *  Description:  Debug output utilities.
 *
 *  Reference:
 *****************************************************************************/

#ifndef SRSLTE_FILTER_H
#define SRSLTE_FILTER_H

#include "srslte/config.h"
#include "srslte/phy/utils/vector.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
typedef struct SRSLTE_API {
  cf_t*  filter_input;
  cf_t*  downsampled_input;
  cf_t*  filter_output;
  bool   is_decimator;
  int    factor;
  int    num_taps;
  float* taps;

} srslte_filt_cc_t;

void srslte_filt_decim_cc_init(srslte_filt_cc_t* q, int factor, int order);

void srslte_filt_decim_cc_free(srslte_filt_cc_t* q);

void srslte_filt_decim_cc_execute(srslte_filt_cc_t* q, cf_t* input, cf_t* downsampled_input, cf_t* output, int size);

void srslte_downsample_cc(cf_t* input, cf_t* output, int M, int size);
#endif // SRSLTE_FILTER_H