// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

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
