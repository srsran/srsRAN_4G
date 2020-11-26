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
 *  File:         decim.h
 *
 *  Description:  Integer linear decimation
 *
 *  Reference:
 *****************************************************************************/

#ifndef SRSLTE_DECIM_H
#define SRSLTE_DECIM_H

#include "srslte/config.h"

SRSLTE_API void srslte_decim_c(cf_t* input, cf_t* output, int M, int len);

SRSLTE_API void srslte_decim_f(float* input, float* output, int M, int len);

#endif // SRSLTE_DECIM_H
