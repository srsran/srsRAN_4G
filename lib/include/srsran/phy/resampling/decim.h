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
 *  File:         decim.h
 *
 *  Description:  Integer linear decimation
 *
 *  Reference:
 *****************************************************************************/

#ifndef SRSRAN_DECIM_H
#define SRSRAN_DECIM_H

#include "srsran/config.h"

SRSRAN_API void srsran_decim_c(cf_t* input, cf_t* output, int M, int len);

SRSRAN_API void srsran_decim_f(float* input, float* output, int M, int len);

#endif // SRSRAN_DECIM_H
