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
 *  File:         sfo.h
 *
 *  Description:  Sampling frequency offset estimation.
 *
 *  Reference:
 *****************************************************************************/

#ifndef SRSLTE_SFO_H
#define SRSLTE_SFO_H

#include "srslte/config.h"

SRSLTE_API float srslte_sfo_estimate(int* t0, int len, float period);

SRSLTE_API float srslte_sfo_estimate_period(int* t0, int* t, int len, float period);

#endif // SRSLTE_SFO_H
