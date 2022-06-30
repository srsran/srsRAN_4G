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

#include "srsran/phy/phch/sch_nr.h"
#include "srsran/config.h"
#include "srsran/phy/fec/cbsegm.h"
#include "srsran/phy/fec/ldpc/ldpc_common.h"
#include "srsran/phy/fec/ldpc/ldpc_rm.h"
#include "srsran/phy/phch/ra_nr.h"
#include "srsran/phy/utils/bit.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/vector.h"

#define SCH_INFO_TX(...) INFO("SCH Tx: " __VA_ARGS__)
#define SCH_INFO_RX(...) INFO("SCH Rx: " __VA_ARGS__)

srsran_basegraph_t srsran_sch_nr_select_basegraph(uint32_t tbs, double R)
{
  // if A ≤ 292 , or if A ≤ 3824 and R ≤ 0.67 , or if R ≤ 0 . 25 , LDPC base graph 2 is used;
  // otherwise, LDPC base graph 1 is used
  srsran_basegraph_t bg = BG1;
  if ((tbs <= 292) || (tbs <= 3824 && R <= 0.67) || (R <= 0.25)) {
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

static int sch_nr_cbsegm(srsran_basegraph_t bg, uint32_t tbs, srsran_cbsegm_t* cbsegm)
{
  if (bg == BG1) {
    if (srsran_cbsegm_ldpc_bg1(cbsegm, tbs) != SRSRAN_SUCCESS) {
      ERROR("Error: calculating LDPC BG1 code block segmentation for tbs=%d", tbs);
      return SRSRAN_ERROR;
    }
  } else {
    if (srsran_cbsegm_ldpc_bg2(cbsegm, tbs) != SRSRAN_SUCCESS) {
      ERROR("Error: calculating LDPC BG1 code block segmentation for tbs=%d", tbs);
      return SRSRAN_ERROR;
    }
  }

  return SRSRAN_SUCCESS;
}

static int sch_nr_Nref(uint32_t N_rb, srsran_mcs_table_t mcs_table, uint32_t max_mimo_layers)
{
  uint32_t           N_re_lbrm = SRSRAN_MAX_NRE_NR * sch_nr_n_prb_lbrm(N_rb);
  double             TCR_lbrm  = 948.0 / 1024.0;
  uint32_t           Qm_lbrm   = (mcs_table == srsran_mcs_table_256qam) ? 8 : 6;
  uint32_t           TBS_LRBM  = srsran_ra_nr_tbs(N_re_lbrm, 1.0, TCR_lbrm, Qm_lbrm, SRSRAN_MIN(4, max_mimo_layers));
  double             R         = 2.0 / 3.0;
  srsran_basegraph_t bg        = srsran_sch_nr_select_basegraph(TBS_LRBM, R);

  // Compute segmentation
  srsran_cbsegm_t cbsegm = {};
  int             r      = sch_nr_cbsegm(bg, TBS_LRBM, &cbsegm);
  if (r < SRSRAN_SUCCESS) {
    ERROR("Error computing TB segmentation");
    return SRSRAN_ERROR;
  }

  return (int)ceil((double)TBS_LRBM / (double)(cbsegm.C * R));
}

int srsran_sch_nr_fill_tb_info(const srsran_carrier_nr_t* carrier,
                               const srsran_sch_cfg_t*    sch_cfg,
                               const srsran_sch_tb_t*     tb,
                               srsran_sch_nr_tb_info_t*   cfg)
{
  if (!sch_cfg || !tb || !cfg) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // LDPC base graph selection
  srsran_basegraph_t bg = srsran_sch_nr_select_basegraph(tb->tbs, tb->R);

  // Compute code block segmentation
  srsran_cbsegm_t cbsegm = {};
  if (sch_nr_cbsegm(bg, tb->tbs, &cbsegm) < SRSRAN_SUCCESS) {
    ERROR("Error calculation TB segmentation");
    return SRSRAN_ERROR;
  }

  if (cbsegm.Z > MAX_LIFTSIZE) {
    ERROR("Error: lifting size Z=%d is out-of-range maximum is %d", cbsegm.Z, MAX_LIFTSIZE);
    return SRSRAN_ERROR;
  }

  cfg->bg   = bg;
  cfg->Qm   = srsran_mod_bits_x_symbol(tb->mod);
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
  if (sch_cfg->limited_buffer_rm) {
    int Nref = sch_nr_Nref(carrier->nof_prb, sch_cfg->mcs_table, 4);
    if (Nref < SRSRAN_SUCCESS) {
      ERROR("Error computing N_ref");
    }
    cfg->Nref = (uint32_t)Nref;
  } else {
    cfg->Nref = SRSRAN_LDPC_MAX_LEN_ENCODED_CB;
  }

  // Calculate number of code blocks after applying CBGTI... not implemented, activate all CB
  for (uint32_t r = 0; r < cbsegm.C; r++) {
    cfg->mask[r] = true;
  }
  for (uint32_t r = cbsegm.C; r < SRSRAN_SCH_NR_MAX_NOF_CB_LDPC; r++) {
    cfg->mask[r] = false;
  }
  cfg->C  = cbsegm.C;
  cfg->Cp = cbsegm.C;

  return SRSRAN_SUCCESS;
}

#define MOD(NUM, DEN) ((NUM) % (DEN))

static inline uint32_t sch_nr_get_E(const srsran_sch_nr_tb_info_t* cfg, uint32_t j)
{
  if (cfg->Nl == 0 || cfg->Qm == 0 || cfg->Cp == 0) {
    ERROR("Invalid Nl (%d), Qm (%d) or Cp (%d)", cfg->Nl, cfg->Qm, cfg->Cp);
    return 0;
  }

  if (j <= (cfg->Cp - MOD(cfg->G / (cfg->Nl * cfg->Qm), cfg->Cp) - 1)) {
    return cfg->Nl * cfg->Qm * (cfg->G / (cfg->Nl * cfg->Qm * cfg->Cp));
  }
  return cfg->Nl * cfg->Qm * SRSRAN_CEIL(cfg->G, cfg->Nl * cfg->Qm * cfg->Cp);
}

static inline int sch_nr_init_common(srsran_sch_nr_t* q)
{
  if (q == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  if (srsran_crc_init(&q->crc_tb_24, SRSRAN_LTE_CRC24A, 24) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  if (srsran_crc_init(&q->crc_cb, SRSRAN_LTE_CRC24B, 24) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  if (srsran_crc_init(&q->crc_tb_16, SRSRAN_LTE_CRC16, 16) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  if (!q->temp_cb) {
    q->temp_cb = srsran_vec_u8_malloc(SRSRAN_LDPC_MAX_LEN_CB * 8);
    if (!q->temp_cb) {
      return SRSRAN_ERROR;
    }
  }

  return SRSRAN_SUCCESS;
}

int srsran_sch_nr_init_tx(srsran_sch_nr_t* q, const srsran_sch_nr_args_t* args)
{
  int ret = sch_nr_init_common(q);
  if (ret < SRSRAN_SUCCESS) {
    return ret;
  }

  srsran_ldpc_encoder_type_t encoder_type = SRSRAN_LDPC_ENCODER_C;

#ifdef LV_HAVE_AVX512
  if (!args->disable_simd) {
    encoder_type = SRSRAN_LDPC_ENCODER_AVX512;
  }
#else // LV_HAVE_AVX512
#ifdef LV_HAVE_AVX2
  if (!args->disable_simd) {
    encoder_type = SRSRAN_LDPC_ENCODER_AVX2;
  }
#endif // LV_HAVE_AVX2
#endif // LV_HAVE_AVX612

  // Iterate over all possible lifting sizes
  for (uint16_t ls = 0; ls <= MAX_LIFTSIZE; ls++) {
    uint8_t ls_index = get_ls_index(ls);

    // Invalid lifting size
    if (ls_index == VOID_LIFTSIZE) {
      q->encoder_bg1[ls] = NULL;
      q->encoder_bg2[ls] = NULL;
      continue;
    }

    q->encoder_bg1[ls] = SRSRAN_MEM_ALLOC(srsran_ldpc_encoder_t, 1);
    if (!q->encoder_bg1[ls]) {
      ERROR("Error: calloc");
      return SRSRAN_ERROR;
    }
    SRSRAN_MEM_ZERO(q->encoder_bg1[ls], srsran_ldpc_encoder_t, 1);

    if (srsran_ldpc_encoder_init(q->encoder_bg1[ls], encoder_type, BG1, ls) < SRSRAN_SUCCESS) {
      ERROR("Error: initialising BG1 LDPC encoder for ls=%d", ls);
      return SRSRAN_ERROR;
    }

    q->encoder_bg2[ls] = SRSRAN_MEM_ALLOC(srsran_ldpc_encoder_t, 1);
    if (!q->encoder_bg2[ls]) {
      return SRSRAN_ERROR;
    }
    SRSRAN_MEM_ZERO(q->encoder_bg2[ls], srsran_ldpc_encoder_t, 1);

    if (srsran_ldpc_encoder_init(q->encoder_bg2[ls], encoder_type, BG2, ls) < SRSRAN_SUCCESS) {
      ERROR("Error: initialising BG2 LDPC encoder for ls=%d", ls);
      return SRSRAN_ERROR;
    }
  }

  if (srsran_ldpc_rm_tx_init(&q->tx_rm) < SRSRAN_SUCCESS) {
    ERROR("Error: initialising Tx LDPC Rate matching");
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

int srsran_sch_nr_init_rx(srsran_sch_nr_t* q, const srsran_sch_nr_args_t* args)
{
  int ret = sch_nr_init_common(q);
  if (ret < SRSRAN_SUCCESS) {
    return ret;
  }

  srsran_ldpc_decoder_type_t decoder_type =
      args->decoder_use_flooded ? SRSRAN_LDPC_DECODER_C_FLOOD : SRSRAN_LDPC_DECODER_C;

#ifdef LV_HAVE_AVX512
  if (!args->disable_simd) {
    decoder_type = args->decoder_use_flooded ? SRSRAN_LDPC_DECODER_C_AVX512_FLOOD : SRSRAN_LDPC_DECODER_C_AVX512;
  }
#else // LV_HAVE_AVX512
#ifdef LV_HAVE_AVX2
  if (!args->disable_simd) {
    decoder_type = args->decoder_use_flooded ? SRSRAN_LDPC_DECODER_C_AVX2_FLOOD : SRSRAN_LDPC_DECODER_C_AVX2;
  }
#endif // LV_HAVE_AVX2
#endif // LV_HAVE_AVX512

  // If the scaling factor is not provided use a default value that allows decoding all possible combinations of nPRB
  // and MCS indexes for all possible MCS tables
  float scaling_factor = isnormal(args->decoder_scaling_factor) ? args->decoder_scaling_factor : 0.8f;

  // Iterate over all possible lifting sizes
  for (uint16_t ls = 0; ls <= MAX_LIFTSIZE; ls++) {
    uint8_t ls_index = get_ls_index(ls);

    // Invalid lifting size
    if (ls_index == VOID_LIFTSIZE) {
      q->decoder_bg1[ls] = NULL;
      q->decoder_bg2[ls] = NULL;
      continue;
    }

    // Initialise LDPC configuration arguments
    srsran_ldpc_decoder_args_t decoder_args = {};
    decoder_args.type                       = decoder_type;
    decoder_args.ls                         = ls;
    decoder_args.scaling_fctr               = scaling_factor;
    decoder_args.max_nof_iter               = args->max_nof_iter;

    q->decoder_bg1[ls] = SRSRAN_MEM_ALLOC(srsran_ldpc_decoder_t, 1);
    if (!q->decoder_bg1[ls]) {
      ERROR("Error: calloc");
      return SRSRAN_ERROR;
    }
    SRSRAN_MEM_ZERO(q->decoder_bg1[ls], srsran_ldpc_decoder_t, 1);

    decoder_args.bg = BG1;
    if (srsran_ldpc_decoder_init(q->decoder_bg1[ls], &decoder_args) < SRSRAN_SUCCESS) {
      ERROR("Error: initialising BG1 LDPC decoder for ls=%d", ls);
      return SRSRAN_ERROR;
    }

    q->decoder_bg2[ls] = SRSRAN_MEM_ALLOC(srsran_ldpc_decoder_t, 1);
    if (!q->decoder_bg2[ls]) {
      ERROR("Error: calloc");
      return SRSRAN_ERROR;
    }
    SRSRAN_MEM_ZERO(q->decoder_bg2[ls], srsran_ldpc_decoder_t, 1);

    decoder_args.bg = BG2;
    if (srsran_ldpc_decoder_init(q->decoder_bg2[ls], &decoder_args) < SRSRAN_SUCCESS) {
      ERROR("Error: initialising BG2 LDPC decoder for ls=%d", ls);
      return SRSRAN_ERROR;
    }
  }

  if (srsran_ldpc_rm_rx_init_c(&q->rx_rm) < SRSRAN_SUCCESS) {
    ERROR("Error: initialising Rx LDPC Rate matching");
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

int srsran_sch_nr_set_carrier(srsran_sch_nr_t* q, const srsran_carrier_nr_t* carrier)
{
  if (!q) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  q->carrier = *carrier;

  return SRSRAN_SUCCESS;
}

void srsran_sch_nr_free(srsran_sch_nr_t* q)
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
      srsran_ldpc_encoder_free(q->encoder_bg1[ls]);
      free(q->encoder_bg1[ls]);
    }
    if (q->encoder_bg2[ls]) {
      srsran_ldpc_encoder_free(q->encoder_bg2[ls]);
      free(q->encoder_bg2[ls]);
    }
    if (q->decoder_bg1[ls]) {
      srsran_ldpc_decoder_free(q->decoder_bg1[ls]);
      free(q->decoder_bg1[ls]);
    }
    if (q->decoder_bg2[ls]) {
      srsran_ldpc_decoder_free(q->decoder_bg2[ls]);
      free(q->decoder_bg2[ls]);
    }
  }

  srsran_ldpc_rm_tx_free(&q->tx_rm);
  srsran_ldpc_rm_rx_free_c(&q->rx_rm);
}

static inline int sch_nr_encode(srsran_sch_nr_t*        q,
                                const srsran_sch_cfg_t* sch_cfg,
                                const srsran_sch_tb_t*  tb,
                                const uint8_t*          data,
                                uint8_t*                e_bits)
{
  // Pointer protection
  if (!q || !sch_cfg || !tb || !data || !e_bits) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  if (!tb->softbuffer.tx) {
    ERROR("Error: Missing Tx softbuffer");
    return SRSRAN_ERROR;
  }

  const uint8_t* input_ptr  = data;
  uint8_t*       output_ptr = e_bits;

  srsran_sch_nr_tb_info_t cfg = {};
  if (srsran_sch_nr_fill_tb_info(&q->carrier, sch_cfg, tb, &cfg) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  // Select encoder and CRC
  srsran_ldpc_encoder_t* encoder = (cfg.bg == BG1) ? q->encoder_bg1[cfg.Z] : q->encoder_bg2[cfg.Z];
  srsran_crc_t*          crc_tb  = (cfg.L_tb == 24) ? &q->crc_tb_24 : &q->crc_tb_16;

  // Check encoder
  if (encoder == NULL) {
    ERROR("Error: encoder for lifting size Z=%d not found (tbs=%d)", cfg.Z, tb->tbs);
    return SRSRAN_ERROR;
  }

  // Check CRC for TB
  if (crc_tb == NULL) {
    ERROR("Error: CRC for TB not found");
    return SRSRAN_ERROR;
  }

  // Soft-buffer number of code-block protection
  if (tb->softbuffer.tx->max_cb < cfg.C) {
    ERROR("Soft-buffer does not have enough code-blocks (max_cb=%d) for a TBS=%d, C=%d.",
          tb->softbuffer.tx->max_cb,
          tb->tbs,
          cfg.C);
    return SRSRAN_ERROR;
  }

  if (tb->softbuffer.tx->max_cb_size < (encoder->liftN - 2 * cfg.Z)) {
    ERROR("Soft-buffer code-block maximum size insufficient (max_cb_size=%d) for a TBS=%d, requires %d.",
          tb->softbuffer.tx->max_cb_size,
          tb->tbs,
          (encoder->liftN - 2 * cfg.Z));
    return SRSRAN_ERROR;
  }

  // Calculate TB CRC
  uint32_t checksum_tb = srsran_crc_checksum_byte(crc_tb, data, tb->tbs);
  if (SRSRAN_DEBUG_ENABLED && get_srsran_verbose_level() >= SRSRAN_VERBOSE_DEBUG && !is_handler_registered()) {
    DEBUG("tb=");
    srsran_vec_fprint_byte(stdout, data, tb->tbs / 8);
  }

  // For each code block...
  uint32_t j = 0;
  for (uint32_t r = 0; r < cfg.C; r++) {
    // Select rate matching circular buffer
    uint8_t* rm_buffer = tb->softbuffer.tx->buffer_b[r];
    if (rm_buffer == NULL) {
      ERROR("Error: soft-buffer provided NULL buffer for cb_idx=%d", r);
      return SRSRAN_ERROR;
    }

    // If data provided, encode and store in RM circular buffer
    if (data != NULL) {
      uint32_t cb_len = cfg.Kp - cfg.L_cb;

      // If it is the last segment...
      if (r == cfg.C - 1) {
        cb_len -= cfg.L_tb;

        // Copy payload without TB CRC
        srsran_bit_unpack_vector(input_ptr, q->temp_cb, (int)cb_len);

        // Append TB CRC
        uint8_t* ptr = &q->temp_cb[cb_len];
        srsran_bit_unpack(checksum_tb, &ptr, cfg.L_tb);
        SCH_INFO_TX("CB %d: appending TB CRC=%06x", r, checksum_tb);
      } else {
        // Copy payload
        srsran_bit_unpack_vector(input_ptr, q->temp_cb, (int)cb_len);
      }

      if (SRSRAN_DEBUG_ENABLED && get_srsran_verbose_level() >= SRSRAN_VERBOSE_DEBUG && !is_handler_registered()) {
        DEBUG("cb%d=", r);
        srsran_vec_fprint_byte(stdout, input_ptr, cb_len / 8);
      }

      input_ptr += cb_len / 8;

      // Attach code block CRC if required
      if (cfg.L_cb) {
        srsran_crc_attach(&q->crc_cb, q->temp_cb, (int)(cfg.Kp - cfg.L_cb));
        SCH_INFO_TX("CB %d: CRC=%06x", r, (uint32_t)srsran_crc_checksum_get(&q->crc_cb));
      }

      // Insert filler bits
      for (uint32_t i = cfg.Kp; i < cfg.Kr; i++) {
        q->temp_cb[i] = FILLER_BIT;
      }

      // Encode code block
      srsran_ldpc_encoder_encode(encoder, q->temp_cb, rm_buffer, cfg.Kr);

      if (SRSRAN_DEBUG_ENABLED && get_srsran_verbose_level() >= SRSRAN_VERBOSE_DEBUG && !is_handler_registered()) {
        DEBUG("encoded=");
        srsran_vec_fprint_b(stdout, rm_buffer, encoder->liftN - 2 * encoder->ls);
      }
    }

    // Skip block
    if (!cfg.mask[r]) {
      continue;
    }

    // Select rate matching output sequence number of bits
    uint32_t E = sch_nr_get_E(&cfg, j);
    j++;

    // LDPC Rate matching
    SCH_INFO_TX("RM CB %d: E=%d; F=%d; BG=%d; Z=%d; RV=%d; Qm=%d; Nref=%d;",
                r,
                E,
                cfg.F,
                cfg.bg == BG1 ? 1 : 2,
                cfg.Z,
                tb->rv,
                cfg.Qm,
                cfg.Nref);
    srsran_ldpc_rm_tx(&q->tx_rm, rm_buffer, output_ptr, E, cfg.bg, cfg.Z, tb->rv, tb->mod, cfg.Nref);
    output_ptr += E;
  }

  return SRSRAN_SUCCESS;
}

static int sch_nr_decode(srsran_sch_nr_t*        q,
                         const srsran_sch_cfg_t* sch_cfg,
                         const srsran_sch_tb_t*  tb,
                         int8_t*                 e_bits,
                         srsran_sch_tb_res_nr_t* res)
{
  // Pointer protection
  if (!q || !sch_cfg || !tb || !e_bits || !res) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Protect softbuffer access
  if (!tb->softbuffer.rx) {
    ERROR("Missing softbuffer!");
    return SRSRAN_ERROR;
  }

  // Protect PDU access
  if (!res->payload) {
    ERROR("Missing payload pointer!");
    return SRSRAN_ERROR;
  }

  int8_t*  input_ptr    = e_bits;
  uint32_t nof_iter_sum = 0;

  srsran_sch_nr_tb_info_t cfg = {};
  if (srsran_sch_nr_fill_tb_info(&q->carrier, sch_cfg, tb, &cfg) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  // Select encoder and CRC
  srsran_ldpc_decoder_t* decoder = (cfg.bg == BG1) ? q->decoder_bg1[cfg.Z] : q->decoder_bg2[cfg.Z];
  srsran_crc_t*          crc_tb  = (cfg.L_tb == 24) ? &q->crc_tb_24 : &q->crc_tb_16;

  // Check decoder
  if (decoder == NULL) {
    ERROR("Error: decoder for lifting size Z=%d not found", cfg.Z);
    return SRSRAN_ERROR;
  }

  // Check CRC for TB
  if (crc_tb == NULL) {
    ERROR("Error: CRC for TB not found");
    return SRSRAN_ERROR;
  }

  // Soft-buffer number of code-block protection
  if (tb->softbuffer.rx->max_cb < cfg.Cp || tb->softbuffer.rx->max_cb_size < (decoder->liftN - 2 * cfg.Z)) {
    return SRSRAN_ERROR;
  }

  // Counter of code blocks that have matched CRC
  uint32_t cb_ok = 0;
  res->crc       = false;

  // For each code block...
  uint32_t j = 0;
  for (uint32_t r = 0; r < cfg.C; r++) {
    bool    decoded   = tb->softbuffer.rx->cb_crc[r];
    int8_t* rm_buffer = (int8_t*)tb->softbuffer.tx->buffer_b[r];
    if (!rm_buffer) {
      ERROR("Error: soft-buffer provided NULL buffer for cb_idx=%d", r);
      return SRSRAN_ERROR;
    }

    // Skip CB if mask indicates no transmission of the CB
    if (!cfg.mask[r]) {
      if (decoded) {
        cb_ok++;
      }
      SCH_INFO_RX("RM CB %d: Disabled, CRC %s ... Skipping", r, decoded ? "OK" : "KO");
      continue;
    }

    // Select rate matching output sequence number of bits
    uint32_t E = sch_nr_get_E(&cfg, j);
    j++;

    // Skip CB if it has a matched CRC
    if (decoded) {
      SCH_INFO_RX("RM CB %d: CRC OK ... Skipping", r);
      cb_ok++;
      continue;
    }

    // LDPC Rate matching
    SCH_INFO_RX("RM CB %d: E=%d; F=%d; BG=%d; Z=%d; RV=%d; Qm=%d; Nref=%d;",
                r,
                E,
                cfg.F,
                cfg.bg == BG1 ? 1 : 2,
                cfg.Z,
                tb->rv,
                cfg.Qm,
                cfg.Nref);
    int n_llr =
        srsran_ldpc_rm_rx_c(&q->rx_rm, input_ptr, rm_buffer, E, cfg.F, cfg.bg, cfg.Z, tb->rv, tb->mod, cfg.Nref);
    if (n_llr < SRSRAN_SUCCESS) {
      ERROR("Error in LDPC rate mateching");
      return SRSRAN_ERROR;
    }

    // Select CB or TB early stop CRC
    srsran_crc_t* crc = (cfg.L_tb == 16) ? &q->crc_tb_16 : &q->crc_tb_24;
    if (cfg.L_cb) {
      crc = &q->crc_cb;
    }

    // Decode. if CRC=KO, then ret=0
    int ret = srsran_ldpc_decoder_decode_crc_c(decoder, rm_buffer, q->temp_cb, n_llr, crc);
    if (ret < SRSRAN_SUCCESS) {
      ERROR("Error decoding CB");
      return SRSRAN_ERROR;
    }

    // Compute number of iterations
    uint32_t n_iter_cb = (ret == 0) ? decoder->max_nof_iter : (uint32_t)ret;
    nof_iter_sum += n_iter_cb;

    // Check if CB is all zeros
    uint32_t cb_len = cfg.Kp - cfg.L_cb;

    tb->softbuffer.rx->cb_crc[r] = (ret != 0);
    SCH_INFO_RX("CB %d/%d iter=%d CRC=%s", r, cfg.C, n_iter_cb, tb->softbuffer.rx->cb_crc[r] ? "OK" : "KO");

    // CB Debug trace
    if (SRSRAN_DEBUG_ENABLED && get_srsran_verbose_level() >= SRSRAN_VERBOSE_DEBUG && !is_handler_registered()) {
      DEBUG("CB %d/%d:", r, cfg.C);
      srsran_vec_fprint_hex(stdout, q->temp_cb, cb_len);
    }

    // Pack and count CRC OK only if CRC is match
    if (tb->softbuffer.rx->cb_crc[r]) {
      srsran_bit_pack_vector(q->temp_cb, tb->softbuffer.rx->data[r], cb_len);
      cb_ok++;
    }

    input_ptr += E;
  }
  // Set average number of iterations
  res->avg_iter = (float)nof_iter_sum / (float)cfg.C;

  // Set average number of iterations
  if (cfg.C > 0) {
    res->avg_iter = (float)nof_iter_sum / (float)cfg.C;
  } else {
    res->avg_iter = NAN;
  }

  // Not all CB are decoded, skip TB union and CRC check
  if (cb_ok != cfg.C) {
    return SRSRAN_SUCCESS;
  }

  uint32_t checksum2  = 0;
  uint8_t* output_ptr = res->payload;

  for (uint32_t r = 0; r < cfg.C; r++) {
    uint32_t cb_len = cfg.Kp - cfg.L_cb;

    // Subtract TB CRC from the last code block
    if (r == cfg.C - 1) {
      cb_len -= cfg.L_tb;
    }

    // Append CB
    srsran_vec_u8_copy(output_ptr, tb->softbuffer.rx->data[r], cb_len / 8);
    output_ptr += cb_len / 8;

    // Compute TB CRC for last block
    if (cfg.C > 1 && r == cfg.C - 1) {
      uint8_t  tb_crc_unpacked[24] = {};
      uint8_t* tb_crc_unpacked_ptr = tb_crc_unpacked;
      srsran_bit_unpack_vector(&tb->softbuffer.rx->data[r][cb_len / 8], tb_crc_unpacked, cfg.L_tb);
      checksum2 = srsran_bit_pack(&tb_crc_unpacked_ptr, cfg.L_tb);
    }
  }

  // Calculate TB CRC from packed data
  if (cfg.C == 1) {
    SCH_INFO_RX("TB: TBS=%d; CRC=%s", tb->tbs, tb->softbuffer.rx->cb_crc[0] ? "OK" : "KO");
    res->crc = true;
  } else {
    // More than one
    uint32_t checksum1 = srsran_crc_checksum_byte(crc_tb, res->payload, tb->tbs);
    res->crc           = (checksum1 == checksum2);
    SCH_INFO_RX("TB: TBS=%d; CRC={%06x, %06x}", tb->tbs, checksum1, checksum2);
  }

  if (SRSRAN_DEBUG_ENABLED && get_srsran_verbose_level() >= SRSRAN_VERBOSE_DEBUG && !is_handler_registered()) {
    DEBUG("Decode: ");
    srsran_vec_fprint_byte(stdout, res->payload, tb->tbs / 8);
  }

  return SRSRAN_SUCCESS;
}

int srsran_dlsch_nr_encode(srsran_sch_nr_t*        q,
                           const srsran_sch_cfg_t* pdsch_cfg,
                           const srsran_sch_tb_t*  tb,
                           const uint8_t*          data,
                           uint8_t*                e_bits)
{
  return sch_nr_encode(q, pdsch_cfg, tb, data, e_bits);
}

int srsran_dlsch_nr_decode(srsran_sch_nr_t*        q,
                           const srsran_sch_cfg_t* sch_cfg,
                           const srsran_sch_tb_t*  tb,
                           int8_t*                 e_bits,
                           srsran_sch_tb_res_nr_t* res)
{
  return sch_nr_decode(q, sch_cfg, tb, e_bits, res);
}

int srsran_ulsch_nr_encode(srsran_sch_nr_t*        q,
                           const srsran_sch_cfg_t* pdsch_cfg,
                           const srsran_sch_tb_t*  tb,
                           const uint8_t*          data,
                           uint8_t*                e_bits)
{
  return sch_nr_encode(q, pdsch_cfg, tb, data, e_bits);
}

int srsran_ulsch_nr_decode(srsran_sch_nr_t*        q,
                           const srsran_sch_cfg_t* sch_cfg,
                           const srsran_sch_tb_t*  tb,
                           int8_t*                 e_bits,
                           srsran_sch_tb_res_nr_t* res)
{
  return sch_nr_decode(q, sch_cfg, tb, e_bits, res);
}

int srsran_sch_nr_tb_info(const srsran_sch_tb_t* tb, const srsran_sch_tb_res_nr_t* res, char* str, uint32_t str_len)
{
  int len = 0;

  if (tb->enabled) {
    len = srsran_print_check(str,
                             str_len,
                             len,
                             "CW%d: mod=%s tbs=%d R=%.3f rv=%d ",
                             tb->cw_idx,
                             srsran_mod_string(tb->mod),
                             tb->tbs / 8,
                             tb->R_prime,
                             tb->rv);

    if (res != NULL) {
      len = srsran_print_check(str, str_len, len, "CRC=%s iter=%.1f ", res->crc ? "OK" : "KO", res->avg_iter);
    }
  }

  return len;
}
