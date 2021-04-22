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

#include "srsran/phy/ch_estimation/csi_rs.h"
#include "srsran/phy/common/sequence.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/vector.h"
#include <complex.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Maximum number of subcarriers occupied by a CSI-RS resource as defined in TS 38.211 Table 7.4.1.5.3-1
 */
#define CSI_RS_MAX_SUBC_PRB 4

/**
 * @brief Maximum number of symbols occupied by a CSI-RS resource as defined in TS 38.211 Table 7.4.1.5.3-1
 */
#define CSI_RS_MAX_SYMBOLS_SLOT 4

static int csi_rs_location_f(const srsran_csi_rs_resource_mapping_t* resource, uint32_t i)
{
  uint32_t count           = 0;
  uint32_t nof_freq_domain = 0;
  uint32_t mul             = 1;
  switch (resource->row) {
    case srsran_csi_rs_resource_mapping_row_1:
      nof_freq_domain = SRSRAN_CSI_RS_NOF_FREQ_DOMAIN_ALLOC_ROW1;
      break;
    case srsran_csi_rs_resource_mapping_row_2:
      nof_freq_domain = SRSRAN_CSI_RS_NOF_FREQ_DOMAIN_ALLOC_ROW2;
      break;
    case srsran_csi_rs_resource_mapping_row_4:
      nof_freq_domain = SRSRAN_CSI_RS_NOF_FREQ_DOMAIN_ALLOC_ROW4;
      mul             = 4;
      break;
    case srsran_csi_rs_resource_mapping_row_other:
      nof_freq_domain = SRSRAN_CSI_RS_NOF_FREQ_DOMAIN_ALLOC_OTHER;
      mul             = 2;
      break;
  }

  for (uint32_t j = 0; j < nof_freq_domain; j++) {
    if (resource->frequency_domain_alloc[nof_freq_domain - 1 - j]) {
      count++;
    }

    if (count == i) {
      return j * mul;
    }
  }

  ERROR("Unhandled configuration");
  return SRSRAN_ERROR;
}

// Table 7.4.1.5.3-1: CSI-RS locations within a slot
static int csi_rs_location_get_k_list(const srsran_csi_rs_resource_mapping_t* resource,
                                      uint32_t                                j,
                                      uint32_t                                k_list[CSI_RS_MAX_SUBC_PRB])
{
  int k0 = csi_rs_location_f(resource, 1);
  //  int k1 = csi_rs_location_f(resource, 2);
  //  int k2 = csi_rs_location_f(resource, 3);
  //  int k3 = csi_rs_location_f(resource, 4);

  if (k0 < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  // Row 1
  if (resource->row == srsran_csi_rs_resource_mapping_row_1 && resource->nof_ports == 1 &&
      resource->density == srsran_csi_rs_resource_mapping_density_three && resource->cdm == srsran_csi_rs_cdm_nocdm &&
      j == 0) {
    k_list[0] = k0;
    k_list[1] = k0 + 4;
    k_list[2] = k0 + 8;
    return 3;
  }

  // Row 2
  if (resource->row == srsran_csi_rs_resource_mapping_row_2 && resource->nof_ports == 1 &&
      resource->cdm == srsran_csi_rs_cdm_nocdm) {
    if (resource->density == srsran_csi_rs_resource_mapping_density_one ||
        resource->density == srsran_csi_rs_resource_mapping_density_dot5_even ||
        resource->density == srsran_csi_rs_resource_mapping_density_dot5_odd) {
      k_list[0] = k0;
      return 1;
    }
  }

  // Row 4
  if (resource->row == srsran_csi_rs_resource_mapping_row_4 && resource->nof_ports == 4 &&
      resource->density == srsran_csi_rs_resource_mapping_density_one && resource->cdm == srsran_csi_rs_cdm_fd_cdm2) {
    if (j == 0) {
      k_list[0] = k0;
      k_list[1] = k0 + 1;
      return 2;
    }
    if (j == 1) {
      k_list[0] = k0 + 2;
      k_list[1] = k0 + 2 + 1;
      return 2;
    }
  }

  ERROR("Unhandled configuration");
  return SRSRAN_ERROR;
}

// Table 7.4.1.5.3-1: CSI-RS locations within a slot
static int csi_rs_location_get_l_list(const srsran_csi_rs_resource_mapping_t* resource,
                                      uint32_t                                j,
                                      uint32_t                                l_list[CSI_RS_MAX_SYMBOLS_SLOT])
{
  uint32_t l0 = resource->first_symbol_idx;

  if (l0 > 13) {
    return SRSRAN_ERROR;
  }

  //  uint32_t l1 = resource->first_symbol_idx2;
  //  if (l1 < 2 || l1 > 12) {
  //    return SRSRAN_ERROR;
  //  }

  // Row 1
  if (resource->row == srsran_csi_rs_resource_mapping_row_1 && resource->nof_ports == 1 &&
      resource->density == srsran_csi_rs_resource_mapping_density_three && resource->cdm == srsran_csi_rs_cdm_nocdm) {
    l_list[0] = l0;
    return 1;
  }

  // Row 2
  if (resource->row == srsran_csi_rs_resource_mapping_row_2 && resource->nof_ports == 1 &&
      resource->cdm == srsran_csi_rs_cdm_nocdm) {
    if (resource->density == srsran_csi_rs_resource_mapping_density_one ||
        resource->density == srsran_csi_rs_resource_mapping_density_dot5_even ||
        resource->density == srsran_csi_rs_resource_mapping_density_dot5_odd) {
      l_list[0] = l0;
      return 1;
    }
  }

  // Row 4
  if (resource->row == srsran_csi_rs_resource_mapping_row_4 && resource->nof_ports == 4 &&
      resource->density == srsran_csi_rs_resource_mapping_density_one && resource->cdm == srsran_csi_rs_cdm_fd_cdm2) {
    if (j == 0) {
      l_list[0] = l0;
      return 1;
    }
    if (j == 1) {
      l_list[0] = l0;
      return 1;
    }
  }

  ERROR("Unhandled configuration");
  return SRSRAN_ERROR;
}

static uint32_t csi_rs_cinit(const srsran_carrier_nr_t*          carrier,
                             const srsran_slot_cfg_t*            slot_cfg,
                             const srsran_csi_rs_nzp_resource_t* resource,
                             uint32_t                            l)
{
  uint32_t n    = SRSRAN_SLOT_NR_MOD(carrier->scs, slot_cfg->idx);
  uint32_t n_id = resource->scrambling_id;

  return ((SRSRAN_NSYMB_PER_SLOT_NR * n + l + 1UL) * (2UL * n_id) << 10UL) + n_id;
}

bool srsran_csi_rs_send(const srsran_csi_rs_period_and_offset_t* periodicity, const srsran_slot_cfg_t* slot_cfg)
{
  if (periodicity == NULL || slot_cfg == NULL) {
    return false;
  }

  if (periodicity->period == 0) {
    return false;
  }

  uint32_t n = ((slot_cfg->idx + periodicity->period) - periodicity->offset) % periodicity->period;

  return n == 0;
}

static int csi_rs_nof_cdm_groups(const srsran_csi_rs_resource_mapping_t* resource)
{
  if (resource->row == srsran_csi_rs_resource_mapping_row_1 && resource->nof_ports == 1 &&
      resource->density == srsran_csi_rs_resource_mapping_density_three && resource->cdm == srsran_csi_rs_cdm_nocdm) {
    return 1;
  }

  // Row 1
  if (resource->row == srsran_csi_rs_resource_mapping_row_2 && resource->nof_ports == 1 &&
      resource->cdm == srsran_csi_rs_cdm_nocdm) {
    if (resource->density == srsran_csi_rs_resource_mapping_density_one ||
        resource->density == srsran_csi_rs_resource_mapping_density_dot5_even ||
        resource->density == srsran_csi_rs_resource_mapping_density_dot5_odd) {
      return 1;
    }
  }

  // Row 2
  if (resource->row == srsran_csi_rs_resource_mapping_row_2 && resource->nof_ports == 1 &&
      resource->cdm == srsran_csi_rs_cdm_nocdm) {
    if (resource->density == srsran_csi_rs_resource_mapping_density_one ||
        resource->density == srsran_csi_rs_resource_mapping_density_dot5_even ||
        resource->density == srsran_csi_rs_resource_mapping_density_dot5_odd) {
      return 1;
    }
  }

  // Row 3
  if (resource->row == srsran_csi_rs_resource_mapping_row_4 && resource->nof_ports == 4 &&
      resource->density == srsran_csi_rs_resource_mapping_density_one && resource->cdm == srsran_csi_rs_cdm_fd_cdm2) {
    return 2;
  }

  ERROR("Unhandled configuration");
  return SRSRAN_ERROR;
}

uint32_t csi_rs_count(srsran_csi_rs_density_t density, uint32_t nprb)
{
  switch (density) {
    case srsran_csi_rs_resource_mapping_density_three:
      return nprb * 3;
    case srsran_csi_rs_resource_mapping_density_dot5_even:
      return nprb / 2;
    case srsran_csi_rs_resource_mapping_density_dot5_odd:
      return nprb / 2;
    case srsran_csi_rs_resource_mapping_density_one:
      return nprb;
    case srsran_csi_rs_resource_mapping_density_spare:
    default:; // Do nothing
  }
  return 0;
}

static uint32_t csi_rs_rb_begin(const srsran_carrier_nr_t* carrier, const srsran_csi_rs_resource_mapping_t* m)
{
  uint32_t ret = SRSRAN_MAX(carrier->start, m->freq_band.start_rb);

  if ((m->density == srsran_csi_rs_resource_mapping_density_dot5_even && ret % 2 == 1) ||
      (m->density == srsran_csi_rs_resource_mapping_density_dot5_odd && ret % 2 == 0)) {
    ret++;
  }

  return ret;
}

static uint32_t csi_rs_rb_end(const srsran_carrier_nr_t* carrier, const srsran_csi_rs_resource_mapping_t* m)
{
  return SRSRAN_MIN(carrier->start + carrier->nof_prb, m->freq_band.start_rb + m->freq_band.nof_rb);
}

static uint32_t csi_rs_rb_stride(const srsran_csi_rs_resource_mapping_t* m)
{
  uint32_t ret = 1;

  // Special .5 density cases
  if (m->density == srsran_csi_rs_resource_mapping_density_dot5_even ||
      m->density == srsran_csi_rs_resource_mapping_density_dot5_odd) {
    // Skip one RB
    ret = 2;
  }

  return ret;
}

int srsran_csi_rs_append_resource_to_pattern(const srsran_carrier_nr_t*              carrier,
                                             const srsran_csi_rs_resource_mapping_t* resource,
                                             srsran_re_pattern_list_t*               re_pattern_list)
{
  // Check inputs
  if (resource == NULL || re_pattern_list == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Create temporal pattern
  srsran_re_pattern_t pattern = {};
  pattern.rb_begin            = csi_rs_rb_begin(carrier, resource);
  pattern.rb_end              = csi_rs_rb_end(carrier, resource);
  pattern.rb_stride           = csi_rs_rb_stride(resource);

  // Calculate number of CDM groups
  int nof_cdm_groups = csi_rs_nof_cdm_groups(resource);
  if (nof_cdm_groups < SRSRAN_SUCCESS) {
    ERROR("Error getting number of CDM groups");
    return SRSRAN_ERROR;
  }

  // Iterate over all CDM groups
  for (int j = 0; j < nof_cdm_groups; j++) {
    // Get SC indexes
    uint32_t k_list[CSI_RS_MAX_SUBC_PRB] = {};
    int      nof_k                       = csi_rs_location_get_k_list(resource, j, k_list);
    if (nof_k < SRSRAN_SUCCESS) {
      ERROR("Error getting indexes for CSI-RS");
      return SRSRAN_ERROR;
    }

    // Fill subcarrier mask
    for (int k = 0; k < nof_k; k++) {
      pattern.sc[k_list[k]] = true;
    }

    // Get OFDM symbol indexes
    uint32_t l_list[CSI_RS_MAX_SUBC_PRB] = {};
    int      nof_l                       = csi_rs_location_get_l_list(resource, j, l_list);
    if (nof_l < SRSRAN_SUCCESS) {
      ERROR("Error getting indexes for CSI-RS");
      return SRSRAN_ERROR;
    }

    // Fill OFDM symbol mask
    for (int l = 0; l < nof_l; l++) {
      pattern.symbol[l_list[l]] = true;
    }

    if (srsran_re_pattern_merge(re_pattern_list, &pattern) < SRSRAN_SUCCESS) {
      ERROR("Error merging pattern");
      return SRSRAN_ERROR;
    }
  }

  return SRSRAN_SUCCESS;
}

int srsran_csi_rs_nzp_put(const srsran_carrier_nr_t*          carrier,
                          const srsran_slot_cfg_t*            slot_cfg,
                          const srsran_csi_rs_nzp_resource_t* resource,
                          cf_t*                               grid)
{
  if (carrier == NULL || resource == NULL || grid == NULL) {
    return SRSRAN_ERROR;
  }

  // Force CDM group to 0
  uint32_t j = 0;

  uint32_t k_list[CSI_RS_MAX_SUBC_PRB];
  int      nof_k = csi_rs_location_get_k_list(&resource->resource_mapping, j, k_list);
  if (nof_k <= 0) {
    return SRSRAN_ERROR;
  }

  uint32_t l_list[CSI_RS_MAX_SYMBOLS_SLOT];
  int      nof_l = csi_rs_location_get_l_list(&resource->resource_mapping, j, l_list);
  if (nof_l <= 0) {
    return SRSRAN_ERROR;
  }

  // Calculate Resource Block boundaries
  uint32_t rb_begin  = csi_rs_rb_begin(carrier, &resource->resource_mapping);
  uint32_t rb_end    = csi_rs_rb_end(carrier, &resource->resource_mapping);
  uint32_t rb_stride = csi_rs_rb_stride(&resource->resource_mapping);

  // Calculate power allocation
  float beta = srsran_convert_dB_to_amplitude((float)resource->power_control_offset);
  if (!isnormal(beta)) {
    beta = 1.0f;
  }

  for (int l_idx = 0; l_idx < nof_l; l_idx++) {
    // Get symbol index
    uint32_t l = l_list[l_idx];

    // Initialise sequence for this OFDM symbol
    uint32_t                cinit          = csi_rs_cinit(carrier, slot_cfg, resource, l);
    srsran_sequence_state_t sequence_state = {};
    srsran_sequence_state_init(&sequence_state, cinit);

    // Skip unallocated RB
    srsran_sequence_state_advance(&sequence_state, 2 * csi_rs_count(resource->resource_mapping.density, rb_begin));

    // Temporal R sequence
    cf_t     r[64];
    uint32_t r_idx = 64;

    // Iterate over frequency domain
    for (uint32_t n = rb_begin; n < rb_end; n += rb_stride) {
      for (uint32_t k_idx = 0; k_idx < nof_k; k_idx++) {
        // Calculate sub-carrier index k
        uint32_t k = SRSRAN_NRE * n + k_list[k_idx];

        // Do we need more r?
        if (r_idx >= 64) {
          // ... Generate a bunch of it!
          srsran_sequence_state_gen_f(&sequence_state, M_SQRT1_2 * beta, (float*)r, 64 * 2);
          r_idx = 0;
        }

        // Put CSI in grid
        grid[l * SRSRAN_NRE * carrier->nof_prb + k] = r[r_idx++];
      }
    }
  }

  return SRSRAN_SUCCESS;
}

int srsran_csi_rs_nzp_measure(const srsran_carrier_nr_t*          carrier,
                              const srsran_slot_cfg_t*            slot_cfg,
                              const srsran_csi_rs_nzp_resource_t* resource,
                              const cf_t*                         grid,
                              srsran_csi_rs_measure_t*            measure)
{
  if (carrier == NULL || resource == NULL || grid == NULL) {
    return SRSRAN_ERROR;
  }

  // Force CDM group to 0
  uint32_t j = 0;

  uint32_t k_list[CSI_RS_MAX_SUBC_PRB];
  int      nof_k = csi_rs_location_get_k_list(&resource->resource_mapping, j, k_list);
  if (nof_k <= 0) {
    return SRSRAN_ERROR;
  }

  uint32_t l_list[CSI_RS_MAX_SYMBOLS_SLOT];
  int      nof_l = csi_rs_location_get_l_list(&resource->resource_mapping, j, l_list);
  if (nof_l <= 0) {
    return SRSRAN_ERROR;
  }

  // Calculate Resource Block boundaries
  uint32_t rb_begin  = csi_rs_rb_begin(carrier, &resource->resource_mapping);
  uint32_t rb_end    = csi_rs_rb_end(carrier, &resource->resource_mapping);
  uint32_t rb_stride = csi_rs_rb_stride(&resource->resource_mapping);

  // Accumulators
  float    epre_acc = 0.0f;
  cf_t     rsrp_acc = 0.0f;
  uint32_t count    = 0;

  for (int l_idx = 0; l_idx < nof_l; l_idx++) {
    // Get symbol index
    uint32_t l = l_list[l_idx];

    // Initialise sequence for this OFDM symbol
    uint32_t                cinit          = csi_rs_cinit(carrier, slot_cfg, resource, l);
    srsran_sequence_state_t sequence_state = {};
    srsran_sequence_state_init(&sequence_state, cinit);

    // Skip unallocated RB
    srsran_sequence_state_advance(&sequence_state, 2 * csi_rs_count(resource->resource_mapping.density, rb_begin));

    // Temporal R sequence
    cf_t     r[64];
    uint32_t r_idx = 64;

    // Iterate over frequency domain
    for (uint32_t n = rb_begin; n < rb_end; n += rb_stride) {
      for (uint32_t k_idx = 0; k_idx < nof_k; k_idx++) {
        // Calculate sub-carrier index k
        uint32_t k = SRSRAN_NRE * n + k_list[k_idx];

        // Do we need more r?
        if (r_idx >= 64) {
          // ... Generate a bunch of it!
          srsran_sequence_state_gen_f(&sequence_state, M_SQRT1_2, (float*)r, 64 * 2);
          r_idx = 0;
        }

        // Take CSI-RS from grid and measure
        cf_t tmp = grid[l * SRSRAN_NRE * carrier->nof_prb + k] * conjf(r[r_idx++]);
        rsrp_acc += tmp;
        epre_acc += __real__ tmp * __real__ tmp + __imag__ tmp * __imag__ tmp;
        count++;
      }
    }
  }

  if (count) {
    measure->epre = epre_acc / (float)count;
    rsrp_acc /= (float)count;
    measure->rsrp = (__real__ rsrp_acc * __real__ rsrp_acc + __imag__ rsrp_acc * __imag__ rsrp_acc);
    if (measure->epre > measure->rsrp) {
      measure->n0 = measure->epre - measure->rsrp;
    } else {
      measure->n0 = 0.0f;
    }
  }

  measure->rsrp_dB = srsran_convert_power_to_dB(measure->rsrp);
  measure->epre_dB = srsran_convert_power_to_dB(measure->epre);
  measure->n0_dB   = srsran_convert_power_to_dB(measure->n0);
  measure->snr_dB  = measure->rsrp_dB - measure->n0_dB;
  measure->nof_re  = count;

  return SRSRAN_SUCCESS;
}

uint32_t srsran_csi_rs_measure_info(const srsran_csi_rs_measure_t* measure, char* str, uint32_t str_len)
{
  return srsran_print_check(str,
                            str_len,
                            0,
                            "rsrp=%+.1f, epre=%+.1f, n0=%+.1f, snr=%+.1f, nof_re=%d",
                            measure->rsrp_dB,
                            measure->epre_dB,
                            measure->n0_dB,
                            measure->snr_dB,
                            measure->nof_re);
}