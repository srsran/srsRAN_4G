// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

/**********************************************************************************************
 *  File:         convcoder.h
 *
 *  Description:  Convolutional encoder.
 *                LTE uses a tail biting convolutional code with constraint length 7
 *                and coding rate 1/3.
 *
 *  Reference:    3GPP TS 36.212 version 10.0.0 Release 10 Sec. 5.1.3.1
 *********************************************************************************************/

#ifndef SRSRAN_CONVCODER_H
#define SRSRAN_CONVCODER_H

#include "srsran/config.h"
#include <stdbool.h>

typedef struct SRSRAN_API {
  uint32_t R;
  uint32_t K;
  int      poly[3];
  bool     tail_biting;
} srsran_convcoder_t;

SRSRAN_API int srsran_convcoder_encode(srsran_convcoder_t* q, uint8_t* input, uint8_t* output, uint32_t frame_length);

#endif // SRSRAN_CONVCODER_H
