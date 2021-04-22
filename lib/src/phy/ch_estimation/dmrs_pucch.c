/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#include "srsran/phy/ch_estimation/dmrs_pucch.h"
#include "srsran/phy/common/sequence.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/vector.h"

// Implements TS 38.211 table 6.4.1.3.1.1-1: Number of DM-RS symbols and the corresponding N_PUCCH...
static uint32_t dmrs_pucch_format1_n_pucch(const srsran_pucch_nr_resource_t* resource, uint32_t m_prime)
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

  ERROR("Invalid case nof_symbols=%d and m_prime=%d", resource->nof_symbols, m_prime);
  return 0;
}

int srsran_dmrs_pucch_format1_put(const srsran_pucch_nr_t*            q,
                                  const srsran_carrier_nr_t*          carrier,
                                  const srsran_pucch_nr_common_cfg_t* cfg,
                                  const srsran_slot_cfg_t*            slot,
                                  const srsran_pucch_nr_resource_t*   resource,
                                  cf_t*                               slot_symbols)
{
  if (q == NULL || carrier == NULL || cfg == NULL || slot == NULL || resource == NULL || slot_symbols == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  if (srsran_pucch_nr_cfg_resource_valid(resource) < SRSRAN_SUCCESS) {
    ERROR("Invalid PUCCH format 1 resource");
    return SRSRAN_ERROR;
  }

  // Get group sequence
  uint32_t u = 0;
  uint32_t v = 0;
  if (srsran_pucch_nr_group_sequence(carrier, cfg, &u, &v) < SRSRAN_SUCCESS) {
    ERROR("Error getting group sequence");
    return SRSRAN_ERROR;
  }

  uint32_t n_pucch = dmrs_pucch_format1_n_pucch(resource, 0);
  if (n_pucch == 0) {
    ERROR("Error getting number of symbols");
    return SRSRAN_ERROR;
  }

  uint32_t l_prime = resource->start_symbol_idx;
  for (uint32_t m = 0; m < n_pucch; m++) {
    // Clause 6.4.1.3.1.2 specifies l=0,2,4...
    uint32_t l = m * 2;

    // Get start of the sequence in resource grid
    cf_t* slot_symbols_ptr = &slot_symbols[(carrier->nof_prb * (l + l_prime) + resource->starting_prb) * SRSRAN_NRE];

    // Get Alpha index
    uint32_t alpha_idx = 0;
    if (srsran_pucch_nr_alpha_idx(carrier, cfg, slot, l, l_prime, resource->initial_cyclic_shift, 0, &alpha_idx) <
        SRSRAN_SUCCESS) {
      ERROR("Calculating alpha");
    }

    // get r_uv sequence from LUT object
    const cf_t* r_uv = srsran_zc_sequence_lut_get(&q->r_uv_1prb, u, v, alpha_idx);
    if (r_uv == NULL) {
      ERROR("Getting r_uv sequence");
      return SRSRAN_ERROR;
    }

    // Get w_i_m
    cf_t w_i_m = srsran_pucch_nr_format1_w(q, n_pucch, resource->time_domain_occ, m);

    // Compute z(n) = w(i) * r_uv(n)
    cf_t z[SRSRAN_NRE];
    srsran_vec_sc_prod_ccc(r_uv, w_i_m, z, SRSRAN_NRE);

    // Put z in the grid
    srsran_vec_cf_copy(slot_symbols_ptr, z, SRSRAN_NRE);
  }

  return SRSRAN_SUCCESS;
}

int srsran_dmrs_pucch_format1_estimate(const srsran_pucch_nr_t*            q,
                                       const srsran_carrier_nr_t*          carrier,
                                       const srsran_pucch_nr_common_cfg_t* cfg,
                                       const srsran_slot_cfg_t*            slot,
                                       const srsran_pucch_nr_resource_t*   resource,
                                       const cf_t*                         slot_symbols,
                                       srsran_chest_ul_res_t*              res)
{
  if (q == NULL || carrier == NULL || cfg == NULL || slot == NULL || resource == NULL || slot_symbols == NULL ||
      res == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  if (srsran_pucch_nr_cfg_resource_valid(resource) < SRSRAN_SUCCESS) {
    ERROR("Invalid PUCCH format 1 resource");
    return SRSRAN_ERROR;
  }

  // Get group sequence
  uint32_t u = 0;
  uint32_t v = 0;
  if (srsran_pucch_nr_group_sequence(carrier, cfg, &u, &v) < SRSRAN_SUCCESS) {
    ERROR("Error getting group sequence");
    return SRSRAN_ERROR;
  }

  uint32_t n_pucch = dmrs_pucch_format1_n_pucch(resource, 0);
  if (n_pucch == 0) {
    ERROR("Error getting number of symbols");
    return SRSRAN_ERROR;
  }

  cf_t     ce[SRSRAN_PUCCH_NR_FORMAT1_N_MAX][SRSRAN_NRE];
  uint32_t l_prime = resource->start_symbol_idx;
  for (uint32_t m = 0; m < n_pucch; m++) {
    // Clause 6.4.1.3.1.2 specifies l=0,2,4...
    uint32_t l = m * 2;

    // Get start of the sequence in resource grid
    const cf_t* slot_symbols_ptr =
        &slot_symbols[(carrier->nof_prb * (l + l_prime) + resource->starting_prb) * SRSRAN_NRE];

    // Get Alpha index
    uint32_t alpha_idx = 0;
    if (srsran_pucch_nr_alpha_idx(carrier, cfg, slot, l, l_prime, resource->initial_cyclic_shift, 0, &alpha_idx) <
        SRSRAN_SUCCESS) {
      ERROR("Calculating alpha");
    }

    // get r_uv sequence from LUT object
    const cf_t* r_uv = srsran_zc_sequence_lut_get(&q->r_uv_1prb, u, v, alpha_idx);
    if (r_uv == NULL) {
      ERROR("Getting r_uv sequence");
      return SRSRAN_ERROR;
    }

    // Get w_i_m
    cf_t w_i_m = srsran_pucch_nr_format1_w(q, n_pucch, resource->time_domain_occ, m);

    // Compute z(n) = w(i) * r_uv(n)
    cf_t z[SRSRAN_NRE];
    srsran_vec_sc_prod_ccc(r_uv, w_i_m, z, SRSRAN_NRE);

    // Calculate least square estimates for this symbol
    srsran_vec_prod_conj_ccc(slot_symbols_ptr, z, ce[m], SRSRAN_NRE);
  }

  // Perform measurements
  float rsrp   = 0.0f;
  float epre   = 0.0f;
  float ta_err = 0.0f;
  for (uint32_t m = 0; m < n_pucch; m++) {
    cf_t corr = srsran_vec_acc_cc(ce[m], SRSRAN_NRE) / SRSRAN_NRE;
    rsrp += __real__ corr * __real__ corr + __imag__ corr * __imag__ corr;
    epre += srsran_vec_avg_power_cf(ce[m], SRSRAN_NRE);
    ta_err += srsran_vec_estimate_frequency(ce[m], SRSRAN_NRE);
  }

  // Average measurements
  rsrp /= n_pucch;
  epre /= n_pucch;
  ta_err /= n_pucch;

  // Set power measures
  rsrp                    = SRSRAN_MIN(rsrp, epre);
  res->rsrp               = rsrp;
  res->rsrp_dBfs          = srsran_convert_power_to_dB(rsrp);
  res->epre               = epre;
  res->epre_dBfs          = srsran_convert_power_to_dB(epre);
  res->noise_estimate     = epre - rsrp;
  res->noise_estimate_dbm = srsran_convert_power_to_dB(res->noise_estimate);
  res->snr                = rsrp / res->noise_estimate;
  res->snr_db             = srsran_convert_power_to_dB(res->snr);

  // Compute Time Aligment error in microseconds
  if (isnormal(ta_err)) {
    ta_err /= 15e3f * (float)(1U << carrier->scs); // Convert from normalized frequency to seconds
    ta_err *= 1e6f;                                       // Convert to micro-seconds
    ta_err     = roundf(ta_err * 10.0f) / 10.0f;          // Round to one tenth of micro-second
    res->ta_us = ta_err;
  } else {
    res->ta_us = 0.0f;
  }

  // Measure CFO
  res->cfo_hz = NAN; // Not implemented

  // Do averaging here
  // ... Not implemented

  // Interpolates between DMRS symbols
  for (uint32_t m = 0; m < n_pucch; m++) {
    uint32_t l      = m * 2 + 1;
    cf_t*    ce_ptr = &res->ce[(carrier->nof_prb * (l + l_prime) + resource->starting_prb) * SRSRAN_NRE];

    if (m != n_pucch - 1) {
      // If it is not the last symbol with DMRS, average between
      srsran_vec_sum_ccc(ce[m], ce[m + 1], ce_ptr, SRSRAN_NRE);
      srsran_vec_sc_prod_cfc(ce_ptr, 0.5f, ce_ptr, SRSRAN_NRE);
    } else if (m != 0) {
      // Extrapolate for the last if more than 1 are provided
      srsran_vec_sc_prod_cfc(ce[m], 3.0f, ce_ptr, SRSRAN_NRE);
      srsran_vec_sub_ccc(ce_ptr, ce[m - 1], ce_ptr, SRSRAN_NRE);
      srsran_vec_sc_prod_cfc(ce_ptr, 0.5f, ce_ptr, SRSRAN_NRE);
    } else {
      // Simply copy the
      srsran_vec_cf_copy(ce_ptr, ce[m], SRSRAN_NRE);
    }
  }

  return SRSRAN_SUCCESS;
}

static uint32_t dmrs_pucch_format2_cinit(const srsran_carrier_nr_t*          carrier,
                                         const srsran_pucch_nr_common_cfg_t* cfg,
                                         const srsran_slot_cfg_t*            slot,
                                         uint32_t                            l)
{
  uint64_t n    = SRSRAN_SLOT_NR_MOD(carrier->scs, slot->idx);
  uint64_t n_id = (cfg->scrambling_id_present) ? cfg->scambling_id : carrier->pci;

  return SRSRAN_SEQUENCE_MOD((((SRSRAN_NSYMB_PER_SLOT_NR * n + l + 1UL) * (2UL * n_id + 1UL)) << 17UL) + 2UL * n_id);
}

int srsran_dmrs_pucch_format2_put(const srsran_pucch_nr_t*            q,
                                  const srsran_carrier_nr_t*          carrier,
                                  const srsran_pucch_nr_common_cfg_t* cfg,
                                  const srsran_slot_cfg_t*            slot,
                                  const srsran_pucch_nr_resource_t*   resource,
                                  cf_t*                               slot_symbols)
{
  if (q == NULL || carrier == NULL || cfg == NULL || slot == NULL || resource == NULL || slot_symbols == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  if (srsran_pucch_nr_cfg_resource_valid(resource) < SRSRAN_SUCCESS) {
    ERROR("Invalid PUCCH format 1 resource");
    return SRSRAN_ERROR;
  }

  uint32_t l_start = resource->start_symbol_idx;
  uint32_t l_end   = resource->start_symbol_idx + resource->nof_symbols;
  uint32_t k_start = SRSRAN_MIN(carrier->nof_prb - 1, resource->starting_prb) * SRSRAN_NRE + 1;
  uint32_t k_end   = SRSRAN_MIN(carrier->nof_prb, resource->starting_prb + resource->nof_prb) * SRSRAN_NRE;
  for (uint32_t l = l_start; l < l_end; l++) {
    // Compute sequence initial state
    uint32_t                cinit    = dmrs_pucch_format2_cinit(carrier, cfg, slot, l);
    srsran_sequence_state_t sequence = {};
    srsran_sequence_state_init(&sequence, cinit);

    // Skip PRBs to start
    srsran_sequence_state_advance(&sequence, 2 * 4 * resource->starting_prb);

    // Generate sequence
    cf_t r_l[SRSRAN_PUCCH_NR_FORMAT2_MAX_NPRB * 4];
    srsran_sequence_state_gen_f(&sequence, M_SQRT1_2, (float*)r_l, 2 * 4 * resource->nof_prb);

    // Put sequence in k = 3 * m + 1
    for (uint32_t k = k_start, i = 0; k < k_end; k += 3, i++) {
      slot_symbols[l * carrier->nof_prb * SRSRAN_NRE + k] = r_l[i];
    }
  }
  return SRSRAN_SUCCESS;
}

int srsran_dmrs_pucch_format2_estimate(const srsran_pucch_nr_t*            q,
                                       const srsran_carrier_nr_t*          carrier,
                                       const srsran_pucch_nr_common_cfg_t* cfg,
                                       const srsran_slot_cfg_t*            slot,
                                       const srsran_pucch_nr_resource_t*   resource,
                                       const cf_t*                         slot_symbols,
                                       srsran_chest_ul_res_t*              res)
{
  if (q == NULL || carrier == NULL || cfg == NULL || slot == NULL || resource == NULL || slot_symbols == NULL ||
      res == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  if (srsran_pucch_nr_cfg_resource_valid(resource) < SRSRAN_SUCCESS) {
    ERROR("Invalid PUCCH format 1 resource");
    return SRSRAN_ERROR;
  }

  cf_t ce[SRSRAN_PUCCH_NR_FORMAT2_MAX_NSYMB][SRSRAN_PUCCH_NR_FORMAT2_MAX_NPRB * 4];

  uint32_t l_start = resource->start_symbol_idx;
  uint32_t l_end   = resource->start_symbol_idx + resource->nof_symbols;
  uint32_t k_start = SRSRAN_MIN(carrier->nof_prb - 1, resource->starting_prb) * SRSRAN_NRE + 1;
  uint32_t k_end   = SRSRAN_MIN(carrier->nof_prb, resource->starting_prb + resource->nof_prb) * SRSRAN_NRE;
  uint32_t nof_ref = 4 * resource->nof_prb;
  for (uint32_t l = l_start, j = 0; l < l_end; l++, j++) {
    // Compute sequence initial state
    uint32_t                cinit    = dmrs_pucch_format2_cinit(carrier, cfg, slot, l);
    srsran_sequence_state_t sequence = {};
    srsran_sequence_state_init(&sequence, cinit);

    // Skip PRBs to start
    srsran_sequence_state_advance(&sequence, 2 * 4 * resource->starting_prb);

    // Generate sequence
    cf_t r_l[SRSRAN_PUCCH_NR_FORMAT2_MAX_NPRB * 4];
    srsran_sequence_state_gen_f(&sequence, M_SQRT1_2, (float*)r_l, 2 * nof_ref);

    // Put sequence in k = 3 * m + 1
    for (uint32_t k = k_start, i = 0; k < k_end; k += 3, i++) {
      ce[j][i] = slot_symbols[l * carrier->nof_prb * SRSRAN_NRE + k];
    }

    srsran_vec_prod_conj_ccc(ce[j], r_l, ce[j], nof_ref);
  }

  // Perform measurements
  float epre   = 0.0f;
  float rsrp   = 0.0f;
  float ta_err = 0.0f;
  for (uint32_t i = 0; i < resource->nof_symbols; i++) {
    cf_t corr = srsran_vec_acc_cc(ce[i], nof_ref) / nof_ref;
    rsrp += __real__ corr * __real__ corr + __imag__ corr * __imag__ corr;
    epre += srsran_vec_avg_power_cf(ce[i], nof_ref);
    ta_err += srsran_vec_estimate_frequency(ce[i], nof_ref);
  }
  epre /= resource->nof_symbols;
  rsrp /= resource->nof_symbols;
  ta_err /= resource->nof_symbols;

  // Set power measures
  rsrp                    = SRSRAN_MIN(rsrp, epre);
  res->rsrp               = rsrp;
  res->rsrp_dBfs          = srsran_convert_power_to_dB(rsrp);
  res->epre               = epre;
  res->epre_dBfs          = srsran_convert_power_to_dB(epre);
  res->noise_estimate     = epre - rsrp;
  res->noise_estimate_dbm = srsran_convert_power_to_dB(res->noise_estimate);
  res->snr                = rsrp / res->noise_estimate;
  res->snr_db             = srsran_convert_power_to_dB(res->snr);

  // Compute Time Aligment error in microseconds
  if (isnormal(ta_err)) {
    ta_err /= 15e3f * (float)(1U << carrier->scs) * 3; // Convert from normalized frequency to seconds
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
      res->ce[l * carrier->nof_prb * SRSRAN_NRE + k] = ce[j][i / 3];
    }
  }

  return SRSRAN_SUCCESS;
}

int srsran_dmrs_pucch_format_3_4_get_symbol_idx(const srsran_pucch_nr_resource_t* resource,
                                                uint32_t idx[SRSRAN_DMRS_PUCCH_FORMAT_3_4_MAX_NSYMB])
{
  if (resource == NULL || idx == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
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
      ERROR("Invalid case (%d)", resource->nof_symbols);
      return SRSRAN_ERROR;
  }

  return count;
}
