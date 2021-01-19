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

  if (srslte_pucch_nr_format1_resource_valid(resource) < SRSLTE_SUCCESS) {
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

  if (q == NULL || carrier == NULL || cfg == NULL || slot == NULL || resource == NULL || slot_symbols == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  if (srslte_pucch_nr_format1_resource_valid(resource) < SRSLTE_SUCCESS) {
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
    cf_t corr = srslte_vec_acc_cc(ce[m], SRSLTE_NRE);
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
  res->noise_estimate     = epre - rsrp;
  res->noise_estimate_dbm = srslte_convert_power_to_dB(res->noise_estimate);
  res->snr                = rsrp / res->noise_estimate;
  res->snr_db             = srslte_convert_power_to_dB(res->snr_db);

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
