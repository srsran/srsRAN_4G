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

#include "srslte/phy/phch/sch_nr.h"
#include "srslte/config.h"
#include "srslte/phy/fec/cbsegm.h"
#include "srslte/phy/fec/ldpc/ldpc_common.h"
#include "srslte/phy/fec/ldpc/ldpc_rm.h"
#include "srslte/phy/utils/bit.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"
#include <stdint.h>
#include <stdlib.h>
#include <strings.h>

int srslte_sch_nr_encoder_init(srslte_sch_nr_encoder_t* q)
{
  if (q == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  if (srslte_crc_init(&q->crc_tb_24, SRSLTE_LTE_CRC24A, 24) < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  if (srslte_crc_init(&q->crc_cb, SRSLTE_LTE_CRC24B, 24) < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  if (srslte_crc_init(&q->crc_tb_16, SRSLTE_LTE_CRC16, 16) < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  q->cb_in = srslte_vec_u8_malloc(SRSLTE_LDPC_MAX_LEN_CB);
  if (!q->cb_in) {
    return SRSLTE_ERROR;
  }

#ifdef LV_HAVE_AVX2
  srslte_ldpc_encoder_type_t encoder_type = SRSLTE_LDPC_ENCODER_AVX2;
#else  // LV_HAVE_AVX2
  srslte_ldpc_encoder_type_t encoder_type = SRSLTE_LDPC_ENCODER_C;
#endif // LV_HAVE_AVX2

  // Iterate over all possible lifting sizes
  for (uint16_t ls = 0; ls < MAX_LIFTSIZE; ls++) {
    uint8_t ls_index = get_ls_index(ls);

    // Invalid lifting size
    if (ls_index == VOID_LIFTSIZE) {
      q->encoder_bg1[ls] = NULL;
      q->encoder_bg2[ls] = NULL;
      continue;
    }

    q->encoder_bg1[ls] = calloc(1, sizeof(srslte_ldpc_encoder_t));
    if (!q->encoder_bg1[ls]) {
      ERROR("Error: calloc\n");
      return SRSLTE_ERROR;
    }

    if (srslte_ldpc_encoder_init(q->encoder_bg1[ls], encoder_type, BG1, ls) < SRSLTE_SUCCESS) {
      ERROR("Error: initialising BG1 LDPC encoder for ls=%d\n", ls);
      return SRSLTE_ERROR;
    }

    q->encoder_bg2[ls] = calloc(1, sizeof(srslte_ldpc_encoder_t));
    if (!q->encoder_bg2[ls]) {
      return SRSLTE_ERROR;
    }

    if (srslte_ldpc_encoder_init(q->encoder_bg2[ls], encoder_type, BG2, ls) < SRSLTE_SUCCESS) {
      ERROR("Error: initialising BG2 LDPC encoder for ls=%d\n", ls);
      return SRSLTE_ERROR;
    }
  }

  return SRSLTE_SUCCESS;
}

int srslte_sch_nr_encoder_set_carrier(srslte_sch_nr_encoder_t* q, const srslte_carrier_nr_t* carrier)
{
  if (!q) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  q->carrier = *carrier;

  return SRSLTE_SUCCESS;
}

int srslte_sch_nr_decoder_init(srslte_sch_nr_decoder_t* q, const srslte_sch_nr_decoder_cfg_t* decoder_cfg)
{
  if (q == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  if (srslte_crc_init(&q->crc_tb_24, SRSLTE_LTE_CRC24A, 24) < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  if (srslte_crc_init(&q->crc_cb, SRSLTE_LTE_CRC24B, 24) < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  if (srslte_crc_init(&q->crc_tb_16, SRSLTE_LTE_CRC16, 16) < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  srslte_ldpc_decoder_type_t decoder_type = SRSLTE_LDPC_DECODER_C;
  if (decoder_cfg->use_flooded) {
#ifdef LV_HAVE_AVX2
    if (decoder_cfg->disable_simd) {
      decoder_type = SRSLTE_LDPC_DECODER_C_FLOOD;
    } else {
      decoder_type = SRSLTE_LDPC_DECODER_C_AVX2_FLOOD;
    }
#else  // LV_HAVE_AVX2
    decoder_type = SRSLTE_LDPC_DECODER_C_FLOOD;
#endif // LV_HAVE_AVX2
  } else {
#ifdef LV_HAVE_AVX2
    if (!decoder_cfg->disable_simd) {
      decoder_type = SRSLTE_LDPC_DECODER_C_AVX2;
    }
#endif // LV_HAVE_AVX2
  }

  float scaling_factor = isnormal(decoder_cfg->scaling_factor) ? decoder_cfg->scaling_factor : 0.75f;

  // Iterate over all possible lifting sizes
  for (uint16_t ls = 0; ls < MAX_LIFTSIZE; ls++) {
    uint8_t ls_index = get_ls_index(ls);

    // Invalid lifting size
    if (ls_index == VOID_LIFTSIZE) {
      q->decoder_bg1[ls] = NULL;
      q->decoder_bg2[ls] = NULL;
      continue;
    }

    q->decoder_bg1[ls] = calloc(1, sizeof(srslte_ldpc_decoder_t));
    if (!q->decoder_bg1[ls]) {
      ERROR("Error: calloc\n");
      return SRSLTE_ERROR;
    }

    if (srslte_ldpc_decoder_init(q->decoder_bg1[ls], decoder_type, BG1, ls, scaling_factor) < SRSLTE_SUCCESS) {
      ERROR("Error: initialising BG1 LDPC decoder for ls=%d\n", ls);
      return SRSLTE_ERROR;
    }

    q->decoder_bg2[ls] = calloc(1, sizeof(srslte_ldpc_decoder_t));
    if (!q->decoder_bg2[ls]) {
      ERROR("Error: calloc\n");
      return SRSLTE_ERROR;
    }

    if (srslte_ldpc_decoder_init(q->decoder_bg2[ls], decoder_type, BG2, ls, scaling_factor) < SRSLTE_SUCCESS) {
      ERROR("Error: initialising BG2 LDPC decoder for ls=%d\n", ls);
      return SRSLTE_ERROR;
    }
  }

  return SRSLTE_SUCCESS;
}

int srslte_(srslte_sch_nr_encoder_t* q)
{
  if (q == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  if (srslte_crc_init(&q->crc_tb_24, SRSLTE_LTE_CRC24A, 24) < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  if (srslte_crc_init(&q->crc_cb, SRSLTE_LTE_CRC24B, 24) < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  if (srslte_crc_init(&q->crc_tb_16, SRSLTE_LTE_CRC16, 16) < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  q->cb_in = srslte_vec_u8_malloc(SRSLTE_LDPC_MAX_LEN_CB);
  if (!q->cb_in) {
    return SRSLTE_ERROR;
  }

#ifdef LV_HAVE_AVX2
  srslte_ldpc_encoder_type_t encoder_type = SRSLTE_LDPC_ENCODER_AVX2;
#else  // LV_HAVE_AVX2
  srslte_ldpc_encoder_type_t encoder_type = SRSLTE_LDPC_ENCODER_C;
#endif // LV_HAVE_AVX2

  // Iterate over all possible lifting sizes
  for (uint16_t ls = 0; ls < MAX_LIFTSIZE; ls++) {
    uint8_t ls_index = get_ls_index(ls);

    // Invalid lifting size
    if (ls_index == VOID_LIFTSIZE) {
      q->encoder_bg1[ls] = NULL;
      q->encoder_bg2[ls] = NULL;
      continue;
    }

    q->encoder_bg1[ls] = calloc(1, sizeof(srslte_ldpc_encoder_t));
    if (!q->encoder_bg1[ls]) {
      ERROR("Error: calloc\n");
      return SRSLTE_ERROR;
    }

    if (srslte_ldpc_encoder_init(q->encoder_bg1[ls], encoder_type, BG1, ls) < SRSLTE_SUCCESS) {
      ERROR("Error: initialising BG1 LDPC encoder for ls=%d\n", ls);
      return SRSLTE_ERROR;
    }

    q->encoder_bg2[ls] = calloc(1, sizeof(srslte_ldpc_encoder_t));
    if (!q->encoder_bg2[ls]) {
      return SRSLTE_ERROR;
    }

    if (srslte_ldpc_encoder_init(q->encoder_bg2[ls], encoder_type, BG2, ls) < SRSLTE_SUCCESS) {
      ERROR("Error: initialising BG2 LDPC encoder for ls=%d\n", ls);
      return SRSLTE_ERROR;
    }
  }

  return SRSLTE_SUCCESS;
}

void srslte_sch_nr_free(srslte_sch_nr_encoder_t* q)
{
  if (q == NULL) {
    return;
  }

  if (q->cb_in) {
    free(q->cb_in);
  }
}

/**
 * Implementation of TS 38.212 V15.9.0 Table 5.4.2.1-1: Value of n_prb_lbrm
 * @param nof_prb Maximum number of PRBs across all configured DL BWPs and UL BWPs of a carrier for DL-SCH and UL-SCH,
 * respectively
 * @return It returns n_prb_lbrm
 */
uint32_t sch_nr_n_prb_lbrm(uint32_t nof_prb)
{
  if (nof_prb < 33) {
    return 32;
  }
  if (nof_prb <= 66) {
    return 32;
  }
  if (nof_prb <= 107) {
    return 107;
  }
  if (nof_prb <= 135) {
    return 135;
  }
  if (nof_prb <= 162) {
    return 162;
  }
  if (nof_prb <= 217) {
    return 217;
  }

  return 273;
}

#define CEIL(NUM, DEN) (((NUM) + ((DEN)-1)) / (DEN))
#define MOD(NUM, DEN) ((NUM) % (DEN))

int srslte_dlsch_nr_encode(srslte_sch_nr_encoder_t*     q,
                           const srslte_pdsch_cfg_nr_t* cfg,
                           const srslte_ra_tb_nr_t*     tb,
                           const uint8_t*               data,
                           uint8_t*                     e_bits)
{
  // Pointer protection
  if (!q || !cfg || !tb || !data || !e_bits) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  uint8_t* output_ptr = e_bits;

  // TS 38.212 V15.9.0 section 7.2.2 LDPC base graph selection
  // if A ≤ 292 , or if A ≤ 3824 and R ≤ 0.67 , or if R ≤ 0 . 25 , LDPC base graph 2 is used;
  // otherwise, LDPC base graph 1 is used,
  srslte_basegraph_t bg = BG1;
  if ((tb->tbs <= 292) || (tb->tbs <= 292 && tb->R <= 0.67) || (tb->R <= 0.25)) {
    bg = BG2;
  }

  // Compute code block segmentation
  srslte_cbsegm_t cbsegm = {};
  if (bg == BG1) {
    if (srslte_cbsegm_ldpc_bg1(&cbsegm, tb->tbs) != SRSLTE_SUCCESS) {
      ERROR("Error: calculating LDPC BG1 code block segmentation for tbs=%d\n", tb->tbs);
      return SRSLTE_ERROR;
    }
  } else {
    if (srslte_cbsegm_ldpc_bg2(&cbsegm, tb->tbs) != SRSLTE_SUCCESS) {
      ERROR("Error: calculating LDPC BG1 code block segmentation for tbs=%d\n", tb->tbs);
      return SRSLTE_ERROR;
    }
  }

  // Select encoder
  if (cbsegm.Z > MAX_LIFTSIZE) {
    ERROR("Error: lifting size Z=%d is out-of-range\n", cbsegm.Z);
    return SRSLTE_ERROR;
  }
  srslte_ldpc_encoder_t* encoder = (bg == BG1) ? q->encoder_bg1[cbsegm.Z] : q->encoder_bg2[cbsegm.Z];
  if (encoder == NULL) {
    ERROR("Error: encoder for lifting size Z=%d not found\n", cbsegm.Z);
    return SRSLTE_ERROR;
  }

  // Soft-buffer number of code-block protection
  if (tb->softbuffer.tx->max_cb < cbsegm.K1 || tb->softbuffer.tx->max_cb < cbsegm.C) {
    return SRSLTE_ERROR;
  }

  // Compute derived RM parameters
  uint32_t Qm = srslte_mod_bits_x_symbol(tb->mod);

  // Calculate Nref
  uint32_t N_re_lbrm = 156 * sch_nr_n_prb_lbrm(q->carrier.nof_prb);
  double   R_lbrm    = 948.0 / 1024.0;
  uint32_t Qm_lbrm   = (cfg->mcs_table == srslte_mcs_table_256qam) ? 8 : 6;
  uint32_t TBS_LRBM  = srslte_ra_nr_tbs(N_re_lbrm, 1.0, R_lbrm, Qm_lbrm, cfg->serving_cell_cfg.max_mimo_layers);
  uint32_t Nref      = ceil(TBS_LRBM / (cbsegm.C * 2.0 / 3.0));
  uint32_t N_L       = tb->N_L;

  // Calculate number of code blocks after applying CBGTI
  uint32_t Cp = cbsegm.C; // ... obviously, not implemented

  // Select CRC
  srslte_crc_t* crc_tb = &q->crc_tb_16;
  if (cbsegm.L_tb == 24) {
    crc_tb = &q->crc_tb_24;
  }
  uint32_t checksum_tb = srslte_crc_checksum_byte(crc_tb, data, tb->tbs);

  // Total number of bits, including CRCs
  uint32_t Bp = cbsegm.tbs + cbsegm.L_tb + cbsegm.L_cb * cbsegm.C;

  // Number of bits per code block
  uint32_t Kp = Bp / cbsegm.C;

  // For each code block...
  uint32_t j = 0;
  for (uint32_t r = 0; r < cbsegm.C; r++) {
    uint8_t* rm_buffer = tb->softbuffer.tx->buffer_b[r];
    if (!rm_buffer) {
      ERROR("Error: soft-buffer provided NULL buffer for cb_idx=%d\n", r);
      return SRSLTE_ERROR;
    }

    // If data provided, encode and store
    if (data) {
      // If it is the last segment...
      if (r == cbsegm.C - 1) {
        // Copy payload without TB CRC
        srslte_bit_unpack_vector(data, q->cb_in, (int)(Kp - cbsegm.L_cb - cbsegm.L_tb));

        // Append TB CRC
        uint8_t* ptr = &q->cb_in[Kp - cbsegm.L_cb - cbsegm.L_tb];
        srslte_bit_unpack(checksum_tb, &ptr, cbsegm.L_cb);
      } else {
        // Copy payload
        srslte_bit_unpack_vector(data, q->cb_in, (int)(Kp - cbsegm.L_cb));
      }

      // Attach code block CRC if required
      if (cbsegm.L_cb) {
        srslte_crc_attach(&q->crc_cb, q->cb_in, Kp);
      }

      // Insert filler bits
      for (uint32_t i = Kp; i < cbsegm.K1; i++) {
        q->cb_in[i] = FILLER_BIT;
      }

      // Encode code block
      srslte_ldpc_encoder_encode(encoder, q->cb_in, rm_buffer, cbsegm.K1);
    }

    // Select rate matching output sequence number of bits
    uint32_t E = 0;
    if (false) {
      // if the r -th coded block is not scheduled for transmission as indicated by CBGTI
      // ... currently not implemented
    } else {
      if (r <= (Cp - MOD(tb->nof_bits / (tb->N_L * Qm), Cp) - 1)) {
        E = N_L * Qm * (tb->nof_bits / (tb->N_L * Qm * Cp));
      } else {
        E = N_L * Qm * CEIL(tb->nof_bits, tb->N_L * Qm * Cp);
      }
      j++;
    }

    // LDPC Rate matching
    srslte_ldpc_rm_tx(&q->rm, rm_buffer, output_ptr, E, bg, cbsegm.Z, tb->rv, tb->mod, Nref);
    output_ptr += E;
  }

  return SRSLTE_SUCCESS;
}
