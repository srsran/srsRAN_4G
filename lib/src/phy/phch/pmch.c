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

#include "prb_dl.h"
#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/phch/pmch.h"
#include "srslte/phy/utils/bit.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"

#define MAX_PMCH_RE (2 * SRSLTE_CP_EXT_NSYMB * 12)

const static srslte_mod_t modulations[4] = {SRSLTE_MOD_BPSK, SRSLTE_MOD_QPSK, SRSLTE_MOD_16QAM, SRSLTE_MOD_64QAM};

static int pmch_cp(srslte_pmch_t* q, cf_t* input, cf_t* output, uint32_t lstart_grant, bool put)
{
  uint32_t s, n, l, lp, lstart, lend, nof_refs;
  cf_t *   in_ptr = input, *out_ptr = output;
  uint32_t offset = 0;

#ifdef DEBUG_IDX
  indices_ptr = 0;
  if (put) {
    offset_original = output;
  } else {
    offset_original = input;
  }
#endif
  nof_refs = 6;
  for (s = 0; s < 2; s++) {
    for (l = 0; l < SRSLTE_CP_EXT_NSYMB; l++) {
      for (n = 0; n < q->cell.nof_prb; n++) {
        // If this PRB is assigned
        if (true) {
          if (s == 0) {
            lstart = lstart_grant;
          } else {
            lstart = 0;
          }
          lend = SRSLTE_CP_EXT_NSYMB;
          lp   = l + s * SRSLTE_CP_EXT_NSYMB;
          if (put) {
            out_ptr = &output[(lp * q->cell.nof_prb + n) * SRSLTE_NRE];
          } else {
            in_ptr = &input[(lp * q->cell.nof_prb + n) * SRSLTE_NRE];
          }
          // This is a symbol in a normal PRB with or without references
          if (l >= lstart && l < lend) {
            if (SRSLTE_SYMBOL_HAS_REF_MBSFN(l, s)) {
              if (l == 0 && s == 1) {
                offset = 1;
              } else {
                offset = 0;
              }
              prb_cp_ref(&in_ptr, &out_ptr, offset, nof_refs, nof_refs, put);
            } else {
              prb_cp(&in_ptr, &out_ptr, 1);
            }
          }
        }
      }
    }
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
 * Puts PMCH in slot number 1
 *
 * Returns the number of symbols written to sf_symbols
 *
 * 36.211 10.3 section 6.3.5
 */
static int pmch_put(srslte_pmch_t* q, cf_t* symbols, cf_t* sf_symbols, uint32_t lstart)
{
  return pmch_cp(q, symbols, sf_symbols, lstart, true);
}

/**
 * Extracts PMCH from slot number 1
 *
 * Returns the number of symbols written to PMCH
 *
 * 36.211 10.3 section 6.3.5
 */
static int pmch_get(srslte_pmch_t* q, cf_t* sf_symbols, cf_t* symbols, uint32_t lstart)
{
  return pmch_cp(q, sf_symbols, symbols, lstart, false);
}

int srslte_pmch_init(srslte_pmch_t* q, uint32_t max_prb, uint32_t nof_rx_antennas)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q != NULL && nof_rx_antennas <= SRSLTE_MAX_PORTS) {

    bzero(q, sizeof(srslte_pmch_t));
    ret = SRSLTE_ERROR;

    q->cell.nof_prb    = max_prb;
    q->cell.nof_ports  = 1;
    q->max_re          = max_prb * MAX_PMCH_RE;
    q->nof_rx_antennas = nof_rx_antennas;

    INFO("Init PMCH: %d PRBs, max_symbols: %d\n", max_prb, q->max_re);

    for (int i = 0; i < 4; i++) {
      if (srslte_modem_table_lte(&q->mod[i], modulations[i])) {
        goto clean;
      }
      srslte_modem_table_bytes(&q->mod[i]);
    }

    srslte_sch_init(&q->dl_sch);

    // Allocate int16_t for reception (LLRs)
    q->e = srslte_vec_i16_malloc(q->max_re * srslte_mod_bits_x_symbol(SRSLTE_MOD_64QAM));
    if (!q->e) {
      goto clean;
    }

    q->d = srslte_vec_cf_malloc(q->max_re);
    if (!q->d) {
      goto clean;
    }

    for (int i = 0; i < SRSLTE_MAX_PORTS; i++) {
      q->x[i] = srslte_vec_cf_malloc(q->max_re);
      if (!q->x[i]) {
        goto clean;
      }
      for (int j = 0; j < q->nof_rx_antennas; j++) {
        q->ce[i][j] = srslte_vec_cf_malloc(q->max_re);
        if (!q->ce[i][j]) {
          goto clean;
        }
      }
    }
    for (int j = 0; j < q->nof_rx_antennas; j++) {
      q->symbols[j] = srslte_vec_cf_malloc(q->max_re);
      if (!q->symbols[j]) {
        goto clean;
      }
    }

    q->seqs = calloc(SRSLTE_MAX_MBSFN_AREA_IDS, sizeof(srslte_pmch_seq_t*));
    if (!q->seqs) {
      perror("calloc");
      goto clean;
    }

    ret = SRSLTE_SUCCESS;
  }
clean:
  if (ret == SRSLTE_ERROR) {
    srslte_pmch_free(q);
  }
  return ret;
}

void srslte_pmch_free(srslte_pmch_t* q)
{
  if (q->e) {
    free(q->e);
  }
  if (q->d) {
    free(q->d);
  }
  for (uint32_t i = 0; i < SRSLTE_MAX_PORTS; i++) {
    if (q->x[i]) {
      free(q->x[i]);
    }
    for (uint32_t j = 0; j < q->nof_rx_antennas; j++) {
      if (q->ce[i][j]) {
        free(q->ce[i][j]);
      }
    }
  }
  for (uint32_t i = 0; i < q->nof_rx_antennas; i++) {
    if (q->symbols[i]) {
      free(q->symbols[i]);
    }
  }
  if (q->seqs) {
    for (uint32_t i = 0; i < SRSLTE_MAX_MBSFN_AREA_IDS; i++) {
      if (q->seqs[i]) {
        srslte_pmch_free_area_id(q, i);
      }
    }
    free(q->seqs);
  }
  for (uint32_t i = 0; i < 4; i++) {
    srslte_modem_table_free(&q->mod[i]);
  }

  srslte_sch_free(&q->dl_sch);

  bzero(q, sizeof(srslte_pmch_t));
}

int srslte_pmch_set_cell(srslte_pmch_t* q, srslte_cell_t cell)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q != NULL && srslte_cell_isvalid(&cell)) {
    q->cell   = cell;
    q->max_re = q->cell.nof_prb * MAX_PMCH_RE;

    INFO("PMCH: Cell config PCI=%d, %d ports, %d PRBs, max_symbols: %d\n",
         q->cell.nof_ports,
         q->cell.id,
         q->cell.nof_prb,
         q->max_re);

    ret = SRSLTE_SUCCESS;
  }
  return ret;
}

/* Precalculate the scramble sequences for a given MBSFN area ID. This function takes a while
 * to execute.
 */
int srslte_pmch_set_area_id(srslte_pmch_t* q, uint16_t area_id)
{
  uint32_t i;
  if (!q->seqs[area_id]) {
    q->seqs[area_id] = calloc(1, sizeof(srslte_pmch_seq_t));
    if (q->seqs[area_id]) {
      for (i = 0; i < SRSLTE_NOF_SF_X_FRAME; i++) {
        if (srslte_sequence_pmch(
                &q->seqs[area_id]->seq[i], 2 * i, area_id, q->max_re * srslte_mod_bits_x_symbol(SRSLTE_MOD_64QAM))) {
          return SRSLTE_ERROR;
        }
      }
    }
  }
  return SRSLTE_SUCCESS;
}

void srslte_pmch_free_area_id(srslte_pmch_t* q, uint16_t area_id)
{
  if (q->seqs[area_id]) {
    for (int i = 0; i < SRSLTE_NOF_SF_X_FRAME; i++) {
      srslte_sequence_free(&q->seqs[area_id]->seq[i]);
    }
    free(q->seqs[area_id]);
    q->seqs[area_id] = NULL;
  }
}

/** Decodes the pmch from the received symbols
 */
int srslte_pmch_decode(srslte_pmch_t*         q,
                       srslte_dl_sf_cfg_t*    sf,
                       srslte_pmch_cfg_t*     cfg,
                       srslte_chest_dl_res_t* channel,
                       cf_t*                  sf_symbols[SRSLTE_MAX_PORTS],
                       srslte_pdsch_res_t*    out)
{

  /* Set pointers for layermapping & precoding */
  uint32_t i, n;
  cf_t*    x[SRSLTE_MAX_LAYERS];

  if (q != NULL && sf_symbols != NULL && out != NULL && cfg != NULL) {
    INFO("Decoding PMCH SF: %d, MBSFN area ID: 0x%x, Mod %s, TBS: %d, NofSymbols: %d, NofBitsE: %d, rv_idx: %d, "
         "C_prb=%d, cfi=%d\n",
         sf->tti % 10,
         cfg->area_id,
         srslte_mod_string(cfg->pdsch_cfg.grant.tb[0].mod),
         cfg->pdsch_cfg.grant.tb[0].tbs,
         cfg->pdsch_cfg.grant.nof_re,
         cfg->pdsch_cfg.grant.tb[0].nof_bits,
         0,
         cfg->pdsch_cfg.grant.nof_prb,
         sf->cfi);

    /* number of layers equals number of ports */
    for (i = 0; i < q->cell.nof_ports; i++) {
      x[i] = q->x[i];
    }
    memset(&x[q->cell.nof_ports], 0, sizeof(cf_t*) * (SRSLTE_MAX_LAYERS - q->cell.nof_ports));

    uint32_t lstart = SRSLTE_NOF_CTRL_SYMBOLS(q->cell, sf->cfi);
    for (int j = 0; j < q->nof_rx_antennas; j++) {
      /* extract symbols */
      n = pmch_get(q, sf_symbols[j], q->symbols[j], lstart);
      if (n != cfg->pdsch_cfg.grant.nof_re) {

        ERROR("PMCH 1 extract symbols error expecting %d symbols but got %d, lstart %d\n",
              cfg->pdsch_cfg.grant.nof_re,
              n,
              lstart);
        return SRSLTE_ERROR;
      }

      /* extract channel estimates */
      for (i = 0; i < q->cell.nof_ports; i++) {
        n = pmch_get(q, channel->ce[i][j], q->ce[i][j], lstart);
        if (n != cfg->pdsch_cfg.grant.nof_re) {
          ERROR("PMCH 2 extract chest error expecting %d symbols but got %d\n", cfg->pdsch_cfg.grant.nof_re, n);
          return SRSLTE_ERROR;
        }
      }
    }

    // No tx diversity in MBSFN
    srslte_predecoding_single_multi(q->symbols,
                                    q->ce[0],
                                    q->d,
                                    NULL,
                                    q->nof_rx_antennas,
                                    cfg->pdsch_cfg.grant.nof_re,
                                    1.0f,
                                    channel->noise_estimate);

    if (SRSLTE_VERBOSE_ISDEBUG()) {
      DEBUG("SAVED FILE subframe.dat: received subframe symbols\n");
      srslte_vec_save_file("subframe2.dat", q->symbols[0], cfg->pdsch_cfg.grant.nof_re * sizeof(cf_t));
      DEBUG("SAVED FILE hest0.dat: channel estimates for port 4\n");
      printf("nof_prb=%d, cp=%d, nof_re=%d, grant_re=%d\n",
             q->cell.nof_prb,
             q->cell.cp,
             SRSLTE_NOF_RE(q->cell),
             cfg->pdsch_cfg.grant.nof_re);
      srslte_vec_save_file("hest2.dat", channel->ce[0][0], SRSLTE_NOF_RE(q->cell) * sizeof(cf_t));
      DEBUG("SAVED FILE pmch_symbols.dat: symbols after equalization\n");
      srslte_vec_save_file("pmch_symbols.bin", q->d, cfg->pdsch_cfg.grant.nof_re * sizeof(cf_t));
    }

    /* demodulate symbols
     * The MAX-log-MAP algorithm used in turbo decoding is unsensitive to SNR estimation,
     * thus we don't need tot set it in thde LLRs normalization
     */
    srslte_demod_soft_demodulate_s(cfg->pdsch_cfg.grant.tb[0].mod, q->d, q->e, cfg->pdsch_cfg.grant.nof_re);

    /* descramble */
    srslte_scrambling_s_offset(&q->seqs[cfg->area_id]->seq[sf->tti % 10], q->e, 0, cfg->pdsch_cfg.grant.tb[0].nof_bits);

    if (SRSLTE_VERBOSE_ISDEBUG()) {
      DEBUG("SAVED FILE llr.dat: LLR estimates after demodulation and descrambling\n");
      srslte_vec_save_file("llr.dat", q->e, cfg->pdsch_cfg.grant.tb[0].nof_bits * sizeof(int16_t));
    }
    out[0].crc                  = (srslte_dlsch_decode(&q->dl_sch, &cfg->pdsch_cfg, q->e, out[0].payload) == 0);
    out[0].avg_iterations_block = srslte_sch_last_noi(&q->dl_sch);

    return SRSLTE_SUCCESS;
  } else {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }
}
void srslte_configure_pmch(srslte_pmch_cfg_t* pmch_cfg, srslte_cell_t* cell, srslte_mbsfn_cfg_t* mbsfn_cfg)
{
  pmch_cfg->area_id                       = 1;
  pmch_cfg->pdsch_cfg.grant.nof_layers    = 1;
  pmch_cfg->pdsch_cfg.grant.nof_prb       = cell->nof_prb;
  pmch_cfg->pdsch_cfg.grant.tb[0].mcs_idx = mbsfn_cfg->mbsfn_mcs;
  pmch_cfg->pdsch_cfg.grant.tb[0].enabled = mbsfn_cfg->enable;
  pmch_cfg->pdsch_cfg.grant.tb[0].rv      = SRSLTE_PMCH_RV;
  pmch_cfg->pdsch_cfg.grant.last_tbs[0]   = 0;
  srslte_dl_fill_ra_mcs(&pmch_cfg->pdsch_cfg.grant.tb[0],
                        pmch_cfg->pdsch_cfg.grant.last_tbs[0],
                        pmch_cfg->pdsch_cfg.grant.nof_prb,
                        false);
  pmch_cfg->pdsch_cfg.grant.nof_tb     = 1;
  pmch_cfg->pdsch_cfg.grant.nof_layers = 1;
  for (int i = 0; i < 2; i++) {
    for (uint32_t j = 0; j < pmch_cfg->pdsch_cfg.grant.nof_prb; j++) {
      pmch_cfg->pdsch_cfg.grant.prb_idx[i][j] = true;
    }
  }
}

int srslte_pmch_encode(srslte_pmch_t*      q,
                       srslte_dl_sf_cfg_t* sf,
                       srslte_pmch_cfg_t*  cfg,
                       uint8_t*            data,
                       cf_t*               sf_symbols[SRSLTE_MAX_PORTS])
{

  int i;
  /* Set pointers for layermapping & precoding */
  cf_t* x[SRSLTE_MAX_LAYERS];
  int   ret = SRSLTE_ERROR_INVALID_INPUTS;
  if (q != NULL && cfg != NULL) {
    for (i = 0; i < q->cell.nof_ports; i++) {
      if (sf_symbols[i] == NULL) {
        return SRSLTE_ERROR_INVALID_INPUTS;
      }
    }

    if (cfg->pdsch_cfg.grant.tb[0].tbs == 0) {
      return SRSLTE_ERROR_INVALID_INPUTS;
    }

    if (cfg->pdsch_cfg.grant.nof_re > q->max_re) {
      ERROR("Error too many RE per subframe (%d). PMCH configured for %d RE (%d PRB)\n",
            cfg->pdsch_cfg.grant.nof_re,
            q->max_re,
            q->cell.nof_prb);
      return SRSLTE_ERROR_INVALID_INPUTS;
    }

    INFO("Encoding PMCH SF: %d, Mod %s, NofBits: %d, NofSymbols: %d, NofBitsE: %d, rv_idx: %d\n",
         sf->tti % 10,
         srslte_mod_string(cfg->pdsch_cfg.grant.tb[0].mod),
         cfg->pdsch_cfg.grant.tb[0].tbs,
         cfg->pdsch_cfg.grant.nof_re,
         cfg->pdsch_cfg.grant.tb[0].nof_bits,
         0);

    /* number of layers equals number of ports */
    for (i = 0; i < q->cell.nof_ports; i++) {
      x[i] = q->x[i];
    }
    memset(&x[q->cell.nof_ports], 0, sizeof(cf_t*) * (SRSLTE_MAX_LAYERS - q->cell.nof_ports));

    // TODO: use tb_encode directly
    if (srslte_dlsch_encode(&q->dl_sch, &cfg->pdsch_cfg, data, q->e)) {
      ERROR("Error encoding TB\n");
      return SRSLTE_ERROR;
    }

    /* scramble */
    srslte_scrambling_bytes(
        &q->seqs[cfg->area_id]->seq[sf->tti % 10], (uint8_t*)q->e, cfg->pdsch_cfg.grant.tb[0].nof_bits);

    srslte_mod_modulate_bytes(
        &q->mod[cfg->pdsch_cfg.grant.tb[0].mod], (uint8_t*)q->e, q->d, cfg->pdsch_cfg.grant.tb[0].nof_bits);

    /* No tx diversity in MBSFN */
    memcpy(q->symbols[0], q->d, cfg->pdsch_cfg.grant.nof_re * sizeof(cf_t));

    /* mapping to resource elements */
    uint32_t lstart = SRSLTE_NOF_CTRL_SYMBOLS(q->cell, sf->cfi);
    for (i = 0; i < q->cell.nof_ports; i++) {
      pmch_put(q, q->symbols[i], sf_symbols[i], lstart);
    }

    ret = SRSLTE_SUCCESS;
  }
  return ret;
}
