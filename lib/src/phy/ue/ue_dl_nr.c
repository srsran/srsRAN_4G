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

static int ue_dl_alloc_prb(srslte_ue_dl_nr_t* q, uint32_t new_nof_prb)
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

  if (srslte_pdsch_nr_init_ue(&q->pdsch, &args->pdsch) < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  if (ue_dl_alloc_prb(q, SRSLTE_MAX_PRB_NR)) {
    ERROR("Error allocating\n");
    return SRSLTE_ERROR;
  }

  srslte_ofdm_cfg_t fft_cfg = {};
  fft_cfg.nof_prb           = 100;
  fft_cfg.symbol_sz         = srslte_symbol_sz(100);
  fft_cfg.keep_dc           = true;

  for (uint32_t i = 0; i < q->nof_rx_antennas; i++) {
    fft_cfg.in_buffer  = input[i];
    fft_cfg.out_buffer = q->sf_symbols[i];
    srslte_ofdm_rx_init_cfg(&q->fft[i], &fft_cfg);
  }

  if (srslte_dmrs_pdsch_init(&q->dmrs, true) < SRSLTE_SUCCESS) {
    ERROR("Error DMRS\n");
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
  srslte_dmrs_pdsch_free(&q->dmrs);

  memset(q, 0, sizeof(srslte_ue_dl_nr_t));
}

int srslte_ue_dl_nr_set_carrier(srslte_ue_dl_nr_t* q, const srslte_carrier_nr_t* carrier)
{
  if (srslte_pdsch_nr_set_carrier(&q->pdsch, carrier) < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  if (srslte_dmrs_pdsch_set_carrier(&q->dmrs, carrier) < SRSLTE_SUCCESS) {
    ERROR("Error DMRS\n");
    return SRSLTE_ERROR;
  }

  if (ue_dl_alloc_prb(q, carrier->nof_prb)) {
    ERROR("Error allocating\n");
    return SRSLTE_ERROR;
  }

  if (carrier->nof_prb != q->carrier.nof_prb) {
    for (uint32_t i = 0; i < q->nof_rx_antennas; i++) {
      srslte_ofdm_rx_set_prb(&q->fft[i], SRSLTE_CP_NORM, carrier->nof_prb);
    }
  }

  q->carrier = *carrier;

  return SRSLTE_SUCCESS;
}

void srslte_ue_dl_nr_estimate_fft(srslte_ue_dl_nr_t* q)
{
  if (q == NULL) {
    return;
  }

  for (uint32_t i = 0; i < q->nof_rx_antennas; i++) {
    srslte_ofdm_rx_sf(&q->fft[i]);
  }
}

int srslte_ue_dl_nr_pdsch_get(srslte_ue_dl_nr_t*             q,
                              const srslte_dl_slot_cfg_t*    slot,
                              const srslte_pdsch_cfg_nr_t*   cfg,
                              const srslte_pdsch_grant_nr_t* grant,
                              srslte_pdsch_res_nr_t*         res)
{

  if (srslte_dmrs_pdsch_estimate(&q->dmrs, slot, cfg, grant, q->sf_symbols[0], &q->chest) < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  if (srslte_pdsch_nr_decode(&q->pdsch, cfg, grant, &q->chest, q->sf_symbols, res) < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}
