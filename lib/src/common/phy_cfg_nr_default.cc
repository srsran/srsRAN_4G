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

#include "srsran/common/phy_cfg_nr_default.h"
#include "srsran/srsran.h"

namespace srsran {

void phy_cfg_nr_default_t::make_carrier_custom_10MHz(srsran_carrier_nr_t& carrier)
{
  carrier.nof_prb                    = 52;
  carrier.max_mimo_layers            = 1;
  carrier.pci                        = 500;
  carrier.absolute_frequency_point_a = 633928;
  carrier.absolute_frequency_ssb     = 634176;
  carrier.offset_to_carrier          = 0;
  carrier.scs                        = srsran_subcarrier_spacing_15kHz;
}

void phy_cfg_nr_default_t::make_tdd_custom_6_4(srsran_tdd_config_nr_t& tdd)
{
  tdd.pattern1.period_ms      = 10;
  tdd.pattern1.nof_dl_slots   = 6;
  tdd.pattern1.nof_dl_symbols = 0;
  tdd.pattern1.nof_ul_slots   = 4;
  tdd.pattern1.nof_ul_symbols = 0;

  // Disable pattern 2
  tdd.pattern2.period_ms = 0;
}

void phy_cfg_nr_default_t::make_pdcch_custom_common_ss(srsran_pdcch_cfg_nr_t& pdcch, const srsran_carrier_nr_t& carrier)
{
  // Configure CORESET ID 1
  pdcch.coreset_present[1]              = true;
  pdcch.coreset[1].id                   = 1;
  pdcch.coreset[1].duration             = 1;
  pdcch.coreset[1].mapping_type         = srsran_coreset_mapping_type_non_interleaved;
  pdcch.coreset[1].precoder_granularity = srsran_coreset_precoder_granularity_reg_bundle;

  // Generate frequency resources for the full BW
  for (uint32_t i = 0; i < SRSRAN_CORESET_FREQ_DOMAIN_RES_SIZE; i++) {
    pdcch.coreset[1].freq_resources[i] = i < SRSRAN_FLOOR(carrier.nof_prb, 6);
  }

  // Configure Search Space 1 as common
  pdcch.search_space_present[1]     = true;
  pdcch.search_space[1].id          = 1;
  pdcch.search_space[1].coreset_id  = 1;
  pdcch.search_space[1].duration    = 1;
  pdcch.search_space[1].formats[0]  = srsran_dci_format_nr_0_0; // DCI format for PUSCH
  pdcch.search_space[1].formats[1]  = srsran_dci_format_nr_1_0; // DCI format for PDSCH
  pdcch.search_space[1].nof_formats = 2;
  pdcch.search_space[1].type        = srsran_search_space_type_common_3;

  // Generate 1 candidate for each aggregation level if possible
  for (uint32_t L = 0; L < SRSRAN_SEARCH_SPACE_NOF_AGGREGATION_LEVELS_NR; L++) {
    pdcch.search_space[1].nof_candidates[L] =
        SRSRAN_MIN(1, srsran_pdcch_nr_max_candidates_coreset(&pdcch.coreset[1], L));
  }
}

void phy_cfg_nr_default_t::make_pdsch_default(srsran_sch_hl_cfg_nr_t& pdsch)
{
  // Select PDSCH time resource allocation
  pdsch.common_time_ra[0].k            = 0;
  pdsch.common_time_ra[0].mapping_type = srsran_sch_mapping_type_A;
  pdsch.common_time_ra[0].sliv = srsran_ra_type2_to_riv(SRSRAN_NSYMB_PER_SLOT_NR - 1, 1, SRSRAN_NSYMB_PER_SLOT_NR);
  pdsch.nof_common_time_ra     = 1;

  // Setup PDSCH DMRS type A position
  pdsch.typeA_pos = srsran_dmrs_sch_typeA_pos_2;
}

void phy_cfg_nr_default_t::make_pusch_default(srsran_sch_hl_cfg_nr_t& pusch)
{
  // Select PUSCH time resource allocation
  pusch.common_time_ra[0].k            = 4;
  pusch.common_time_ra[0].mapping_type = srsran_sch_mapping_type_A;
  pusch.common_time_ra[0].sliv         = srsran_ra_type2_to_riv(SRSRAN_NSYMB_PER_SLOT_NR, 0, SRSRAN_NSYMB_PER_SLOT_NR);
  pusch.nof_common_time_ra             = 1;

  // Setup PUSCH DMRS type A position
  pusch.typeA_pos = srsran_dmrs_sch_typeA_pos_2;
}

void phy_cfg_nr_default_t::make_pucch_custom_one(srsran_pucch_nr_hl_cfg_t& pucch)
{
  // PUCCH Resource for format 1
  srsran_pucch_nr_resource_t resource_small = {};
  resource_small.starting_prb               = 0;
  resource_small.format                     = SRSRAN_PUCCH_NR_FORMAT_1;
  resource_small.initial_cyclic_shift       = 0;
  resource_small.nof_symbols                = 14;
  resource_small.start_symbol_idx           = 0;
  resource_small.time_domain_occ            = 0;

  // PUCCH Resource for format 2
  srsran_pucch_nr_resource_t resource_big = {};
  resource_big.starting_prb               = 51;
  resource_big.format                     = SRSRAN_PUCCH_NR_FORMAT_2;
  resource_big.nof_prb                    = 1;
  resource_big.nof_symbols                = 2;
  resource_big.start_symbol_idx           = 0;

  // Resource for SR
  srsran_pucch_nr_resource_t resource_sr = {};
  resource_sr.starting_prb               = 51;
  resource_sr.format                     = SRSRAN_PUCCH_NR_FORMAT_1;
  resource_sr.initial_cyclic_shift       = 0;
  resource_sr.nof_symbols                = 14;
  resource_sr.start_symbol_idx           = 0;
  resource_sr.time_domain_occ            = 0;

  pucch.enabled = true;

  // Set format 1 for 1-2 bits
  pucch.sets[0].resources[0]  = resource_small;
  pucch.sets[0].resources[1]  = resource_small;
  pucch.sets[0].resources[2]  = resource_small;
  pucch.sets[0].resources[3]  = resource_small;
  pucch.sets[0].resources[4]  = resource_small;
  pucch.sets[0].resources[5]  = resource_small;
  pucch.sets[0].resources[6]  = resource_small;
  pucch.sets[0].resources[7]  = resource_small;
  pucch.sets[0].nof_resources = 8;

  // Set format 2 for more bits
  pucch.sets[1].resources[0]  = resource_big;
  pucch.sets[1].resources[1]  = resource_big;
  pucch.sets[1].resources[2]  = resource_big;
  pucch.sets[1].resources[3]  = resource_big;
  pucch.sets[1].resources[4]  = resource_big;
  pucch.sets[1].resources[5]  = resource_big;
  pucch.sets[1].resources[6]  = resource_big;
  pucch.sets[1].resources[7]  = resource_big;
  pucch.sets[1].nof_resources = 8;

  // Configure scheduling request
  pucch.sr_resources[1].configured = true;
  pucch.sr_resources[1].sr_id      = 0;
  pucch.sr_resources[1].period     = 40;
  pucch.sr_resources[1].offset     = 8;
  pucch.sr_resources[1].resource   = resource_sr;
}

void phy_cfg_nr_default_t::make_harq_auto(srsran_harq_ack_cfg_hl_t&     harq,
                                          const srsran_carrier_nr_t&    carrier,
                                          const srsran_tdd_config_nr_t& tdd_cfg)
{
  // Generate as many entries as DL slots
  harq.nof_dl_data_to_ul_ack = SRSRAN_MAX(tdd_cfg.pattern1.nof_dl_slots, SRSRAN_MAX_NOF_DL_DATA_TO_UL);

  // Set PDSCH to ACK timing delay to 4 or more
  for (uint32_t n = 0; n < harq.nof_dl_data_to_ul_ack; n++) {
    // Set the first slots into the first UL slot
    if (n < (harq.nof_dl_data_to_ul_ack - 4)) {
      harq.dl_data_to_ul_ack[n] = harq.nof_dl_data_to_ul_ack - n;
      continue;
    }

    // After that try if n+4 is UL slot
    if (srsran_tdd_nr_is_ul(&tdd_cfg, carrier.scs, n + 4)) {
      harq.dl_data_to_ul_ack[n] = 4;
      continue;
    }

    // Otherwise set delay to the first UL slot of the next TDD period
    harq.dl_data_to_ul_ack[n] = 2 * harq.nof_dl_data_to_ul_ack - n;
  }

  // Zero the rest
  for (uint32_t i = harq.nof_dl_data_to_ul_ack; i < SRSRAN_MAX_NOF_DL_DATA_TO_UL; i++) {
    harq.dl_data_to_ul_ack[i] = 0;
  }

  // Select dynamic HARQ-ACK codebook
  harq.harq_ack_codebook = srsran_pdsch_harq_ack_codebook_dynamic;
}

void phy_cfg_nr_default_t::make_prach_default_lte(srsran_prach_cfg_t& prach)
{
  prach.config_idx   = 0;
  prach.freq_offset  = 2;
  prach.root_seq_idx = 0;
}

phy_cfg_nr_default_t::phy_cfg_nr_default_t(const reference_cfg_t& reference_cfg)
{
  switch (reference_cfg.carrier) {
    case reference_cfg_t::R_CARRIER_CUSTOM_10MHZ:
      make_carrier_custom_10MHz(carrier);
      break;
  }

  switch (reference_cfg.tdd) {
    case reference_cfg_t::R_TDD_CUSTOM_6_4:
      make_tdd_custom_6_4(tdd);
      break;
  }

  switch (reference_cfg.pdcch) {
    case reference_cfg_t::R_PDCCH_CUSTOM_COMMON_SS:
      make_pdcch_custom_common_ss(pdcch, carrier);
      break;
  }

  switch (reference_cfg.pdsch) {
    case reference_cfg_t::R_PDSCH_DEFAULT:
      make_pdsch_default(pdsch);
      break;
  }

  switch (reference_cfg.pusch) {
    case reference_cfg_t::R_PUSCH_DEFAULT:
      make_pusch_default(pusch);
      break;
  }

  switch (reference_cfg.pucch) {
    case reference_cfg_t::R_PUCCH_CUSTOM_ONE:
      make_pusch_default(pusch);
      break;
  }

  switch (reference_cfg.harq) {
    case reference_cfg_t::R_HARQ_AUTO:
      make_harq_auto(harq_ack, carrier, tdd);
      break;
  }

  switch (reference_cfg.prach) {
    case reference_cfg_t::R_PRACH_DEFAULT_LTE:
      make_prach_default_lte(prach);
      break;
  }
}

} // namespace srsran