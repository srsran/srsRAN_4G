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

#include "srslte/phy/enb/enb_ul.h"

#include "srslte/srslte.h"
#include <complex.h>
#include <math.h>
#include <string.h>

int srslte_enb_ul_init(srslte_enb_ul_t* q, cf_t* in_buffer, uint32_t max_prb)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q != NULL) {
    ret = SRSLTE_ERROR;

    bzero(q, sizeof(srslte_enb_ul_t));

    q->sf_symbols = srslte_vec_cf_malloc(SRSLTE_SF_LEN_RE(max_prb, SRSLTE_CP_NORM));
    if (!q->sf_symbols) {
      perror("malloc");
      goto clean_exit;
    }

    q->chest_res.ce = srslte_vec_cf_malloc(SRSLTE_SF_LEN_RE(max_prb, SRSLTE_CP_NORM));
    if (!q->chest_res.ce) {
      perror("malloc");
      goto clean_exit;
    }

    srslte_ofdm_cfg_t ofdm_cfg = {};
    ofdm_cfg.nof_prb           = max_prb;
    ofdm_cfg.in_buffer         = in_buffer;
    ofdm_cfg.out_buffer        = q->sf_symbols;
    ofdm_cfg.cp                = SRSLTE_CP_NORM;
    ofdm_cfg.freq_shift_f      = -0.5f;
    ofdm_cfg.normalize         = false;
    ofdm_cfg.rx_window_offset  = 0.5f;
    if (srslte_ofdm_rx_init_cfg(&q->fft, &ofdm_cfg)) {
      ERROR("Error initiating FFT\n");
      goto clean_exit;
    }

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

void srslte_enb_ul_free(srslte_enb_ul_t* q)
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

int srslte_enb_ul_add_rnti(srslte_enb_ul_t* q, uint16_t rnti)
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

void srslte_enb_ul_fft(srslte_enb_ul_t* q)
{
  srslte_ofdm_rx_sf(&q->fft);
}

static int get_pucch(srslte_enb_ul_t* q, srslte_ul_sf_cfg_t* ul_sf, srslte_pucch_cfg_t* cfg, srslte_pucch_res_t* res)
{
  int                ret                               = SRSLTE_SUCCESS;
  uint32_t           n_pucch_i[SRSLTE_PUCCH_MAX_ALLOC] = {};
  srslte_pucch_res_t pucch_res                         = {};
  uint32_t           uci_cfg_total_ack                 = srslte_uci_cfg_total_ack(&cfg->uci_cfg);

  // Drop CQI if there is collision with ACK
  if (!cfg->simul_cqi_ack && uci_cfg_total_ack > 0 && cfg->uci_cfg.cqi.data_enable) {
    cfg->uci_cfg.cqi.data_enable = false;
  }

  // Select format
  cfg->format = srslte_pucch_proc_select_format(&q->cell, cfg, &cfg->uci_cfg, NULL);
  if (cfg->format == SRSLTE_PUCCH_FORMAT_ERROR) {
    ERROR("Returned Error while selecting PUCCH format\n");
    return SRSLTE_ERROR;
  }

  // Get possible resources
  int nof_resources = srslte_pucch_proc_get_resources(&q->cell, cfg, &cfg->uci_cfg, NULL, n_pucch_i);
  if (nof_resources < 1 || nof_resources > SRSLTE_PUCCH_CS_MAX_ACK) {
    ERROR("No PUCCH resource could be calculated (%d)\n", nof_resources);
    return SRSLTE_ERROR;
  }

  // Initialise minimum correlation
  res->correlation = 0.0f;

  // Iterate possible resources and select the one with higher correlation
  for (int i = 0; i < nof_resources && ret == SRSLTE_SUCCESS; i++) {
    // Configure resource
    cfg->n_pucch = n_pucch_i[i];

    // Prepare configuration
    if (srslte_chest_ul_estimate_pucch(&q->chest, ul_sf, cfg, q->sf_symbols, &q->chest_res)) {
      ERROR("Error estimating PUCCH DMRS\n");
      return SRSLTE_ERROR;
    }

    ret = srslte_pucch_decode(&q->pucch, ul_sf, cfg, &q->chest_res, q->sf_symbols, &pucch_res);
    if (ret < SRSLTE_SUCCESS) {
      ERROR("Error decoding PUCCH\n");
    } else {

      // Get PUCCH Format 1b with channel selection if used
      if (uci_cfg_total_ack > 0 && cfg->format == SRSLTE_PUCCH_FORMAT_1B &&
          cfg->ack_nack_feedback_mode == SRSLTE_PUCCH_ACK_NACK_FEEDBACK_MODE_CS) {
        uint8_t b[2] = {pucch_res.uci_data.ack.ack_value[0], pucch_res.uci_data.ack.ack_value[1]};
        srslte_pucch_cs_get_ack(cfg, &cfg->uci_cfg, i, b, &pucch_res.uci_data);
      }

      // Check correlation value, keep maximum
      if (i == 0 || pucch_res.correlation > res->correlation) {
        *res = pucch_res;
      }
    }
  }

  return ret;
}

int srslte_enb_ul_get_pucch(srslte_enb_ul_t*    q,
                            srslte_ul_sf_cfg_t* ul_sf,
                            srslte_pucch_cfg_t* cfg,
                            srslte_pucch_res_t* res)
{

  if (!srslte_pucch_cfg_isvalid(cfg, q->cell.nof_prb)) {
    ERROR("Invalid PUCCH configuration\n");
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  if (get_pucch(q, ul_sf, cfg, res)) {
    return SRSLTE_ERROR;
  }

  // If we are looking for SR and ACK at the same time and ret=0, means there is no SR.
  // try again to decode ACK only
  if (cfg->uci_cfg.is_scheduling_request_tti && srslte_uci_cfg_total_ack(&cfg->uci_cfg) && !res->detected) {
    cfg->uci_cfg.is_scheduling_request_tti = false;
    if (get_pucch(q, ul_sf, cfg, res)) {
      return SRSLTE_ERROR;
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
