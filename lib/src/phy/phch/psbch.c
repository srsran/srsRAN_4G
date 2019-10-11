/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

#include <srslte/phy/phch/psbch.h>

#include <srslte/phy/fec/rm_conv.h>
#include <srslte/phy/modem/mod.h>
#include <srslte/phy/utils/bit.h>
#include <srslte/phy/utils/debug.h>
#include <srslte/phy/utils/vector.h>
#include <stdlib.h>
#include <string.h>

void slbch_interleave_gen(uint32_t H_prime_total, uint32_t N_pusch_symbs, uint32_t Qm, uint32_t* interleaver_lut)
{
  uint32_t NL   = 1;
  uint32_t Cmux = N_pusch_symbs;
  uint32_t Rmux = H_prime_total * Qm * NL / Cmux;
  uint32_t y_indices[Rmux][Cmux];

  for (int i = 0; i < Rmux; i++) {
    for (int k = 0; k < Cmux; k++) {
      y_indices[i][k] = Rmux * k + i;
    }
  }

  uint32_t arrayIdx = 0;
  for (int i = 0; i < Rmux; i += 2) {
    for (int k = 0; k < Cmux; k++) {
      interleaver_lut[arrayIdx++] = y_indices[i][k];
      interleaver_lut[arrayIdx++] = y_indices[i + 1][k];
    }
  }
}

int srslte_psbch_init(srslte_psbch_t* q, uint32_t N_sl_id, uint32_t nof_prb)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q != NULL) {

    ret = SRSLTE_ERROR;

    q->nof_prb = nof_prb;
    q->N_sl_id = N_sl_id;

    q->nof_prb_psbch = 6;

    q->a = srslte_vec_malloc(sizeof(uint8_t) * SRSLTE_SL_BCH_PAYLOADCRC_LEN);
    if (!q->a) {
      ERROR("Error allocating memmory\n");
      return SRSLTE_ERROR;
    }
    q->d = srslte_vec_malloc(sizeof(uint8_t) * SRSLTE_SL_BCH_ENCODED_LEN);
    if (!q->d) {
      ERROR("Error allocating memmory\n");
      return SRSLTE_ERROR;
    }
    bzero(q->d, sizeof(uint8_t) * SRSLTE_SL_BCH_ENCODED_LEN);

    // crc
    q->crc_poly = 0x11021;
    if (srslte_crc_init(&q->crc_mib, q->crc_poly, SRSLTE_SL_BCH_CRC_LEN)) {
      ERROR("Error crc init");
      return SRSLTE_ERROR;
    }
    q->crc_temp = srslte_vec_malloc(sizeof(uint8_t) * SRSLTE_SL_BCH_CRC_LEN);
    if (!q->crc_temp) {
      ERROR("Error allocating memmory\n");
      return SRSLTE_ERROR;
    }

    // channel coding
    q->encoder.K           = 7;
    q->encoder.R           = 3;
    q->encoder.tail_biting = true;
    int poly[3]            = {0x6D, 0x4F, 0x57};
    memcpy(q->encoder.poly, poly, 3 * sizeof(int));

    // channel decoding
    if (srslte_viterbi_init(&q->dec, SRSLTE_VITERBI_37, poly, 56, true)) {
      return SRSLTE_ERROR;
    }

    // rate matching
    q->nof_symbols = 8;
    q->E           = q->nof_symbols * SRSLTE_NRE * q->nof_prb_psbch * 2;
    q->e           = srslte_vec_malloc(sizeof(uint8_t) * q->E);
    if (!q->e) {
      ERROR("Error allocating memmory\n");
      return SRSLTE_ERROR;
    }
    q->e_16 = srslte_vec_malloc(sizeof(int16_t) * q->E);
    if (!q->e_16) {
      ERROR("Error allocating memmory\n");
      return SRSLTE_ERROR;
    }
    bzero(q->e, sizeof(uint8_t) * q->E);

    // interleaving
    q->Qm              = 2; // Always QPSK
    q->interleaver_lut = srslte_vec_malloc(sizeof(uint32_t) * q->E);
    if (!q->interleaver_lut) {
      ERROR("Error allocating memmory\n");
      return SRSLTE_ERROR;
    }
    slbch_interleave_gen(q->E / 2, q->nof_symbols, q->Qm, q->interleaver_lut);
    q->codeword = srslte_vec_malloc(sizeof(uint8_t) * q->E);
    if (!q->codeword) {
      ERROR("Error allocating memmory\n");
      return SRSLTE_ERROR;
    }

    // scrambling
    bzero(&q->seq, sizeof(srslte_sequence_t));
    srslte_sequence_LTE_pr(&q->seq, q->E, q->N_sl_id);

    // modulation
    if (srslte_modem_table_lte(&q->mod, SRSLTE_MOD_QPSK)) {
      return SRSLTE_ERROR;
    }

    q->len_after_mod = q->E / q->mod.nbits_x_symbol;
    q->symbols       = srslte_vec_malloc(sizeof(cf_t) * q->len_after_mod);
    if (!q->symbols) {
      ERROR("Error allocating memmory\n");
      return SRSLTE_ERROR;
    }
    bzero(q->symbols, sizeof(cf_t) * q->len_after_mod);

    q->llr = srslte_vec_malloc(sizeof(float) * q->E);
    if (!q->llr) {
      ERROR("Error allocating memmory\n");
      return SRSLTE_ERROR;
    }
    bzero(q->llr, sizeof(float) * q->E);

    // dft precoding
    q->precoding_scaling = 1.0;
    if (srslte_dft_precoding_init(&q->dft_precoder, q->nof_prb_psbch, true)) {
      return SRSLTE_ERROR;
    }

    q->scfdma_symbols = srslte_vec_malloc(sizeof(cf_t) * q->len_after_mod);
    if (!q->scfdma_symbols) {
      ERROR("Error allocating memmory\n");
      return SRSLTE_ERROR;
    }
    bzero(q->scfdma_symbols, sizeof(cf_t) * q->len_after_mod);

    // idft predecoding
    if (srslte_dft_precoding_init(&q->idft_precoder, q->nof_prb_psbch, false)) {
      return SRSLTE_ERROR;
    }

    ret = SRSLTE_SUCCESS;
  }

  return ret;
}

void srslte_psbch_encode(srslte_psbch_t* q, uint8_t* mib_sl)
{
  // ********************************************************************************************************
  // SL-BCH Processing
  // ********************************************************************************************************

  // CRC Attachment
  memcpy(q->a, mib_sl, sizeof(uint8_t) * SRSLTE_SL_BCH_PAYLOAD_LEN);
  srslte_crc_attach(&q->crc_mib, q->a, SRSLTE_SL_BCH_PAYLOAD_LEN);

  // Channel Coding
  srslte_convcoder_encode(&q->encoder, q->a, q->d, SRSLTE_SL_BCH_PAYLOADCRC_LEN);

  // Rate matching
  q->nof_symbols = 8;
  q->E           = q->nof_symbols * (SRSLTE_NRE * q->nof_prb_psbch) * 2;
  srslte_rm_conv_tx(q->d, SRSLTE_SL_BCH_ENCODED_LEN, q->e, q->E);

  // Interleaving
  for (int i = 0; i < q->E; i++) {
    q->codeword[i] = (uint8_t)q->e[q->interleaver_lut[i]];
  }

  // ********************************************************************************************************
  // PSBCH Processing
  // ********************************************************************************************************

  // Scrambling
  srslte_scrambling_b(&q->seq, q->codeword);

  // Modulation
  srslte_mod_modulate(&q->mod, q->codeword, q->symbols, q->E);

  // Layer Mapping - TS 36.211 SEC 9.6.3 - Single layer

  // DFT Precoding
  srslte_dft_precoding(&q->dft_precoder, q->symbols, q->scfdma_symbols, q->nof_prb_psbch, q->nof_symbols);

  // Precoding - TS 36.211 SEC 9.6.5 - Single antenna port
}

void srslte_psbch_put(srslte_psbch_t* q, cf_t* sf_buffer)
{
  uint32_t samplePos = 0;
  uint32_t k         = q->nof_prb * SRSLTE_NRE / 2 - 36;

  // Mapping to physical resources
  for (uint32_t i = 0; i < SRSLTE_CP_NORM_SF_NSYMB; i++) {
    if (i == 1 || i == 2 || i == 3 || i == 10 || i == 11 || i == 12) {
      continue;
    }
    memcpy(&sf_buffer[k + i * q->nof_prb * SRSLTE_NRE],
           &q->scfdma_symbols[samplePos],
           (SRSLTE_NRE * q->nof_prb_psbch) * sizeof(cf_t));
    samplePos += (SRSLTE_NRE * q->nof_prb_psbch);
  }
}

void srslte_psbch_get(srslte_psbch_t* q, cf_t* sf_buffer)
{
  uint32_t samplePos = 0;
  uint32_t k         = q->nof_prb * SRSLTE_NRE / 2 - 36;

  // Get PSBCH RE's
  for (uint32_t i = 0; i < SRSLTE_CP_NORM_SF_NSYMB; i++) {
    if (i == 1 || i == 2 || i == 3 || i == 10 || i == 11 || i == 12) {
      continue;
    }
    memcpy(&q->scfdma_symbols[samplePos],
           &sf_buffer[k + i * q->nof_prb * SRSLTE_NRE],
           (SRSLTE_NRE * q->nof_prb_psbch) * sizeof(cf_t));
    samplePos += (SRSLTE_NRE * q->nof_prb_psbch);
  }
}

int srslte_psbch_decode(srslte_psbch_t* q, uint8_t* mib_sl)
{
  // ********************************************************************************************************
  // PSBCH Processing
  // ********************************************************************************************************

  // Precoding - TS 36.211 SEC 9.6.5 - Single antenna port Skipped

  // IDFT Precoding
  q->len_after_mod = q->E / q->Qm;
  srslte_dft_precoding(&q->idft_precoder, q->scfdma_symbols, q->symbols, q->nof_prb_psbch, q->nof_symbols);

  // Layer Mapping - TS 36.211 SEC 9.6.3 - Single layer Skipped

  // Demodulation
  srslte_demod_soft_demodulate(SRSLTE_MOD_QPSK, q->symbols, q->llr, q->len_after_mod);

  // Descramble
  for (int i = 0; i < q->E; i++) {
    q->codeword[i] = q->llr[i] > 0 ? (uint8_t)1 : (uint8_t)0;
  }
  srslte_scrambling_b(&q->seq, q->codeword);

  // TS 36.211 Section 9.3.2: The last SC-FDMA symbol in a sidelink subframe serves as a guard period and shall not be
  // used for sidelink transmission.
  bzero(&q->codeword[1008], sizeof(uint8_t) * 144);

  // ********************************************************************************************************
  // SL-BCH Processing
  // ********************************************************************************************************

  // Deinterleaving
  for (int i = 0; i < q->E; i++) {
    q->e_16[q->interleaver_lut[i]] = q->codeword[i];
  }

  // Unrate Matching
  float input_rm_rate_matching[q->E];
  float output_rm_rate_matching[SRSLTE_SL_BCH_ENCODED_LEN];
  for (int i = 0; i < q->E; i++) {
    input_rm_rate_matching[i] = (float)q->e_16[i];
  }
  srslte_rm_conv_rx(input_rm_rate_matching, q->E, output_rm_rate_matching, SRSLTE_SL_BCH_ENCODED_LEN);

  // Channel Decoding
  uint16_t decoder_input[SRSLTE_SL_BCH_ENCODED_LEN];
  srslte_vec_quant_fus(output_rm_rate_matching, decoder_input, 8192, 32767.5, 65535, SRSLTE_SL_BCH_ENCODED_LEN);
  srslte_viterbi_decode_us(&q->dec, decoder_input, q->a, SRSLTE_SL_BCH_PAYLOADCRC_LEN);

  // RM CRC
  memcpy(q->crc_temp, &q->a[SRSLTE_SL_BCH_PAYLOAD_LEN], sizeof(uint8_t) * SRSLTE_SL_BCH_CRC_LEN);
  srslte_crc_attach(&q->crc_mib, q->a, SRSLTE_SL_BCH_PAYLOAD_LEN);
  if (srslte_bit_diff(q->crc_temp, &q->a[SRSLTE_SL_BCH_PAYLOAD_LEN], SRSLTE_SL_BCH_CRC_LEN) != 0) {
    printf("Error in mib_sl crc check\n");
    return SRSLTE_ERROR;
  }

  memcpy(mib_sl, q->a, sizeof(uint8_t) * SRSLTE_SL_BCH_PAYLOAD_LEN);
  return SRSLTE_SUCCESS;
}

void srslte_psbch_free(srslte_psbch_t* q)
{
  if (q) {

    srslte_dft_precoding_free(&q->dft_precoder);
    srslte_dft_precoding_free(&q->idft_precoder);
    srslte_viterbi_free(&q->dec);
    srslte_sequence_free(&q->seq);
    srslte_modem_table_free(&q->mod);

    if (q->a) {
      free(q->a);
    }
    if (q->d) {
      free(q->d);
    }
    if (q->crc_temp) {
      free(q->crc_temp);
    }
    if (q->e) {
      free(q->e);
    }
    if (q->e_16) {
      free(q->e_16);
    }
    if (q->interleaver_lut) {
      free(q->interleaver_lut);
    }
    if (q->codeword) {
      free(q->codeword);
    }
    if (q->symbols) {
      free(q->symbols);
    }
    if (q->llr) {
      free(q->llr);
    }
    if (q->scfdma_symbols) {
      free(q->scfdma_symbols);
    }

    bzero(q, sizeof(srslte_psbch_t));
  }
}