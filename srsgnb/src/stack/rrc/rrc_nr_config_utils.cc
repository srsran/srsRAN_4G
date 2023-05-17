/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#include "srsgnb/hdr/stack/rrc/rrc_nr_config_utils.h"
#include "srsran/common/band_helper.h"

#define HANDLE_ERROR(x)                                                                                                \
  do {                                                                                                                 \
    if (x != SRSRAN_SUCCESS) {                                                                                         \
      return SRSRAN_ERROR;                                                                                             \
    }                                                                                                                  \
  } while (0)

#define ERROR_IF_NOT(x, fmt, ...)                                                                                      \
  do {                                                                                                                 \
    if (not(x)) {                                                                                                      \
      fprintf(stderr, "ERROR: " fmt "\n", ##__VA_ARGS__);                                                              \
      return SRSRAN_ERROR;                                                                                             \
    }                                                                                                                  \
  } while (0)

using namespace asn1::rrc_nr;

namespace srsenb {

uint32_t coreset_get_bw(const asn1::rrc_nr::ctrl_res_set_s& coreset)
{
  uint32_t prb_count = 0;

  // Iterate all the frequency domain resources bit-map...
  for (uint32_t i = 0; i < SRSRAN_CORESET_FREQ_DOMAIN_RES_SIZE; i++) {
    // ... and count 6 PRB for every frequency domain resource that it is enabled
    if (coreset.freq_domain_res.get(i)) {
      prb_count += 6;
    }
  }

  // Return the total count of physical resource blocks
  return prb_count;
}

int coreset_get_pdcch_nr_max_candidates(const asn1::rrc_nr::ctrl_res_set_s& coreset, uint32_t aggregation_level)
{
  uint32_t coreset_bw = coreset_get_bw(coreset);
  uint32_t nof_cce    = (coreset_bw * coreset.dur) / 6;

  uint32_t L              = 1U << aggregation_level;
  uint32_t nof_candidates = nof_cce / L;

  return SRSRAN_MIN(nof_candidates, SRSRAN_SEARCH_SPACE_MAX_NOF_CANDIDATES_NR);
}

ctrl_res_set_s make_default_coreset(uint8_t coreset_id, uint32_t nof_prb)
{
  ctrl_res_set_s coreset;
  coreset.ctrl_res_set_id = coreset_id;
  // Generate frequency resources for the full BW
  for (uint32_t i = 0; i < SRSRAN_CORESET_FREQ_DOMAIN_RES_SIZE; i++) {
    coreset.freq_domain_res.set(coreset.freq_domain_res.length() - i - 1, i < SRSRAN_FLOOR(nof_prb, 6));
  }
  coreset.dur = 1;
  coreset.cce_reg_map_type.set_non_interleaved();
  coreset.precoder_granularity.value = ctrl_res_set_s::precoder_granularity_opts::same_as_reg_bundle;
  return coreset;
}

search_space_s make_default_common_search_space(uint8_t ss_id, const ctrl_res_set_s& cs)
{
  search_space_s ss;
  ss.search_space_id                                = ss_id;
  ss.ctrl_res_set_id_present                        = true;
  ss.ctrl_res_set_id                                = cs.ctrl_res_set_id;
  ss.dur_present                                    = false; // false for duration=1
  ss.monitoring_slot_periodicity_and_offset_present = true;
  ss.monitoring_slot_periodicity_and_offset.set_sl1();
  ss.monitoring_symbols_within_slot_present = true;
  ss.monitoring_symbols_within_slot.from_number(0b10000000000000);
  ss.search_space_type_present                                                    = true;
  ss.search_space_type.set_common().dci_format0_minus0_and_format1_minus0_present = true;
  ss.nrof_candidates_present                                                      = true;
  uint32_t nof_cand = SRSRAN_MIN(coreset_get_pdcch_nr_max_candidates(cs, 0), 2);
  asn1::number_to_enum(ss.nrof_candidates.aggregation_level1, nof_cand);
  nof_cand = SRSRAN_MIN(coreset_get_pdcch_nr_max_candidates(cs, 1), 2);
  asn1::number_to_enum(ss.nrof_candidates.aggregation_level2, nof_cand);
  nof_cand = SRSRAN_MIN(coreset_get_pdcch_nr_max_candidates(cs, 2), 2);
  asn1::number_to_enum(ss.nrof_candidates.aggregation_level4, nof_cand);
  nof_cand = SRSRAN_MIN(coreset_get_pdcch_nr_max_candidates(cs, 3), 2);
  asn1::number_to_enum(ss.nrof_candidates.aggregation_level8, nof_cand);
  nof_cand = SRSRAN_MIN(coreset_get_pdcch_nr_max_candidates(cs, 4), 2);
  asn1::number_to_enum(ss.nrof_candidates.aggregation_level16, nof_cand);

  return ss;
}

/// Generate default phy cell configuration
void generate_default_nr_phy_cell(phy_cell_cfg_nr_t& phy_cell)
{
  phy_cell = {};

  phy_cell.carrier.scs             = srsran_subcarrier_spacing_15kHz;
  phy_cell.carrier.nof_prb         = 52;
  phy_cell.carrier.max_mimo_layers = 1;
}

/// Generate default rrc nr cell configuration
void generate_default_nr_cell(rrc_cell_cfg_nr_t& cell)
{
  cell                         = {};
  cell.coreset0_idx            = 7;
  cell.ssb_absolute_freq_point = 0; // auto derived
  cell.num_ra_preambles        = 8;
  generate_default_nr_phy_cell(cell.phy_cell);

  // PDCCH
  // - Add CORESET#2 as UE-specific
  cell.pdcch_cfg_ded.ctrl_res_set_to_add_mod_list.resize(1);
  auto& coreset2 = cell.pdcch_cfg_ded.ctrl_res_set_to_add_mod_list[0];
  coreset2       = make_default_coreset(2, cell.phy_cell.carrier.nof_prb);

  // - Add SearchSpace#2 as UE-specific -> CORESET#2
  cell.pdcch_cfg_ded.search_spaces_to_add_mod_list.resize(1);
  auto& ss2                                                 = cell.pdcch_cfg_ded.search_spaces_to_add_mod_list[0];
  ss2                                                       = make_default_common_search_space(2, coreset2);
  ss2.search_space_type.set_ue_specific().dci_formats.value = asn1::rrc_nr::search_space_s::search_space_type_c_::
      ue_specific_s_::dci_formats_opts::formats0_minus0_and_minus1_minus0;
}

int derive_ssb_params(bool                        is_sa,
                      uint32_t                    dl_arfcn,
                      uint32_t                    band,
                      srsran_subcarrier_spacing_t pdcch_scs,
                      uint32_t                    coreset0_idx,
                      uint32_t                    nof_prb,
                      rrc_cell_cfg_nr_t&          cell)
{
  // Verify essential parameters are specified and valid
  ERROR_IF_NOT(dl_arfcn > 0, "Invalid DL ARFCN=%d", dl_arfcn);
  ERROR_IF_NOT(band > 0, "Band is a mandatory parameter");
  ERROR_IF_NOT(pdcch_scs < srsran_subcarrier_spacing_invalid, "Invalid carrier SCS");
  ERROR_IF_NOT(coreset0_idx < 15, "Invalid controlResourceSetZero");
  ERROR_IF_NOT(nof_prb > 0, "Invalid DL number of PRBS=%d", nof_prb);

  srsran::srsran_band_helper band_helper;

  double   dl_freq_hz               = band_helper.nr_arfcn_to_freq(dl_arfcn);
  uint32_t dl_absolute_freq_point_a = band_helper.get_abs_freq_point_a_arfcn(nof_prb, dl_arfcn);

  // derive SSB pattern and scs
  cell.ssb_pattern = band_helper.get_ssb_pattern(band, srsran_subcarrier_spacing_15kHz);
  if (cell.ssb_pattern == SRSRAN_SSB_PATTERN_A) {
    // 15kHz SSB SCS
    cell.ssb_scs = srsran_subcarrier_spacing_15kHz;
  } else {
    // try to optain SSB pattern for same band with 30kHz SCS
    cell.ssb_pattern = band_helper.get_ssb_pattern(band, srsran_subcarrier_spacing_30kHz);
    if (cell.ssb_pattern == SRSRAN_SSB_PATTERN_B || cell.ssb_pattern == SRSRAN_SSB_PATTERN_C) {
      // SSB SCS is 30 kHz
      cell.ssb_scs = srsran_subcarrier_spacing_30kHz;
    } else {
      srsran_terminate("Can't derive SSB pattern from band %d", band);
    }
  }

  // derive SSB position
  int coreset0_rb_offset = 0;
  if (is_sa) {
    // Get offset in RBs between CORESET#0 and SSB
    coreset0_rb_offset = srsran_coreset0_ssb_offset(coreset0_idx, cell.ssb_scs, pdcch_scs);
    ERROR_IF_NOT(coreset0_rb_offset >= 0, "Failed to compute RB offset between CORESET#0 and SSB");
  } else {
    // TODO: Verify if specified SSB frequency is valid
  }
  uint32_t ssb_abs_freq_point =
      band_helper.get_abs_freq_ssb_arfcn(band, cell.ssb_scs, dl_absolute_freq_point_a, coreset0_rb_offset);
  ERROR_IF_NOT(ssb_abs_freq_point > 0,
               "Can't derive SSB freq point for dl_arfcn=%d and band %d",
               band_helper.freq_to_nr_arfcn(dl_freq_hz),
               band);

  // Convert to frequency for PHY
  cell.ssb_absolute_freq_point = ssb_abs_freq_point;
  cell.ssb_freq_hz             = band_helper.nr_arfcn_to_freq(ssb_abs_freq_point);

  double   pointA_abs_freq_Hz = dl_freq_hz - nof_prb * SRSRAN_NRE * SRSRAN_SUBC_SPACING_NR(pdcch_scs) / 2;
  uint32_t ssb_pointA_freq_offset_Hz =
      (cell.ssb_freq_hz > pointA_abs_freq_Hz) ? (uint32_t)(cell.ssb_freq_hz - pointA_abs_freq_Hz) : 0;

  cell.ssb_offset = (uint32_t)(ssb_pointA_freq_offset_Hz / SRSRAN_SUBC_SPACING_NR(pdcch_scs)) % SRSRAN_NRE;

  // Validate Coreset0 has space
  srsran_coreset_t coreset0 = {};
  ERROR_IF_NOT(
      srsran_coreset_zero(
          cell.phy_cell.cell_id, ssb_pointA_freq_offset_Hz, cell.ssb_scs, pdcch_scs, coreset0_idx, &coreset0) == 0,
      "Deriving parameters for coreset0: index=%d, ssb_pointA_offset=%d kHz\n",
      coreset0_idx,
      ssb_pointA_freq_offset_Hz / 1000);

  ERROR_IF_NOT(srsran_coreset_start_rb(&coreset0) + srsran_coreset_get_bw(&coreset0) <= cell.phy_cell.carrier.nof_prb,
               "Coreset0 index=%d is not compatible with DL ARFCN %d in band %d\n",
               coreset0_idx,
               cell.dl_arfcn,
               cell.band);

  // Validate Coreset0 has less than 3 symbols
  ERROR_IF_NOT(coreset0.duration < 3,
               "Coreset0 index=%d is not supported due to overlap with SSB. Select a coreset0 index from 38.213 Table "
               "13-1 such that N_symb_coreset < 3\n",
               coreset0_idx);

  // Validate Coreset0 has more than 24 RB
  ERROR_IF_NOT(srsran_coreset_get_bw(&coreset0) > 24,
               "Coreset0 configuration index=%d has only %d RB. A coreset0 index >= 6 is required such as N_rb >= 48\n",
               srsran_coreset_get_bw(&coreset0),
               coreset0_idx);

  return SRSRAN_SUCCESS;
}

int derive_phy_cell_freq_params(uint32_t dl_arfcn, uint32_t ul_arfcn, phy_cell_cfg_nr_t& phy_cell)
{
  // Verify essential parameters are specified and valid
  ERROR_IF_NOT(dl_arfcn > 0, "DL ARFCN is a mandatory parameter");

  // Use helper class to derive NR carrier parameters
  srsran::srsran_band_helper band_helper;

  // derive DL freq from ARFCN
  if (phy_cell.carrier.dl_center_frequency_hz == 0) {
    phy_cell.carrier.dl_center_frequency_hz = band_helper.nr_arfcn_to_freq(dl_arfcn);
  }

  // derive UL freq from ARFCN
  if (phy_cell.carrier.ul_center_frequency_hz == 0) {
    // auto-detect UL frequency
    if (ul_arfcn == 0) {
      // derive UL ARFCN from given DL ARFCN
      ul_arfcn = band_helper.get_ul_arfcn_from_dl_arfcn(dl_arfcn);
      ERROR_IF_NOT(ul_arfcn > 0, "Can't derive UL ARFCN from DL ARFCN %d", dl_arfcn);
    }
    phy_cell.carrier.ul_center_frequency_hz = band_helper.nr_arfcn_to_freq(ul_arfcn);
  }

  return SRSRAN_SUCCESS;
}

int set_derived_nr_cell_params(bool is_sa, rrc_cell_cfg_nr_t& cell)
{
  // Verify essential parameters are specified and valid
  ERROR_IF_NOT(cell.dl_arfcn > 0, "DL ARFCN is a mandatory parameter");
  ERROR_IF_NOT(cell.band > 0, "Band is a mandatory parameter");
  ERROR_IF_NOT(cell.phy_cell.carrier.nof_prb > 0, "Number of PRBs is a mandatory parameter");

  // Use helper class to derive NR carrier parameters
  srsran::srsran_band_helper band_helper;

  if (cell.ul_arfcn == 0) {
    // derive UL ARFCN from given DL ARFCN
    cell.ul_arfcn = band_helper.get_ul_arfcn_from_dl_arfcn(cell.dl_arfcn);
    ERROR_IF_NOT(cell.ul_arfcn > 0, "Can't derive UL ARFCN from DL ARFCN %d", cell.dl_arfcn);
  }

  // duplex mode
  cell.duplex_mode = band_helper.get_duplex_mode(cell.band);

  // PointA
  cell.dl_absolute_freq_point_a = band_helper.get_abs_freq_point_a_arfcn(cell.phy_cell.carrier.nof_prb, cell.dl_arfcn);
  cell.ul_absolute_freq_point_a = band_helper.get_abs_freq_point_a_arfcn(cell.phy_cell.carrier.nof_prb, cell.ul_arfcn);

  // Derive phy_cell parameters that depend on ARFCNs
  derive_phy_cell_freq_params(cell.dl_arfcn, cell.ul_arfcn, cell.phy_cell);

  // Derive SSB params
  ERROR_IF_NOT(derive_ssb_params(is_sa,
                                 cell.dl_arfcn,
                                 cell.band,
                                 cell.phy_cell.carrier.scs,
                                 cell.coreset0_idx,
                                 cell.phy_cell.carrier.nof_prb,
                                 cell) == 0,
               "Deriving SSB parameters\n");

  cell.phy_cell.carrier.ssb_center_freq_hz = cell.ssb_freq_hz;

  // Derive remaining config params
  if (not is_sa) {
    // Configure CORESET#1
    cell.pdcch_cfg_common.common_ctrl_res_set_present = true;
    cell.pdcch_cfg_common.common_ctrl_res_set         = make_default_coreset(1, cell.phy_cell.carrier.nof_prb);
  }

  // Configure SearchSpace#1
  cell.pdcch_cfg_common.common_search_space_list.resize(1);
  auto& ss1 = cell.pdcch_cfg_common.common_search_space_list[0];
  if (is_sa) {
    // Configure SearchSpace#1 -> CORESET#0
    ctrl_res_set_s dummy_coreset                  = make_default_coreset(0, cell.phy_cell.carrier.nof_prb);
    ss1                                           = make_default_common_search_space(1, dummy_coreset);
    ss1.nrof_candidates.aggregation_level1.value  = search_space_s::nrof_candidates_s_::aggregation_level1_opts::n0;
    ss1.nrof_candidates.aggregation_level2.value  = search_space_s::nrof_candidates_s_::aggregation_level2_opts::n0;
    ss1.nrof_candidates.aggregation_level4.value  = search_space_s::nrof_candidates_s_::aggregation_level4_opts::n1;
    ss1.nrof_candidates.aggregation_level8.value  = search_space_s::nrof_candidates_s_::aggregation_level8_opts::n0;
    ss1.nrof_candidates.aggregation_level16.value = search_space_s::nrof_candidates_s_::aggregation_level16_opts::n0;
  } else {
    // Configure SearchSpace#1 -> CORESET#1
    ss1 = make_default_common_search_space(1, cell.pdcch_cfg_common.common_ctrl_res_set);
    //    cell.phy_cell.pdcch.search_space[1].type       = srsran_search_space_type_common_3;
  }
  cell.pdcch_cfg_common.ra_search_space_present = true;
  cell.pdcch_cfg_common.ra_search_space         = ss1.search_space_id;

  return SRSRAN_SUCCESS;
}

int set_derived_nr_rrc_params(rrc_nr_cfg_t& rrc_cfg)
{
  for (rrc_cell_cfg_nr_t& cell : rrc_cfg.cell_list) {
    HANDLE_ERROR(set_derived_nr_cell_params(rrc_cfg.is_standalone, cell));
  }
  return SRSRAN_SUCCESS;
}

int check_nr_pdcch_cfg_valid(const srsran_pdcch_cfg_nr_t& pdcch)
{
  // Verify Search Spaces
  std::array<bool, SRSRAN_UE_DL_NR_MAX_NOF_CORESET> used_coresets{};
  for (uint32_t ss_id = 0; ss_id < SRSRAN_UE_DL_NR_MAX_NOF_SEARCH_SPACE; ++ss_id) {
    if (pdcch.search_space_present[ss_id]) {
      const srsran_search_space_t& ss = pdcch.search_space[ss_id];
      ERROR_IF_NOT(ss.id == ss_id, "SearchSpace#%d should match list index", ss_id);
      uint32_t cs_id = ss.coreset_id;
      ERROR_IF_NOT(pdcch.coreset_present[cs_id], "SearchSpace#%d points to absent CORESET#%d", ss_id, cs_id);
      used_coresets[cs_id] = true;
    }
  }

  // Verify CORESET id
  for (uint32_t cs_id = 0; cs_id < SRSRAN_UE_DL_NR_MAX_NOF_CORESET; ++cs_id) {
    ERROR_IF_NOT(pdcch.coreset_present[cs_id] == used_coresets[cs_id], "CORESET#%d is configured but not used", cs_id);
  }

  return SRSRAN_SUCCESS;
}

} // namespace srsenb
