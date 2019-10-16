/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

#include "srslte/phy/enb/enb_ul.h"

#include "srslte/srslte.h"
#include <complex.h>
#include <math.h>
#include <string.h>

#define CURRENT_FFTSIZE   srslte_symbol_sz(q->cell.nof_prb)
#define CURRENT_SFLEN     SRSLTE_SF_LEN(CURRENT_FFTSIZE)

#define CURRENT_SLOTLEN_RE SRSLTE_SLOT_LEN_RE(q->cell.nof_prb, q->cell.cp)
#define CURRENT_SFLEN_RE SRSLTE_NOF_RE(q->cell)

int srslte_enb_ul_init(srslte_enb_ul_t *q,
                       cf_t *in_buffer,
                       uint32_t max_prb)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q != NULL) {
    ret = SRSLTE_ERROR;

    bzero(q, sizeof(srslte_enb_ul_t));

    q->sf_symbols = srslte_vec_malloc(SRSLTE_SF_LEN_RE(max_prb, SRSLTE_CP_NORM) * sizeof(cf_t));
    if (!q->sf_symbols) {
      perror("malloc");
      goto clean_exit;
    }

    q->chest_res.ce = srslte_vec_malloc(SRSLTE_SF_LEN_RE(max_prb, SRSLTE_CP_NORM) * sizeof(cf_t));
    if (!q->chest_res.ce) {
      perror("malloc");
      goto clean_exit;
    }

    if (srslte_ofdm_rx_init(&q->fft, SRSLTE_CP_NORM, in_buffer, q->sf_symbols, max_prb)) {
      ERROR("Error initiating FFT\n");
      goto clean_exit;
    }
    srslte_ofdm_set_normalize(&q->fft, false);
    srslte_ofdm_set_freq_shift(&q->fft, -0.5);

    if (srslte_pucch_init_enb(&q->pucch)) {
      ERROR("Error creating PUCCH object\n");
      goto clean_exit;
    }

    if (srslte_pusch_init_enb(&q->pusch, max_prb)) {
      ERROR("Error creating PUSCH object\n");
      goto clean_exit;
    }

    if (srslte_chest_ul_init(&q->chest, max_prb)) {
      ERROR("Error initiating channel estimator\n");
      goto clean_exit;
    }

    ret = SRSLTE_SUCCESS;

  } else {
    ERROR("Invalid parameters\n");
  }

clean_exit:
  if (ret == SRSLTE_ERROR) {
    srslte_enb_ul_free(q);
  }
  return ret;
}

void srslte_enb_ul_free(srslte_enb_ul_t *q)
{
  if (q) {

    srslte_ofdm_rx_free(&q->fft);
    srslte_pucch_free(&q->pucch);
    srslte_pusch_free(&q->pusch);
    srslte_chest_ul_free(&q->chest);

    if (q->sf_symbols) {
      free(q->sf_symbols);
    }
    if (q->chest_res.ce) {
      free(q->chest_res.ce);
    }
    bzero(q, sizeof(srslte_enb_ul_t));
  }
}

int srslte_enb_ul_set_cell(srslte_enb_ul_t* q, srslte_cell_t cell, srslte_refsignal_dmrs_pusch_cfg_t* pusch_cfg)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q != NULL && srslte_cell_isvalid(&cell)) {
    if (cell.id != q->cell.id || q->cell.nof_prb == 0) {
      q->cell = cell;

      if (srslte_ofdm_rx_set_prb(&q->fft, q->cell.cp, q->cell.nof_prb)) {
        ERROR("Error initiating FFT\n");
        return SRSLTE_ERROR;
      }

      if (srslte_pucch_set_cell(&q->pucch, q->cell)) {
        ERROR("Error creating PUCCH object\n");
        return SRSLTE_ERROR;
      }

      if (srslte_pusch_set_cell(&q->pusch, q->cell)) {
        ERROR("Error creating PUSCH object\n");
        return SRSLTE_ERROR;
      }

      if (srslte_chest_ul_set_cell(&q->chest, cell)) {
        ERROR("Error initiating channel estimator\n");
        return SRSLTE_ERROR;
      }

      // SRS is a dedicated configuration
      srslte_chest_ul_pregen(&q->chest, pusch_cfg);

      ret = SRSLTE_SUCCESS;
    }
  } else {
    ERROR("Invalid cell properties: Id=%d, Ports=%d, PRBs=%d\n", cell.id, cell.nof_ports, cell.nof_prb);
  }
  return ret;
}

int srslte_enb_ul_add_rnti(srslte_enb_ul_t *q, uint16_t rnti)
{
  if (srslte_pucch_set_rnti(&q->pucch, rnti)) {
    ERROR("Error setting PUCCH rnti\n");
    return -1;
  }
  if (srslte_pusch_set_rnti(&q->pusch, rnti)) {
    ERROR("Error setting PUSCH rnti\n");
    return -1;
  }
  return 0;
}

void srslte_enb_ul_rem_rnti(srslte_enb_ul_t* q, uint16_t rnti)
{
  srslte_pucch_free_rnti(&q->pucch, rnti);
  srslte_pusch_free_rnti(&q->pusch, rnti);
}

void srslte_enb_ul_fft(srslte_enb_ul_t *q)
{
  srslte_ofdm_rx_sf(&q->fft);
}

static int get_pucch(srslte_enb_ul_t* q, srslte_ul_sf_cfg_t* ul_sf, srslte_pucch_cfg_t* cfg, srslte_pucch_res_t* res)
{
  srslte_uci_value_t uci_value_default = {};
  srslte_ue_ul_pucch_resource_selection(&q->cell, cfg, &cfg->uci_cfg, &uci_value_default);

  // Prepare configuration
  if (srslte_chest_ul_estimate_pucch(&q->chest, ul_sf, cfg, q->sf_symbols, &q->chest_res)) {
    ERROR("Error estimating PUCCH DMRS\n");
    return SRSLTE_ERROR;
  }

  int ret_val = srslte_pucch_decode(&q->pucch, ul_sf, cfg, &q->chest_res, q->sf_symbols, res);
  if (ret_val < 0) {
    ERROR("Error decoding PUCCH\n");
    return SRSLTE_ERROR;
  }
  return ret_val;
}

uint32_t srslte_enb_ul_get_pucch_prb_idx(srslte_cell_t* cell, srslte_pucch_cfg_t* cfg, uint32_t ns)
{
  // compute Format and n_pucch
  srslte_ue_ul_pucch_resource_selection(cell, cfg, &cfg->uci_cfg, NULL);

  // compute prb_idx
  return srslte_pucch_n_prb(cell, cfg, ns);
}

int srslte_enb_ul_get_pucch(srslte_enb_ul_t*    q,
                            srslte_ul_sf_cfg_t* ul_sf,
                            srslte_pucch_cfg_t* cfg,
                            srslte_pucch_res_t* res)
{

  if (!srslte_pucch_cfg_isvalid(cfg, q->cell.nof_prb)) {
    ERROR("Invalid PUCCH configuration\n");
    return -1;
  }

  if (get_pucch(q, ul_sf, cfg, res)) {
    return -1;
  }

  // If we are looking for SR and ACK at the same time and ret=0, means there is no SR.
  // try again to decode ACK only
  if (cfg->uci_cfg.is_scheduling_request_tti && srslte_uci_cfg_total_ack(&cfg->uci_cfg) && !res->detected) {
    cfg->uci_cfg.is_scheduling_request_tti = false;
    if (get_pucch(q, ul_sf, cfg, res)) {
      return -1;
    }
  }

  return SRSLTE_SUCCESS;
}

int srslte_enb_ul_get_pusch(srslte_enb_ul_t*    q,
                            srslte_ul_sf_cfg_t* ul_sf,
                            srslte_pusch_cfg_t* cfg,
                            srslte_pusch_res_t* res)
{
  srslte_chest_ul_estimate_pusch(&q->chest, ul_sf, cfg, q->sf_symbols, &q->chest_res);

  return srslte_pusch_decode(&q->pusch, ul_sf, cfg, &q->chest_res, q->sf_symbols, res);
}
