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

/**********************************************************************************************
 *  File:         convcoder.h
 *
 *  Description:  Convolutional encoder.
 *                LTE uses a tail biting convolutional code with constraint length 7
 *                and coding rate 1/3.
 *
 *  Reference:    3GPP TS 36.212 version 10.0.0 Release 10 Sec. 5.1.3.1
 *********************************************************************************************/

#ifndef SRSLTE_CONVCODER_H
#define SRSLTE_CONVCODER_H

#include "srslte/config.h"
#include <stdbool.h>

typedef struct SRSLTE_API {
  uint32_t R;
  uint32_t K;
  int      poly[3];
  bool     tail_biting;
} srslte_convcoder_t;

SRSLTE_API int srslte_convcoder_encode(srslte_convcoder_t* q, uint8_t* input, uint8_t* output, uint32_t frame_length);

#endif // SRSLTE_CONVCODER_H
