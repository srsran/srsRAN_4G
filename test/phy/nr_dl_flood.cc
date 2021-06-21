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

#include "srsran/common/test_common.h"
#include "test_bench.h"

test_bench::args_t::args_t(int argc, char** argv)
{
  // Flag configuration as valid
  valid = true;

  cell_list.resize(1);
  cell_list[0].carrier.nof_prb         = 52;
  cell_list[0].carrier.max_mimo_layers = 1;
  cell_list[0].carrier.pci             = 500;

  phy_cfg.carrier = cell_list[0].carrier;

  phy_cfg.carrier.absolute_frequency_point_a = 633928;
  phy_cfg.carrier.absolute_frequency_ssb     = 634176;
  phy_cfg.carrier.offset_to_carrier          = 0;
  phy_cfg.carrier.scs                        = srsran_subcarrier_spacing_15kHz;
  phy_cfg.carrier.nof_prb                    = 52;

  phy_cfg.ssb.periodicity_ms       = 5;
  phy_cfg.ssb.position_in_burst[0] = true;
  phy_cfg.ssb.scs                  = srsran_subcarrier_spacing_30kHz;

  phy_cfg.pdcch.coreset_present[1]              = true;
  phy_cfg.pdcch.coreset[1].id                   = 1;
  phy_cfg.pdcch.coreset[1].freq_resources[0]    = true;
  phy_cfg.pdcch.coreset[1].freq_resources[1]    = true;
  phy_cfg.pdcch.coreset[1].freq_resources[2]    = true;
  phy_cfg.pdcch.coreset[1].freq_resources[3]    = true;
  phy_cfg.pdcch.coreset[1].freq_resources[4]    = true;
  phy_cfg.pdcch.coreset[1].freq_resources[5]    = true;
  phy_cfg.pdcch.coreset[1].freq_resources[6]    = true;
  phy_cfg.pdcch.coreset[1].freq_resources[7]    = true;
  phy_cfg.pdcch.coreset[1].duration             = 1;
  phy_cfg.pdcch.coreset[1].mapping_type         = srsran_coreset_mapping_type_non_interleaved;
  phy_cfg.pdcch.coreset[1].precoder_granularity = srsran_coreset_precoder_granularity_reg_bundle;
  phy_cfg.pdcch.coreset_present[2]              = true;
  phy_cfg.pdcch.coreset[2].id                   = 2;
  phy_cfg.pdcch.coreset[2].freq_resources[0]    = true;
  phy_cfg.pdcch.coreset[2].freq_resources[1]    = true;
  phy_cfg.pdcch.coreset[2].freq_resources[2]    = true;
  phy_cfg.pdcch.coreset[2].freq_resources[3]    = true;
  phy_cfg.pdcch.coreset[2].freq_resources[4]    = true;
  phy_cfg.pdcch.coreset[2].freq_resources[5]    = true;
  phy_cfg.pdcch.coreset[2].freq_resources[6]    = true;
  phy_cfg.pdcch.coreset[2].freq_resources[7]    = true;
  phy_cfg.pdcch.coreset[2].duration             = 1;
  phy_cfg.pdcch.coreset[2].mapping_type         = srsran_coreset_mapping_type_non_interleaved;
  phy_cfg.pdcch.coreset[2].precoder_granularity = srsran_coreset_precoder_granularity_reg_bundle;

  phy_cfg.pdcch.search_space_present[1]           = true;
  phy_cfg.pdcch.search_space[1].id                = 1;
  phy_cfg.pdcch.search_space[1].coreset_id        = 1;
  phy_cfg.pdcch.search_space[1].duration          = 1;
  phy_cfg.pdcch.search_space[1].nof_candidates[0] = 1;
  phy_cfg.pdcch.search_space[1].nof_candidates[1] = 1;
  phy_cfg.pdcch.search_space[1].nof_candidates[2] = 1;
  phy_cfg.pdcch.search_space[1].nof_candidates[3] = 0;
  phy_cfg.pdcch.search_space[1].nof_candidates[4] = 0;
  phy_cfg.pdcch.search_space[1].formats[0]        = srsran_dci_format_nr_0_0;
  phy_cfg.pdcch.search_space[1].formats[1]        = srsran_dci_format_nr_1_0;
  phy_cfg.pdcch.search_space[1].nof_formats       = 2;
  phy_cfg.pdcch.search_space[1].type              = srsran_search_space_type_common_3;

  phy_cfg.pdcch.search_space_present[2]           = true;
  phy_cfg.pdcch.search_space[2].id                = 2;
  phy_cfg.pdcch.search_space[2].coreset_id        = 2;
  phy_cfg.pdcch.search_space[2].duration          = 1;
  phy_cfg.pdcch.search_space[2].nof_candidates[0] = 0;
  phy_cfg.pdcch.search_space[2].nof_candidates[1] = 2;
  phy_cfg.pdcch.search_space[2].nof_candidates[2] = 1;
  phy_cfg.pdcch.search_space[2].nof_candidates[3] = 0;
  phy_cfg.pdcch.search_space[2].nof_candidates[4] = 0;
  phy_cfg.pdcch.search_space[2].formats[0]        = srsran_dci_format_nr_0_0;
  phy_cfg.pdcch.search_space[2].formats[1]        = srsran_dci_format_nr_1_0;
  phy_cfg.pdcch.search_space[2].nof_formats       = 2;
  phy_cfg.pdcch.search_space[2].type              = srsran_search_space_type_ue;

  phy_cfg.pdcch.ra_search_space_present = true;
  phy_cfg.pdcch.ra_search_space         = phy_cfg.pdcch.search_space[1];
  phy_cfg.pdcch.ra_search_space.type    = srsran_search_space_type_common_1;

  phy_cfg.pdsch.common_time_ra[0].mapping_type = srsran_sch_mapping_type_A;
  phy_cfg.pdsch.common_time_ra[0].sliv         = 40;
  phy_cfg.pdsch.common_time_ra[1].mapping_type = srsran_sch_mapping_type_A;
  phy_cfg.pdsch.common_time_ra[1].sliv         = 57;
  phy_cfg.pdsch.nof_common_time_ra             = 2;

  phy_cfg.pusch.common_time_ra[0].k            = 4;
  phy_cfg.pusch.common_time_ra[0].mapping_type = srsran_sch_mapping_type_A;
  phy_cfg.pusch.common_time_ra[0].sliv         = 27;
  phy_cfg.pusch.common_time_ra[1].k            = 5;
  phy_cfg.pusch.common_time_ra[1].mapping_type = srsran_sch_mapping_type_A;
  phy_cfg.pusch.common_time_ra[1].sliv         = 27;
  phy_cfg.pusch.nof_common_time_ra             = 2;

  phy_cfg.pdsch.typeA_pos = srsran_dmrs_sch_typeA_pos_2;
  phy_cfg.pusch.typeA_pos = srsran_dmrs_sch_typeA_pos_2;

  phy_cfg.tdd.pattern1.period_ms      = 10;
  phy_cfg.tdd.pattern1.nof_dl_slots   = 7;
  phy_cfg.tdd.pattern1.nof_dl_symbols = 6;
  phy_cfg.tdd.pattern1.nof_ul_slots   = 4;
  phy_cfg.tdd.pattern1.nof_ul_symbols = 4;

  phy_cfg.pdsch.dmrs_typeA.additional_pos = srsran_dmrs_sch_add_pos_1;
  phy_cfg.pdsch.dmrs_typeA.present        = true;
  phy_cfg.pdsch.alloc                     = srsran_resource_alloc_type1;

  phy_cfg.pucch.enabled               = true;
  srsran_pucch_nr_resource_t& pucch0  = phy_cfg.pucch.sets[0].resources[0];
  srsran_pucch_nr_resource_t& pucch1  = phy_cfg.pucch.sets[0].resources[1];
  srsran_pucch_nr_resource_t& pucch2  = phy_cfg.pucch.sets[0].resources[2];
  srsran_pucch_nr_resource_t& pucch3  = phy_cfg.pucch.sets[0].resources[3];
  srsran_pucch_nr_resource_t& pucch4  = phy_cfg.pucch.sets[0].resources[4];
  srsran_pucch_nr_resource_t& pucch5  = phy_cfg.pucch.sets[0].resources[5];
  srsran_pucch_nr_resource_t& pucch6  = phy_cfg.pucch.sets[0].resources[6];
  srsran_pucch_nr_resource_t& pucch7  = phy_cfg.pucch.sets[0].resources[7];
  phy_cfg.pucch.sets[0].nof_resources = 8;
  srsran_pucch_nr_resource_t& pucch8  = phy_cfg.pucch.sets[1].resources[0];
  srsran_pucch_nr_resource_t& pucch9  = phy_cfg.pucch.sets[1].resources[1];
  srsran_pucch_nr_resource_t& pucch10 = phy_cfg.pucch.sets[1].resources[2];
  srsran_pucch_nr_resource_t& pucch11 = phy_cfg.pucch.sets[1].resources[3];
  srsran_pucch_nr_resource_t& pucch12 = phy_cfg.pucch.sets[1].resources[4];
  srsran_pucch_nr_resource_t& pucch13 = phy_cfg.pucch.sets[1].resources[5];
  srsran_pucch_nr_resource_t& pucch14 = phy_cfg.pucch.sets[1].resources[6];
  srsran_pucch_nr_resource_t& pucch15 = phy_cfg.pucch.sets[1].resources[7];
  phy_cfg.pucch.sets[1].nof_resources = 8;

  pucch0.starting_prb         = 0;
  pucch0.format               = SRSRAN_PUCCH_NR_FORMAT_1;
  pucch0.initial_cyclic_shift = 0;
  pucch0.nof_symbols          = 14;
  pucch0.start_symbol_idx     = 0;
  pucch0.time_domain_occ      = 0;
  pucch1                      = pucch0;
  pucch1.initial_cyclic_shift = 4;
  pucch1.time_domain_occ      = 0;
  pucch2                      = pucch0;
  pucch2.initial_cyclic_shift = 8;
  pucch2.time_domain_occ      = 0;
  pucch3                      = pucch0;
  pucch3.initial_cyclic_shift = 0;
  pucch3.time_domain_occ      = 1;
  pucch4                      = pucch0;
  pucch4.initial_cyclic_shift = 0;
  pucch4.time_domain_occ      = 1;
  pucch5                      = pucch0;
  pucch5.initial_cyclic_shift = 4;
  pucch5.time_domain_occ      = 1;
  pucch6                      = pucch0;
  pucch6.initial_cyclic_shift = 0;
  pucch6.time_domain_occ      = 2;
  pucch7                      = pucch0;
  pucch7.initial_cyclic_shift = 4;
  pucch7.time_domain_occ      = 2;

  pucch8.starting_prb     = 51;
  pucch8.format           = SRSRAN_PUCCH_NR_FORMAT_2;
  pucch8.nof_prb          = 1;
  pucch8.nof_symbols      = 2;
  pucch8.start_symbol_idx = 0;

  pucch9                   = pucch8;
  pucch9.start_symbol_idx  = 2;
  pucch10                  = pucch8;
  pucch10.start_symbol_idx = 4;
  pucch11                  = pucch8;
  pucch11.start_symbol_idx = 6;
  pucch12                  = pucch8;
  pucch12.start_symbol_idx = 8;
  pucch13                  = pucch8;
  pucch13.start_symbol_idx = 10;
  pucch14                  = pucch8;
  pucch14.start_symbol_idx = 12;
  pucch15                  = pucch8;
  pucch15.starting_prb     = 1;
  pucch15.start_symbol_idx = 0;

  srsran_pucch_nr_resource_t& pucch16 = phy_cfg.pucch.sr_resources[1].resource;
  pucch16.starting_prb                = 0;
  pucch16.format                      = SRSRAN_PUCCH_NR_FORMAT_1;
  pucch16.initial_cyclic_shift        = 8;
  pucch16.nof_symbols                 = 14;
  pucch16.start_symbol_idx            = 0;
  pucch16.time_domain_occ             = 2;

  phy_cfg.pucch.sr_resources[1].configured = true;
  phy_cfg.pucch.sr_resources[1].sr_id      = 0;
  phy_cfg.pucch.sr_resources[1].period     = 40;
  phy_cfg.pucch.sr_resources[1].offset     = 8;
  phy_cfg.pucch.sr_resources[1].resource   = pucch16;

  phy_cfg.harq_ack.dl_data_to_ul_ack[0] = 8;
  phy_cfg.harq_ack.dl_data_to_ul_ack[1] = 7;
  phy_cfg.harq_ack.dl_data_to_ul_ack[2] = 6;
  phy_cfg.harq_ack.dl_data_to_ul_ack[3] = 5;
  phy_cfg.harq_ack.dl_data_to_ul_ack[4] = 4;
  phy_cfg.harq_ack.dl_data_to_ul_ack[5] = 12;
  phy_cfg.harq_ack.dl_data_to_ul_ack[6] = 11;

  phy_cfg.prach.freq_offset = 2;
}

class ue_dummy_stack : public srsue::stack_interface_phy_nr
{
private:
  uint16_t rnti  = 0;
  bool     valid = false;

  struct dummy_harq_proc {
    static const uint32_t  MAX_TB_SZ  = SRSRAN_LDPC_MAX_LEN_CB * SRSRAN_SCH_NR_MAX_NOF_CB_LDPC;
    srsran_softbuffer_rx_t softbuffer = {};

    dummy_harq_proc()
    {
      // Initialise softbuffer
      if (srsran_softbuffer_rx_init_guru(&softbuffer, SRSRAN_SCH_NR_MAX_NOF_CB_LDPC, SRSRAN_LDPC_MAX_LEN_ENCODED_CB) <
          SRSRAN_SUCCESS) {
        ERROR("Error Tx buffer");
      }
    }

    ~dummy_harq_proc() { srsran_softbuffer_rx_free(&softbuffer); }
  };
  srsran::circular_array<dummy_harq_proc, SRSRAN_MAX_HARQ_PROC_DL_NR> rx_harq_proc;

public:
  struct args_t {
    uint16_t rnti = 0x1234;
  };
  ue_dummy_stack(const args_t& args) : rnti(args.rnti) { valid = true; }
  void         in_sync() override {}
  void         out_of_sync() override {}
  void         run_tti(const uint32_t tti) override {}
  int          sf_indication(const uint32_t tti) override { return 0; }
  sched_rnti_t get_dl_sched_rnti_nr(const uint32_t tti) override { return {rnti, srsran_rnti_type_c}; }
  sched_rnti_t get_ul_sched_rnti_nr(const uint32_t tti) override { return {rnti, srsran_rnti_type_c}; }
  void         new_grant_dl(const uint32_t cc_idx, const mac_nr_grant_dl_t& grant, tb_action_dl_t* action) override
  {
    action->tb.enabled    = true;
    action->tb.softbuffer = &rx_harq_proc[grant.pid].softbuffer;
  }
  void tb_decoded(const uint32_t cc_idx, const mac_nr_grant_dl_t& grant, tb_action_dl_result_t result) override {}
  void new_grant_ul(const uint32_t cc_idx, const mac_nr_grant_ul_t& grant, tb_action_ul_t* action) override {}
  void prach_sent(uint32_t tti, uint32_t s_id, uint32_t t_id, uint32_t f_id, uint32_t ul_carrier_id) override {}
  bool sr_opportunity(uint32_t tti, uint32_t sr_id, bool meas_gap, bool ul_sch_tx) override { return false; }
  bool is_valid() const { return valid; }
};

class gnb_dummy_stack : public srsenb::stack_interface_phy_nr
{
private:
  srslog::basic_logger&                                                logger = srslog::fetch_basic_logger("GNB STK");
  const uint16_t                                                       rnti   = 0x1234;
  const uint32_t                                                       mcs    = 1;
  const srsran::circular_array<bool, SRSRAN_NOF_SF_X_FRAME>            pdsch_mask;
  srsran::circular_array<srsran_dci_location_t, SRSRAN_NOF_SF_X_FRAME> dci_dl_location;
  srsran::circular_array<srsran_dci_location_t, SRSRAN_NOF_SF_X_FRAME> dci_ul_location;
  srsran::circular_array<uint32_t, SRSRAN_NOF_SF_X_FRAME>              dl_data_to_ul_ack;
  bool                                                                 valid         = false;
  srsran_search_space_t                                                ss            = {};
  srsran_dci_format_nr_t                                               dci_format_ul = SRSRAN_DCI_FORMAT_NR_COUNT;
  srsran_dci_format_nr_t                                               dci_format_dl = SRSRAN_DCI_FORMAT_NR_COUNT;
  uint32_t                                                             dl_freq_res   = 0;
  uint32_t                                                             dl_time_res   = 0;
  srsran_random_t                                                      random_gen    = nullptr;

  struct dummy_harq_proc {
    static const uint32_t  MAX_TB_SZ = SRSRAN_LDPC_MAX_LEN_CB * SRSRAN_SCH_NR_MAX_NOF_CB_LDPC;
    std::vector<uint8_t>   data;
    srsran_softbuffer_tx_t softbuffer = {};

    dummy_harq_proc()
    {
      // Allocate data
      data.resize(MAX_TB_SZ);

      // Initialise softbuffer
      if (srsran_softbuffer_tx_init_guru(&softbuffer, SRSRAN_SCH_NR_MAX_NOF_CB_LDPC, SRSRAN_LDPC_MAX_LEN_ENCODED_CB) <
          SRSRAN_SUCCESS) {
        ERROR("Error Tx buffer");
      }
    }

    ~dummy_harq_proc() { srsran_softbuffer_tx_free(&softbuffer); }
  };
  srsran::circular_array<dummy_harq_proc, SRSRAN_MAX_HARQ_PROC_DL_NR> tx_harq_proc;

public:
  struct args_t {
    srsran::phy_cfg_nr_t                                phy_cfg; ///< Physical layer configuration
    uint16_t                                            rnti                     = 0x1234; ///< C-RNTI
    uint32_t                                            mcs                      = 10;     ///< Modulation code scheme
    srsran::circular_array<bool, SRSRAN_NOF_SF_X_FRAME> pdsch_mask               = {};     ///< PDSCH scheduling mask
    uint32_t                                            ss_id                    = 1;      ///< Search Space identifier
    uint32_t                                            pdcch_aggregation_level  = 0;      ///< PDCCH aggregation level
    uint32_t                                            pdcch_dl_candidate_index = 0;  ///< PDCCH DL DCI candidate index
    uint32_t                                            pdcch_ul_candidate_index = 0;  ///< PDCCH UL DCI candidate index
    uint32_t                                            dl_start_rb              = 0;  ///< Start resource block
    uint32_t                                            dl_length_rb             = 0l; ///< Number of resource blocks
    uint32_t                                            dl_time_res              = 0;  ///< PDSCH time resource
  };

  gnb_dummy_stack(args_t args) :
    pdsch_mask(args.pdsch_mask),
    mcs(args.mcs),
    rnti(args.rnti),
    dl_time_res(args.dl_time_res)
  {
    random_gen = srsran_random_init(0x1234);

    // Select search space
    if (args.ss_id >= SRSRAN_UE_DL_NR_MAX_NOF_SEARCH_SPACE) {
      logger.error("Search Space Id  (%d) is out-of-range (%d)", args.ss_id, SRSRAN_UE_DL_NR_MAX_NOF_SEARCH_SPACE);
      return;
    }
    if (not args.phy_cfg.pdcch.search_space_present[args.ss_id]) {
      logger.error("Search Space Id  (%d) is not present", args.ss_id);
      return;
    }
    ss = args.phy_cfg.pdcch.search_space[args.ss_id];

    // Select CORESET
    if (ss.coreset_id >= SRSRAN_UE_DL_NR_MAX_NOF_CORESET) {
      logger.error("CORESET Id  (%d) is out-of-range (%d)", ss.coreset_id, SRSRAN_UE_DL_NR_MAX_NOF_CORESET);
      return;
    }
    if (not args.phy_cfg.pdcch.coreset_present[ss.coreset_id]) {
      logger.error("CORESET Id  (%d) is not present", args.ss_id);
      return;
    }
    const srsran_coreset_t& coreset = args.phy_cfg.pdcch.coreset[ss.coreset_id];

    // Select DCI locations
    for (uint32_t slot = 0; slot < SRSRAN_NOF_SF_X_FRAME; slot++) {
      std::array<uint32_t, SRSRAN_SEARCH_SPACE_MAX_NOF_CANDIDATES_NR> ncce = {};
      int n = srsran_pdcch_nr_locations_coreset(&coreset, &ss, rnti, args.pdcch_aggregation_level, slot++, ncce.data());
      if (n < SRSRAN_SUCCESS) {
        logger.error(
            "Error generating locations for slot %d and aggregation level %d", slot, args.pdcch_aggregation_level);
        return;
      }
      uint32_t nof_candidates = (uint32_t)n;

      // DCI DL
      if (args.pdcch_dl_candidate_index >= nof_candidates or
          args.pdcch_dl_candidate_index >= SRSRAN_SEARCH_SPACE_MAX_NOF_CANDIDATES_NR) {
        logger.error("Candidate index %d exceeds the number of candidates %d for aggregation level %d",
                     args.pdcch_dl_candidate_index,
                     n,
                     args.pdcch_aggregation_level);
        return;
      }
      dci_dl_location[slot].L    = args.pdcch_aggregation_level;
      dci_dl_location[slot].ncce = ncce[args.pdcch_dl_candidate_index];

      // DCI UL
      if (args.pdcch_ul_candidate_index >= nof_candidates or
          args.pdcch_ul_candidate_index >= SRSRAN_SEARCH_SPACE_MAX_NOF_CANDIDATES_NR) {
        logger.error("Candidate index %d exceeds the number of candidates %d for aggregation level %d",
                     args.pdcch_ul_candidate_index,
                     n,
                     args.pdcch_aggregation_level);
        return;
      }
      dci_ul_location[slot].L    = args.pdcch_aggregation_level;
      dci_ul_location[slot].ncce = ncce[args.pdcch_ul_candidate_index];
    }

    // Select DCI formats
    for (uint32_t i = 0; i < ss.nof_formats; i++) {
      // Select DL format
      if (ss.formats[i] == srsran_dci_format_nr_1_0 or ss.formats[i] == srsran_dci_format_nr_1_1) {
        dci_format_dl = ss.formats[i];
      }

      // Select DL format
      if (ss.formats[i] == srsran_dci_format_nr_0_0 or ss.formats[i] == srsran_dci_format_nr_0_1) {
        dci_format_ul = ss.formats[i];
      }
    }

    // Validate that a DCI format is selected
    if (dci_format_dl == SRSRAN_DCI_FORMAT_NR_COUNT or dci_format_ul == SRSRAN_DCI_FORMAT_NR_COUNT) {
      logger.error("Missing valid DL or UL DCI format in search space");
      return;
    }

    // Select DL frequency domain resources
    dl_freq_res = srsran_ra_nr_type1_riv(args.phy_cfg.carrier.nof_prb, args.dl_start_rb, args.dl_length_rb);

    // Setup DL Data to ACK timing
    for (uint32_t i = 0; i < SRSRAN_NOF_SF_X_FRAME; i++) {
      dl_data_to_ul_ack[i] = args.phy_cfg.harq_ack.dl_data_to_ul_ack[i % SRSRAN_MAX_NOF_DL_DATA_TO_UL];
    }

    // If reached this point the configuration is valid
    valid = true;
  }

  ~gnb_dummy_stack() { srsran_random_free(random_gen); }

  bool is_valid() const { return valid; }

  int sf_indication(const uint32_t tti) override { return 0; }
  int rx_data_indication(rx_data_ind_t& grant) override { return 0; }

  int get_dl_sched(uint32_t tti, dl_sched_list_t& dl_sched_res) override
  {
    // Check input
    if (dl_sched_res.size() == 0) {
      return SRSRAN_ERROR;
    }

    // Check PDSCH mask, if no PDSCH shall be scheduled, do not set any grant and skip
    if (not pdsch_mask[tti]) {
      dl_sched_res[0].nof_grants = 0;
      return SRSRAN_SUCCESS;
    }

    // Select grant and set data
    dl_sched_grant_t& grant = dl_sched_res[0].pdsch[0];
    grant.data[0]           = tx_harq_proc[tti].data.data();
    grant.softbuffer_tx[0]  = &tx_harq_proc[tti].softbuffer;

    // Second TB is not used
    grant.data[1]          = nullptr;
    grant.softbuffer_tx[1] = nullptr;

    // Reset Tx softbuffer always
    srsran_softbuffer_tx_reset(grant.softbuffer_tx[0]);

    // Generate random data
    srsran_random_byte_vector(random_gen, grant.data[0], SRSRAN_LDPC_MAX_LEN_CB * SRSRAN_SCH_NR_MAX_NOF_CB_LDPC / 8);

    // It currently support only one grant
    dl_sched_res[0].nof_grants = 1;

    // Fill DCI
    srsran_dci_dl_nr_t& dci   = grant.dci;
    dci.ctx.location          = dci_dl_location[tti];
    dci.ctx.ss_type           = ss.type;
    dci.ctx.coreset_id        = ss.coreset_id;
    dci.ctx.rnti_type         = srsran_rnti_type_c;
    dci.ctx.format            = dci_format_dl;
    dci.ctx.rnti              = rnti;
    dci.freq_domain_assigment = dl_freq_res;
    dci.time_domain_assigment = dl_time_res;
    dci.mcs                   = mcs;
    dci.rv                    = 0;
    dci.ndi                   = (tti / SRSRAN_NOF_SF_X_FRAME) % 2;
    dci.pid                   = tti % SRSRAN_NOF_SF_X_FRAME;
    dci.dai                   = tti % SRSRAN_NOF_SF_X_FRAME;
    dci.tpc                   = 1;
    dci.pucch_resource        = 0;
    dci.harq_feedback         = dl_data_to_ul_ack[tti];

    return SRSRAN_SUCCESS;
  }

  int get_ul_sched(uint32_t tti, ul_sched_list_t& ul_sched_res) override { return 0; }
};

int main(int argc, char** argv)
{
  srslog::init();

  // Parse test bench arguments
  test_bench::args_t args(argc, argv);
  args.gnb_args.log_id_preamble  = "GNB/";
  args.gnb_args.log_level        = "warning";
  args.gnb_args.nof_workers      = 1;
  args.ue_args.log.id_preamble   = " UE/";
  args.ue_args.log.phy_level     = "warning";
  args.ue_args.log.phy_hex_limit = 0;
  args.ue_args.nof_phy_threads   = 1;

  // Parse arguments
  TESTASSERT(args.valid);

  // Create UE stack arguments
  ue_dummy_stack::args_t ue_stack_args = {};
  ue_stack_args.rnti                   = 0x1234;

  // Create UE stack
  ue_dummy_stack ue_stack(ue_stack_args);
  TESTASSERT(ue_stack.is_valid());

  // Create GNB stack arguments
  gnb_dummy_stack::args_t gnb_stack_args = {};
  gnb_stack_args.rnti                    = 0x1234;
  gnb_stack_args.mcs                     = 10;
  for (bool& mask : gnb_stack_args.pdsch_mask) {
    mask = true;
  }
  gnb_stack_args.phy_cfg      = args.phy_cfg;
  gnb_stack_args.dl_start_rb  = 0;
  gnb_stack_args.dl_length_rb = args.phy_cfg.carrier.nof_prb;

  // Create GNB stack
  gnb_dummy_stack gnb_stack(gnb_stack_args);
  TESTASSERT(gnb_stack.is_valid());

  // Create test bench
  test_bench tb(args, gnb_stack, ue_stack);

  // Assert bench is initialised correctly
  TESTASSERT(tb.is_initialised());

  for (uint32_t i = 0; i < 20; i++) {
    TESTASSERT(tb.run_tti());
  }

  // If reached here, the test is successful
  return SRSRAN_SUCCESS;
}
