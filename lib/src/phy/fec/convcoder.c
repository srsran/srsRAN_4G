/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "parity.h"
#include "srslte/phy/fec/convcoder.h"

/**
 * Convolution encodes according to given parameters.
 *
 * q->R is rate
 * q->tail_biting enables tail biting
 * q->K is a parameter for tail biting
 *
 * @param[in] q Convolution coder parameters
 * @param[in] input Unpacked bit array. Size frame_length
 * @param[out] output Unpacked bit array. Size q->R*frame_length if q->tail_biting, else q->R*(frame_length + q->K - 1)
 * @param[in] frame_length Number of bits in input_array
 * @return Number of bits in output
 */
int srslte_convcoder_encode(srslte_convcoder_t* q, uint8_t* input, uint8_t* output, uint32_t frame_length)
{
  uint32_t sr;
  uint32_t i, j;

  if (q != NULL && input != NULL && output != NULL && frame_length > q->K + 1) {
    uint32_t len = q->tail_biting ? frame_length : (frame_length + q->K - 1);
    if (q->tail_biting) {
      sr = 0;
      for (i = frame_length - q->K + 1; i < frame_length; i++) {
        sr = (sr << 1) | (input[i] & 1);
      }
    } else {
      sr = 0;
    }
    for (i = 0; i < len; i++) {
      uint8_t bit = (i < frame_length) ? (input[i] & 1) : 0;
      sr          = (sr << 1) | bit;
      for (j = 0; j < q->R; j++) {
        output[q->R * i + j] = parity(sr & q->poly[j]);
      }
    }
    return q->R * len;
  } else {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }
}
