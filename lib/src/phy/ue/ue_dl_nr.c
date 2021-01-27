/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srslte/phy/ue/ue_dl_nr.h"

#define UE_DL_NR_PDCCH_CORR_DEFAULT_THR 0.5f
#define UE_DL_NR_PDCCH_EPRE_DEFAULT_THR -10.0f

static int ue_dl_nr_alloc_prb(srslte_ue_dl_nr_t* q, uint32_t new_nof_prb)
{
  if (q->max_prb < new_nof_prb) {
    q->max_prb = new_nof_prb;

    srslte_chest_dl_res_free(&q->chest);
    if (srslte_chest_dl_res_init(&q->chest, q->max_prb) < SRSLTE_SUCCESS) {
      return SRSLTE_ERROR;
    }

    for (uint32_t i = 0; i < q->nof_rx_antennas; i++) {
      if (q->sf_symbols[i] != NULL) {
        free(q->sf_symbols[i]);
      }

      q->sf_symbols[i] = srslte_vec_cf_malloc(SRSLTE_SLOT_LEN_RE_NR(q->max_prb));
      if (q->sf_symbols[i] == NULL) {
        ERROR("Malloc\n");
        return SRSLTE_ERROR;
      }
    }
  }

  return SRSLTE_SUCCESS;
}

int srslte_ue_dl_nr_init(srslte_ue_dl_nr_t* q, cf_t* input[SRSLTE_MAX_PORTS], const srslte_ue_dl_nr_args_t* args)
{
  if (!q || !input || !args) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  if (args->nof_rx_antennas == 0) {
    ERROR("Error invalid number of antennas (%d)\n", args->nof_rx_antennas);
    return SRSLTE_ERROR;
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

  if (srslte_pdsch_nr_init_ue(&q->pdsch, &args->pdsch) < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  if (srslte_pdcch_nr_init_rx(&q->pdcch, &args->pdcch)) {
    return SRSLTE_ERROR;
  }

  if (ue_dl_nr_alloc_prb(q, args->nof_max_prb)) {
    ERROR("Error allocating\n");
    return SRSLTE_ERROR;
  }

  srslte_ofdm_cfg_t fft_cfg = {};
  fft_cfg.nof_prb           = args->nof_max_prb;
  fft_cfg.symbol_sz         = srslte_symbol_sz(args->nof_max_prb);
  fft_cfg.keep_dc           = true;

  for (uint32_t i = 0; i < q->nof_rx_antennas; i++) {
    fft_cfg.in_buffer  = input[i];
    fft_cfg.out_buffer = q->sf_symbols[i];
    srslte_ofdm_rx_init_cfg(&q->fft[i], &fft_cfg);
  }

  if (srslte_dmrs_sch_init(&q->dmrs_pdsch, true) < SRSLTE_SUCCESS) {
    ERROR("Error DMRS\n");
    return SRSLTE_ERROR;
  }

  q->pdcch_ce = SRSLTE_MEM_ALLOC(srslte_dmrs_pdcch_ce_t, 1);
  if (q->pdcch_ce == NULL) {
    ERROR("Error alloc\n");
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

void srslte_ue_dl_nr_free(srslte_ue_dl_nr_t* q)
{
  if (q == NULL) {
    return;
  }

  for (uint32_t i = 0; i < SRSLTE_MAX_PORTS; i++) {
    srslte_ofdm_rx_free(&q->fft[i]);

    if (q->sf_symbols[i] != NULL) {
      free(q->sf_symbols[i]);
    }
  }

  srslte_chest_dl_res_free(&q->chest);
  srslte_pdsch_nr_free(&q->pdsch);
  srslte_dmrs_sch_free(&q->dmrs_pdsch);

  for (uint32_t i = 0; i < SRSLTE_UE_DL_NR_MAX_NOF_CORESET; i++) {
    srslte_dmrs_pdcch_estimator_free(&q->dmrs_pdcch[i]);
  }
  srslte_pdcch_nr_free(&q->pdcch);

  if (q->pdcch_ce) {
    free(q->pdcch_ce);
  }

  SRSLTE_MEM_ZERO(q, srslte_ue_dl_nr_t, 1);
}

int srslte_ue_dl_nr_set_carrier(srslte_ue_dl_nr_t* q, const srslte_carrier_nr_t* carrier)
{
  if (srslte_pdsch_nr_set_carrier(&q->pdsch, carrier) < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  if (srslte_dmrs_sch_set_carrier(&q->dmrs_pdsch, carrier) < SRSLTE_SUCCESS) {
    ERROR("Error DMRS\n");
    return SRSLTE_ERROR;
  }

  if (ue_dl_nr_alloc_prb(q, carrier->nof_prb)) {
    ERROR("Error allocating\n");
    return SRSLTE_ERROR;
  }

  if (carrier->nof_prb != q->carrier.nof_prb) {
    for (uint32_t i = 0; i < q->nof_rx_antennas; i++) {
      srslte_ofdm_cfg_t cfg = {};
      cfg.nof_prb           = carrier->nof_prb;
      cfg.symbol_sz         = srslte_min_symbol_sz_rb(carrier->nof_prb);
      cfg.cp                = SRSLTE_CP_NORM;
      srslte_ofdm_rx_init_cfg(&q->fft[i], &cfg);
    }
  }

  q->carrier = *carrier;

  return SRSLTE_SUCCESS;
}

int srslte_ue_dl_nr_set_config(srslte_ue_dl_nr_t* q, const srslte_ue_dl_nr_pdcch_cfg_t* cfg)
{
  if (q == NULL || cfg == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  // Copy new configuration
  q->cfg = *cfg;

  // iterate over all possible CORESET and initialise/update the present ones
  for (uint32_t i = 0; i < SRSLTE_UE_DL_NR_MAX_NOF_CORESET; i++) {
    if (cfg->coreset_present[i]) {
      if (srslte_dmrs_pdcch_estimator_init(&q->dmrs_pdcch[i], &q->carrier, &cfg->coreset[i]) < SRSLTE_SUCCESS) {
        return SRSLTE_ERROR;
      }
    }
  }

  return SRSLTE_SUCCESS;
}

void srslte_ue_dl_nr_estimate_fft(srslte_ue_dl_nr_t* q, const srslte_dl_slot_cfg_t* slot_cfg)
{
  if (q == NULL || slot_cfg == NULL) {
    return;
  }

  // OFDM demodulation
  for (uint32_t i = 0; i < q->nof_rx_antennas; i++) {
    srslte_ofdm_rx_sf(&q->fft[i]);
  }

  // Estimate PDCCH channel for every configured CORESET
  for (uint32_t i = 0; i < SRSLTE_UE_DL_NR_MAX_NOF_CORESET; i++) {
    if (q->cfg.coreset_present[i]) {
      srslte_dmrs_pdcch_estimate(&q->dmrs_pdcch[i], slot_cfg, q->sf_symbols[0]);
    }
  }
}

static int ue_dl_nr_find_dci_ncce(srslte_ue_dl_nr_t*     q,
                                  srslte_dci_msg_nr_t*   dci_msg,
                                  srslte_pdcch_nr_res_t* pdcch_res,
                                  uint32_t               coreset_id)
{
  srslte_dmrs_pdcch_measure_t m = {};

  // Measures the PDCCH transmission DMRS
  if (srslte_dmrs_pdcch_get_measure(&q->dmrs_pdcch[coreset_id], &dci_msg->location, &m) < SRSLTE_SUCCESS) {
    ERROR("Error getting measure location L=%d, ncce=%d\n", dci_msg->location.L, dci_msg->location.ncce);
    return SRSLTE_ERROR;
  }

  // If measured correlation is invalid, early return
  if (!isnormal(m.norm_corr)) {
    INFO("Discarded PDCCH candidate L=%d;ncce=%d; Invalid measurement;\n", dci_msg->location.L, dci_msg->location.ncce);
    return SRSLTE_SUCCESS;
  }

  // Compare EPRE with threshold
  if (m.epre_dBfs < q->pdcch_dmrs_epre_thr) {
    INFO("Discarded PDCCH candidate L=%d;ncce=%d; EPRE is too weak (%.1f<%.1f);\n",
         dci_msg->location.L,
         dci_msg->location.ncce,
         m.epre_dBfs,
         q->pdcch_dmrs_epre_thr);
    return SRSLTE_SUCCESS;
  }

  // Compare DMRS correlation with threshold
  if (m.norm_corr < q->pdcch_dmrs_corr_thr) {
    INFO("Discarded PDCCH candidate L=%d;ncce=%d; Correlation is too low (%.1f<%.1f); EPRE=%+.2f; RSRP=%+.2f;\n",
         dci_msg->location.L,
         dci_msg->location.ncce,
         m.norm_corr,
         q->pdcch_dmrs_corr_thr,
         m.epre_dBfs,
         m.rsrp_dBfs);
    return SRSLTE_SUCCESS;
  }

  // Extract PDCCH channel estimates
  if (srslte_dmrs_pdcch_get_ce(&q->dmrs_pdcch[coreset_id], &dci_msg->location, q->pdcch_ce) < SRSLTE_SUCCESS) {
    ERROR("Error extracting PDCCH DMRS\n");
    return SRSLTE_ERROR;
  }

  // Decode PDCCH
  if (srslte_pdcch_nr_decode(&q->pdcch, q->sf_symbols[0], q->pdcch_ce, dci_msg, pdcch_res) < SRSLTE_SUCCESS) {
    ERROR("Error decoding PDCCH\n");
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

static int ue_dl_nr_find_dl_dci_ss(srslte_ue_dl_nr_t*          q,
                                   const srslte_dl_slot_cfg_t* slot_cfg,
                                   uint32_t                    search_space_id,
                                   uint16_t                    rnti,
                                   srslte_rnti_type_t          rnti_type,
                                   srslte_dci_dl_nr_t*         dci_dl_list,
                                   uint32_t                    nof_dci_msg)
{
  // Check inputs
  if (q == NULL || slot_cfg == NULL || dci_dl_list == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  // Select Search space
  srslte_search_space_t* search_space = &q->cfg.search_space[search_space_id];

  // Select CORESET
  uint32_t coreset_id = search_space->coreset_id;
  if (coreset_id >= SRSLTE_UE_DL_NR_MAX_NOF_CORESET || !q->cfg.coreset_present[coreset_id]) {
    ERROR("CORESET %d is not present in search space %d\n", search_space->coreset_id, search_space->id);
    return SRSLTE_ERROR;
  }
  srslte_coreset_t* coreset = &q->cfg.coreset[search_space->coreset_id];

  // Set CORESET in PDCCH decoder
  if (srslte_pdcch_nr_set_carrier(&q->pdcch, &q->carrier, coreset) < SRSLTE_SUCCESS) {
    ERROR("Setting carrier and CORESETºn");
    return SRSLTE_ERROR;
  }

  uint32_t count = 0;

  // Hard-coded values
  srslte_dci_format_nr_t dci_format = srslte_dci_format_nr_1_0;

  // Calculate number of DCI bits
  int dci_nof_bits = srslte_dci_nr_format_1_0_sizeof(&q->carrier, coreset, rnti_type);
  if (dci_nof_bits <= SRSLTE_SUCCESS) {
    ERROR("Error DCI size\n");
    return SRSLTE_ERROR;
  }

  // Iterate all possible aggregation levels
  for (uint32_t L = 0; L < SRSLTE_SEARCH_SPACE_NOF_AGGREGATION_LEVELS_NR && count < nof_dci_msg; L++) {
    // Calculate possible PDCCH DCI candidates
    uint32_t candidates[SRSLTE_SEARCH_SPACE_MAX_NOF_CANDIDATES_NR] = {};
    int nof_candidates = srslte_pdcch_nr_locations_coreset(coreset, search_space, rnti, L, slot_cfg->idx, candidates);
    if (nof_candidates < SRSLTE_SUCCESS) {
      ERROR("Error calculating DCI candidate location\n");
      return SRSLTE_ERROR;
    }

    // Iterate over the candidates
    for (int ncce_idx = 0; ncce_idx < nof_candidates && count < nof_dci_msg; ncce_idx++) {
      srslte_dci_msg_nr_t dci_msg = {};
      dci_msg.location.L          = L;
      dci_msg.location.ncce       = candidates[ncce_idx];
      dci_msg.search_space        = search_space->type;
      dci_msg.rnti_type           = rnti_type;
      dci_msg.rnti                = rnti;
      dci_msg.format              = dci_format;
      dci_msg.nof_bits            = (uint32_t)dci_nof_bits;

      srslte_pdcch_nr_res_t res = {};
      if (ue_dl_nr_find_dci_ncce(q, &dci_msg, &res, coreset_id) < SRSLTE_SUCCESS) {
        return SRSLTE_ERROR;
      }

      if (res.crc) {
        if (srslte_dci_nr_format_1_0_unpack(&q->carrier, coreset, &dci_msg, &dci_dl_list[count]) < SRSLTE_SUCCESS) {
          ERROR("Error unpacking DCI\n");
          return SRSLTE_ERROR;
        }

        INFO("Found DCI in L=%d,ncce=%d\n", dci_msg.location.L, dci_msg.location.ncce);
        count++;
      }
    }
  }

  return (int)count;
}

int srslte_ue_dl_nr_find_dl_dci(srslte_ue_dl_nr_t*          q,
                                const srslte_dl_slot_cfg_t* slot_cfg,
                                uint16_t                    rnti,
                                srslte_dci_dl_nr_t*         dci_dl_list,
                                uint32_t                    nof_dci_msg)
{
  int count = 0;

  // Check inputs
  if (q == NULL || slot_cfg == NULL || dci_dl_list == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  // If the UE looks for a RAR and RA search space is provided, search for it
  if (q->cfg.ra_search_space_present && rnti == q->cfg.ra_rnti) {
    // Check if the RA search space is valid
    uint32_t ra_ss_id = q->cfg.ra_search_space_id;
    if (ra_ss_id >= SRSLTE_UE_DL_NR_MAX_NOF_SEARCH_SPACE || !q->cfg.search_space_present[ra_ss_id]) {
      ERROR("Invalid RA Search Space ID (%d)", ra_ss_id);
      return SRSLTE_ERROR;
    }

    // Find DCIs in the RA search space
    return ue_dl_nr_find_dl_dci_ss(
        q, slot_cfg, ra_ss_id, rnti, srslte_rnti_type_ra, &dci_dl_list[count], nof_dci_msg - count);
  }

  // Iterate all possible search spaces
  for (uint32_t i = 0; i < SRSLTE_UE_DL_NR_MAX_NOF_SEARCH_SPACE && count < nof_dci_msg; i++) {
    // Skip search space if not present
    if (!q->cfg.search_space_present[i]) {
      continue;
    }

    // Find DCIs in the selected search space
    int ret =
        ue_dl_nr_find_dl_dci_ss(q, slot_cfg, i, rnti, srslte_rnti_type_c, &dci_dl_list[count], nof_dci_msg - count);
    if (ret < SRSLTE_SUCCESS) {
      ERROR("Error searching DCI");
      return SRSLTE_ERROR;
    }

    // Count the found DCIs
    count += ret;
  }

  return count;
}

int srslte_ue_dl_nr_decode_pdsch(srslte_ue_dl_nr_t*          q,
                                 const srslte_dl_slot_cfg_t* slot,
                                 const srslte_sch_cfg_nr_t*  cfg,
                                 srslte_pdsch_res_nr_t*      res)
{

  if (srslte_dmrs_sch_estimate(&q->dmrs_pdsch, slot, cfg, &cfg->grant, q->sf_symbols[0], &q->chest) < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  if (srslte_pdsch_nr_decode(&q->pdsch, cfg, &cfg->grant, &q->chest, q->sf_symbols, res) < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  if (SRSLTE_DEBUG_ENABLED && srslte_verbose >= SRSLTE_VERBOSE_INFO && !handler_registered) {
    char str[512];
    srslte_ue_dl_nr_pdsch_info(q, cfg, res, str, sizeof(str));
    INFO("PDSCH: %s\n", str);
  }

  return SRSLTE_SUCCESS;
}

int srslte_ue_dl_nr_pdsch_info(const srslte_ue_dl_nr_t*     q,
                               const srslte_sch_cfg_nr_t*   cfg,
                               const srslte_pdsch_res_nr_t* res,
                               char*                        str,
                               uint32_t                     str_len)
{
  int len = 0;

  // Append channel estimator info
  // ...

  // Append PDSCH info
  len += srslte_pdsch_nr_rx_info(&q->pdsch, cfg, &cfg->grant, res, &str[len], str_len - len);

  return len;
}
