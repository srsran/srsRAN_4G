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

/*!
 * \file polar_chanalloc.c
 * \brief Definition of the subchannel allocation block.
 * \author Jesus Gomez
 * \date 2020
 *
 * \copyright Software Radio Systems Limited
 *
 */

#include "srsran/phy/fec/polar/polar_chanalloc.h"
#include <string.h>

void srsran_polar_chanalloc_tx(const uint8_t*  message,
                               uint8_t*        input_encoder,
                               const uint16_t  N,
                               const uint16_t  K,
                               const uint8_t   nPC,
                               const uint16_t* K_set,
                               const uint16_t* PC_set)
{
  bzero(input_encoder, N * sizeof(uint8_t));

  uint16_t i_o = 0;
  if (nPC == 0) {
    for (uint16_t i = 0; i < K; i++) {
      i_o                = K_set[i];
      input_encoder[i_o] = message[i];
    }
  } else {
    uint16_t tmpy0 = 0;
    uint16_t y0    = 0;
    uint16_t y1    = 0;
    uint16_t y2    = 0;
    uint16_t y3    = 0;
    uint16_t y4    = 0;
    uint16_t iKPC  = 0;
    uint16_t iPC   = 0;
    uint16_t iK    = 0;
    for (i_o = 0; i_o < N; i_o++) {
      // circ. shift register
      tmpy0 = y0;
      y0    = y1;
      y1    = y2;
      y2    = y3;
      y3    = y4;
      y4    = tmpy0;
      if (i_o == K_set[iKPC]) { // information bit
        iKPC = iKPC + 1;
        if (i_o == PC_set[iPC]) { // parity bit
          iPC++;
          input_encoder[i_o] = y0;
        } else {
          input_encoder[i_o] = message[iK];
          y0                 = y0 ^ message[iK];
          iK++;
        }
      }
    }
  }
}

void srsran_polar_chanalloc_rx(const uint8_t*  output_decoder,
                               uint8_t*        message,
                               const uint16_t  K,
                               const uint8_t   nPC,
                               const uint16_t* K_set,
                               const uint16_t* PC_set)
{
  uint16_t i_o = 0;
  uint16_t iPC = 0;
  uint16_t iK  = 0;
  for (uint16_t iKPC = 0; iKPC < (uint16_t)(K + nPC); iKPC++) {
    i_o = K_set[iKPC];        // includes parity bits
    if (i_o == PC_set[iPC]) { // skip
      iPC = iPC + 1;
    } else {
      message[iK] = output_decoder[i_o];
      iK          = iK + 1;
    }
  }
}
