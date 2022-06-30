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

#include "srsran/phy/gnb/gnb_ul.h"
#include "srsran/phy/ch_estimation/dmrs_pucch.h"

/**
 * @brief Shifts FFT window a fraction of the cyclic prefix. Set to 0.0f for disabling.
 * @note Increases protection against inter-symbol interference in case of synchronization error in expense of computing
 * performance
 */
#define GNB_UL_NR_FFT_WINDOW_OFFSET 0.5f

/**
 * @brief Minimum PUSCH DMRS measured SINR default value
 */
#define GNB_UL_PUSCH_MIN_SNR_DEFAULT -10.0f

static int gnb_ul_alloc_prb(srsran_gnb_ul_t* q, uint32_t new_nof_prb)
{
  if (q->max_prb < new_nof_prb) {
    q->max_prb = new_nof_prb;

    srsran_chest_dl_res_free(&q->chest_pusch);
    if (srsran_chest_dl_res_init(&q->chest_pusch, q->max_prb) < SRSRAN_SUCCESS) {
      return SRSRAN_ERROR;
    }

    srsran_chest_ul_res_free(&q->chest_pucch);
    if (srsran_chest_ul_res_init(&q->chest_pucch, q->max_prb) < SRSRAN_SUCCESS) {
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

  // Set PUSCH minimum SNR, use default value if the given is NAN, INF or zero
  q->pusch_min_snr_dB = GNB_UL_PUSCH_MIN_SNR_DEFAULT;
  if (isnormal(args->pusch_min_snr_dB)) {
    q->pusch_min_snr_dB = args->pusch_min_snr_dB;
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
  srsran_chest_dl_res_free(&q->chest_pusch);
  srsran_chest_ul_res_free(&q->chest_pucch);

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

  srsran_ofdm_cfg_t ofdm_cfg     = {};
  ofdm_cfg.nof_prb               = carrier->nof_prb;
  ofdm_cfg.rx_window_offset      = GNB_UL_NR_FFT_WINDOW_OFFSET;
  ofdm_cfg.symbol_sz             = srsran_min_symbol_sz_rb(carrier->nof_prb);
  ofdm_cfg.keep_dc               = true;
  ofdm_cfg.phase_compensation_hz = carrier->ul_center_frequency_hz;

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

  if (srsran_dmrs_sch_estimate(&q->dmrs, slot_cfg, cfg, grant, q->sf_symbols[0], &q->chest_pusch) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  // Check PUSCH DMRS minimum SNR and abort PUSCH decoding if it is below the threshold
  if (q->dmrs.csi.snr_dB < q->pusch_min_snr_dB) {
    // Set PUSCH data as not decoded
    data->tb[0].crc      = false;
    data->tb[0].avg_iter = NAN;
    data->uci.valid      = false;
    return SRSRAN_SUCCESS;
  }

  if (srsran_pusch_nr_decode(&q->pusch, cfg, grant, &q->chest_pusch, q->sf_symbols, data) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

static int gnb_ul_decode_pucch_format1(srsran_gnb_ul_t*                    q,
                                       const srsran_slot_cfg_t*            slot_cfg,
                                       const srsran_pucch_nr_common_cfg_t* cfg,
                                       const srsran_pucch_nr_resource_t*   resource,
                                       const srsran_uci_cfg_nr_t*          uci_cfg,
                                       srsran_uci_value_nr_t*              uci_value)
{
  uint8_t b[SRSRAN_PUCCH_NR_FORMAT1_MAX_NOF_BITS] = {};

  // Set ACK bits
  uint32_t nof_bits = SRSRAN_MIN(SRSRAN_PUCCH_NR_FORMAT1_MAX_NOF_BITS, uci_cfg->ack.count);

  // Set SR bits
  // For a positive SR transmission using PUCCH format 1, the UE transmits the PUCCH as described in [4, TS
  // 38.211] by setting b ( 0 ) = 0 .
  if (nof_bits == 0 && uci_cfg->o_sr > 0) {
    nof_bits = 1;
  }

  // Channel estimation
  if (srsran_dmrs_pucch_format1_estimate(&q->pucch, cfg, slot_cfg, resource, q->sf_symbols[0], &q->chest_pucch) <
      SRSRAN_SUCCESS) {
    ERROR("Error in PUCCH format 1 estimation");
    return SRSRAN_ERROR;
  }

  // Actual decode
  float norm_corr = 0.0f;
  if (srsran_pucch_nr_format1_decode(
          &q->pucch, cfg, slot_cfg, resource, &q->chest_pucch, q->sf_symbols[0], b, nof_bits, &norm_corr) <
      SRSRAN_SUCCESS) {
    ERROR("Error in PUCCH format 1 decoding");
    return SRSRAN_ERROR;
  }

  // As format 1 with positive SR is not encoded with any payload, set SR to 1
  if (uci_cfg->sr_positive_present) {
    uci_value->sr = 1;
  }

  // Take valid decision
  uci_value->valid = (norm_corr > 0.5f);

  // De-multiplex ACK bits
  for (uint32_t i = 0; i < nof_bits; i++) {
    uci_value->ack[i] = b[i];
  }

  return SRSRAN_SUCCESS;
}

static int gnb_ul_decode_pucch_format2(srsran_gnb_ul_t*                    q,
                                       const srsran_slot_cfg_t*            slot_cfg,
                                       const srsran_pucch_nr_common_cfg_t* cfg,
                                       const srsran_pucch_nr_resource_t*   resource,
                                       const srsran_uci_cfg_nr_t*          uci_cfg,
                                       srsran_uci_value_nr_t*              uci_value)
{
  if (srsran_dmrs_pucch_format2_estimate(&q->pucch, cfg, slot_cfg, resource, q->sf_symbols[0], &q->chest_pucch) <
      SRSRAN_SUCCESS) {
    ERROR("Error in PUCCH format 2 estimation");
    return SRSRAN_ERROR;
  }

  if (srsran_pucch_nr_format_2_3_4_decode(
          &q->pucch, cfg, slot_cfg, resource, uci_cfg, &q->chest_pucch, q->sf_symbols[0], uci_value) < SRSRAN_SUCCESS) {
    ERROR("Error in PUCCH format 2 decoding");
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

int srsran_gnb_ul_get_pucch(srsran_gnb_ul_t*                    q,
                            const srsran_slot_cfg_t*            slot_cfg,
                            const srsran_pucch_nr_common_cfg_t* cfg,
                            const srsran_pucch_nr_resource_t*   resource,
                            const srsran_uci_cfg_nr_t*          uci_cfg,
                            srsran_uci_value_nr_t*              uci_value,
                            srsran_csi_trs_measurements_t*      meas)
{
  if (q == NULL || slot_cfg == NULL || cfg == NULL || resource == NULL || uci_cfg == NULL || uci_value == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Estimate channel
  switch (resource->format) {
    case SRSRAN_PUCCH_NR_FORMAT_1:
      if (gnb_ul_decode_pucch_format1(q, slot_cfg, cfg, resource, uci_cfg, uci_value) < SRSRAN_SUCCESS) {
        return SRSRAN_ERROR;
      }
      break;
    case SRSRAN_PUCCH_NR_FORMAT_2:
      if (gnb_ul_decode_pucch_format2(q, slot_cfg, cfg, resource, uci_cfg, uci_value) < SRSRAN_SUCCESS) {
        return SRSRAN_ERROR;
      }
      break;
    case SRSRAN_PUCCH_NR_FORMAT_0:
    case SRSRAN_PUCCH_NR_FORMAT_3:
    case SRSRAN_PUCCH_NR_FORMAT_4:
    case SRSRAN_PUCCH_NR_FORMAT_ERROR:
      ERROR("Invalid or not implemented PUCCH-NR format %d", (int)resource->format);
      return SRSRAN_ERROR;
  }

  // Copy DMRS measurements
  if (meas != NULL) {
    meas->rsrp       = q->chest_pucch.rsrp;
    meas->rsrp_dB    = q->chest_pucch.rsrp_dBfs;
    meas->epre       = q->chest_pucch.epre;
    meas->epre_dB    = q->chest_pucch.epre_dBfs;
    meas->n0         = q->chest_pucch.noise_estimate;
    meas->n0_dB      = q->chest_pucch.noise_estimate_dbFs;
    meas->snr_dB     = q->chest_pucch.snr_db;
    meas->cfo_hz     = q->chest_pucch.cfo_hz;
    meas->cfo_hz_max = NAN; // Unavailable
    meas->delay_us   = q->chest_pucch.ta_us;
    meas->nof_re     = 0; // Unavailable
  }

  return SRSRAN_SUCCESS;
}

uint32_t srsran_gnb_ul_pucch_info(srsran_gnb_ul_t*                     q,
                                  const srsran_pucch_nr_resource_t*    resource,
                                  const srsran_uci_data_nr_t*          uci_data,
                                  const srsran_csi_trs_measurements_t* csi,
                                  char*                                str,
                                  uint32_t                             str_len)
{
  if (q == NULL || uci_data == NULL) {
    return 0;
  }

  uint32_t len = 0;

  len += srsran_pucch_nr_info(resource, uci_data, &str[len], str_len - len);

  len += srsran_csi_meas_info_short(csi, &str[len], str_len - len);

  len = srsran_print_check(str, str_len, len, "valid=%c ", uci_data->value.valid ? 'y' : 'n');

  return len;
}

uint32_t srsran_gnb_ul_pusch_info(srsran_gnb_ul_t*             q,
                                  const srsran_sch_cfg_nr_t*   cfg,
                                  const srsran_pusch_res_nr_t* res,
                                  char*                        str,
                                  uint32_t                     str_len)
{
  if (q == NULL || cfg == NULL || res == NULL) {
    return 0;
  }

  uint32_t len = 0;

  len += srsran_pusch_nr_rx_info(&q->pusch, cfg, &cfg->grant, res, str, str_len - len);

  // Append channel estimator info
  len += srsran_csi_meas_info_short(&q->dmrs.csi, &str[len], str_len - len);

  return len;
}
