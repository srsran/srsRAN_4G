/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#include "srsran/phy/ue/ue_dl_nr.h"
#include <complex.h>

#define UE_DL_NR_PDCCH_CORR_DEFAULT_THR 0.5f
#define UE_DL_NR_PDCCH_EPRE_DEFAULT_THR -80.0f

/**
 * @brief Shifts FFT window a fraction of the cyclic prefix. Set to 0.0f for disabling.
 * @note Increases protection against inter-symbol interference in case of synchronization error in expense of computing
 * performance
 */
#define UE_DL_NR_FFT_WINDOW_OFFSET 0.5f

static int ue_dl_nr_alloc_prb(srsran_ue_dl_nr_t* q, uint32_t new_nof_prb)
{
  if (q->max_prb < new_nof_prb) {
    q->max_prb = new_nof_prb;

    srsran_chest_dl_res_free(&q->chest);
    if (srsran_chest_dl_res_init(&q->chest, q->max_prb) < SRSRAN_SUCCESS) {
      return SRSRAN_ERROR;
    }

    for (uint32_t i = 0; i < q->nof_rx_antennas; i++) {
      if (q->sf_symbols[i] != NULL) {
        free(q->sf_symbols[i]);
      }

      q->sf_symbols[i] = srsran_vec_cf_malloc(SRSRAN_SLOT_LEN_RE_NR(q->max_prb));
      if (q->sf_symbols[i] == NULL) {
        ERROR("Malloc");
        return SRSRAN_ERROR;
      }
    }
  }

  return SRSRAN_SUCCESS;
}

int srsran_ue_dl_nr_init(srsran_ue_dl_nr_t* q, cf_t* input[SRSRAN_MAX_PORTS], const srsran_ue_dl_nr_args_t* args)
{
  if (!q || !input || !args) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  if (args->nof_rx_antennas == 0) {
    ERROR("Error invalid number of antennas (%d)", args->nof_rx_antennas);
    return SRSRAN_ERROR;
  }

  q->nof_rx_antennas = args->nof_rx_antennas;
  if (isnormal(args->pdcch_dmrs_corr_thr)) {
    q->pdcch_dmrs_corr_thr = args->pdcch_dmrs_corr_thr;
  } else {
    q->pdcch_dmrs_corr_thr = UE_DL_NR_PDCCH_CORR_DEFAULT_THR;
  }
  if (isnormal(args->pdcch_dmrs_epre_thr)) {
    q->pdcch_dmrs_epre_thr = args->pdcch_dmrs_epre_thr;
  } else {
    q->pdcch_dmrs_epre_thr = UE_DL_NR_PDCCH_EPRE_DEFAULT_THR;
  }

  if (srsran_pdsch_nr_init_ue(&q->pdsch, &args->pdsch) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  if (srsran_pdcch_nr_init_rx(&q->pdcch, &args->pdcch)) {
    return SRSRAN_ERROR;
  }

  if (ue_dl_nr_alloc_prb(q, args->nof_max_prb)) {
    ERROR("Error allocating");
    return SRSRAN_ERROR;
  }

  srsran_ofdm_cfg_t fft_cfg = {};
  fft_cfg.nof_prb           = args->nof_max_prb;
  fft_cfg.symbol_sz         = srsran_min_symbol_sz_rb(args->nof_max_prb);
  fft_cfg.keep_dc           = true;
  fft_cfg.rx_window_offset  = UE_DL_NR_FFT_WINDOW_OFFSET;

  for (uint32_t i = 0; i < q->nof_rx_antennas; i++) {
    fft_cfg.in_buffer  = input[i];
    fft_cfg.out_buffer = q->sf_symbols[i];
    srsran_ofdm_rx_init_cfg(&q->fft[i], &fft_cfg);
  }

  if (srsran_dmrs_sch_init(&q->dmrs_pdsch, true) < SRSRAN_SUCCESS) {
    ERROR("Error DMRS");
    return SRSRAN_ERROR;
  }

  q->pdcch_ce = SRSRAN_MEM_ALLOC(srsran_dmrs_pdcch_ce_t, 1);
  if (q->pdcch_ce == NULL) {
    ERROR("Error alloc");
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

void srsran_ue_dl_nr_free(srsran_ue_dl_nr_t* q)
{
  if (q == NULL) {
    return;
  }

  for (uint32_t i = 0; i < SRSRAN_MAX_PORTS; i++) {
    srsran_ofdm_rx_free(&q->fft[i]);

    if (q->sf_symbols[i] != NULL) {
      free(q->sf_symbols[i]);
    }
  }

  srsran_chest_dl_res_free(&q->chest);
  srsran_pdsch_nr_free(&q->pdsch);
  srsran_dmrs_sch_free(&q->dmrs_pdsch);

  for (uint32_t i = 0; i < SRSRAN_UE_DL_NR_MAX_NOF_CORESET; i++) {
    srsran_dmrs_pdcch_estimator_free(&q->dmrs_pdcch[i]);
  }
  srsran_pdcch_nr_free(&q->pdcch);

  if (q->pdcch_ce) {
    free(q->pdcch_ce);
  }

  SRSRAN_MEM_ZERO(q, srsran_ue_dl_nr_t, 1);
}

int srsran_ue_dl_nr_set_carrier(srsran_ue_dl_nr_t* q, const srsran_carrier_nr_t* carrier)
{
  if (srsran_pdsch_nr_set_carrier(&q->pdsch, carrier) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  if (srsran_dmrs_sch_set_carrier(&q->dmrs_pdsch, carrier) < SRSRAN_SUCCESS) {
    ERROR("Error DMRS");
    return SRSRAN_ERROR;
  }

  if (ue_dl_nr_alloc_prb(q, carrier->nof_prb)) {
    ERROR("Error allocating");
    return SRSRAN_ERROR;
  }

  if (carrier->nof_prb != q->carrier.nof_prb) {
    for (uint32_t i = 0; i < q->nof_rx_antennas; i++) {
      srsran_ofdm_cfg_t cfg     = {};
      cfg.nof_prb               = carrier->nof_prb;
      cfg.symbol_sz             = srsran_min_symbol_sz_rb(carrier->nof_prb);
      cfg.cp                    = SRSRAN_CP_NORM;
      cfg.keep_dc               = true;
      cfg.rx_window_offset      = UE_DL_NR_FFT_WINDOW_OFFSET;
      cfg.phase_compensation_hz = carrier->dl_center_frequency_hz;

      srsran_ofdm_rx_init_cfg(&q->fft[i], &cfg);
    }
  }

  q->carrier = *carrier;

  return SRSRAN_SUCCESS;
}

int srsran_ue_dl_nr_set_pdcch_config(srsran_ue_dl_nr_t*           q,
                                     const srsran_pdcch_cfg_nr_t* cfg,
                                     const srsran_dci_cfg_nr_t*   dci_cfg)
{
  if (q == NULL || cfg == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Copy new configuration
  q->cfg = *cfg;

  // iterate over all possible CORESET and initialise/update the present ones
  for (uint32_t i = 0; i < SRSRAN_UE_DL_NR_MAX_NOF_CORESET; i++) {
    // Skip CORESET if not present
    if (!cfg->coreset_present[i]) {
      continue;
    }

    // Initialise estimator for the CORESET
    if (srsran_dmrs_pdcch_estimator_init(&q->dmrs_pdcch[i], &q->carrier, &cfg->coreset[i]) < SRSRAN_SUCCESS) {
      return SRSRAN_ERROR;
    }
  }

  // Configure DCI sizes
  if (srsran_dci_nr_set_cfg(&q->dci, dci_cfg) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

void srsran_ue_dl_nr_estimate_fft(srsran_ue_dl_nr_t* q, const srsran_slot_cfg_t* slot_cfg)
{
  if (q == NULL || slot_cfg == NULL) {
    return;
  }

  // OFDM demodulation
  for (uint32_t i = 0; i < q->nof_rx_antennas; i++) {
    srsran_ofdm_rx_sf(&q->fft[i]);
  }

  // Estimate PDCCH channel for every configured CORESET
  for (uint32_t i = 0; i < SRSRAN_UE_DL_NR_MAX_NOF_CORESET; i++) {
    if (q->cfg.coreset_present[i]) {
      srsran_dmrs_pdcch_estimate(&q->dmrs_pdcch[i], slot_cfg, q->sf_symbols[0]);
    }
  }
}

static int ue_dl_nr_find_dci_ncce(srsran_ue_dl_nr_t*     q,
                                  srsran_dci_msg_nr_t*   dci_msg,
                                  srsran_pdcch_nr_res_t* pdcch_res,
                                  uint32_t               coreset_id)
{
  // Select debug information
  srsran_ue_dl_nr_pdcch_info_t* pdcch_info = NULL;
  if (q->pdcch_info_count < SRSRAN_MAX_NOF_CANDIDATES_SLOT_NR) {
    pdcch_info = &q->pdcch_info[q->pdcch_info_count];
    q->pdcch_info_count++;
  } else {
    ERROR("The UE does not expect more than %d candidates in this serving cell", SRSRAN_MAX_NOF_CANDIDATES_SLOT_NR);
    return SRSRAN_ERROR;
  }
  SRSRAN_MEM_ZERO(pdcch_info, srsran_ue_dl_nr_pdcch_info_t, 1);
  pdcch_info->dci_ctx            = dci_msg->ctx;
  pdcch_info->nof_bits           = dci_msg->nof_bits;
  srsran_dmrs_pdcch_measure_t* m = &pdcch_info->measure;

  // Measures the PDCCH transmission DMRS
  srsran_dci_location_t location = dci_msg->ctx.location;
  if (srsran_dmrs_pdcch_get_measure(&q->dmrs_pdcch[coreset_id], &location, m) < SRSRAN_SUCCESS) {
    ERROR("Error getting measure location L=%d, ncce=%d", location.L, location.ncce);
    return SRSRAN_ERROR;
  }

  // If measured correlation is invalid, early return
  if (!isnormal(m->norm_corr)) {
    INFO("Discarded PDCCH candidate L=%d;ncce=%d; Invalid measurement;", location.L, location.ncce);
    return SRSRAN_SUCCESS;
  }

  // Compare EPRE with threshold
  if (m->epre_dBfs < q->pdcch_dmrs_epre_thr) {
    INFO("Discarded PDCCH candidate L=%d;ncce=%d; EPRE is too weak (%.1f<%.1f);",
         location.L,
         location.ncce,
         m->epre_dBfs,
         q->pdcch_dmrs_epre_thr);
    return SRSRAN_SUCCESS;
  }

  // Compare DMRS correlation with threshold
  if (m->norm_corr < q->pdcch_dmrs_corr_thr) {
    INFO("Discarded PDCCH candidate L=%d;ncce=%d; Correlation is too low (%.1f<%.1f); EPRE=%+.2f; RSRP=%+.2f;",
         location.L,
         location.ncce,
         m->norm_corr,
         q->pdcch_dmrs_corr_thr,
         m->epre_dBfs,
         m->rsrp_dBfs);
    return SRSRAN_SUCCESS;
  }

  // Extract PDCCH channel estimates
  if (srsran_dmrs_pdcch_get_ce(&q->dmrs_pdcch[coreset_id], &location, q->pdcch_ce) < SRSRAN_SUCCESS) {
    ERROR("Error extracting PDCCH DMRS");
    return SRSRAN_ERROR;
  }

  // Decode PDCCH
  if (srsran_pdcch_nr_decode(&q->pdcch, q->sf_symbols[0], q->pdcch_ce, dci_msg, pdcch_res) < SRSRAN_SUCCESS) {
    ERROR("Error decoding PDCCH");
    return SRSRAN_ERROR;
  }

#if 0
  static uint32_t num_pdcch = 0;
  char            tmpstr[64];
  snprintf(tmpstr, 64, "pdcch_symbols%d.dat", num_pdcch);
  printf("save %d syms to %s\n", q->pdcch.M / 4, tmpstr);
  srsran_vec_save_file(tmpstr, q->pdcch.symbols, q->pdcch.M / 4 * sizeof(cf_t));
  // srsran_vec_fprint_c(stdout, q->pdcch.symbols, q->pdcch.M/4);
  num_pdcch++;
#endif

  // Save information
  pdcch_info->result = *pdcch_res;

  return SRSRAN_SUCCESS;
}

static bool find_dci_msg(srsran_dci_msg_nr_t* dci_msg, uint32_t nof_dci_msg, srsran_dci_msg_nr_t* match)
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

static int ue_dl_nr_find_dci_ss(srsran_ue_dl_nr_t*           q,
                                const srsran_slot_cfg_t*     slot_cfg,
                                const srsran_search_space_t* search_space,
                                uint16_t                     rnti,
                                srsran_rnti_type_t           rnti_type)
{
  uint32_t dci_sizes[SRSRAN_DCI_NR_MAX_NOF_SIZES] = {};
  uint32_t dci_sizes_count                        = 0;

  // Select CORESET
  uint32_t coreset_id = search_space->coreset_id;
  if (coreset_id >= SRSRAN_UE_DL_NR_MAX_NOF_CORESET || !q->cfg.coreset_present[coreset_id]) {
    ERROR("CORESET %d is not present in search space %d", search_space->coreset_id, search_space->id);
    return SRSRAN_ERROR;
  }
  srsran_coreset_t* coreset = &q->cfg.coreset[search_space->coreset_id];

  // Set CORESET in PDCCH decoder
  if (srsran_pdcch_nr_set_carrier(&q->pdcch, &q->carrier, coreset) < SRSRAN_SUCCESS) {
    ERROR("Setting carrier and CORESETºn");
    return SRSRAN_ERROR;
  }

  // Iterate all possible formats
  for (uint32_t format_idx = 0; format_idx < SRSRAN_MIN(search_space->nof_formats, SRSRAN_DCI_FORMAT_NR_COUNT);
       format_idx++) {
    srsran_dci_format_nr_t dci_format = search_space->formats[format_idx];

    // Calculate number of DCI bits
    uint32_t dci_nof_bits = srsran_dci_nr_size(&q->dci, search_space->type, dci_format);
    if (dci_nof_bits == 0) {
      ERROR("Error DCI size");
      return SRSRAN_ERROR;
    }

    // Skip DCI format if the size was already searched for the search space
    bool skip = false;
    for (uint32_t i = 0; i < dci_sizes_count && !skip; i++) {
      if (dci_nof_bits == dci_sizes[i]) {
        skip = true;
      }
    }
    if (skip) {
      continue;
    }

    // Append size
    if (dci_sizes_count >= SRSRAN_DCI_NR_MAX_NOF_SIZES) {
      ERROR("Exceed maximum number of DCI sizes");
      return SRSRAN_ERROR;
    }
    dci_sizes[dci_sizes_count++] = dci_nof_bits;

    // Iterate all possible aggregation levels
    for (uint32_t L = 0;
         L < SRSRAN_SEARCH_SPACE_NOF_AGGREGATION_LEVELS_NR && q->dl_dci_msg_count < SRSRAN_MAX_DCI_MSG_NR;
         L++) {
      // Calculate possible PDCCH DCI candidates
      uint32_t candidates[SRSRAN_SEARCH_SPACE_MAX_NOF_CANDIDATES_NR] = {};
      int      nof_candidates                                        = srsran_pdcch_nr_locations_coreset(
                                                      coreset, search_space, rnti, L, SRSRAN_SLOT_NR_MOD(q->carrier.scs, slot_cfg->idx), candidates);
      if (nof_candidates < SRSRAN_SUCCESS) {
        ERROR("Error calculating DCI candidate location");
        return SRSRAN_ERROR;
      }

      // Iterate over the candidates
      for (int ncce_idx = 0; ncce_idx < nof_candidates && q->dl_dci_msg_count < SRSRAN_MAX_DCI_MSG_NR; ncce_idx++) {
        // Build DCI context
        srsran_dci_ctx_t ctx = {};
        ctx.location.L       = L;
        ctx.location.ncce    = candidates[ncce_idx];
        ctx.ss_type          = search_space->type;
        ctx.coreset_id       = search_space->coreset_id;
        ctx.coreset_start_rb = srsran_coreset_start_rb(&q->cfg.coreset[search_space->coreset_id]);
        ctx.rnti_type        = rnti_type;
        ctx.rnti             = rnti;
        ctx.format           = dci_format;

        // Build DCI message
        srsran_dci_msg_nr_t dci_msg = {};
        dci_msg.ctx                 = ctx;
        dci_msg.nof_bits            = (uint32_t)dci_nof_bits;

        // Find and decode PDCCH transmission in the given ncce
        srsran_pdcch_nr_res_t res = {};
        if (ue_dl_nr_find_dci_ncce(q, &dci_msg, &res, coreset_id) < SRSRAN_SUCCESS) {
          return SRSRAN_ERROR;
        }

        // If the CRC was not match, move to next candidate
        if (!res.crc) {
          continue;
        }

        // Detect if the DCI is the right direction
        if (!srsran_dci_nr_valid_direction(&dci_msg)) {
          // Change grant format direction
          switch (dci_msg.ctx.format) {
            case srsran_dci_format_nr_0_0:
              dci_msg.ctx.format = srsran_dci_format_nr_1_0;
              break;
            case srsran_dci_format_nr_0_1:
              dci_msg.ctx.format = srsran_dci_format_nr_1_1;
              break;
            case srsran_dci_format_nr_1_0:
              dci_msg.ctx.format = srsran_dci_format_nr_0_0;
              break;
            case srsran_dci_format_nr_1_1:
              dci_msg.ctx.format = srsran_dci_format_nr_0_1;
              break;
            default:
              continue;
          }
        }

        // If UL grant, enqueue in UL list
        if (dci_msg.ctx.format == srsran_dci_format_nr_0_0 || dci_msg.ctx.format == srsran_dci_format_nr_0_1) {
          // If the pending UL grant list is full or has the dci message, keep moving
          if (q->ul_dci_count >= SRSRAN_MAX_DCI_MSG_NR || find_dci_msg(q->ul_dci_msg, q->ul_dci_count, &dci_msg)) {
            continue;
          }

          // Save the grant in the pending UL grant list
          q->ul_dci_msg[q->ul_dci_count] = dci_msg;
          q->ul_dci_count++;

          // Move to next candidate
          continue;
        }

        // Check if the grant exists already in the DL list
        if (find_dci_msg(q->dl_dci_msg, q->dl_dci_msg_count, &dci_msg)) {
          // The same DCI is in the list, keep moving
          continue;
        }

        INFO("Found DCI in L=%d,ncce=%d", dci_msg.ctx.location.L, dci_msg.ctx.location.ncce);
        // Append DCI message into the list
        q->dl_dci_msg[q->dl_dci_msg_count] = dci_msg;
        q->dl_dci_msg_count++;
      }
    }
  }

  return SRSRAN_SUCCESS;
}

int srsran_ue_dl_nr_find_dl_dci(srsran_ue_dl_nr_t*       q,
                                const srsran_slot_cfg_t* slot_cfg,
                                uint16_t                 rnti,
                                srsran_rnti_type_t       rnti_type,
                                srsran_dci_dl_nr_t*      dci_dl_list,
                                uint32_t                 nof_dci_msg)
{
  // Check inputs
  if (q == NULL || slot_cfg == NULL || dci_dl_list == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Limit maximum number of DCI messages to find
  nof_dci_msg = SRSRAN_MIN(nof_dci_msg, SRSRAN_MAX_DCI_MSG_NR);

  // Reset grant and blind search information counters
  q->dl_dci_msg_count = 0;
  q->pdcch_info_count = 0;

  // If the UE looks for a RAR and RA search space is provided, search for it
  if (q->cfg.ra_search_space_present && rnti_type == srsran_rnti_type_ra) {
    // Find DCIs in the RA search space
    int ret = ue_dl_nr_find_dci_ss(q, slot_cfg, &q->cfg.ra_search_space, rnti, rnti_type);
    if (ret < SRSRAN_SUCCESS) {
      ERROR("Error searching RAR DCI");
      return SRSRAN_ERROR;
    }
  } else {
    // Iterate all possible common and UE search spaces
    for (uint32_t i = 0; i < SRSRAN_UE_DL_NR_MAX_NOF_SEARCH_SPACE && q->dl_dci_msg_count < nof_dci_msg; i++) {
      // Skip search space if not present
      if (!q->cfg.search_space_present[i]) {
        continue;
      }

      // Find DCIs in the selected search space
      int ret = ue_dl_nr_find_dci_ss(q, slot_cfg, &q->cfg.search_space[i], rnti, rnti_type);
      if (ret < SRSRAN_SUCCESS) {
        ERROR("Error searching DCI");
        return SRSRAN_ERROR;
      }
    }
  }

  // Convert found DCI messages into DL grants
  uint32_t dci_msg_count = SRSRAN_MIN(nof_dci_msg, q->dl_dci_msg_count);
  for (uint32_t i = 0; i < dci_msg_count; i++) {
    if (srsran_dci_nr_dl_unpack(&q->dci, &q->dl_dci_msg[i], &dci_dl_list[i]) < SRSRAN_SUCCESS) {
      ERROR("Error unpacking grant %d;", i);
      return SRSRAN_ERROR;
    }
  }

  return (int)dci_msg_count;
}

int srsran_ue_dl_nr_find_ul_dci(srsran_ue_dl_nr_t*       q,
                                const srsran_slot_cfg_t* slot_cfg,
                                uint16_t                 rnti,
                                srsran_rnti_type_t       rnti_type,
                                srsran_dci_ul_nr_t*      dci_ul_list,
                                uint32_t                 nof_dci_msg)
{
  int count = 0;

  // Check inputs
  if (q == NULL || slot_cfg == NULL || dci_ul_list == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Get DCI messages from the pending list
  for (uint32_t i = 0; i < q->ul_dci_count && count < nof_dci_msg; i++) {
    srsran_dci_msg_nr_t* dci_msg = &q->ul_dci_msg[i];

    if (dci_msg->ctx.rnti_type != rnti_type || dci_msg->ctx.rnti != rnti) {
      continue;
    }

    if (srsran_dci_nr_ul_unpack(&q->dci, dci_msg, &dci_ul_list[count]) < SRSRAN_SUCCESS) {
      ERROR("Unpacking DCI 0_0");
      continue;
    }
    count++;
  }

  // Reset pending UL grant list
  q->ul_dci_count = 0;

  return count;
}

int srsran_ue_dl_nr_decode_pdsch(srsran_ue_dl_nr_t*         q,
                                 const srsran_slot_cfg_t*   slot,
                                 const srsran_sch_cfg_nr_t* cfg,
                                 srsran_pdsch_res_nr_t*     res)
{
  if (srsran_dmrs_sch_estimate(&q->dmrs_pdsch, slot, cfg, &cfg->grant, q->sf_symbols[0], &q->chest) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  if (srsran_pdsch_nr_decode(&q->pdsch, cfg, &cfg->grant, &q->chest, q->sf_symbols, res) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  if (SRSRAN_DEBUG_ENABLED && get_srsran_verbose_level() >= SRSRAN_VERBOSE_INFO && !is_handler_registered()) {
    char str[512];
    srsran_ue_dl_nr_pdsch_info(q, cfg, res, str, sizeof(str));
    INFO("PDSCH: %s", str);
  }

  return SRSRAN_SUCCESS;
}

uint32_t srsran_ue_dl_nr_pdsch_info(const srsran_ue_dl_nr_t*    q,
                                    const srsran_sch_cfg_nr_t*  cfg,
                                    const srsran_pdsch_res_nr_t res[SRSRAN_MAX_CODEWORDS],
                                    char*                       str,
                                    uint32_t                    str_len)
{
  uint32_t len = 0;

  // Append PDSCH info
  len += srsran_pdsch_nr_rx_info(&q->pdsch, cfg, &cfg->grant, res, &str[len], str_len - len);

  // Append channel estimator info
  len += srsran_csi_meas_info_short(&q->dmrs_pdsch.csi, &str[len], str_len - len);

  return len;
}

int srsran_ue_dl_nr_csi_measure_trs(const srsran_ue_dl_nr_t*       q,
                                    const srsran_slot_cfg_t*       slot_cfg,
                                    const srsran_csi_rs_nzp_set_t* csi_rs_nzp_set,
                                    srsran_csi_trs_measurements_t* measurement)
{
  if (q == NULL || slot_cfg == NULL || csi_rs_nzp_set == NULL || measurement == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  return srsran_csi_rs_nzp_measure_trs(&q->carrier, slot_cfg, csi_rs_nzp_set, q->sf_symbols[0], measurement);
}

int srsran_ue_dl_nr_csi_measure_channel(const srsran_ue_dl_nr_t*           q,
                                        const srsran_slot_cfg_t*           slot_cfg,
                                        const srsran_csi_rs_nzp_set_t*     csi_rs_nzp_set,
                                        srsran_csi_channel_measurements_t* measurement)
{
  if (q == NULL || slot_cfg == NULL || csi_rs_nzp_set == NULL || measurement == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  return srsran_csi_rs_nzp_measure_channel(&q->carrier, slot_cfg, csi_rs_nzp_set, q->sf_symbols[0], measurement);
}
