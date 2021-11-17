/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/hdr/stack/rrc/rrc_nr_config_utils.h"
#include "srsran/common/band_helper.h"

#define RETURN_IF_ERROR(x)                                                                                             \
  do {                                                                                                                 \
    if (x != SRSRAN_SUCCESS) {                                                                                         \
      return SRSRAN_ERROR;                                                                                             \
    }                                                                                                                  \
  } while (0)

#define INVALID_PARAM(x, fmt, ...)                                                                                     \
  do {                                                                                                                 \
    if (not(x)) {                                                                                                      \
      fprintf(stderr, "ERROR: " fmt, ##__VA_ARGS__);                                                                   \
      return SRSRAN_ERROR;                                                                                             \
    }                                                                                                                  \
  } while (0)

namespace srsenb {

/// Generate default phy cell configuration
void generate_default_nr_phy_cell(phy_cell_cfg_nr_t& phy_cell)
{
  phy_cell = {};

  phy_cell.carrier.scs             = srsran_subcarrier_spacing_15kHz;
  phy_cell.carrier.nof_prb         = 52;
  phy_cell.carrier.max_mimo_layers = 1;

  phy_cell.dl_freq_hz       = 0; // auto set
  phy_cell.ul_freq_hz       = 0;
  phy_cell.num_ra_preambles = 52;

  // PRACH
  phy_cell.prach.is_nr                 = true;
  phy_cell.prach.config_idx            = 8;
  phy_cell.prach.root_seq_idx          = 0;
  phy_cell.prach.freq_offset           = 1;
  phy_cell.prach.num_ra_preambles      = phy_cell.num_ra_preambles;
  phy_cell.prach.hs_flag               = false;
  phy_cell.prach.tdd_config.configured = false;

  // PDSCH
  phy_cell.pdsch.rs_power = 0;
  phy_cell.pdsch.p_b      = 0;
}

/// Generate default rrc nr cell configuration
void generate_default_nr_cell(rrc_cell_cfg_nr_t& cell)
{
  cell                         = {};
  cell.coreset0_idx            = 6;
  cell.ssb_absolute_freq_point = 0; // auto derived
  generate_default_nr_phy_cell(cell.phy_cell);
}

/// Generate CORESET#0 and SSB absolute frequency (if not specified)
int derive_coreset0_params(rrc_cell_cfg_nr_t& cell)
{
  // Generate CORESET#0
  cell.phy_cell.pdcch.coreset_present[0] = true;
  // Get pointA and SSB absolute frequencies
  double pointA_abs_freq_Hz =
      cell.phy_cell.carrier.dl_center_frequency_hz -
      cell.phy_cell.carrier.nof_prb * SRSRAN_NRE * SRSRAN_SUBC_SPACING_NR(cell.phy_cell.carrier.scs) / 2;
  double ssb_abs_freq_Hz = cell.phy_cell.carrier.ssb_center_freq_hz;
  // Calculate integer SSB to pointA frequency offset in Hz
  uint32_t ssb_pointA_freq_offset_Hz =
      (ssb_abs_freq_Hz > pointA_abs_freq_Hz) ? (uint32_t)(ssb_abs_freq_Hz - pointA_abs_freq_Hz) : 0;
  int ret = srsran_coreset_zero(cell.phy_cell.carrier.pci,
                                ssb_pointA_freq_offset_Hz,
                                cell.ssb_cfg.scs,
                                cell.phy_cell.carrier.scs,
                                cell.coreset0_idx,
                                &cell.phy_cell.pdcch.coreset[0]);
  INVALID_PARAM(ret == SRSRAN_SUCCESS, "Failed to generate CORESET#0");
  return SRSRAN_SUCCESS;
}

int derive_ssb_params(bool                        is_sa,
                      uint32_t                    dl_arfcn,
                      uint32_t                    band,
                      srsran_subcarrier_spacing_t pdcch_scs,
                      uint32_t                    coreset0_idx,
                      uint32_t                    nof_prb,
                      srsran_ssb_cfg_t&           ssb)
{
  // Verify essential parameters are specified and valid
  INVALID_PARAM(dl_arfcn > 0, "Invalid DL ARFCN=%d", dl_arfcn);
  INVALID_PARAM(band > 0, "Band is a mandatory parameter");
  INVALID_PARAM(pdcch_scs < srsran_subcarrier_spacing_invalid, "Invalid carrier SCS");
  INVALID_PARAM(coreset0_idx < 15, "Invalid controlResourceSetZero");
  INVALID_PARAM(nof_prb > 0, "Invalid DL number of PRBS=%d", nof_prb);

  srsran::srsran_band_helper band_helper;

  double   dl_freq_hz               = band_helper.nr_arfcn_to_freq(dl_arfcn);
  uint32_t dl_absolute_freq_point_a = band_helper.get_abs_freq_point_a_arfcn(nof_prb, dl_arfcn);

  ssb.center_freq_hz = dl_freq_hz;
  ssb.duplex_mode    = band_helper.get_duplex_mode(band);

  // derive SSB pattern and scs
  ssb.pattern = band_helper.get_ssb_pattern(band, srsran_subcarrier_spacing_15kHz);
  if (ssb.pattern == SRSRAN_SSB_PATTERN_A) {
    // 15kHz SSB SCS
    ssb.scs = srsran_subcarrier_spacing_15kHz;
  } else {
    // try to optain SSB pattern for same band with 30kHz SCS
    ssb.pattern = band_helper.get_ssb_pattern(band, srsran_subcarrier_spacing_30kHz);
    if (ssb.pattern == SRSRAN_SSB_PATTERN_B || ssb.pattern == SRSRAN_SSB_PATTERN_C) {
      // SSB SCS is 30 kHz
      ssb.scs = srsran_subcarrier_spacing_30kHz;
    } else {
      srsran_terminate("Can't derive SSB pattern from band %d", band);
    }
  }

  // derive SSB position
  int coreset0_rb_offset = 0;
  if (is_sa) {
    // Get offset in RBs between CORESET#0 and SSB
    coreset0_rb_offset = srsran_coreset0_ssb_offset(coreset0_idx, ssb.scs, pdcch_scs);
    INVALID_PARAM(coreset0_rb_offset >= 0, "Failed to compute RB offset between CORESET#0 and SSB");
  } else {
    // TODO: Verify if specified SSB frequency is valid
  }
  uint32_t ssb_abs_freq_point =
      band_helper.get_abs_freq_ssb_arfcn(band, ssb.scs, dl_absolute_freq_point_a, coreset0_rb_offset);
  INVALID_PARAM(ssb_abs_freq_point > 0,
                "Can't derive SSB freq point for dl_arfcn=%d and band %d",
                band_helper.freq_to_nr_arfcn(dl_freq_hz),
                band);

  // Convert to frequency for PHY
  ssb.ssb_freq_hz = band_helper.nr_arfcn_to_freq(ssb_abs_freq_point);

  ssb.periodicity_ms = 10; // TODO: make a param
  ssb.beta_pss       = 0.0;
  ssb.beta_sss       = 0.0;
  ssb.beta_pbch      = 0.0;
  ssb.beta_pbch_dmrs = 0.0;
  // set by PHY layer in worker_pool::set_common_cfg
  ssb.srate_hz = 0.0;
  ssb.scaling  = 0.0;

  return SRSRAN_SUCCESS;
}

int derive_phy_cell_freq_params(uint32_t dl_arfcn, uint32_t ul_arfcn, phy_cell_cfg_nr_t& phy_cell)
{
  // Verify essential parameters are specified and valid
  INVALID_PARAM(dl_arfcn > 0, "DL ARFCN is a mandatory parameter");

  // Use helper class to derive NR carrier parameters
  srsran::srsran_band_helper band_helper;

  // derive DL freq from ARFCN
  if (phy_cell.dl_freq_hz == 0) {
    phy_cell.dl_freq_hz = band_helper.nr_arfcn_to_freq(dl_arfcn);
  }

  // derive UL freq from ARFCN
  if (phy_cell.ul_freq_hz == 0) {
    // auto-detect UL frequency
    if (ul_arfcn == 0) {
      // derive UL ARFCN from given DL ARFCN
      ul_arfcn = band_helper.get_ul_arfcn_from_dl_arfcn(dl_arfcn);
      INVALID_PARAM(ul_arfcn > 0, "Can't derive UL ARFCN from DL ARFCN %d", dl_arfcn);
    }
    phy_cell.ul_freq_hz = band_helper.nr_arfcn_to_freq(ul_arfcn);
  }

  // copy center frequencies
  phy_cell.carrier.dl_center_frequency_hz = phy_cell.dl_freq_hz;
  phy_cell.carrier.ul_center_frequency_hz = phy_cell.ul_freq_hz;

  return SRSRAN_SUCCESS;
}

int set_derived_nr_cell_params(bool is_sa, rrc_cell_cfg_nr_t& cell)
{
  // Verify essential parameters are specified and valid
  INVALID_PARAM(cell.dl_arfcn > 0, "DL ARFCN is a mandatory parameter");
  INVALID_PARAM(cell.band > 0, "Band is a mandatory parameter");
  INVALID_PARAM(cell.phy_cell.carrier.nof_prb > 0, "Number of PRBs is a mandatory parameter");

  // Use helper class to derive NR carrier parameters
  srsran::srsran_band_helper band_helper;

  if (cell.ul_arfcn == 0) {
    // derive UL ARFCN from given DL ARFCN
    cell.ul_arfcn = band_helper.get_ul_arfcn_from_dl_arfcn(cell.dl_arfcn);
    INVALID_PARAM(cell.ul_arfcn > 0, "Can't derive UL ARFCN from DL ARFCN %d", cell.dl_arfcn);
  }

  // duplex mode
  cell.duplex_mode = band_helper.get_duplex_mode(cell.band);

  // PointA
  cell.dl_absolute_freq_point_a = band_helper.get_abs_freq_point_a_arfcn(cell.phy_cell.carrier.nof_prb, cell.dl_arfcn);
  cell.ul_absolute_freq_point_a = band_helper.get_abs_freq_point_a_arfcn(cell.phy_cell.carrier.nof_prb, cell.ul_arfcn);

  // Derive phy_cell parameters that depend on ARFCNs
  derive_phy_cell_freq_params(cell.dl_arfcn, cell.ul_arfcn, cell.phy_cell);

  // Derive SSB params
  derive_ssb_params(is_sa,
                    cell.dl_arfcn,
                    cell.band,
                    cell.phy_cell.carrier.scs,
                    cell.coreset0_idx,
                    cell.phy_cell.carrier.nof_prb,
                    cell.ssb_cfg);
  cell.phy_cell.carrier.ssb_center_freq_hz = cell.ssb_cfg.ssb_freq_hz;
  cell.ssb_absolute_freq_point             = band_helper.freq_to_nr_arfcn(cell.ssb_cfg.ssb_freq_hz);

  // Derive remaining config params
  if (is_sa) {
    derive_coreset0_params(cell);
    cell.phy_cell.pdcch.search_space_present[0]           = true;
    cell.phy_cell.pdcch.search_space[0].id                = 0;
    cell.phy_cell.pdcch.search_space[0].coreset_id        = 0;
    cell.phy_cell.pdcch.search_space[0].type              = srsran_search_space_type_common_0;
    cell.phy_cell.pdcch.search_space[0].nof_candidates[0] = 1;
    cell.phy_cell.pdcch.search_space[0].nof_candidates[1] = 1;
    cell.phy_cell.pdcch.search_space[0].nof_candidates[2] = 1;
    cell.phy_cell.pdcch.search_space[0].nof_candidates[3] = 0;
    cell.phy_cell.pdcch.search_space[0].nof_candidates[4] = 0;
    cell.phy_cell.pdcch.search_space[0].nof_formats       = 1;
    cell.phy_cell.pdcch.search_space[0].formats[0]        = srsran_dci_format_nr_1_0;
    cell.phy_cell.pdcch.search_space[0].duration          = 1;
    cell.phy_cell.pdcch.search_space_present[1]           = true;
    cell.phy_cell.pdcch.search_space[1].id                = 1;
    cell.phy_cell.pdcch.search_space[1].coreset_id        = 0;
    cell.phy_cell.pdcch.search_space[1].type              = srsran_search_space_type_common_3;
    cell.phy_cell.pdcch.search_space[1].nof_candidates[0] = 0;
    cell.phy_cell.pdcch.search_space[1].nof_candidates[1] = 0;
    cell.phy_cell.pdcch.search_space[1].nof_candidates[2] = 1;
    cell.phy_cell.pdcch.search_space[1].nof_candidates[3] = 0;
    cell.phy_cell.pdcch.search_space[1].nof_candidates[4] = 0;
    cell.phy_cell.pdcch.search_space[1].nof_formats       = 2;
    cell.phy_cell.pdcch.search_space[1].formats[0]        = srsran_dci_format_nr_0_0; // DCI format for PUSCH
    cell.phy_cell.pdcch.search_space[1].formats[1]        = srsran_dci_format_nr_1_0; // DCI format for PDSCH
    cell.phy_cell.pdcch.search_space[1].duration          = 1;
  } else {
    // Configure CORESET#1
    cell.phy_cell.pdcch.coreset_present[1]              = true;
    cell.phy_cell.pdcch.coreset[1].id                   = 1;
    cell.phy_cell.pdcch.coreset[1].duration             = 1;
    cell.phy_cell.pdcch.coreset[1].mapping_type         = srsran_coreset_mapping_type_non_interleaved;
    cell.phy_cell.pdcch.coreset[1].precoder_granularity = srsran_coreset_precoder_granularity_reg_bundle;

    // Generate frequency resources for the full BW
    for (uint32_t i = 0; i < SRSRAN_CORESET_FREQ_DOMAIN_RES_SIZE; i++) {
      cell.phy_cell.pdcch.coreset[1].freq_resources[i] = i < SRSRAN_FLOOR(cell.phy_cell.carrier.nof_prb, 6);
    }

    // Configure SearchSpace#1 -> CORESET#1
    cell.phy_cell.pdcch.search_space_present[1]    = true;
    cell.phy_cell.pdcch.search_space[1].id         = 1;
    cell.phy_cell.pdcch.search_space[1].coreset_id = 1;
    cell.phy_cell.pdcch.search_space[1].type       = srsran_search_space_type_common_3;
    // Generate frequency resources for the full BW
    for (uint32_t L = 0; L < SRSRAN_SEARCH_SPACE_NOF_AGGREGATION_LEVELS_NR; L++) {
      cell.phy_cell.pdcch.search_space[1].nof_candidates[L] =
          SRSRAN_MIN(2, srsran_pdcch_nr_max_candidates_coreset(&cell.phy_cell.pdcch.coreset[1], L));
    }
    cell.phy_cell.pdcch.search_space[1].nof_formats = 2;
    cell.phy_cell.pdcch.search_space[1].formats[0]  = srsran_dci_format_nr_0_0; // DCI format for PUSCH
    cell.phy_cell.pdcch.search_space[1].formats[1]  = srsran_dci_format_nr_1_0; // DCI format for PDSCH
    cell.phy_cell.pdcch.search_space[1].duration    = 1;
  }

  cell.phy_cell.pdcch.ra_search_space_present = true;
  cell.phy_cell.pdcch.ra_search_space         = cell.phy_cell.pdcch.search_space[1];
  cell.phy_cell.pdcch.ra_search_space.type    = srsran_search_space_type_common_1;

  // Derive remaining PHY cell params
  cell.phy_cell.prach.num_ra_preambles      = cell.phy_cell.num_ra_preambles;
  cell.phy_cell.prach.tdd_config.configured = (cell.duplex_mode == SRSRAN_DUPLEX_MODE_TDD);

  return check_nr_cell_cfg_valid(cell, is_sa);
}

int check_nr_cell_cfg_valid(const rrc_cell_cfg_nr_t& cell, bool is_sa)
{
  // verify SSB params are consistent
  INVALID_PARAM(cell.ssb_cfg.center_freq_hz == cell.phy_cell.dl_freq_hz, "Invalid SSB param generation");
  RETURN_IF_ERROR(check_nr_phy_cell_cfg_valid(cell.phy_cell));

  return SRSRAN_SUCCESS;
}

int check_nr_phy_cell_cfg_valid(const phy_cell_cfg_nr_t& phy_cell)
{
  RETURN_IF_ERROR(check_nr_pdcch_cfg_valid(phy_cell.pdcch));
  return SRSRAN_SUCCESS;
}

int check_nr_pdcch_cfg_valid(const srsran_pdcch_cfg_nr_t& pdcch)
{
  // Verify Search Spaces
  for (uint32_t ss_id = 0; ss_id < SRSRAN_UE_DL_NR_MAX_NOF_SEARCH_SPACE; ++ss_id) {
    if (pdcch.search_space_present[ss_id]) {
      const srsran_search_space_t& ss = pdcch.search_space[ss_id];
      INVALID_PARAM(ss.id == ss_id, "SearchSpace#%d should match list index", ss_id);
      uint32_t cs_id = ss.coreset_id;
      INVALID_PARAM(pdcch.coreset_present[cs_id], "SearchSpace#%d points to absent CORESET#%d", ss_id, cs_id);
    }
  }
  return SRSRAN_SUCCESS;
}

} // namespace srsenb