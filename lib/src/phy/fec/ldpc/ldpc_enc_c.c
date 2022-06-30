/**
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

/*!
 * \file ldpc_enc_c.c
 * \brief Definition of the LDPC encoder inner functions (not optimized).
 * \author David Gregoratti
 * \date 2020
 *
 * \copyright Software Radio Systems Limited
 *
 */

#include <stdint.h>

#include "srsran/phy/fec/ldpc/base_graph.h"
#include "srsran/phy/fec/ldpc/ldpc_encoder.h"
#include "srsran/phy/utils/debug.h"

void encode_ext_region(srsran_ldpc_encoder_t* q, uint8_t* output, uint8_t n_layers)
{

  uint8_t(*aux)[q->ls] = q->ptr;

  int m    = 0;
  int skip = 0;
  int i    = 0;
  int k    = 0;

  uint16_t* this_shift = NULL;

  uint8_t tmp_out = 0;

  // Encode the extended region. In case of puncturing or IR-HARQ, we could focus on
  // specific check nodes instead of processing all of them from m = 4 to m = M - 1.
  for (m = 4; m < n_layers; m++) {
    skip = (q->bgK + m - 2) * q->ls;
    for (i = 0; i < q->ls; i++) {
      // the systematic part has already been computed
      output[skip + i] = aux[m][i];
      // sum the contribution due to the high-rate region, with the proper circular shifts
      for (k = 0; k < 4; k++) {
        this_shift = q->pcm + q->bgK + k + m * q->bgN;
        if (*this_shift != NO_CNCT) {
          tmp_out = *(output + (q->bgK - 2 + k) * q->ls + ((i + *this_shift) % q->ls));
          output[skip + i] ^= tmp_out;
        }
      }
    }
  }
}

void preprocess_systematic_bits(srsran_ldpc_encoder_t* q, const uint8_t* input)
{
  uint8_t(*aux)[q->ls] = q->ptr;

  int       N   = q->bgN;
  int       K   = q->bgK;
  int       M   = q->bgM;
  int       ls  = q->ls;
  uint16_t* pcm = q->pcm;

  int            i             = 0;
  int            k             = 0;
  int            m             = 0;
  uint16_t*      this_shift    = NULL;
  const uint8_t* this_in_chunk = NULL;

  bzero(aux, M * ls * sizeof(uint8_t));

  // split the input message into K chunks of ls bits each and, for all chunks
  for (k = 0; k < K; k++) {
    this_in_chunk = input + k * ls;
    // for all check nodes
    // NB: if looking for performance you can do the following loop only over the high-rate
    // region of the PCM (m=0,1,2,3) and over the check nodes that result in a transmitted
    // coded bit after puncturing or IR-HARQ (see Deliverable D1 Section 3.4).
    for (m = 0; m < M; m++) {
      // entry of pcm corresponding to the current input chunk and the current check node
      this_shift = pcm + k + m * N;

      // xor array aux[m] with a circularly shifted version of the current input chunk, unless
      // the current check node and variable node are not connected.
      for (i = 0; i < ls; i++) {
        // mask with 1 to remove the filler bit flag
        aux[m][i] ^= *this_shift != NO_CNCT ? 1U & (*(this_in_chunk + ((i + *this_shift) % ls))) : 0;
      }
    }
  }
}

void encode_high_rate_case1(void* q_, uint8_t* output)
{
  srsran_ldpc_encoder_t* q = (srsran_ldpc_encoder_t*)q_;
  uint8_t(*aux)[q->ls]     = q->ptr;

  int ls = q->ls;
  int k  = 0;

  int skip0 = (q->bgK - 2) * ls;
  int skip1 = (q->bgK - 1) * ls;
  int skip2 = q->bgK * ls;
  int skip3 = (q->bgK + 1) * ls;
  for (k = 0; k < ls; k++) {
    // first chunk of parity bits
    output[skip0 + k] = aux[0][k] ^ aux[1][k];
    output[skip0 + k] ^= aux[2][k];
    output[skip0 + k] ^= aux[3][k];
  }
  for (k = 0; k < ls; k++) {
    // second chunk of parity bits
    output[skip1 + k] = aux[0][k] ^ output[skip0 + ((k + 1) % ls)];
    // fourth chunk of parity bits
    output[skip3 + k] = aux[3][k] ^ output[skip0 + ((k + 1) % ls)];
    // third chunk of parity bits
    output[skip2 + k] = aux[2][k] ^ output[skip3 + k];
  }
}

void encode_high_rate_case2(void* q_, uint8_t* output)
{
  srsran_ldpc_encoder_t* q = (srsran_ldpc_encoder_t*)q_;
  uint8_t(*aux)[q->ls]     = q->ptr;

  int ls = q->ls;
  int i  = 0;
  int k  = 0;

  int skip0 = (q->bgK - 2) * ls;
  int skip1 = (q->bgK - 1) * ls;
  int skip2 = q->bgK * ls;
  int skip3 = (q->bgK + 1) * ls;
  for (k = 0; k < ls; k++) {
    i = (k - 105) % ls;
    i = i >= 0 ? i : i + ls;

    // first chunk of parity bits
    output[skip0 + k] = aux[0][i] ^ aux[1][i];
    output[skip0 + k] ^= aux[2][i];
    output[skip0 + k] ^= aux[3][i];
    // second chunk of parity bits
    output[skip1 + k] = aux[0][k] ^ output[skip0 + k];
    // fourth chunk of parity bits
    output[skip3 + k] = aux[3][k] ^ output[skip0 + k];
    // third chunk of parity bits
    output[skip2 + k] = aux[2][k] ^ output[skip3 + k];
  }
}

void encode_high_rate_case3(void* q_, uint8_t* output)
{
  srsran_ldpc_encoder_t* q = (srsran_ldpc_encoder_t*)q_;
  uint8_t(*aux)[q->ls]     = q->ptr;

  int ls = q->ls;
  int i  = 0;
  int k  = 0;

  int skip0 = (q->bgK - 2) * ls;
  int skip1 = (q->bgK - 1) * ls;
  int skip2 = q->bgK * ls;
  int skip3 = (q->bgK + 1) * ls;
  for (k = 0; k < ls; k++) {
    i = (k - 1) % ls;
    i = i >= 0 ? i : i + ls;

    // first chunk of parity bits
    output[skip0 + k] = aux[0][i] ^ aux[1][i];
    output[skip0 + k] ^= aux[2][i];
    output[skip0 + k] ^= aux[3][i];
    // second chunk of parity bits
    output[skip1 + k] = aux[0][k] ^ output[skip0 + k];
    // third chunk of parity bits
    output[skip2 + k] = aux[1][k] ^ output[skip1 + k];
    // fourth chunk of parity bits
    output[skip3 + k] = aux[3][k] ^ output[skip0 + k];
  }
}

void encode_high_rate_case4(void* q_, uint8_t* output)
{
  srsran_ldpc_encoder_t* q = (srsran_ldpc_encoder_t*)q_;
  uint8_t(*aux)[q->ls]     = q->ptr;

  int ls = q->ls;
  int k  = 0;

  int skip0 = (q->bgK - 2) * ls;
  int skip1 = (q->bgK - 1) * ls;
  int skip2 = q->bgK * ls;
  int skip3 = (q->bgK + 1) * ls;
  for (k = 0; k < ls; k++) {
    // first chunk of parity bits
    output[skip0 + k] = aux[0][k] ^ aux[1][k];
    output[skip0 + k] ^= aux[2][k];
    output[skip0 + k] ^= aux[3][k];
  }
  for (k = 0; k < ls; k++) {
    // second chunk of parity bits
    output[skip1 + k] = aux[0][k] ^ output[skip0 + ((k + 1) % ls)];
    // third chunk of parity bits
    output[skip2 + k] = aux[1][k] ^ output[skip1 + k];
    // fourth chunk of parity bits
    output[skip3 + k] = aux[3][k] ^ output[skip0 + ((k + 1) % ls)];
  }
}
