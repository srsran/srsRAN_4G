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

#ifndef SRSRAN_DUMMY_GNB_STACK_H
#define SRSRAN_DUMMY_GNB_STACK_H

#include <srsran/adt/circular_array.h>
#include <srsran/interfaces/gnb_interfaces.h>
#include <srsran/interfaces/rrc_nr_interface_types.h>

class gnb_dummy_stack : public srsenb::stack_interface_phy_nr
{
private:
  srslog::basic_logger&                                                logger = srslog::fetch_basic_logger("GNB STK");
  const uint16_t                                                       rnti   = 0x1234;
  const uint32_t                                                       mcs    = 1;
  srsran::circular_array<srsran_dci_location_t, SRSRAN_NOF_SF_X_FRAME> dci_dl_location;
  srsran::circular_array<srsran_dci_location_t, SRSRAN_NOF_SF_X_FRAME> dci_ul_location;
  srsran::circular_array<uint32_t, SRSRAN_NOF_SF_X_FRAME>              dl_data_to_ul_ack;
  srsran_search_space_t                                                ss            = {};
  srsran_dci_format_nr_t                                               dci_format_ul = SRSRAN_DCI_FORMAT_NR_COUNT;
  srsran_dci_format_nr_t                                               dci_format_dl = SRSRAN_DCI_FORMAT_NR_COUNT;
  uint32_t                                                             dl_freq_res   = 0;
  uint32_t                                                             dl_time_res   = 0;
  srsran_random_t                                                      random_gen    = nullptr;
  srsran::phy_cfg_nr_t                                                 phy_cfg       = {};
  bool                                                                 valid         = false;

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
    srsran::phy_cfg_nr_t phy_cfg;                           ///< Physical layer configuration
    uint16_t             rnti                     = 0x1234; ///< C-RNTI
    uint32_t             mcs                      = 10;     ///< Modulation code scheme
    uint32_t             ss_id                    = 1;      ///< Search Space identifier
    uint32_t             pdcch_aggregation_level  = 0;      ///< PDCCH aggregation level
    uint32_t             pdcch_dl_candidate_index = 0;      ///< PDCCH DL DCI candidate index
    uint32_t             pdcch_ul_candidate_index = 0;      ///< PDCCH UL DCI candidate index
    uint32_t             dl_start_rb              = 0;      ///< Start resource block
    uint32_t             dl_length_rb             = 0l;     ///< Number of resource blocks
    uint32_t             dl_time_res              = 0;      ///< PDSCH time resource
  };

  gnb_dummy_stack(args_t args) : mcs(args.mcs), rnti(args.rnti), dl_time_res(args.dl_time_res), phy_cfg(args.phy_cfg)
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

  int rx_data_indication(rx_data_ind_t& grant) override { return 0; }

  int slot_indication(const srsran_slot_cfg_t& slot_cfg) override { return 0; }

  int get_dl_sched(const srsran_slot_cfg_t& slot_cfg, dl_sched_t& dl_sched) override
  {
    // Check if it is TDD DL slot and PDSCH mask, if no PDSCH shall be scheduled, do not set any grant and skip
    if (not srsran_tdd_nr_is_dl(&phy_cfg.tdd, phy_cfg.carrier.scs, slot_cfg.idx)) {
      return SRSRAN_SUCCESS;
    }

    // Instantiate PDCCH and PDSCH
    pdcch_dl_t pdcch = {};
    pdsch_t    pdsch = {};

    // Select grant and set data
    pdsch.data[0] = tx_harq_proc[slot_cfg.idx].data.data();

    // Second TB is not used
    pdsch.data[1] = nullptr;

    // Fill DCI configuration
    pdcch.dci_cfg = phy_cfg.get_dci_cfg();

    // Fill DCI
    srsran_dci_dl_nr_t& dci   = pdcch.dci;
    dci.ctx.location          = dci_dl_location[slot_cfg.idx];
    dci.ctx.ss_type           = ss.type;
    dci.ctx.coreset_id        = ss.coreset_id;
    dci.ctx.rnti_type         = srsran_rnti_type_c;
    dci.ctx.format            = dci_format_dl;
    dci.ctx.rnti              = rnti;
    dci.freq_domain_assigment = dl_freq_res;
    dci.time_domain_assigment = dl_time_res;
    dci.mcs                   = mcs;
    dci.rv                    = 0;
    dci.ndi                   = (slot_cfg.idx / SRSRAN_NOF_SF_X_FRAME) % 2;
    dci.pid                   = slot_cfg.idx % SRSRAN_NOF_SF_X_FRAME;
    dci.dai                   = slot_cfg.idx % SRSRAN_NOF_SF_X_FRAME;
    dci.tpc                   = 1;
    dci.pucch_resource        = 0;
    dci.harq_feedback         = dl_data_to_ul_ack[TTI_TX(slot_cfg.idx)];

    // Create PDSCH configuration
    if (srsran_ra_dl_dci_to_grant_nr(&phy_cfg.carrier, &slot_cfg, &phy_cfg.pdsch, &dci, &pdsch.sch, &pdsch.sch.grant) <
        SRSRAN_SUCCESS) {
      logger.error("Error converting DCI to grant");
      return SRSRAN_ERROR;
    }

    // Generate random data
    srsran_random_byte_vector(random_gen, pdsch.data[0], pdsch.sch.grant.tb[0].tbs / 8);

    // Set softbuffer
    pdsch.sch.grant.tb[0].softbuffer.tx = &tx_harq_proc[slot_cfg.idx].softbuffer;

    // Reset Tx softbuffer always
    srsran_softbuffer_tx_reset(pdsch.sch.grant.tb[0].softbuffer.tx);

    // Push scheduling results
    dl_sched.pdcch_dl.push_back(pdcch);
    dl_sched.pdsch.push_back(pdsch);

    return SRSRAN_SUCCESS;
  }

  int get_ul_sched(const srsran_slot_cfg_t& slot_cfg, ul_sched_t& ul_sched) override { return 0; }
};

#endif // SRSRAN_DUMMY_GNB_STACK_H
