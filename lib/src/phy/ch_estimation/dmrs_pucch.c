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

#include "srslte/phy/ch_estimation/dmrs_pucch.h"
#include "srslte/phy/common/sequence.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"

// Implements TS 38.211 table 6.4.1.3.1.1-1: Number of DM-RS symbols and the corresponding N_PUCCH...
static uint32_t dmrs_pucch_format1_n_pucch(const srslte_pucch_nr_resource_t* resource, uint32_t m_prime)
{
  if (resource->intra_slot_hopping) {
    if (m_prime == 0) {
      switch (resource->nof_symbols) {
        case 4:
        case 5:
          return 1;
        case 6:
        case 7:
        case 8:
        case 9:
          return 2;
        case 10:
        case 11:
        case 12:
        case 13:
          return 3;
        case 14:
          return 4;
        default:; // Do nothing
      }
    } else {
      switch (resource->nof_symbols) {
        case 4:
        case 6:
          return 1;
        case 5:
        case 7:
        case 8:
        case 10:
          return 2;
        case 9:
        case 11:
        case 12:
        case 14:
          return 3;
        case 13:
          return 4;
        default:; // Do nothing
      }
    }
  } else if (m_prime == 0) {
    switch (resource->nof_symbols) {
      case 4:
        return 2;
      case 5:
      case 6:
        return 3;
      case 7:
      case 8:
        return 4;
      case 9:
      case 10:
        return 5;
      case 11:
      case 12:
        return 6;
      case 13:
      case 14:
        return 7;
      default:; // Do nothing
    }
  }

  ERROR("Invalid case nof_symbols=%d and m_prime=%d\n", resource->nof_symbols, m_prime);
  return 0;
}

int srslte_dmrs_pucch_format1_put(const srslte_pucch_nr_t*            q,
                                  const srslte_carrier_nr_t*          carrier,
                                  const srslte_pucch_nr_common_cfg_t* cfg,
                                  const srslte_dl_slot_cfg_t*         slot,
                                  const srslte_pucch_nr_resource_t*   resource,
                                  cf_t*                               slot_symbols)
{

  if (q == NULL || carrier == NULL || cfg == NULL || slot == NULL || resource == NULL || slot_symbols == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  if (srslte_pucch_nr_cfg_resource_valid(resource) < SRSLTE_SUCCESS) {
    ERROR("Invalid PUCCH format 1 resource\n");
    return SRSLTE_ERROR;
  }

  // Get group sequence
  uint32_t u = 0;
  uint32_t v = 0;
  if (srslte_pucch_nr_group_sequence(carrier, cfg, &u, &v) < SRSLTE_SUCCESS) {
    ERROR("Error getting group sequence\n");
    return SRSLTE_ERROR;
  }

  uint32_t n_pucch = dmrs_pucch_format1_n_pucch(resource, 0);
  if (n_pucch == 0) {
    ERROR("Error getting number of symbols\n");
    return SRSLTE_ERROR;
  }

  uint32_t l_prime = resource->start_symbol_idx;
  for (uint32_t m = 0; m < n_pucch; m++) {
    // Clause 6.4.1.3.1.2 specifies l=0,2,4...
    uint32_t l = m * 2;

    // Get start of the sequence in resource grid
    cf_t* slot_symbols_ptr = &slot_symbols[(carrier->nof_prb * (l + l_prime) + resource->starting_prb) * SRSLTE_NRE];

    // Get Alpha index
    uint32_t alpha_idx = 0;
    if (srslte_pucch_nr_alpha_idx(carrier, cfg, slot, l, l_prime, resource->initial_cyclic_shift, 0, &alpha_idx) <
        SRSLTE_SUCCESS) {
      ERROR("Calculating alpha\n");
    }

    // get r_uv sequence from LUT object
    const cf_t* r_uv = srslte_zc_sequence_lut_get(&q->r_uv_1prb, u, v, alpha_idx);
    if (r_uv == NULL) {
      ERROR("Getting r_uv sequence\n");
      return SRSLTE_ERROR;
    }

    // Get w_i_m
    cf_t w_i_m = srslte_pucch_nr_format1_w(q, n_pucch, resource->time_domain_occ, m);

    // Compute z(n) = w(i) * r_uv(n)
    cf_t z[SRSLTE_NRE];
    srslte_vec_sc_prod_ccc(r_uv, w_i_m, z, SRSLTE_NRE);

    // Put z in the grid
    srslte_vec_cf_copy(slot_symbols_ptr, z, SRSLTE_NRE);
  }

  return SRSLTE_SUCCESS;
}

int srslte_dmrs_pucch_format1_estimate(const srslte_pucch_nr_t*            q,
                                       const srslte_carrier_nr_t*          carrier,
                                       const srslte_pucch_nr_common_cfg_t* cfg,
                                       const srslte_dl_slot_cfg_t*         slot,
                                       const srslte_pucch_nr_resource_t*   resource,
                                       const cf_t*                         slot_symbols,
                                       srslte_chest_ul_res_t*              res)
{

  if (q == NULL || carrier == NULL || cfg == NULL || slot == NULL || resource == NULL || slot_symbols == NULL ||
      res == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  if (srslte_pucch_nr_cfg_resource_valid(resource) < SRSLTE_SUCCESS) {
    ERROR("Invalid PUCCH format 1 resource\n");
    return SRSLTE_ERROR;
  }

  // Get group sequence
  uint32_t u = 0;
  uint32_t v = 0;
  if (srslte_pucch_nr_group_sequence(carrier, cfg, &u, &v) < SRSLTE_SUCCESS) {
    ERROR("Error getting group sequence\n");
    return SRSLTE_ERROR;
  }

  uint32_t n_pucch = dmrs_pucch_format1_n_pucch(resource, 0);
  if (n_pucch == 0) {
    ERROR("Error getting number of symbols\n");
    return SRSLTE_ERROR;
  }

  cf_t     ce[SRSLTE_PUCCH_NR_FORMAT1_N_MAX][SRSLTE_NRE];
  uint32_t l_prime = resource->start_symbol_idx;
  for (uint32_t m = 0; m < n_pucch; m++) {
    // Clause 6.4.1.3.1.2 specifies l=0,2,4...
    uint32_t l = m * 2;

    // Get start of the sequence in resource grid
    const cf_t* slot_symbols_ptr =
        &slot_symbols[(carrier->nof_prb * (l + l_prime) + resource->starting_prb) * SRSLTE_NRE];

    // Get Alpha index
    uint32_t alpha_idx = 0;
    if (srslte_pucch_nr_alpha_idx(carrier, cfg, slot, l, l_prime, resource->initial_cyclic_shift, 0, &alpha_idx) <
        SRSLTE_SUCCESS) {
      ERROR("Calculating alpha\n");
    }

    // get r_uv sequence from LUT object
    const cf_t* r_uv = srslte_zc_sequence_lut_get(&q->r_uv_1prb, u, v, alpha_idx);
    if (r_uv == NULL) {
      ERROR("Getting r_uv sequence\n");
      return SRSLTE_ERROR;
    }

    // Get w_i_m
    cf_t w_i_m = srslte_pucch_nr_format1_w(q, n_pucch, resource->time_domain_occ, m);

    // Compute z(n) = w(i) * r_uv(n)
    cf_t z[SRSLTE_NRE];
    srslte_vec_sc_prod_ccc(r_uv, w_i_m, z, SRSLTE_NRE);

    // Calculate least square estimates for this symbol
    srslte_vec_prod_conj_ccc(slot_symbols_ptr, z, ce[m], SRSLTE_NRE);
  }

  // Perform measurements
  float rsrp   = 0.0f;
  float epre   = 0.0f;
  float ta_err = 0.0f;
  for (uint32_t m = 0; m < n_pucch; m++) {
    cf_t corr = srslte_vec_acc_cc(ce[m], SRSLTE_NRE) / SRSLTE_NRE;
    rsrp += __real__ corr * __real__ corr + __imag__ corr * __imag__ corr;
    epre += srslte_vec_avg_power_cf(ce[m], SRSLTE_NRE);
    ta_err += srslte_vec_estimate_frequency(ce[m], SRSLTE_NRE);
  }

  // Average measurements
  rsrp /= n_pucch;
  epre /= n_pucch;
  ta_err /= n_pucch;

  // Set power measures
  rsrp                    = SRSLTE_MIN(rsrp, epre);
  res->rsrp               = rsrp;
  res->rsrp_dBfs          = srslte_convert_power_to_dB(rsrp);
  res->epre               = epre;
  res->epre_dBfs          = srslte_convert_power_to_dB(epre);
  res->noise_estimate     = epre - rsrp;
  res->noise_estimate_dbm = srslte_convert_power_to_dB(res->noise_estimate);
  res->snr                = rsrp / res->noise_estimate;
  res->snr_db             = srslte_convert_power_to_dB(res->snr);

  // Compute Time Aligment error in microseconds
  if (isnormal(ta_err)) {
    ta_err /= 15e3f * (float)(1U << carrier->numerology); // Convert from normalized frequency to seconds
    ta_err *= 1e6f;                                       // Convert to micro-seconds
    ta_err     = roundf(ta_err * 10.0f) / 10.0f;          // Round to one tenth of micro-second
    res->ta_us = ta_err;
  } else {
    res->ta_us = 0.0f;
  }

  // Measure CFO
  res->cfo = NAN; // Not implemented

  // Do averaging here
  // ... Not implemented

  // Interpolates between DMRS symbols
  for (uint32_t m = 0; m < n_pucch; m++) {
    uint32_t l      = m * 2 + 1;
    cf_t*    ce_ptr = &res->ce[(carrier->nof_prb * (l + l_prime) + resource->starting_prb) * SRSLTE_NRE];

    if (m != n_pucch - 1) {
      // If it is not the last symbol with DMRS, average between
      srslte_vec_sum_ccc(ce[m], ce[m + 1], ce_ptr, SRSLTE_NRE);
      srslte_vec_sc_prod_cfc(ce_ptr, 0.5f, ce_ptr, SRSLTE_NRE);
    } else if (m != 0) {
      // Extrapolate for the last if more than 1 are provided
      srslte_vec_sc_prod_cfc(ce[m], 3.0f, ce_ptr, SRSLTE_NRE);
      srslte_vec_sub_ccc(ce_ptr, ce[m - 1], ce_ptr, SRSLTE_NRE);
      srslte_vec_sc_prod_cfc(ce_ptr, 0.5f, ce_ptr, SRSLTE_NRE);
    } else {
      // Simply copy the
      srslte_vec_cf_copy(ce_ptr, ce[m], SRSLTE_NRE);
    }
  }

  return SRSLTE_SUCCESS;
}

static uint32_t dmrs_pucch_format2_cinit(const srslte_carrier_nr_t*          carrier,
                                         const srslte_pucch_nr_common_cfg_t* cfg,
                                         const srslte_dl_slot_cfg_t*         slot,
                                         uint32_t                            l)
{
  uint32_t n    = SRSLTE_SLOT_NR_MOD(slot->idx, carrier->numerology);
  uint32_t n_id = (cfg->scrambling_id_present) ? cfg->scambling_id : carrier->id;

  return SRSLTE_SEQUENCE_MOD((((SRSLTE_NSYMB_PER_SLOT_NR * n + l + 1U) * (2U * n_id + 1U)) << 17U) + 2U * n_id);
}

int srslte_dmrs_pucch_format2_put(const srslte_pucch_nr_t*            q,
                                  const srslte_carrier_nr_t*          carrier,
                                  const srslte_pucch_nr_common_cfg_t* cfg,
                                  const srslte_dl_slot_cfg_t*         slot,
                                  const srslte_pucch_nr_resource_t*   resource,
                                  cf_t*                               slot_symbols)
{
  if (q == NULL || carrier == NULL || cfg == NULL || slot == NULL || resource == NULL || slot_symbols == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  if (srslte_pucch_nr_cfg_resource_valid(resource) < SRSLTE_SUCCESS) {
    ERROR("Invalid PUCCH format 1 resource\n");
    return SRSLTE_ERROR;
  }

  uint32_t l_start = resource->start_symbol_idx;
  uint32_t l_end   = resource->start_symbol_idx + resource->nof_symbols;
  uint32_t k_start = SRSLTE_MIN(carrier->nof_prb - 1, resource->starting_prb) * SRSLTE_NRE + 1;
  uint32_t k_end   = SRSLTE_MIN(carrier->nof_prb, resource->starting_prb + resource->nof_prb) * SRSLTE_NRE;
  for (uint32_t l = l_start; l < l_end; l++) {
    // Compute sequence initial state
    uint32_t                cinit    = dmrs_pucch_format2_cinit(carrier, cfg, slot, l);
    srslte_sequence_state_t sequence = {};
    srslte_sequence_state_init(&sequence, cinit);

    // Skip PRBs to start
    srslte_sequence_state_advance(&sequence, 2 * 4 * resource->starting_prb);

    // Generate sequence
    cf_t r_l[SRSLTE_PUCCH_NR_FORMAT2_MAX_NPRB * 4];
    srslte_sequence_state_gen_f(&sequence, M_SQRT1_2, (float*)r_l, 2 * 4 * resource->nof_prb);

    // Put sequence in k = 3 * m + 1
    for (uint32_t k = k_start, i = 0; k < k_end; k += 3, i++) {
      slot_symbols[l * carrier->nof_prb * SRSLTE_NRE + k] = r_l[i];
    }
  }
  return SRSLTE_SUCCESS;
}

int srslte_dmrs_pucch_format2_estimate(const srslte_pucch_nr_t*            q,
                                       const srslte_carrier_nr_t*          carrier,
                                       const srslte_pucch_nr_common_cfg_t* cfg,
                                       const srslte_dl_slot_cfg_t*         slot,
                                       const srslte_pucch_nr_resource_t*   resource,
                                       const cf_t*                         slot_symbols,
                                       srslte_chest_ul_res_t*              res)
{
  if (q == NULL || carrier == NULL || cfg == NULL || slot == NULL || resource == NULL || slot_symbols == NULL ||
      res == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  if (srslte_pucch_nr_cfg_resource_valid(resource) < SRSLTE_SUCCESS) {
    ERROR("Invalid PUCCH format 1 resource\n");
    return SRSLTE_ERROR;
  }

  cf_t ce[SRSLTE_PUCCH_NR_FORMAT2_MAX_NSYMB][SRSLTE_PUCCH_NR_FORMAT2_MAX_NPRB * 4];

  uint32_t l_start = resource->start_symbol_idx;
  uint32_t l_end   = resource->start_symbol_idx + resource->nof_symbols;
  uint32_t k_start = SRSLTE_MIN(carrier->nof_prb - 1, resource->starting_prb) * SRSLTE_NRE + 1;
  uint32_t k_end   = SRSLTE_MIN(carrier->nof_prb, resource->starting_prb + resource->nof_prb) * SRSLTE_NRE;
  uint32_t nof_ref = 4 * resource->nof_prb;
  for (uint32_t l = l_start, j = 0; l < l_end; l++, j++) {
    // Compute sequence initial state
    uint32_t                cinit    = dmrs_pucch_format2_cinit(carrier, cfg, slot, l);
    srslte_sequence_state_t sequence = {};
    srslte_sequence_state_init(&sequence, cinit);

    // Skip PRBs to start
    srslte_sequence_state_advance(&sequence, 2 * 4 * resource->starting_prb);

    // Generate sequence
    cf_t r_l[SRSLTE_PUCCH_NR_FORMAT2_MAX_NPRB * 4];
    srslte_sequence_state_gen_f(&sequence, M_SQRT1_2, (float*)r_l, 2 * nof_ref);

    // Put sequence in k = 3 * m + 1
    for (uint32_t k = k_start, i = 0; k < k_end; k += 3, i++) {
      ce[j][i] = slot_symbols[l * carrier->nof_prb * SRSLTE_NRE + k];
    }

    srslte_vec_prod_conj_ccc(ce[j], r_l, ce[j], nof_ref);
  }

  // Perform measurements
  float epre   = 0.0f;
  float rsrp   = 0.0f;
  float ta_err = 0.0f;
  for (uint32_t i = 0; i < resource->nof_symbols; i++) {
    cf_t corr = srslte_vec_acc_cc(ce[i], nof_ref) / nof_ref;
    rsrp += __real__ corr * __real__ corr + __imag__ corr * __imag__ corr;
    epre += srslte_vec_avg_power_cf(ce[i], nof_ref);
    ta_err += srslte_vec_estimate_frequency(ce[i], nof_ref);
  }
  epre /= resource->nof_symbols;
  rsrp /= resource->nof_symbols;
  ta_err /= resource->nof_symbols;

  // Set power measures
  rsrp                    = SRSLTE_MIN(rsrp, epre);
  res->rsrp               = rsrp;
  res->rsrp_dBfs          = srslte_convert_power_to_dB(rsrp);
  res->epre               = epre;
  res->epre_dBfs          = srslte_convert_power_to_dB(epre);
  res->noise_estimate     = epre - rsrp;
  res->noise_estimate_dbm = srslte_convert_power_to_dB(res->noise_estimate);
  res->snr                = rsrp / res->noise_estimate;
  res->snr_db             = srslte_convert_power_to_dB(res->snr);

  // Compute Time Aligment error in microseconds
  if (isnormal(ta_err)) {
    ta_err /= 15e3f * (float)(1U << carrier->numerology) * 3; // Convert from normalized frequency to seconds
    ta_err *= 1e6f;                                           // Convert to micro-seconds
    ta_err     = roundf(ta_err * 10.0f) / 10.0f;              // Round to one tenth of micro-second
    res->ta_us = ta_err;
  } else {
    res->ta_us = 0.0f;
  }

  // Perform averaging
  // ...

  // Zero order hold
  for (uint32_t l = l_start, j = 0; l < l_end; l++, j++) {
    for (uint32_t k = k_start - 1, i = 0; k < k_end; k++, i++) {
      res->ce[l * carrier->nof_prb * SRSLTE_NRE + k] = ce[j][i / 3];
    }
  }

  return SRSLTE_SUCCESS;
}

int srslte_dmrs_pucch_format_3_4_get_symbol_idx(const srslte_pucch_nr_resource_t* resource,
                                                uint32_t idx[SRSLTE_DMRS_PUCCH_FORMAT_3_4_MAX_NSYMB])
{
  if (resource == NULL || idx == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  int count = 0;

  switch (resource->nof_symbols) {
    case 4:
      if (resource->intra_slot_hopping) {
        idx[count++] = 0;
        idx[count++] = 2;
      } else {
        idx[count++] = 1;
      }
      break;
    case 5:
      idx[count++] = 0;
      idx[count++] = 3;
      break;
    case 6:
    case 7:
      idx[count++] = 1;
      idx[count++] = 4;
      break;
    case 8:
      idx[count++] = 1;
      idx[count++] = 5;
      break;
    case 9:
      idx[count++] = 1;
      idx[count++] = 6;
      break;
    case 10:
      if (resource->additional_dmrs) {
        idx[count++] = 1;
        idx[count++] = 3;
        idx[count++] = 6;
        idx[count++] = 8;
      } else {
        idx[count++] = 2;
        idx[count++] = 7;
      }
      break;
    case 11:
      if (resource->additional_dmrs) {
        idx[count++] = 1;
        idx[count++] = 3;
        idx[count++] = 6;
        idx[count++] = 9;
      } else {
        idx[count++] = 2;
        idx[count++] = 7;
      }
      break;
    case 12:
      if (resource->additional_dmrs) {
        idx[count++] = 1;
        idx[count++] = 4;
        idx[count++] = 7;
        idx[count++] = 10;
      } else {
        idx[count++] = 2;
        idx[count++] = 8;
      }
      break;
    case 13:
      if (resource->additional_dmrs) {
        idx[count++] = 1;
        idx[count++] = 4;
        idx[count++] = 7;
        idx[count++] = 11;
      } else {
        idx[count++] = 2;
        idx[count++] = 9;
      }
      break;
    case 14:
      if (resource->additional_dmrs) {
        idx[count++] = 1;
        idx[count++] = 5;
        idx[count++] = 8;
        idx[count++] = 12;
      } else {
        idx[count++] = 3;
        idx[count++] = 10;
      }
      break;
    default:
      ERROR("Invalid case (%d)\n", resource->nof_symbols);
      return SRSLTE_ERROR;
  }

  return count;
}
