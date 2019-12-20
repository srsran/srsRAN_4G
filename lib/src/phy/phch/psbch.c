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

#include "srslte/phy/phch/psbch.h"
#include "srslte/phy/fec/rm_conv.h"
#include "srslte/phy/modem/mod.h"
#include "srslte/phy/utils/bit.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"
#include <stdlib.h>
#include <string.h>

#define HAVE_INTERLEAVING 1

#if HAVE_INTERLEAVING
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
#endif // HAVE_INTERLEAVING

int srslte_psbch_init(srslte_psbch_t* q, uint32_t nof_prb, uint32_t N_sl_id, srslte_sl_tm_t tm, srslte_cp_t cp)
{
  bzero(q, sizeof(srslte_psbch_t));

  q->N_sl_id = N_sl_id;
  q->tm      = tm;

  q->nof_prb = nof_prb;

  if (SRSLTE_CP_ISEXT(cp)) {
    ERROR("Extended CP is not supported yet.");
    return SRSLTE_ERROR;
  }
  q->cp = cp;

  // Calculate rate matching params
  if (q->tm <= SRSLTE_SIDELINK_TM2) {
    q->nof_data_symbols = SRSLTE_PSBCH_TM12_NUM_DATA_SYMBOLS;
    q->sl_bch_tb_len    = SRSLTE_MIB_SL_LEN;
  } else {
    q->nof_data_symbols = SRSLTE_PSBCH_TM34_NUM_DATA_SYMBOLS;
    q->sl_bch_tb_len    = SRSLTE_MIB_SL_V2X_LEN;
  }
  q->nof_data_re        = q->nof_data_symbols * (SRSLTE_NRE * SRSLTE_PSBCH_NOF_PRB);
  q->sl_bch_tb_crc_len  = q->sl_bch_tb_len + SRSLTE_SL_BCH_CRC_LEN;
  q->sl_bch_encoded_len = 3 * q->sl_bch_tb_crc_len;

  q->c = srslte_vec_malloc(sizeof(uint8_t) * q->sl_bch_tb_crc_len);
  if (!q->c) {
    ERROR("Error allocating memory\n");
    return SRSLTE_ERROR;
  }

  q->d = srslte_vec_malloc(sizeof(uint8_t) * q->sl_bch_encoded_len);
  if (!q->d) {
    ERROR("Error allocating memory\n");
    return SRSLTE_ERROR;
  }

  q->d_float = srslte_vec_malloc(sizeof(float) * q->sl_bch_encoded_len);
  if (!q->d_float) {
    ERROR("Error allocating memory\n");
    return SRSLTE_ERROR;
  }

  // CRC
  if (srslte_crc_init(&q->crc_mib_sl, SRSLTE_LTE_CRC16, SRSLTE_SL_BCH_CRC_LEN)) {
    ERROR("Error crc init");
    return SRSLTE_ERROR;
  }

  q->crc_temp = srslte_vec_malloc(sizeof(uint8_t) * SRSLTE_SL_BCH_CRC_LEN);
  if (!q->crc_temp) {
    ERROR("Error allocating memory\n");
    return SRSLTE_ERROR;
  }

  // Channel coding
  q->encoder.K           = 7;
  q->encoder.R           = 3;
  q->encoder.tail_biting = true;
  int poly[3]            = {0x6D, 0x4F, 0x57};
  memcpy(q->encoder.poly, poly, 3 * sizeof(int));

  if (srslte_viterbi_init(&q->dec, SRSLTE_VITERBI_37, poly, q->sl_bch_tb_crc_len, true)) {
    return SRSLTE_ERROR;
  }

  // QPSK modulation
  // 3GPP TS 36.211 version 15.6.0 Release 15 Sec. 9.6 - Table 9.6.2-1: PSBCH modulation schemes
  q->Qm = srslte_mod_bits_x_symbol(SRSLTE_MOD_QPSK);
  q->E  = q->nof_data_re * q->Qm;

  q->e = srslte_vec_malloc(sizeof(uint8_t) * q->E);
  if (!q->e) {
    ERROR("Error allocating memory\n");
    return SRSLTE_ERROR;
  }

  q->e_float = srslte_vec_malloc(sizeof(float) * q->E);
  if (!q->e_float) {
    ERROR("Error allocating memory\n");
    return SRSLTE_ERROR;
  }

#if HAVE_INTERLEAVING
  // Interleaving
  q->interleaver_lut = srslte_vec_malloc(sizeof(uint32_t) * q->E);
  if (!q->interleaver_lut) {
    ERROR("Error allocating memory\n");
    return SRSLTE_ERROR;
  }
  slbch_interleave_gen(q->nof_data_re, q->nof_data_symbols, q->Qm, q->interleaver_lut);
#endif

  // Scrambling
  bzero(&q->seq, sizeof(srslte_sequence_t));
  if (srslte_sequence_LTE_pr(&q->seq, q->E, N_sl_id) != SRSLTE_SUCCESS) {
    ERROR("Error srslte_sequence_LTE_pr\n");
    return SRSLTE_ERROR;
  }

  q->codeword = srslte_vec_malloc(sizeof(uint8_t) * q->E);
  if (!q->codeword) {
    ERROR("Error allocating memory\n");
    return SRSLTE_ERROR;
  }

  // Modulation QPSK
  if (srslte_modem_table_lte(&q->mod, SRSLTE_MOD_QPSK) != SRSLTE_SUCCESS) {
    ERROR("Error srslte_modem_table_lte\n");
    return SRSLTE_ERROR;
  }

  q->mod_symbols = srslte_vec_malloc(sizeof(cf_t) * q->nof_data_re);
  if (!q->mod_symbols) {
    ERROR("Error allocating memory\n");
    return SRSLTE_ERROR;
  }

  q->llr = srslte_vec_malloc(sizeof(float) * q->E);
  if (!q->llr) {
    ERROR("Error allocating memory\n");
    return SRSLTE_ERROR;
  }

  // Transform precoding
  q->precoding_scaling = 1.0;
  if (srslte_dft_precoding_init_tx(&q->dft_precoder, SRSLTE_PSBCH_NOF_PRB) != SRSLTE_SUCCESS) {
    ERROR("Error srslte_dft_precoding_init\n");
    return SRSLTE_ERROR;
  }

  q->scfdma_symbols = srslte_vec_malloc(sizeof(cf_t) * q->nof_data_re);
  if (!q->scfdma_symbols) {
    ERROR("Error allocating memory\n");
    return SRSLTE_ERROR;
  }

  if (srslte_dft_precoding_init_rx(&q->idft_precoder, SRSLTE_PSBCH_NOF_PRB) != SRSLTE_SUCCESS) {
    ERROR("Error srslte_idft_precoding_init\n");
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

int srslte_psbch_encode(srslte_psbch_t* q, uint8_t* input, uint32_t input_len, cf_t* sf_buffer)
{
  if (input == NULL || input_len > q->sl_bch_tb_len) {
    ERROR("Can't encode PSBCH, input too long (%d > %d)\n", input_len, q->sl_bch_tb_len);
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  // Copy into codeword buffer
  memcpy(q->c, input, sizeof(uint8_t) * input_len);

  // CRC Attachment
  srslte_crc_attach(&q->crc_mib_sl, q->c, input_len);

  // Channel Coding
  srslte_convcoder_encode(&q->encoder, q->c, q->d, q->sl_bch_tb_crc_len);

  // Rate matching
  srslte_rm_conv_tx(q->d, q->sl_bch_encoded_len, q->codeword, q->E);

#if HAVE_INTERLEAVING
  // PUSCH de-interleaving
  for (int i = 0; i < q->E; i++) {
    q->e[i] = q->codeword[q->interleaver_lut[i]];
  }
#endif

  // Scrambling
  srslte_scrambling_b(&q->seq, q->e);

  // Modulation
  srslte_mod_modulate(&q->mod, q->e, q->mod_symbols, q->E);

  // Layer Mapping
  // Void: Single layer
  // 3GPP TS 36.211 version 15.6.0 Release 15 Sec. 9.6.3

  // Transform precoding
  srslte_dft_precoding(&q->dft_precoder, q->mod_symbols, q->scfdma_symbols, SRSLTE_PSBCH_NOF_PRB, q->nof_data_symbols);

  // Precoding
  // Void: Single antenna port
  // 3GPP TS 36.211 version 15.6.0 Release 15 Sec. 9.6.5

  // RE mapping
  srslte_psbch_put(q, q->scfdma_symbols, sf_buffer);

  return SRSLTE_SUCCESS;
}

int srslte_psbch_decode(srslte_psbch_t* q, cf_t* equalized_sf_syms, uint8_t* output, uint32_t max_output_len)
{
  if (max_output_len < q->sl_bch_tb_len) {
    ERROR("Can't decode PSBCH, provided buffer too small (%d < %d)\n", max_output_len, q->sl_bch_tb_len);
    return SRSLTE_ERROR;
  }

  // RE extraction
  if (q->nof_data_re != srslte_psbch_get(q, equalized_sf_syms, q->scfdma_symbols)) {
    ERROR("There was an error getting the PSBCH symbols\n");
    return SRSLTE_ERROR;
  }

  // Precoding
  // Void: Single antenna port
  // 3GPP TS 36.211 version 15.6.0 Release 15 Sec. 9.6.5

  // Transform precoding
  srslte_dft_precoding(&q->idft_precoder, q->scfdma_symbols, q->mod_symbols, SRSLTE_PSBCH_NOF_PRB, q->nof_data_symbols);

  // Layer Mapping
  // Void: Single layer
  // 3GPP TS 36.211 version 15.6.0 Release 15 Sec. 9.6.3

  // Demodulation
  srslte_demod_soft_demodulate(SRSLTE_MOD_QPSK, q->mod_symbols, q->e_float, q->nof_data_re);

  // De-scramble
  srslte_scrambling_f(&q->seq, q->e_float);

#if HAVE_INTERLEAVING
  // Deinterleaving
  for (int i = 0; i < q->E; i++) {
    q->e_float[q->interleaver_lut[i]] = q->e_float[i];
  }
#endif

  // Rate match
  srslte_rm_conv_rx(q->e_float, q->E, q->d_float, q->sl_bch_encoded_len);

  // Channel decoding
  srslte_viterbi_decode_f(&q->dec, q->d_float, q->c, q->sl_bch_tb_crc_len);

  printf("after viterbi\n");
  srslte_vec_fprint_b(stdout, q->c, q->sl_bch_tb_crc_len);

  // Copy received crc to temp
  memcpy(q->crc_temp, &q->c[q->sl_bch_tb_len], sizeof(uint8_t) * SRSLTE_SL_BCH_CRC_LEN);

  // Re-attach crc
  srslte_crc_attach(&q->crc_mib_sl, q->c, q->sl_bch_tb_len);

  // CRC check
  if (srslte_bit_diff(q->crc_temp, &q->c[q->sl_bch_tb_len], SRSLTE_SL_BCH_CRC_LEN) != 0) {
    return SRSLTE_ERROR;
  }

  // Remove CRC and copy to output buffer
  memcpy(output, q->c, sizeof(uint8_t) * q->sl_bch_tb_len);

  return SRSLTE_SUCCESS;
}

int srslte_psbch_reset(srslte_psbch_t* q, uint32_t N_sl_id)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;
  if (q != NULL) {
    if (q->N_sl_id != N_sl_id) {
      q->N_sl_id = N_sl_id;

      // Regen scrambling sequence
      if (srslte_sequence_LTE_pr(&q->seq, q->E, N_sl_id) != SRSLTE_SUCCESS) {
        ERROR("Error srslte_sequence_LTE_pr\n");
        return SRSLTE_ERROR;
      }
    }
    ret = SRSLTE_SUCCESS;
  }
  return ret;
}

int srslte_psbch_put(srslte_psbch_t* q, cf_t* symbols, cf_t* sf_buffer)
{
  uint32_t sample_pos = 0;
  uint32_t k          = q->nof_prb * SRSLTE_NRE / 2 - 36;

  // Mapping to physical resources
  for (uint32_t i = 0; i < SRSLTE_CP_NORM_SF_NSYMB; i++) {
    if (srslte_psbch_is_symbol(SRSLTE_SIDELINK_DATA_SYMBOL, q->tm, i)) {
      memcpy(&sf_buffer[k + i * q->nof_prb * SRSLTE_NRE],
             &symbols[sample_pos],
             sizeof(cf_t) * (SRSLTE_NRE * SRSLTE_PSBCH_NOF_PRB));
      sample_pos += (SRSLTE_NRE * SRSLTE_PSBCH_NOF_PRB);
    }
  }

  return sample_pos;
}

int srslte_psbch_get(srslte_psbch_t* q, cf_t* sf_buffer, cf_t* symbols)
{
  uint32_t sample_pos = 0;
  uint32_t k          = q->nof_prb * SRSLTE_NRE / 2 - 36;

  // Get PSBCH REs
  for (uint32_t i = 0; i < SRSLTE_CP_NORM_SF_NSYMB; i++) {
    if (srslte_psbch_is_symbol(SRSLTE_SIDELINK_DATA_SYMBOL, q->tm, i)) {
      memcpy(&symbols[sample_pos],
             &sf_buffer[k + i * q->nof_prb * SRSLTE_NRE],
             sizeof(cf_t) * (SRSLTE_NRE * SRSLTE_PSBCH_NOF_PRB));
      sample_pos += (SRSLTE_NRE * SRSLTE_PSBCH_NOF_PRB);
    }
  }

  return sample_pos;
}

void srslte_psbch_free(srslte_psbch_t* q)
{
  if (q) {
    srslte_dft_precoding_free(&q->dft_precoder);
    srslte_dft_precoding_free(&q->idft_precoder);
    srslte_viterbi_free(&q->dec);
    srslte_sequence_free(&q->seq);
    srslte_modem_table_free(&q->mod);

    if (q->crc_temp) {
      free(q->crc_temp);
    }

    if (q->c) {
      free(q->c);
    }
    if (q->d) {
      free(q->d);
    }
    if (q->d_float) {
      free(q->d_float);
    }
    if (q->e) {
      free(q->e);
    }
    if (q->e_float) {
      free(q->e_float);
    }
    if (q->interleaver_lut) {
      free(q->interleaver_lut);
    }
    if (q->codeword) {
      free(q->codeword);
    }
    if (q->llr) {
      free(q->llr);
    }
    if (q->mod_symbols) {
      free(q->mod_symbols);
    }
    if (q->scfdma_symbols) {
      free(q->scfdma_symbols);
    }

    bzero(q, sizeof(srslte_psbch_t));
  }
}
