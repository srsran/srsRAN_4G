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

#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/fec/cbsegm.h"
#include "srslte/phy/fec/convcoder.h"
#include "srslte/phy/fec/crc.h"
#include "srslte/phy/fec/rm_conv.h"
#include "srslte/phy/phch/uci.h"
#include "srslte/phy/utils/bit.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"

/* Table 5.2.2.6.4-1: Basis sequence for (32, O) code */
static uint8_t M_basis_seq[SRSLTE_UCI_M_BASIS_SEQ_LEN][SRSLTE_UCI_MAX_ACK_SR_BITS] = {
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1}, {1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1}, {1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 1},
    {1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1}, {1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1}, {1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 1},
    {1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1}, {1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1}, {1, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1},
    {1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 1}, {1, 0, 1, 0, 0, 1, 1, 1, 0, 1, 1}, {1, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1},
    {1, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1}, {1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1}, {1, 0, 0, 0, 1, 1, 0, 1, 0, 0, 1},
    {1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1}, {1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 0}, {1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0},
    {1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0}, {1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0}, {1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1},
    {1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1}, {1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1}, {1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1},
    {1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0}, {1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1}, {1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0},
    {1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0}, {1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0}, {1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

static inline bool encode_M_basis_seq_u16(uint16_t w, uint32_t bit_idx)
{
  /// Table 5.2.2.6.4-1: Basis sequence for (32, O) code compressed in uint16_t types
  const uint16_t M_basis_seq_b[SRSLTE_UCI_M_BASIS_SEQ_LEN] = {
      0b10000000011, 0b11000000111, 0b11101001001, 0b10100001101, 0b10010001111, 0b10111010011, 0b11101010101,
      0b10110011001, 0b11010011011, 0b11001011101, 0b11011100101, 0b10101100111, 0b11110101001, 0b11010101011,
      0b10010110001, 0b11011110011, 0b01001110111, 0b00100111001, 0b00011111011, 0b00001100001, 0b10001000101,
      0b11000001011, 0b10110010001, 0b11100010111, 0b01111011111, 0b10011100011, 0b01100101101, 0b01110101111,
      0b00101110101, 0b00111111101, 0b11111111111, 0b00000000001,
  };

  // Apply mask
  uint16_t d = (uint16_t)w & M_basis_seq_b[bit_idx % SRSLTE_UCI_M_BASIS_SEQ_LEN];

  // Compute parity
  d ^= (uint16_t)(d >> 8U);
  d ^= (uint16_t)(d >> 4U);
  d &= 0xf;
  d = (0x6996U >> d) & 1U;

  // Return false if 0, otherwise it returns true
  return (d != 0);
}

static uint8_t M_basis_seq_pucch[20][13] = {
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0}, {1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0},
    {1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1}, {1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1},
    {1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 1}, {1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1},
    {1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1}, {1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1},
    {1, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 1, 1}, {1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1},
    {1, 0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1}, {1, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 1},
    {1, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1}, {1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1},
    {1, 0, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1}, {1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1},
    {1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 0, 1, 1}, {1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1},
    {1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0}, {1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0},
};

void srslte_uci_cqi_pucch_init(srslte_uci_cqi_pucch_t* q)
{
  uint8_t word[16];

  uint32_t nwords = 1 << SRSLTE_UCI_MAX_CQI_LEN_PUCCH;
  q->cqi_table    = srslte_vec_malloc(nwords * sizeof(int8_t*));
  q->cqi_table_s  = srslte_vec_malloc(nwords * sizeof(int16_t*));

  for (uint32_t w = 0; w < nwords; w++) {
    q->cqi_table[w]   = srslte_vec_malloc(SRSLTE_UCI_CQI_CODED_PUCCH_B * sizeof(int8_t));
    q->cqi_table_s[w] = srslte_vec_malloc(SRSLTE_UCI_CQI_CODED_PUCCH_B * sizeof(int16_t));
    uint8_t* ptr      = word;
    srslte_bit_unpack(w, &ptr, SRSLTE_UCI_MAX_CQI_LEN_PUCCH);
    srslte_uci_encode_cqi_pucch(word, SRSLTE_UCI_MAX_CQI_LEN_PUCCH, q->cqi_table[w]);
    for (int j = 0; j < SRSLTE_UCI_CQI_CODED_PUCCH_B; j++) {
      q->cqi_table_s[w][j] = (int16_t)(2 * q->cqi_table[w][j] - 1);
    }
  }
}

void srslte_uci_cqi_pucch_free(srslte_uci_cqi_pucch_t* q)
{
  uint32_t nwords = 1 << SRSLTE_UCI_MAX_CQI_LEN_PUCCH;
  for (uint32_t w = 0; w < nwords; w++) {
    if (q->cqi_table[w]) {
      free(q->cqi_table[w]);
    }
    if (q->cqi_table_s[w]) {
      free(q->cqi_table_s[w]);
    }
  }
  free(q->cqi_table);
  free(q->cqi_table_s);
}

/* Encode UCI CQI/PMI as described in 5.2.3.3 of 36.212
 */
int srslte_uci_encode_cqi_pucch(uint8_t* cqi_data, uint32_t cqi_len, uint8_t b_bits[SRSLTE_UCI_CQI_CODED_PUCCH_B])
{
  if (cqi_len <= SRSLTE_UCI_MAX_CQI_LEN_PUCCH) {
    for (uint32_t i = 0; i < SRSLTE_UCI_CQI_CODED_PUCCH_B; i++) {
      uint64_t x = 0;
      for (uint32_t n = 0; n < cqi_len; n++) {
        x += cqi_data[n] * M_basis_seq_pucch[i][n];
      }
      b_bits[i] = (uint8_t)(x % 2);
    }
    return SRSLTE_SUCCESS;
  } else {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }
}

int srslte_uci_encode_cqi_pucch_from_table(srslte_uci_cqi_pucch_t* q,
                                           uint8_t*                cqi_data,
                                           uint32_t                cqi_len,
                                           uint8_t                 b_bits[SRSLTE_UCI_CQI_CODED_PUCCH_B])
{
  if (cqi_len <= SRSLTE_UCI_MAX_CQI_LEN_PUCCH) {
    bzero(&cqi_data[cqi_len], SRSLTE_UCI_MAX_CQI_LEN_PUCCH - cqi_len);
    uint8_t* ptr    = cqi_data;
    uint32_t packed = srslte_bit_pack(&ptr, SRSLTE_UCI_MAX_CQI_LEN_PUCCH);
    memcpy(b_bits, q->cqi_table[packed], SRSLTE_UCI_CQI_CODED_PUCCH_B);

    return SRSLTE_SUCCESS;
  } else {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }
}

/* Decode UCI CQI/PMI over PUCCH
 */
int16_t srslte_uci_decode_cqi_pucch(srslte_uci_cqi_pucch_t* q,
                                    int16_t                 b_bits[SRSLTE_CQI_MAX_BITS],
                                    uint8_t*                cqi_data,
                                    uint32_t                cqi_len)
{
  if (cqi_len < SRSLTE_UCI_MAX_CQI_LEN_PUCCH && b_bits != NULL && cqi_data != NULL) {
    uint32_t max_w    = 0;
    int32_t  max_corr = INT32_MIN;
    uint32_t nwords   = 1 << SRSLTE_UCI_MAX_CQI_LEN_PUCCH;
    for (uint32_t w = 0; w < nwords; w += 1 << (SRSLTE_UCI_MAX_CQI_LEN_PUCCH - cqi_len)) {

      // Calculate correlation with pregenerated word and select maximum
      int32_t corr = srslte_vec_dot_prod_sss(q->cqi_table_s[w], b_bits, SRSLTE_UCI_CQI_CODED_PUCCH_B);
      if (corr > max_corr) {
        max_corr = corr;
        max_w    = w;
      }
    }
    // Convert word to bits again
    uint8_t* ptr = cqi_data;
    srslte_bit_unpack(max_w, &ptr, SRSLTE_UCI_MAX_CQI_LEN_PUCCH);

    INFO("Decoded CQI: w=%d, corr=%d\n", max_w, max_corr);
    return max_corr;
  } else {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }
}

void srslte_uci_encode_m_basis_bits(const uint8_t* input, uint32_t input_len, uint8_t* output, uint32_t output_len)
{
  // Limit number of input bits
  input_len = SRSLTE_MIN(input_len, SRSLTE_UCI_MAX_ACK_SR_BITS);

  // Pack input bits
  uint16_t w = 0;
  for (uint32_t i = 0; i < input_len; i++) {
    w |= (input[i] & 1U) << i;
  }

  // Encode bits
  for (uint32_t i = 0; i < SRSLTE_MIN(output_len, SRSLTE_UCI_M_BASIS_SEQ_LEN); i++) {
    output[i] = encode_M_basis_seq_u16(w, i);
  }

  // Avoid repeating operation by copying repeated sequence
  for (uint32_t i = SRSLTE_UCI_M_BASIS_SEQ_LEN; i < output_len; i++) {
    output[i] = output[i % SRSLTE_UCI_M_BASIS_SEQ_LEN];
  }
}

int32_t srslte_uci_decode_m_basis_bits(const int16_t* llr, uint32_t nof_llr, uint8_t* data, uint32_t data_len)
{
  int32_t  max_corr = 0; ///< Stores maximum correlation
  uint16_t max_data = 0; ///< Stores the word for maximum correlation

  // Return invalid inputs if data is not provided
  if (!llr || !data) {
    ERROR("Invalid inputs\n");
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  // Return invalid inputs if not enough LLR are provided
  if (nof_llr < SRSLTE_UCI_M_BASIS_SEQ_LEN) {
    ERROR("Not enough LLR bits are provided %d. Required %d;\n", nof_llr, SRSLTE_UCI_M_BASIS_SEQ_LEN);
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  // Limit data to maximum
  data_len = SRSLTE_MIN(data_len, SRSLTE_UCI_MAX_ACK_SR_BITS);

  // Brute force all possible sequences
  uint16_t max_guess = (1 << data_len); ///< Maximum guess bit combination
  for (uint16_t guess = 0; guess < max_guess; guess++) {
    int32_t corr = 0;

    /// Compute correlation for the number of LLR
    bool early_termination = false;
    for (uint32_t i = 0; i < nof_llr && !early_termination; i++) {
      // Encode guess word
      bool d = encode_M_basis_seq_u16(guess, i);

      // Correlate
      corr += (int32_t)(d ? llr[i] : -llr[i]);

      // Limit correlation to half range
      corr = SRSLTE_MIN(corr, INT32_MAX / 2);

      /// Early terminates if at least SRSLTE_UCI_M_BASIS_SEQ_LEN/4 LLR processed and negative correlation
      early_termination |= (i > SRSLTE_UCI_M_BASIS_SEQ_LEN / 4) && (corr < 0);

      /// Early terminates if the correlation overflows
      early_termination |= (corr < -INT32_MAX / 2);
    }

    // Take decision
    if (corr > max_corr) {
      max_corr = corr;
      max_data = guess;
    }
  }

  // Unpack
  for (uint32_t i = 0; i < data_len; i++) {
    data[i] = (uint8_t)((max_data >> i) & 1U);
  }

  // Return correlation
  return max_corr;
}

void cqi_pusch_pregen(srslte_uci_cqi_pusch_t* q)
{
  uint8_t word[11];

  for (int i = 0; i < 11; i++) {
    uint32_t nwords   = (1 << (i + 1));
    q->cqi_table[i]   = srslte_vec_u8_malloc(nwords * 32);
    q->cqi_table_s[i] = srslte_vec_i16_malloc(nwords * 32);
    for (uint32_t w = 0; w < nwords; w++) {
      uint8_t* ptr = word;
      srslte_bit_unpack(w, &ptr, i + 1);
      srslte_uci_encode_m_basis_bits(word, i + 1, &q->cqi_table[i][32 * w], SRSLTE_UCI_M_BASIS_SEQ_LEN);
      for (int j = 0; j < 32; j++) {
        q->cqi_table_s[i][32 * w + j] = 2 * q->cqi_table[i][32 * w + j] - 1;
      }
    }
  }
}

void cqi_pusch_pregen_free(srslte_uci_cqi_pusch_t* q)
{
  for (int i = 0; i < 11; i++) {
    if (q->cqi_table[i]) {
      free(q->cqi_table[i]);
    }
    if (q->cqi_table_s[i]) {
      free(q->cqi_table_s[i]);
    }
  }
}

int srslte_uci_cqi_init(srslte_uci_cqi_pusch_t* q)
{
  if (srslte_crc_init(&q->crc, SRSLTE_LTE_CRC8, 8)) {
    return SRSLTE_ERROR;
  }
  int poly[3] = {0x6D, 0x4F, 0x57};
  if (srslte_viterbi_init(&q->viterbi, SRSLTE_VITERBI_37, poly, SRSLTE_UCI_MAX_CQI_LEN_PUSCH, true)) {
    return SRSLTE_ERROR;
  }

  cqi_pusch_pregen(q);

  return SRSLTE_SUCCESS;
}

void srslte_uci_cqi_free(srslte_uci_cqi_pusch_t* q)
{
  srslte_viterbi_free(&q->viterbi);

  cqi_pusch_pregen_free(q);
}

static uint32_t Q_prime_cqi(srslte_pusch_cfg_t* cfg, uint32_t O, float beta, uint32_t Q_prime_ri)
{

  uint32_t K = cfg->K_segm;

  uint32_t Q_prime = 0;
  uint32_t L       = (O < 11) ? 0 : 8;
  uint32_t x       = 999999;

  if (K > 0) {
    x = (uint32_t)ceilf((float)(O + L) * cfg->grant.L_prb * SRSLTE_NRE * cfg->grant.nof_symb * beta / K);
  }

  Q_prime = SRSLTE_MIN(x, cfg->grant.L_prb * SRSLTE_NRE * cfg->grant.nof_symb - Q_prime_ri);

  return Q_prime;
}

uint32_t srslte_qprime_cqi_ext(uint32_t L_prb, uint32_t nof_symbols, uint32_t tbs, float beta)
{
  srslte_pusch_cfg_t cfg = {};
  cfg.grant.L_prb        = L_prb;
  cfg.grant.nof_symb     = nof_symbols;
  cfg.K_segm             = tbs;
  // O is the number of CQI + CRC len (8). See 5.2.2.6
  return Q_prime_cqi(&cfg, SRSLTE_UCI_CQI_CODED_PUCCH_B + 8, beta, 0);
}

/* Encode UCI CQI/PMI for payloads equal or lower to 11 bits (Sec 5.2.2.6.4)
 */
int encode_cqi_short(srslte_uci_cqi_pusch_t* q, uint8_t* data, uint32_t nof_bits, uint8_t* q_bits, uint32_t Q)
{
  if (nof_bits <= 11 && nof_bits > 0 && q != NULL && data != NULL && q_bits != NULL) {
    uint8_t* ptr = data;
    uint32_t w   = srslte_bit_pack(&ptr, nof_bits);

    for (int i = 0; i < Q; i++) {
      q_bits[i] = q->cqi_table[nof_bits - 1][w * 32 + (i % 32)];
    }
    return SRSLTE_SUCCESS;
  } else {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }
}

// For decoding the block-encoded CQI we use ML decoding
int decode_cqi_short(srslte_uci_cqi_pusch_t* q, int16_t* q_bits, uint32_t Q, uint8_t* data, uint32_t nof_bits)
{
  if (nof_bits <= 11 && nof_bits > 0 && q != NULL && data != NULL && q_bits != NULL) {
    // Accumulate all copies of the 32-length sequence
    if (Q > 32) {
      int i = 1;
      for (; i < Q / 32; i++) {
        srslte_vec_sum_sss(&q_bits[i * 32], q_bits, q_bits, 32);
      }
      srslte_vec_sum_sss(&q_bits[i * 32], q_bits, q_bits, Q % 32);
    }

    uint32_t max_w    = 0;
    int32_t  max_corr = INT32_MIN;
    for (uint32_t w = 0; w < (1 << nof_bits); w++) {

      // Calculate correlation with pregenerated word and select maximum
      int32_t corr = srslte_vec_dot_prod_sss(&q->cqi_table_s[nof_bits - 1][w * 32], q_bits, SRSLTE_MIN(32, Q));
      if (corr > max_corr) {
        max_corr = corr;
        max_w    = w;
      }
    }
    // Convert word to bits again
    uint8_t* ptr = data;
    srslte_bit_unpack(max_w, &ptr, nof_bits);

    INFO("Decoded CQI: w=%d, corr=%d\n", max_w, max_corr);
    return SRSLTE_SUCCESS;
  } else {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }
}

/* Encode UCI CQI/PMI for payloads greater than 11 bits (go through CRC, conv coder and rate match)
 */
int encode_cqi_long(srslte_uci_cqi_pusch_t* q, uint8_t* data, uint32_t nof_bits, uint8_t* q_bits, uint32_t Q)
{
  srslte_convcoder_t encoder;

  if (nof_bits + 8 < SRSLTE_UCI_MAX_CQI_LEN_PUSCH && q != NULL && data != NULL && q_bits != NULL) {
    int poly[3]         = {0x6D, 0x4F, 0x57};
    encoder.K           = 7;
    encoder.R           = 3;
    encoder.tail_biting = true;
    memcpy(encoder.poly, poly, 3 * sizeof(int));

    memcpy(q->tmp_cqi, data, sizeof(uint8_t) * nof_bits);
    srslte_crc_attach(&q->crc, q->tmp_cqi, nof_bits);

    DEBUG("cqi_crc_tx=");
    if (SRSLTE_VERBOSE_ISDEBUG()) {
      srslte_vec_fprint_b(stdout, q->tmp_cqi, nof_bits + 8);
    }

    srslte_convcoder_encode(&encoder, q->tmp_cqi, q->encoded_cqi, nof_bits + 8);

    DEBUG("cconv_tx=");
    if (SRSLTE_VERBOSE_ISDEBUG()) {
      srslte_vec_fprint_b(stdout, q->encoded_cqi, 3 * (nof_bits + 8));
    }

    srslte_rm_conv_tx(q->encoded_cqi, 3 * (nof_bits + 8), q_bits, Q);

    return SRSLTE_SUCCESS;
  } else {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }
}

int decode_cqi_long(srslte_uci_cqi_pusch_t* q, int16_t* q_bits, uint32_t Q, uint8_t* data, uint32_t nof_bits)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;
  if (nof_bits + 8 < SRSLTE_UCI_MAX_CQI_LEN_PUSCH && q != NULL && data != NULL && q_bits != NULL) {

    srslte_rm_conv_rx_s(q_bits, Q, q->encoded_cqi_s, 3 * (nof_bits + 8));

    DEBUG("cconv_rx=");
    if (SRSLTE_VERBOSE_ISDEBUG()) {
      srslte_vec_fprint_s(stdout, q->encoded_cqi_s, 3 * (nof_bits + 8));
    }

    srslte_viterbi_decode_s(&q->viterbi, q->encoded_cqi_s, q->tmp_cqi, nof_bits + 8);

    DEBUG("cqi_crc_rx=");
    if (SRSLTE_VERBOSE_ISDEBUG()) {
      srslte_vec_fprint_b(stdout, q->tmp_cqi, nof_bits + 8);
    }

    ret = srslte_crc_checksum(&q->crc, q->tmp_cqi, nof_bits + 8);
    if (ret == 0) {
      memcpy(data, q->tmp_cqi, nof_bits * sizeof(uint8_t));
      ret = 1;
    } else {
      ret = 0;
    }
  }
  return ret;
}

/* Encode UCI CQI/PMI
 */
int srslte_uci_decode_cqi_pusch(srslte_uci_cqi_pusch_t* q,
                                srslte_pusch_cfg_t*     cfg,
                                int16_t*                q_bits,
                                float                   beta,
                                uint32_t                Q_prime_ri,
                                uint32_t                cqi_len,
                                uint8_t*                cqi_data,
                                bool*                   cqi_ack)
{
  if (beta < 0) {
    ERROR("Error beta is reserved\n");
    return -1;
  }
  uint32_t Q_prime = Q_prime_cqi(cfg, cqi_len, beta, Q_prime_ri);
  uint32_t Qm      = srslte_mod_bits_x_symbol(cfg->grant.tb.mod);

  int ret = SRSLTE_ERROR;
  if (cqi_len <= 11) {
    ret = decode_cqi_short(q, q_bits, Q_prime * Qm, cqi_data, cqi_len);
    if (cqi_ack) {
      *cqi_ack = true;
    }
  } else {
    ret = decode_cqi_long(q, q_bits, Q_prime * Qm, cqi_data, cqi_len);
    if (ret == 1) {
      if (cqi_ack) {
        *cqi_ack = true;
      }
      ret = 0;
    } else if (ret == 0) {
      if (cqi_ack) {
        *cqi_ack = false;
      }
    }
  }
  if (ret) {
    return ret;
  } else {
    return (int)Q_prime;
  }
}

/* Encode UCI CQI/PMI as described in 5.2.2.6 of 36.212
 */
int srslte_uci_encode_cqi_pusch(srslte_uci_cqi_pusch_t* q,
                                srslte_pusch_cfg_t*     cfg,
                                uint8_t*                cqi_data,
                                uint32_t                cqi_len,
                                float                   beta,
                                uint32_t                Q_prime_ri,
                                uint8_t*                q_bits)
{
  if (beta < 0) {
    ERROR("Error beta is reserved\n");
    return -1;
  }

  uint32_t Q_prime = Q_prime_cqi(cfg, cqi_len, beta, Q_prime_ri);
  uint32_t Qm      = srslte_mod_bits_x_symbol(cfg->grant.tb.mod);

  int ret = SRSLTE_ERROR;
  if (cqi_len <= 11) {
    ret = encode_cqi_short(q, cqi_data, cqi_len, q_bits, Q_prime * Qm);
  } else {
    ret = encode_cqi_long(q, cqi_data, cqi_len, q_bits, Q_prime * Qm);
  }
  if (ret) {
    return ret;
  } else {
    return (int)Q_prime;
  }
}

/* Generates UCI-ACK bits and computes position in q bits */
static int uci_ulsch_interleave_ack_gen(uint32_t          ack_q_bit_idx,
                                        uint32_t          Qm,
                                        uint32_t          H_prime_total,
                                        uint32_t          N_pusch_symbs,
                                        srslte_uci_bit_t* ack_bits)
{

  const uint32_t ack_column_set_norm[4] = {2, 3, 8, 9};
  const uint32_t ack_column_set_ext[4]  = {1, 2, 6, 7};

  if (H_prime_total / N_pusch_symbs >= 1 + ack_q_bit_idx / 4) {
    uint32_t row    = H_prime_total / N_pusch_symbs - 1 - ack_q_bit_idx / 4;
    uint32_t colidx = (3 * ack_q_bit_idx) % 4;
    uint32_t col    = N_pusch_symbs > 10 ? ack_column_set_norm[colidx] : ack_column_set_ext[colidx];
    for (uint32_t k = 0; k < Qm; k++) {
      ack_bits[k].position = row * Qm + (H_prime_total / N_pusch_symbs) * col * Qm + k;
    }
    return SRSLTE_SUCCESS;
  } else {
    ERROR("Error interleaving UCI-ACK bit idx %d for H_prime_total=%d and N_pusch_symbs=%d\n",
          ack_q_bit_idx,
          H_prime_total,
          N_pusch_symbs);
    return SRSLTE_ERROR;
  }
}

/* Inserts UCI-RI bits into the correct positions in the g buffer before interleaving */
static int uci_ulsch_interleave_ri_gen(uint32_t          ri_q_bit_idx,
                                       uint32_t          Qm,
                                       uint32_t          H_prime_total,
                                       uint32_t          N_pusch_symbs,
                                       srslte_uci_bit_t* ri_bits)
{

  static uint32_t ri_column_set_norm[4] = {1, 4, 7, 10};
  static uint32_t ri_column_set_ext[4]  = {0, 3, 5, 8};

  if (H_prime_total / N_pusch_symbs >= 1 + ri_q_bit_idx / 4) {
    uint32_t row    = H_prime_total / N_pusch_symbs - 1 - ri_q_bit_idx / 4;
    uint32_t colidx = (3 * ri_q_bit_idx) % 4;
    uint32_t col    = N_pusch_symbs > 10 ? ri_column_set_norm[colidx] : ri_column_set_ext[colidx];

    for (uint32_t k = 0; k < Qm; k++) {
      ri_bits[k].position = row * Qm + (H_prime_total / N_pusch_symbs) * col * Qm + k;
    }
    return SRSLTE_SUCCESS;
  } else {
    ERROR("Error interleaving UCI-RI bit idx %d for H_prime_total=%d and N_pusch_symbs=%d\n",
          ri_q_bit_idx,
          H_prime_total,
          N_pusch_symbs);
    return SRSLTE_ERROR;
  }
}

static uint32_t Q_prime_ri_ack(srslte_pusch_cfg_t* cfg, uint32_t O, uint32_t O_cqi, float beta)
{

  if (beta < 0) {
    ERROR("Error beta is reserved\n");
    return -1;
  }

  uint32_t K = cfg->K_segm;

  // If not carrying UL-SCH, get Q_prime according to 5.2.4.1
  if (K == 0) {
    if (O_cqi <= 11) {
      K = O_cqi;
    } else {
      K = O_cqi + 8;
    }
  }

  uint32_t x = (uint32_t)ceilf((float)O * cfg->grant.L_prb * SRSLTE_NRE * cfg->grant.nof_symb * beta / K);

  uint32_t Q_prime = SRSLTE_MIN(x, 4 * cfg->grant.L_prb * SRSLTE_NRE);

  return Q_prime;
}

uint32_t srslte_qprime_ack_ext(uint32_t L_prb, uint32_t nof_symbols, uint32_t tbs, uint32_t nof_ack, float beta)
{
  srslte_pusch_cfg_t cfg = {};
  cfg.grant.L_prb        = L_prb;
  cfg.grant.nof_symb     = nof_symbols;
  cfg.K_segm             = tbs;
  return Q_prime_ri_ack(&cfg, nof_ack, 0, beta);
}

static uint32_t encode_ri_ack(const uint8_t data[2], uint32_t O_ack, uint8_t Qm, srslte_uci_bit_t* q_encoded_bits)
{
  uint32_t i = 0;

  if (O_ack == 1) {
    q_encoded_bits[i++].type = data[0] ? UCI_BIT_1 : UCI_BIT_0;
    q_encoded_bits[i++].type = UCI_BIT_REPETITION;
    while (i < Qm) {
      q_encoded_bits[i++].type = UCI_BIT_PLACEHOLDER;
    }
  } else if (O_ack == 2) {
    q_encoded_bits[i++].type = data[0] ? UCI_BIT_1 : UCI_BIT_0;
    q_encoded_bits[i++].type = data[1] ? UCI_BIT_1 : UCI_BIT_0;
    while (i < Qm) {
      q_encoded_bits[i++].type = UCI_BIT_PLACEHOLDER;
    }
    q_encoded_bits[i++].type = (data[0] ^ data[1]) ? UCI_BIT_1 : UCI_BIT_0;
    q_encoded_bits[i++].type = data[0] ? UCI_BIT_1 : UCI_BIT_0;
    while (i < Qm * 2) {
      q_encoded_bits[i++].type = UCI_BIT_PLACEHOLDER;
    }
    q_encoded_bits[i++].type = data[1] ? UCI_BIT_1 : UCI_BIT_0;
    q_encoded_bits[i++].type = (data[0] ^ data[1]) ? UCI_BIT_1 : UCI_BIT_0;
    while (i < Qm * 3) {
      q_encoded_bits[i++].type = UCI_BIT_PLACEHOLDER;
    }
  }

  return i;
}

static uint32_t
encode_ack_long(const uint8_t* data, uint32_t O_ack, uint8_t Q_m, uint32_t Q_prime, srslte_uci_bit_t* q_encoded_bits)
{
  uint32_t Q_ack = Q_m * Q_prime;

  if (O_ack > SRSLTE_UCI_MAX_ACK_BITS) {
    ERROR("Error encoding long ACK bits: O_ack can't be higher than %d\n", SRSLTE_UCI_MAX_ACK_BITS);
    return 0;
  }

  for (uint32_t i = 0; i < Q_ack; i++) {
    uint32_t q_i = 0;
    for (uint32_t n = 0; n < O_ack; n++) {
      q_i = (q_i + (data[n] * M_basis_seq[i % 32][n])) % 2;
    }
    q_encoded_bits[i].type = q_i ? UCI_BIT_1 : UCI_BIT_0;
  }

  return Q_ack;
}

static int32_t decode_ri_ack_1bit(const int16_t* q_bits, const uint8_t* c_seq, uint8_t data[1])
{
  int32_t sum = (int32_t)(q_bits[0] + q_bits[1]);

  if (data) {
    data[0] = (sum > 0) ? 1 : 0;
  }

  return abs(sum);
}

static int32_t decode_ri_ack_2bits(const int16_t* llr, uint32_t Qm, uint8_t data[2])
{
  uint32_t p0 = Qm * 0 + 0;
  uint32_t p1 = Qm * 0 + 1;
  uint32_t p2 = Qm * 1 + 0;
  uint32_t p3 = Qm * 1 + 1;
  uint32_t p4 = Qm * 2 + 0;
  uint32_t p5 = Qm * 2 + 1;

  int16_t sum1 = llr[p0] + llr[p3];
  int16_t sum2 = llr[p1] + llr[p4];
  int16_t sum3 = llr[p2] + llr[p5];

  data[0] = (sum1 > 0) ? 1 : 0;
  data[1] = (sum2 > 0) ? 1 : 0;

  bool parity_check = (sum3 > 0) == (data[0] ^ data[1]);

  // Return 0 if parity check is not valid
  return (parity_check ? (abs(sum1) + abs(sum2) + abs(sum3)) : 0);
}

// Table 5.2.2.6-A
const static uint8_t w_scram[4][4] = {{1, 1, 1, 1}, {1, 0, 1, 0}, {1, 1, 0, 0}, {1, 0, 0, 1}};

static void uci_ack_scramble_tdd(srslte_uci_bit_t* q, uint32_t O_ack, uint32_t Q_ack, uint32_t N_bundle)
{
  if (N_bundle == 0) {
    return;
  }

  uint32_t wi = (N_bundle - 1) % 4;

  uint32_t m = O_ack == 1 ? 1 : 3;

  srslte_uci_bit_type_t q_m1 = q[0].type;
  uint32_t              k    = 0;
  for (uint32_t i = 0; i < Q_ack; i++) {
    switch (q[i].type) {
      case UCI_BIT_REPETITION:
        // A repetition bit always comes after a 1 or 0 so we can do i-1
        if (i > 0) {
          q[i].type = ((q_m1 == UCI_BIT_1 ? 1 : 0) + w_scram[wi][k / m]) % 2;
        }
        k = (k + 1) % (4 * m);
        break;
      case UCI_BIT_PLACEHOLDER:
        // do not change
        break;
      default:
        q_m1      = q[i].type;
        q[i].type = ((q[i].type == UCI_BIT_1 ? 1 : 0) + w_scram[wi][k / m]) % 2;
        k         = (k + 1) % (4 * m);
        break;
    }
  }
}

/* Encode UCI ACK/RI bits as described in 5.2.2.6 of 36.212
 *  Currently only supporting 1-bit RI
 */
int srslte_uci_encode_ack_ri(srslte_pusch_cfg_t* cfg,
                             uint8_t*            data,
                             uint32_t            O_ack,
                             uint32_t            O_cqi,
                             float               beta,
                             uint32_t            H_prime_total,
                             bool                input_is_ri,
                             uint32_t            N_bundle,
                             srslte_uci_bit_t*   bits)
{
  if (beta < 0) {
    ERROR("Error beta is reserved\n");
    return -1;
  }
  uint32_t Q_prime = Q_prime_ri_ack(cfg, O_ack, O_cqi, beta);

  uint32_t Q_ack = 0;
  uint32_t Qm    = srslte_mod_bits_x_symbol(cfg->grant.tb.mod);

  if (O_ack < 3) {
    uint32_t enc_len = encode_ri_ack(data, O_ack, Qm, bits);
    // Repeat bits Q_prime times, remainder bits will be ignored later
    while (Q_ack < Q_prime * Qm) {
      for (uint32_t j = 0; j < enc_len; j++) {
        bits[Q_ack++].type = bits[j].type;
      }
    }
  } else {
    Q_ack = encode_ack_long(data, O_ack, Qm, Q_prime, bits);
  }

  // Generate interleaver positions
  if (Q_ack > 0) {
    for (uint32_t i = 0; i < Q_prime; i++) {
      if (input_is_ri) {
        uci_ulsch_interleave_ri_gen(i, Qm, H_prime_total, cfg->grant.nof_symb, &bits[Qm * i]);
      } else {
        uci_ulsch_interleave_ack_gen(i, Qm, H_prime_total, cfg->grant.nof_symb, &bits[Qm * i]);
      }
    }

    // TDD-bundling scrambling
    if (!input_is_ri && N_bundle && O_ack > 0) {
      uci_ack_scramble_tdd(bits, O_ack, Q_prime * Qm, N_bundle);
    }
  }

  return (int)Q_prime;
}

/* Decode UCI ACK/RI bits as described in 5.2.2.6 of 36.212
 *  Currently only supporting 1-bit RI
 */
int srslte_uci_decode_ack_ri(srslte_pusch_cfg_t* cfg,
                             int16_t*            q_bits,
                             uint8_t*            c_seq,
                             float               beta,
                             uint32_t            H_prime_total,
                             uint32_t            O_cqi,
                             srslte_uci_bit_t*   ack_ri_bits,
                             uint8_t*            data,
                             bool*               valid,
                             uint32_t            nof_bits,
                             bool                is_ri)
{
  if (beta < 0) {
    ERROR("Error beta (%f) is reserved\n", beta);
    return SRSLTE_ERROR;
  }

  uint32_t Qprime = Q_prime_ri_ack(cfg, nof_bits, O_cqi, beta);
  uint32_t Qm     = srslte_mod_bits_x_symbol(cfg->grant.tb.mod);

  int16_t  llr_acc[32] = {}; ///< LLR accumulator
  uint32_t nof_acc =
      (nof_bits == 1) ? Qm : (nof_bits == 2) ? Qm * 3 : SRSLTE_UCI_M_BASIS_SEQ_LEN; ///< Number of required LLR
  uint32_t count_acc = 0;                                                           ///< LLR counter

  for (uint32_t i = 0; i < Qprime; i++) {
    if (is_ri) {
      uci_ulsch_interleave_ri_gen(i, Qm, H_prime_total, cfg->grant.nof_symb, &ack_ri_bits[count_acc]);
    } else {
      uci_ulsch_interleave_ack_gen(i, Qm, H_prime_total, cfg->grant.nof_symb, &ack_ri_bits[count_acc]);
    }

    /// Extract and accumulate LLR
    for (uint32_t j = 0; j < Qm; j++, count_acc++) {
      // Calculate circular LLR index
      uint32_t acc_idx = count_acc % nof_acc;
      uint32_t pos     = ack_ri_bits[count_acc].position;

      int16_t q = q_bits[pos];

      // Remove scrambling of repeated bits
      if (nof_bits == 1) {
        if (acc_idx == 1 && pos > 0) {
          q = (c_seq[pos] == c_seq[pos - 1]) ? +q : -q;
        }
      }

      // Accumulate LLR
      llr_acc[acc_idx] += q;

      /// Limit accumulator boundaries
      llr_acc[acc_idx] = SRSLTE_MIN(llr_acc[acc_idx], INT16_MAX / 2);
      llr_acc[acc_idx] = SRSLTE_MAX(llr_acc[acc_idx], -INT16_MAX / 2);
    }
  }

  /// Decode UCI HARQ/ACK bits as described in 5.2.2.6 of 36.212
  int32_t thr  = count_acc * ((Qm < 4) ? 100 : (Qm < 6) ? 200 : (Qm < 8) ? 700 : 1000) / 2;
  int32_t corr = 0;
  switch (nof_bits) {
    case 1:
      corr = decode_ri_ack_1bit(llr_acc, c_seq, data);
      break;
    case 2:
      corr = decode_ri_ack_2bits(llr_acc, Qm, data);
      break;
    default:
      // For more than 2 bits...
      corr = srslte_uci_decode_m_basis_bits(llr_acc, SRSLTE_UCI_M_BASIS_SEQ_LEN, data, nof_bits);
  }

  if (valid) {
    *valid = corr > thr;
  }

  return (int)Qprime;
}

uint32_t srslte_uci_cfg_total_ack(const srslte_uci_cfg_t* uci_cfg)
{
  uint32_t nof_ack = 0;
  for (uint32_t i = 0; i < SRSLTE_MAX_CARRIERS; i++) {
    nof_ack += uci_cfg->ack[i].nof_acks;
  }
  return nof_ack;
}

void srslte_uci_data_reset(srslte_uci_data_t* uci_data)
{
  bzero(uci_data, sizeof(srslte_uci_data_t));

  /* Set all ACKs to DTX */
  memset(uci_data->value.ack.ack_value, 2, SRSLTE_UCI_MAX_ACK_BITS);
}

int srslte_uci_data_info(srslte_uci_cfg_t* uci_cfg, srslte_uci_value_t* uci_data, char* str, uint32_t str_len)
{
  int n = 0;

  if (uci_cfg->is_scheduling_request_tti) {
    n = srslte_print_check(str, str_len, n, ", sr=%s", uci_data->scheduling_request ? "yes" : "no");
  }

  uint32_t nof_acks = srslte_uci_cfg_total_ack(uci_cfg);
  if (nof_acks) {
    n = srslte_print_check(str, str_len, n, ", ack=");
    if (uci_data->ack.valid) {

      for (uint32_t i = 0; i < nof_acks; i++) {
        n = srslte_print_check(str, str_len, n, "%d", uci_data->ack.ack_value[i]);
      }
      if (uci_cfg->ack[0].N_bundle) {
        n = srslte_print_check(str, str_len, n, ", n_bundle=%d", uci_cfg->ack[0].N_bundle);
      }
    } else {
      n = srslte_print_check(str, str_len, n, "invalid");
    }
  }

  if (uci_cfg->cqi.ri_len) {
    n = srslte_print_check(str, str_len, n, ", ri=%d", uci_data->ri);
  }

  if (uci_cfg->cqi.data_enable) {
    char cqi_str[SRSLTE_CQI_STR_MAX_CHAR] = "";
    srslte_cqi_value_tostring(&uci_cfg->cqi, &uci_data->cqi, cqi_str, SRSLTE_CQI_STR_MAX_CHAR);
    n = srslte_print_check(str, str_len, n, "%s (cc=%d)", cqi_str, uci_cfg->cqi.scell_index);
  }

  return n;
}
