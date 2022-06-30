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

#define RESOURCE_ERROR(R)                                                                                              \
  do {                                                                                                                 \
    char res_info_str[256];                                                                                            \
    srsran_csi_rs_resource_mapping_info(R, res_info_str, (uint32_t)sizeof(res_info_str));                              \
    ERROR("Unhandled configuration %s", res_info_str);                                                                 \
  } while (false)

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
      return (int)(j * mul);
    }
  }

  // Inform about an unhandled configuration
  RESOURCE_ERROR(resource);
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
  if (resource->row == srsran_csi_rs_resource_mapping_row_2 && resource->cdm == srsran_csi_rs_cdm_nocdm) {
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

  // Inform about an unhandled configuration
  RESOURCE_ERROR(resource);
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
  if (resource->row == srsran_csi_rs_resource_mapping_row_2 && resource->cdm == srsran_csi_rs_cdm_nocdm) {
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

  // Inform about an unhandled configuration
  RESOURCE_ERROR(resource);
  return SRSRAN_ERROR;
}

static uint32_t csi_rs_cinit(const srsran_carrier_nr_t*          carrier,
                             const srsran_slot_cfg_t*            slot_cfg,
                             const srsran_csi_rs_nzp_resource_t* resource,
                             uint32_t                            l)
{
  uint32_t n    = SRSRAN_SLOT_NR_MOD(carrier->scs, slot_cfg->idx);
  uint32_t n_id = resource->scrambling_id;

  return SRSRAN_SEQUENCE_MOD((((SRSRAN_NSYMB_PER_SLOT_NR * n + l + 1UL) * (2UL * n_id + 1UL)) << 10UL) + n_id);
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
  // Row 1
  if (resource->row == srsran_csi_rs_resource_mapping_row_1 && resource->nof_ports == 1 &&
      resource->density == srsran_csi_rs_resource_mapping_density_three && resource->cdm == srsran_csi_rs_cdm_nocdm) {
    return 1;
  }

  // Row 2
  if (resource->row == srsran_csi_rs_resource_mapping_row_2 && resource->cdm == srsran_csi_rs_cdm_nocdm) {
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

  // Inform about an unhandled configuration
  RESOURCE_ERROR(resource);
  return SRSRAN_ERROR;
}

bool srsran_csi_rs_resource_mapping_is_valid(const srsran_csi_rs_resource_mapping_t* res)
{
  if (res == NULL) {
    return false;
  }

  if (csi_rs_nof_cdm_groups(res) < 1) {
    return false;
  }

  uint32_t l_list[CSI_RS_MAX_SYMBOLS_SLOT] = {};
  if (csi_rs_location_get_l_list(res, 0, l_list) < SRSRAN_SUCCESS) {
    return false;
  }

  uint32_t k_list[CSI_RS_MAX_SUBC_PRB] = {};
  if (csi_rs_location_get_k_list(res, 0, k_list) < SRSRAN_SUCCESS) {
    return false;
  }

  return true;
}

uint32_t srsran_csi_rs_resource_mapping_info(const srsran_csi_rs_resource_mapping_t* res, char* str, uint32_t str_len)
{
  uint32_t len = 0;

  const char* row_str         = "invalid";
  uint32_t    nof_freq_domain = 0;
  switch (res->row) {
    case srsran_csi_rs_resource_mapping_row_1:
      row_str         = "1";
      nof_freq_domain = SRSRAN_CSI_RS_NOF_FREQ_DOMAIN_ALLOC_ROW1;
      break;
    case srsran_csi_rs_resource_mapping_row_2:
      row_str         = "2";
      nof_freq_domain = SRSRAN_CSI_RS_NOF_FREQ_DOMAIN_ALLOC_ROW2;
      break;
    case srsran_csi_rs_resource_mapping_row_4:
      row_str         = "4";
      nof_freq_domain = SRSRAN_CSI_RS_NOF_FREQ_DOMAIN_ALLOC_ROW4;
      break;
    case srsran_csi_rs_resource_mapping_row_other:
      row_str         = "other";
      nof_freq_domain = SRSRAN_CSI_RS_NOF_FREQ_DOMAIN_ALLOC_OTHER;
      break;
  }

  const char* cdm_str = "invalid";
  switch (res->cdm) {
    case srsran_csi_rs_cdm_nocdm:
      cdm_str = "nocdm";
      break;
    case srsran_csi_rs_cdm_fd_cdm2:
      cdm_str = "FD-CDM2";
      break;
    case srsran_csi_rs_cdm_cdm4_fd2_td2:
      cdm_str = "CDM4-FD2-TD2";
      break;
    case srsran_csi_rs_cdm_cdm8_fd2_td4:
      cdm_str = "CDM8-FD2-TD4";
      break;
  }

  const char* density_str = "invalid";
  switch (res->density) {
    case srsran_csi_rs_resource_mapping_density_three:
      density_str = "3";
      break;
    case srsran_csi_rs_resource_mapping_density_dot5_even:
      density_str = ".5 (even)";
      break;
    case srsran_csi_rs_resource_mapping_density_dot5_odd:
      density_str = ".5 (odd)";
      break;
    case srsran_csi_rs_resource_mapping_density_one:
      density_str = "1";
      break;
    case srsran_csi_rs_resource_mapping_density_spare:
      density_str = "spare";
      break;
  }

  char frequency_domain_alloc[SRSRAN_CSI_RS_NOF_FREQ_DOMAIN_ALLOC_MAX + 1];
  srsran_vec_sprint_bin(frequency_domain_alloc,
                        SRSRAN_CSI_RS_NOF_FREQ_DOMAIN_ALLOC_MAX + 1,
                        (uint8_t*)res->frequency_domain_alloc,
                        nof_freq_domain);

  len = srsran_print_check(str,
                           str_len,
                           len,
                           "row=%s freq=%s nof_ports=%d fist_symb=%d fist_symb2=%d cdm=%s density=%s rb=(%d:%d)",
                           row_str,
                           frequency_domain_alloc,
                           res->nof_ports,
                           res->first_symbol_idx,
                           res->first_symbol_idx2,
                           cdm_str,
                           density_str,
                           res->freq_band.start_rb,
                           res->freq_band.start_rb + res->freq_band.nof_rb - 1);

  return len;
}

uint32_t csi_rs_count(srsran_csi_rs_density_t density, uint32_t nprb)
{
  switch (density) {
    case srsran_csi_rs_resource_mapping_density_three:
      return nprb * 3;
    case srsran_csi_rs_resource_mapping_density_dot5_even:
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

int srsran_csi_rs_nzp_put_resource(const srsran_carrier_nr_t*          carrier,
                                   const srsran_slot_cfg_t*            slot_cfg,
                                   const srsran_csi_rs_nzp_resource_t* resource,
                                   cf_t*                               grid)
{
  // Verify inputs
  if (carrier == NULL || slot_cfg == NULL || resource == NULL || grid == NULL) {
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

int srsran_csi_rs_nzp_put_set(const srsran_carrier_nr_t*     carrier,
                              const srsran_slot_cfg_t*       slot_cfg,
                              const srsran_csi_rs_nzp_set_t* set,
                              cf_t*                          grid)
{
  // Verify inputs
  if (carrier == NULL || slot_cfg == NULL || set == NULL || grid == NULL) {
    return SRSRAN_ERROR;
  }

  uint32_t count = 0;

  // Iterate all resources in set
  for (uint32_t i = 0; i < set->count; i++) {
    // Skip resource
    if (!srsran_csi_rs_send(&set->data[i].periodicity, slot_cfg)) {
      continue;
    }

    // Put resource
    if (srsran_csi_rs_nzp_put_resource(carrier, slot_cfg, &set->data[i], grid) < SRSRAN_SUCCESS) {
      ERROR("Error putting NZP-CSI-RS resource");
      return SRSRAN_ERROR;
    }
    count++;
  }

  return (int)count;
}

/**
 * @brief Internal NZP-CSI-RS measurement structure
 */
typedef struct {
  uint32_t cri;      ///< CSI-RS resource identifier
  uint32_t l0;       ///< First OFDM symbol carrying CSI-RS
  float    epre;     ///< Linear EPRE
  cf_t     corr;     ///< Correlation
  float    delay_us; ///< Estimated average delay
  uint32_t nof_re;   ///< Total number of resource elements
} csi_rs_nzp_resource_measure_t;

static int csi_rs_nzp_measure_resource(const srsran_carrier_nr_t*          carrier,
                                       const srsran_slot_cfg_t*            slot_cfg,
                                       const srsran_csi_rs_nzp_resource_t* resource,
                                       const cf_t*                         grid,
                                       csi_rs_nzp_resource_measure_t*      measure)
{
  // Force CDM group to 0
  uint32_t j = 0;

  // Get subcarrier indexes
  uint32_t k_list[CSI_RS_MAX_SUBC_PRB];
  int      nof_k = csi_rs_location_get_k_list(&resource->resource_mapping, j, k_list);
  if (nof_k <= 0) {
    return SRSRAN_ERROR;
  }

  // Calculate average CSI-RS RE stride
  float avg_k_stride = (float)((k_list[0] + SRSRAN_NRE) - k_list[nof_k - 1]);
  for (uint32_t i = 1; i < (uint32_t)nof_k; i++) {
    avg_k_stride += (float)(k_list[i] - k_list[i - 1]);
  }
  avg_k_stride /= (float)nof_k;
  if (!isnormal(avg_k_stride)) {
    ERROR("Invalid avg_k_stride");
    return SRSRAN_ERROR;
  }

  // Get symbol indexes
  uint32_t l_list[CSI_RS_MAX_SYMBOLS_SLOT];
  int      nof_l = csi_rs_location_get_l_list(&resource->resource_mapping, j, l_list);
  if (nof_l <= 0) {
    return SRSRAN_ERROR;
  }

  // Calculate Resource Block boundaries
  uint32_t rb_begin  = csi_rs_rb_begin(carrier, &resource->resource_mapping);
  uint32_t rb_end    = csi_rs_rb_end(carrier, &resource->resource_mapping);
  uint32_t rb_stride = csi_rs_rb_stride(&resource->resource_mapping);

  // Calculate ideal number of RE per symbol
  uint32_t nof_re = csi_rs_count(resource->resource_mapping.density, rb_end - rb_begin);

  // Accumulators
  float epre_acc  = 0.0f;
  cf_t  corr_acc  = 0.0f;
  float delay_acc = 0.0f;

  // Initialise measurement
  SRSRAN_MEM_ZERO(measure, csi_rs_nzp_resource_measure_t, 1);

  // Iterate time symbols
  for (int l_idx = 0; l_idx < nof_l; l_idx++) {
    // Get symbol index
    uint32_t l = l_list[l_idx];

    // Initialise sequence for this OFDM symbol
    uint32_t                cinit          = csi_rs_cinit(carrier, slot_cfg, resource, l);
    srsran_sequence_state_t sequence_state = {};
    srsran_sequence_state_init(&sequence_state, cinit);

    // Skip unallocated RB
    srsran_sequence_state_advance(&sequence_state, 2 * csi_rs_count(resource->resource_mapping.density, rb_begin));

    // Temporal Least Square Estimates
    cf_t     lse[CSI_RS_MAX_SUBC_PRB * SRSRAN_MAX_PRB_NR];
    uint32_t count_re = 0;

    // Extract RE
    for (uint32_t n = rb_begin; n < rb_end; n += rb_stride) {
      for (uint32_t k_idx = 0; k_idx < nof_k; k_idx++) {
        // Calculate sub-carrier index k
        uint32_t k = SRSRAN_NRE * n + k_list[k_idx];

        lse[count_re++] = grid[l * SRSRAN_NRE * carrier->nof_prb + k];
      }
    }

    // Verify RE count matches the expected number of RE
    if (count_re == 0 || count_re != nof_re) {
      ERROR("Unmatched number of RE (%d != %d)", count_re, nof_re);
      return SRSRAN_ERROR;
    }

    // Compute LSE
    cf_t r[CSI_RS_MAX_SUBC_PRB * SRSRAN_MAX_PRB_NR];
    srsran_sequence_state_gen_f(&sequence_state, M_SQRT1_2, (float*)r, 2 * count_re);
    srsran_vec_prod_conj_ccc(lse, r, lse, count_re);

    // Compute average delay
    float delay = srsran_vec_estimate_frequency(lse, (int)count_re);
    delay_acc += delay;

    // Pre-compensate delay to avoid RSRP measurements get affected by average delay
    srsran_vec_apply_cfo(lse, delay, lse, (int)count_re);

    // Compute EPRE
    epre_acc += srsran_vec_avg_power_cf(lse, count_re);

    // Compute correlation
    corr_acc += srsran_vec_acc_cc(lse, count_re) / (float)count_re;
  }

  // Set measure fields
  measure->cri      = resource->id;
  measure->l0       = l_list[0];
  measure->epre     = epre_acc / (float)nof_l;
  measure->corr     = corr_acc / (float)nof_l;
  measure->delay_us = 1e6f * delay_acc / ((float)nof_l * SRSRAN_SUBC_SPACING_NR(carrier->scs));
  measure->nof_re   = nof_l * nof_re;

  return SRSRAN_SUCCESS;
}

static int csi_rs_nzp_measure_set(const srsran_carrier_nr_t*     carrier,
                                  const srsran_slot_cfg_t*       slot_cfg,
                                  const srsran_csi_rs_nzp_set_t* set,
                                  const cf_t*                    grid,
                                  csi_rs_nzp_resource_measure_t  measurements[SRSRAN_PHCH_CFG_MAX_NOF_CSI_RS_PER_SET])
{
  uint32_t count = 0;

  // Iterate all resources in set
  for (uint32_t i = 0; i < set->count; i++) {
    // Skip resource
    if (!srsran_csi_rs_send(&set->data[i].periodicity, slot_cfg)) {
      continue;
    }

    // Perform measurement
    if (csi_rs_nzp_measure_resource(carrier, slot_cfg, &set->data[i], grid, &measurements[count]) < SRSRAN_SUCCESS) {
      ERROR("Error measuring NZP-CSI-RS resource");
      return SRSRAN_ERROR;
    }
    count++;
  }

  return count;
}

int srsran_csi_rs_nzp_measure(const srsran_carrier_nr_t*          carrier,
                              const srsran_slot_cfg_t*            slot_cfg,
                              const srsran_csi_rs_nzp_resource_t* resource,
                              const cf_t*                         grid,
                              srsran_csi_trs_measurements_t*      measure)
{
  if (carrier == NULL || slot_cfg == NULL || resource == NULL || grid == NULL || measure == NULL) {
    return SRSRAN_ERROR;
  }

  csi_rs_nzp_resource_measure_t m = {};
  if (csi_rs_nzp_measure_resource(carrier, slot_cfg, resource, grid, &m) < SRSRAN_SUCCESS) {
    ERROR("Error measuring NZP-CSI-RS resource");
    return SRSRAN_ERROR;
  }

  // Copy measurements
  measure->epre     = m.epre;
  measure->rsrp     = SRSRAN_CSQABS(m.corr);
  measure->delay_us = m.delay_us;
  measure->nof_re   = m.nof_re;

  // Estimate noise from EPRE and RSPR
  if (measure->epre > measure->rsrp) {
    measure->n0 = measure->epre - measure->rsrp;
  } else {
    measure->n0 = 0.0f;
  }

  // CFO cannot be estimated with a single resource
  measure->cfo_hz     = 0.0f;
  measure->cfo_hz_max = 0.0f;

  // Calculate logarithmic measurements
  measure->rsrp_dB = srsran_convert_power_to_dB(measure->rsrp);
  measure->epre_dB = srsran_convert_power_to_dB(measure->epre);
  measure->n0_dB   = srsran_convert_power_to_dB(measure->n0);
  measure->snr_dB  = measure->rsrp_dB - measure->n0_dB;

  return SRSRAN_SUCCESS;
}

int srsran_csi_rs_nzp_measure_trs(const srsran_carrier_nr_t*     carrier,
                                  const srsran_slot_cfg_t*       slot_cfg,
                                  const srsran_csi_rs_nzp_set_t* set,
                                  const cf_t*                    grid,
                                  srsran_csi_trs_measurements_t* measure)
{
  // Verify inputs
  if (carrier == NULL || slot_cfg == NULL || set == NULL || grid == NULL || measure == NULL) {
    return SRSRAN_ERROR;
  }

  // Verify it is a TRS set
  if (!set->trs_info) {
    ERROR("The set is not configured as TRS");
    return SRSRAN_ERROR;
  }

  // Perform Measurements
  csi_rs_nzp_resource_measure_t measurements[SRSRAN_PHCH_CFG_MAX_NOF_CSI_RS_PER_SET];
  int                           ret = csi_rs_nzp_measure_set(carrier, slot_cfg, set, grid, measurements);

  // Return to prevent assigning negative values to count
  if (ret < SRSRAN_SUCCESS) {
    ERROR("Error performing measurements");
    return SRSRAN_ERROR;
  }
  uint32_t count = (uint32_t)ret;

  // No NZP-CSI-RS has been scheduled for this slot
  if (count == 0) {
    return 0;
  }

  // Make sure at least 2 measurements are scheduled
  if (count < 2) {
    ERROR("Not enough NZP-CSI-RS (%d) have been scheduled for this slot", count);
    return SRSRAN_ERROR;
  }

  // Make sure initial simbols are in ascending order
  for (uint32_t i = 1; i < count; i++) {
    if (measurements[i].l0 <= measurements[i - 1].l0) {
      ERROR("NZP-CSI-RS are not in ascending order (%d <= %d)", measurements[i].l0, measurements[i - 1].l0);
      return SRSRAN_ERROR;
    }
  }

  // Average measurements
  float    epre_sum  = 0.0f;
  float    rsrp_sum  = 0.0f;
  float    delay_sum = 0.0f;
  uint32_t nof_re    = 0;
  for (uint32_t i = 0; i < count; i++) {
    epre_sum += measurements[i].epre / (float)count;
    rsrp_sum += SRSRAN_CSQABS(measurements[i].corr) / (float)count;
    delay_sum += measurements[i].delay_us / (float)count;
    nof_re += measurements[i].nof_re;
  }

  // Compute CFO
  float cfo_sum = 0.0f;
  float cfo_max = 0.0f;
  for (uint32_t i = 1; i < count; i++) {
    float time_diff    = srsran_symbol_distance_s(measurements[i - 1].l0, measurements[i].l0, carrier->scs);
    float phase_diff   = cargf(measurements[i].corr * conjf(measurements[i - 1].corr));
    float cfo_max_temp = 0.0f;

    // Avoid zero division
    if (isnormal(time_diff)) {
      // Calculate maximum CFO from this pair of symbols
      cfo_max_temp = 1.0f / time_diff;

      // Calculate the actual CFO of this pair of symbols
      cfo_sum += phase_diff / (2.0f * M_PI * time_diff * (count - 1));
    }

    // Select the lowest CFO
    cfo_max = SRSRAN_MIN(cfo_max_temp, cfo_max);
  }

  // Copy measurements
  measure->epre       = epre_sum;
  measure->rsrp       = rsrp_sum;
  measure->delay_us   = delay_sum;
  measure->cfo_hz     = cfo_sum;
  measure->cfo_hz_max = cfo_max;
  measure->nof_re     = nof_re;

  // Estimate noise from EPRE and RSPR
  if (measure->epre > measure->rsrp) {
    measure->n0 = measure->epre - measure->rsrp;
  } else {
    measure->n0 = 0.0f;
  }

  // Calculate logarithmic measurements
  measure->rsrp_dB = srsran_convert_power_to_dB(measure->rsrp);
  measure->epre_dB = srsran_convert_power_to_dB(measure->epre);
  measure->n0_dB   = srsran_convert_power_to_dB(measure->n0);
  measure->snr_dB  = measure->rsrp_dB - measure->n0_dB;

  return count;
}

int srsran_csi_rs_nzp_measure_channel(const srsran_carrier_nr_t*         carrier,
                                      const srsran_slot_cfg_t*           slot_cfg,
                                      const srsran_csi_rs_nzp_set_t*     set,
                                      const cf_t*                        grid,
                                      srsran_csi_channel_measurements_t* measure)
{
  // Verify inputs
  if (carrier == NULL || slot_cfg == NULL || set == NULL || grid == NULL || measure == NULL) {
    return SRSRAN_ERROR;
  }

  // Perform Measurements
  csi_rs_nzp_resource_measure_t measurements[SRSRAN_PHCH_CFG_MAX_NOF_CSI_RS_PER_SET];
  int                           ret = csi_rs_nzp_measure_set(carrier, slot_cfg, set, grid, measurements);

  // Return to prevent assigning negative values to count
  if (ret < SRSRAN_SUCCESS) {
    ERROR("Error performing measurements");
    return SRSRAN_ERROR;
  }
  uint32_t count = (uint32_t)ret;

  // No NZP-CSI-RS has been scheduled for this slot
  if (count == 0) {
    return 0;
  }

  // Average measurements
  float epre_sum = 0.0f;
  float rsrp_sum = 0.0f;
  for (uint32_t i = 0; i < count; i++) {
    epre_sum += measurements[i].epre / (float)count;
    rsrp_sum += SRSRAN_CSQABS(measurements[i].corr) / (float)count;
  }

  // Estimate noise from EPRE and RSPR
  float n0 = 0.0f;
  if (epre_sum > rsrp_sum) {
    n0 = epre_sum - rsrp_sum;
  }
  float n0_db = srsran_convert_power_to_dB(n0);

  // Set measurements
  measure->cri               = measurements[0].cri;
  measure->wideband_rsrp_dBm = srsran_convert_power_to_dB(rsrp_sum);
  measure->wideband_epre_dBm = srsran_convert_power_to_dB(epre_sum);
  measure->wideband_snr_db   = measure->wideband_rsrp_dBm - n0_db;

  // Set other parameters
  measure->K_csi_rs  = count;
  measure->nof_ports = 1; // No other value is currently supported

  // Return the number of active resources for this slot
  return count;
}

/**
 * @brief Internal ZP-CSI-RS measurement structure
 */
typedef struct {
  uint32_t cri;    ///< CSI-RS resource identifier
  uint32_t l0;     ///< First OFDM symbol carrying CSI-RS
  float    epre;   ///< Linear EPRE
  uint32_t nof_re; ///< Total number of resource elements
} csi_rs_zp_resource_measure_t;

static int csi_rs_zp_measure_resource(const srsran_carrier_nr_t*         carrier,
                                      const srsran_slot_cfg_t*           slot_cfg,
                                      const srsran_csi_rs_zp_resource_t* resource,
                                      const cf_t*                        grid,
                                      csi_rs_zp_resource_measure_t*      measure)
{
  // Force CDM group to 0
  uint32_t j = 0;

  // Get subcarrier indexes
  uint32_t k_list[CSI_RS_MAX_SUBC_PRB];
  int      nof_k = csi_rs_location_get_k_list(&resource->resource_mapping, j, k_list);
  if (nof_k <= 0) {
    return SRSRAN_ERROR;
  }

  // Calculate average CSI-RS RE stride
  float avg_k_stride = (float)((k_list[0] + SRSRAN_NRE) - k_list[nof_k - 1]);
  for (uint32_t i = 1; i < (uint32_t)nof_k; i++) {
    avg_k_stride += (float)(k_list[i] - k_list[i - 1]);
  }
  avg_k_stride /= (float)nof_k;
  if (!isnormal(avg_k_stride)) {
    ERROR("Invalid avg_k_stride");
    return SRSRAN_ERROR;
  }

  // Get symbol indexes
  uint32_t l_list[CSI_RS_MAX_SYMBOLS_SLOT];
  int      nof_l = csi_rs_location_get_l_list(&resource->resource_mapping, j, l_list);
  if (nof_l <= 0) {
    return SRSRAN_ERROR;
  }

  // Calculate Resource Block boundaries
  uint32_t rb_begin  = csi_rs_rb_begin(carrier, &resource->resource_mapping);
  uint32_t rb_end    = csi_rs_rb_end(carrier, &resource->resource_mapping);
  uint32_t rb_stride = csi_rs_rb_stride(&resource->resource_mapping);

  // Calculate ideal number of RE per symbol
  uint32_t nof_re = csi_rs_count(resource->resource_mapping.density, rb_end - rb_begin);

  // Accumulators
  float epre_acc = 0.0f;

  // Initialise measurement
  SRSRAN_MEM_ZERO(measure, csi_rs_zp_resource_measure_t, 1);

  // Iterate time symbols
  for (int l_idx = 0; l_idx < nof_l; l_idx++) {
    // Get symbol index
    uint32_t l = l_list[l_idx];

    // Temporal Least Square Estimates
    cf_t     temp[CSI_RS_MAX_SUBC_PRB * SRSRAN_MAX_PRB_NR];
    uint32_t count_re = 0;

    // Extract RE
    for (uint32_t n = rb_begin; n < rb_end; n += rb_stride) {
      for (uint32_t k_idx = 0; k_idx < nof_k; k_idx++) {
        // Calculate sub-carrier index k
        uint32_t k = SRSRAN_NRE * n + k_list[k_idx];

        temp[count_re++] = grid[l * SRSRAN_NRE * carrier->nof_prb + k];
      }
    }

    // Verify RE count matches the expected number of RE
    if (count_re == 0 || count_re != nof_re) {
      ERROR("Unmatched number of RE (%d != %d)", count_re, nof_re);
      return SRSRAN_ERROR;
    }

    // Compute EPRE
    epre_acc += srsran_vec_avg_power_cf(temp, count_re);
  }

  // Set measure fields
  measure->cri    = resource->id;
  measure->l0     = l_list[0];
  measure->epre   = epre_acc / (float)nof_l;
  measure->nof_re = nof_l * nof_re;

  return SRSRAN_SUCCESS;
}

static int csi_rs_zp_measure_set(const srsran_carrier_nr_t*    carrier,
                                 const srsran_slot_cfg_t*      slot_cfg,
                                 const srsran_csi_rs_zp_set_t* set,
                                 const cf_t*                   grid,
                                 csi_rs_zp_resource_measure_t  measurements[SRSRAN_PHCH_CFG_MAX_NOF_CSI_RS_PER_SET])
{
  uint32_t count = 0;

  // Iterate all resources in set
  for (uint32_t i = 0; i < set->count; i++) {
    // Skip resource
    if (!srsran_csi_rs_send(&set->data[i].periodicity, slot_cfg)) {
      continue;
    }

    // Perform measurement
    if (csi_rs_zp_measure_resource(carrier, slot_cfg, &set->data[i], grid, &measurements[count]) < SRSRAN_SUCCESS) {
      ERROR("Error measuring NZP-CSI-RS resource");
      return SRSRAN_ERROR;
    }
    count++;
  }

  return count;
}

int srsran_csi_rs_zp_measure_channel(const srsran_carrier_nr_t*         carrier,
                                     const srsran_slot_cfg_t*           slot_cfg,
                                     const srsran_csi_rs_zp_set_t*      set,
                                     const cf_t*                        grid,
                                     srsran_csi_channel_measurements_t* measure)
{
  // Verify inputs
  if (carrier == NULL || slot_cfg == NULL || set == NULL || grid == NULL || measure == NULL) {
    return SRSRAN_ERROR;
  }

  // Perform Measurements
  csi_rs_zp_resource_measure_t measurements[SRSRAN_PHCH_CFG_MAX_NOF_CSI_RS_PER_SET];
  int                          ret = csi_rs_zp_measure_set(carrier, slot_cfg, set, grid, measurements);

  // Return to prevent assigning negative values to count
  if (ret < SRSRAN_SUCCESS) {
    ERROR("Error performing measurements");
    return SRSRAN_ERROR;
  }
  uint32_t count = (uint32_t)ret;

  // No NZP-CSI-RS has been scheduled for this slot
  if (count == 0) {
    return 0;
  }

  // Average measurements
  float epre_sum = 0.0f;
  for (uint32_t i = 0; i < count; i++) {
    epre_sum += measurements[i].epre / (float)count;
  }

  // Set measurements
  measure->cri               = measurements[0].cri;
  measure->wideband_rsrp_dBm = NAN;
  measure->wideband_epre_dBm = srsran_convert_power_to_dB(epre_sum);
  measure->wideband_snr_db   = NAN;

  // Set other parameters
  measure->K_csi_rs  = count;
  measure->nof_ports = 1; // No other value is currently supported

  // Return the number of active resources for this slot
  return count;
}

uint32_t srsran_csi_rs_measure_info(const srsran_csi_trs_measurements_t* measure, char* str, uint32_t str_len)
{
  uint32_t len = 0;

  len = srsran_print_check(str,
                           str_len,
                           len,
                           "rsrp=%+.1f epre=%+.1f n0=%+.1f snr=%+.1f delay_us=%+.1f ",
                           measure->rsrp_dB,
                           measure->epre_dB,
                           measure->n0_dB,
                           measure->snr_dB,
                           measure->delay_us);

  // Append measured CFO and the maximum CFO that can be measured
  if (isnormal(measure->cfo_hz_max)) {
    len = srsran_print_check(str, str_len, len, "cfo_hz=%+.1f cfo_hz_max=%+.1f", measure->cfo_hz, measure->cfo_hz_max);
  }

  return len;
}
