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
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/vector.h"
#include <stdlib.h>
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
const char* srsran_rnti_type_str_short(srsran_rnti_type_t rnti_type)
{
  switch (rnti_type) {
    case srsran_rnti_type_c:
      return "c";
    case srsran_rnti_type_p:
      return "p";
    case srsran_rnti_type_si:
      return "si";
    case srsran_rnti_type_ra:
      return "ra";
    case srsran_rnti_type_tc:
      return "tc";
    case srsran_rnti_type_cs:
      return "cs";
    case srsran_rnti_type_sp_csi:
      return "sp-csi";
    case srsran_rnti_type_mcs_c:
      return "mcs-c";
    default:; // Do nothing
  }
  return "unknown";
}

const char* srsran_ss_type_str(srsran_search_space_type_t ss_type)
{
  switch (ss_type) {
    case srsran_search_space_type_common_0:
      return "common0";
    case srsran_search_space_type_common_0A:
      return "common0A";
    case srsran_search_space_type_common_1:
      return "common1";
    case srsran_search_space_type_common_2:
      return "common2";
    case srsran_search_space_type_common_3:
      return "common3";
    case srsran_search_space_type_ue:
      return "ue";
    case srsran_search_space_type_rar:
      return "rar";
    case srsran_search_space_type_cg:
      return "cg";
    default:; // Do nothing
      break;
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

uint32_t srsran_coreset_start_rb(const srsran_coreset_t* coreset)
{
  // Protect CORESET access
  if (coreset == NULL) {
    return 0;
  }

  // Iterates all the possible frequency resources trying to find the first enabled
  for (uint32_t res = 0; res < SRSRAN_CORESET_FREQ_DOMAIN_RES_SIZE; res++) {
    // If the frequency resource is enabled...
    if (coreset->freq_resources[res]) {
      // ... return the lowest resource block index
      return 6 * res + coreset->offset_rb;
    }
  }

  // Returns the start resource index
  return 0;
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

static const uint32_t phy_common_nr_valid_std_symbol_sz[PHY_COMMON_NR_NOF_VALID_SYMB_SZ] =
    {128, 256, 512, 1024, 1536, 2048, 4096};

uint32_t srsran_min_symbol_sz_rb(uint32_t nof_prb)
{
  uint32_t nof_re = nof_prb * SRSRAN_NRE;

  if (nof_re == 0) {
    return 0;
  }

  // Select all valid symbol sizes by default
  const uint32_t* symbol_table = phy_common_nr_valid_symbol_sz;

  // Select standard LTE symbol sizes table
  if (srsran_symbol_size_is_standard()) {
    symbol_table = phy_common_nr_valid_std_symbol_sz;

    // As the selected symbol size is the minimum that fits the entire number of RE, 106 RB would select into 1536 point
    // symbol size. However, it shall use 2048 point symbol size to match LTE standard rate. Because of this, it forces
    // bandwidths bigger than 79 RB that would use 1536 symbol size to select 2048.
    if (nof_prb > 79 && nof_prb < 1536 / SRSRAN_NRE) {
      return 2048;
    }
  }

  // For each symbol size in the table...
  for (uint32_t i = 0; i < PHY_COMMON_NR_NOF_VALID_SYMB_SZ; i++) {
    // Check if the number of RE fit in the symbol
    if (symbol_table[i] > nof_re) {
      // Returns the smallest symbol size that fits the number of RE
      return symbol_table[i];
    }
  }

  // The number of RE exceeds the maximum symbol size
  return 0;
}

int srsran_symbol_sz_from_srate(double srate_hz, srsran_subcarrier_spacing_t scs)
{
  // Make sure srate is valid
  if (!isnormal(srate_hz) || srate_hz < 0.0) {
    return SRSRAN_ERROR;
  }

  // Convert srate to integer and Hz
  uint32_t srate_int_hz = (uint32_t)srate_hz;

  // Get subcarrier spacing in Hz
  uint32_t scs_int_hz = SRSRAN_SUBC_SPACING_NR(scs);

  // Check the symbol size if a integer
  if (srate_int_hz % scs_int_hz != 0) {
    ERROR("Invalid sampling rate %.2f MHz with subcarrrier spacing %d kHz", srate_hz / 1e6, scs_int_hz / 1000);
    return SRSRAN_ERROR;
  }

  // Calculate symbol size in samples
  uint32_t symbol_sz = srate_int_hz / scs_int_hz;

  // Verify the symbol size can have an integer cyclic prefix size
  if ((symbol_sz * 144U) % 2048 != 0 && (symbol_sz * (16U << (uint32_t)scs)) % 2048 != 0) {
    ERROR("The sampling rate %.2f MHz with subcarrrier spacing %d kHz", srate_hz / 1e6, scs_int_hz / 1000);
    return SRSRAN_ERROR;
  }

  return (int)symbol_sz;
}

float srsran_symbol_offset_s(uint32_t l, srsran_subcarrier_spacing_t scs)
{
  // Compute at what symbol there is a longer CP
  uint32_t cp_boundary = SRSRAN_EXT_CP_SYMBOL(scs);

  // Number of samples (DFT + short CP) in between the first and l symbols
  uint32_t N = ((2048 + 144) * l) >> (uint32_t)scs;

  // Add extended CP samples from first OFDM symbol
  N += 16 * SRSRAN_CEIL(l, cp_boundary);

  // Compute time using reference sampling rate
  float TS = SRSRAN_LTE_TS;

  // Return symbol offset in seconds
  return (float)N * TS;
}

float srsran_symbol_distance_s(uint32_t l0, uint32_t l1, srsran_subcarrier_spacing_t scs)
{
  // l0 must be smaller than l1
  if (l0 >= l1) {
    return 0.0f;
  }

  // Return symbol distance in seconds
  return srsran_symbol_offset_s(l1, scs) - srsran_symbol_offset_s(l0, scs);
}

static bool tdd_nr_is_dl(const srsran_tdd_config_nr_t* cfg, uint32_t numerology, uint32_t slot_idx)
{
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

bool srsran_duplex_nr_is_dl(const srsran_duplex_config_nr_t* cfg, uint32_t numerology, uint32_t slot_idx)

{
  // Protect NULL pointer access
  if (cfg == NULL) {
    return false;
  }

  // In case of TDD
  if (cfg->mode == SRSRAN_DUPLEX_MODE_TDD) {
    return tdd_nr_is_dl(&cfg->tdd, numerology, slot_idx);
  }

  return true;
}

static bool tdd_nr_is_ul(const srsran_tdd_config_nr_t* cfg, uint32_t numerology, uint32_t slot_idx)
{
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

bool srsran_duplex_nr_is_ul(const srsran_duplex_config_nr_t* cfg, uint32_t numerology, uint32_t slot_idx)
{
  // Protect NULL pointer access
  if (cfg == NULL) {
    return false;
  }

  // In case of TDD
  if (cfg->mode == SRSRAN_DUPLEX_MODE_TDD) {
    return tdd_nr_is_ul(&cfg->tdd, numerology, slot_idx);
  }

  return true;
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

uint32_t srsran_csi_meas_info(const srsran_csi_trs_measurements_t* meas, char* str, uint32_t str_len)
{
  if (meas == NULL || str == NULL || str_len == 0) {
    return 0;
  }

  return srsran_print_check(str,
                            str_len,
                            0,
                            "rsrp=%+.1f epre=%+.1f n0=%+.1f snr=%+.1f cfo=%+.1f cfo_max=%.0f delay=%+.1f",
                            meas->rsrp_dB,
                            meas->epre_dB,
                            meas->n0_dB,
                            meas->snr_dB,
                            meas->cfo_hz,
                            meas->cfo_hz_max,
                            meas->delay_us);
}

uint32_t srsran_csi_meas_info_short(const srsran_csi_trs_measurements_t* meas, char* str, uint32_t str_len)
{
  if (meas == NULL || str == NULL || str_len == 0) {
    return 0;
  }

  return srsran_print_check(str,
                            str_len,
                            0,
                            "epre=%+.1f snr=%+.1f cfo=%+.1f delay=%+.1f ",
                            meas->epre_dB,
                            meas->snr_dB,
                            meas->cfo_hz,
                            meas->delay_us);
}

srsran_subcarrier_spacing_t srsran_subcarrier_spacing_from_str(const char* str)
{
  if (str == NULL) {
    return srsran_subcarrier_spacing_invalid;
  }

  uint32_t scs = (uint32_t)roundf(strtof(str, NULL));
  switch (scs) {
    case 15:
    case 15000:
      return srsran_subcarrier_spacing_15kHz;
    case 30:
    case 30000:
      return srsran_subcarrier_spacing_30kHz;
    case 60:
    case 60000:
      return srsran_subcarrier_spacing_60kHz;
    case 120:
    case 120000:
      return srsran_subcarrier_spacing_120kHz;
    case 240:
    case 240000:
      return srsran_subcarrier_spacing_240kHz;
    default:; // Do nothing
  }

  return srsran_subcarrier_spacing_invalid;
}

const char* srsran_subcarrier_spacing_to_str(srsran_subcarrier_spacing_t scs)
{
  switch (scs) {
    case srsran_subcarrier_spacing_15kHz:
      return "15kHz";
    case srsran_subcarrier_spacing_30kHz:
      return "30kHz";
    case srsran_subcarrier_spacing_60kHz:
      return "60kHz";
    case srsran_subcarrier_spacing_120kHz:
      return "120kHz";
    case srsran_subcarrier_spacing_240kHz:
      return "240kHz";
    case srsran_subcarrier_spacing_invalid:
    default:
      return "invalid";
  }
}

void srsran_combine_csi_trs_measurements(const srsran_csi_trs_measurements_t* a,
                                         const srsran_csi_trs_measurements_t* b,
                                         srsran_csi_trs_measurements_t*       dst)
{
  // Verify inputs
  if (a == NULL || b == NULL || dst == NULL) {
    return;
  }

  // Protect from zero division
  uint32_t nof_re_sum = a->nof_re + b->nof_re;
  if (nof_re_sum == 0) {
    SRSRAN_MEM_ZERO(dst, srsran_csi_trs_measurements_t, 1);
    return;
  }

  // Perform proportional average
  dst->rsrp       = SRSRAN_VEC_PMA(a->rsrp, a->nof_re, b->rsrp, b->nof_re);
  dst->rsrp_dB    = SRSRAN_VEC_PMA(a->rsrp_dB, a->nof_re, b->rsrp_dB, b->nof_re);
  dst->epre       = SRSRAN_VEC_PMA(a->epre, a->nof_re, b->epre, b->nof_re);
  dst->epre_dB    = SRSRAN_VEC_PMA(a->epre_dB, a->nof_re, b->epre_dB, b->nof_re);
  dst->n0         = SRSRAN_VEC_PMA(a->n0, a->nof_re, b->n0, b->nof_re);
  dst->n0_dB      = SRSRAN_VEC_PMA(a->n0_dB, a->nof_re, b->n0_dB, b->nof_re);
  dst->snr_dB     = SRSRAN_VEC_PMA(a->snr_dB, a->nof_re, b->snr_dB, b->nof_re);
  dst->cfo_hz     = SRSRAN_VEC_PMA(a->cfo_hz, a->nof_re, b->cfo_hz, b->nof_re);
  dst->cfo_hz_max = SRSRAN_MAX(a->cfo_hz_max, b->cfo_hz_max);
  dst->delay_us   = SRSRAN_VEC_PMA(a->delay_us, a->nof_re, b->delay_us, b->nof_re);
  dst->nof_re     = nof_re_sum;
}

typedef struct {
  uint32_t mux_pattern;
  uint32_t nof_prb;
  uint32_t nof_symb;
  uint32_t offset_rb; ///< Defined by TS 36.213 section 13 UE procedure for monitoring Type0-PDCCH CSS sets:
  ///< Offset respect to the SCS of the CORESET for Type0-PDCCH CSS set, provided by
  ///< subCarrierSpacingCommon, from the smallest RB index of the CORESET for Type0-PDCCH CSS set
  ///< to the smallest RB index of the common RB overlapping with the first RB of the
  ///< corresponding SS/PBCH block.
} coreset_zero_entry_t;

static const coreset_zero_entry_t coreset_zero_15_15[16] = {
    {1, 24, 2, 0},
    {1, 24, 2, 2},
    {1, 24, 2, 4},
    {1, 24, 3, 0},
    {1, 24, 3, 2},
    {1, 24, 3, 4},
    {1, 48, 1, 12},
    {1, 48, 1, 16},
    {1, 48, 2, 12},
    {1, 48, 2, 16},
    {1, 48, 3, 12},
    {1, 48, 3, 16},
    {1, 96, 1, 38},
    {1, 96, 2, 38},
    {1, 96, 3, 38},
    {},
};

static const coreset_zero_entry_t coreset_zero_15_30[16] = {
    {1, 24, 2, 5},
    {1, 24, 2, 6},
    {1, 24, 2, 7},
    {1, 24, 2, 8},
    {1, 24, 3, 5},
    {1, 24, 3, 6},
    {1, 24, 3, 7},
    {1, 24, 3, 8},
    {1, 48, 1, 18},
    {1, 48, 1, 20},
    {1, 48, 2, 18},
    {1, 48, 2, 20},
    {1, 48, 3, 18},
    {1, 48, 3, 20},
    {},
    {},
};

static const coreset_zero_entry_t coreset_zero_30_15[16] = {
    {1, 48, 1, 2},
    {1, 48, 1, 6},
    {1, 48, 2, 2},
    {1, 48, 2, 6},
    {1, 48, 3, 2},
    {1, 48, 3, 6},
    {1, 96, 1, 28},
    {1, 96, 2, 28},
    {1, 96, 3, 28},
    {},
    {},
    {},
    {},
    {},
    {},
    {},
};

int srsran_coreset_zero(uint32_t                    n_cell_id,
                        uint32_t                    ssb_pointA_freq_offset_Hz,
                        srsran_subcarrier_spacing_t ssb_scs,
                        srsran_subcarrier_spacing_t pdcch_scs,
                        uint32_t                    idx,
                        srsran_coreset_t*           coreset)
{
  // Verify inputs
  if (coreset == NULL || idx >= 16) {
    ERROR("Invalid CORESET Zero inputs. coreset=%p, idx=%d", coreset, idx);
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Default entry to NULL
  const coreset_zero_entry_t* entry = NULL;

  // Table 13-1: Set of resource blocks and slot symbols of CORESET for Type0-PDCCH search space set
  // when {SS/PBCH block, PDCCH} SCS is {15, 15} kHz for frequency bands with minimum channel
  // bandwidth 5 MHz or 10 MHz
  if (ssb_scs == srsran_subcarrier_spacing_15kHz && pdcch_scs == srsran_subcarrier_spacing_15kHz) {
    entry = &coreset_zero_15_15[idx];
  }
  // Table 13-2: Set of resource blocks and slot symbols of CORESET for Type0-PDCCH search space set
  // when {SS/PBCH block, PDCCH} SCS is {15, 30} kHz for frequency bands with minimum channel
  // bandwidth 5 MHz or 10 MHz
  if (ssb_scs == srsran_subcarrier_spacing_15kHz && pdcch_scs == srsran_subcarrier_spacing_30kHz) {
    entry = &coreset_zero_15_30[idx];
  }

  // Table 13-3: Set of resource blocks and slot symbols of CORESET for Type0-PDCCH search space set
  // when {SS/PBCH block, PDCCH} SCS is {30, 15} kHz for frequency bands with minimum channel
  // bandwidth 5 MHz or 10 MHz
  if (ssb_scs == srsran_subcarrier_spacing_30kHz && pdcch_scs == srsran_subcarrier_spacing_15kHz) {
    entry = &coreset_zero_30_15[idx];
  }

  // Check a valid entry has been selected
  if (entry == NULL) {
    ERROR("Unhandled case ssb_scs=%s, pdcch_scs=%s",
          srsran_subcarrier_spacing_to_str(ssb_scs),
          srsran_subcarrier_spacing_to_str(pdcch_scs));
    return SRSRAN_ERROR;
  }

  if (entry->nof_prb == 0) {
    ERROR("Reserved case ssb_scs=%s, pdcch_scs=%s, idx=%d",
          srsran_subcarrier_spacing_to_str(ssb_scs),
          srsran_subcarrier_spacing_to_str(pdcch_scs),
          idx);
    return SRSRAN_ERROR;
  }

  // Calculate CORESET offset in RB
  uint32_t ssb_half_bw_Hz = SRSRAN_SUBC_SPACING_NR(ssb_scs) * (SRSRAN_SSB_BW_SUBC / 2U);
  if (ssb_pointA_freq_offset_Hz > ssb_half_bw_Hz) {
    // Move SSB center to lowest SSB subcarrier
    ssb_pointA_freq_offset_Hz -= ssb_half_bw_Hz;
  } else {
    ssb_pointA_freq_offset_Hz = 0;
  }
  uint32_t ssb_pointA_freq_offset_rb =
      SRSRAN_FLOOR(ssb_pointA_freq_offset_Hz, SRSRAN_NRE * SRSRAN_SUBC_SPACING_NR(pdcch_scs));
  uint32_t offset_rb =
      (ssb_pointA_freq_offset_rb > entry->offset_rb) ? (ssb_pointA_freq_offset_rb - entry->offset_rb) : 0;

  // Set CORESET fields
  coreset->id                         = 0;
  coreset->dmrs_scrambling_id_present = false;
  coreset->duration                   = entry->nof_symb;
  coreset->offset_rb                  = offset_rb;

  // Set CCE-to-REG mapping according to TS 38.211 section 7.3.2.2
  coreset->mapping_type         = srsran_coreset_mapping_type_interleaved;
  coreset->reg_bundle_size      = srsran_coreset_bundle_size_n6;
  coreset->interleaver_size     = srsran_coreset_bundle_size_n2;
  coreset->precoder_granularity = srsran_coreset_precoder_granularity_reg_bundle;
  coreset->shift_index          = n_cell_id;

  // Set CORESET frequency resource mask
  for (uint32_t i = 0; i < SRSRAN_CORESET_FREQ_DOMAIN_RES_SIZE; i++) {
    coreset->freq_resources[i] = (i < (entry->nof_prb / 6));
  }

  return SRSRAN_SUCCESS;
}

const char* srsran_ssb_pattern_to_str(srsran_ssb_patern_t pattern)
{
  switch (pattern) {
    case SRSRAN_SSB_PATTERN_A:
      return "A";
    case SRSRAN_SSB_PATTERN_B:
      return "B";
    case SRSRAN_SSB_PATTERN_C:
      return "C";
    case SRSRAN_SSB_PATTERN_D:
      return "D";
    case SRSRAN_SSB_PATTERN_E:
      return "E";
    case SRSRAN_SSB_PATTERN_INVALID:
    default:
      break;
  }
  return "Invalid";
}

srsran_ssb_patern_t srsran_ssb_pattern_fom_str(const char* str)
{
  if (str == NULL) {
    return SRSRAN_SSB_PATTERN_INVALID;
  }

  if (strcasecmp(str, "A") == 0) {
    return SRSRAN_SSB_PATTERN_A;
  }

  if (strcasecmp(str, "B") == 0) {
    return SRSRAN_SSB_PATTERN_B;
  }

  if (strcasecmp(str, "C") == 0) {
    return SRSRAN_SSB_PATTERN_C;
  }

  if (strcasecmp(str, "D") == 0) {
    return SRSRAN_SSB_PATTERN_D;
  }

  if (strcasecmp(str, "E") == 0) {
    return SRSRAN_SSB_PATTERN_E;
  }

  return SRSRAN_SSB_PATTERN_INVALID;
}

bool srsran_carrier_nr_equal(const srsran_carrier_nr_t* a, const srsran_carrier_nr_t* b)
{
  if (a == NULL || b == NULL) {
    return false;
  }

  bool ret = (a->pci == b->pci);
  ret      = ret && (a->dl_center_frequency_hz == b->dl_center_frequency_hz);
  ret      = ret && (a->ul_center_frequency_hz == b->ul_center_frequency_hz);
  ret      = ret && (a->ssb_center_freq_hz == b->ssb_center_freq_hz);
  ret      = ret && (a->offset_to_carrier == b->offset_to_carrier);
  ret      = ret && (a->scs == b->scs);
  ret      = ret && (a->nof_prb == b->nof_prb);
  ret      = ret && (a->start == b->start);
  ret      = ret && (a->max_mimo_layers == b->max_mimo_layers);

  return ret;
}