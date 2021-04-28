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

#include "srsran/phy/common/phy_common_nr.h"
#include "srsran/phy/utils/vector.h"
#include <string.h>

const char* srsran_rnti_type_str(srsran_rnti_type_t rnti_type)
{
  switch (rnti_type) {
    case srsran_rnti_type_c:
      return "C-RNTI";
    case srsran_rnti_type_p:
      return "P-RNTI";
    case srsran_rnti_type_si:
      return "SI-RNTI";
    case srsran_rnti_type_ra:
      return "RA-RNTI";
    case srsran_rnti_type_tc:
      return "TC-RNTI";
    case srsran_rnti_type_cs:
      return "CS-RNTI";
    case srsran_rnti_type_sp_csi:
      return "SP-CSI-RNTI";
    case srsran_rnti_type_mcs_c:
      return "MCS-C-RNTI";
    default:; // Do nothing
  }
  return "unknown";
}

const char* srsran_dci_format_nr_string(srsran_dci_format_nr_t format)
{
  switch (format) {
    case srsran_dci_format_nr_0_0:
      return "0_0";
    case srsran_dci_format_nr_0_1:
      return "0_1";
    case srsran_dci_format_nr_1_0:
      return "1_0";
    case srsran_dci_format_nr_1_1:
      return "1_1";
    case srsran_dci_format_nr_2_0:
      return "2_0";
    case srsran_dci_format_nr_2_1:
      return "2_1";
    case srsran_dci_format_nr_2_2:
      return "2_2";
    case srsran_dci_format_nr_2_3:
      return "2_3";
    case srsran_dci_format_nr_rar:
      return "RAR";
    case srsran_dci_format_nr_cg:
      return "CG";
    default:
    case SRSRAN_DCI_FORMAT_NR_COUNT:
      break;
  }
  return "unknown";
}

uint32_t srsran_coreset_get_bw(const srsran_coreset_t* coreset)
{
  uint32_t prb_count = 0;

  // Iterate all the frequency domain resources bit-map...
  for (uint32_t i = 0; i < SRSRAN_CORESET_FREQ_DOMAIN_RES_SIZE; i++) {
    // ... and count 6 PRB for every frequency domain resource that it is enabled
    if (coreset->freq_resources[i]) {
      prb_count += 6;
    }
  }

  // Return the total count of physical resource blocks
  return prb_count;
}

uint32_t srsran_coreset_get_sz(const srsran_coreset_t* coreset)
{
  // Returns the number of resource elements in time and frequency domains
  return srsran_coreset_get_bw(coreset) * SRSRAN_NRE * coreset->duration;
}

const char* srsran_sch_mapping_type_to_str(srsran_sch_mapping_type_t mapping_type)
{
  switch (mapping_type) {
    case srsran_sch_mapping_type_A:
      return "A";
    case srsran_sch_mapping_type_B:
      return "B";
    default:
      return "undefined";
  }
}

const char* srsran_mcs_table_to_str(srsran_mcs_table_t mcs_table)
{
  switch (mcs_table) {
    case srsran_mcs_table_64qam:
      return "64qam";
    case srsran_mcs_table_256qam:
      return "256qam";
    case srsran_mcs_table_qam64LowSE:
      return "qam64LowSE";
    default:
      return "undefined";
  }
}

srsran_mcs_table_t srsran_mcs_table_from_str(const char* str)
{
  if (strcmp(str, "64qam") == 0) {
    return srsran_mcs_table_64qam;
  }
  if (strcmp(str, "256qam") == 0) {
    return srsran_mcs_table_256qam;
  }
  if (strcmp(str, "qam64LowSE") == 0) {
    return srsran_mcs_table_qam64LowSE;
  }
  return srsran_mcs_table_N;
}

#define PHY_COMMON_NR_NOF_VALID_SYMB_SZ 10U

static const uint32_t phy_common_nr_valid_symbol_sz[PHY_COMMON_NR_NOF_VALID_SYMB_SZ] =
    {128, 256, 384, 512, 768, 1024, 1536, 2048, 3072, 4096};

uint32_t srsran_min_symbol_sz_rb(uint32_t nof_prb)
{
  uint32_t nof_re = nof_prb * SRSRAN_NRE;

  if (nof_re == 0) {
    return 0;
  }

  for (uint32_t i = 0; i < PHY_COMMON_NR_NOF_VALID_SYMB_SZ; i++) {
    if (phy_common_nr_valid_symbol_sz[i] > nof_re) {
      return phy_common_nr_valid_symbol_sz[i];
    }
  }

  return 0;
}

float srsran_symbol_distance_s(uint32_t l0, uint32_t l1, uint32_t numerology)
{
  // l0 must be smaller than l1
  if (l0 >= l1) {
    return 0.0f;
  }

  // Count number of symbols in between
  uint32_t count = l1 - l0;

  // Compute at what symbol there is a longer CP
  uint32_t cp_boundary = 7U << numerology;

  // Select whether extra CP shall be added
  uint32_t extra_cp = 0;
  if (l0 < cp_boundary && l1 >= cp_boundary) {
    extra_cp = 16;
  }

  // Compute reference FFT size
  uint32_t N = (2048 + 144) * count + extra_cp;

  // Return symbol distance in microseconds
  return (N << numerology) * SRSRAN_LTE_TS;
}

bool srsran_tdd_nr_is_dl(const srsran_tdd_config_nr_t* cfg, uint32_t numerology, uint32_t slot_idx)
{
  // Protect NULL pointer access
  if (cfg == NULL) {
    return false;
  }

  // Prevent zero division
  if (cfg->pattern1.period_ms == 0 && cfg->pattern2.period_ms == 0) {
    return false;
  }

  // Calculate slot index within the TDD overall period
  uint32_t slot_x_ms       = 1U << numerology; // Number of slots per millisecond
  uint32_t period_sum      = (cfg->pattern1.period_ms + cfg->pattern2.period_ms) * slot_x_ms; // Total perdiod sum
  uint32_t slot_idx_period = slot_idx % period_sum; // Slot index within the period

  // Select pattern
  const srsran_tdd_pattern_t* pattern = &cfg->pattern1;
  if ((slot_idx_period >= cfg->pattern1.period_ms * slot_x_ms)) {
    pattern = &cfg->pattern2;
    slot_idx_period -= cfg->pattern1.period_ms * slot_x_ms; // Remove pattern 1 offset
  }

  // Check DL boundaries
  return (slot_idx_period < pattern->nof_dl_slots ||
          (slot_idx_period == pattern->nof_dl_slots && pattern->nof_dl_symbols != 0));
}

bool srsran_tdd_nr_is_ul(const srsran_tdd_config_nr_t* cfg, uint32_t numerology, uint32_t slot_idx)
{
  // Protect NULL pointer access
  if (cfg == NULL) {
    return false;
  }

  // Prevent zero division
  if (cfg->pattern1.period_ms == 0 && cfg->pattern2.period_ms == 0) {
    return false;
  }

  // Calculate slot index within the TDD overall period
  uint32_t slot_x_ms       = 1U << numerology; // Number of slots per millisecond
  uint32_t period_sum      = (cfg->pattern1.period_ms + cfg->pattern2.period_ms) * slot_x_ms; // Total perdiod sum
  uint32_t slot_idx_period = slot_idx % period_sum; // Slot index within the period

  // Select pattern
  const srsran_tdd_pattern_t* pattern = &cfg->pattern1;
  if ((slot_idx_period >= cfg->pattern1.period_ms * slot_x_ms)) {
    pattern = &cfg->pattern2;
    slot_idx_period -= cfg->pattern1.period_ms * slot_x_ms; // Remove pattern 1 offset
  }

  // Calculate slot in which UL starts
  uint32_t start_ul = (pattern->period_ms * slot_x_ms - pattern->nof_ul_slots) - 1;

  // Check UL boundaries
  return (slot_idx_period > start_ul || (slot_idx_period == start_ul && pattern->nof_ul_symbols != 0));
}

int srsran_carrier_to_cell(const srsran_carrier_nr_t* carrier, srsran_cell_t* cell)
{
  // Protect memory access
  if (carrier == NULL || cell == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Ensure cell is initialised+
  SRSRAN_MEM_ZERO(cell, srsran_cell_t, 1);

  // Select number of PRB
  if (carrier->nof_prb <= 25) {
    cell->nof_prb = 25;
  } else if (carrier->nof_prb <= 52) {
    cell->nof_prb = 50;
  } else if (carrier->nof_prb <= 79) {
    cell->nof_prb = 75;
  } else if (carrier->nof_prb <= 106) {
    cell->nof_prb = 100;
  } else {
    return SRSRAN_ERROR_OUT_OF_BOUNDS;
  }

  // Set other parameters
  cell->id        = carrier->pci;
  cell->nof_ports = carrier->max_mimo_layers;

  return SRSRAN_SUCCESS;
}
