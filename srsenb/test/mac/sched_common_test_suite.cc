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

#include "sched_common_test_suite.h"
#include "srslte/common/test_common.h"
#include <set>

using srslte::tti_point;

namespace srsenb {

int test_pusch_collisions(const sf_output_res_t& sf_out, uint32_t enb_cc_idx, const prbmask_t* expected_ul_mask)
{
  auto&     cell_params = sf_out.cc_params[enb_cc_idx];
  auto&     ul_result   = sf_out.ul_cc_result[enb_cc_idx];
  uint32_t  nof_prb     = cell_params.nof_prb();
  prbmask_t ul_allocs(nof_prb);

  auto try_ul_fill = [&](prb_interval alloc, const char* ch_str, bool strict = true) {
    CONDERROR(alloc.stop() > nof_prb, "Allocated RBs %s out-of-bounds", alloc.to_string().c_str());
    CONDERROR(alloc.empty(), "Allocations must have at least one PRB");
    if (strict and ul_allocs.any(alloc.start(), alloc.stop())) {
      TESTERROR("Collision Detected of %s alloc=%s and cumulative_mask=0x%s",
                ch_str,
                alloc.to_string().c_str(),
                ul_allocs.to_hex().c_str());
    }
    ul_allocs.fill(alloc.start(), alloc.stop(), true);
    return SRSLTE_SUCCESS;
  };

  /* TEST: Check if there is space for PRACH */
  bool is_prach_tti_tx_ul =
      srslte_prach_tti_opportunity_config_fdd(cell_params.cfg.prach_config, to_tx_ul(sf_out.tti_rx).to_uint(), -1);
  if (is_prach_tti_tx_ul) {
    try_ul_fill({cell_params.cfg.prach_freq_offset, cell_params.cfg.prach_freq_offset + 6}, "PRACH");
  }

  /* TEST: check collisions in PUCCH */
  bool strict = nof_prb != 6 or (not is_prach_tti_tx_ul); // and not tti_data.ul_pending_msg3_present);
  try_ul_fill({0, (uint32_t)cell_params.cfg.nrb_pucch}, "PUCCH", strict);
  try_ul_fill({cell_params.cfg.cell.nof_prb - cell_params.cfg.nrb_pucch, (uint32_t)cell_params.cfg.cell.nof_prb},
              "PUCCH",
              strict);

  /* TEST: check collisions in the UL PUSCH */
  for (uint32_t i = 0; i < ul_result.nof_dci_elems; ++i) {
    uint32_t L, RBstart;
    srslte_ra_type2_from_riv(ul_result.pusch[i].dci.type2_alloc.riv, &L, &RBstart, nof_prb, nof_prb);
    strict = ul_result.pusch[i].needs_pdcch or nof_prb != 6; // Msg3 may collide with PUCCH at PRB==6
    try_ul_fill({RBstart, RBstart + L}, "PUSCH", strict);
  }

  CONDERROR(expected_ul_mask != nullptr and *expected_ul_mask != ul_allocs,
            "The derived UL PRB mask %s does not match the expected one %s",
            ul_allocs.to_string().c_str(),
            expected_ul_mask->to_string().c_str());

  return SRSLTE_SUCCESS;
}

int extract_dl_prbmask(const srslte_cell_t&               cell,
                       const srslte_dci_dl_t&             dci,
                       srslte::bounded_bitset<100, true>& alloc_mask)
{
  srslte_pdsch_grant_t grant;
  srslte_dl_sf_cfg_t   dl_sf = {};

  alloc_mask.resize(cell.nof_prb);
  alloc_mask.reset();

  CONDERROR(srslte_ra_dl_dci_to_grant(&cell, &dl_sf, SRSLTE_TM1, false, &dci, &grant) == SRSLTE_ERROR,
            "Failed to decode PDSCH grant");
  for (uint32_t j = 0; j < alloc_mask.size(); ++j) {
    if (grant.prb_idx[0][j]) {
      alloc_mask.set(j);
    }
  }
  return SRSLTE_SUCCESS;
}

int test_pdsch_collisions(const sf_output_res_t& sf_out, uint32_t enb_cc_idx, const rbgmask_t* expected_rbgmask)
{
  auto&                             cell_params = sf_out.cc_params[enb_cc_idx];
  auto&                             dl_result   = sf_out.dl_cc_result[enb_cc_idx];
  srslte::bounded_bitset<100, true> dl_allocs(cell_params.nof_prb()), alloc_mask(cell_params.nof_prb());
  rbgmask_t                         rbgmask{cell_params.nof_rbgs};

  auto try_dl_mask_fill = [&](const srslte_dci_dl_t& dci, const char* channel) {
    if (extract_dl_prbmask(cell_params.cfg.cell, dci, alloc_mask) != SRSLTE_SUCCESS) {
      return SRSLTE_ERROR;
    }
    CONDERROR(alloc_mask.none(), "DL allocation must occupy at least one RBG.");
    if ((dl_allocs & alloc_mask).any()) {
      TESTERROR("Detected collision in the DL %s allocation (%s intersects %s)",
                channel,
                dl_allocs.to_string().c_str(),
                alloc_mask.to_string().c_str());
    }
    dl_allocs |= alloc_mask;
    return SRSLTE_SUCCESS;
  };

  // Decode BC allocations, check collisions, and fill cumulative mask
  for (uint32_t i = 0; i < dl_result.nof_bc_elems; ++i) {
    TESTASSERT(try_dl_mask_fill(dl_result.bc[i].dci, "BC") == SRSLTE_SUCCESS);
  }

  // Decode RAR allocations, check collisions, and fill cumulative mask
  for (uint32_t i = 0; i < dl_result.nof_rar_elems; ++i) {
    TESTASSERT(try_dl_mask_fill(dl_result.rar[i].dci, "RAR") == SRSLTE_SUCCESS);
  }

  // forbid Data in DL if its ACKs conflict with PRACH for PRB==6
  if (cell_params.nof_prb() == 6) {
    if (srslte_prach_tti_opportunity_config_fdd(
            cell_params.cfg.prach_config, to_tx_dl_ack(sf_out.tti_rx).to_uint(), -1)) {
      dl_allocs.fill(0, dl_allocs.size());
    }
  }

  // Decode Data allocations, check collisions and fill cumulative mask
  for (uint32_t i = 0; i < dl_result.nof_data_elems; ++i) {
    TESTASSERT(try_dl_mask_fill(dl_result.data[i].dci, "data") == SRSLTE_SUCCESS);
  }

  // TEST: check for holes in the PRB mask (RBGs not fully filled)
  rbgmask.resize(cell_params.nof_rbgs);
  rbgmask.reset();
  srslte::bounded_bitset<100, true> rev_alloc = ~dl_allocs;
  for (uint32_t i = 0; i < cell_params.nof_rbgs; ++i) {
    uint32_t lim = SRSLTE_MIN((i + 1) * cell_params.P, dl_allocs.size());
    bool     val = dl_allocs.any(i * cell_params.P, lim);
    CONDERROR(rev_alloc.any(i * cell_params.P, lim) and val, "No holes can be left in an RBG");
    if (val) {
      rbgmask.set(i);
    }
  }

  CONDERROR(expected_rbgmask != nullptr and *expected_rbgmask != rbgmask,
            "The derived DL RBG mask %s does not match the expected one %s",
            rbgmask.to_string().c_str(),
            expected_rbgmask->to_string().c_str());

  return SRSLTE_SUCCESS;
}

/**
 * - SIB1 is allocated in correct TTIs
 * - TB size is adequate for SIB allocation
 * - The SIBs with index>1 are allocated in expected TTI windows
 */
int test_sib_scheduling(const sf_output_res_t& sf_out, uint32_t enb_cc_idx)
{
  const auto& cell_params   = sf_out.cc_params[enb_cc_idx];
  const auto& dl_result     = sf_out.dl_cc_result[enb_cc_idx];
  uint32_t    sfn           = to_tx_dl(sf_out.tti_rx).to_uint() / 10;
  uint32_t    sf_idx        = to_tx_dl(sf_out.tti_rx).to_uint() % 10;
  bool        sib1_expected = ((sfn % 2) == 0) and sf_idx == 5;

  using bc_elem     = const sched_interface::dl_sched_bc_t;
  bc_elem* bc_begin = &dl_result.bc[0];
  bc_elem* bc_end   = &dl_result.bc[dl_result.nof_bc_elems];

  /* Test if SIB1 was correctly scheduled */
  auto it = std::find_if(bc_begin, bc_end, [](bc_elem& elem) { return elem.index == 0; });
  CONDERROR(sib1_expected and it == bc_end, "Failed to allocate SIB1 in even sfn, sf_idx==5");
  CONDERROR(not sib1_expected and it != bc_end, "SIB1 allocated in wrong TTI.");

  /* Test if any SIB was scheduled with wrong index, tbs, or outside of its window */
  for (bc_elem* bc = bc_begin; bc != bc_end; ++bc) {
    if (bc->index == 0) {
      continue;
    }
    CONDERROR(bc->index >= sched_interface::MAX_SIBS, "Invalid SIB idx=%d", bc->index + 1);
    CONDERROR(bc->tbs < cell_params.cfg.sibs[bc->index].len,
              "Allocated BC process with TBS=%d < sib_len=%d",
              bc->tbs,
              cell_params.cfg.sibs[bc->index].len);
    uint32_t x         = (bc->index - 1) * cell_params.cfg.si_window_ms;
    uint32_t sf        = x % 10;
    uint32_t sfn_start = sfn;
    while ((sfn_start % cell_params.cfg.sibs[bc->index].period_rf) != x / 10) {
      sfn_start--;
    }
    srslte::tti_point    win_start{sfn_start * 10 + sf};
    srslte::tti_interval window{win_start, win_start + cell_params.cfg.si_window_ms};
    CONDERROR(not window.contains(to_tx_dl(sf_out.tti_rx)), "Scheduled SIB is outside of its SIB window");
  }
  return SRSLTE_SUCCESS;
}

int test_pdcch_collisions(const sf_output_res_t&                   sf_out,
                          uint32_t                                 enb_cc_idx,
                          const srslte::bounded_bitset<128, true>* expected_cce_mask)
{
  const auto& cell_params = sf_out.cc_params[enb_cc_idx];
  const auto& dl_result   = sf_out.dl_cc_result[enb_cc_idx];
  const auto& ul_result   = sf_out.ul_cc_result[enb_cc_idx];
  int         ret         = srslte_regs_pdcch_ncce(cell_params.regs.get(), dl_result.cfi);
  TESTASSERT(ret > 0);
  uint32_t                          ncce = ret;
  srslte::bounded_bitset<128, true> used_cce{ncce};

  // Helper Function: checks if there is any collision. If not, fills the PDCCH mask
  auto try_cce_fill = [&](const srslte_dci_location_t& dci_loc, const char* ch) {
    uint32_t cce_start = dci_loc.ncce, cce_stop = dci_loc.ncce + (1u << dci_loc.L);
    CONDERROR(dci_loc.L == 0, "The aggregation level %d is not valid", dci_loc.L);
    CONDERROR(
        cce_start >= ncce or cce_stop > ncce, "The CCE positions (%u, %u) do not fit in PDCCH", cce_start, cce_stop);
    CONDERROR(
        used_cce.any(cce_start, cce_stop), "%s DCI collision between CCE positions (%u, %u)", ch, cce_start, cce_stop);
    used_cce.fill(cce_start, cce_stop);
    return SRSLTE_SUCCESS;
  };

  /* TEST: verify there are no dci collisions for UL, DL data, BC, RAR */
  for (uint32_t i = 0; i < ul_result.nof_dci_elems; ++i) {
    const auto& pusch = ul_result.pusch[i];
    if (not pusch.needs_pdcch) {
      // In case of non-adaptive retx or Msg3
      continue;
    }
    try_cce_fill(pusch.dci.location, "UL");
  }
  for (uint32_t i = 0; i < dl_result.nof_data_elems; ++i) {
    try_cce_fill(dl_result.data[i].dci.location, "DL data");
  }
  for (uint32_t i = 0; i < dl_result.nof_bc_elems; ++i) {
    try_cce_fill(dl_result.bc[i].dci.location, "DL BC");
  }
  for (uint32_t i = 0; i < dl_result.nof_rar_elems; ++i) {
    try_cce_fill(dl_result.rar[i].dci.location, "DL RAR");
  }

  CONDERROR(expected_cce_mask != nullptr and *expected_cce_mask != used_cce,
            "The derived PDCCH mask %s does not match the expected one %s",
            used_cce.to_string().c_str(),
            expected_cce_mask->to_string().c_str());

  return SRSLTE_SUCCESS;
}

int test_dci_content_common(const sf_output_res_t& sf_out, uint32_t enb_cc_idx)
{
  const auto& cell_params = sf_out.cc_params[enb_cc_idx];
  const auto& dl_result   = sf_out.dl_cc_result[enb_cc_idx];
  const auto& ul_result   = sf_out.ul_cc_result[enb_cc_idx];

  std::set<uint16_t> alloc_rntis;
  for (uint32_t i = 0; i < ul_result.nof_dci_elems; ++i) {
    const auto& pusch = ul_result.pusch[i];
    uint16_t    rnti  = pusch.dci.rnti;
    CONDERROR(pusch.tbs == 0, "Allocated PUSCH with invalid TBS=%d", pusch.tbs);
    CONDERROR(alloc_rntis.count(rnti) > 0, "The user rnti=0x%x got allocated multiple times in UL", rnti);
    alloc_rntis.insert(pusch.dci.rnti);
    CONDERROR(not((pusch.current_tx_nb == 0) xor (pusch.dci.tb.rv != 0)), "Number of txs incorrectly set");
    if (not pusch.needs_pdcch) {
      // In case of non-adaptive retx or Msg3
      continue;
    }
    if (pusch.dci.tb.rv == 0) {
      // newTx
      CONDERROR(pusch.dci.format != SRSLTE_DCI_FORMAT0, "Incorrect UL DCI format");
      CONDERROR(pusch.dci.tb.mcs_idx > 28, "Incorrect UL MCS index");
    }
  }

  alloc_rntis.clear();
  for (uint32_t i = 0; i < dl_result.nof_data_elems; ++i) {
    auto&    data = dl_result.data[i];
    uint16_t rnti = data.dci.rnti;
    CONDERROR(data.tbs[0] == 0 and data.tbs[1] == 0, "Allocated DL data has empty TBS");
    CONDERROR(alloc_rntis.count(rnti) > 0, "The user rnti=0x%x got allocated multiple times in DL", rnti);
    alloc_rntis.insert(data.dci.rnti);
    for (uint32_t tb = 0; tb < 2; ++tb) {
      if (data.tbs[tb] == 0) {
        continue;
      }
      if (data.dci.tb[tb].rv == 0) {
        // newTx
        CONDERROR(data.nof_pdu_elems[tb] == 0, "Allocated DL grant does not have MAC SDUs");
        CONDERROR(data.nof_pdu_elems[tb] > sched_interface::MAX_RLC_PDU_LIST,
                  "Number of SDUs in DL grant exceeds limit");
        uint32_t alloc_bytes = 0;
        for (uint32_t pdu = 0; pdu < data.nof_pdu_elems[tb]; ++pdu) {
          alloc_bytes += data.pdu[tb][pdu].nbytes;
        }
        CONDERROR(alloc_bytes > data.tbs[tb], "The bytes allocated to individual MAC SDUs is larger than total TBS");
        CONDERROR(data.dci.tb[tb].mcs_idx > 28, "Incorrect DL MCS index");
      }
    }
  }
  for (uint32_t i = 0; i < dl_result.nof_bc_elems; ++i) {
    auto& bc = dl_result.bc[i];
    if (bc.type == sched_interface::dl_sched_bc_t::BCCH) {
      CONDERROR(bc.tbs < cell_params.cfg.sibs[bc.index].len,
                "Allocated BC process with TBS=%d < sib_len=%d",
                bc.tbs,
                cell_params.cfg.sibs[bc.index].len);
    } else if (bc.type == sched_interface::dl_sched_bc_t::PCCH) {
      CONDERROR(bc.tbs == 0, "Allocated paging process with invalid TBS=%d", bc.tbs);
    } else {
      TESTERROR("Invalid broadcast process id=%d", (int)bc.type);
    }
  }
  for (uint32_t i = 0; i < dl_result.nof_rar_elems; ++i) {
    const auto& rar = dl_result.rar[i];
    CONDERROR(rar.tbs == 0, "Allocated RAR process with invalid TBS=%d", rar.tbs);
  }

  return SRSLTE_SUCCESS;
}

int test_all_common(const sf_output_res_t& sf_out)
{
  for (uint32_t i = 0; i < sf_out.cc_params.size(); ++i) {
    TESTASSERT(test_pusch_collisions(sf_out, i, nullptr) == SRSLTE_SUCCESS);
    TESTASSERT(test_pdsch_collisions(sf_out, i, nullptr) == SRSLTE_SUCCESS);
    TESTASSERT(test_sib_scheduling(sf_out, i) == SRSLTE_SUCCESS);
    TESTASSERT(test_pdcch_collisions(sf_out, i, nullptr) == SRSLTE_SUCCESS);
    TESTASSERT(test_dci_content_common(sf_out, i) == SRSLTE_SUCCESS);
  }
  return SRSLTE_SUCCESS;
}

} // namespace srsenb
