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
#include "srslte/phy/ue/ue_ul_nr.h"
#include "srslte/phy/utils/debug.h"
#include <complex.h>

int srslte_ue_ul_nr_init(srslte_ue_ul_nr_t* q, cf_t* output, const srslte_ue_ul_nr_args_t* args)
{
  if (q == NULL || output == NULL || args == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  // Initialise memory
  SRSLTE_MEM_ZERO(q, srslte_ue_ul_nr_t, 1);

  q->max_prb = args->nof_max_prb;

  q->sf_symbols[0] = srslte_vec_cf_malloc(SRSLTE_SLOT_LEN_RE_NR(q->max_prb));
  if (q->sf_symbols[0] == NULL) {
    ERROR("Malloc");
    return SRSLTE_ERROR;
  }

  if (srslte_pusch_nr_init_ue(&q->pusch, &args->pusch) < SRSLTE_SUCCESS) {
    ERROR("Initiating PUSCH");
    return SRSLTE_ERROR;
  }

  srslte_ofdm_cfg_t fft_cfg = {};
  fft_cfg.nof_prb           = args->nof_max_prb;
  fft_cfg.symbol_sz         = srslte_min_symbol_sz_rb(args->nof_max_prb);
  fft_cfg.keep_dc           = true;
  fft_cfg.in_buffer         = q->sf_symbols[0];
  fft_cfg.out_buffer        = output;
  if (srslte_ofdm_tx_init_cfg(&q->ifft, &fft_cfg) < SRSLTE_SUCCESS) {
    ERROR("Initiating OFDM");
    return SRSLTE_ERROR;
  }

  if (srslte_dmrs_sch_init(&q->dmrs, false) < SRSLTE_SUCCESS) {
    ERROR("Error DMRS\n");
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

int srslte_ue_ul_nr_set_carrier(srslte_ue_ul_nr_t* q, const srslte_carrier_nr_t* carrier)
{
  if (q == NULL || carrier == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  q->carrier = *carrier;

  srslte_ofdm_cfg_t fft_cfg = {};
  fft_cfg.nof_prb           = carrier->nof_prb;
  fft_cfg.symbol_sz         = srslte_min_symbol_sz_rb(carrier->nof_prb);
  fft_cfg.keep_dc           = true;
  if (srslte_ofdm_tx_init_cfg(&q->ifft, &fft_cfg) < SRSLTE_SUCCESS) {
    ERROR("Initiating OFDM");
    return SRSLTE_ERROR;
  }

  if (srslte_pusch_nr_set_carrier(&q->pusch, carrier) < SRSLTE_SUCCESS) {
    ERROR("Setting PUSCH carrier");
    return SRSLTE_ERROR;
  }

  if (srslte_dmrs_sch_set_carrier(&q->dmrs, carrier)) {
    ERROR("Setting DMRS carrier");
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

int srslte_ue_ul_nr_encode_pusch(srslte_ue_ul_nr_t*         q,
                                 const srslte_slot_cfg_t*   slot_cfg,
                                 const srslte_sch_cfg_nr_t* pusch_cfg,
                                 uint8_t*                   data_)
{
  uint8_t* data[SRSLTE_MAX_TB] = {};
  data[0]                      = data_;

  // Check inputs
  if (q == NULL || pusch_cfg == NULL || data_ == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  // Encode PUSCH
  if (srslte_pusch_nr_encode(&q->pusch, pusch_cfg, &pusch_cfg->grant, data, q->sf_symbols) < SRSLTE_SUCCESS) {
    ERROR("Encoding PUSCH");
    return SRSLTE_ERROR;
  }

  // Put DMRS
  if (srslte_dmrs_sch_put_sf(&q->dmrs, slot_cfg, pusch_cfg, &pusch_cfg->grant, q->sf_symbols[0])) {
    ERROR("Putting DMRS");
    return SRSLTE_ERROR;
  }

  // Temporary symbol phase shift
  uint32_t nof_re = SRSLTE_NRE * q->carrier.nof_prb;
  for (uint32_t i = 0; i < 2; i++) {
    for (uint32_t j = 0; j < 7; j++) {
      srslte_vec_sc_prod_ccc(&q->sf_symbols[0][(i * 7 + j) * nof_re],
                             cexpf(I * (11.0f * j - 2.0f) * M_PI / 16),
                             &q->sf_symbols[0][(i * 7 + j) * nof_re],
                             nof_re);
    }
  }

  // Generate signal
  srslte_ofdm_tx_sf(&q->ifft);

  return SRSLTE_SUCCESS;
}

void srslte_ue_ul_nr_free(srslte_ue_ul_nr_t* q)
{
  if (q == NULL) {
    return;
  }
  srslte_ofdm_tx_free(&q->ifft);
  if (q->sf_symbols[0] != NULL) {
    free(q->sf_symbols[0]);
  }
  srslte_pusch_nr_free(&q->pusch);
  srslte_dmrs_sch_free(&q->dmrs);
}

int srslte_ue_ul_nr_pusch_info(const srslte_ue_ul_nr_t* q, const srslte_sch_cfg_nr_t* cfg, char* str, uint32_t str_len)
{
  int len = 0;

  // Append PDSCH info
  len += srslte_pusch_nr_tx_info(&q->pusch, cfg, &cfg->grant, &str[len], str_len - len);

  return len;
}