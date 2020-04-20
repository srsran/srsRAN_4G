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

#include <string.h>

#include "srslte/phy/fec/rm_turbo.h"
#include "srslte/phy/modem/demod_soft.h"
#include "srslte/phy/phch/pssch.h"
#include "srslte/phy/phch/ra.h"
#include "srslte/phy/phch/sch.h"
#include "srslte/phy/utils/bit.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"

int srslte_pssch_init(srslte_pssch_t* q, srslte_cell_sl_t cell, srslte_sl_comm_resource_pool_t sl_comm_resource_pool)
{
  if (q == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  q->cell                  = cell;
  q->sl_comm_resource_pool = sl_comm_resource_pool;

  if (cell.tm == SRSLTE_SIDELINK_TM1 || cell.tm == SRSLTE_SIDELINK_TM2) {
    if (cell.cp == SRSLTE_CP_NORM) {
      q->nof_data_symbols = SRSLTE_PSSCH_TM12_NUM_DATA_SYMBOLS;
    } else {
      q->nof_data_symbols = SRSLTE_PSSCH_TM12_NUM_DATA_SYMBOLS_CP_EXT;
    }
  } else if (cell.tm == SRSLTE_SIDELINK_TM3 || cell.tm == SRSLTE_SIDELINK_TM4) {
    if (cell.cp == SRSLTE_CP_NORM) {
      q->nof_data_symbols = SRSLTE_PSSCH_TM34_NUM_DATA_SYMBOLS;
    } else {
      ERROR("Invalid CP for PSSCH, SL TM 3/4\n");
      return SRSLTE_ERROR_INVALID_INPUTS;
    }
  } else {
    ERROR("Invalid SL TM\n");
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  // Transport Block
  q->b = srslte_vec_u8_malloc(SRSLTE_SL_SCH_MAX_TB_LEN + SRSLTE_PSSCH_CRC_LEN);
  if (!q->b) {
    ERROR("Error allocating memory\n");
    return SRSLTE_ERROR;
  }

  // Transport Block CRC
  if (srslte_crc_init(&q->tb_crc, SRSLTE_LTE_CRC24A, SRSLTE_PSSCH_CRC_LEN)) {
    ERROR("Error Transport Block CRC init\n");
    return SRSLTE_ERROR;
  }
  q->tb_crc_temp = srslte_vec_u8_malloc(SRSLTE_PSSCH_CRC_LEN);
  if (!q->tb_crc_temp) {
    ERROR("Error allocating memory\n");
    return SRSLTE_ERROR;
  }

  // Code Block Segmentation
  q->c_r = srslte_vec_u8_malloc(SRSLTE_TCOD_MAX_LEN_CB);
  if (!q->c_r) {
    ERROR("Error allocating memory\n");
    return SRSLTE_ERROR;
  }
  q->f_16 = srslte_vec_i16_malloc(SRSLTE_MAX_CODEWORD_LEN);
  if (!q->f_16) {
    ERROR("Error allocating memory\n");
    return SRSLTE_ERROR;
  }
  q->c_r_bytes = srslte_vec_u8_malloc(SRSLTE_TCOD_MAX_LEN_CB / 8);
  if (!q->c_r_bytes) {
    ERROR("Error allocating memory\n");
    return SRSLTE_ERROR;
  }

  // Code Block CRC
  if (srslte_crc_init(&q->cb_crc, SRSLTE_LTE_CRC24B, SRSLTE_PSSCH_CRC_LEN)) {
    ERROR("Error Code Block CRC init\n");
    return SRSLTE_ERROR;
  }
  q->cb_crc_temp = srslte_vec_u8_malloc(SRSLTE_PSSCH_CRC_LEN);
  if (!q->cb_crc_temp) {
    ERROR("Error allocating memory\n");
    return SRSLTE_ERROR;
  }

  // Channel Coding
  srslte_tcod_init(&q->tcod, SRSLTE_TCOD_MAX_LEN_CB);
  q->d_r = srslte_vec_u8_malloc(SRSLTE_PSSCH_MAX_CODED_BITS);
  if (!q->d_r) {
    ERROR("Error allocating memory\n");
    return SRSLTE_ERROR;
  }
  srslte_tdec_init(&q->tdec, SRSLTE_TCOD_MAX_LEN_CB);
  srslte_tdec_force_not_sb(&q->tdec);
  q->d_r_16 = srslte_vec_i16_malloc(SRSLTE_PSSCH_MAX_CODED_BITS);
  if (!q->d_r_16) {
    ERROR("Error allocating memory\n");
    return SRSLTE_ERROR;
  }

  // Rate Matching
  q->e_r = srslte_vec_u8_malloc(SRSLTE_MAX_CODEWORD_LEN);
  if (!q->e_r) {
    ERROR("Error allocating memory\n");
    return SRSLTE_ERROR;
  }
  q->buff_b = srslte_vec_u8_malloc(SRSLTE_PSSCH_MAX_CODED_BITS);
  if (!q->buff_b) {
    ERROR("Error allocating memory\n");
    return SRSLTE_ERROR;
  }
  srslte_vec_u8_zero(q->buff_b, SRSLTE_PSSCH_MAX_CODED_BITS);
  q->e_r_16 = srslte_vec_i16_malloc(SRSLTE_MAX_CODEWORD_LEN);
  if (!q->e_r_16) {
    ERROR("Error allocating memory\n");
    return SRSLTE_ERROR;
  }
  srslte_rm_turbo_gentables();

  // Code Block Concatenation
  q->f = srslte_vec_u8_malloc(SRSLTE_MAX_CODEWORD_LEN);
  if (!q->f) {
    ERROR("Error allocating memory\n");
    return SRSLTE_ERROR;
  }

  // Interleaving
  q->f_bytes = srslte_vec_u8_malloc(SRSLTE_MAX_CODEWORD_LEN / 8);
  if (!q->f_bytes) {
    ERROR("Error allocating memory\n");
    return SRSLTE_ERROR;
  }
  q->interleaver_lut = srslte_vec_u32_malloc(SRSLTE_MAX_CODEWORD_LEN);
  if (!q->interleaver_lut) {
    ERROR("Error allocating memory\n");
    return SRSLTE_ERROR;
  }
  srslte_vec_u32_zero(q->interleaver_lut, SRSLTE_MAX_CODEWORD_LEN);

  // Scrambling
  q->codeword = srslte_vec_u8_malloc(SRSLTE_MAX_CODEWORD_LEN);
  if (!q->codeword) {
    ERROR("Error allocating memory\n");
    return SRSLTE_ERROR;
  }
  q->codeword_bytes = srslte_vec_u8_malloc(SRSLTE_MAX_CODEWORD_LEN / 8);
  if (!q->codeword_bytes) {
    ERROR("Error allocating memory\n");
    return SRSLTE_ERROR;
  }

  // Modulation
  q->symbols = srslte_vec_cf_malloc(SRSLTE_MAX_CODEWORD_LEN);
  if (!q->symbols) {
    ERROR("Error allocating memory\n");
    return SRSLTE_ERROR;
  }
  srslte_vec_cf_zero(q->symbols, SRSLTE_MAX_CODEWORD_LEN);

  q->bits_after_demod = srslte_vec_u8_malloc(SRSLTE_MAX_CODEWORD_LEN);
  if (!q->bits_after_demod) {
    ERROR("Error allocating memory\n");
    return SRSLTE_ERROR;
  }
  q->bytes_after_demod = srslte_vec_u8_malloc(SRSLTE_MAX_CODEWORD_LEN / 8);
  if (!q->bytes_after_demod) {
    ERROR("Error allocating memory\n");
    return SRSLTE_ERROR;
  }

  for (int i = 0; i < SRSLTE_MOD_NITEMS; i++) {
    if (srslte_modem_table_lte(&q->mod[i], (srslte_mod_t)i)) {
      ERROR("Error initiating modem tables\n");
      return SRSLTE_ERROR;
    }
  }

  // Demodulation
  q->llr = srslte_vec_i16_malloc(SRSLTE_MAX_CODEWORD_LEN);
  if (!q->llr) {
    ERROR("Error allocating memory\n");
    return SRSLTE_ERROR;
  }

  // Transform Precoding
  q->scfdma_symbols = srslte_vec_cf_malloc(q->nof_data_symbols * SRSLTE_NRE * SRSLTE_MAX_PRB);
  if (!q->scfdma_symbols) {
    ERROR("Error allocating memory\n");
    return SRSLTE_ERROR;
  }
  if (srslte_dft_precoding_init(&q->dft_precoder, SRSLTE_MAX_PRB, true)) {
    ERROR("Error DFT precoder init\n");
    return SRSLTE_ERROR;
  }
  if (srslte_dft_precoding_init(&q->idft_precoder, SRSLTE_MAX_PRB, false)) {
    ERROR("Error in DFT precoder init\n");
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

int srslte_pssch_set_cfg(srslte_pssch_t* q, srslte_pssch_cfg_t pssch_cfg)
{
  if (q == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  q->pssch_cfg = pssch_cfg;

  q->mod_idx = srslte_ra_ul_mod_from_mcs(pssch_cfg.mcs_idx);
  q->Qm      = srslte_mod_bits_x_symbol(q->mod_idx);
  q->sl_sch_tb_len =
      srslte_ra_tbs_from_idx(srslte_ra_tbs_idx_from_mcs(pssch_cfg.mcs_idx, false, true), pssch_cfg.nof_prb);

  if (q->cell.tm == SRSLTE_SIDELINK_TM1 || q->cell.tm == SRSLTE_SIDELINK_TM2) {
    q->nof_data_symbols = SRSLTE_PSSCH_TM12_NUM_DATA_SYMBOLS;

    if (q->cell.cp == SRSLTE_CP_EXT) {
      q->nof_data_symbols = SRSLTE_PSSCH_TM12_NUM_DATA_SYMBOLS_CP_EXT;
    }
  } else if (q->cell.tm == SRSLTE_SIDELINK_TM3 || q->cell.tm == SRSLTE_SIDELINK_TM4) {
    q->nof_data_symbols = SRSLTE_PSSCH_TM34_NUM_DATA_SYMBOLS;
  } else {
    return SRSLTE_ERROR;
  }

  q->nof_tx_symbols = q->nof_data_symbols - 1; // Last OFDM symbol is used in channel processing but not transmitted
  q->nof_tx_re      = q->nof_tx_symbols * SRSLTE_NRE * pssch_cfg.nof_prb;
  q->nof_data_re    = q->nof_data_symbols * SRSLTE_NRE * pssch_cfg.nof_prb;

  q->E                  = q->nof_data_re * q->Qm;
  q->G                  = srslte_mod_bits_x_symbol(srslte_ra_ul_mod_from_mcs(pssch_cfg.mcs_idx)) * q->nof_data_re;
  q->scfdma_symbols_len = q->G / q->Qm;

  return SRSLTE_SUCCESS;
}

int srslte_pssch_encode(srslte_pssch_t* q, uint8_t* input, uint32_t input_len, cf_t* sf_buffer)
{
  if (!input || input_len > q->sl_sch_tb_len) {
    ERROR("Can't encode PSSCH, input too long (%d > %d)\n", input_len, q->sl_sch_tb_len);
    return SRSLTE_ERROR;
  }

  srslte_cbsegm(&q->cb_segm, q->sl_sch_tb_len);
  uint32_t L = SRSLTE_PSSCH_CRC_LEN;
  if (q->cb_segm.C == 1) {
    L = 0;
  }

  uint32_t K_r   = 0;
  uint32_t E_r   = 0;
  uint32_t s     = 0;
  q->G           = 0;
  uint32_t Gp    = q->E / q->Qm;
  uint32_t gamma = Gp % q->cb_segm.C;

  // Copy into codeword buffer
  memcpy(q->b, input, sizeof(uint8_t) * input_len);

  // Transport block CRC attachment
  srslte_crc_attach(&q->tb_crc, q->b, q->sl_sch_tb_len);

  for (int r = 0; r < q->cb_segm.C; r++) {

    // Code block segmentation
    if (r < q->cb_segm.C2) {
      K_r = q->cb_segm.K2;
    } else {
      K_r = q->cb_segm.K1;
    }

    if (r <= (q->cb_segm.C - gamma - 1)) {
      E_r = q->Qm * (Gp / q->cb_segm.C);
    } else {
      E_r = q->Qm * ((uint32_t)ceilf((float)Gp / q->cb_segm.C));
    }

    if (r == 0) {
      memset(q->c_r, SRSLTE_TX_NULL, sizeof(uint8_t) * q->cb_segm.F);
      memcpy(&q->c_r[q->cb_segm.F], q->b, sizeof(uint8_t) * (K_r - L));
      s += (K_r - L - q->cb_segm.F);
    } else {
      memcpy(q->c_r, &q->b[s], sizeof(uint8_t) * (K_r - L));
      s += (K_r - L);
    }

    // Code block CRC attachment
    if (q->cb_segm.C > 1) {
      if (r == 0) {
        srslte_vec_u8_zero(q->c_r, q->cb_segm.F);
      }
      srslte_crc_attach(&q->cb_crc, q->c_r, (int)(K_r - L));
      if (r == 0) {
        memset(q->c_r, SRSLTE_TX_NULL, sizeof(uint8_t) * q->cb_segm.F);
      }
    }

    // Channel coding
    srslte_tcod_encode(&q->tcod, q->c_r, q->d_r, K_r);

    // Rate matching
    srslte_vec_u8_zero(q->buff_b, SRSLTE_PSSCH_MAX_CODED_BITS);
    srslte_rm_turbo_tx(q->buff_b, SRSLTE_PSSCH_MAX_CODED_BITS, q->d_r, 3 * K_r + SRSLTE_TCOD_TOTALTAIL, q->e_r, E_r, 0);

    if (q->pssch_cfg.rv_idx > 0) {
      srslte_rm_turbo_tx(q->buff_b,
                         SRSLTE_PSSCH_MAX_CODED_BITS,
                         q->d_r,
                         3 * K_r + SRSLTE_TCOD_TOTALTAIL,
                         q->e_r,
                         E_r,
                         srslte_pssch_rv[q->pssch_cfg.rv_idx]);
    }

    // Code block concatenation
    memcpy(&q->f[q->G], q->e_r, sizeof(uint8_t) * E_r);
    q->G += E_r;
  }

  // Interleaving
  srslte_bit_pack_vector(q->f, q->f_bytes, q->G);
  srslte_sl_ulsch_interleave(q->f_bytes, q->Qm, q->G / q->Qm, q->nof_data_symbols, q->codeword_bytes);
  srslte_bit_unpack_vector(q->codeword_bytes, q->codeword, q->G);

  // Scrambling follows 3GPP TS 36.211 version 15.6.0 Release 15 Sec. 9.3.1
  srslte_sequence_LTE_pr(
      &q->scrambling_seq, q->G, q->pssch_cfg.N_x_id * 16384 + (q->pssch_cfg.sf_idx % 10) * 512 + 510);
  srslte_scrambling_b(&q->scrambling_seq, q->codeword);

  // Modulation
  srslte_mod_modulate(&q->mod[q->mod_idx], q->codeword, q->symbols, q->G);

  // Layer Mapping
  // Voided: Single layer
  // 3GPP TS 36.211 version 15.6.0 Release 15 Sec. 9.3.3

  // Transform Precoding
  srslte_dft_precoding(&q->dft_precoder, q->symbols, q->scfdma_symbols, q->pssch_cfg.nof_prb, q->nof_data_symbols);

  // Precoding
  // Voided: Single antenna port
  // 3GPP TS 36.211 version 15.6.0 Release 15 Sec. 9.3.5

  // RE mapping
  if (q->nof_tx_re != srslte_pssch_put(q, sf_buffer, q->scfdma_symbols)) {
    ERROR("There was an error mapping the PSSCH symbols\n");
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

int srslte_pssch_decode(srslte_pssch_t* q, cf_t* equalized_sf_syms, uint8_t* output, uint32_t output_len)
{
  if (output_len < q->sl_sch_tb_len) {
    ERROR("Can't decode PSSCH, provided buffer too small (%d < %d)\n", output_len, q->sl_sch_tb_len);
    return SRSLTE_ERROR;
  }

  // RE extraction
  if (q->nof_tx_re != srslte_pssch_get(q, equalized_sf_syms, q->scfdma_symbols)) {
    ERROR("There was an error getting the PSSCH symbols\n");
    return SRSLTE_ERROR;
  }

  // Precoding
  // Voided: Single antenna port
  // 3GPP TS 36.211 version 15.6.0 Release 15 Sec. 9.3.5

  // Transform Predecoding
  if (srslte_dft_precoding(
          &q->idft_precoder, q->scfdma_symbols, q->symbols, q->pssch_cfg.nof_prb, q->nof_data_symbols) !=
      SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  // Layer Mapping
  // Voided: Single layer
  // 3GPP TS 36.211 version 15.6.0 Release 15 Sec. 9.3.3

  // Demodulation
  srslte_demod_soft_demodulate_s(q->Qm / 2, q->symbols, q->llr, q->G / q->Qm);

  // Descramble follows 3GPP TS 36.211 version 15.6.0 Release 15 Sec. 9.3.1
  srslte_sequence_LTE_pr(
      &q->scrambling_seq, q->G, q->pssch_cfg.N_x_id * 16384 + (q->pssch_cfg.sf_idx % 10) * 512 + 510);
  srslte_scrambling_s(&q->scrambling_seq, q->llr);

  srslte_cbsegm(&q->cb_segm, q->sl_sch_tb_len);
  uint32_t L = SRSLTE_PSSCH_CRC_LEN;
  if (q->cb_segm.C == 1) {
    L = 0;
  }

  uint32_t B     = 0;
  uint32_t K_r   = 0;
  uint32_t E_r   = 0;
  uint32_t s     = 0;
  uint32_t Gp    = q->E / q->Qm;
  uint32_t gamma = Gp % q->cb_segm.C;

  // Deinterleaving
  srslte_sl_ulsch_deinterleave(q->llr, q->Qm, q->G / q->Qm, q->nof_data_symbols, q->f_16, q->interleaver_lut);

  for (int r = 0; r < q->cb_segm.C; r++) {
    // Code block segmentation
    if (r < q->cb_segm.C2) {
      K_r = q->cb_segm.K2;
    } else {
      K_r = q->cb_segm.K1;
    }

    if (r <= (q->cb_segm.C - gamma - 1)) {
      E_r = q->Qm * (Gp / q->cb_segm.C);
    } else {
      E_r = q->Qm * ((uint32_t)ceilf((float)Gp / q->cb_segm.C));
    }

    memcpy(q->e_r_16, &q->f_16[s], sizeof(int16_t) * E_r);
    s += E_r;

    // Rate matching
    uint32_t cb_len_idx = r < q->cb_segm.C1 ? q->cb_segm.K1_idx : q->cb_segm.K2_idx;
    srslte_vec_i16_zero(q->d_r_16, SRSLTE_PSSCH_MAX_CODED_BITS);
    srslte_rm_turbo_rx_lut_(q->e_r_16, q->d_r_16, E_r, cb_len_idx, srslte_pssch_rv[q->pssch_cfg.rv_idx], false);

    // Channel decoding
    srslte_tdec_new_cb(&q->tdec, K_r);
    srslte_tdec_run_all(&q->tdec, q->d_r_16, q->c_r_bytes, 3, K_r);
    srslte_bit_unpack_vector(q->c_r_bytes, q->c_r, K_r);

    if (q->cb_segm.C > 1) {

      // Copy received crc to temp
      memcpy(q->cb_crc_temp, &q->c_r[(K_r - L)], sizeof(uint8_t) * L);

      // Re-attach crc
      srslte_crc_attach(&q->cb_crc, q->c_r, (int)(K_r - L));

      // CRC check
      if (srslte_bit_diff(q->cb_crc_temp, &q->c_r[(K_r - L)], L) != 0) {
        return SRSLTE_ERROR;
      }
    }

    // Code Block Concatenation, dettach CRC and remove filler bits
    if (r == 0) {
      memcpy(q->b, &q->c_r[q->cb_segm.F], sizeof(uint8_t) * (K_r - L - q->cb_segm.F));
      B += (K_r - L - q->cb_segm.F);
    } else {
      memcpy(&q->b[B], q->c_r, sizeof(uint8_t) * (K_r - L));
      B += (K_r - L);
    }
  }

  // Copy received crc to temp
  memcpy(q->tb_crc_temp, &q->b[B - SRSLTE_PSSCH_CRC_LEN], sizeof(uint8_t) * SRSLTE_PSSCH_CRC_LEN);

  // Re-attach crc
  srslte_crc_attach(&q->tb_crc, q->b, (int)(B - SRSLTE_PSSCH_CRC_LEN));

  // CRC check
  if (srslte_bit_diff(q->tb_crc_temp, &q->b[(B - SRSLTE_PSSCH_CRC_LEN)], SRSLTE_PSSCH_CRC_LEN) != 0) {
    return SRSLTE_ERROR;
  }

  // Remove CRC and copy to output buffer
  memcpy(output, q->b, sizeof(uint8_t) * q->sl_sch_tb_len);

  return SRSLTE_SUCCESS;
}

int srslte_pssch_put(srslte_pssch_t* q, cf_t* sf_buffer, cf_t* symbols)
{
  uint32_t sample_pos = 0;
  uint32_t k          = q->pssch_cfg.prb_start_idx * SRSLTE_NRE;
  for (int i = 0; i < srslte_sl_get_num_symbols(q->cell.tm, q->cell.cp); i++) {
    if (srslte_pssch_is_symbol(SRSLTE_SIDELINK_DATA_SYMBOL, q->cell.tm, i, q->cell.cp)) {

      if (q->cell.tm == SRSLTE_SIDELINK_TM1 || q->cell.tm == SRSLTE_SIDELINK_TM2) {
        if (q->pssch_cfg.nof_prb <= q->sl_comm_resource_pool.prb_num) {
          k = q->pssch_cfg.prb_start_idx * SRSLTE_NRE;
          memcpy(&sf_buffer[k + i * q->cell.nof_prb * SRSLTE_NRE],
                 &symbols[sample_pos],
                 sizeof(cf_t) * (SRSLTE_NRE * q->pssch_cfg.nof_prb));
          sample_pos += (SRSLTE_NRE * q->pssch_cfg.nof_prb);
        } else {
          // First band
          k = q->pssch_cfg.prb_start_idx * SRSLTE_NRE;
          memcpy(&sf_buffer[k + i * q->cell.nof_prb * SRSLTE_NRE],
                 &symbols[sample_pos],
                 sizeof(cf_t) * (SRSLTE_NRE * q->sl_comm_resource_pool.prb_num));
          sample_pos += (SRSLTE_NRE * q->sl_comm_resource_pool.prb_num);

          // Second band
          if ((q->sl_comm_resource_pool.prb_num * 2) >
              (q->sl_comm_resource_pool.prb_end - q->sl_comm_resource_pool.prb_start + 1)) {
            k = (q->sl_comm_resource_pool.prb_end + 1 - q->sl_comm_resource_pool.prb_num + 1) * SRSLTE_NRE;
          } else {
            k = (q->sl_comm_resource_pool.prb_end + 1 - q->sl_comm_resource_pool.prb_num) * SRSLTE_NRE;
          }
          memcpy(&sf_buffer[k + i * q->cell.nof_prb * SRSLTE_NRE],
                 &symbols[sample_pos],
                 sizeof(cf_t) * (SRSLTE_NRE * (q->pssch_cfg.nof_prb - q->sl_comm_resource_pool.prb_num)));
          sample_pos += (SRSLTE_NRE * (q->pssch_cfg.nof_prb - q->sl_comm_resource_pool.prb_num));
        }
      } else if (q->cell.tm == SRSLTE_SIDELINK_TM3 || q->cell.tm == SRSLTE_SIDELINK_TM4) {
        memcpy(&sf_buffer[k + i * q->cell.nof_prb * SRSLTE_NRE],
               &symbols[sample_pos],
               sizeof(cf_t) * (SRSLTE_NRE * q->pssch_cfg.nof_prb));
        sample_pos += (SRSLTE_NRE * q->pssch_cfg.nof_prb);
      }
    }
  }

  return sample_pos;
}

int srslte_pssch_get(srslte_pssch_t* q, cf_t* sf_buffer, cf_t* symbols)
{
  uint32_t sample_pos = 0;
  uint32_t k          = q->pssch_cfg.prb_start_idx * SRSLTE_NRE;
  for (int i = 0; i < srslte_sl_get_num_symbols(q->cell.tm, q->cell.cp); i++) {
    if (srslte_pssch_is_symbol(SRSLTE_SIDELINK_DATA_SYMBOL, q->cell.tm, i, q->cell.cp)) {
      if (q->cell.tm == SRSLTE_SIDELINK_TM1 || q->cell.tm == SRSLTE_SIDELINK_TM2) {
        if (q->pssch_cfg.nof_prb <= q->sl_comm_resource_pool.prb_num) {
          k = q->pssch_cfg.prb_start_idx * SRSLTE_NRE;
          memcpy(&symbols[sample_pos],
                 &sf_buffer[k + i * q->cell.nof_prb * SRSLTE_NRE],
                 sizeof(cf_t) * (SRSLTE_NRE * q->pssch_cfg.nof_prb));
          sample_pos += (SRSLTE_NRE * q->pssch_cfg.nof_prb);
        } else {
          // First band
          k = q->pssch_cfg.prb_start_idx * SRSLTE_NRE;
          memcpy(&symbols[sample_pos],
                 &sf_buffer[k + i * q->cell.nof_prb * SRSLTE_NRE],
                 sizeof(cf_t) * (SRSLTE_NRE * q->sl_comm_resource_pool.prb_num));
          sample_pos += (SRSLTE_NRE * q->sl_comm_resource_pool.prb_num);

          // Second band
          if ((q->sl_comm_resource_pool.prb_num * 2) >
              (q->sl_comm_resource_pool.prb_end - q->sl_comm_resource_pool.prb_start + 1)) {
            k = (q->sl_comm_resource_pool.prb_end + 1 - q->sl_comm_resource_pool.prb_num + 1) * SRSLTE_NRE;
          } else {
            k = (q->sl_comm_resource_pool.prb_end + 1 - q->sl_comm_resource_pool.prb_num) * SRSLTE_NRE;
          }
          memcpy(&symbols[sample_pos],
                 &sf_buffer[k + i * q->cell.nof_prb * SRSLTE_NRE],
                 sizeof(cf_t) * (SRSLTE_NRE * (q->pssch_cfg.nof_prb - q->sl_comm_resource_pool.prb_num)));
          sample_pos += (SRSLTE_NRE * (q->pssch_cfg.nof_prb - q->sl_comm_resource_pool.prb_num));
        }
      } else if (q->cell.tm == SRSLTE_SIDELINK_TM3 || q->cell.tm == SRSLTE_SIDELINK_TM4) {
        memcpy(&symbols[sample_pos],
               &sf_buffer[k + i * q->cell.nof_prb * SRSLTE_NRE],
               sizeof(cf_t) * (SRSLTE_NRE * q->pssch_cfg.nof_prb));
        sample_pos += (SRSLTE_NRE * q->pssch_cfg.nof_prb);
      }
    }
  }

  return sample_pos;
}

void srslte_pssch_free(srslte_pssch_t* q)
{
  if (q) {
    srslte_dft_precoding_free(&q->dft_precoder);
    srslte_dft_precoding_free(&q->idft_precoder);
    srslte_tcod_free(&q->tcod);
    srslte_tdec_free(&q->tdec);
    srslte_sequence_free(&q->scrambling_seq);
    srslte_rm_turbo_free_tables();

    for (int i = 0; i < SRSLTE_MOD_NITEMS; i++) {
      srslte_modem_table_free(&q->mod[i]);
    }

    if (q->b) {
      free(q->b);
    }
    if (q->tb_crc_temp) {
      free(q->tb_crc_temp);
    }
    if (q->cb_crc_temp) {
      free(q->cb_crc_temp);
    }
    if (q->c_r) {
      free(q->c_r);
    }
    if (q->c_r_bytes) {
      free(q->c_r_bytes);
    }
    if (q->d_r) {
      free(q->d_r);
    }
    if (q->d_r_16) {
      free(q->d_r_16);
    }
    if (q->e_r) {
      free(q->e_r);
    }
    if (q->e_r_16) {
      free(q->e_r_16);
    }
    if (q->buff_b) {
      free(q->buff_b);
    }
    if (q->f) {
      free(q->f);
    }
    if (q->f_16) {
      free(q->f_16);
    }
    if (q->f_bytes) {
      free(q->f_bytes);
    }
    if (q->codeword) {
      free(q->codeword);
    }
    if (q->codeword_bytes) {
      free(q->codeword_bytes);
    }
    if (q->llr) {
      free(q->llr);
    }
    if (q->interleaver_lut) {
      free(q->interleaver_lut);
    }
    if (q->symbols) {
      free(q->symbols);
    }
    if (q->scfdma_symbols) {
      free(q->scfdma_symbols);
    }
    if (q->bits_after_demod) {
      free(q->bits_after_demod);
    }
    if (q->bytes_after_demod) {
      free(q->bytes_after_demod);
    }

    bzero(q, sizeof(srslte_pssch_t));
  }
}