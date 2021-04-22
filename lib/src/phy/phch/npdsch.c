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

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "prb_dl.h"
#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/fec/convolutional/rm_conv.h"
#include "srsran/phy/phch/npdsch.h"
#include "srsran/phy/utils/bit.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/vector.h"

#define CURRENT_SFLEN_RE SRSRAN_SF_LEN_RE(q->cell.base.nof_prb, q->cell.base.cp)

#define DUMP_SIGNALS 0
#define RE_EXT_DEBUG 0

int srsran_npdsch_cp(srsran_npdsch_t* q, cf_t* input, cf_t* output, srsran_ra_nbiot_dl_grant_t* grant, bool put)
{
  uint32_t l, nof_lte_refs, nof_nbiot_refs;
  cf_t *   in_ptr = input, *out_ptr = output;

#if RE_EXT_DEBUG
  int num_extracted = 0;
#endif

  // sanity check
  if (q == NULL || input == NULL || output == NULL || grant == NULL) {
    return 0;
  }

  if (put) {
    out_ptr += (grant->l_start * q->cell.base.nof_prb * SRSRAN_NRE) + q->cell.nbiot_prb * SRSRAN_NRE;
  } else {
    in_ptr += (grant->l_start * q->cell.base.nof_prb * SRSRAN_NRE) + q->cell.nbiot_prb * SRSRAN_NRE;
  }

  if (q->cell.nof_ports == 1) {
    nof_nbiot_refs = 2;
  } else {
    nof_nbiot_refs = 4;
  }

  if (q->cell.base.nof_ports == 1) {
    nof_lte_refs = 2;
  } else {
    nof_lte_refs = 4;
  }

  bool skip_crs = false;
  if (q->cell.mode == SRSRAN_NBIOT_MODE_INBAND_SAME_PCI || q->cell.mode == SRSRAN_NBIOT_MODE_INBAND_DIFFERENT_PCI) {
    skip_crs = true;
  }

  if (q->cell.mode == SRSRAN_NBIOT_MODE_INBAND_SAME_PCI && q->cell.n_id_ncell != q->cell.base.id) {
    fprintf(stderr,
            "Cell IDs must match in operation mode inband same PCI (%d != %d)\n",
            q->cell.n_id_ncell,
            q->cell.base.id);
    return 0;
  }

  // start mapping at specified OFDM symbol
  for (l = grant->l_start; l < SRSRAN_CP_NORM_SF_NSYMB; l++) {
    uint32_t delta  = (q->cell.base.nof_prb - 1) * SRSRAN_NRE; // the number of REs skipped in each OFDM symbol
    uint32_t offset = 0; // the number of REs left out before start of the REF signal RE
    if (l == 5 || l == 6 || l == 12 || l == 13) {
      // always skip NRS
      if (nof_nbiot_refs == 2) {
        if (l == 5 || l == 12) {
          offset = q->cell.n_id_ncell % 6;
          delta  = q->cell.n_id_ncell % 6 == 5 ? 1 : 0;
        } else {
          offset = (q->cell.n_id_ncell + 3) % 6;
          delta  = (q->cell.n_id_ncell + 3) % 6 == 5 ? 1 : 0;
        }
      } else if (nof_nbiot_refs == 4) {
        offset = q->cell.n_id_ncell % 3;
        delta  = (q->cell.n_id_ncell + ((q->cell.n_id_ncell >= 5) ? 0 : 3)) % 6 == 5 ? 1 : 0;
      } else {
        fprintf(stderr, "Error %d NB-IoT reference symbols not supported.\n", nof_nbiot_refs);
        return SRSRAN_ERROR;
      }
      prb_cp_ref(&in_ptr, &out_ptr, offset, nof_nbiot_refs, nof_nbiot_refs, put);
    } else if ((l == 0 || l == 4 || l == 7 || l == 11) && skip_crs) {
      // skip LTE's CRS (TODO: use base cell ID?)
      if (nof_lte_refs == 2) {
        if (l == 0 || l == 7) {
          offset = q->cell.base.id % 6;
          delta  = (q->cell.base.id + 3) % 6 == 2 ? 1 : 0;
        } else if (l == 4 || l == 11) {
          offset = (q->cell.base.id + 3) % 6;
          delta  = (q->cell.base.id + ((q->cell.base.id <= 5) ? 3 : 0)) % 6 == 5 ? 1 : 0;
        }
      } else {
        offset = q->cell.base.id % 3;
        delta  = q->cell.base.id % 3 == 2 ? 1 : 0;
      }
      prb_cp_ref(&in_ptr, &out_ptr, offset, nof_lte_refs, nof_lte_refs, put);
    } else {
      // occupy entire symbol
      prb_cp(&in_ptr, &out_ptr, 1);
    }

    if (put) {
      out_ptr += delta;
    } else {
      in_ptr += delta;
    }

#if RE_EXT_DEBUG
    printf("\nl=%d, delta=%d offset=%d\n", l, delta, offset);
    uint32_t num_extracted_this_sym = abs((int)(output - out_ptr)) - num_extracted;
    printf("  - extracted total of %d RE after symbol %d (this symbol=%d)\n",
           abs((int)(output - out_ptr)),
           l,
           num_extracted_this_sym);
    srsran_vec_fprint_c(stdout, &output[num_extracted], num_extracted_this_sym);
    num_extracted = abs((int)(output - out_ptr));
#endif
  }

  int r;
  if (put) {
    r = abs((int)(input - in_ptr));
  } else {
    r = abs((int)(output - out_ptr));
  }

  return r;
}

/**
 * Puts NPDSCH in the subframe
 *
 * Returns the number of symbols written to sf_symbols
 *
 * 36.211 10.3 section 6.3.5
 */
int srsran_npdsch_put(srsran_npdsch_t* q, cf_t* symbols, cf_t* sf_symbols, srsran_ra_nbiot_dl_grant_t* grant)
{
  return srsran_npdsch_cp(q, symbols, sf_symbols, grant, true);
}

/**
 * Extracts NPDSCH from the subframe
 *
 * Returns the number of symbols read
 *
 * 36.211 10.3 section 6.3.5
 */
int srsran_npdsch_get(srsran_npdsch_t* q, cf_t* sf_symbols, cf_t* symbols, srsran_ra_nbiot_dl_grant_t* grant)
{
  return srsran_npdsch_cp(q, sf_symbols, symbols, grant, false);
}

/** Initializes the NPDSCH transmitter and receiver */
int srsran_npdsch_init(srsran_npdsch_t* q)
{
  int ret = SRSRAN_ERROR_INVALID_INPUTS;

  if (q != NULL) {
    bzero(q, sizeof(srsran_npdsch_t));
    ret = SRSRAN_ERROR;

    q->max_re      = SRSRAN_NPDSCH_MAX_RE * SRSRAN_NPDSCH_MAX_NOF_SF;
    q->rnti_is_set = false;

    INFO("Init NPDSCH: max_re's: %d", q->max_re);

    if (srsran_modem_table_lte(&q->mod, SRSRAN_MOD_QPSK)) {
      goto clean;
    }

    int poly[3] = {0x6D, 0x4F, 0x57};
    if (srsran_viterbi_init(&q->decoder, SRSRAN_VITERBI_37, poly, SRSRAN_NPDSCH_MAX_TBS_CRC, true)) {
      goto clean;
    }
    if (srsran_crc_init(&q->crc, SRSRAN_LTE_CRC24A, SRSRAN_NPDSCH_CRC_LEN)) {
      goto clean;
    }
    q->encoder.K           = 7;
    q->encoder.R           = 3;
    q->encoder.tail_biting = true;
    memcpy(q->encoder.poly, poly, 3 * sizeof(int));

    q->d = srsran_vec_cf_malloc(q->max_re);
    if (!q->d) {
      goto clean;
    }
    for (uint32_t i = 0; i < SRSRAN_MAX_PORTS; i++) {
      q->ce[i] = srsran_vec_cf_malloc(q->max_re);
      if (!q->ce[i]) {
        goto clean;
      }
      for (int k = 0; k < q->max_re / 2; k++) {
        q->ce[i][k] = 1;
      }
      q->x[i] = srsran_vec_cf_malloc(q->max_re);
      if (!q->x[i]) {
        goto clean;
      }
      q->symbols[i] = srsran_vec_cf_malloc(q->max_re);
      if (!q->symbols[i]) {
        goto clean;
      }
      q->sib_symbols[i] = srsran_vec_cf_malloc(q->max_re);
      if (!q->sib_symbols[i]) {
        goto clean;
      }
    }
    q->llr = srsran_vec_f_malloc(q->max_re * 2);
    if (!q->llr) {
      goto clean;
    }
    bzero(q->llr, sizeof(float) * q->max_re * 2);

    q->temp = srsran_vec_u8_malloc(SRSRAN_NPDSCH_MAX_TBS_CRC);
    if (!q->temp) {
      goto clean;
    }
    q->rm_b = srsran_vec_u8_malloc(q->max_re * 2);
    if (!q->rm_b) {
      goto clean;
    }
    ret = SRSRAN_SUCCESS;
  }
clean:
  if (ret == SRSRAN_ERROR) {
    srsran_npdsch_free(q);
  }
  return ret;
}

void srsran_npdsch_free(srsran_npdsch_t* q)
{
  if (q->d) {
    free(q->d);
  }
  if (q->temp) {
    free(q->temp);
  }
  if (q->rm_b) {
    free(q->rm_b);
  }
  if (q->llr) {
    free(q->llr);
  }
  for (uint32_t i = 0; i < SRSRAN_MAX_PORTS; i++) {
    if (q->ce[i]) {
      free(q->ce[i]);
    }
    if (q->x[i]) {
      free(q->x[i]);
    }
    if (q->symbols[i]) {
      free(q->symbols[i]);
    }
    if (q->sib_symbols[i]) {
      free(q->sib_symbols[i]);
    }
  }
  for (uint32_t i = 0; i < SRSRAN_NPDSCH_NUM_SEQ; i++) {
    srsran_sequence_free(&q->seq[i]);
  }

  srsran_modem_table_free(&q->mod);
  srsran_viterbi_free(&q->decoder);
  bzero(q, sizeof(srsran_npdsch_t));
}

int srsran_npdsch_set_cell(srsran_npdsch_t* q, srsran_nbiot_cell_t cell)
{
  int ret = SRSRAN_ERROR_INVALID_INPUTS;

  if (q != NULL && srsran_nbiot_cell_isvalid(&cell)) {
    q->cell = cell;

    INFO("NPDSCH: Cell config n_id_ncell=%d, %d ports, %d PRBs base cell, max_symbols: %d",
         q->cell.n_id_ncell,
         q->cell.nof_ports,
         q->cell.base.nof_prb,
         q->max_re);

    ret = SRSRAN_SUCCESS;
  }
  return ret;
}

/* Precalculate the NPDSCH scramble sequences for a given RNTI. This function takes a while
 * to execute, so shall be called once the final C-RNTI has been allocated for the session.
 * It computes sequences for all subframes for both even and odd SFN's, a total of 20
 */
int srsran_npdsch_set_rnti(srsran_npdsch_t* q, uint16_t rnti)
{
  for (int k = 0; k < 2; k++) {
    for (int i = 0; i < SRSRAN_NOF_SF_X_FRAME; i++) {
      if (srsran_sequence_npdsch(&q->seq[k * SRSRAN_NOF_SF_X_FRAME + i],
                                 rnti,
                                 0,
                                 k,
                                 2 * i,
                                 q->cell.n_id_ncell,
                                 q->max_re * srsran_mod_bits_x_symbol(SRSRAN_MOD_QPSK))) {
        return SRSRAN_ERROR;
      }
    }
  }

  q->rnti_is_set = true;
  q->rnti        = rnti;
  return SRSRAN_SUCCESS;
}

int srsran_npdsch_decode(srsran_npdsch_t*        q,
                         srsran_npdsch_cfg_t*    cfg,
                         srsran_softbuffer_rx_t* softbuffer,
                         cf_t*                   sf_symbols,
                         cf_t*                   ce[SRSRAN_MAX_PORTS],
                         float                   noise_estimate,
                         uint32_t                sfn,
                         uint8_t*                data)
{
  if (q != NULL && sf_symbols != NULL && data != NULL && cfg != NULL) {
    if (q->rnti_is_set) {
      return srsran_npdsch_decode_rnti(q, cfg, softbuffer, sf_symbols, ce, noise_estimate, q->rnti, sfn, data, 0);
    } else {
      fprintf(stderr, "Must call srsran_npdsch_set_rnti() before calling srsran_npdsch_decode()\n");
      return SRSRAN_ERROR;
    }
  } else {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }
}

/** Decodes the NPDSCH from the received symbols
 */
int srsran_npdsch_decode_rnti(srsran_npdsch_t*        q,
                              srsran_npdsch_cfg_t*    cfg,
                              srsran_softbuffer_rx_t* softbuffer,
                              cf_t*                   sf_symbols,
                              cf_t*                   ce[SRSRAN_MAX_PORTS],
                              float                   noise_estimate,
                              uint16_t                rnti,
                              uint32_t                sfn,
                              uint8_t*                data,
                              uint32_t                rep_counter)
{
  // Set pointers for layermapping & precoding
  uint32_t n;
  cf_t*    x[SRSRAN_MAX_LAYERS];

  if (q != NULL && sf_symbols != NULL && data != NULL && cfg != NULL) {
    INFO("%d.x: Decoding NPDSCH: RNTI: 0x%x, Mod %s, TBS: %d, NofSymbols: %d * %d, NofBitsE: %d * %d",
         sfn,
         rnti,
         srsran_mod_string(cfg->grant.mcs[0].mod),
         cfg->grant.mcs[0].tbs,
         cfg->grant.nof_sf,
         cfg->nbits.nof_re,
         cfg->grant.nof_sf,
         cfg->nbits.nof_bits);

    // number of layers equals number of ports
    for (int i = 0; i < q->cell.nof_ports; i++) {
      x[i] = q->x[i];
    }
    memset(&x[q->cell.nof_ports], 0, sizeof(cf_t*) * (SRSRAN_MAX_LAYERS - q->cell.nof_ports));

    // extract RE of all subframes of this grant
    uint32_t total_syms = 0;
    for (int i = 0; i < cfg->grant.nof_sf; i++) {
      // extract symbols
      n = srsran_npdsch_get(q, &sf_symbols[i * CURRENT_SFLEN_RE], &q->symbols[0][i * cfg->nbits.nof_re], &cfg->grant);
      if (n != cfg->nbits.nof_re) {
        fprintf(stderr, "Error expecting %d symbols but got %d\n", cfg->nbits.nof_re, n);
        return SRSRAN_ERROR;
      }

      // extract channel estimates
      for (int k = 0; k < q->cell.nof_ports; k++) {
        n = srsran_npdsch_get(q, &ce[k][i * CURRENT_SFLEN_RE], &q->ce[k][i * cfg->nbits.nof_re], &cfg->grant);
        if (n != cfg->nbits.nof_re) {
          fprintf(stderr, "Error expecting %d symbols but got %d\n", cfg->nbits.nof_re, n);
          return SRSRAN_ERROR;
        }
      }
      total_syms += cfg->nbits.nof_re;
    }

    if (total_syms != cfg->grant.nof_sf * cfg->nbits.nof_re) {
      fprintf(stderr, "Error expecting %d symbols but got %d\n", cfg->grant.nof_sf * cfg->nbits.nof_re, total_syms);
      return SRSRAN_ERROR;
    }

#if DUMP_SIGNALS
    if (SRSRAN_VERBOSE_ISDEBUG()) {
      DEBUG("SAVED FILE npdsch_rx_mapping_output.bin: NPDSCH after extracting symbols", 0);
      srsran_vec_save_file(
          "npdsch_rx_mapping_output.bin", q->symbols[0], cfg->grant.nof_sf * cfg->nbits.nof_re * sizeof(cf_t));
    }
#endif

    /* TODO: only diversity is supported */
    if (q->cell.nof_ports == 1) {
      // no need for layer demapping
      srsran_predecoding_single(
          q->symbols[0], q->ce[0], q->d, NULL, cfg->grant.nof_sf * cfg->nbits.nof_re, 1.0, noise_estimate);
    } else {
      srsran_predecoding_diversity(
          q->symbols[0], q->ce, x, q->cell.nof_ports, cfg->grant.nof_sf * cfg->nbits.nof_re, 1.0);
      srsran_layerdemap_diversity(
          x, q->d, q->cell.nof_ports, cfg->grant.nof_sf * cfg->nbits.nof_re / q->cell.nof_ports);
    }

#if DUMP_SIGNALS
    if (SRSRAN_VERBOSE_ISDEBUG()) {
      DEBUG("SAVED FILE npdsch_rx_predecode_output.bin: NPDSCH after predecoding symbols", 0);
      srsran_vec_save_file(
          "npdsch_rx_predecode_output.bin", q->d, cfg->grant.nof_sf * cfg->nbits.nof_re * sizeof(cf_t));
    }
#endif

    // demodulate symbols
    srsran_demod_soft_demodulate(SRSRAN_MOD_QPSK, q->d, q->llr, cfg->grant.nof_sf * cfg->nbits.nof_re);

#if DUMP_SIGNALS
    uint8_t demodbuf[320];
    hard_qpsk_demod(q->d, demodbuf, cfg->nbits.nof_re);

    if (SRSRAN_VERBOSE_ISDEBUG()) {
      DEBUG("SAVED FILE npdsch_rx_demod_output.bin: NPDSCH after (hard) de-modulation", 0);
      srsran_vec_save_file("npdsch_rx_demod_output.bin", demodbuf, cfg->nbits.nof_bits);
    }
#endif

    // descramble
    if (q->cell.is_r14 && rnti == SRSRAN_SIRNTI) {
      srsran_sequence_t seq;
      if (srsran_sequence_npdsch_bcch_r14(
              &seq, cfg->grant.start_sfn, q->cell.n_id_ncell, cfg->grant.nof_sf * cfg->nbits.nof_bits)) {
        return SRSRAN_ERROR;
      }
      srsran_scrambling_f_offset(&seq, q->llr, 0, cfg->grant.nof_sf * cfg->nbits.nof_bits);
      srsran_sequence_free(&seq);
    } else {
      if (rnti != q->rnti) {
        srsran_sequence_t seq;
        if (srsran_sequence_npdsch(&seq,
                                   rnti,
                                   0,
                                   cfg->grant.start_sfn,
                                   2 * cfg->grant.start_sfidx,
                                   q->cell.n_id_ncell,
                                   cfg->grant.nof_sf * cfg->nbits.nof_bits)) {
          return SRSRAN_ERROR;
        }
        srsran_scrambling_f_offset(&seq, q->llr, 0, cfg->grant.nof_sf * cfg->nbits.nof_bits);
        srsran_sequence_free(&seq);
      } else {
        // odd SFN's take the second half of the seq array
        int seq_pos = ((cfg->grant.start_sfn % 2) * SRSRAN_NOF_SF_X_FRAME) + cfg->grant.start_sfidx;
        srsran_scrambling_f_offset(&q->seq[seq_pos], q->llr, 0, cfg->grant.nof_sf * cfg->nbits.nof_bits);
      }
    }

#if DUMP_SIGNALS
    if (SRSRAN_VERBOSE_ISDEBUG()) {
      DEBUG("SAVED FILE npdsch_rx_descramble_output.bin: NPDSCH after de-scrambling", 0);
      srsran_vec_save_file("npdsch_rx_descramble_output.bin", q->llr, cfg->nbits.nof_bits);
    }
#endif

    // decode only this transmission
    return srsran_npdsch_rm_and_decode(q, cfg, q->llr, data);
  } else {
    fprintf(stderr, "srsran_npdsch_decode_rnti() called with invalid parameters.\n");
    return SRSRAN_ERROR_INVALID_INPUTS;
  }
}

int srsran_npdsch_rm_and_decode(srsran_npdsch_t* q, srsran_npdsch_cfg_t* cfg, float* softbits, uint8_t* data)
{
  // unrate-matching
  uint32_t coded_len = 3 * (cfg->grant.mcs[0].tbs + SRSRAN_NPDSCH_CRC_LEN);
  bzero(q->rm_f, sizeof(float) * SRSRAN_NPDSCH_MAX_TBS_ENC);
  srsran_rm_conv_rx(softbits, cfg->grant.nof_sf * cfg->nbits.nof_bits, q->rm_f, coded_len);

  // TODO: normalization needed?

  // viterbi decoder
  srsran_viterbi_decode_f(&q->decoder, q->rm_f, q->temp, cfg->grant.mcs[0].tbs + SRSRAN_NPDSCH_CRC_LEN);

#if DUMP_SIGNALS
  if (SRSRAN_VERBOSE_ISDEBUG()) {
    DEBUG("SAVED FILE npdsch_rx_viterbidecode_output.bin: NPDSCH after viterbi decoding", 0);
    srsran_vec_save_file("npdsch_rx_viterbidecode_output.bin", q->temp, cfg->grant.mcs[0].tbs + SRSRAN_NPDSCH_CRC_LEN);
  }
#endif

  // verify CRC sum
  uint8_t* x      = &q->temp[cfg->grant.mcs[0].tbs];
  uint32_t tx_sum = srsran_bit_pack(&x, SRSRAN_NPDSCH_CRC_LEN);
  uint32_t rx_sum = srsran_crc_checksum(&q->crc, q->temp, cfg->grant.mcs[0].tbs);

  if (rx_sum == tx_sum && rx_sum != 0) {
    srsran_bit_pack_vector(q->temp, data, cfg->grant.mcs[0].tbs);
    return SRSRAN_SUCCESS;
  } else {
    return SRSRAN_ERROR;
  }
}

/* This functions encodes an NPDSCH and maps it on to the provided subframe.
 * It only ever writes a single subframe but it can be called multiple times
 * to write more subframes of a previously encoded NPDSCH. For this purpose
 * it uses the is_encoded flag in the NPDSCH config object.
 * The same applies to its sister functions below.
 */
int srsran_npdsch_encode(srsran_npdsch_t*        q,
                         srsran_npdsch_cfg_t*    cfg,
                         srsran_softbuffer_tx_t* softbuffer,
                         uint8_t*                data,
                         cf_t*                   sf_symbols[SRSRAN_MAX_PORTS])
{
  if (q != NULL && data != NULL && cfg != NULL) {
    if (q->rnti_is_set) {
      return srsran_npdsch_encode_rnti(q, cfg, softbuffer, data, q->rnti, sf_symbols);
    } else {
      fprintf(stderr, "Must call srsran_npdsch_set_rnti() to set the encoder/decoder RNTI\n");
      return SRSRAN_ERROR;
    }
  } else {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }
}

/** Converts the PDSCH data bits to symbols mapped to the slot ready for transmission
 */
int srsran_npdsch_encode_rnti(srsran_npdsch_t*        q,
                              srsran_npdsch_cfg_t*    cfg,
                              srsran_softbuffer_tx_t* softbuffer,
                              uint8_t*                data,
                              uint16_t                rnti,
                              cf_t*                   sf_symbols[SRSRAN_MAX_PORTS])
{
  if (rnti != q->rnti) {
    // TODO: skip sequence init if cfg->is_encoded==true
    srsran_sequence_t seq;
    if (q->cell.is_r14 && rnti == SRSRAN_SIRNTI) {
      if (srsran_sequence_npdsch_bcch_r14(
              &seq, cfg->grant.start_sfn, q->cell.n_id_ncell, cfg->grant.nof_sf * cfg->nbits.nof_bits)) {
        return SRSRAN_ERROR;
      }
    } else {
      if (srsran_sequence_npdsch(&seq,
                                 rnti,
                                 0,
                                 cfg->grant.start_sfidx,
                                 2 * cfg->grant.start_sfidx,
                                 q->cell.n_id_ncell,
                                 cfg->nbits.nof_bits * cfg->grant.nof_sf)) {
        return SRSRAN_ERROR;
      }
    }
    int r = srsran_npdsch_encode_seq(q, cfg, softbuffer, data, &seq, sf_symbols);
    srsran_sequence_free(&seq);
    return r;
  } else {
    int seq_pos = ((cfg->grant.start_sfn % 2) * SRSRAN_NOF_SF_X_FRAME) + cfg->grant.start_sfidx;
    return srsran_npdsch_encode_seq(q, cfg, softbuffer, data, &q->seq[seq_pos], sf_symbols);
  }
}

int srsran_npdsch_encode_seq(srsran_npdsch_t*        q,
                             srsran_npdsch_cfg_t*    cfg,
                             srsran_softbuffer_tx_t* softbuffer,
                             uint8_t*                data,
                             srsran_sequence_t*      seq,
                             cf_t*                   sf_symbols[SRSRAN_MAX_PORTS])
{
  /* Set pointers for layermapping & precoding */
  cf_t* x[SRSRAN_MAX_LAYERS];
  int   ret = SRSRAN_ERROR_INVALID_INPUTS;

  if (q != NULL && data != NULL && cfg != NULL) {
    for (int i = 0; i < q->cell.nof_ports; i++) {
      if (sf_symbols[i] == NULL) {
        return SRSRAN_ERROR_INVALID_INPUTS;
      }

      // Set up pointer for Tx symbols
      q->tx_syms[i] = (cfg->has_bcch) ? q->sib_symbols[i] : q->symbols[i];
    }

    if (cfg->grant.mcs[0].tbs == 0) {
      return SRSRAN_ERROR_INVALID_INPUTS;
    }

    if (cfg->nbits.nof_re > q->max_re) {
      fprintf(stderr,
              "Error too many RE per subframe (%d). NPDSCH configured for %d RE (%d PRB)\n",
              cfg->nbits.nof_re,
              q->max_re,
              q->cell.base.nof_prb);
      return SRSRAN_ERROR_INVALID_INPUTS;
    }

    // make sure to run run full NPDSCH procedure only once
    if (!cfg->is_encoded) {
      INFO("Encoding NPDSCH: Mod %s, NofBits: %d, NofSymbols: %d * %d, NofBitsE: %d * %d",
           srsran_mod_string(cfg->grant.mcs[0].mod),
           cfg->grant.mcs[0].tbs,
           cfg->grant.nof_sf,
           cfg->nbits.nof_re,
           cfg->grant.nof_sf,
           cfg->nbits.nof_bits);

      /* number of layers equals number of ports */
      for (int i = 0; i < q->cell.nof_ports; i++) {
        x[i] = q->x[i];
      }
      memset(&x[q->cell.nof_ports], 0, sizeof(cf_t*) * (SRSRAN_MAX_LAYERS - q->cell.nof_ports));

      int len = cfg->grant.mcs[0].tbs;

      // unpack input
      srsran_bit_unpack_vector(data, q->data, len);

      // attach CRC
      srsran_crc_attach(&q->crc, q->data, len);
      len += SRSRAN_NPDSCH_CRC_LEN;

#if DUMP_SIGNALS
      if (SRSRAN_VERBOSE_ISDEBUG()) {
        DEBUG("SAVED FILE npdsch_tx_convcoder_input.bin: NPDSCH before convolution coding", 0);
        srsran_vec_save_file("npdsch_tx_convcoder_input.bin", q->data, len);
      }
#endif

      // encode
      srsran_convcoder_encode(&q->encoder, q->data, q->data_enc, len);
      len *= 3;

      // rate-match to allocated bits and scramble output
      srsran_rm_conv_tx(q->data_enc, len, q->rm_b, cfg->nbits.nof_bits * cfg->grant.nof_sf);
      len = cfg->nbits.nof_bits * cfg->grant.nof_sf;

#if DUMP_SIGNALS
      if (SRSRAN_VERBOSE_ISDEBUG()) {
        DEBUG("SAVED FILE npdsch_tx_scramble_input.bin: NPDSCH before scrambling", 0);
        srsran_vec_save_file("npdsch_tx_scramble_input.bin", q->rm_b, len);
      }
#endif

      // scramble
      srsran_scrambling_b_offset(seq, q->rm_b, 0, len);

#if DUMP_SIGNALS
      if (SRSRAN_VERBOSE_ISDEBUG()) {
        DEBUG("SAVED FILE npdsch_tx_mod_input.bin: NPDSCH before modulation", 0);
        srsran_vec_save_file("npdsch_tx_mod_input.bin", q->rm_b, len);
      }
#endif

      // modulate bits
      srsran_mod_modulate(&q->mod, (uint8_t*)q->rm_b, q->d, len);
      len = cfg->nbits.nof_re * cfg->grant.nof_sf;

#if DUMP_SIGNALS
      if (SRSRAN_VERBOSE_ISDEBUG()) {
        DEBUG("SAVED FILE npdsch_tx_precode_input.bin: NPDSCH before precoding symbols", 0);
        srsran_vec_save_file("npdsch_tx_precode_input.bin", q->d, len * sizeof(cf_t));
      }
#endif

      // TODO: only diversity supported
      if (q->cell.base.nof_ports > 1) {
        srsran_layermap_diversity(q->d, x, q->cell.base.nof_ports, len);
        srsran_precoding_diversity(x, q->tx_syms, q->cell.base.nof_ports, len / q->cell.base.nof_ports, 1.0);
      } else {
        memcpy(q->tx_syms[0], q->d, len * sizeof(cf_t));
      }

#if DUMP_SIGNALS
      if (SRSRAN_VERBOSE_ISDEBUG()) {
        DEBUG("SAVED FILE npdsch_tx_mapping_input.bin: NPDSCH before mapping to resource elements", 0);
        srsran_vec_save_file("npdsch_tx_mapping_input.bin", q->tx_syms[0], len * sizeof(cf_t));
      }
#endif

      cfg->is_encoded = true;
    }

    // mapping to resource elements
    if (cfg->is_encoded) {
      INFO("Mapping %d NPDSCH REs, sf_idx=%d/%d rep=%d/%d total=%d/%d",
           cfg->nbits.nof_re,
           cfg->sf_idx + 1,
           cfg->grant.nof_sf,
           cfg->rep_idx + 1,
           cfg->grant.nof_rep,
           cfg->num_sf + 1,
           cfg->grant.nof_sf * cfg->grant.nof_rep);
      for (int i = 0; i < q->cell.nof_ports; i++) {
        srsran_npdsch_put(q, &q->tx_syms[i][cfg->sf_idx * cfg->nbits.nof_re], sf_symbols[i], &cfg->grant);
      }
      cfg->num_sf++;

      // Decide whether we retransmit the same SF or the next
      if (cfg->has_bcch) {
        // NPDSCH with BCCH is always transmitted in sequence
        cfg->sf_idx++;
        if (cfg->sf_idx == cfg->grant.nof_sf) {
          cfg->rep_idx++;
        }
      } else {
        // NPDSCH without BCCH transmits up to 3 repetitions after another
        cfg->rep_idx++;
        int m = SRSRAN_MIN(cfg->grant.nof_rep, 4);
        if (cfg->rep_idx % m == 0) {
          cfg->sf_idx++;
          // start with first SF again after all have been tx'ed m-times
          if (cfg->sf_idx == cfg->grant.nof_sf) {
            cfg->sf_idx = 0;
          } else {
            cfg->rep_idx -= m;
          }
        }
      }
    }
    ret = SRSRAN_SUCCESS;
  }
  return ret;
}

/* Configures the structure srsran_npdsch_cfg_t from a DL grant.
 * If grant is NULL, the grant is assumed to be already stored in cfg->grant
 */
int srsran_npdsch_cfg(srsran_npdsch_cfg_t*        cfg,
                      srsran_nbiot_cell_t         cell,
                      srsran_ra_nbiot_dl_grant_t* grant,
                      uint32_t                    sf_idx)
{
  if (cfg) {
    if (grant) {
      memcpy(&cfg->grant, grant, sizeof(srsran_ra_nbiot_dl_grant_t));
    }

    // Compute number of RE
    srsran_ra_nbiot_dl_grant_to_nbits(&cfg->grant, cell, sf_idx, &cfg->nbits);
    cfg->sf_idx     = 0;
    cfg->rep_idx    = 0;
    cfg->num_sf     = 0;
    cfg->is_encoded = false;
    cfg->has_bcch   = cfg->grant.has_sib1; // The UE needs to set this to true for other SIBs too

    return SRSRAN_SUCCESS;
  } else {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }
}

/** Reads the HFN from the SIB1-NB, according to TS 36.331 v13.2.0 Section 6.7.1
 *
 * The function assumes that the 2 LSB read from the MIB-NB are already set inside
 * the argument structure. It extracts the 8 MSB from the SIB1 and updates the HFN
 * accordingly.
 *
 * \param msg The packed SIB1-NB
 * \param sib The SIB structure containing the MIB-part of the HFN
 */
void srsran_npdsch_sib1_unpack(uint8_t* const msg, srsran_sys_info_block_type_1_nb_t* sib)
{
  uint8_t unpacked[SRSRAN_NPDSCH_MAX_TBS];
  srsran_bit_unpack_vector(msg, unpacked, SRSRAN_NPDSCH_MAX_TBS);

  uint8_t* tmp = unpacked;
  if (sib) {
    tmp += 12;
    sib->hyper_sfn = ((srsran_bit_pack(&tmp, 8) & 0xFF) << 2 | (sib->hyper_sfn & 0x3)) & 0x3FF;
  }
}
