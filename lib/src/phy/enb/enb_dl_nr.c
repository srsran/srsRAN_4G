/**
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

#include "srslte/phy/enb/enb_dl_nr.h"

static int enb_dl_alloc_prb(srslte_enb_dl_nr_t* q, uint32_t new_nof_prb)
{
  if (q->max_prb < new_nof_prb) {
    q->max_prb = new_nof_prb;

    for (uint32_t i = 0; i < q->nof_tx_antennas; i++) {
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

int srslte_enb_dl_nr_init(srslte_enb_dl_nr_t* q, cf_t* output[SRSLTE_MAX_PORTS], const srslte_enb_dl_nr_args_t* args)
{
  if (!q || !output || !args) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  if (args->nof_tx_antennas == 0) {
    ERROR("Error invalid number of antennas (%d)\n", args->nof_tx_antennas);
    return SRSLTE_ERROR;
  }

  q->nof_tx_antennas = args->nof_tx_antennas;

  if (srslte_pdsch_nr_init_enb(&q->pdsch, &args->pdsch) < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  if (enb_dl_alloc_prb(q, args->nof_max_prb) < SRSLTE_SUCCESS) {
    ERROR("Error allocating\n");
    return SRSLTE_ERROR;
  }

  srslte_ofdm_cfg_t fft_cfg = {};
  fft_cfg.nof_prb           = args->nof_max_prb;
  fft_cfg.symbol_sz         = srslte_symbol_sz(args->nof_max_prb);
  fft_cfg.keep_dc           = true;

  for (uint32_t i = 0; i < q->nof_tx_antennas; i++) {
    fft_cfg.in_buffer  = q->sf_symbols[i];
    fft_cfg.out_buffer = output[i];
    srslte_ofdm_tx_init_cfg(&q->fft[i], &fft_cfg);
  }

  if (srslte_dmrs_sch_init(&q->dmrs, false) < SRSLTE_SUCCESS) {
    ERROR("Error DMRS\n");
    return SRSLTE_ERROR;
  }

  if (srslte_pdcch_nr_init_tx(&q->pdcch, &args->pdcch) < SRSLTE_SUCCESS) {
    ERROR("Error PDCCH\n");
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

void srslte_enb_dl_nr_free(srslte_enb_dl_nr_t* q)
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

  srslte_pdsch_nr_free(&q->pdsch);
  srslte_dmrs_sch_free(&q->dmrs);

  srslte_pdcch_nr_free(&q->pdcch);

  SRSLTE_MEM_ZERO(q, srslte_enb_dl_nr_t, 1);
}

int srslte_enb_dl_nr_set_carrier(srslte_enb_dl_nr_t* q, const srslte_carrier_nr_t* carrier)
{
  if (srslte_pdsch_nr_set_carrier(&q->pdsch, carrier) < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  if (srslte_dmrs_sch_set_carrier(&q->dmrs, carrier) < SRSLTE_SUCCESS) {
    ERROR("Error DMRS\n");
    return SRSLTE_ERROR;
  }

  if (enb_dl_alloc_prb(q, carrier->nof_prb) < SRSLTE_SUCCESS) {
    ERROR("Error allocating\n");
    return SRSLTE_ERROR;
  }

  if (carrier->nof_prb != q->carrier.nof_prb) {
    for (uint32_t i = 0; i < q->nof_tx_antennas; i++) {
      srslte_ofdm_tx_set_prb(&q->fft[i], SRSLTE_CP_NORM, carrier->nof_prb);
    }
  }

  q->carrier = *carrier;

  return SRSLTE_SUCCESS;
}

int srslte_enb_dl_nr_set_coreset(srslte_enb_dl_nr_t* q, const srslte_coreset_t* coreset)
{
  if (q == NULL || coreset == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  q->coreset = *coreset;

  if (srslte_pdcch_nr_set_carrier(&q->pdcch, &q->carrier, &q->coreset) < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

void srslte_enb_dl_nr_gen_signal(srslte_enb_dl_nr_t* q)
{
  if (q == NULL) {
    return;
  }

  for (uint32_t i = 0; i < q->nof_tx_antennas; i++) {
    srslte_ofdm_tx_sf(&q->fft[i]);
  }
}

int srslte_enb_dl_nr_base_zero(srslte_enb_dl_nr_t* q)
{
  if (q == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  for (uint32_t i = 0; i < q->nof_tx_antennas; i++) {
    srslte_vec_cf_zero(q->sf_symbols[i], SRSLTE_SLOT_LEN_RE_NR(q->carrier.nof_prb));
  }

  return SRSLTE_SUCCESS;
}

int srslte_enb_dl_nr_pdcch_put(srslte_enb_dl_nr_t*         q,
                               const srslte_dl_slot_cfg_t* slot_cfg,
                               const srslte_dci_dl_nr_t*   dci_dl)
{
  if (q == NULL || slot_cfg == NULL || dci_dl == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  // Put DMRS
  if (srslte_dmrs_pdcch_put(&q->carrier, &q->coreset, slot_cfg, &dci_dl->location, q->sf_symbols[0]) < SRSLTE_SUCCESS) {
    ERROR("Error putting PDCCH DMRS\n");
    return SRSLTE_ERROR;
  }

  // Pack DCI
  srslte_dci_msg_nr_t dci_msg = {};
  if (srslte_dci_nr_pack(&q->carrier, &q->coreset, dci_dl, &dci_msg) < SRSLTE_SUCCESS) {
    ERROR("Error packing DL DCI\n");
    return SRSLTE_ERROR;
  }

  // PDCCH Encode
  if (srslte_pdcch_nr_encode(&q->pdcch, &dci_msg, q->sf_symbols[0]) < SRSLTE_SUCCESS) {
    ERROR("Error encoding PDCCH\n");
    return SRSLTE_ERROR;
  }

  INFO("DCI DL NR: L=%d; ncce=%d;\n", dci_dl->location.L, dci_dl->location.ncce);

  return SRSLTE_SUCCESS;
}

int srslte_enb_dl_nr_pdsch_put(srslte_enb_dl_nr_t*         q,
                               const srslte_dl_slot_cfg_t* slot,
                               const srslte_sch_cfg_nr_t*  cfg,
                               uint8_t*                    data[SRSLTE_MAX_TB])
{

  if (srslte_dmrs_sch_put_sf(&q->dmrs, slot, cfg, &cfg->grant, q->sf_symbols[0]) < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  if (srslte_pdsch_nr_encode(&q->pdsch, cfg, &cfg->grant, data, q->sf_symbols) < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

int srslte_enb_dl_nr_pdsch_info(const srslte_enb_dl_nr_t*  q,
                                const srslte_sch_cfg_nr_t* cfg,
                                char*                      str,
                                uint32_t                   str_len)
{
  int len = 0;

  // Append PDSCH info
  len += srslte_pdsch_nr_tx_info(&q->pdsch, cfg, &cfg->grant, &str[len], str_len - len);

  return len;
}
