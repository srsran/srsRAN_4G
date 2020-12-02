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

#include "srslte/phy/ch_estimation/csi_rs.h"
#include "srslte/phy/common/sequence.h"
#include "srslte/phy/utils/vector.h"
#include <complex.h>
#include <stdbool.h>
#include <stdint.h>

#define SRSLTE_CSI_RS_NOF_FREQ_DOMAIN_ALLOC_ROW1 4
#define SRSLTE_CSI_RS_NOF_FREQ_DOMAIN_ALLOC_ROW2 12
#define SRSLTE_CSI_RS_NOF_FREQ_DOMAIN_ALLOC_ROW4 3
#define SRSLTE_CSI_RS_NOF_FREQ_DOMAIN_ALLOC_OTHER 6
#define SRSLTE_CSI_RS_NOF_FREQ_DOMAIN_ALLOC_MAX 12

#define CSI_RS_MAX_CDM_GROUP 16

static int csi_rs_location_f(const srslte_csi_rs_resource_mapping_t* resource, uint32_t i)
{
  uint32_t count           = 0;
  uint32_t nof_freq_domain = 0;
  switch (resource->row) {
    case srslte_csi_rs_resource_mapping_row_1:
      nof_freq_domain = SRSLTE_CSI_RS_NOF_FREQ_DOMAIN_ALLOC_ROW1;
      break;
    case srslte_csi_rs_resource_mapping_row_2:
      nof_freq_domain = SRSLTE_CSI_RS_NOF_FREQ_DOMAIN_ALLOC_ROW2;
      break;
    case srslte_csi_rs_resource_mapping_row_4:
      nof_freq_domain = SRSLTE_CSI_RS_NOF_FREQ_DOMAIN_ALLOC_ROW4;
      break;
    case srslte_csi_rs_resource_mapping_row_other:
      nof_freq_domain = SRSLTE_CSI_RS_NOF_FREQ_DOMAIN_ALLOC_OTHER;
      break;
  }

  for (uint32_t j = 0; j < nof_freq_domain; j++) {
    if (resource->frequency_domain_alloc[j]) {
      count++;
    }

    if (count == i) {
      return i;
    }
  }

  return SRSLTE_ERROR;
}

// Table 7.4.1.5.3-1: CSI-RS locations within a slot
static int csi_rs_location_get_k_list(const srslte_csi_rs_resource_mapping_t* resource,
                                      uint32_t                                k_list[CSI_RS_MAX_CDM_GROUP])
{
  int k0 = csi_rs_location_f(resource, 1);
  //  int k1 = csi_rs_location_f(resource, 2);
  //  int k2 = csi_rs_location_f(resource, 3);
  //  int k3 = csi_rs_location_f(resource, 4);

  if (k0 < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  // Row 1
  if (resource->row == srslte_csi_rs_resource_mapping_row_1 && resource->ports == 1 &&
      resource->density == srslte_csi_rs_resource_mapping_density_three && resource->cdm == srslte_csi_rs_cdm_nocdm) {
    k_list[0] = k0;
    k_list[1] = k0 + 4;
    k_list[3] = k0 + 8;
    return 3;
  }

  return SRSLTE_ERROR;
}

// Table 7.4.1.5.3-1: CSI-RS locations within a slot
static int csi_rs_location_get_l_list(const srslte_csi_rs_resource_mapping_t* resource,
                                      uint32_t                                l_list[CSI_RS_MAX_CDM_GROUP])
{
  uint32_t l0 = resource->first_symbol_idx;

  if (l0 > 13) {
    return SRSLTE_ERROR;
  }

  //  uint32_t l1 = resource->first_symbol_idx2;
  //  if (l1 < 2 || l1 > 12) {
  //    return SRSLTE_ERROR;
  //  }

  // Row 1
  if (resource->row == srslte_csi_rs_resource_mapping_row_1 && resource->ports == 1 &&
      resource->density == srslte_csi_rs_resource_mapping_density_three && resource->cdm == srslte_csi_rs_cdm_nocdm) {
    l_list[0] = l0;
    return 1;
  }

  return SRSLTE_ERROR;
}

uint32_t csi_rs_cinit(const srslte_carrier_nr_t*          carrier,
                      const srslte_dl_slot_cfg_t*         slot_cfg,
                      const srslte_csi_rs_nzp_resource_t* resource,
                      uint32_t                            l)
{
  uint32_t n    = slot_cfg->idx % SRSLTE_NSLOTS_PER_FRAME_NR(carrier->numerology);
  uint32_t n_id = resource->scrambling_id;

  return ((SRSLTE_NSYMB_PER_SLOT_NR * n + l + 1UL) * (2UL * n_id) << 10UL) + n_id;
}

bool srslte_csi_send(const srslte_csi_rs_period_and_offset_t* periodicity, const srslte_dl_slot_cfg_t* slot_cfg)
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

uint32_t csi_rs_count(srslte_csi_rs_density_t density, uint32_t nprb)
{
  switch (density) {

    case srslte_csi_rs_resource_mapping_density_three:
      return nprb * 3;
    case srslte_csi_rs_resource_mapping_density_dot5_even:
      return nprb / 2;
    case srslte_csi_rs_resource_mapping_density_dot5_odd:
      return nprb / 2;
    case srslte_csi_rs_resource_mapping_density_one:
      return nprb;
    case srslte_csi_rs_resource_mapping_density_spare:
    default:; // Do nothing
  }
  return 0;
}

int srslte_csi_rs_nzp_put(const srslte_carrier_nr_t*          carrier,
                          const srslte_dl_slot_cfg_t*         slot_cfg,
                          const srslte_csi_rs_nzp_resource_t* resource,
                          cf_t*                               grid)
{
  if (carrier == NULL || resource == NULL || grid == NULL) {
    return SRSLTE_ERROR;
  }

  uint32_t k_list[CSI_RS_MAX_CDM_GROUP];
  int      nof_k = csi_rs_location_get_k_list(&resource->resource_mapping, k_list);
  if (nof_k <= 0) {
    return SRSLTE_ERROR;
  }

  uint32_t l_list[CSI_RS_MAX_CDM_GROUP];
  int      nof_l = csi_rs_location_get_l_list(&resource->resource_mapping, l_list);
  if (nof_l <= 0) {
    return SRSLTE_ERROR;
  }

  // Calculate Resource Block boundaries
  uint32_t rb_begin  = resource->resource_mapping.freq_band.start_rb;
  uint32_t rb_end    = resource->resource_mapping.freq_band.start_rb + resource->resource_mapping.freq_band.nof_rb;
  uint32_t rb_stride = 1;

  // Calculate power allocation
  float beta = srslte_convert_dB_to_amplitude((float)resource->power_control_offset);
  if (!isnormal(beta)) {
    beta = 1.0f;
  }

  // Special .5 density cases
  if (resource->resource_mapping.density == srslte_csi_rs_resource_mapping_density_dot5_even ||
      resource->resource_mapping.density == srslte_csi_rs_resource_mapping_density_dot5_odd) {
    // Increase the start by one if:
    // - Even and starts with odd
    // - Odd and starts with even
    if ((resource->resource_mapping.density == srslte_csi_rs_resource_mapping_density_dot5_even && rb_begin % 2 == 1) ||
        (resource->resource_mapping.density == srslte_csi_rs_resource_mapping_density_dot5_odd && rb_begin % 2 == 0)) {
      rb_begin++;
    }

    // Skip one RB
    rb_stride = 2;
  }

  for (int l_idx = 0; l_idx < nof_l; l_idx++) {
    // Get symbol index
    uint32_t l = l_list[l_idx];

    // Initialise sequence for this OFDM symbol
    uint32_t                cinit          = csi_rs_cinit(carrier, slot_cfg, resource, l);
    srslte_sequence_state_t sequence_state = {};
    srslte_sequence_state_init(&sequence_state, cinit);

    // Skip unallocated RB
    srslte_sequence_state_advance(&sequence_state, 2 * csi_rs_count(resource->resource_mapping.density, rb_begin));

    // Temporal R sequence
    cf_t     r[64];
    uint32_t r_idx = 64;

    // Iterate over frequency domain
    for (uint32_t n = rb_begin; n < rb_end; n += rb_stride) {
      for (uint32_t k_idx = 0; k_idx < nof_k; k_idx++) {
        // Calculate sub-carrier index k
        uint32_t k = SRSLTE_NRE * n + k_list[k_idx];

        // Do we need more r?
        if (r_idx >= 64) {
          // ... Generate a bunch of it!
          srslte_sequence_state_gen_f(&sequence_state, M_SQRT1_2 * beta, (float*)r, 64 * 2);
          r_idx = 0;
        }

        // Put CSI in grid
        grid[l * SRSLTE_NRE * carrier->nof_prb + k] = r[r_idx++];
      }
    }
  }

  return SRSLTE_SUCCESS;
}

int srslte_csi_rs_nzp_measure(const srslte_carrier_nr_t*          carrier,
                              const srslte_dl_slot_cfg_t*         slot_cfg,
                              const srslte_csi_rs_nzp_resource_t* resource,
                              const cf_t*                         grid,
                              srslte_csi_rs_measure_t*            measure)
{
  if (carrier == NULL || resource == NULL || grid == NULL) {
    return SRSLTE_ERROR;
  }

  uint32_t k_list[CSI_RS_MAX_CDM_GROUP];
  int      nof_k = csi_rs_location_get_k_list(&resource->resource_mapping, k_list);
  if (nof_k <= 0) {
    return SRSLTE_ERROR;
  }

  uint32_t l_list[CSI_RS_MAX_CDM_GROUP];
  int      nof_l = csi_rs_location_get_l_list(&resource->resource_mapping, l_list);
  if (nof_l <= 0) {
    return SRSLTE_ERROR;
  }

  // Calculate Resource Block boundaries
  uint32_t rb_begin  = resource->resource_mapping.freq_band.start_rb;
  uint32_t rb_end    = resource->resource_mapping.freq_band.start_rb + resource->resource_mapping.freq_band.nof_rb;
  uint32_t rb_stride = 1;

  // Calculate power allocation
  float beta = srslte_convert_dB_to_amplitude((float)resource->power_control_offset);
  if (!isnormal(beta)) {
    beta = 1.0f;
  }

  // Special .5 density cases
  if (resource->resource_mapping.density == srslte_csi_rs_resource_mapping_density_dot5_even ||
      resource->resource_mapping.density == srslte_csi_rs_resource_mapping_density_dot5_odd) {
    // Increase the start by one if:
    // - Even and starts with odd
    // - Odd and starts with even
    if ((resource->resource_mapping.density == srslte_csi_rs_resource_mapping_density_dot5_even && rb_begin % 2 == 1) ||
        (resource->resource_mapping.density == srslte_csi_rs_resource_mapping_density_dot5_odd && rb_begin % 2 == 0)) {
      rb_begin++;
    }

    // Skip one RB
    rb_stride = 2;
  }

  // Accumulators
  float    epre_acc = 0.0f;
  cf_t     rsrp_acc = 0.0f;
  uint32_t count    = 0;

  for (int l_idx = 0; l_idx < nof_l; l_idx++) {
    // Get symbol index
    uint32_t l = l_list[l_idx];

    // Initialise sequence for this OFDM symbol
    uint32_t                cinit          = csi_rs_cinit(carrier, slot_cfg, resource, l);
    srslte_sequence_state_t sequence_state = {};
    srslte_sequence_state_init(&sequence_state, cinit);

    // Skip unallocated RB
    srslte_sequence_state_advance(&sequence_state, 2 * csi_rs_count(resource->resource_mapping.density, rb_begin));

    // Temporal R sequence
    cf_t     r[64];
    uint32_t r_idx = 64;

    // Iterate over frequency domain
    for (uint32_t n = rb_begin; n < rb_end; n += rb_stride) {
      for (uint32_t k_idx = 0; k_idx < nof_k; k_idx++) {
        // Calculate sub-carrier index k
        uint32_t k = SRSLTE_NRE * n + k_list[k_idx];

        // Do we need more r?
        if (r_idx >= 64) {
          // ... Generate a bunch of it!
          srslte_sequence_state_gen_f(&sequence_state, M_SQRT1_2 / beta, (float*)r, 64 * 2);
          r_idx = 0;
        }

        // Take CSI-RS from grid and measure
        cf_t tmp = grid[l * SRSLTE_NRE * carrier->nof_prb + k] * conjf(r[r_idx++]);
        rsrp_acc += tmp;
        epre_acc = __real__ tmp * __real__ tmp + __imag__ tmp * __imag__ tmp;
        count++;
      }
    }
  }

  if (count) {
    measure->epre = epre_acc / (float)count;
    measure->rsrp = cabsf(rsrp_acc) / (float)count;
  }

  return SRSLTE_SUCCESS;
}