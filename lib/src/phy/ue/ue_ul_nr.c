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

  if (srslte_pucch_nr_set_carrier(&q->pucch, carrier) < SRSLTE_SUCCESS) {
    ERROR("Setting PUSCH carrier");
    return SRSLTE_ERROR;
  }

  if (srslte_dmrs_sch_set_carrier(&q->dmrs, carrier)) {
    ERROR("Setting DMRS carrier");
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

int srslte_ue_ul_nr_encode_pusch(srslte_ue_ul_nr_t*            q,
                                 const srslte_slot_cfg_t*      slot_cfg,
                                 const srslte_sch_cfg_nr_t*    pusch_cfg,
                                 const srslte_pusch_data_nr_t* data)
{
  // Check inputs
  if (q == NULL || pusch_cfg == NULL || data == NULL) {
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

  // Set ACK bits
  uint32_t nof_bits = SRSLTE_MIN(SRSLTE_PUCCH_NR_FORMAT1_MAX_NOF_BITS, uci_data->cfg.o_ack);
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

  if (srslte_dmrs_pucch_format1_put(&q->pucch, &q->carrier, cfg, slot, resource, q->sf_symbols[0])) {
    return SRSLTE_ERROR;
  }

  return srslte_pucch_nr_format1_encode(&q->pucch, cfg, slot, resource, b, nof_bits, q->sf_symbols[0]);
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
              &q->pucch, cfg, slot_cfg, resource, &uci_data->cfg, &uci_data->value, q->sf_symbols[0]) <
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

int srslte_ue_ul_nr_sr_send_slot(const srslte_pucch_nr_sr_resource_t sr_resources[SRSLTE_PUCCH_MAX_NOF_SR_RESOURCES],
                                 uint32_t                            slot_idx,
                                 uint32_t                            sr_resource_id[SRSLTE_PUCCH_MAX_NOF_SR_RESOURCES])
{
  int count = 0;

  // Check inputs
  if (sr_resources == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  // Iterate over all SR resources
  for (uint32_t i = 0; i < SRSLTE_PUCCH_MAX_NOF_SR_RESOURCES; i++) {
    const srslte_pucch_nr_sr_resource_t* res = &sr_resources[i];

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