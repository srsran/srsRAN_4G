/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/phy/gnb/gnb_ul.h"

/**
 * @brief Shifts FFT window a fraction of the cyclic prefix. Set to 0.0f for disabling.
 * @note Increases protection against inter-symbol interference in case of synchronization error in expense of computing
 * performance
 */
#define GNB_UL_NR_FFT_WINDOW_OFFSET 0.5f

static int gnb_ul_alloc_prb(srsran_gnb_ul_t* q, uint32_t new_nof_prb)
{
  if (q->max_prb < new_nof_prb) {
    q->max_prb = new_nof_prb;

    srsran_chest_dl_res_free(&q->chest);
    if (srsran_chest_dl_res_init(&q->chest, q->max_prb) < SRSRAN_SUCCESS) {
      return SRSRAN_ERROR;
    }

    if (q->sf_symbols[0] != NULL) {
      free(q->sf_symbols[0]);
    }

    q->sf_symbols[0] = srsran_vec_cf_malloc(SRSRAN_SLOT_LEN_RE_NR(q->max_prb));
    if (q->sf_symbols[0] == NULL) {
      ERROR("Malloc");
      return SRSRAN_ERROR;
    }
  }

  return SRSRAN_SUCCESS;
}

int srsran_gnb_ul_init(srsran_gnb_ul_t* q, cf_t* input, const srsran_gnb_ul_args_t* args)
{
  if (q == NULL || args == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  if (gnb_ul_alloc_prb(q, args->nof_max_prb) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  if (srsran_pusch_nr_init_gnb(&q->pusch, &args->pusch) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  if (srsran_pucch_nr_init(&q->pucch, &args->pucch) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  if (srsran_dmrs_sch_init(&q->dmrs, true) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  srsran_ofdm_cfg_t ofdm_cfg = {};
  ofdm_cfg.nof_prb           = args->nof_max_prb;
  ofdm_cfg.in_buffer         = input;
  ofdm_cfg.out_buffer        = q->sf_symbols[0];
  ofdm_cfg.rx_window_offset  = GNB_UL_NR_FFT_WINDOW_OFFSET;
  ofdm_cfg.symbol_sz         = srsran_min_symbol_sz_rb(args->nof_max_prb);
  ofdm_cfg.keep_dc           = true;

  if (srsran_ofdm_rx_init_cfg(&q->fft, &ofdm_cfg) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

void srsran_gnb_ul_free(srsran_gnb_ul_t* q)
{
  if (q == NULL) {
    return;
  }

  srsran_ofdm_tx_free(&q->fft);
  srsran_pusch_nr_free(&q->pusch);
  srsran_pucch_nr_free(&q->pucch);
  srsran_dmrs_sch_free(&q->dmrs);
  srsran_chest_dl_res_free(&q->chest);

  if (q->sf_symbols[0] != NULL) {
    free(q->sf_symbols[0]);
  }

  SRSRAN_MEM_ZERO(q, srsran_gnb_ul_t, 1);
}

int srsran_gnb_ul_set_carrier(srsran_gnb_ul_t* q, const srsran_carrier_nr_t* carrier)
{
  if (q == NULL || carrier == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  q->carrier = *carrier;

  if (gnb_ul_alloc_prb(q, carrier->nof_prb) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  if (srsran_pusch_nr_set_carrier(&q->pusch, carrier) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  if (srsran_pucch_nr_set_carrier(&q->pucch, carrier) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  if (srsran_dmrs_sch_set_carrier(&q->dmrs, carrier) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  srsran_ofdm_cfg_t ofdm_cfg = {};
  ofdm_cfg.nof_prb           = carrier->nof_prb;
  ofdm_cfg.rx_window_offset  = GNB_UL_NR_FFT_WINDOW_OFFSET;
  ofdm_cfg.symbol_sz         = srsran_min_symbol_sz_rb(carrier->nof_prb);
  ofdm_cfg.keep_dc           = true;

  if (srsran_ofdm_rx_init_cfg(&q->fft, &ofdm_cfg) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

int srsran_gnb_ul_fft(srsran_gnb_ul_t* q)
{
  if (q == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  srsran_ofdm_rx_sf(&q->fft);

  return SRSRAN_SUCCESS;
}

int srsran_gnb_ul_get_pusch(srsran_gnb_ul_t*             q,
                            const srsran_slot_cfg_t*     slot_cfg,
                            const srsran_sch_cfg_nr_t*   cfg,
                            const srsran_sch_grant_nr_t* grant,
                            srsran_pusch_res_nr_t*       data)
{
  if (q == NULL || cfg == NULL || grant == NULL || data == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  if (srsran_dmrs_sch_estimate(&q->dmrs, slot_cfg, cfg, grant, q->sf_symbols[0], &q->chest) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  if (srsran_pusch_nr_decode(&q->pusch, cfg, grant, &q->chest, q->sf_symbols, data) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}