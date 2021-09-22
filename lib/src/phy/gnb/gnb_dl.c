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

#include "srsran/phy/gnb/gnb_dl.h"
#include <complex.h>

static float gnb_dl_get_norm_factor(uint32_t nof_prb)
{
  return 0.05f / sqrtf(nof_prb);
}

static int gnb_dl_alloc_prb(srsran_gnb_dl_t* q, uint32_t new_nof_prb)
{
  if (q->max_prb < new_nof_prb) {
    q->max_prb = new_nof_prb;

    for (uint32_t i = 0; i < q->nof_tx_antennas; i++) {
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

int srsran_gnb_dl_init(srsran_gnb_dl_t* q, cf_t* output[SRSRAN_MAX_PORTS], const srsran_gnb_dl_args_t* args)
{
  if (!q || !output || !args) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  if (args->nof_tx_antennas == 0) {
    ERROR("Error invalid number of antennas (%d)", args->nof_tx_antennas);
    return SRSRAN_ERROR;
  }

  q->nof_tx_antennas = args->nof_tx_antennas;

  if (srsran_pdsch_nr_init_enb(&q->pdsch, &args->pdsch) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  if (gnb_dl_alloc_prb(q, args->nof_max_prb) < SRSRAN_SUCCESS) {
    ERROR("Error allocating");
    return SRSRAN_ERROR;
  }

  srsran_ofdm_cfg_t fft_cfg = {};
  fft_cfg.nof_prb           = args->nof_max_prb;
  fft_cfg.symbol_sz         = srsran_min_symbol_sz_rb(args->nof_max_prb);
  fft_cfg.keep_dc           = true;

  for (uint32_t i = 0; i < q->nof_tx_antennas; i++) {
    fft_cfg.in_buffer  = q->sf_symbols[i];
    fft_cfg.out_buffer = output[i];
    srsran_ofdm_tx_init_cfg(&q->fft[i], &fft_cfg);
  }

  if (srsran_dmrs_sch_init(&q->dmrs, false) < SRSRAN_SUCCESS) {
    ERROR("Error DMRS");
    return SRSRAN_ERROR;
  }

  if (srsran_pdcch_nr_init_tx(&q->pdcch, &args->pdcch) < SRSRAN_SUCCESS) {
    ERROR("Error PDCCH");
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

void srsran_gnb_dl_free(srsran_gnb_dl_t* q)
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

  srsran_pdsch_nr_free(&q->pdsch);
  srsran_dmrs_sch_free(&q->dmrs);

  srsran_pdcch_nr_free(&q->pdcch);

  SRSRAN_MEM_ZERO(q, srsran_gnb_dl_t, 1);
}

int srsran_gnb_dl_set_carrier(srsran_gnb_dl_t* q, const srsran_carrier_nr_t* carrier)
{
  if (srsran_pdsch_nr_set_carrier(&q->pdsch, carrier) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  if (srsran_dmrs_sch_set_carrier(&q->dmrs, carrier) < SRSRAN_SUCCESS) {
    ERROR("Error DMRS");
    return SRSRAN_ERROR;
  }

  if (gnb_dl_alloc_prb(q, carrier->nof_prb) < SRSRAN_SUCCESS) {
    ERROR("Error allocating");
    return SRSRAN_ERROR;
  }

  if (carrier->nof_prb != q->carrier.nof_prb) {
    srsran_ofdm_cfg_t fft_cfg = {};
    fft_cfg.nof_prb           = carrier->nof_prb;
    fft_cfg.symbol_sz         = srsran_min_symbol_sz_rb(carrier->nof_prb);
    fft_cfg.keep_dc           = true;

    for (uint32_t i = 0; i < q->nof_tx_antennas; i++) {
      fft_cfg.in_buffer = q->sf_symbols[i];
      srsran_ofdm_tx_init_cfg(&q->fft[i], &fft_cfg);
    }
  }

  q->carrier = *carrier;

  return SRSRAN_SUCCESS;
}

int srsran_gnb_dl_set_pdcch_config(srsran_gnb_dl_t*             q,
                                   const srsran_pdcch_cfg_nr_t* cfg,
                                   const srsran_dci_cfg_nr_t*   dci_cfg)
{
  if (q == NULL || cfg == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  q->pdcch_cfg = *cfg;

  if (srsran_pdcch_nr_set_carrier(&q->pdcch, &q->carrier, &q->pdcch_cfg.coreset[0]) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  if (srsran_dci_nr_set_cfg(&q->dci, dci_cfg) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

void srsran_gnb_dl_gen_signal(srsran_gnb_dl_t* q)
{
  if (q == NULL) {
    return;
  }

  float norm_factor = gnb_dl_get_norm_factor(q->pdsch.carrier.nof_prb);

  for (uint32_t i = 0; i < q->nof_tx_antennas; i++) {
    srsran_ofdm_tx_sf(&q->fft[i]);

    srsran_vec_sc_prod_cfc(q->fft[i].cfg.out_buffer, norm_factor, q->fft[i].cfg.out_buffer, (uint32_t)q->fft[i].sf_sz);
  }
}

float srsran_gnb_dl_get_maximum_signal_power_dBfs(uint32_t nof_prb)
{
  return srsran_convert_amplitude_to_dB(gnb_dl_get_norm_factor(nof_prb)) +
         srsran_convert_power_to_dB((float)nof_prb * SRSRAN_NRE) + 3.0f;
}

int srsran_gnb_dl_base_zero(srsran_gnb_dl_t* q)
{
  if (q == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  for (uint32_t i = 0; i < q->nof_tx_antennas; i++) {
    srsran_vec_cf_zero(q->sf_symbols[i], SRSRAN_SLOT_LEN_RE_NR(q->carrier.nof_prb));
  }

  return SRSRAN_SUCCESS;
}

static int
gnb_dl_pdcch_put_msg(srsran_gnb_dl_t* q, const srsran_slot_cfg_t* slot_cfg, const srsran_dci_msg_nr_t* dci_msg)
{
  if (dci_msg->ctx.coreset_id >= SRSRAN_UE_DL_NR_MAX_NOF_CORESET ||
      !q->pdcch_cfg.coreset_present[dci_msg->ctx.coreset_id]) {
    ERROR("Invalid CORESET ID %d", dci_msg->ctx.coreset_id);
    return SRSRAN_ERROR;
  }
  srsran_coreset_t* coreset = &q->pdcch_cfg.coreset[dci_msg->ctx.coreset_id];

  if (srsran_pdcch_nr_set_carrier(&q->pdcch, &q->carrier, coreset) < SRSRAN_SUCCESS) {
    ERROR("Error setting PDCCH carrier/CORESET");
    return SRSRAN_ERROR;
  }

  // Put DMRS
  if (srsran_dmrs_pdcch_put(&q->carrier, coreset, slot_cfg, &dci_msg->ctx.location, q->sf_symbols[0]) <
      SRSRAN_SUCCESS) {
    ERROR("Error putting PDCCH DMRS");
    return SRSRAN_ERROR;
  }

  // PDCCH Encode
  if (srsran_pdcch_nr_encode(&q->pdcch, dci_msg, q->sf_symbols[0]) < SRSRAN_SUCCESS) {
    ERROR("Error encoding PDCCH");
    return SRSRAN_ERROR;
  }

  INFO("DCI DL NR: L=%d; ncce=%d;", dci_msg->ctx.location.L, dci_msg->ctx.location.ncce);

  return SRSRAN_SUCCESS;
}

int srsran_gnb_dl_pdcch_put_dl(srsran_gnb_dl_t* q, const srsran_slot_cfg_t* slot_cfg, const srsran_dci_dl_nr_t* dci_dl)
{
  if (q == NULL || slot_cfg == NULL || dci_dl == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Pack DCI
  srsran_dci_msg_nr_t dci_msg = {};
  if (srsran_dci_nr_dl_pack(&q->dci, dci_dl, &dci_msg) < SRSRAN_SUCCESS) {
    ERROR("Error packing DL DCI");
    return SRSRAN_ERROR;
  }

  INFO("DCI DL NR: L=%d; ncce=%d;", dci_dl->ctx.location.L, dci_dl->ctx.location.ncce);

  return gnb_dl_pdcch_put_msg(q, slot_cfg, &dci_msg);
}

int srsran_gnb_dl_pdcch_put_ul(srsran_gnb_dl_t* q, const srsran_slot_cfg_t* slot_cfg, const srsran_dci_ul_nr_t* dci_ul)
{
  if (q == NULL || slot_cfg == NULL || dci_ul == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Pack DCI
  srsran_dci_msg_nr_t dci_msg = {};
  if (srsran_dci_nr_ul_pack(&q->dci, dci_ul, &dci_msg) < SRSRAN_SUCCESS) {
    ERROR("Error packing UL DCI");
    return SRSRAN_ERROR;
  }

  INFO("DCI DL NR: L=%d; ncce=%d;", dci_ul->ctx.location.L, dci_ul->ctx.location.ncce);

  return gnb_dl_pdcch_put_msg(q, slot_cfg, &dci_msg);
}

int srsran_gnb_dl_pdsch_put(srsran_gnb_dl_t*           q,
                            const srsran_slot_cfg_t*   slot,
                            const srsran_sch_cfg_nr_t* cfg,
                            uint8_t*                   data[SRSRAN_MAX_TB])
{
  if (srsran_dmrs_sch_put_sf(&q->dmrs, slot, cfg, &cfg->grant, q->sf_symbols[0]) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  if (srsran_pdsch_nr_encode(&q->pdsch, cfg, &cfg->grant, data, q->sf_symbols) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

int srsran_gnb_dl_pdsch_info(const srsran_gnb_dl_t* q, const srsran_sch_cfg_nr_t* cfg, char* str, uint32_t str_len)
{
  int len = 0;

  // Append PDSCH info
  len += srsran_pdsch_nr_tx_info(&q->pdsch, cfg, &cfg->grant, &str[len], str_len - len);

  return len;
}

int srsran_gnb_dl_pdcch_dl_info(const srsran_gnb_dl_t* q, const srsran_dci_dl_nr_t* dci, char* str, uint32_t str_len)
{
  int len = 0;

  // Append PDCCH info
  len += srsran_dci_dl_nr_to_str(&q->dci, dci, &str[len], str_len - len);

  return len;
}

int srsran_gnb_dl_pdcch_ul_info(const srsran_gnb_dl_t* q, const srsran_dci_ul_nr_t* dci, char* str, uint32_t str_len)
{
  int len = 0;

  // Append PDCCH info
  len += srsran_dci_ul_nr_to_str(&q->dci, dci, &str[len], str_len - len);

  return len;
}
