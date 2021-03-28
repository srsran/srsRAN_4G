/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#include "srsran/phy/enb/enb_dl_nr.h"
#include <complex.h>

static int enb_dl_alloc_prb(srsran_enb_dl_nr_t* q, uint32_t new_nof_prb)
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

int srsran_enb_dl_nr_init(srsran_enb_dl_nr_t* q, cf_t* output[SRSRAN_MAX_PORTS], const srsran_enb_dl_nr_args_t* args)
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

  if (enb_dl_alloc_prb(q, args->nof_max_prb) < SRSRAN_SUCCESS) {
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

void srsran_enb_dl_nr_free(srsran_enb_dl_nr_t* q)
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

  SRSRAN_MEM_ZERO(q, srsran_enb_dl_nr_t, 1);
}

int srsran_enb_dl_nr_set_carrier(srsran_enb_dl_nr_t* q, const srsran_carrier_nr_t* carrier)
{
  if (srsran_pdsch_nr_set_carrier(&q->pdsch, carrier) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  if (srsran_dmrs_sch_set_carrier(&q->dmrs, carrier) < SRSRAN_SUCCESS) {
    ERROR("Error DMRS");
    return SRSRAN_ERROR;
  }

  if (enb_dl_alloc_prb(q, carrier->nof_prb) < SRSRAN_SUCCESS) {
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

int srsran_enb_dl_nr_set_coreset(srsran_enb_dl_nr_t* q, const srsran_coreset_t* coreset)
{
  if (q == NULL || coreset == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  q->coreset = *coreset;

  if (srsran_pdcch_nr_set_carrier(&q->pdcch, &q->carrier, &q->coreset) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

void srsran_enb_dl_nr_gen_signal(srsran_enb_dl_nr_t* q)
{
  if (q == NULL) {
    return;
  }

  for (uint32_t i = 0; i < q->nof_tx_antennas; i++) {
    srsran_ofdm_tx_sf(&q->fft[i]);
  }
}

int srsran_enb_dl_nr_base_zero(srsran_enb_dl_nr_t* q)
{
  if (q == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  for (uint32_t i = 0; i < q->nof_tx_antennas; i++) {
    srsran_vec_cf_zero(q->sf_symbols[i], SRSRAN_SLOT_LEN_RE_NR(q->carrier.nof_prb));
  }

  return SRSRAN_SUCCESS;
}

int srsran_enb_dl_nr_pdcch_put(srsran_enb_dl_nr_t*       q,
                               const srsran_slot_cfg_t*  slot_cfg,
                               const srsran_dci_dl_nr_t* dci_dl)
{
  if (q == NULL || slot_cfg == NULL || dci_dl == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Put DMRS
  if (srsran_dmrs_pdcch_put(&q->carrier, &q->coreset, slot_cfg, &dci_dl->location, q->sf_symbols[0]) < SRSRAN_SUCCESS) {
    ERROR("Error putting PDCCH DMRS");
    return SRSRAN_ERROR;
  }

  // Pack DCI
  srsran_dci_msg_nr_t dci_msg = {};
  if (srsran_dci_nr_pack(&q->carrier, &q->coreset, dci_dl, &dci_msg) < SRSRAN_SUCCESS) {
    ERROR("Error packing DL DCI");
    return SRSRAN_ERROR;
  }

  // PDCCH Encode
  if (srsran_pdcch_nr_encode(&q->pdcch, &dci_msg, q->sf_symbols[0]) < SRSRAN_SUCCESS) {
    ERROR("Error encoding PDCCH");
    return SRSRAN_ERROR;
  }

  INFO("DCI DL NR: L=%d; ncce=%d;", dci_dl->location.L, dci_dl->location.ncce);

  return SRSRAN_SUCCESS;
}

int srsran_enb_dl_nr_pdsch_put(srsran_enb_dl_nr_t*        q,
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

int srsran_enb_dl_nr_pdsch_info(const srsran_enb_dl_nr_t*  q,
                                const srsran_sch_cfg_nr_t* cfg,
                                char*                      str,
                                uint32_t                   str_len)
{
  int len = 0;

  // Append PDSCH info
  len += srsran_pdsch_nr_tx_info(&q->pdsch, cfg, &cfg->grant, &str[len], str_len - len);

  return len;
}
