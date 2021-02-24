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
#include "srslte/phy/ue/ue_ul_nr.h"
#include "srslte/phy/ch_estimation/dmrs_pucch.h"
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
    ERROR("Error DMRS");
    return SRSLTE_ERROR;
  }

  if (srslte_pucch_nr_init(&q->pucch, &args->pucch) < SRSLTE_SUCCESS) {
    ERROR("Error UCI\n");
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

  // Fill with zeros the whole resource grid
  srslte_vec_cf_zero(q->sf_symbols[0], SRSLTE_SLOT_LEN_RE_NR(q->carrier.nof_prb));

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

  // Generate signal
  srslte_ofdm_tx_sf(&q->ifft);

  return SRSLTE_SUCCESS;
}

static int ue_ul_nr_encode_pucch_format0(srslte_ue_ul_nr_t*                q,
                                         const srslte_pucch_nr_resource_t* resource,
                                         const srslte_uci_data_nr_t*       uci_data)
{
  ERROR("Not implemented");
  return SRSLTE_ERROR;
}

static int ue_ul_nr_encode_pucch_format1(srslte_ue_ul_nr_t*                  q,
                                         const srslte_slot_cfg_t*            slot,
                                         const srslte_pucch_nr_common_cfg_t* cfg,
                                         const srslte_pucch_nr_resource_t*   resource,
                                         const srslte_uci_data_nr_t*         uci_data)
{
  uint8_t b[SRSLTE_PUCCH_NR_FORMAT1_MAX_NOF_BITS] = {};
  b[0]                                            = uci_data->value.ack[0];
  uint32_t nof_bits                               = 1;

  if (srslte_dmrs_pucch_format1_put(&q->pucch, &q->carrier, cfg, slot, resource, q->sf_symbols[0])) {
    return SRSLTE_ERROR;
  }

  return srslte_pucch_nr_format1_encode(&q->pucch, &q->carrier, cfg, slot, resource, b, nof_bits, q->sf_symbols[0]);
}

int srslte_ue_ul_nr_encode_pucch(srslte_ue_ul_nr_t*                  q,
                                 const srslte_slot_cfg_t*            slot_cfg,
                                 const srslte_pucch_nr_common_cfg_t* cfg,
                                 const srslte_pucch_nr_resource_t*   resource,
                                 const srslte_uci_data_nr_t*         uci_data)
{
  // Check inputs
  if (q == NULL || slot_cfg == NULL || resource == NULL || uci_data == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  // Fill with zeros the whole resource grid
  srslte_vec_cf_zero(q->sf_symbols[0], SRSLTE_SLOT_LEN_RE_NR(q->carrier.nof_prb));

  // Actual PUCCH encoding
  switch (resource->format) {
    case SRSLTE_PUCCH_NR_FORMAT_0:
      if (ue_ul_nr_encode_pucch_format0(q, resource, uci_data) < SRSLTE_SUCCESS) {
        return SRSLTE_ERROR;
      }
      break;
    case SRSLTE_PUCCH_NR_FORMAT_1:
      if (ue_ul_nr_encode_pucch_format1(q, slot_cfg, cfg, resource, uci_data) < SRSLTE_SUCCESS) {
        return SRSLTE_ERROR;
      }
      break;
    case SRSLTE_PUCCH_NR_FORMAT_2:
      if (srslte_dmrs_pucch_format2_put(&q->pucch, &q->carrier, cfg, slot_cfg, resource, q->sf_symbols[0])) {
        return SRSLTE_ERROR;
      }
      if (srslte_pucch_nr_format_2_3_4_encode(
              &q->pucch, &q->carrier, cfg, slot_cfg, resource, &uci_data->cfg, &uci_data->value, q->sf_symbols[0]) <
          SRSLTE_SUCCESS) {
        return SRSLTE_ERROR;
      }
      break;
    case SRSLTE_PUCCH_NR_FORMAT_3:
    case SRSLTE_PUCCH_NR_FORMAT_4:
      ERROR("PUCCH format %d NOT implemented", (int)resource->format);
      break;
    case SRSLTE_PUCCH_NR_FORMAT_ERROR:
    default:
      ERROR("Invalid case");
      break;
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

  SRSLTE_MEM_ZERO(q, srslte_ue_ul_nr_t, 1);
}

int srslte_ue_ul_nr_pusch_info(const srslte_ue_ul_nr_t* q, const srslte_sch_cfg_nr_t* cfg, char* str, uint32_t str_len)
{
  int len = 0;

  // Append PDSCH info
  len += srslte_pusch_nr_tx_info(&q->pusch, cfg, &cfg->grant, &str[len], str_len - len);

  return len;
}

int srslte_ue_ul_nr_pucch_info(const srslte_pucch_nr_resource_t* resource,
                               const srslte_uci_data_nr_t*       uci_data,
                               char*                             str,
                               uint32_t                          str_len)
{
  int len = 0;

  // Append PDSCH info
  len += srslte_pucch_nr_tx_info(resource, uci_data, &str[len], str_len - len);

  return len;
}
