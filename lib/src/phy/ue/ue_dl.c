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

#include "srslte/phy/ue/ue_dl.h"

#include "srslte/srslte.h"
#include <string.h>

#define CURRENT_FFTSIZE srslte_symbol_sz(q->cell.nof_prb)
#define CURRENT_SFLEN_RE SRSLTE_NOF_RE(q->cell)
#define MAX_SFLEN_RE SRSLTE_SF_LEN_RE(max_prb, q->cell.cp)

const static srslte_dci_format_t ue_dci_formats[8][2] = {
    /* Mode 1 */ {SRSLTE_DCI_FORMAT1A, SRSLTE_DCI_FORMAT1},
    /* Mode 2 */ {SRSLTE_DCI_FORMAT1A, SRSLTE_DCI_FORMAT1},
    /* Mode 3 */ {SRSLTE_DCI_FORMAT1A, SRSLTE_DCI_FORMAT2A},
    /* Mode 4 */ {SRSLTE_DCI_FORMAT1A, SRSLTE_DCI_FORMAT2},
    /* Mode 5 */ {SRSLTE_DCI_FORMAT1A, SRSLTE_DCI_FORMAT1D},
    /* Mode 6 */ {SRSLTE_DCI_FORMAT1A, SRSLTE_DCI_FORMAT1B},
    /* Mode 7 */ {SRSLTE_DCI_FORMAT1A, SRSLTE_DCI_FORMAT1},
    /* Mode 8 */ {SRSLTE_DCI_FORMAT1A, SRSLTE_DCI_FORMAT2B}};

static srslte_dci_format_t common_formats[]   = {SRSLTE_DCI_FORMAT1A, SRSLTE_DCI_FORMAT1C};
const uint32_t             nof_common_formats = 2;

// mi value as in table 6.9-1 36.213 for regs vector. For FDD, uses only 1st
const static uint32_t mi_reg_idx[3]     = {1, 0, 2};
const static uint32_t mi_reg_idx_inv[3] = {1, 0, 2};

// Table 6.9-1: mi value for differnt ul/dl TDD configurations
const static uint32_t mi_tdd_table[7][10] = {{2, 1, 0, 0, 0, 2, 1, 0, 0, 0},  // ul/dl 0
                                             {0, 1, 0, 0, 1, 0, 1, 0, 0, 1},  // ul/dl 1
                                             {0, 0, 0, 1, 0, 0, 0, 0, 1, 0},  // ul/dl 2
                                             {1, 0, 0, 0, 0, 0, 0, 0, 1, 1},  // ul/dl 3
                                             {0, 0, 0, 0, 0, 0, 0, 0, 1, 1},  // ul/dl 4
                                             {0, 0, 0, 0, 0, 0, 0, 0, 1, 0},  // ul/dl 5
                                             {1, 1, 0, 0, 0, 1, 1, 0, 0, 1}}; // ul/dl 6

#define MI_VALUE(sf_idx) ((q->cell.frame_type == SRSLTE_FDD) ? 1 : mi_tdd_table[sf->tdd_config.sf_config][sf_idx])
#define MI_IDX(sf_idx)                                                                                                 \
  (mi_reg_idx_inv[MI_VALUE(sf_idx)] +                                                                                  \
   ((q->cell.frame_type == SRSLTE_TDD && q->cell.phich_length == SRSLTE_PHICH_EXT && (sf_idx == 1 || sf_idx == 6))     \
        ? 3                                                                                                            \
        : 0))

int srslte_ue_dl_init(srslte_ue_dl_t* q, cf_t* in_buffer[SRSLTE_MAX_PORTS], uint32_t max_prb, uint32_t nof_rx_antennas)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q != NULL && nof_rx_antennas <= SRSLTE_MAX_PORTS) {
    ret = SRSLTE_ERROR;

    bzero(q, sizeof(srslte_ue_dl_t));

    q->pending_ul_dci_count = 0;
    q->nof_rx_antennas      = nof_rx_antennas;
    q->mi_auto              = true;
    q->mi_manual_index      = 0;
    q->pregen_rnti          = 0;

    for (int j = 0; j < SRSLTE_MAX_PORTS; j++) {
      q->sf_symbols[j] = srslte_vec_cf_malloc(MAX_SFLEN_RE);
      if (!q->sf_symbols[j]) {
        perror("malloc");
        goto clean_exit;
      }
    }

    srslte_ofdm_cfg_t ofdm_cfg = {};
    ofdm_cfg.nof_prb           = max_prb;
    ofdm_cfg.cp                = SRSLTE_CP_NORM;
    ofdm_cfg.rx_window_offset  = 0.0f;
    ofdm_cfg.normalize         = false;
    for (int i = 0; i < nof_rx_antennas; i++) {
      ofdm_cfg.in_buffer  = in_buffer[i];
      ofdm_cfg.out_buffer = q->sf_symbols[i];
      ofdm_cfg.sf_type    = SRSLTE_SF_NORM;

      if (srslte_ofdm_rx_init_cfg(&q->fft[i], &ofdm_cfg)) {
        ERROR("Error initiating FFT\n");
        goto clean_exit;
      }
    }

    ofdm_cfg.in_buffer  = in_buffer[0];
    ofdm_cfg.out_buffer = q->sf_symbols[0];
    ofdm_cfg.sf_type    = SRSLTE_SF_MBSFN;
    if (srslte_ofdm_rx_init_cfg(&q->fft_mbsfn, &ofdm_cfg)) {
      ERROR("Error initiating FFT for MBSFN subframes \n");
      goto clean_exit;
    }
    srslte_ofdm_set_non_mbsfn_region(&q->fft_mbsfn, 2); // Set a default to init

    if (srslte_chest_dl_init(&q->chest, max_prb, nof_rx_antennas)) {
      ERROR("Error initiating channel estimator\n");
      goto clean_exit;
    }
    if (srslte_chest_dl_res_init(&q->chest_res, max_prb)) {
      ERROR("Error initiating channel estimator\n");
      goto clean_exit;
    }
    if (srslte_pcfich_init(&q->pcfich, nof_rx_antennas)) {
      ERROR("Error creating PCFICH object\n");
      goto clean_exit;
    }
    if (srslte_phich_init(&q->phich, nof_rx_antennas)) {
      ERROR("Error creating PHICH object\n");
      goto clean_exit;
    }

    if (srslte_pdcch_init_ue(&q->pdcch, max_prb, nof_rx_antennas)) {
      ERROR("Error creating PDCCH object\n");
      goto clean_exit;
    }

    if (srslte_pdsch_init_ue(&q->pdsch, max_prb, nof_rx_antennas)) {
      ERROR("Error creating PDSCH object\n");
      goto clean_exit;
    }

    if (srslte_pmch_init(&q->pmch, max_prb, nof_rx_antennas)) {
      ERROR("Error creating PMCH object\n");
      goto clean_exit;
    }

    ret = SRSLTE_SUCCESS;
  } else {
    ERROR("Invalid parameters\n");
  }

clean_exit:
  if (ret == SRSLTE_ERROR) {
    srslte_ue_dl_free(q);
  }
  return ret;
}

void srslte_ue_dl_free(srslte_ue_dl_t* q)
{
  if (q) {
    for (int port = 0; port < SRSLTE_MAX_PORTS; port++) {
      srslte_ofdm_rx_free(&q->fft[port]);
    }
    srslte_ofdm_rx_free(&q->fft_mbsfn);
    srslte_chest_dl_free(&q->chest);
    srslte_chest_dl_res_free(&q->chest_res);
    for (int i = 0; i < MI_NOF_REGS; i++) {
      srslte_regs_free(&q->regs[i]);
    }
    srslte_pcfich_free(&q->pcfich);
    srslte_phich_free(&q->phich);
    srslte_pdcch_free(&q->pdcch);
    srslte_pdsch_free(&q->pdsch);
    srslte_pmch_free(&q->pmch);
    for (int j = 0; j < SRSLTE_MAX_PORTS; j++) {
      if (q->sf_symbols[j]) {
        free(q->sf_symbols[j]);
      }
    }
    bzero(q, sizeof(srslte_ue_dl_t));
  }
}

int srslte_ue_dl_set_cell(srslte_ue_dl_t* q, srslte_cell_t cell)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q != NULL && srslte_cell_isvalid(&cell)) {
    q->pending_ul_dci_count = 0;

    if (q->cell.id != cell.id || q->cell.nof_prb == 0) {
      if (q->cell.nof_prb != 0) {
        for (int i = 0; i < MI_NOF_REGS; i++) {
          srslte_regs_free(&q->regs[i]);
        }
      }
      q->cell = cell;
      for (int i = 0; i < MI_NOF_REGS; i++) {
        if (srslte_regs_init_opts(&q->regs[i], q->cell, mi_reg_idx[i % 3], i > 2)) {
          ERROR("Error resizing REGs\n");
          return SRSLTE_ERROR;
        }
      }
      for (int port = 0; port < q->nof_rx_antennas; port++) {
        if (srslte_ofdm_rx_set_prb(&q->fft[port], q->cell.cp, q->cell.nof_prb)) {
          ERROR("Error resizing FFT\n");
          return SRSLTE_ERROR;
        }
      }

      // In TDD, initialize PDCCH and PHICH for the worst case: max ncces and phich groupds respectively
      uint32_t pdcch_init_reg = 0;
      uint32_t phich_init_reg = 0;
      if (q->cell.frame_type == SRSLTE_TDD) {
        pdcch_init_reg = 1; // mi=0
        phich_init_reg = 2; // mi=2
      }

      if (srslte_ofdm_rx_set_prb(&q->fft_mbsfn, SRSLTE_CP_EXT, q->cell.nof_prb)) {
        ERROR("Error resizing MBSFN FFT\n");
        return SRSLTE_ERROR;
      }

      if (srslte_chest_dl_set_cell(&q->chest, q->cell)) {
        ERROR("Error resizing channel estimator\n");
        return SRSLTE_ERROR;
      }
      if (srslte_pcfich_set_cell(&q->pcfich, &q->regs[0], q->cell)) {
        ERROR("Error resizing PCFICH object\n");
        return SRSLTE_ERROR;
      }
      if (srslte_phich_set_cell(&q->phich, &q->regs[phich_init_reg], q->cell)) {
        ERROR("Error resizing PHICH object\n");
        return SRSLTE_ERROR;
      }

      if (srslte_pdcch_set_cell(&q->pdcch, &q->regs[pdcch_init_reg], q->cell)) {
        ERROR("Error resizing PDCCH object\n");
        return SRSLTE_ERROR;
      }

      if (srslte_pdsch_set_cell(&q->pdsch, q->cell)) {
        ERROR("Error resizing PDSCH object\n");
        return SRSLTE_ERROR;
      }

      if (srslte_pmch_set_cell(&q->pmch, q->cell)) {
        ERROR("Error resizing PMCH object\n");
        return SRSLTE_ERROR;
      }
    }
    if (q->pregen_rnti) {
      srslte_ue_dl_set_rnti(q, q->pregen_rnti);
    }
    ret = SRSLTE_SUCCESS;
  } else {
    ERROR("Invalid cell properties ue_dl: Id=%d, Ports=%d, PRBs=%d\n", cell.id, cell.nof_ports, cell.nof_prb);
  }
  return ret;
}

void srslte_ue_dl_set_non_mbsfn_region(srslte_ue_dl_t* q, uint8_t non_mbsfn_region_length)
{
  srslte_ofdm_set_non_mbsfn_region(&q->fft_mbsfn, non_mbsfn_region_length);
}

void srslte_ue_dl_set_mi_auto(srslte_ue_dl_t* q)
{
  q->mi_auto = true;
}

void srslte_ue_dl_set_mi_manual(srslte_ue_dl_t* q, uint32_t mi_idx)
{
  q->mi_auto         = false;
  q->mi_manual_index = mi_idx;
}

/* Precalculate the PDSCH scramble sequences for a given RNTI. This function takes a while
 * to execute, so shall be called once the final C-RNTI has been allocated for the session.
 * For the connection procedure, use srslte_pusch_encode_rnti() or srslte_pusch_decode_rnti() functions
 */
void srslte_ue_dl_set_rnti(srslte_ue_dl_t* q, uint16_t rnti)
{

  srslte_pdsch_set_rnti(&q->pdsch, rnti);

  srslte_dl_sf_cfg_t sf_cfg;
  ZERO_OBJECT(sf_cfg);

  // Compute UE-specific and Common search space for this RNTI
  for (int i = 0; i < MI_NOF_REGS; i++) {
    srslte_pdcch_set_regs(&q->pdcch, &q->regs[i]);
    for (int cfi = 0; cfi < 3; cfi++) {
      sf_cfg.cfi = cfi + 1;
      for (int sf_idx = 0; sf_idx < 10; sf_idx++) {
        sf_cfg.tti                                     = sf_idx;
        q->current_ss_ue[i][cfi][sf_idx].nof_locations = srslte_pdcch_ue_locations(
            &q->pdcch, &sf_cfg, q->current_ss_ue[i][cfi][sf_idx].loc, MAX_CANDIDATES_UE, rnti);
      }
      q->current_ss_common[i][cfi].nof_locations =
          srslte_pdcch_common_locations(&q->pdcch, q->current_ss_common[i][cfi].loc, MAX_CANDIDATES_COM, cfi + 1);
    }
  }
  q->pregen_rnti = rnti;
}

/* Set the area ID on pmch and chest_dl to generate scrambling sequence and reference
 * signals.
 */
int srslte_ue_dl_set_mbsfn_area_id(srslte_ue_dl_t* q, uint16_t mbsfn_area_id)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;
  if (q != NULL) {
    ret = SRSLTE_ERROR;
    if (srslte_chest_dl_set_mbsfn_area_id(&q->chest, mbsfn_area_id)) {
      ERROR("Error setting MBSFN area ID \n");
      return ret;
    }
    if (srslte_pmch_set_area_id(&q->pmch, mbsfn_area_id)) {
      ERROR("Error setting MBSFN area ID \n");
      return ret;
    }
    q->current_mbsfn_area_id = mbsfn_area_id;
    ret                      = SRSLTE_SUCCESS;
  }
  return ret;
}

static void set_mi_value(srslte_ue_dl_t* q, srslte_dl_sf_cfg_t* sf, srslte_ue_dl_cfg_t* cfg)
{
  uint32_t sf_idx = sf->tti % 10;
  // Set mi value in pdcch region
  if (q->mi_auto) {
    INFO("Setting PHICH mi value auto. sf_idx=%d, mi=%d, idx=%d\n", sf_idx, MI_VALUE(sf_idx), MI_IDX(sf_idx));
    srslte_phich_set_regs(&q->phich, &q->regs[MI_IDX(sf_idx)]);
    srslte_pdcch_set_regs(&q->pdcch, &q->regs[MI_IDX(sf_idx)]);
  } else {
    // No subframe 1 or 6 so no need to consider it
    INFO("Setting PHICH mi value manual. sf_idx=%d, mi=%d, idx=%d\n",
         sf_idx,
         q->mi_manual_index,
         mi_reg_idx_inv[q->mi_manual_index]);
    srslte_phich_set_regs(&q->phich, &q->regs[mi_reg_idx_inv[q->mi_manual_index]]);
    srslte_pdcch_set_regs(&q->pdcch, &q->regs[mi_reg_idx_inv[q->mi_manual_index]]);
  }
}

static int estimate_pdcch_pcfich(srslte_ue_dl_t* q, srslte_dl_sf_cfg_t* sf, srslte_ue_dl_cfg_t* cfg)
{
  if (q) {

    float cfi_corr = 0;

    set_mi_value(q, sf, cfg);

    /* Get channel estimates for each port */
    srslte_chest_dl_estimate_cfg(&q->chest, sf, &cfg->chest_cfg, q->sf_symbols, &q->chest_res);

    /* First decode PCFICH and obtain CFI */
    if (srslte_pcfich_decode(&q->pcfich, sf, &q->chest_res, q->sf_symbols, &cfi_corr) < 0) {
      ERROR("Error decoding PCFICH\n");
      return SRSLTE_ERROR;
    }

    if (q->cell.frame_type == SRSLTE_TDD && ((sf->tti % 10) == 1 || (sf->tti % 10) == 6) && sf->cfi == 3) {
      sf->cfi = 2;
      INFO("Received CFI=3 in subframe 1 or 6 and TDD. Setting to 2\n");
    }

    if (srslte_pdcch_extract_llr(&q->pdcch, sf, &q->chest_res, q->sf_symbols)) {
      ERROR("Extracting PDCCH LLR\n");
      return false;
    }

    INFO("Decoded CFI=%d with correlation %.2f, sf_idx=%d\n", sf->cfi, cfi_corr, sf->tti % 10);

    return SRSLTE_SUCCESS;
  } else {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }
}

int srslte_ue_dl_decode_fft_estimate(srslte_ue_dl_t* q, srslte_dl_sf_cfg_t* sf, srslte_ue_dl_cfg_t* cfg)
{
  if (q) {
    /* Run FFT for all subframe data */
    for (int j = 0; j < q->nof_rx_antennas; j++) {
      if (sf->sf_type == SRSLTE_SF_MBSFN) {
        srslte_ofdm_rx_sf(&q->fft_mbsfn);
      } else {
        srslte_ofdm_rx_sf(&q->fft[j]);
      }
    }
    return estimate_pdcch_pcfich(q, sf, cfg);
  } else {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }
}

int srslte_ue_dl_decode_fft_estimate_noguru(srslte_ue_dl_t*     q,
                                            srslte_dl_sf_cfg_t* sf,
                                            srslte_ue_dl_cfg_t* cfg,
                                            cf_t*               input[SRSLTE_MAX_PORTS])
{
  if (q && input) {
    /* Run FFT for all subframe data */
    for (int j = 0; j < q->nof_rx_antennas; j++) {
      if (sf->sf_type == SRSLTE_SF_MBSFN) {
        srslte_ofdm_rx_sf_ng(&q->fft_mbsfn, input[j], q->sf_symbols[j]);
      } else {
        srslte_ofdm_rx_sf_ng(&q->fft[j], input[j], q->sf_symbols[j]);
      }
    }
    return estimate_pdcch_pcfich(q, sf, cfg);
  } else {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }
}

static bool find_dci(srslte_dci_msg_t* dci_msg, uint32_t nof_dci_msg, srslte_dci_msg_t* match)
{
  bool     found    = false;
  uint32_t nof_bits = match->nof_bits;

  for (int k = 0; k < nof_dci_msg && !found; k++) {
    if (dci_msg[k].nof_bits == nof_bits) {
      if (memcmp(dci_msg[k].payload, match->payload, nof_bits) == 0) {
        found = true;
      }
    }
  }

  return found;
}

static int dci_blind_search(srslte_ue_dl_t*     q,
                            srslte_dl_sf_cfg_t* sf,
                            uint16_t            rnti,
                            dci_blind_search_t* search_space,
                            srslte_dci_cfg_t*   dci_cfg,
                            srslte_dci_msg_t    dci_msg[SRSLTE_MAX_DCI_MSG])
{
  uint32_t nof_dci = 0;
  if (rnti) {
    int i = 0;
    while ((dci_cfg->cif_enabled || !nof_dci) && (i < search_space->nof_locations) && (nof_dci < SRSLTE_MAX_DCI_MSG)) {
      DEBUG("Searching format %s in %d,%d (%d/%d)\n",
            srslte_dci_format_string(search_space->format),
            search_space->loc[i].ncce,
            search_space->loc[i].L,
            i,
            search_space->nof_locations);

      dci_msg[nof_dci].location = search_space->loc[i];
      dci_msg[nof_dci].format   = search_space->format;
      dci_msg[nof_dci].rnti     = 0;
      if (srslte_pdcch_decode_msg(&q->pdcch, sf, dci_cfg, &dci_msg[nof_dci])) {
        ERROR("Error decoding DCI msg\n");
        return SRSLTE_ERROR;
      }

      if ((dci_msg[nof_dci].rnti == rnti) && (dci_msg[nof_dci].nof_bits > 0)) {

        dci_msg[nof_dci].rnti = rnti;
        // If searching for Format1A but found Format0 save it for later
        if (dci_msg[nof_dci].format == SRSLTE_DCI_FORMAT0 && search_space->format == SRSLTE_DCI_FORMAT1A) {
          /* If there is space for accumulate another UL DCI dci and it was not detected before, then store it */
          if (q->pending_ul_dci_count < SRSLTE_MAX_CARRIERS &&
              !find_dci(q->pending_ul_dci_msg, q->pending_ul_dci_count, &dci_msg[nof_dci])) {
            srslte_dci_msg_t* pending_ul_dci_msg = &q->pending_ul_dci_msg[q->pending_ul_dci_count];
            pending_ul_dci_msg->format           = dci_msg[nof_dci].format;
            pending_ul_dci_msg->location         = dci_msg[nof_dci].location;
            pending_ul_dci_msg->nof_bits         = dci_msg[nof_dci].nof_bits;
            pending_ul_dci_msg->rnti             = dci_msg[nof_dci].rnti;
            memcpy(pending_ul_dci_msg->payload, dci_msg[nof_dci].payload, dci_msg[nof_dci].nof_bits);
            q->pending_ul_dci_count++;
          }
          // Else if we found it, save location and keep going if required
        } else if (dci_msg[nof_dci].format == search_space->format) {
          /* Check if the DCI is duplicated */
          if (!find_dci(dci_msg, (uint32_t)nof_dci, &dci_msg[nof_dci])) {
            nof_dci++;
          }
        }
      }
      i++;
    }
  } else {
    ERROR("RNTI not specified\n");
  }
  return nof_dci;
}

int srslte_ue_dl_find_ul_dci(srslte_ue_dl_t*     q,
                             srslte_dl_sf_cfg_t* sf,
                             srslte_ue_dl_cfg_t* dl_cfg,
                             uint16_t            rnti,
                             srslte_dci_ul_t     dci_ul[SRSLTE_MAX_DCI_MSG])
{
  srslte_dci_msg_t dci_msg[SRSLTE_MAX_DCI_MSG];
  uint32_t         nof_msg = 0;

  if (rnti) {
    /* Do not search if an UL DCI is already pending */
    if (q->pending_ul_dci_count) {
      nof_msg                 = SRSLTE_MIN(SRSLTE_MAX_DCI_MSG, q->pending_ul_dci_count);
      q->pending_ul_dci_count = 0;
      memcpy(dci_msg, q->pending_ul_dci_msg, sizeof(srslte_dci_msg_t) * nof_msg);
    } else {

      uint32_t sf_idx = sf->tti % 10;
      uint32_t cfi    = sf->cfi;

      set_mi_value(q, sf, dl_cfg);

      // Configure and run DCI blind search
      dci_blind_search_t search_space;
      search_space.nof_locations     = 0;
      dci_blind_search_t* current_ss = &search_space;
      if (q->pregen_rnti == rnti) {
        current_ss = &q->current_ss_ue[MI_IDX(sf_idx)][cfi - 1][sf_idx];
      } else {
        // If locations are not pre-generated, generate them now
        current_ss->nof_locations = srslte_pdcch_ue_locations(&q->pdcch, sf, current_ss->loc, MAX_CANDIDATES_UE, rnti);
      }

      current_ss->format = SRSLTE_DCI_FORMAT0;
      INFO("Searching UL C-RNTI in %d ue locations\n", search_space.nof_locations);
      nof_msg = dci_blind_search(q, sf, rnti, current_ss, &dl_cfg->cfg.dci, dci_msg);
    }

    // Unpack DCI messages
    for (uint32_t i = 0; i < nof_msg; i++) {
      if (srslte_dci_msg_unpack_pusch(&q->cell, sf, &dl_cfg->cfg.dci, &dci_msg[i], &dci_ul[i])) {
        ERROR("Unpacking UL DCI\n");
        return SRSLTE_ERROR;
      }
    }

    return nof_msg;

  } else {
    return 0;
  }
}

// Blind search for SI/P/RA-RNTI
static int find_dl_dci_type_siprarnti(srslte_ue_dl_t*     q,
                                      srslte_dl_sf_cfg_t* sf,
                                      srslte_ue_dl_cfg_t* dl_cfg,
                                      uint16_t            rnti,
                                      srslte_dci_msg_t    dci_msg[SRSLTE_MAX_DCI_MSG])
{
  int ret = 0;

  srslte_dci_cfg_t dci_cfg = dl_cfg->cfg.dci;

  // Configure and run DCI blind search
  dci_blind_search_t search_space;
  search_space.nof_locations = srslte_pdcch_common_locations(&q->pdcch, search_space.loc, MAX_CANDIDATES_COM, sf->cfi);
  INFO("Searching SI/P/RA-RNTI in %d common locations, %d formats, tti=%d, cfi=%d, rnti=0x%x\n",
       search_space.nof_locations,
       nof_common_formats,
       sf->tti,
       sf->cfi,
       rnti);

  // Disable extended CSI request and SRS request for non C-RNTI
  dci_cfg.multiple_csi_request_enabled = false;
  dci_cfg.srs_request_enabled          = false;

  // Search for RNTI only if there is room for the common search space
  if (search_space.nof_locations > 0) {
    for (uint32_t f = 0; f < nof_common_formats; f++) {
      search_space.format = common_formats[f];
      if ((ret = dci_blind_search(q, sf, rnti, &search_space, &dci_cfg, dci_msg))) {
        return ret;
      }
    }
  }
  return SRSLTE_SUCCESS;
}

// Blind search for C-RNTI
static int find_dl_dci_type_crnti(srslte_ue_dl_t*     q,
                                  srslte_dl_sf_cfg_t* sf,
                                  srslte_ue_dl_cfg_t* cfg,
                                  uint16_t            rnti,
                                  srslte_dci_msg_t    dci_msg[SRSLTE_MAX_DCI_MSG])
{
  int                 ret = SRSLTE_SUCCESS;
  dci_blind_search_t  search_space;
  dci_blind_search_t* current_ss = &search_space;

  uint32_t         sf_idx  = sf->tti % 10;
  uint32_t         cfi     = sf->cfi;
  srslte_dci_cfg_t dci_cfg = cfg->cfg.dci;

  // Search first Common SS

  // Disable extended CSI request and SRS request in common SS
  dci_cfg.multiple_csi_request_enabled = false;
  dci_cfg.srs_request_enabled          = false;

  // Search Format 1A in the Common SS also
  if (q->pregen_rnti == rnti) {
    current_ss = &q->current_ss_common[MI_IDX(sf_idx)][cfi - 1];
  } else {
    // If locations are not pre-generated, generate them now
    current_ss->nof_locations = srslte_pdcch_common_locations(&q->pdcch, current_ss->loc, MAX_CANDIDATES_COM, cfi);
  }

  // Search for RNTI only if there is room for the common search space
  if (current_ss->nof_locations > 0) {
    current_ss->format = SRSLTE_DCI_FORMAT1A;
    INFO("Searching DL C-RNTI in %d ue locations, format 1A\n", current_ss->nof_locations);
    if ((ret = dci_blind_search(q, sf, rnti, current_ss, &dci_cfg, dci_msg))) {
      return ret;
    }
  }

  // Search UE-specific search space
  dci_cfg = cfg->cfg.dci;
  if (q->pregen_rnti == rnti) {
    current_ss = &q->current_ss_ue[MI_IDX(sf_idx)][cfi - 1][sf_idx];
  } else {
    // If locations are not pre-generated, generate them now
    current_ss->nof_locations = srslte_pdcch_ue_locations(&q->pdcch, sf, current_ss->loc, MAX_CANDIDATES_UE, rnti);
  }

  if (cfg->cfg.tm > SRSLTE_TM8) {
    ERROR("Searching DL CRNTI: Invalid TM=%d\n", cfg->cfg.tm + 1);
    return SRSLTE_ERROR;
  }

  for (int f = 0; f < 2; f++) {
    srslte_dci_format_t format = ue_dci_formats[cfg->cfg.tm][f];

    INFO("Searching DL C-RNTI %s in %d ue locations\n", srslte_dci_format_string(format), current_ss->nof_locations);

    current_ss->format = format;
    if ((ret = dci_blind_search(q, sf, rnti, current_ss, &dci_cfg, dci_msg))) {
      return ret;
    }
  }

  return SRSLTE_SUCCESS;
}

int srslte_ue_dl_find_dl_dci(srslte_ue_dl_t*     q,
                             srslte_dl_sf_cfg_t* sf,
                             srslte_ue_dl_cfg_t* dl_cfg,
                             uint16_t            rnti,
                             srslte_dci_dl_t     dci_dl[SRSLTE_MAX_DCI_MSG])
{
  set_mi_value(q, sf, dl_cfg);

  srslte_dci_msg_t dci_msg[SRSLTE_MAX_DCI_MSG] = {};

  int nof_msg = 0;
  if (rnti == SRSLTE_SIRNTI || rnti == SRSLTE_PRNTI || SRSLTE_RNTI_ISRAR(rnti)) {
    nof_msg = find_dl_dci_type_siprarnti(q, sf, dl_cfg, rnti, dci_msg);
  } else {
    nof_msg = find_dl_dci_type_crnti(q, sf, dl_cfg, rnti, dci_msg);
  }

  if (nof_msg < 0) {
    ERROR("Invalid number of DCI messages\n");
    return SRSLTE_ERROR;
  }

  // Unpack DCI messages
  for (uint32_t i = 0; i < nof_msg; i++) {
    if (srslte_dci_msg_unpack_pdsch(&q->cell, sf, &dl_cfg->cfg.dci, &dci_msg[i], &dci_dl[i])) {
      ERROR("Unpacking DL DCI\n");
      return SRSLTE_ERROR;
    }
  }
  return nof_msg;
}

int srslte_ue_dl_dci_to_pdsch_grant(srslte_ue_dl_t*       q,
                                    srslte_dl_sf_cfg_t*   sf,
                                    srslte_ue_dl_cfg_t*   cfg,
                                    srslte_dci_dl_t*      dci,
                                    srslte_pdsch_grant_t* grant)
{
  return srslte_ra_dl_dci_to_grant(&q->cell, sf, cfg->cfg.tm, cfg->cfg.pdsch.use_tbs_index_alt, dci, grant);
}

int srslte_ue_dl_decode_pdsch(srslte_ue_dl_t*     q,
                              srslte_dl_sf_cfg_t* sf,
                              srslte_pdsch_cfg_t* pdsch_cfg,
                              srslte_pdsch_res_t  data[SRSLTE_MAX_CODEWORDS])
{
  return srslte_pdsch_decode(&q->pdsch, sf, pdsch_cfg, &q->chest_res, q->sf_symbols, data);
}

int srslte_ue_dl_decode_pmch(srslte_ue_dl_t*     q,
                             srslte_dl_sf_cfg_t* sf,
                             srslte_pmch_cfg_t*  pmch_cfg,
                             srslte_pdsch_res_t  data[SRSLTE_MAX_CODEWORDS])
{
  return srslte_pmch_decode(&q->pmch, sf, pmch_cfg, &q->chest_res, q->sf_symbols, &data[0]);
}

int srslte_ue_dl_decode_phich(srslte_ue_dl_t*       q,
                              srslte_dl_sf_cfg_t*   sf,
                              srslte_ue_dl_cfg_t*   cfg,
                              srslte_phich_grant_t* grant,
                              srslte_phich_res_t*   result)
{
  srslte_phich_resource_t n_phich;

  uint32_t sf_idx = sf->tti % 10;

  set_mi_value(q, sf, cfg);

  srslte_phich_calc(&q->phich, grant, &n_phich);
  INFO("Decoding PHICH sf_idx=%d, n_prb_lowest=%d, n_dmrs=%d, I_phich=%d, n_group=%d, n_seq=%d, Ngroups=%d, Nsf=%d\n",
       sf_idx,
       grant->n_prb_lowest,
       grant->n_dmrs,
       grant->I_phich,
       n_phich.ngroup,
       n_phich.nseq,
       srslte_phich_ngroups(&q->phich),
       srslte_phich_nsf(&q->phich));

  if (!srslte_phich_decode(&q->phich, sf, &q->chest_res, n_phich, q->sf_symbols, result)) {
    INFO("Decoded PHICH %d with distance %f\n", result->ack_value, result->distance);
    return 0;
  } else {
    ERROR("Error decoding PHICH\n");
    return -1;
  }
}

/* Compute the Rank Indicator (RI) and Precoder Matrix Indicator (PMI) by computing the Signal to Interference plus
 * Noise Ratio (SINR), valid for TM4 */
static int select_pmi(srslte_ue_dl_t* q, uint32_t ri, uint32_t* pmi, float* sinr_db)
{
  uint32_t best_pmi = 0;
  float    sinr_list[SRSLTE_MAX_CODEBOOKS];

  if (q->cell.nof_ports < 2) {
    /* Do nothing */
    return SRSLTE_SUCCESS;
  } else {
    if (srslte_pdsch_select_pmi(&q->pdsch, &q->chest_res, ri + 1, &best_pmi, sinr_list)) {
      DEBUG("SINR calculation error");
      return SRSLTE_ERROR;
    }

    /* Set PMI */
    if (pmi != NULL) {
      *pmi = best_pmi;
    }

    /* Set PMI */
    if (sinr_db != NULL) {
      *sinr_db = srslte_convert_power_to_dB(sinr_list[best_pmi % SRSLTE_MAX_CODEBOOKS]);
    }
  }

  return SRSLTE_SUCCESS;
}

static int select_ri_pmi(srslte_ue_dl_t* q, uint32_t* ri, uint32_t* pmi, float* sinr_db)
{
  float    best_sinr_db = -INFINITY;
  uint32_t best_pmi = 0, best_ri = 0;
  uint32_t max_ri = SRSLTE_MIN(q->nof_rx_antennas, q->cell.nof_ports);

  if (q->cell.nof_ports < 2) {
    /* Do nothing */
    return SRSLTE_SUCCESS;
  } else {
    /* Select the best Rank indicator (RI) and Precoding Matrix Indicator (PMI) */
    for (uint32_t this_ri = 0; this_ri < max_ri; this_ri++) {
      uint32_t this_pmi     = 0;
      float    this_sinr_db = 0.0f;
      if (select_pmi(q, this_ri, &this_pmi, &this_sinr_db)) {
        DEBUG("SINR calculation error");
        return SRSLTE_ERROR;
      }

      /* Find best SINR, force maximum number of layers if SNR is higher than 30 dB */
      if (this_sinr_db > best_sinr_db + 0.1 || this_sinr_db > 20.0) {
        best_sinr_db = this_sinr_db;
        best_pmi     = this_pmi;
        best_ri      = this_ri;
      }
    }
  }

  /* Set RI */
  if (ri != NULL) {
    *ri = best_ri;
  }

  /* Set PMI */
  if (pmi != NULL) {
    *pmi = best_pmi;
  }

  /* Set SINR */
  if (sinr_db != NULL) {
    *sinr_db = best_sinr_db;
  }

  return SRSLTE_SUCCESS;
}

/* Compute the Rank Indicator (RI) by computing the condition number, valid for TM3 */
int srslte_ue_dl_select_ri(srslte_ue_dl_t* q, uint32_t* ri, float* cn)
{
  float _cn = INFINITY;
  int   ret = srslte_pdsch_compute_cn(&q->pdsch, &q->chest_res, &_cn);

  if (ret == SRSLTE_SUCCESS) {
    /* Set Condition number */
    if (cn) {
      *cn = _cn;
    }

    /* Set rank indicator */
    if (ri) {
      *ri = (uint8_t)((_cn < 17.0f) ? 1 : 0);
    }
  }

  return ret;
}

void srslte_ue_dl_gen_cqi_periodic(srslte_ue_dl_t*     q,
                                   srslte_ue_dl_cfg_t* cfg,
                                   uint32_t            wideband_value,
                                   uint32_t            tti,
                                   srslte_uci_data_t*  uci_data)
{
  if (srslte_cqi_periodic_ri_send(&cfg->cfg.cqi_report, tti, q->cell.frame_type)) {
    /* Compute RI, PMI and SINR */
    if (q->nof_rx_antennas > 1) {
      if (cfg->cfg.tm == SRSLTE_TM3) {
        srslte_ue_dl_select_ri(q, &cfg->last_ri, NULL);
      } else if (cfg->cfg.tm == SRSLTE_TM4) {
        select_ri_pmi(q, &cfg->last_ri, NULL, NULL);
      }
    } else {
      cfg->last_ri = 0;
    }
    uci_data->cfg.cqi.ri_len = 1;
    uci_data->value.ri       = cfg->last_ri;
  } else if (srslte_cqi_periodic_send(&cfg->cfg.cqi_report, tti, q->cell.frame_type)) {

    if (cfg->cfg.cqi_report.format_is_subband) {
      // TODO: Implement subband periodic reports
      uci_data->cfg.cqi.type                    = SRSLTE_CQI_TYPE_SUBBAND;
      uci_data->value.cqi.subband.subband_cqi   = wideband_value;
      uci_data->value.cqi.subband.subband_label = 0;
    } else {
      uci_data->cfg.cqi.type                    = SRSLTE_CQI_TYPE_WIDEBAND;
      uci_data->value.cqi.wideband.wideband_cqi = wideband_value;
      if (cfg->cfg.tm == SRSLTE_TM4) {
        uint32_t pmi = 0;
        select_pmi(q, cfg->last_ri, &pmi, NULL);

        uci_data->cfg.cqi.pmi_present     = true;
        uci_data->cfg.cqi.rank_is_not_one = (cfg->last_ri != 0);
        uci_data->value.cqi.wideband.pmi  = (uint8_t)pmi;
      }
    }
    uci_data->cfg.cqi.data_enable = true;
    uci_data->cfg.cqi.ri_len      = 0;
    uci_data->value.ri            = cfg->last_ri;
  }
}

void srslte_ue_dl_gen_cqi_aperiodic(srslte_ue_dl_t*     q,
                                    srslte_ue_dl_cfg_t* cfg,
                                    uint32_t            wideband_value,
                                    srslte_uci_data_t*  uci_data)
{
  uint32_t pmi     = 0;
  float    sinr_db = 0.0f;

  switch (cfg->cfg.cqi_report.aperiodic_mode) {
    case SRSLTE_CQI_MODE_30:
      /* only Higher Layer-configured subband feedback support right now, according to TS36.213 section 7.2.1
        - A UE shall report a wideband CQI value which is calculated assuming transmission on set S subbands
        - The UE shall also report one subband CQI value for each set S subband. The subband CQI
          value is calculated assuming transmission only in the subband
        - Both the wideband and subband CQI represent channel quality for the first codeword,
          even when RI>1
        - For transmission mode 3 the reported CQI values are calculated conditioned on the
          reported RI. For other transmission modes they are reported conditioned on rank 1.
      */

      uci_data->cfg.cqi.type                          = SRSLTE_CQI_TYPE_SUBBAND_HL;
      uci_data->value.cqi.subband_hl.wideband_cqi_cw0 = wideband_value;

      // TODO: implement subband CQI properly
      uci_data->value.cqi.subband_hl.subband_diff_cqi_cw0 = 0; // Always report zero offset on all subbands
      uci_data->cfg.cqi.N = (q->cell.nof_prb > 7) ? (uint32_t)srslte_cqi_hl_get_no_subbands(q->cell.nof_prb) : 0;
      uci_data->cfg.cqi.data_enable = true;

      /* Set RI = 1 */
      if (cfg->cfg.tm == SRSLTE_TM3 || cfg->cfg.tm == SRSLTE_TM4) {
        if (q->nof_rx_antennas > 1) {
          srslte_ue_dl_select_ri(q, &cfg->last_ri, NULL);
          uci_data->value.ri       = (uint8_t)cfg->last_ri;
          uci_data->cfg.cqi.ri_len = 1;
        } else {
          uci_data->value.ri = 0;
        }
      } else {
        uci_data->cfg.cqi.ri_len = 0;
      }

      break;
    case SRSLTE_CQI_MODE_31:
      /* only Higher Layer-configured subband feedback support right now, according to TS36.213 section 7.2.1
        - A single precoding matrix is selected from the codebook subset assuming transmission on set S subbands
        - A UE shall report one subband CQI value per codeword for each set S subband which are calculated assuming
          the use of the single precoding matrix in all subbands and assuming transmission in the corresponding
          subband.
        - A UE shall report a wideband CQI value per codeword which is calculated assuming the use of the single
          precoding matrix in all subbands and transmission on set S subbands
        - The UE shall report the single selected precoding matrix indicator.
        - For transmission mode 4 the reported PMI and CQI values are calculated conditioned on the reported RI. For
          other transmission modes they are reported conditioned on rank 1.
      */
      /* Loads the latest SINR according to the calculated RI and PMI */
      pmi     = 0;
      sinr_db = 0.0f;
      select_ri_pmi(q, &cfg->last_ri, &pmi, &sinr_db);

      /* Fill CQI Report */
      uci_data->cfg.cqi.type = SRSLTE_CQI_TYPE_SUBBAND_HL;

      uci_data->value.cqi.subband_hl.wideband_cqi_cw0     = srslte_cqi_from_snr(sinr_db + cfg->snr_to_cqi_offset);
      uci_data->value.cqi.subband_hl.subband_diff_cqi_cw0 = 0; // Always report zero offset on all subbands

      if (cfg->last_ri > 0) {
        uci_data->cfg.cqi.rank_is_not_one                   = true;
        uci_data->value.cqi.subband_hl.wideband_cqi_cw1     = srslte_cqi_from_snr(sinr_db + cfg->snr_to_cqi_offset);
        uci_data->value.cqi.subband_hl.subband_diff_cqi_cw1 = 0; // Always report zero offset on all subbands
      }

      uci_data->value.cqi.subband_hl.pmi   = pmi;
      uci_data->cfg.cqi.pmi_present        = true;
      uci_data->cfg.cqi.four_antenna_ports = (q->cell.nof_ports == 4);
      uci_data->cfg.cqi.N = (uint32_t)((q->cell.nof_prb > 7) ? srslte_cqi_hl_get_no_subbands(q->cell.nof_prb) : 0);

      uci_data->cfg.cqi.data_enable = true;
      uci_data->cfg.cqi.ri_len      = 1;
      uci_data->value.ri            = cfg->last_ri;

      break;
    default:
      ERROR("CQI mode %d not supported\n", cfg->cfg.cqi_report.aperiodic_mode);
      break;
  }
}

static void ue_dl_gen_ack_fdd_none(const srslte_pdsch_ack_t* ack_info, srslte_uci_data_t* uci_data)
{
  // Set all carriers number of ACKs to 0
  for (uint32_t i = 0; i < ack_info->nof_cc; i++) {
    uci_data->cfg.ack[i].nof_acks = 0;
  }
}

static void
ue_dl_gen_ack_fdd_pcell_skip_drx(const srslte_pdsch_ack_t* ack_info, srslte_uci_data_t* uci_data, uint32_t nof_tb)
{
  uint32_t ack_idx = 0;

  // Find ACK/NACK
  if (ack_info->cc[0].m[0].present) {
    for (uint32_t tb = 0; tb < nof_tb; tb++) {
      if (ack_info->cc[0].m[0].value[tb] != 2) {
        uci_data->value.ack.ack_value[ack_idx] = ack_info->cc[0].m[0].value[tb];
        ack_idx++;
      }
    }
  }

  // Set number of ACKs for PCell
  uci_data->cfg.ack[0].nof_acks = ack_idx;

  // Set rest of carriers to 0 ACKs
  for (uint32_t i = 1; i < ack_info->nof_cc; i++) {
    uci_data->cfg.ack[i].nof_acks = 0;
  }
}

static void
ue_dl_gen_ack_fdd_all_keep_drx(const srslte_pdsch_ack_t* ack_info, srslte_uci_data_t* uci_data, uint32_t nof_tb)
{

  for (uint32_t cc_idx = 0; cc_idx < ack_info->nof_cc; cc_idx++) {
    // Find ACK/NACK
    if (ack_info->cc[cc_idx].m[0].present) {
      for (uint32_t tb = 0; tb < nof_tb; tb++) {
        if (ack_info->cc[cc_idx].m[0].value[tb] != 2) {
          uci_data->value.ack.ack_value[cc_idx * nof_tb + tb] = ack_info->cc[cc_idx].m[0].value[tb];
        }
      }
    }

    // Set all carriers to maximum number of TBs
    uci_data->cfg.ack[cc_idx].nof_acks = nof_tb;
  }
}

static void
ue_dl_gen_ack_fdd_all_spatial_bundling(const srslte_pdsch_ack_t* ack_info, srslte_uci_data_t* uci_data, uint32_t nof_tb)
{
  uint32_t nof_ack = 0;

  for (uint32_t cc_idx = 0; cc_idx < ack_info->nof_cc; cc_idx++) {
    if (ack_info->cc[cc_idx].m[0].present) {
      uci_data->value.ack.ack_value[cc_idx] = 1;
      for (uint32_t tb = 0; tb < nof_tb; tb++) {
        if (ack_info->cc[cc_idx].m[0].value[tb] != 2) {
          uci_data->value.ack.ack_value[cc_idx] &= ack_info->cc[cc_idx].m[0].value[tb];
          nof_ack++;
        }
      }
    } else {
      uci_data->value.ack.ack_value[cc_idx] = 2;
    }
  }

  // If no ACK is counted, set all zero, bundle otherwise
  for (uint32_t i = 0; i < SRSLTE_PUCCH_CS_MAX_CARRIERS; i++) {
    uci_data->cfg.ack[i].nof_acks = (nof_ack == 0) ? 0 : 1;
  }
}

/* UE downlink procedure for reporting HARQ-ACK bits in FDD, Section 7.3 36.213
 */
static void gen_ack_fdd(const srslte_pdsch_ack_t* ack_info, srslte_uci_data_t* uci_data)
{
  // Number of transport blocks for the current Transmission Mode
  uint32_t nof_tb = 1;
  if (ack_info->transmission_mode > SRSLTE_TM2) {
    nof_tb = SRSLTE_MAX_CODEWORDS;
  }

  // Count number of transmissions
  uint32_t tb_count     = 0; // All transmissions
  uint32_t tb_count_cc0 = 0; // Transmissions on PCell
  for (uint32_t cc_idx = 0; cc_idx < ack_info->nof_cc; cc_idx++) {
    for (uint32_t tb = 0; tb < nof_tb; tb++) {
      if (ack_info->cc[cc_idx].m[0].present && ack_info->cc[cc_idx].m[0].value[tb] != 2) {
        tb_count++;
      }

      // Save primary cell number of TB
      if (cc_idx == 0) {
        tb_count_cc0 = tb_count;
      }
    }
  }

  // if no transmission counted return without reporting any ACK/NACK
  if (tb_count == 0) {
    ue_dl_gen_ack_fdd_none(ack_info, uci_data);
    return;
  }

  // Count total of Uplink Control Bits
  uint32_t total_uci_bits =
      tb_count + srslte_cqi_size(&uci_data->cfg.cqi) + (uci_data->value.scheduling_request ? 1 : 0);

  // Does CSI report need to be transmitted?
  bool csi_report = uci_data->cfg.cqi.data_enable || uci_data->cfg.cqi.ri_len;

  // Logic for dropping CSI report if required
  if (csi_report && !ack_info->is_pusch_available) {
    bool drop_csi_report = true; ///< CSI report shall be dropped by default

    // 3GPP 36.213 R.15 Section 10.1.1:
    // For FDD or for FDD-TDD and primary cell frame structure type 1 and for a UE that is configured with more than
    // one serving cell, in case of collision between a periodic CSI report and an HARQ-ACK in a same subframe without
    // PUSCH,

    // - if the parameter simultaneousAckNackAndCQI provided by higher layers is set TRUE and if the HARQ-ACK
    //   corresponds to a PDSCH transmission or PDCCH/EPDCCH indicating downlink SPS release only on the
    //   primary cell, then the periodic CSI report is multiplexed with HARQ-ACK on PUCCH using PUCCH format 2/2a/2b
    drop_csi_report &= !(tb_count_cc0 == tb_count && ack_info->simul_cqi_ack);

    // - else if the UE is configured with PUCCH format 3 and if the parameter simultaneousAckNackAndCQI-Format3-
    //   r11 provided by higher layers is set TRUE, and if PUCCH resource is determined according to subclause
    //   10.1.2.2.2, and
    //   - if the total number of bits in the subframe corresponding to HARQ-ACKs, SR (if any), and the CSI is not
    //     larger than 22 or
    //   - if the total number of bits in the subframe corresponding to spatially bundled HARQ-ACKs, SR (if any),
    //     and the CSI is not larger than 22 then the periodic CSI report is multiplexed with HARQ-ACK on PUCCH
    //     using the determined PUCCH format 3 resource according to [4]
    drop_csi_report &= !(ack_info->simul_cqi_ack_pucch3 && total_uci_bits <= 22);

    // - otherwise, CSI is dropped
    if (drop_csi_report) {
      uci_data->cfg.cqi.data_enable = false;
      uci_data->cfg.cqi.ri_len      = 0;
      csi_report                    = false;
    }
  }

  // For each HARQ ACK/NACK feedback mode
  switch (ack_info->ack_nack_feedback_mode) {

    case SRSLTE_PUCCH_ACK_NACK_FEEDBACK_MODE_NORMAL:
      // Get ACK from PCell only, skipping DRX
      ue_dl_gen_ack_fdd_pcell_skip_drx(ack_info, uci_data, nof_tb);
      break;
    case SRSLTE_PUCCH_ACK_NACK_FEEDBACK_MODE_CS:
      // Configured with more than serving cell and PUCCH Format 1b with channel selection
      if (ack_info->nof_cc == 1) {
        ue_dl_gen_ack_fdd_pcell_skip_drx(ack_info, uci_data, nof_tb);
      } else if (ack_info->is_pusch_available) {
        ue_dl_gen_ack_fdd_all_keep_drx(ack_info, uci_data, nof_tb);
      } else if (uci_data->value.scheduling_request) {
        // For FDD with PUCCH format 1b with channel selection, when both HARQ-ACK and SR are transmitted in the same
        // sub-frame a UE shall transmit the HARQ-ACK on its assigned HARQ-ACK PUCCH resource with channel selection as
        // defined in subclause 10.1.2.2.1 for a negative SR transmission and transmit one HARQ-ACK bit per serving cell
        // on its assigned SR PUCCH resource for a positive SR transmission according to the following:
        // − if only one transport block or a PDCCH indicating downlink SPS release is detected on a serving cell, the
        //   HARQ-ACK bit for the serving cell is the HARQ-ACK bit corresponding to the transport block or the PDCCH
        //   indicating downlink SPS release;
        // − if two transport blocks are received on a serving cell, the HARQ-ACK bit for the serving cell is generated
        //   by spatially bundling the HARQ-ACK bits corresponding to the transport blocks;
        // − if neither PDSCH transmission for which HARQ-ACK response shall be provided nor PDCCH indicating
        //   downlink SPS release is detected for a serving cell, the HARQ-ACK bit for the serving cell is set to NACK;
        ue_dl_gen_ack_fdd_all_spatial_bundling(ack_info, uci_data, nof_tb);
      } else if (csi_report) {
        ue_dl_gen_ack_fdd_pcell_skip_drx(ack_info, uci_data, nof_tb);
      } else {
        ue_dl_gen_ack_fdd_all_keep_drx(ack_info, uci_data, nof_tb);
      }
      break;
    case SRSLTE_PUCCH_ACK_NACK_FEEDBACK_MODE_PUCCH3:
      // According to 3GPP 36.213 Section 10.1.2.2.2 PUCCH format 3 HARQ-ACK procedure
      // For FDD with PUCCH format 3, the UE shall use PUCCH resource n_pucch_3 or n_pucch_1 for transmission of
      // HARQ-ACK in subframe n where
      // - for a PDSCH transmission only on the primary cell indicated by the detection of a corresponding PDCCH in
      //   subframe n − 4 , or for a PDCCH indicating downlink SPS release (defined in subclause 9.2) in subframe n − 4
      //   on the primary cell, the UE shall use PUCCH format 1a/1b and PUCCH resource n_pucch_1.
      // - for a PDSCH transmission only on the primary cell where there is not a corresponding PDCCH detected on
      //   subframe n - 4, the UE shall use PUCCH format 1a/1b and PUCCH resource n_pucch_1 where the value of n_pucch_1
      //   is determined according to higher layer configuration and Table 9.2-2.
      // - for a PDSCH transmission on the secondary cell indicated by the detection of a corresponding PDCCH in
      //   subframe n − 4 , the UE shall use PUCCH format 3 and PUCCH resource n_pucch_3  where the value of n PUCCH
      //   is determined according to higher layer configuration and Table 10.1.2.2.2-1.
      if (tb_count == tb_count_cc0) {
        ue_dl_gen_ack_fdd_pcell_skip_drx(ack_info, uci_data, nof_tb);
      } else {
        ue_dl_gen_ack_fdd_all_keep_drx(ack_info, uci_data, nof_tb);
      }
      break;
    case SRSLTE_PUCCH_ACK_NACK_FEEDBACK_MODE_ERROR:
    default:; // Do nothing
      break;
  }

  // n_cce values are just copied
  for (uint32_t i = 0; i < ack_info->nof_cc; i++) {
    uci_data->cfg.ack[i].ncce[0]       = ack_info->cc[i].m[0].resource.n_cce;
    uci_data->cfg.ack[i].grant_cc_idx  = ack_info->cc[i].m[0].resource.grant_cc_idx;
    uci_data->cfg.ack[i].tpc_for_pucch = ack_info->cc[i].m[0].resource.tpc_for_pucch;
  }
}

// Table 7.3-1
static const uint32_t multiple_acknack[10][2] =
    {{0, 0}, {1, 1}, {1, 0}, {0, 1}, {1, 1}, {1, 0}, {0, 1}, {1, 1}, {1, 0}, {0, 1}};

/* UE downlink procedure for reporting HARQ-ACK bits in TDD, Section 7.3 36.213
 */
static void gen_ack_tdd(bool is_tdd_mode16, const srslte_pdsch_ack_t* ack_info, srslte_uci_data_t* uci_data)
{
  uint32_t V_dai_dl = 0;

  uint32_t nof_tb = 1;
  if (ack_info->transmission_mode > SRSLTE_TM2) {
    nof_tb = SRSLTE_MAX_CODEWORDS;
  }

  if (ack_info->nof_cc > 1) {
    fprintf(stderr, "Error generating HARQ-ACK bits. Only 1 CC is supported in TDD\n");
  }

  // Arrange bits for FDD or TDD Bundling or Multiplexing.
  const srslte_pdsch_ack_cc_t* ack_value = &ack_info->cc[0];
  srslte_uci_cfg_ack_t*        ack_cfg   = &uci_data->cfg.ack[0];

  uint32_t min_k = 10;

  if (ack_value->M > 0) {

    ack_cfg->tdd_ack_M = ack_value->M;

    // ACK/NACK bundling or multiplexing and M=1
    if (!ack_info->tdd_ack_multiplex || ack_value->M == 1) {
      for (uint32_t tb = 0; tb < nof_tb; tb++) {
        bool first_in_bundle = true;
        for (uint32_t k = 0; k < ack_value->M; k++) {
          if (ack_value->m[k].present && ack_value->m[k].value[tb] != 2) {
            // Bundle on time domain
            if (first_in_bundle) {
              uci_data->value.ack.ack_value[tb] = ack_value->m[k].value[tb];
              first_in_bundle                   = false;
            } else {
              uci_data->value.ack.ack_value[tb] =
                  (uint8_t)(((uci_data->value.ack.ack_value[tb] == 1) & (ack_value->m[k].value[tb])) ? 1 : 0);
            }
            // V_dai_dl is for the one with lowest k value
            if (ack_value->m[k].k < min_k) {
              min_k              = ack_value->m[k].k;
              V_dai_dl           = ack_value->m[k].resource.v_dai_dl + 1; // Table 7.3-X
              ack_cfg->ncce[0]   = ack_value->m[k].resource.n_cce;
              ack_cfg->tdd_ack_m = k;
            }
          }
        }
      }
      // ACK/NACK multiplexing and M > 1
    } else {
      for (uint32_t k = 0; k < ack_value->M; k++) {
        // Bundle spatial domain
        bool spatial_ack = true;
        for (uint32_t i = 0; i < nof_tb; i++) {
          if (ack_value->m[k].value[i] != 2) {
            spatial_ack &= (ack_value->m[k].value[i] == 1);
          }
        }
        // In multiplexing for pusch, sort them accordingly
        if (ack_value->m[k].present) {
          uint32_t p = k;
          if (ack_info->is_pusch_available && ack_info->is_grant_available) {
            p = ack_value->m[k].resource.v_dai_dl;
          }
          uci_data->value.ack.ack_value[p] = (uint8_t)(spatial_ack ? 1 : 0);
          ack_cfg->ncce[p]                 = ack_value->m[k].resource.n_cce;
        }
      }
    }
  }

  bool missing_ack = false;

  // Calculate U_dai and count number of ACK for this subframe by spatial bundling across codewords
  uint32_t nof_pos_acks   = 0;
  uint32_t U_dai          = 0;
  uint32_t nof_total_acks = 0;
  for (uint32_t i = 0; i < ack_value->M; i++) {
    bool bundle_spatial = false;
    bool first_bundle   = true;
    for (uint32_t j = 0; j < nof_tb; j++) {
      if (ack_value->m[i].present) {
        if (first_bundle) {
          bundle_spatial = ack_value->m[i].value[j] == 1;
          U_dai++;
          first_bundle = false;
        } else {
          bundle_spatial &= ack_value->m[i].value[j] == 1;
        }
        if (bundle_spatial) {
          nof_pos_acks++;
        }
        if (ack_value->m[i].value[j] != 2) {
          nof_total_acks++;
        }
      }
    }
  }

  // For TDD PUSCH
  if (is_tdd_mode16) {

    uint32_t V_dai_ul = ack_info->V_dai_ul + 1; // Table 7.3-x

    ack_cfg->tdd_is_multiplex = ack_info->tdd_ack_multiplex;

    // Bundling or multiplexing and M=1
    if (!ack_info->tdd_ack_multiplex || ack_info->cc[0].M == 1) {
      // 1 or 2 ACK/NACK bits
      ack_cfg->nof_acks = nof_tb;

      // Determine if there is any missing ACK/NACK in the set and N_bundle value

      // Case not transmitting on PUSCH
      if (!ack_info->is_pusch_available) {
        if ((V_dai_dl != (U_dai - 1) % 4 + 1 && U_dai > 0) || U_dai == 0) {
          // In ul procedure 10.2, skip ACK/NACK in bundling PUCCH
          ack_cfg->nof_acks = 0;
          if (U_dai > 0) {
            missing_ack = true;
          }
        }
        // Transmitting on PUSCH and based on detected PDCCH
      } else if (ack_info->is_grant_available) {
        if (V_dai_ul != (U_dai - 1) % 4 + 1) {
          bzero(uci_data->value.ack.ack_value, nof_tb);
          ack_cfg->N_bundle = V_dai_ul + 2;
        } else {
          ack_cfg->N_bundle = V_dai_ul;
        }
        // do not transmit case
        if (V_dai_ul == 4 && U_dai == 0) {
          ack_cfg->nof_acks = 0;
        }
        // Transmitting on PUSCH not based on grant
      } else {
        if (V_dai_dl != (U_dai - 1) % 4 + 1 && U_dai > 0) {
          bzero(uci_data->value.ack.ack_value, nof_tb);
        }
        ack_cfg->N_bundle = U_dai;
        // do not transmit case
        if (U_dai == 0) {
          ack_cfg->nof_acks = 0;
        }
      }

      // In PUSCH and MIMO, nack 2nd codeword if not received, in PUCCH do not transmit
      if (nof_tb == 2 && uci_data->value.ack.ack_value[1] == 2 && ack_cfg->nof_acks == 2) {
        if (!ack_info->is_pusch_available) {
          ack_cfg->nof_acks = 1;
        } else {
          uci_data->value.ack.ack_value[1] = 0;
        }
      }

      // Multiplexing and M>1
    } else {
      if (ack_info->is_pusch_available) {
        if (ack_info->is_grant_available) {
          // Do not transmit if...
          if (!(V_dai_ul == 4 && U_dai == 0)) {
            ack_cfg->nof_acks = V_dai_ul;
          }
        } else {
          ack_cfg->nof_acks = ack_info->cc[0].M;
        }

        // Set DTX bits to NACKs
        uint32_t count_acks = 0;
        for (uint32_t i = 0; i < ack_cfg->nof_acks; i++) {
          if (uci_data->value.ack.ack_value[i] == 2) {
            uci_data->value.ack.ack_value[i] = 0;
          } else {
            count_acks++;
          }
        }
        if (!count_acks) {
          ack_cfg->nof_acks = 0;
        }
      } else {
        ack_cfg->nof_acks = ack_info->cc[0].M;
      }
    }
  } else {
    ack_cfg->N_bundle = 1;
    ack_cfg->nof_acks = nof_total_acks;
  }

  // Multiple ACK/NACK responses with SR and CQI
  if (ack_cfg->nof_acks && !ack_info->is_pusch_available &&
      (uci_data->value.scheduling_request ||
       ((uci_data->cfg.cqi.data_enable || uci_data->cfg.cqi.ri_len) && ack_info->simul_cqi_ack))) {
    if (missing_ack) {
      uci_data->value.ack.ack_value[0] = 0;
      uci_data->value.ack.ack_value[1] = 0;
    } else {
      nof_pos_acks                     = SRSLTE_MIN(9, nof_pos_acks);
      uci_data->value.ack.ack_value[0] = multiple_acknack[nof_pos_acks][0];
      uci_data->value.ack.ack_value[1] = multiple_acknack[nof_pos_acks][1];
    }
    ack_cfg->nof_acks = 2;
  }
}

/* UE downlink procedure for reporting ACK/NACK, Section 7.3 36.213
 */
void srslte_ue_dl_gen_ack(const srslte_cell_t*      cell,
                          const srslte_dl_sf_cfg_t* sf,
                          const srslte_pdsch_ack_t* ack_info,
                          srslte_uci_data_t*        uci_data)
{
  if (cell->frame_type == SRSLTE_FDD) {
    gen_ack_fdd(ack_info, uci_data);
  } else {
    bool is_tdd_mode16 = sf->tdd_config.sf_config >= 1 && sf->tdd_config.sf_config <= 6;
    gen_ack_tdd(is_tdd_mode16, ack_info, uci_data);
  }
}

int srslte_ue_dl_find_and_decode(srslte_ue_dl_t*     q,
                                 srslte_dl_sf_cfg_t* sf,
                                 srslte_ue_dl_cfg_t* cfg,
                                 srslte_pdsch_cfg_t* pdsch_cfg,
                                 uint8_t*            data[SRSLTE_MAX_CODEWORDS],
                                 bool                acks[SRSLTE_MAX_CODEWORDS])
{
  int ret = SRSLTE_ERROR;

  srslte_dci_dl_t    dci_dl[SRSLTE_MAX_DCI_MSG] = {};
  srslte_pmch_cfg_t  pmch_cfg;
  srslte_pdsch_res_t pdsch_res[SRSLTE_MAX_CODEWORDS];

  // Use default values for PDSCH decoder
  ZERO_OBJECT(pmch_cfg);

  uint32_t mi_set_len;
  if (q->cell.frame_type == SRSLTE_TDD && !sf->tdd_config.configured) {
    mi_set_len = 3;
  } else {
    mi_set_len = 1;
  }

  // Blind search PHICH mi value
  ret = 0;
  for (uint32_t i = 0; i < mi_set_len && !ret; i++) {

    if (mi_set_len == 1) {
      srslte_ue_dl_set_mi_auto(q);
    } else {
      srslte_ue_dl_set_mi_manual(q, i);
    }

    if ((ret = srslte_ue_dl_decode_fft_estimate(q, sf, cfg)) < 0) {
      return ret;
    }

    ret = srslte_ue_dl_find_dl_dci(q, sf, cfg, pdsch_cfg->rnti, dci_dl);
  }

  if (ret == 1) {
    // Logging
    if (SRSLTE_DEBUG_ENABLED && srslte_verbose >= SRSLTE_VERBOSE_INFO) {
      char str[512];
      srslte_dci_dl_info(&dci_dl[0], str, 512);
      INFO("PDCCH: %s, snr=%.1f dB\n", str, q->chest_res.snr_db);
    }

    // Force known MBSFN grant
    if (sf->sf_type == SRSLTE_SF_MBSFN) {
      dci_dl[0].rnti                    = SRSLTE_MRNTI;
      dci_dl[0].alloc_type              = SRSLTE_RA_ALLOC_TYPE0;
      dci_dl[0].type0_alloc.rbg_bitmask = 0xffffffff;
      dci_dl[0].tb[0].rv                = 0;
      dci_dl[0].tb[0].mcs_idx           = 2;
      dci_dl[0].format                  = SRSLTE_DCI_FORMAT1;
    }

    // Convert DCI message to DL grant
    if (srslte_ue_dl_dci_to_pdsch_grant(q, sf, cfg, &dci_dl[0], &pdsch_cfg->grant)) {
      ERROR("Error unpacking DCI\n");
      return SRSLTE_ERROR;
    }

    // Calculate RV if not provided in the grant and reset softbuffer
    for (int i = 0; i < SRSLTE_MAX_CODEWORDS; i++) {
      if (pdsch_cfg->grant.tb[i].enabled) {
        if (pdsch_cfg->grant.tb[i].rv < 0) {
          uint32_t sfn              = sf->tti / 10;
          uint32_t k                = (sfn / 2) % 4;
          pdsch_cfg->grant.tb[i].rv = ((uint32_t)ceilf((float)1.5 * k)) % 4;
        }
        srslte_softbuffer_rx_reset_tbs(pdsch_cfg->softbuffers.rx[i], (uint32_t)pdsch_cfg->grant.tb[i].tbs);
      }
    }

    bool decode_enable = false;
    for (uint32_t tb = 0; tb < SRSLTE_MAX_CODEWORDS; tb++) {
      if (pdsch_cfg->grant.tb[tb].enabled) {
        decode_enable         = true;
        pdsch_res[tb].payload = data[tb];
        pdsch_res[tb].crc     = false;
      }
    }

    if (decode_enable) {
      if (sf->sf_type == SRSLTE_SF_NORM) {
        if (srslte_ue_dl_decode_pdsch(q, sf, pdsch_cfg, pdsch_res)) {
          ERROR("ERROR: Decoding PDSCH\n");
          ret = -1;
        }
      } else {
        pmch_cfg.pdsch_cfg = *pdsch_cfg;
        if (srslte_ue_dl_decode_pmch(q, sf, &pmch_cfg, pdsch_res)) {
          ERROR("Decoding PMCH\n");
          ret = -1;
        }
      }
    }

    for (uint32_t tb = 0; tb < SRSLTE_MAX_CODEWORDS; tb++) {
      if (pdsch_cfg->grant.tb[tb].enabled) {
        acks[tb] = pdsch_res[tb].crc;
      }
    }
  } else {
    ERROR("Decoded %d DCIs\n", ret);
  }
  return ret;
}

void srslte_ue_dl_save_signal(srslte_ue_dl_t* q, srslte_dl_sf_cfg_t* sf, srslte_pdsch_cfg_t* pdsch_cfg)
{
  uint32_t cfi = sf->cfi;
  uint32_t tti = sf->tti % 10;

  srslte_vec_save_file("sf_symbols", q->sf_symbols, SRSLTE_NOF_RE(q->cell) * sizeof(cf_t));
  printf("%d samples\n", SRSLTE_NOF_RE(q->cell));
  srslte_vec_save_file("ce0", q->chest_res.ce[0], SRSLTE_NOF_RE(q->cell) * sizeof(cf_t));
  if (q->cell.nof_ports > 1) {
    srslte_vec_save_file("ce1", q->chest_res.ce[1], SRSLTE_NOF_RE(q->cell) * sizeof(cf_t));
  }
  srslte_vec_save_file("pcfich_ce0", q->pcfich.ce[0], q->pcfich.nof_symbols * sizeof(cf_t));
  srslte_vec_save_file("pcfich_ce1", q->pcfich.ce[1], q->pcfich.nof_symbols * sizeof(cf_t));
  srslte_vec_save_file("pcfich_symbols", q->pcfich.symbols[0], q->pcfich.nof_symbols * sizeof(cf_t));
  srslte_vec_save_file("pcfich_eq_symbols", q->pcfich.d, q->pcfich.nof_symbols * sizeof(cf_t));
  srslte_vec_save_file("pcfich_llr", q->pcfich.data_f, PCFICH_CFI_LEN * sizeof(float));

  srslte_vec_save_file("pdcch_ce0", q->pdcch.ce[0], q->pdcch.nof_cce[cfi - 1] * 36 * sizeof(cf_t));
  srslte_vec_save_file("pdcch_ce1", q->pdcch.ce[1], q->pdcch.nof_cce[cfi - 1] * 36 * sizeof(cf_t));
  srslte_vec_save_file("pdcch_symbols", q->pdcch.symbols[0], q->pdcch.nof_cce[cfi - 1] * 36 * sizeof(cf_t));
  srslte_vec_save_file("pdcch_eq_symbols", q->pdcch.d, q->pdcch.nof_cce[cfi - 1] * 36 * sizeof(cf_t));
  srslte_vec_save_file("pdcch_llr", q->pdcch.llr, q->pdcch.nof_cce[cfi - 1] * 72 * sizeof(float));

  srslte_vec_save_file("pdsch_symbols", q->pdsch.d[0], pdsch_cfg->grant.nof_re * sizeof(cf_t));
  srslte_vec_save_file("llr", q->pdsch.e[0], pdsch_cfg->grant.tb[0].nof_bits * sizeof(cf_t));
  printf("Saved files for tti=%d, sf=%d, cfi=%d, tbs=%d, rv=%d\n",
         tti,
         tti % 10,
         cfi,
         pdsch_cfg->grant.tb[0].tbs,
         pdsch_cfg->grant.tb[0].rv);
}
