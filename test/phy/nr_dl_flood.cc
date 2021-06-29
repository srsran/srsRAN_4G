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

#include "dummy_gnb_stack.h"
#include "srsran/common/test_common.h"
#include "test_bench.h"

test_bench::args_t::args_t(int argc, char** argv)
{
  // Flag configuration as valid
  valid = true;

  phy_cfg.carrier.nof_prb                    = 52;
  phy_cfg.carrier.max_mimo_layers            = 1;
  phy_cfg.carrier.pci                        = 500;
  phy_cfg.carrier.absolute_frequency_point_a = 633928;
  phy_cfg.carrier.absolute_frequency_ssb     = 634176;
  phy_cfg.carrier.offset_to_carrier          = 0;
  phy_cfg.carrier.scs                        = srsran_subcarrier_spacing_15kHz;

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
  phy_cfg.pdcch.search_space[1].formats[0]        = srsran_dci_format_nr_0_0; // DCI format for PUSCH
  phy_cfg.pdcch.search_space[1].formats[1]        = srsran_dci_format_nr_1_0; // DCI format for PDSCH
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
  phy_cfg.pdcch.search_space[2].formats[0]        = srsran_dci_format_nr_0_1;
  phy_cfg.pdcch.search_space[2].formats[1]        = srsran_dci_format_nr_1_1;
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
  phy_cfg.tdd.pattern1.nof_dl_slots   = 5;
  phy_cfg.tdd.pattern1.nof_dl_symbols = 0;
  phy_cfg.tdd.pattern1.nof_ul_slots   = 5;
  phy_cfg.tdd.pattern1.nof_ul_symbols = 0;

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
  phy_cfg.harq_ack.dl_data_to_ul_ack[5] = 4;
  phy_cfg.harq_ack.dl_data_to_ul_ack[6] = 4;
  phy_cfg.harq_ack.dl_data_to_ul_ack[7] = 4;
  phy_cfg.harq_ack.harq_ack_codebook    = srsran_pdsch_harq_ack_codebook_dynamic;

  phy_cfg.prach.freq_offset = 2;

  cell_list.resize(1);
  cell_list[0].carrier = phy_cfg.carrier;
  cell_list[0].rf_port = 0;
  cell_list[0].cell_id = 0;
  cell_list[0].pdcch   = phy_cfg.pdcch;
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

int main(int argc, char** argv)
{
  srslog::init();

  // Parse test bench arguments
  test_bench::args_t args(argc, argv);
  args.gnb_args.log_id_preamble  = "GNB/";
  args.gnb_args.log_level        = "info";
  args.gnb_args.nof_phy_threads  = 1;
  args.ue_args.log.id_preamble   = " UE/";
  args.ue_args.log.phy_level     = "info";
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
  gnb_stack_args.phy_cfg                 = args.phy_cfg;
  gnb_stack_args.dl_start_rb             = 0;
  gnb_stack_args.dl_length_rb            = args.phy_cfg.carrier.nof_prb;

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
