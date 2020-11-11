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
#include "srslte/phy/phch/ra_nr.h"
#include "srslte/phy/utils/bit.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"

srslte_basegraph_t srslte_sch_nr_select_basegraph(uint32_t tbs, double R)
{
  // if A ≤ 292 , or if A ≤ 3824 and R ≤ 0.67 , or if R ≤ 0 . 25 , LDPC base graph 2 is used;
  // otherwise, LDPC base graph 1 is used
  srslte_basegraph_t bg = BG1;
  if ((tbs <= 292) || (tbs <= 292 && R <= 0.67) || (R <= 0.25)) {
    bg = BG2;
  }

  return bg;
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

int srslte_dlsch_nr_fill_cfg(srslte_sch_nr_t*            q,
                             const srslte_sch_cfg_t*     sch_cfg,
                             const srslte_sch_tb_t*      tb,
                             srslte_sch_nr_common_cfg_t* cfg)
{

  if (!sch_cfg || !tb || !cfg) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  // LDPC base graph selection
  srslte_basegraph_t bg = srslte_sch_nr_select_basegraph(tb->tbs, tb->R);

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

  if (cbsegm.Z > MAX_LIFTSIZE) {
    ERROR("Error: lifting size Z=%d is out-of-range maximum is %d\n", cbsegm.Z, MAX_LIFTSIZE);
    return SRSLTE_ERROR;
  }

  cfg->bg   = bg;
  cfg->Qm   = srslte_mod_bits_x_symbol(tb->mod);
  cfg->A    = tb->tbs;
  cfg->L_tb = cbsegm.L_tb;
  cfg->L_cb = cbsegm.L_cb;
  cfg->B    = cbsegm.tbs + cbsegm.L_tb;
  cfg->Bp   = cfg->B + cbsegm.L_cb * cbsegm.C;
  cfg->Kp   = cfg->Bp / cbsegm.C;
  cfg->Kr   = cbsegm.K1;
  cfg->F    = cfg->Kr - cfg->Kp;
  cfg->Z    = cbsegm.Z;
  cfg->G    = tb->nof_bits;
  cfg->Nl   = tb->N_L;

  // Calculate Nref
  uint32_t N_re_lbrm = 156 * sch_nr_n_prb_lbrm(q->carrier.nof_prb);
  double   R_lbrm    = 948.0 / 1024.0;
  uint32_t Qm_lbrm   = (sch_cfg->mcs_table == srslte_mcs_table_256qam) ? 8 : 6;
  uint32_t TBS_LRBM  = srslte_ra_nr_tbs(N_re_lbrm, 1.0, R_lbrm, Qm_lbrm, sch_cfg->max_mimo_layers);
  cfg->Nref          = ceil(TBS_LRBM / (cbsegm.C * 2.0 / 3.0));

  // Calculate number of code blocks after applying CBGTI... not implemented, activate all CB
  for (uint32_t r = 0; r < cbsegm.C; r++) {
    cfg->mask[r] = true;
  }
  for (uint32_t r = cbsegm.C; r < SRSLTE_SCH_NR_MAX_NOF_CB_LDPC; r++) {
    cfg->mask[r] = false;
  }
  cfg->C  = cbsegm.C;
  cfg->Cp = cbsegm.C;

  // Select encoder
  cfg->encoder = (bg == BG1) ? q->encoder_bg1[cfg->Z] : q->encoder_bg2[cfg->Z];

  // Select decoder
  cfg->decoder = (bg == BG1) ? q->decoder_bg1[cfg->Z] : q->decoder_bg2[cfg->Z];

  // Select CRC for TB
  cfg->crc_tb = (cbsegm.L_tb == 24) ? &q->crc_tb_24 : &q->crc_tb_16;

  return SRSLTE_SUCCESS;
}

#define CEIL(NUM, DEN) (((NUM) + ((DEN)-1)) / (DEN))
#define MOD(NUM, DEN) ((NUM) % (DEN))

static inline uint32_t sch_nr_get_E(const srslte_sch_nr_common_cfg_t* cfg, uint32_t j)
{
  if (cfg->Nl == 0 || cfg->Qm == 0 || cfg->Cp == 0) {
    ERROR("Invalid Nl (%d), Qm (%d) or Cp (%d)\n", cfg->Nl, cfg->Qm, cfg->Cp);
    return 0;
  }

  if (j <= (cfg->Cp - MOD(cfg->G / (cfg->Nl * cfg->Qm), cfg->Cp) - 1)) {
    return cfg->Nl * cfg->Qm * (cfg->G / (cfg->Nl * cfg->Qm * cfg->Cp));
  }
  return cfg->Nl * cfg->Qm * CEIL(cfg->G, cfg->Nl * cfg->Qm * cfg->Cp);
}

static inline int sch_nr_init_common(srslte_sch_nr_t* q)
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

  if (!q->temp_cb) {
    q->temp_cb = srslte_vec_u8_malloc(SRSLTE_LDPC_MAX_LEN_CB * 8);
    if (!q->temp_cb) {
      return SRSLTE_ERROR;
    }
  }

  return SRSLTE_SUCCESS;
}

int srslte_sch_nr_init_tx(srslte_sch_nr_t* q)
{
  int ret = sch_nr_init_common(q);
  if (ret < SRSLTE_SUCCESS) {
    return ret;
  }

  srslte_ldpc_encoder_type_t encoder_type = SRSLTE_LDPC_ENCODER_C;

  // Iterate over all possible lifting sizes
  for (uint16_t ls = 0; ls <= MAX_LIFTSIZE; ls++) {
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

  if (srslte_ldpc_rm_tx_init(&q->tx_rm) < SRSLTE_SUCCESS) {
    ERROR("Error: initialising Tx LDPC Rate matching\n");
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

int srslte_sch_nr_init_rx(srslte_sch_nr_t* q, const srslte_sch_nr_decoder_cfg_t* decoder_cfg)
{
  int ret = sch_nr_init_common(q);
  if (ret < SRSLTE_SUCCESS) {
    return ret;
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
  for (uint16_t ls = 0; ls <= MAX_LIFTSIZE; ls++) {
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

  if (srslte_ldpc_rm_rx_init_c(&q->rx_rm) < SRSLTE_SUCCESS) {
    ERROR("Error: initialising Rx LDPC Rate matching\n");
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

int srslte_sch_nr_set_carrier(srslte_sch_nr_t* q, const srslte_carrier_nr_t* carrier)
{
  if (!q) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  q->carrier = *carrier;

  return SRSLTE_SUCCESS;
}

void srslte_sch_nr_free(srslte_sch_nr_t* q)
{
  // Protect pointer
  if (!q) {
    return;
  }

  if (q->temp_cb) {
    free(q->temp_cb);
  }

  for (uint16_t ls = 0; ls <= MAX_LIFTSIZE; ls++) {
    if (q->encoder_bg1[ls]) {
      srslte_ldpc_encoder_free(q->encoder_bg1[ls]);
      free(q->encoder_bg1[ls]);
    }
    if (q->encoder_bg2[ls]) {
      srslte_ldpc_encoder_free(q->encoder_bg2[ls]);
      free(q->encoder_bg2[ls]);
    }
    if (q->decoder_bg1[ls]) {
      srslte_ldpc_decoder_free(q->decoder_bg1[ls]);
      free(q->decoder_bg1[ls]);
    }
    if (q->decoder_bg2[ls]) {
      srslte_ldpc_decoder_free(q->decoder_bg2[ls]);
      free(q->decoder_bg2[ls]);
    }
  }

  srslte_ldpc_rm_tx_free(&q->tx_rm);
  srslte_ldpc_rm_rx_free_c(&q->rx_rm);
}

int srslte_dlsch_nr_encode(srslte_sch_nr_t*        q,
                           const srslte_sch_cfg_t* pdsch_cfg,
                           const srslte_sch_tb_t*  tb,
                           const uint8_t*          data,
                           uint8_t*                e_bits)
{
  // Pointer protection
  if (!q || !pdsch_cfg || !tb || !data || !e_bits) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  const uint8_t* input_ptr  = data;
  uint8_t*       output_ptr = e_bits;

  srslte_sch_nr_common_cfg_t cfg = {};
  if (srslte_dlsch_nr_fill_cfg(q, pdsch_cfg, tb, &cfg) < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  // Check encoder
  if (cfg.encoder == NULL) {
    ERROR("Error: encoder for lifting size Z=%d not found\n", cfg.Z);
    return SRSLTE_ERROR;
  }

  // Check CRC for TB
  if (cfg.crc_tb == NULL) {
    ERROR("Error: CRC for TB not found\n");
    return SRSLTE_ERROR;
  }

  // Soft-buffer number of code-block protection
  if (tb->softbuffer.tx->max_cb < cfg.C) {
    ERROR("Soft-buffer does not have enough code-blocks (max_cb=%d) for a TBS=%d, C=%d.\n",
          tb->softbuffer.tx->max_cb,
          tb->tbs,
          cfg.C);
    return SRSLTE_ERROR;
  }

  if (tb->softbuffer.tx->max_cb_size < (cfg.encoder->liftN - 2 * cfg.Z)) {
    ERROR("Soft-buffer code-block maximum size insufficient (max_cb_size=%d) for a TBS=%d, requires %d.\n",
          tb->softbuffer.tx->max_cb_size,
          tb->tbs,
          (cfg.encoder->liftN - 2 * cfg.Z));
    return SRSLTE_ERROR;
  }

  // Calculate TB CRC
  uint32_t checksum_tb = srslte_crc_checksum_byte(cfg.crc_tb, data, tb->tbs);
  if (SRSLTE_DEBUG_ENABLED && srslte_verbose >= SRSLTE_VERBOSE_INFO && !handler_registered) {
    printf("Encode: ");
    srslte_vec_fprint_byte(stdout, data, tb->tbs / 8);
  }

  // For each code block...
  uint32_t j = 0;
  for (uint32_t r = 0; r < cfg.C; r++) {
    // Select rate matching circular buffer
    uint8_t* rm_buffer = tb->softbuffer.tx->buffer_b[r];
    if (rm_buffer == NULL) {
      ERROR("Error: soft-buffer provided NULL buffer for cb_idx=%d\n", r);
      return SRSLTE_ERROR;
    }

    // If data provided, encode and store in RM circular buffer
    if (data != NULL) {
      uint32_t cb_len = cfg.Kp - cfg.L_cb;

      // If it is the last segment...
      if (r == cfg.C - 1) {
        cb_len -= cfg.L_tb;

        // Copy payload without TB CRC
        srslte_bit_unpack_vector(input_ptr, q->temp_cb, (int)cb_len);

        // Append TB CRC
        uint8_t* ptr = &q->temp_cb[cb_len];
        srslte_bit_unpack(checksum_tb, &ptr, cfg.L_tb);
        INFO("CB %d: appending TB CRC=%06x\n", r, checksum_tb);
      } else {
        // Copy payload
        srslte_bit_unpack_vector(input_ptr, q->temp_cb, (int)cb_len);
      }

      if (SRSLTE_DEBUG_ENABLED && srslte_verbose >= SRSLTE_VERBOSE_INFO && !handler_registered) {
        printf("CB %d:", r);
        srslte_vec_fprint_byte(stdout, input_ptr, cb_len / 8);
      }

      input_ptr += cb_len / 8;

      // Attach code block CRC if required
      if (cfg.L_cb) {
        srslte_crc_attach(&q->crc_cb, q->temp_cb, (int)(cfg.Kp - cfg.L_cb));
        INFO("CB %d: CRC=%06x\n", r, (uint32_t)srslte_crc_checksum_get(&q->crc_cb));
      }

      // Insert filler bits
      for (uint32_t i = cfg.Kp; i < cfg.Kr; i++) {
        q->temp_cb[i] = FILLER_BIT;
      }

      // Encode code block
      srslte_ldpc_encoder_encode(cfg.encoder, q->temp_cb, rm_buffer, cfg.Kr);
    }

    // Skip block
    if (!cfg.mask[r]) {
      continue;
    }

    // Select rate matching output sequence number of bits
    uint32_t E = sch_nr_get_E(&cfg, j);
    j++;

    // LDPC Rate matching
    INFO("RM CB %d: E=%d; F=%d; BG=%d; Z=%d; RV=%d; Qm=%d; Nref=%d;\n",
         r,
         E,
         cfg.F,
         cfg.bg == BG1 ? 1 : 2,
         cfg.Z,
         tb->rv,
         cfg.Qm,
         cfg.Nref);
    srslte_ldpc_rm_tx(&q->tx_rm, rm_buffer, output_ptr, E, cfg.bg, cfg.Z, tb->rv, tb->mod, cfg.Nref);
    output_ptr += E;
  }

  return SRSLTE_SUCCESS;
}

int srslte_dlsch_nr_decode(srslte_sch_nr_t*        q,
                           const srslte_sch_cfg_t* sch_cfg,
                           const srslte_sch_tb_t*  tb,
                           int8_t*                 e_bits,
                           uint8_t*                data,
                           bool*                   crc_ok)
{
  // Pointer protection
  if (!q || !sch_cfg || !tb || !data || !e_bits || !crc_ok) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  int8_t* input_ptr = e_bits;

  srslte_sch_nr_common_cfg_t cfg = {};
  if (srslte_dlsch_nr_fill_cfg(q, sch_cfg, tb, &cfg) < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  // Check decoder
  if (cfg.decoder == NULL) {
    ERROR("Error: decoder for lifting size Z=%d not found\n", cfg.Z);
    return SRSLTE_ERROR;
  }

  // Check CRC for TB
  if (cfg.crc_tb == NULL) {
    ERROR("Error: CRC for TB not found\n");
    return SRSLTE_ERROR;
  }

  // Soft-buffer number of code-block protection
  if (tb->softbuffer.rx->max_cb < cfg.Cp || tb->softbuffer.rx->max_cb_size < (cfg.decoder->liftN - 2 * cfg.Z)) {
    return SRSLTE_ERROR;
  }

  // Counter of code blocks that have matched CRC
  uint32_t cb_ok = 0;

  // For each code block...
  uint32_t j = 0;
  for (uint32_t r = 0; r < cfg.C; r++) {
    bool    decoded   = tb->softbuffer.rx->cb_crc[r];
    int8_t* rm_buffer = (int8_t*)tb->softbuffer.tx->buffer_b[r];
    if (!rm_buffer) {
      ERROR("Error: soft-buffer provided NULL buffer for cb_idx=%d\n", r);
      return SRSLTE_ERROR;
    }

    // Skip CB if mask indicates no transmission of the CB
    if (!cfg.mask[r]) {
      if (decoded) {
        cb_ok++;
      }
      INFO("RM CB %d: Disabled, CRC %s ... Skipping\n", r, decoded ? "OK" : "KO");
      continue;
    }

    // Select rate matching output sequence number of bits
    uint32_t E = sch_nr_get_E(&cfg, j);
    j++;

    // Skip CB if it has a matched CRC
    if (decoded) {
      INFO("RM CB %d: CRC OK ... Skipping\n", r);
      cb_ok++;
      continue;
    }

    // LDPC Rate matching
    INFO("RM CB %d: E=%d; F=%d; BG=%d; Z=%d; RV=%d; Qm=%d; Nref=%d;\n",
         r,
         E,
         cfg.F,
         cfg.bg == BG1 ? 1 : 2,
         cfg.Z,
         tb->rv,
         cfg.Qm,
         cfg.Nref);
    srslte_ldpc_rm_rx_c(&q->rx_rm, input_ptr, rm_buffer, E, cfg.F, cfg.bg, cfg.Z, tb->rv, tb->mod, cfg.Nref);

    // Decode
    srslte_ldpc_decoder_decode_c(cfg.decoder, rm_buffer, q->temp_cb);

    // Compute CB CRC
    uint32_t cb_len = cfg.Kp - cfg.L_cb;
    if (cfg.L_cb) {
      uint8_t* ptr                 = q->temp_cb + cb_len;
      uint32_t checksum1           = srslte_crc_checksum(&q->crc_cb, q->temp_cb, (int)cb_len);
      uint32_t checksum2           = srslte_bit_pack(&ptr, cfg.L_cb);
      tb->softbuffer.rx->cb_crc[r] = (checksum1 == checksum2);

      INFO("CB %d/%d: CRC={%06x, %06x} ... %s\n",
           r,
           cfg.C,
           checksum1,
           checksum2,
           tb->softbuffer.rx->cb_crc[r] ? "OK" : "KO");
    } else {
      tb->softbuffer.rx->cb_crc[r] = true;
    }

    // Pack and count CRC OK only if CRC is match
    if (tb->softbuffer.rx->cb_crc[r]) {
      srslte_bit_pack_vector(q->temp_cb, tb->softbuffer.rx->data[r], cb_len);
      cb_ok++;
    }

    input_ptr += E;
  }

  // All CB are decoded
  if (cb_ok == cfg.C) {
    uint32_t checksum2  = 0;
    uint8_t* output_ptr = data;

    for (uint32_t r = 0; r < cfg.C; r++) {
      uint32_t cb_len = cfg.Kp - cfg.L_cb;

      // Subtract TB CRC from the last code block
      if (r == cfg.C - 1) {
        cb_len -= cfg.L_tb;
      }

      srslte_vec_u8_copy(output_ptr, tb->softbuffer.rx->data[r], cb_len / 8);
      output_ptr += cb_len / 8;

      if (SRSLTE_DEBUG_ENABLED && srslte_verbose >= SRSLTE_VERBOSE_INFO && !handler_registered) {
        printf("CB %d:", r);
        srslte_vec_fprint_byte(stdout, tb->softbuffer.rx->data[r], cb_len / 8);
      }

      if (r == cfg.C - 1) {
        uint8_t  tb_crc_unpacked[24] = {};
        uint8_t* tb_crc_unpacked_ptr = tb_crc_unpacked;
        srslte_bit_unpack_vector(&tb->softbuffer.rx->data[r][cb_len / 8], tb_crc_unpacked, cfg.L_tb);
        checksum2 = srslte_bit_pack(&tb_crc_unpacked_ptr, cfg.L_tb);
      }
    }

    // Calculate TB CRC from packed data
    uint32_t checksum1 = srslte_crc_checksum_byte(cfg.crc_tb, data, tb->tbs);
    *crc_ok            = (checksum1 == checksum2);

    INFO("TB: TBS=%d; CRC={%06x, %06x}\n", tb->tbs, checksum1, checksum2);
    if (SRSLTE_DEBUG_ENABLED && srslte_verbose >= SRSLTE_VERBOSE_INFO && !handler_registered) {
      printf("Decode: ");
      srslte_vec_fprint_byte(stdout, data, tb->tbs / 8);
    }
  } else {
    *crc_ok = false;
  }

  return SRSLTE_SUCCESS;
}
