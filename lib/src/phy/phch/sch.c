/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#include "srsran/phy/utils/bit.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/vector.h"
#include "srsran/srsran.h"
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#define SRSRAN_PDSCH_MIN_TDEC_ITERS 2
#define SRSRAN_PDSCH_MAX_TDEC_ITERS 10

#ifdef LV_HAVE_SSE
#include <immintrin.h>
#endif /* LV_HAVE_SSE */

/* 36.213 Table 8.6.3-1: Mapping of HARQ-ACK offset values and the index signalled by higher layers */
static inline float get_beta_harq_offset(uint32_t idx)
{
  const float beta_harq_offset[16] = {
      2.0, 2.5, 3.125, 4.0, 5.0, 6.250, 8.0, 10.0, 12.625, 15.875, 20.0, 31.0, 50.0, 80.0, 126.0, -1.0};
  float ret = beta_harq_offset[0];

  if (idx < 15) {
    ret = beta_harq_offset[idx];
  } else {
    ERROR("Invalid input %d (min: %d, max: %d)", idx, 0, 14);
  }

  return ret;
}

/* 36.213 Table 8.6.3-2: Mapping of RI offset values and the index signalled by higher layers */
static inline float get_beta_ri_offset(uint32_t idx)
{
  const float beta_ri_offset[16] = {
      1.25, 1.625, 2.0, 2.5, 3.125, 4.0, 5.0, 6.25, 8.0, 10.0, 12.625, 15.875, 20.0, -1.0, -1.0, -1.0};
  float ret = beta_ri_offset[0];

  if (idx < 13) {
    ret = beta_ri_offset[idx];
  } else {
    ERROR("Invalid input %d (min: %d, max: %d)", idx, 0, 12);
  }

  return ret;
}

/* 36.213 Table 8.6.3-3: Mapping of CQI offset values and the index signalled by higher layers */
static inline float get_beta_cqi_offset(uint32_t idx)
{
  const float beta_cqi_offset[16] = {
      -1.0, -1.0, 1.125, 1.25, 1.375, 1.625, 1.750, 2.0, 2.25, 2.5, 2.875, 3.125, 3.5, 4.0, 5.0, 6.25};
  float ret = beta_cqi_offset[2];

  if (idx > 1 && idx < 16) {
    ret = beta_cqi_offset[idx];
  } else {
    ERROR("Invalid input %d (min: %d, max: %d)", idx, 2, 15);
  }

  return ret;
}

float srsran_sch_beta_cqi(uint32_t I_cqi)
{
  if (I_cqi < 16) {
    return get_beta_cqi_offset(I_cqi);
  } else {
    return 0;
  }
}

float srsran_sch_beta_ack(uint32_t I_harq)
{
  if (I_harq < 16) {
    return get_beta_harq_offset(I_harq);
  } else {
    return 0;
  }
}

uint32_t srsran_sch_find_Ioffset_ack(float beta)
{
  for (int i = 0; i < 16; i++) {
    if (get_beta_harq_offset(i) >= beta) {
      return i;
    }
  }
  return 0;
}

uint32_t srsran_sch_find_Ioffset_ri(float beta)
{
  for (int i = 0; i < 16; i++) {
    if (get_beta_ri_offset(i) >= beta) {
      return i;
    }
  }
  return 0;
}

uint32_t srsran_sch_find_Ioffset_cqi(float beta)
{
  for (int i = 0; i < 16; i++) {
    if (get_beta_cqi_offset(i) >= beta) {
      return i;
    }
  }
  return 0;
}

#define SCH_MAX_G_BITS (SRSRAN_MAX_PRB * 12 * 12 * 12)

int srsran_sch_init(srsran_sch_t* q)
{
  int ret = SRSRAN_ERROR_INVALID_INPUTS;
  if (q) {
    ret = SRSRAN_ERROR;
    bzero(q, sizeof(srsran_sch_t));

    if (srsran_crc_init(&q->crc_tb, SRSRAN_LTE_CRC24A, 24)) {
      ERROR("Error initiating CRC");
      goto clean;
    }
    if (srsran_crc_init(&q->crc_cb, SRSRAN_LTE_CRC24B, 24)) {
      ERROR("Error initiating CRC");
      goto clean;
    }

    if (srsran_tcod_init(&q->encoder, SRSRAN_TCOD_MAX_LEN_CB)) {
      ERROR("Error initiating Turbo Coder");
      goto clean;
    }
    if (srsran_tdec_init(&q->decoder, SRSRAN_TCOD_MAX_LEN_CB)) {
      ERROR("Error initiating Turbo Decoder");
      goto clean;
    }

    q->max_iterations = SRSRAN_PDSCH_MAX_TDEC_ITERS;

    srsran_rm_turbo_gentables();

    // Allocate int16 for reception (LLRs)
    q->cb_in = srsran_vec_u8_malloc((SRSRAN_TCOD_MAX_LEN_CB + 8) / 8);
    if (!q->cb_in) {
      goto clean;
    }

    q->parity_bits = srsran_vec_u8_malloc((3 * SRSRAN_TCOD_MAX_LEN_CB + 16) / 8);
    if (!q->parity_bits) {
      goto clean;
    }
    q->temp_g_bits = srsran_vec_u8_malloc(SCH_MAX_G_BITS);
    if (!q->temp_g_bits) {
      goto clean;
    }
    bzero(q->temp_g_bits, SRSRAN_MAX_PRB * 12 * 12 * 12);
    q->ul_interleaver = srsran_vec_u32_malloc(SCH_MAX_G_BITS);
    if (!q->ul_interleaver) {
      goto clean;
    }
    if (srsran_uci_cqi_init(&q->uci_cqi)) {
      goto clean;
    }

    ret = SRSRAN_SUCCESS;
  }
clean:
  if (ret == SRSRAN_ERROR) {
    srsran_sch_free(q);
  }
  return ret;
}

void srsran_sch_free(srsran_sch_t* q)
{
  srsran_rm_turbo_free_tables();

  if (q->cb_in) {
    free(q->cb_in);
  }
  if (q->parity_bits) {
    free(q->parity_bits);
  }
  if (q->temp_g_bits) {
    free(q->temp_g_bits);
  }
  if (q->ul_interleaver) {
    free(q->ul_interleaver);
  }
  srsran_tdec_free(&q->decoder);
  srsran_tcod_free(&q->encoder);
  srsran_uci_cqi_free(&q->uci_cqi);
  bzero(q, sizeof(srsran_sch_t));
}

void srsran_sch_set_max_noi(srsran_sch_t* q, uint32_t max_iterations)
{
  if (max_iterations == 0) {
    max_iterations = SRSRAN_PDSCH_MAX_TDEC_ITERS;
  }

  q->max_iterations = max_iterations;
}

float srsran_sch_last_noi(srsran_sch_t* q)
{
  return q->avg_iterations;
}

/* Encode a transport block according to 36.212 5.3.2
 *
 */
static int encode_tb_off(srsran_sch_t*           q,
                         srsran_softbuffer_tx_t* softbuffer,
                         srsran_cbsegm_t*        cb_segm,
                         uint32_t                Qm,
                         uint32_t                rv,
                         uint32_t                nof_e_bits,
                         uint8_t*                data,
                         uint8_t*                e_bits,
                         uint32_t                w_offset)
{
  uint32_t i;
  uint32_t cb_len = 0, rp = 0, wp = 0, rlen = 0, n_e = 0;
  int      ret = SRSRAN_ERROR_INVALID_INPUTS;

  if (q != NULL && e_bits != NULL && cb_segm != NULL && softbuffer != NULL) {
    if (cb_segm->F) {
      ERROR("Error filler bits are not supported. Use standard TBS");
      return SRSRAN_ERROR;
    }

    if (cb_segm->C > softbuffer->max_cb) {
      ERROR("Error number of CB to encode (%d) exceeds soft buffer size (%d CBs)", cb_segm->C, softbuffer->max_cb);
      return SRSRAN_ERROR;
    }

    if (Qm == 0) {
      ERROR("Invalid Qm");
      return SRSRAN_ERROR;
    }

    uint32_t Gp = nof_e_bits / Qm;

    uint32_t gamma = Gp;
    if (cb_segm->C > 0) {
      gamma = Gp % cb_segm->C;
    }

    /* Reset TB CRC */
    srsran_crc_set_init(&q->crc_tb, 0);

    wp = 0;
    rp = 0;
    for (i = 0; i < cb_segm->C; i++) {
      uint32_t cblen_idx;
      /* Get read lengths */
      if (i < cb_segm->C2) {
        cb_len    = cb_segm->K2;
        cblen_idx = cb_segm->K2_idx;
      } else {
        cb_len    = cb_segm->K1;
        cblen_idx = cb_segm->K1_idx;
      }
      if (cb_segm->C > 1) {
        rlen = cb_len - 24;
      } else {
        rlen = cb_len;
      }
      if (i <= cb_segm->C - gamma - 1) {
        n_e = Qm * (Gp / cb_segm->C);
      } else {
        n_e = Qm * ((uint32_t)ceilf((float)Gp / cb_segm->C));
      }

      INFO("CB#%d: cb_len: %d, rlen: %d, wp: %d, rp: %d, E: %d", i, cb_len, rlen, wp, rp, n_e);

      if (data) {
        bool last_cb = false;

        /* Copy data to another buffer, making space for the Codeblock CRC */
        if (i < cb_segm->C - 1) {
          // Copy data
          memcpy(q->cb_in, &data[rp / 8], rlen * sizeof(uint8_t) / 8);
        } else {
          INFO("Last CB, appending parity: %d from %d and 24 to %d", rlen - 24, rp, rlen - 24);

          /* Append Transport Block parity bits to the last CB */
          memcpy(q->cb_in, &data[rp / 8], (rlen - 24) * sizeof(uint8_t) / 8);
          last_cb = true;
        }

        /* Turbo Encoding
         * If Codeblock CRC is required it is given the CRC instance pointer, otherwise CRC pointer shall be NULL
         */
        srsran_tcod_encode_lut(&q->encoder,
                               &q->crc_tb,
                               (cb_segm->C > 1) ? &q->crc_cb : NULL,
                               q->cb_in,
                               q->parity_bits,
                               cblen_idx,
                               last_cb);
      }
      DEBUG("RM cblen_idx=%d, n_e=%d, wp=%d, nof_e_bits=%d", cblen_idx, n_e, wp, nof_e_bits);

      /* Rate matching */
      if (srsran_rm_turbo_tx_lut(softbuffer->buffer_b[i],
                                 q->cb_in,
                                 q->parity_bits,
                                 &e_bits[(wp + w_offset) / 8],
                                 cblen_idx,
                                 n_e,
                                 (wp + w_offset) % 8,
                                 rv)) {
        ERROR("Error in rate matching");
        return SRSRAN_ERROR;
      }

      /* Set read/write pointers */
      rp += rlen;
      wp += n_e;
    }

    INFO("END CB#%d: wp: %d, rp: %d", i, wp, rp);
    ret = SRSRAN_SUCCESS;
  } else {
    ERROR("Invalid parameters: e_bits=%d, cb_segm=%d, softbuffer=%d", e_bits != 0, cb_segm != 0, softbuffer != 0);
  }
  return ret;
}

static int encode_tb(srsran_sch_t*           q,
                     srsran_softbuffer_tx_t* soft_buffer,
                     srsran_cbsegm_t*        cb_segm,
                     uint32_t                Qm,
                     uint32_t                rv,
                     uint32_t                nof_e_bits,
                     uint8_t*                data,
                     uint8_t*                e_bits)
{
  return encode_tb_off(q, soft_buffer, cb_segm, Qm, rv, nof_e_bits, data, e_bits, 0);
}

bool decode_tb_cb(srsran_sch_t*           q,
                  srsran_softbuffer_rx_t* softbuffer,
                  srsran_cbsegm_t*        cb_segm,
                  uint32_t                Qm,
                  uint32_t                rv,
                  uint32_t                nof_e_bits,
                  void*                   e_bits,
                  uint8_t*                data)
{
  int8_t*  e_bits_b = e_bits;
  int16_t* e_bits_s = e_bits;

  if (cb_segm->C > SRSRAN_MAX_CODEBLOCKS) {
    ERROR("Error SRSRAN_MAX_CODEBLOCKS=%d", SRSRAN_MAX_CODEBLOCKS);
    return false;
  }

  q->avg_iterations = 0;

  for (int cb_idx = 0; cb_idx < cb_segm->C; cb_idx++) {
    /* Do not process blocks with CRC Ok */
    if (softbuffer->cb_crc[cb_idx] == false) {
      uint32_t cb_len     = cb_idx < cb_segm->C1 ? cb_segm->K1 : cb_segm->K2;
      uint32_t cb_len_idx = cb_idx < cb_segm->C1 ? cb_segm->K1_idx : cb_segm->K2_idx;

      uint32_t rlen  = cb_segm->C == 1 ? cb_len : (cb_len - 24);
      uint32_t Gp    = nof_e_bits / Qm;
      uint32_t gamma = cb_segm->C > 0 ? Gp % cb_segm->C : Gp;
      uint32_t n_e   = Qm * (Gp / cb_segm->C);

      uint32_t rp   = cb_idx * n_e;
      uint32_t n_e2 = n_e;

      if (cb_idx > cb_segm->C - gamma) {
        n_e2 = n_e + Qm;
        rp   = (cb_segm->C - gamma) * n_e + (cb_idx - (cb_segm->C - gamma)) * n_e2;
      }

      if (q->llr_is_8bit) {
        if (srsran_rm_turbo_rx_lut_8bit(&e_bits_b[rp], (int8_t*)softbuffer->buffer_f[cb_idx], n_e2, cb_len_idx, rv)) {
          ERROR("Error in rate matching");
          return SRSRAN_ERROR;
        }
      } else {
        if (srsran_rm_turbo_rx_lut(&e_bits_s[rp], softbuffer->buffer_f[cb_idx], n_e2, cb_len_idx, rv)) {
          ERROR("Error in rate matching");
          return SRSRAN_ERROR;
        }
      }

      srsran_tdec_new_cb(&q->decoder, cb_len);

      // Run iterations and use CRC for early stopping
      bool     early_stop = false;
      uint32_t cb_noi     = 0;
      do {
        if (q->llr_is_8bit) {
          srsran_tdec_iteration_8bit(&q->decoder, (int8_t*)softbuffer->buffer_f[cb_idx], &data[cb_idx * rlen / 8]);
        } else {
          srsran_tdec_iteration(&q->decoder, softbuffer->buffer_f[cb_idx], &data[cb_idx * rlen / 8]);
        }
        q->avg_iterations++;
        cb_noi++;

        uint32_t      len_crc;
        srsran_crc_t* crc_ptr;

        if (cb_segm->C > 1) {
          len_crc = cb_len;
          crc_ptr = &q->crc_cb;
        } else {
          len_crc = cb_segm->tbs + 24;
          crc_ptr = &q->crc_tb;
        }

        // CRC is OK and ran the minimum number of iterations
        if (!srsran_crc_checksum_byte(crc_ptr, &data[cb_idx * rlen / 8], len_crc) &&
            (cb_noi >= SRSRAN_PDSCH_MIN_TDEC_ITERS)) {
          softbuffer->cb_crc[cb_idx] = true;
          early_stop                 = true;

          // CRC is error and exceeded maximum iterations for this CB.
          // Early stop the whole transport block.
        }

      } while (cb_noi < q->max_iterations && !early_stop);

      INFO("CB %d: rp=%d, n_e=%d, cb_len=%d, CRC=%s, rlen=%d, iterations=%d/%d",
           cb_idx,
           rp,
           n_e2,
           cb_len,
           early_stop ? "OK" : "KO",
           rlen,
           cb_noi,
           q->max_iterations);

    } else {
      // Copy decoded data from previous transmissions
      uint32_t cb_len = cb_idx < cb_segm->C1 ? cb_segm->K1 : cb_segm->K2;
      uint32_t rlen   = cb_segm->C == 1 ? cb_len : (cb_len - 24);
      memcpy(&data[cb_idx * rlen / 8], softbuffer->data[cb_idx], rlen / 8 * sizeof(uint8_t));
    }
  }

  softbuffer->tb_crc = true;
  for (int i = 0; i < cb_segm->C && softbuffer->tb_crc; i++) {
    /* If one CB failed return false */
    softbuffer->tb_crc = softbuffer->cb_crc[i];
  }
  // If TB CRC failed, save correct CB for next retransmission
  if (!softbuffer->tb_crc) {
    for (int i = 0; i < cb_segm->C; i++) {
      if (softbuffer->cb_crc[i]) {
        uint32_t cb_len = i < cb_segm->C1 ? cb_segm->K1 : cb_segm->K2;
        uint32_t rlen   = cb_segm->C == 1 ? cb_len : (cb_len - 24);
        memcpy(softbuffer->data[i], &data[i * rlen / 8], rlen / 8 * sizeof(uint8_t));
      }
    }
  }

  q->avg_iterations /= (float)cb_segm->C;
  return softbuffer->tb_crc;
}

/**
 * Decode a transport block according to 36.212 5.3.2
 *
 * @param[in] q
 * @param[inout] softbuffer Initialized softbuffer
 * @param[in] cb_segm Code block segmentation parameters
 * @param[in] e_bits Input transport block
 * @param[in] Qm Modulation type
 * @param[in] rv Redundancy Version. Indicates which part of FEC bits is in input buffer
 * @param[out] softbuffer Initialized output softbuffer
 * @param[out] data Decoded transport block
 * @return negative if error in parameters or CRC error in decoding
 */
static int decode_tb(srsran_sch_t*           q,
                     srsran_softbuffer_rx_t* softbuffer,
                     srsran_cbsegm_t*        cb_segm,
                     uint32_t                Qm,
                     uint32_t                rv,
                     uint32_t                nof_e_bits,
                     int16_t*                e_bits,
                     uint8_t*                data)
{
  // Check inputs
  if (q == NULL || data == NULL || softbuffer == NULL || e_bits == NULL || cb_segm == NULL || Qm == 0) {
    ERROR("Missing inputs: data=%d, softbuffer=%d, e_bits=%d, cb_segm=%d Qm=%d",
          data != 0,
          softbuffer != 0,
          e_bits != 0,
          cb_segm != 0,
          Qm);
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Check segmentation is valid
  if (cb_segm->tbs == 0 || cb_segm->C == 0) {
    return SRSRAN_SUCCESS;
  }

  if (cb_segm->F) {
    fprintf(stderr, "Error filler bits are not supported. Use standard TBS\n");
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  if (cb_segm->C > softbuffer->max_cb) {
    fprintf(stderr,
            "Error number of CB to decode (%d) exceeds soft buffer size (%d CBs)\n",
            cb_segm->C,
            softbuffer->max_cb);
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Process Codeblocks
  bool cb_crc_ok = decode_tb_cb(q, softbuffer, cb_segm, Qm, rv, nof_e_bits, e_bits, data);

  // If any of the CBs CRC is KO
  if (!cb_crc_ok) {
    INFO("Error in CB parity");
    return SRSRAN_ERROR;
  }

  // One CB CRC OK, means TB CRC is OK.
  if (cb_segm->C == 1) {
    INFO("TB decoded OK");
    return SRSRAN_SUCCESS;
  }

  // Check TB CRC for whole TB
  if (srsran_crc_match_byte(&q->crc_tb, data, cb_segm->tbs)) {
    INFO("TB decoded OK");
    return SRSRAN_SUCCESS;
  }

  // TB CRC check failed, as at least one CB had a false alarm, reset all CB CRC flags in the softbuffer
  srsran_softbuffer_rx_reset_cb_crc(softbuffer, cb_segm->C);

  INFO("Error in TB parity");
  return SRSRAN_ERROR;
}

int srsran_dlsch_decode(srsran_sch_t* q, srsran_pdsch_cfg_t* cfg, int16_t* e_bits, uint8_t* data)
{
  return srsran_dlsch_decode2(q, cfg, e_bits, data, 0, 1);
}

int srsran_dlsch_decode2(srsran_sch_t*       q,
                         srsran_pdsch_cfg_t* cfg,
                         int16_t*            e_bits,
                         uint8_t*            data,
                         int                 tb_idx,
                         uint32_t            nof_layers)
{
  uint32_t Nl = 1;

  if (nof_layers != cfg->grant.nof_tb) {
    Nl = 2;
  }
  // Prepare cbsegm
  srsran_cbsegm_t cb_segm;
  if (srsran_cbsegm(&cb_segm, (uint32_t)cfg->grant.tb[tb_idx].tbs)) {
    ERROR("Error computing Codeword (%d) segmentation for TBS=%d", tb_idx, cfg->grant.tb[tb_idx].tbs);
    return SRSRAN_ERROR;
  }

  uint32_t Qm = srsran_mod_bits_x_symbol(cfg->grant.tb[tb_idx].mod);

  return decode_tb(q,
                   cfg->softbuffers.rx[tb_idx],
                   &cb_segm,
                   Qm * Nl,
                   cfg->grant.tb[tb_idx].rv,
                   cfg->grant.tb[tb_idx].nof_bits,
                   e_bits,
                   data);
}

/**
 * Encode transport block. Segments into code blocks, adds channel coding, and does rate matching.
 *
 * @param[in] q Initialized
 * @param[in] cfg Encoding parameters
 * @param[inout] softbuffer Initialized softbuffer
 * @param[in] data Byte array of data. Size is implicit in cb_segm
 * @param e_bits
 * @return Error code
 */
int srsran_dlsch_encode(srsran_sch_t* q, srsran_pdsch_cfg_t* cfg, uint8_t* data, uint8_t* e_bits)
{
  return srsran_dlsch_encode2(q, cfg, data, e_bits, 0, 1);
}

int srsran_dlsch_encode2(srsran_sch_t*       q,
                         srsran_pdsch_cfg_t* cfg,
                         uint8_t*            data,
                         uint8_t*            e_bits,
                         int                 tb_idx,
                         uint32_t            nof_layers)
{
  uint32_t Nl = 1;

  if (nof_layers != cfg->grant.nof_tb) {
    Nl = 2;
  }

  // Prepare cbsegm
  srsran_cbsegm_t cb_segm;
  if (srsran_cbsegm(&cb_segm, (uint32_t)cfg->grant.tb[tb_idx].tbs)) {
    ERROR("Error computing Codeword (%d) segmentation for TBS=%d", tb_idx, cfg->grant.tb[tb_idx].tbs);
    return SRSRAN_ERROR;
  }

  uint32_t Qm = srsran_mod_bits_x_symbol(cfg->grant.tb[tb_idx].mod);

  return encode_tb(q,
                   cfg->softbuffers.tx[tb_idx],
                   &cb_segm,
                   Qm * Nl,
                   cfg->grant.tb[tb_idx].rv,
                   cfg->grant.tb[tb_idx].nof_bits,
                   data,
                   e_bits);
}

/* Compute the interleaving function on-the-fly, because it depends on number of RI bits
 * Profiling show that the computation of this matrix is neglegible.
 */
static void ulsch_interleave_gen(uint32_t  H_prime_total,
                                 uint32_t  N_pusch_symbs,
                                 uint32_t  Qm,
                                 uint8_t*  ri_present,
                                 uint32_t* interleaver_lut)
{
  uint32_t rows = H_prime_total / N_pusch_symbs;
  uint32_t cols = N_pusch_symbs;
  uint32_t idx  = 0;
  for (uint32_t j = 0; j < rows; j++) {
    for (uint32_t i = 0; i < cols; i++) {
      for (uint32_t k = 0; k < Qm; k++) {
        if (ri_present && ri_present[j * Qm + i * rows * Qm + k]) {
          interleaver_lut[j * Qm + i * rows * Qm + k] = 0;
        } else {
          interleaver_lut[j * Qm + i * rows * Qm + k] = idx;
          idx++;
        }
      }
    }
  }
}

static void ulsch_interleave_qm2(const uint8_t* g_bits,
                                 uint32_t       rows,
                                 uint32_t       cols,
                                 uint8_t*       q_bits,
                                 uint32_t       ri_min_row,
                                 const uint8_t* ri_present)
{
  uint32_t bit_read_idx = 0;

  for (uint32_t j = 0; j < ri_min_row; j++) {
    for (uint32_t i = 0; i < cols; i++) {
      uint32_t k = (i * rows + j) * 2;

      uint32_t read_byte_idx  = bit_read_idx / 8;
      uint32_t read_bit_idx   = bit_read_idx % 8;
      uint32_t write_byte_idx = k / 8;
      uint32_t write_bit_idx  = k % 8;
      uint8_t  w              = (g_bits[read_byte_idx] >> (6 - read_bit_idx)) & (uint8_t)0x03;
      q_bits[write_byte_idx] |= w << (6 - write_bit_idx);

      bit_read_idx += 2;
    }
  }

  for (uint32_t j = ri_min_row; j < rows; j++) {
    for (uint32_t i = 0; i < cols; i++) {
      uint32_t k = (i * rows + j) * 2;

      if (ri_present[k]) {
        /* do nothing */
      } else {
        uint32_t read_byte_idx  = bit_read_idx / 8;
        uint32_t read_bit_idx   = bit_read_idx % 8;
        uint32_t write_byte_idx = k / 8;
        uint32_t write_bit_idx  = k % 8;
        uint8_t  w              = (g_bits[read_byte_idx] >> (6 - read_bit_idx)) & (uint8_t)0x03;
        q_bits[write_byte_idx] |= w << (6 - write_bit_idx);

        bit_read_idx += 2;
      }
    }
  }
}

static void ulsch_interleave_qm4(uint8_t*       g_bits,
                                 uint32_t       rows,
                                 uint32_t       cols,
                                 uint8_t*       q_bits,
                                 uint32_t       ri_min_row,
                                 const uint8_t* ri_present)
{
  uint32_t bit_read_idx = 0;

  for (uint32_t j = 0; j < ri_min_row; j++) {
    int32_t i = 0;

#ifdef DOES_NOT_WORK
#ifndef HAVE_NEON
    __m128i  _counter = _mm_slli_epi32(_mm_add_epi32(_mm_mullo_epi32(_counter0, _rows), _mm_set1_epi32(j)), 2);
    uint8_t* _g_bits  = &g_bits[bit_read_idx / 8];

    /* First bits are aligned to byte */
    if (0 == (bit_read_idx & 0x3)) {
      for (; i < (cols - 3); i += 4) {
        uint8_t w1 = *(_g_bits++);
        uint8_t w2 = *(_g_bits++);

        __m128i _write_byte_idx = _mm_srli_epi32(_counter, 3);
        __m128i _write_bit_idx  = _mm_and_si128(_counter, _7);
        __m128i _write_shift    = _mm_sub_epi32(_4, _write_bit_idx);

        q_bits[_mm_extract_epi32(_write_byte_idx, 0)] |= (w1 >> 0x4) << _mm_extract_epi32(_write_shift, 0);
        q_bits[_mm_extract_epi32(_write_byte_idx, 1)] |= (w1 & 0xf) << _mm_extract_epi32(_write_shift, 1);
        q_bits[_mm_extract_epi32(_write_byte_idx, 2)] |= (w2 >> 0x4) << _mm_extract_epi32(_write_shift, 2);
        q_bits[_mm_extract_epi32(_write_byte_idx, 3)] |= (w2 & 0xf) << _mm_extract_epi32(_write_shift, 3);
        _counter = _mm_add_epi32(_counter, _inc);
      }
    } else {
      for (; i < (cols - 3); i += 4) {
        __m128i _write_byte_idx = _mm_srli_epi32(_counter, 3);
        __m128i _write_bit_idx  = _mm_and_si128(_counter, _7);
        __m128i _write_shift    = _mm_sub_epi32(_4, _write_bit_idx);

        uint8_t w1 = *(_g_bits);
        uint8_t w2 = *(_g_bits++);
        uint8_t w3 = *(_g_bits++);
        q_bits[_mm_extract_epi32(_write_byte_idx, 0)] |= (w1 & 0xf) << _mm_extract_epi32(_write_shift, 0);
        q_bits[_mm_extract_epi32(_write_byte_idx, 1)] |= (w2 >> 0x4) << _mm_extract_epi32(_write_shift, 1);
        q_bits[_mm_extract_epi32(_write_byte_idx, 2)] |= (w2 & 0xf) << _mm_extract_epi32(_write_shift, 2);
        q_bits[_mm_extract_epi32(_write_byte_idx, 3)] |= (w3 >> 0x4) << _mm_extract_epi32(_write_shift, 3);

        _counter = _mm_add_epi32(_counter, _inc);
      }
    }
    bit_read_idx += i * 4;
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */

    /* Spare bits */
    for (; i < cols; i++) {
      uint32_t k = (i * rows + j) * 4;

      uint32_t read_byte_idx  = bit_read_idx / 8;
      uint32_t read_bit_idx   = bit_read_idx % 8;
      uint32_t write_byte_idx = k / 8;
      uint32_t write_bit_idx  = k % 8;
      uint8_t  w              = (g_bits[read_byte_idx] >> (4 - read_bit_idx)) & (uint8_t)0x0f;
      q_bits[write_byte_idx] |= w << (4 - write_bit_idx);

      bit_read_idx += 4;
    }
  }

  /* Do rows containing RI */
  for (uint32_t j = ri_min_row; j < rows; j++) {
    for (uint32_t i = 0; i < cols; i++) {
      uint32_t k = (i * rows + j) * 4;

      if (ri_present[k]) {
        /* do nothing */
      } else {
        uint32_t read_byte_idx  = bit_read_idx / 8;
        uint32_t read_bit_idx   = bit_read_idx % 8;
        uint32_t write_byte_idx = k / 8;
        uint32_t write_bit_idx  = k % 8;
        uint8_t  w              = (g_bits[read_byte_idx] >> (4 - read_bit_idx)) & (uint8_t)0x0f;
        q_bits[write_byte_idx] |= w << (4 - write_bit_idx);

        bit_read_idx += 4;
      }
    }
  }
}

static void ulsch_interleave_qm6(const uint8_t* g_bits,
                                 uint32_t       rows,
                                 uint32_t       cols,
                                 uint8_t*       q_bits,
                                 uint32_t       ri_min_row,
                                 const uint8_t* ri_present)
{
  uint32_t bit_read_idx = 0;

  for (uint32_t j = 0; j < ri_min_row; j++) {
    for (uint32_t i = 0; i < cols; i++) {
      uint32_t k = (i * rows + j) * 6;

      uint32_t read_byte_idx  = bit_read_idx / 8;
      uint32_t read_bit_idx   = bit_read_idx % 8;
      uint32_t write_byte_idx = k / 8;
      uint32_t write_bit_idx  = k % 8;
      uint8_t  w;

      switch (read_bit_idx) {
        case 0:
          w = g_bits[read_byte_idx] >> 2;
          break;
        case 2:
          w = g_bits[read_byte_idx] & (uint8_t)0x3f;
          break;
        case 4:
          w = ((g_bits[read_byte_idx] << 2) | (g_bits[read_byte_idx + 1] >> 6)) & (uint8_t)0x3f;
          break;
        case 6:
          w = ((g_bits[read_byte_idx] << 4) | (g_bits[read_byte_idx + 1] >> 4)) & (uint8_t)0x3f;
          break;
        default:
          w = 0;
      }

      switch (write_bit_idx) {
        case 0:
          q_bits[write_byte_idx] |= w << 2;
          break;
        case 2:
          q_bits[write_byte_idx] |= w;
          break;
        case 4:
          q_bits[write_byte_idx] |= w >> 2;
          q_bits[write_byte_idx + 1] |= w << 6;
          break;
        case 6:
          q_bits[write_byte_idx] |= w >> 4;
          q_bits[write_byte_idx + 1] |= w << 4;
          break;
        default:
          /* Do nothing */;
      }

      bit_read_idx += 6;
    }
  }

  for (uint32_t j = ri_min_row; j < rows; j++) {
    for (uint32_t i = 0; i < cols; i++) {
      uint32_t k = (i * rows + j) * 6;

      if (ri_present[k]) {
        /* do nothing */
      } else {
        uint32_t read_byte_idx  = bit_read_idx / 8;
        uint32_t read_bit_idx   = bit_read_idx % 8;
        uint32_t write_byte_idx = k / 8;
        uint32_t write_bit_idx  = k % 8;
        uint8_t  w;

        switch (read_bit_idx) {
          case 0:
            w = g_bits[read_byte_idx] >> 2;
            break;
          case 2:
            w = g_bits[read_byte_idx] & (uint8_t)0x3f;
            break;
          case 4:
            w = ((g_bits[read_byte_idx] << 2) | (g_bits[read_byte_idx + 1] >> 6)) & (uint8_t)0x3f;
            break;
          case 6:
            w = ((g_bits[read_byte_idx] << 4) | (g_bits[read_byte_idx + 1] >> 4)) & (uint8_t)0x3f;
            break;
          default:
            w = 0;
        }

        switch (write_bit_idx) {
          case 0:
            q_bits[write_byte_idx] |= w << 2;
            break;
          case 2:
            q_bits[write_byte_idx] |= w;
            break;
          case 4:
            q_bits[write_byte_idx] |= w >> 2;
            q_bits[write_byte_idx + 1] |= w << 6;
            break;
          case 6:
            q_bits[write_byte_idx] |= w >> 4;
            q_bits[write_byte_idx + 1] |= w << 4;
            break;
          default:
            /* Do nothing */;
        }

        bit_read_idx += 6;
      }
    }
  }
}

/* UL-SCH channel interleaver according to 5.2.2.8 of 36.212 */
static void ulsch_interleave(uint8_t*          g_bits,
                             uint32_t          Qm,
                             uint32_t          H_prime_total,
                             uint32_t          N_pusch_symbs,
                             uint8_t*          q_bits,
                             srsran_uci_bit_t* ri_bits,
                             uint32_t          nof_ri_bits,
                             uint8_t*          ri_present)
{
  if (N_pusch_symbs == 0 || Qm == 0 || H_prime_total == 0 || H_prime_total < N_pusch_symbs) {
    ERROR("Invalid input: N_pusch_symbs=%d, Qm=%d, H_prime_total=%d, N_pusch_symbs=%d",
          N_pusch_symbs,
          Qm,
          H_prime_total,
          N_pusch_symbs);
    return;
  }

  const uint32_t nof_bits   = H_prime_total * Qm;
  uint32_t       rows       = H_prime_total / N_pusch_symbs;
  uint32_t       cols       = N_pusch_symbs;
  uint32_t       ri_min_row = rows;

  // Prepare ri_bits for fast search using temp_buffer
  if (nof_ri_bits > 0) {
    for (uint32_t i = 0; i < nof_ri_bits; i++) {
      uint32_t ri_row = (ri_bits[i].position / Qm) % rows;

      if (ri_row < ri_min_row) {
        ri_min_row = ri_row;
      }

      ri_present[ri_bits[i].position] = 1;
    }
  }

  bzero(q_bits, nof_bits / 8);
  switch (Qm) {
    case 2:
      ulsch_interleave_qm2(g_bits, rows, cols, q_bits, ri_min_row, ri_present);
      break;
    case 4:
      ulsch_interleave_qm4(g_bits, rows, cols, q_bits, ri_min_row, ri_present);
      break;
    case 6:
      ulsch_interleave_qm6(g_bits, rows, cols, q_bits, ri_min_row, ri_present);
      break;
    default:
      /* This line should never be reached */
      ERROR("Wrong Qm (%d)", Qm);
  }

  // Reset temp_buffer because will be reused next time
  if (nof_ri_bits > 0) {
    for (uint32_t i = 0; i < nof_ri_bits; i++) {
      ri_present[ri_bits[i].position] = 0;
    }
  }
}

/* UL-SCH channel deinterleaver according to 5.2.2.8 of 36.212 */
void ulsch_deinterleave(int16_t*          q_bits,
                        uint32_t          Qm,
                        uint32_t          H_prime_total,
                        uint32_t          N_pusch_symbs,
                        int16_t*          g_bits,
                        srsran_uci_bit_t* ri_bits,
                        uint32_t          nof_ri_bits,
                        uint8_t*          ri_present,
                        uint32_t*         inteleaver_lut)
{
  // Prepare ri_bits for fast search using temp_buffer
  if (nof_ri_bits > 0) {
    for (uint32_t i = 0; i < nof_ri_bits; i++) {
      ri_present[ri_bits[i].position] = 1;
    }
  }

  // Generate interleaver table and interleave samples
  ulsch_interleave_gen(H_prime_total, N_pusch_symbs, Qm, ri_present, inteleaver_lut);
  srsran_vec_lut_sis(q_bits, inteleaver_lut, g_bits, H_prime_total * Qm);

  // Reset temp_buffer because will be reused next time
  if (nof_ri_bits > 0) {
    for (uint32_t i = 0; i < nof_ri_bits; i++) {
      ri_present[ri_bits[i].position] = 0;
    }
  }
}

static int uci_decode_ri_ack(srsran_sch_t*       q,
                             srsran_pusch_cfg_t* cfg,
                             int16_t*            q_bits,
                             uint8_t*            c_seq,
                             srsran_uci_value_t* uci_data)
{
  int ret = 0;

  uint32_t Q_prime_ack = 0;
  uint32_t Q_prime_ri  = 0;

  uint32_t nb_q = cfg->grant.tb.nof_bits;
  uint32_t Qm   = srsran_mod_bits_x_symbol(cfg->grant.tb.mod);

  if (Qm == 0) {
    ERROR("Invalid modulation %s", srsran_mod_string(cfg->grant.tb.mod));
    return SRSRAN_ERROR;
  }

  // If there is RI and CQI, assume RI = 1 for the purpose of RI/ACK decoding (3GPP 36.212 Clause 5.2.4.1. )
  if (cfg->uci_cfg.cqi.data_enable) {
    if (cfg->uci_cfg.cqi.type == SRSRAN_CQI_TYPE_SUBBAND_HL && cfg->uci_cfg.cqi.ri_len) {
      cfg->uci_cfg.cqi.rank_is_not_one = false;
    }
  }

  uint32_t cqi_len = srsran_cqi_size(&cfg->uci_cfg.cqi);

  // Deinterleave and decode HARQ bits
  if (srsran_uci_cfg_total_ack(&cfg->uci_cfg) > 0) {
    float beta = get_beta_harq_offset(cfg->uci_offset.I_offset_ack);
    if (cfg->grant.tb.tbs == 0) {
      float beta_cqi = get_beta_cqi_offset(cfg->uci_offset.I_offset_cqi);
      if (!isnormal(beta_cqi)) {
        ERROR("Invalid beta_cqi (%d, %f)", cfg->uci_offset.I_offset_cqi, beta_cqi);
        return SRSRAN_ERROR;
      }
      beta /= beta_cqi;
    }
    ret = srsran_uci_decode_ack_ri(cfg,
                                   q_bits,
                                   c_seq,
                                   beta,
                                   nb_q / Qm,
                                   cqi_len,
                                   q->ack_ri_bits,
                                   uci_data->ack.ack_value,
                                   &uci_data->ack.valid,
                                   srsran_uci_cfg_total_ack(&cfg->uci_cfg),
                                   false);
    if (ret < 0) {
      return ret;
    }
    Q_prime_ack = (uint32_t)ret;

    // Set zeros to HARQ bits
    for (uint32_t i = 0; i < Q_prime_ack * Qm; i++) {
      q_bits[q->ack_ri_bits[i].position] = 0;
    }
  }

  // Deinterleave and decode RI bits
  if (cfg->uci_cfg.cqi.ri_len > 0) {
    float beta = get_beta_ri_offset(cfg->uci_offset.I_offset_ri);
    if (cfg->grant.tb.tbs == 0) {
      float beta_cqi = get_beta_cqi_offset(cfg->uci_offset.I_offset_cqi);
      if (!isnormal(beta_cqi)) {
        ERROR("Invalid beta_cqi (%d, %f)", cfg->uci_offset.I_offset_cqi, beta_cqi);
        return SRSRAN_ERROR;
      }
      beta /= beta_cqi;
    }
    ret = srsran_uci_decode_ack_ri(cfg,
                                   q_bits,
                                   c_seq,
                                   beta,
                                   nb_q / Qm,
                                   cqi_len,
                                   q->ack_ri_bits,
                                   &uci_data->ri,
                                   NULL,
                                   cfg->uci_cfg.cqi.ri_len,
                                   true);
    if (ret < 0) {
      return ret;
    }
    Q_prime_ri = (uint32_t)ret;
  }

  // Now set correct RI
  if (cfg->uci_cfg.cqi.data_enable) {
    if (cfg->uci_cfg.cqi.type == SRSRAN_CQI_TYPE_SUBBAND_HL && cfg->uci_cfg.cqi.ri_len) {
      cfg->uci_cfg.cqi.rank_is_not_one = uci_data->ri > 0;
    }
  }

  return Q_prime_ri;
}

int srsran_ulsch_decode(srsran_sch_t*       q,
                        srsran_pusch_cfg_t* cfg,
                        int16_t*            q_bits,
                        int16_t*            g_bits,
                        uint8_t*            c_seq,
                        uint8_t*            data,
                        srsran_uci_value_t* uci_data)
{
  int ret = SRSRAN_ERROR_INVALID_INPUTS;

  // Prepare cbsegm
  srsran_cbsegm_t cb_segm;
  if (srsran_cbsegm(&cb_segm, (uint32_t)cfg->grant.tb.tbs)) {
    ERROR("Error computing segmentation for TBS=%d", cfg->grant.tb.tbs);
    return SRSRAN_ERROR;
  }

  uint32_t nb_q = cfg->grant.tb.nof_bits;
  uint32_t Qm   = srsran_mod_bits_x_symbol(cfg->grant.tb.mod);

  cfg->K_segm = cb_segm.C1 * cb_segm.K1 + cb_segm.C2 * cb_segm.K2;

  // Decode RI/HARQ values
  if ((ret = uci_decode_ri_ack(q, cfg, q_bits, c_seq, uci_data)) < 0) {
    ERROR("Error decoding RI/HARQ bits");
    return SRSRAN_ERROR;
  }

  uint32_t Q_prime_ri = (uint32_t)ret;

  // Deinterleave data and CQI in ULSCH
  ulsch_deinterleave(q_bits,
                     Qm,
                     nb_q / Qm,
                     cfg->grant.nof_symb,
                     g_bits,
                     q->ack_ri_bits,
                     Q_prime_ri * Qm,
                     q->temp_g_bits,
                     q->ul_interleaver);

  // Decode CQI (multiplexed at the front of ULSCH)
  uint32_t Q_prime_cqi = 0;
  uint32_t e_offset    = 0;
  if (cfg->uci_cfg.cqi.data_enable) {
    uint32_t cqi_len = srsran_cqi_size(&cfg->uci_cfg.cqi);
    uint8_t  cqi_buff[SRSRAN_CQI_MAX_BITS];
    ZERO_OBJECT(cqi_buff);
    ret = srsran_uci_decode_cqi_pusch(&q->uci_cqi,
                                      cfg,
                                      g_bits,
                                      get_beta_cqi_offset(cfg->uci_offset.I_offset_cqi),
                                      Q_prime_ri,
                                      cqi_len,
                                      cqi_buff,
                                      &uci_data->cqi.data_crc);
    if (ret < 0) {
      return ret;
    }
    srsran_cqi_value_unpack(&cfg->uci_cfg.cqi, cqi_buff, &uci_data->cqi);
    Q_prime_cqi = (uint32_t)ret;
  }

  e_offset += Q_prime_cqi * Qm;

  // Decode ULSCH
  if (cb_segm.tbs > 0) {
    uint32_t G = nb_q / Qm - Q_prime_ri - Q_prime_cqi;
    ret        = decode_tb(q, cfg->softbuffers.rx, &cb_segm, Qm, cfg->grant.tb.rv, G * Qm, &g_bits[e_offset], data);
  }
  return ret;
}

int srsran_ulsch_encode(srsran_sch_t*       q,
                        srsran_pusch_cfg_t* cfg,
                        uint8_t*            data,
                        srsran_uci_value_t* uci_data,
                        uint8_t*            g_bits,
                        uint8_t*            q_bits)
{
  int ret;

  uint32_t e_offset    = 0;
  uint32_t Q_prime_cqi = 0;
  uint32_t Q_prime_ack = 0;
  uint32_t Q_prime_ri  = 0;

  uint32_t nb_q = cfg->grant.tb.nof_bits;
  uint32_t Qm   = srsran_mod_bits_x_symbol(cfg->grant.tb.mod);

  if (Qm == 0) {
    ERROR("Invalid input");
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  if (cfg->grant.nof_symb == 0) {
    ERROR("Invalid input");
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Prepare cbsegm
  srsran_cbsegm_t cb_segm;
  if (srsran_cbsegm(&cb_segm, (uint32_t)cfg->grant.tb.tbs)) {
    ERROR("Error computing segmentation for TBS=%d", cfg->grant.tb.tbs);
    return SRSRAN_ERROR;
  }

  cfg->K_segm = cb_segm.C1 * cb_segm.K1 + cb_segm.C2 * cb_segm.K2;

  int     uci_cqi_len = 0;
  uint8_t cqi_buff[SRSRAN_CQI_MAX_BITS];
  ZERO_OBJECT(cqi_buff);

  if (cfg->uci_cfg.cqi.data_enable) {
    uci_cqi_len = (uint32_t)srsran_cqi_value_pack(&cfg->uci_cfg.cqi, &uci_data->cqi, cqi_buff);
    if (uci_cqi_len < 0) {
      ERROR("Error encoding CQI bits");
      return SRSRAN_ERROR;
    }
  }

  if (cfg->uci_cfg.cqi.ri_len > 0) {
    float beta = get_beta_ri_offset(cfg->uci_offset.I_offset_ri);
    if (cb_segm.tbs == 0) {
      beta /= get_beta_cqi_offset(cfg->uci_offset.I_offset_cqi);
    }
    uint8_t ri[2] = {uci_data->ri, 0};
    ret           = srsran_uci_encode_ack_ri(cfg,
                                   ri,
                                   cfg->uci_cfg.cqi.ri_len,
                                   (uint32_t)uci_cqi_len,
                                   beta,
                                   nb_q / Qm,
                                   true,
                                   cfg->uci_cfg.ack[0].N_bundle,
                                   q->ack_ri_bits);
    if (ret < 0) {
      return ret;
    }
    Q_prime_ri = (uint32_t)ret;
  }

  // Encode CQI
  if (uci_cqi_len > 0) {
    ret = srsran_uci_encode_cqi_pusch(&q->uci_cqi,
                                      cfg,
                                      cqi_buff,
                                      (uint32_t)uci_cqi_len,
                                      get_beta_cqi_offset(cfg->uci_offset.I_offset_cqi),
                                      Q_prime_ri,
                                      q->temp_g_bits);
    if (ret < 0) {
      return ret;
    }
    Q_prime_cqi = (uint32_t)ret;

    srsran_bit_pack_vector(q->temp_g_bits, g_bits, Q_prime_cqi * Qm);

    // Reset the buffer because will be reused in ulsch_interleave
    srsran_vec_u8_zero(q->temp_g_bits, Q_prime_cqi * Qm);
  }

  e_offset += Q_prime_cqi * Qm;

  // Encode UL-SCH
  if (cb_segm.tbs > 0) {
    uint32_t G = nb_q / Qm - Q_prime_ri - Q_prime_cqi;
    ret        = encode_tb_off(
        q, cfg->softbuffers.tx, &cb_segm, Qm, cfg->grant.tb.rv, G * Qm, data, &g_bits[e_offset / 8], e_offset % 8);
    if (ret) {
      return ret;
    }
  }

  // Interleave UL-SCH (and RI and CQI)
  ulsch_interleave(g_bits, Qm, nb_q / Qm, cfg->grant.nof_symb, q_bits, q->ack_ri_bits, Q_prime_ri * Qm, q->temp_g_bits);

  // Encode (and interleave) ACK
  if (srsran_uci_cfg_total_ack(&cfg->uci_cfg) > 0) {
    float beta = get_beta_harq_offset(cfg->uci_offset.I_offset_ack);
    if (cb_segm.tbs == 0) {
      beta /= get_beta_cqi_offset(cfg->uci_offset.I_offset_cqi);
    }
    ret = srsran_uci_encode_ack_ri(cfg,
                                   uci_data->ack.ack_value,
                                   srsran_uci_cfg_total_ack(&cfg->uci_cfg),
                                   (uint32_t)uci_cqi_len,
                                   beta,
                                   nb_q / Qm,
                                   false,
                                   cfg->uci_cfg.ack[0].N_bundle,
                                   &q->ack_ri_bits[Q_prime_ri * Qm]);
    if (ret < 0) {
      return ret;
    }
    Q_prime_ack = (uint32_t)ret;
  }

  uint32_t nof_ri_ack_bits = (Q_prime_ack + Q_prime_ri) * Qm;

  for (uint32_t i = 0; i < nof_ri_ack_bits; i++) {
    uint32_t p = q->ack_ri_bits[i].position;
    if (p < nb_q) {
      if (q->ack_ri_bits[i].type == UCI_BIT_1) {
        q_bits[p / 8] |= (1U << (7 - p % 8));
      } else {
        q_bits[p / 8] &= ~(1U << (7 - p % 8));
      }
    } else {
      ERROR("Invalid RI/ACK bit %d/%d, position %d. Max bits=%d, Qm=%d", i, nof_ri_ack_bits, p, nb_q, Qm);
    }
  }

  INFO("Q_prime_ack=%d, Q_prime_cqi=%d, Q_prime_ri=%d", Q_prime_ack, Q_prime_cqi, Q_prime_ri);

  return nof_ri_ack_bits;
}

void srsran_sl_ulsch_interleave(uint8_t* g_bits,
                                uint32_t Qm,
                                uint32_t H_prime_total,
                                uint32_t N_pusch_symbs,
                                uint8_t* q_bits)
{
  ulsch_interleave(g_bits, Qm, H_prime_total, N_pusch_symbs, q_bits, NULL, 0, false);
}

void srsran_sl_ulsch_deinterleave(int16_t*  q_bits,
                                  uint32_t  Qm,
                                  uint32_t  H_prime_total,
                                  uint32_t  N_pusch_symbs,
                                  int16_t*  g_bits,
                                  uint32_t* inteleaver_lut)
{
  ulsch_deinterleave(q_bits, Qm, H_prime_total, N_pusch_symbs, g_bits, NULL, 0, NULL, inteleaver_lut);
}
