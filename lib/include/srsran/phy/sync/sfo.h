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
 *  File:         sfo.h
 *
 *  Description:  Sampling frequency offset estimation.
 *
 *  Reference:
 *****************************************************************************/

#ifndef SRSRAN_SFO_H
#define SRSRAN_SFO_H

#include "srsran/config.h"

SRSRAN_API float srsran_sfo_estimate(int* t0, int len, float period);

SRSRAN_API float srsran_sfo_estimate_period(int* t0, int* t, int len, float period);

#endif // SRSRAN_SFO_H
