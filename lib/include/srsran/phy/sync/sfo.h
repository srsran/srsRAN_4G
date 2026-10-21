// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

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
