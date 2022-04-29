/**
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */
#include "srsran/phy/ue/ue_ul_nr.h"
#include "srsran/phy/ch_estimation/dmrs_pucch.h"
#include "srsran/phy/utils/debug.h"
#include <complex.h>

int srsran_ue_ul_nr_init(srsran_ue_ul_nr_t* q, cf_t* output, const srsran_ue_ul_nr_args_t* args)
{
  if (q == NULL || output == NULL || args == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Initialise memory
  SRSRAN_MEM_ZERO(q, srsran_ue_ul_nr_t, 1);

  q->max_prb = args->nof_max_prb;

  q->sf_symbols[0] = srsran_vec_cf_malloc(SRSRAN_SLOT_LEN_RE_NR(q->max_prb));
  if (q->sf_symbols[0] == NULL) {
    ERROR("Malloc");
    return SRSRAN_ERROR;
  }

  if (srsran_pusch_nr_init_ue(&q->pusch, &args->pusch) < SRSRAN_SUCCESS) {
    ERROR("Initiating PUSCH");
    return SRSRAN_ERROR;
  }

  srsran_ofdm_cfg_t fft_cfg = {};
  fft_cfg.nof_prb           = args->nof_max_prb;
  fft_cfg.symbol_sz         = srsran_min_symbol_sz_rb(args->nof_max_prb);
  fft_cfg.keep_dc           = true;
  fft_cfg.in_buffer         = q->sf_symbols[0];
  fft_cfg.out_buffer        = output;
  if (srsran_ofdm_tx_init_cfg(&q->ifft, &fft_cfg) < SRSRAN_SUCCESS) {
    ERROR("Initiating OFDM");
    return SRSRAN_ERROR;
  }

  if (srsran_dmrs_sch_init(&q->dmrs, false) < SRSRAN_SUCCESS) {
    ERROR("Error DMRS");
    return SRSRAN_ERROR;
  }

  if (srsran_pucch_nr_init(&q->pucch, &args->pucch) < SRSRAN_SUCCESS) {
    ERROR("Error UCI\n");
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

int srsran_ue_ul_nr_set_carrier(srsran_ue_ul_nr_t* q, const srsran_carrier_nr_t* carrier)
{
  if (q == NULL || carrier == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  q->carrier = *carrier;

  srsran_ofdm_cfg_t fft_cfg     = {};
  fft_cfg.nof_prb               = carrier->nof_prb;
  fft_cfg.symbol_sz             = srsran_min_symbol_sz_rb(carrier->nof_prb);
  fft_cfg.keep_dc               = true;
  fft_cfg.phase_compensation_hz = carrier->ul_center_frequency_hz;
  if (srsran_ofdm_tx_init_cfg(&q->ifft, &fft_cfg) < SRSRAN_SUCCESS) {
    ERROR("Initiating OFDM");
    return SRSRAN_ERROR;
  }

  if (srsran_pusch_nr_set_carrier(&q->pusch, carrier) < SRSRAN_SUCCESS) {
    ERROR("Setting PUSCH carrier");
    return SRSRAN_ERROR;
  }

  if (srsran_pucch_nr_set_carrier(&q->pucch, carrier) < SRSRAN_SUCCESS) {
    ERROR("Setting PUSCH carrier");
    return SRSRAN_ERROR;
  }

  if (srsran_dmrs_sch_set_carrier(&q->dmrs, carrier)) {
    ERROR("Setting DMRS carrier");
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

void srsran_ue_ul_nr_set_freq_offset(srsran_ue_ul_nr_t* q, float freq_offset_hz)
{
  if (q == NULL) {
    return;
  }

  q->freq_offset_hz = -freq_offset_hz;
}

int srsran_ue_ul_nr_encode_pusch(srsran_ue_ul_nr_t*            q,
                                 const srsran_slot_cfg_t*      slot_cfg,
                                 const srsran_sch_cfg_nr_t*    pusch_cfg,
                                 const srsran_pusch_data_nr_t* data)
{
  // Check inputs
  if (q == NULL || pusch_cfg == NULL || data == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Fill with zeros the whole resource grid
  srsran_vec_cf_zero(q->sf_symbols[0], SRSRAN_SLOT_LEN_RE_NR(q->carrier.nof_prb));

  // Encode PUSCH
  if (srsran_pusch_nr_encode(&q->pusch, pusch_cfg, &pusch_cfg->grant, data, q->sf_symbols) < SRSRAN_SUCCESS) {
    ERROR("Encoding PUSCH");
    return SRSRAN_ERROR;
  }

  // Put DMRS
  if (srsran_dmrs_sch_put_sf(&q->dmrs, slot_cfg, pusch_cfg, &pusch_cfg->grant, q->sf_symbols[0])) {
    ERROR("Putting DMRS");
    return SRSRAN_ERROR;
  }

  // Generate signal
  srsran_ofdm_tx_sf(&q->ifft);

  // Normalise to peak
  uint32_t max_idx  = srsran_vec_max_abs_ci(q->ifft.cfg.out_buffer, q->ifft.sf_sz);
  float    max_peak = cabsf(q->ifft.cfg.out_buffer[max_idx]);
  if (isnormal(max_peak)) {
    srsran_vec_sc_prod_cfc(q->ifft.cfg.out_buffer, 0.99f / max_peak, q->ifft.cfg.out_buffer, q->ifft.sf_sz);
  }

  // Apply frequency offset
  if (isnormal(q->freq_offset_hz)) {
    srsran_vec_apply_cfo(
        q->ifft.cfg.out_buffer, -q->freq_offset_hz / (1000.0f * q->ifft.sf_sz), q->ifft.cfg.out_buffer, q->ifft.sf_sz);
  }

  return SRSRAN_SUCCESS;
}

static int ue_ul_nr_encode_pucch_format0(srsran_ue_ul_nr_t*                q,
                                         const srsran_pucch_nr_resource_t* resource,
                                         const srsran_uci_data_nr_t*       uci_data)
{
  ERROR("Not implemented");
  return SRSRAN_ERROR;
}

static int ue_ul_nr_encode_pucch_format1(srsran_ue_ul_nr_t*                  q,
                                         const srsran_slot_cfg_t*            slot,
                                         const srsran_pucch_nr_common_cfg_t* cfg,
                                         const srsran_pucch_nr_resource_t*   resource,
                                         const srsran_uci_data_nr_t*         uci_data)
{
  uint8_t b[SRSRAN_PUCCH_NR_FORMAT1_MAX_NOF_BITS] = {};

  // Set ACK bits
  uint32_t nof_bits = SRSRAN_MIN(SRSRAN_PUCCH_NR_FORMAT1_MAX_NOF_BITS, uci_data->cfg.ack.count);
  for (uint32_t i = 0; i < nof_bits; i++) {
    b[i] = uci_data->value.ack[i];
  }

  // Set SR bits
  // For a positive SR transmission using PUCCH format 1, the UE transmits the PUCCH as described in [4, TS
  // 38.211] by setting b ( 0 ) = 0 .
  if (nof_bits == 0 && uci_data->cfg.o_sr > 0 && uci_data->value.sr > 0) {
    b[0]     = 0;
    nof_bits = 1;
  }

  if (srsran_dmrs_pucch_format1_put(&q->pucch, &q->carrier, cfg, slot, resource, q->sf_symbols[0])) {
    return SRSRAN_ERROR;
  }

  return srsran_pucch_nr_format1_encode(&q->pucch, cfg, slot, resource, b, nof_bits, q->sf_symbols[0]);
}

int srsran_ue_ul_nr_encode_pucch(srsran_ue_ul_nr_t*                  q,
                                 const srsran_slot_cfg_t*            slot_cfg,
                                 const srsran_pucch_nr_common_cfg_t* cfg,
                                 const srsran_pucch_nr_resource_t*   resource,
                                 const srsran_uci_data_nr_t*         uci_data)
{
  // Check inputs
  if (q == NULL || slot_cfg == NULL || resource == NULL || uci_data == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Fill with zeros the whole resource grid
  srsran_vec_cf_zero(q->sf_symbols[0], SRSRAN_SLOT_LEN_RE_NR(q->carrier.nof_prb));

  // Actual PUCCH encoding
  switch (resource->format) {
    case SRSRAN_PUCCH_NR_FORMAT_0:
      if (ue_ul_nr_encode_pucch_format0(q, resource, uci_data) < SRSRAN_SUCCESS) {
        return SRSRAN_ERROR;
      }
      break;
    case SRSRAN_PUCCH_NR_FORMAT_1:
      if (ue_ul_nr_encode_pucch_format1(q, slot_cfg, cfg, resource, uci_data) < SRSRAN_SUCCESS) {
        return SRSRAN_ERROR;
      }
      break;
    case SRSRAN_PUCCH_NR_FORMAT_2:
      if (srsran_dmrs_pucch_format2_put(&q->pucch, &q->carrier, cfg, slot_cfg, resource, q->sf_symbols[0])) {
        return SRSRAN_ERROR;
      }
      if (srsran_pucch_nr_format_2_3_4_encode(
              &q->pucch, cfg, slot_cfg, resource, &uci_data->cfg, &uci_data->value, q->sf_symbols[0]) <
          SRSRAN_SUCCESS) {
        return SRSRAN_ERROR;
      }
      break;
    case SRSRAN_PUCCH_NR_FORMAT_3:
    case SRSRAN_PUCCH_NR_FORMAT_4:
      ERROR("PUCCH format %d NOT implemented", (int)resource->format);
      break;
    case SRSRAN_PUCCH_NR_FORMAT_ERROR:
    default:
      ERROR("Invalid case");
      break;
  }

  // Generate signal
  srsran_ofdm_tx_sf(&q->ifft);

  // Normalise to peak
  uint32_t max_idx  = srsran_vec_max_abs_ci(q->ifft.cfg.out_buffer, q->ifft.sf_sz);
  float    max_peak = cabsf(q->ifft.cfg.out_buffer[max_idx]);
  if (isnormal(max_peak)) {
    srsran_vec_sc_prod_cfc(q->ifft.cfg.out_buffer, 0.99f / max_peak, q->ifft.cfg.out_buffer, q->ifft.sf_sz);
  }

  // Apply frequency offset
  if (isnormal(q->freq_offset_hz)) {
    srsran_vec_apply_cfo(
        q->ifft.cfg.out_buffer, -q->freq_offset_hz / (1000.0f * q->ifft.sf_sz), q->ifft.cfg.out_buffer, q->ifft.sf_sz);
  }

  return SRSRAN_SUCCESS;
}

void srsran_ue_ul_nr_free(srsran_ue_ul_nr_t* q)
{
  if (q == NULL) {
    return;
  }
  srsran_ofdm_tx_free(&q->ifft);
  if (q->sf_symbols[0] != NULL) {
    free(q->sf_symbols[0]);
  }
  srsran_pucch_nr_free(&q->pucch);
  srsran_pusch_nr_free(&q->pusch);
  srsran_dmrs_sch_free(&q->dmrs);

  SRSRAN_MEM_ZERO(q, srsran_ue_ul_nr_t, 1);
}

int srsran_ue_ul_nr_pusch_info(const srsran_ue_ul_nr_t*     q,
                               const srsran_sch_cfg_nr_t*   cfg,
                               const srsran_uci_value_nr_t* uci_value,
                               char*                        str,
                               uint32_t                     str_len)
{
  int len = 0;

  // Append PDSCH info
  len += srsran_pusch_nr_tx_info(&q->pusch, cfg, &cfg->grant, uci_value, &str[len], str_len - len);

  len = srsran_print_check(str, str_len, len, " cfo=%.0f", q->freq_offset_hz);

  return len;
}

int srsran_ue_ul_nr_pucch_info(const srsran_pucch_nr_resource_t* resource,
                               const srsran_uci_data_nr_t*       uci_data,
                               char*                             str,
                               uint32_t                          str_len)
{
  int len = 0;

  // Append PDSCH info
  len += srsran_pucch_nr_info(resource, uci_data, &str[len], str_len - len);

  return len;
}

int srsran_ue_ul_nr_sr_send_slot(const srsran_pucch_nr_sr_resource_t sr_resources[SRSRAN_PUCCH_MAX_NOF_SR_RESOURCES],
                                 uint32_t                            slot_idx,
                                 uint32_t                            sr_resource_id[SRSRAN_PUCCH_MAX_NOF_SR_RESOURCES])
{
  int count = 0;

  // Check inputs
  if (sr_resources == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Iterate over all SR resources
  for (uint32_t i = 0; i < SRSRAN_PUCCH_MAX_NOF_SR_RESOURCES; i++) {
    const srsran_pucch_nr_sr_resource_t* res = &sr_resources[i];

    // Skip if resource is not provided
    if (!res->configured) {
      continue;
    }

    // Check periodicity and offset condition
    if ((slot_idx + res->period - res->offset) % res->period == 0) {
      if (sr_resource_id != NULL) {
        sr_resource_id[count] = i;
      }
      count++;
    }
  }

  // If the program reached this point is because there is no SR transmission opportunity
  return count;
}