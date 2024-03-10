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

#include "sched_common_test_suite.h"
#include "srsran/common/test_common.h"
#include <set>

using srsran::tti_point;

namespace srsenb {

int test_pusch_collisions(const sf_output_res_t& sf_out, uint32_t enb_cc_idx, const prbmask_t* expected_ul_mask)
{
  auto&     cell_params = sf_out.cc_params[enb_cc_idx];
  auto&     ul_result   = sf_out.ul_cc_result[enb_cc_idx];
  uint32_t  nof_prb     = cell_params.nof_prb();
  prbmask_t ul_allocs(nof_prb);

  auto try_ul_fill = [&](prb_interval alloc, const char* ch_str, bool strict = true) {
    CONDERROR(alloc.stop() > nof_prb, "Allocated RBs %s out-of-bounds", fmt::format("{}", alloc).c_str());
    CONDERROR(alloc.empty(), "Allocations must have at least one PRB");
    CONDERROR(strict and ul_allocs.any(alloc.start(), alloc.stop()),
              "Collision Detected of %s alloc=%s and cumulative_mask=0x%s",
              ch_str,
              fmt::format("{}", alloc).c_str(),
              fmt::format("{:x}", ul_allocs).c_str());
    ul_allocs.fill(alloc.start(), alloc.stop(), true);
    return SRSRAN_SUCCESS;
  };

  /* TEST: Check if there is space for PRACH */
  bool is_prach_tti_tx_ul =
      srsran_prach_in_window_config_fdd(cell_params.cfg.prach_config, to_tx_ul(sf_out.tti_rx).to_uint(), -1);
  if (is_prach_tti_tx_ul) {
    try_ul_fill({cell_params.cfg.prach_freq_offset, cell_params.cfg.prach_freq_offset + 6}, "PRACH");
  }

  /* TEST: check collisions in PUCCH */
  bool               strict    = nof_prb != 6 or (not is_prach_tti_tx_ul); // and not tti_data.ul_pending_msg3_present);
  uint32_t           pucch_nrb = (cell_params.cfg.nrb_pucch > 0) ? (uint32_t)cell_params.cfg.nrb_pucch : 0;
  srsran_pucch_cfg_t pucch_cfg = cell_params.pucch_cfg_common;
  pucch_cfg.n_pucch            = cell_params.nof_cce_table[SRSRAN_NOF_CFI - 1] - 1 + cell_params.cfg.n1pucch_an;
  pucch_nrb                    = std::max(pucch_nrb, srsran_pucch_m(&pucch_cfg, cell_params.cfg.cell.cp) / 2 + 1);
  try_ul_fill({0, pucch_nrb}, "PUCCH", strict);
  try_ul_fill({cell_params.cfg.cell.nof_prb - pucch_nrb, (uint32_t)cell_params.cfg.cell.nof_prb}, "PUCCH", strict);

  /* TEST: check collisions in the UL PUSCH */
  for (uint32_t i = 0; i < ul_result.pusch.size(); ++i) {
    uint32_t L, RBstart;
    srsran_ra_type2_from_riv(ul_result.pusch[i].dci.type2_alloc.riv, &L, &RBstart, nof_prb, nof_prb);
    strict = ul_result.pusch[i].needs_pdcch or nof_prb != 6; // Msg3 may collide with PUCCH at PRB==6
    try_ul_fill({RBstart, RBstart + L}, "PUSCH", strict);
  }

  CONDERROR(expected_ul_mask != nullptr and *expected_ul_mask != ul_allocs,
            "The derived UL PRB mask %s does not match the expected one %s",
            fmt::format("{}", ul_allocs).c_str(),
            fmt::format("{}", *expected_ul_mask).c_str());

  return SRSRAN_SUCCESS;
}

int extract_dl_prbmask(const srsran_cell_t&               cell,
                       const srsran_dci_dl_t&             dci,
                       srsran::bounded_bitset<100, true>& alloc_mask)
{
  srsran_pdsch_grant_t grant;
  srsran_dl_sf_cfg_t   dl_sf = {};

  alloc_mask.resize(cell.nof_prb);
  alloc_mask.reset();

  CONDERROR(srsran_ra_dl_dci_to_grant(&cell, &dl_sf, SRSRAN_TM1, false, &dci, &grant) == SRSRAN_ERROR,
            "Failed to decode PDSCH grant");
  for (uint32_t j = 0; j < alloc_mask.size(); ++j) {
    if (grant.prb_idx[0][j]) {
      alloc_mask.set(j);
    }
  }
  return SRSRAN_SUCCESS;
}

int test_pdsch_collisions(const sf_output_res_t& sf_out, uint32_t enb_cc_idx, const rbgmask_t* expected_rbgmask)
{
  auto&                             cell_params = sf_out.cc_params[enb_cc_idx];
  auto&                             dl_result   = sf_out.dl_cc_result[enb_cc_idx];
  srsran::bounded_bitset<100, true> dl_allocs(cell_params.nof_prb()), alloc_mask(cell_params.nof_prb());
  rbgmask_t                         rbgmask{cell_params.nof_rbgs};

  auto try_dl_mask_fill = [&](const srsran_dci_dl_t& dci, const char* channel) {
    if (extract_dl_prbmask(cell_params.cfg.cell, dci, alloc_mask) != SRSRAN_SUCCESS) {
      return SRSRAN_ERROR;
    }
    CONDERROR(alloc_mask.none(), "DL allocation must occupy at least one RBG.");
    CONDERROR((dl_allocs & alloc_mask).any(),
              "Detected collision in the DL %s allocation (%s intersects %s)",
              channel,
              fmt::format("{}", dl_allocs).c_str(),
              fmt::format("{}", alloc_mask).c_str());
    dl_allocs |= alloc_mask;
    return SRSRAN_SUCCESS;
  };

  // Decode BC allocations, check collisions, and fill cumulative mask
  for (uint32_t i = 0; i < dl_result.bc.size(); ++i) {
    TESTASSERT(try_dl_mask_fill(dl_result.bc[i].dci, "BC") == SRSRAN_SUCCESS);
  }

  // Decode RAR allocations, check collisions, and fill cumulative mask
  for (uint32_t i = 0; i < dl_result.rar.size(); ++i) {
    TESTASSERT(try_dl_mask_fill(dl_result.rar[i].dci, "RAR") == SRSRAN_SUCCESS);
  }

  // forbid Data in DL if its ACKs conflict with PRACH for PRB==6
  if (cell_params.nof_prb() == 6) {
    if (srsran_prach_in_window_config_fdd(cell_params.cfg.prach_config, to_tx_dl_ack(sf_out.tti_rx).to_uint(), -1)) {
      dl_allocs.fill(0, dl_allocs.size());
    }
  }

  // Decode Data allocations, check collisions and fill cumulative mask
  for (uint32_t i = 0; i < dl_result.data.size(); ++i) {
    TESTASSERT(try_dl_mask_fill(dl_result.data[i].dci, "data") == SRSRAN_SUCCESS);
  }

  // TEST: check for holes in the PRB mask (RBGs not fully filled)
  rbgmask.resize(cell_params.nof_rbgs);
  rbgmask.reset();
  srsran::bounded_bitset<100, true> rev_alloc = ~dl_allocs;
  for (uint32_t i = 0; i < cell_params.nof_rbgs; ++i) {
    uint32_t lim = SRSRAN_MIN((i + 1) * cell_params.P, dl_allocs.size());
    bool     val = dl_allocs.any(i * cell_params.P, lim);
    CONDERROR(rev_alloc.any(i * cell_params.P, lim) and val, "No holes can be left in an RBG");
    if (val) {
      rbgmask.set(i);
    }
  }

  CONDERROR(expected_rbgmask != nullptr and *expected_rbgmask != rbgmask,
            "The derived DL RBG mask %s does not match the expected one %s",
            fmt::format("{}", rbgmask).c_str(),
            fmt::format("{}", *expected_rbgmask).c_str());

  return SRSRAN_SUCCESS;
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
  bc_elem* bc_begin = dl_result.bc.begin();
  bc_elem* bc_end   = dl_result.bc.end();

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
    CONDERROR(bc->tbs < cell_params.cfg.sibs[bc->index].get_length(),
              "Allocated BC process with TBS=%d < sib_len=%d",
              bc->tbs,
              cell_params.cfg.sibs[bc->index].get_length());
    CONDERROR(bc->dci.rnti != 0xffff, "Invalid rnti=0x%x for SIB%d", bc->dci.rnti, bc->index);
    CONDERROR(bc->dci.format != SRSRAN_DCI_FORMAT1A, "Invalid DCI format for SIB%d", bc->index);

    uint32_t x         = (bc->index - 1) * cell_params.cfg.si_window_ms;
    uint32_t sf        = x % 10;
    uint32_t sfn_start = sfn;
    while ((sfn_start % cell_params.cfg.sibs[bc->index].get_period_rf()) != x / 10) {
      sfn_start--;
    }
    srsran::tti_point    win_start{sfn_start * 10 + sf};
    srsran::tti_interval window{win_start, win_start + cell_params.cfg.si_window_ms};
    CONDERROR(not window.contains(to_tx_dl(sf_out.tti_rx)), "Scheduled SIB is outside of its SIB window");
  }
  return SRSRAN_SUCCESS;
}

int test_pdcch_collisions(const sf_output_res_t&                   sf_out,
                          uint32_t                                 enb_cc_idx,
                          const srsran::bounded_bitset<128, true>* expected_cce_mask)
{
  const auto& cell_params = sf_out.cc_params[enb_cc_idx];
  const auto& dl_result   = sf_out.dl_cc_result[enb_cc_idx];
  const auto& ul_result   = sf_out.ul_cc_result[enb_cc_idx];
  int         ret         = srsran_regs_pdcch_ncce(cell_params.regs.get(), dl_result.cfi);
  TESTASSERT(ret > 0);
  uint32_t                          ncce = ret;
  srsran::bounded_bitset<128, true> used_cce{ncce};

  // Helper Function: checks if there is any collision. If not, fills the PDCCH mask
  auto try_cce_fill = [&](const srsran_dci_location_t& dci_loc, const char* ch) {
    uint32_t cce_start = dci_loc.ncce, cce_stop = dci_loc.ncce + (1u << dci_loc.L);
    CONDERROR(dci_loc.L > 3, "The aggregation level %d is not valid", dci_loc.L);
    CONDERROR(
        cce_start >= ncce or cce_stop > ncce, "The CCE positions (%u, %u) do not fit in PDCCH", cce_start, cce_stop);
    CONDERROR(
        used_cce.any(cce_start, cce_stop), "%s DCI collision between CCE positions (%u, %u)", ch, cce_start, cce_stop);
    used_cce.fill(cce_start, cce_stop);
    return SRSRAN_SUCCESS;
  };

  /* TEST: verify there are no dci collisions for UL, DL data, BC, RAR */
  for (uint32_t i = 0; i < ul_result.pusch.size(); ++i) {
    const auto& pusch = ul_result.pusch[i];
    if (not pusch.needs_pdcch) {
      // In case of non-adaptive retx or Msg3
      continue;
    }
    try_cce_fill(pusch.dci.location, "UL");
  }
  for (uint32_t i = 0; i < dl_result.data.size(); ++i) {
    try_cce_fill(dl_result.data[i].dci.location, "DL data");
  }
  for (uint32_t i = 0; i < dl_result.bc.size(); ++i) {
    try_cce_fill(dl_result.bc[i].dci.location, "DL BC");
  }
  for (uint32_t i = 0; i < dl_result.rar.size(); ++i) {
    try_cce_fill(dl_result.rar[i].dci.location, "DL RAR");
  }

  CONDERROR(expected_cce_mask != nullptr and *expected_cce_mask != used_cce,
            "The derived PDCCH mask %s does not match the expected one %s",
            fmt::format("{}", used_cce).c_str(),
            fmt::format("{}", *expected_cce_mask).c_str());

  return SRSRAN_SUCCESS;
}

int test_dci_content_common(const sf_output_res_t& sf_out, uint32_t enb_cc_idx)
{
  const auto& cell_params = sf_out.cc_params[enb_cc_idx];
  const auto& dl_result   = sf_out.dl_cc_result[enb_cc_idx];
  const auto& ul_result   = sf_out.ul_cc_result[enb_cc_idx];

  std::set<uint16_t> alloc_rntis;
  for (uint32_t i = 0; i < ul_result.pusch.size(); ++i) {
    const auto& pusch = ul_result.pusch[i];
    uint16_t    rnti  = pusch.dci.rnti;
    CONDERROR(pusch.tbs == 0, "Allocated PUSCH with invalid TBS=%d", pusch.tbs);
    CONDERROR(alloc_rntis.count(rnti) > 0, "The user rnti=0x%x got allocated multiple times in UL", rnti);
    alloc_rntis.insert(pusch.dci.rnti);
    CONDERROR(not(((pusch.current_tx_nb % 4) == 0) xor (pusch.dci.tb.rv != 0)),
              "[rnti=0x%x] Number of txs incorrectly set",
              rnti);
    if (not pusch.needs_pdcch) {
      // In case of non-adaptive retx or Msg3
      continue;
    }
    if (pusch.dci.tb.rv == 0) {
      // newTx
      CONDERROR(pusch.dci.format != SRSRAN_DCI_FORMAT0, "Incorrect UL DCI format");
      CONDERROR(pusch.dci.tb.mcs_idx > 28, "Incorrect UL MCS index");
    }
  }

  alloc_rntis.clear();
  for (uint32_t i = 0; i < dl_result.data.size(); ++i) {
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

  // TEST: max coderate is not exceeded for RA and Broadcast
  srsran_dl_sf_cfg_t dl_sf = {};
  dl_sf.cfi                = sf_out.dl_cc_result[enb_cc_idx].cfi;
  dl_sf.tti                = to_tx_dl(sf_out.tti_rx).to_uint();
  auto test_ra_bc_coderate = [&dl_sf, &cell_params](uint32_t tbs, const srsran_dci_dl_t& dci) {
    srsran_pdsch_grant_t grant = {};
    srsran_ra_dl_grant_to_grant_prb_allocation(&dci, &grant, cell_params.cfg.cell.nof_prb);
    uint32_t       nof_re   = srsran_ra_dl_grant_nof_re(&cell_params.cfg.cell, &dl_sf, &grant);
    float          coderate = srsran_coderate(tbs * 8, nof_re);
    const uint32_t Qm       = 2;
    CONDERROR(
        coderate > 0.932f * Qm, "Max coderate was exceeded from %s DCI", dci.rnti == SRSRAN_SIRNTI ? "SIB" : "RAR");
    return SRSRAN_SUCCESS;
  };

  for (uint32_t i = 0; i < dl_result.bc.size(); ++i) {
    const sched_interface::dl_sched_bc_t& bc = dl_result.bc[i];
    if (bc.type == sched_interface::dl_sched_bc_t::BCCH) {
      CONDERROR(bc.tbs < cell_params.cfg.sibs[bc.index].get_length(),
                "Allocated BC process with TBS=%d < sib_len=%d",
                bc.tbs,
                cell_params.cfg.sibs[bc.index].get_length());
    } else if (bc.type == sched_interface::dl_sched_bc_t::PCCH) {
      CONDERROR(bc.tbs == 0, "Allocated paging process with invalid TBS=%d", bc.tbs);
    } else {
      TESTERROR("Invalid broadcast process id=%d", (int)bc.type);
    }

    TESTASSERT(test_ra_bc_coderate(bc.tbs, bc.dci) == SRSRAN_SUCCESS);
  }

  for (uint32_t i = 0; i < dl_result.rar.size(); ++i) {
    const auto& rar = dl_result.rar[i];
    CONDERROR(rar.tbs == 0, "Allocated RAR process with invalid TBS=%d", rar.tbs);

    // TEST: max coderate is not exceeded
    TESTASSERT(test_ra_bc_coderate(rar.tbs, rar.dci) == SRSRAN_SUCCESS);
  }

  return SRSRAN_SUCCESS;
}

int test_all_common(const sf_output_res_t& sf_out)
{
  for (uint32_t i = 0; i < sf_out.cc_params.size(); ++i) {
    TESTASSERT(test_pusch_collisions(sf_out, i, nullptr) == SRSRAN_SUCCESS);
    TESTASSERT(test_pdsch_collisions(sf_out, i, nullptr) == SRSRAN_SUCCESS);
    TESTASSERT(test_sib_scheduling(sf_out, i) == SRSRAN_SUCCESS);
    TESTASSERT(test_pdcch_collisions(sf_out, i, nullptr) == SRSRAN_SUCCESS);
    TESTASSERT(test_dci_content_common(sf_out, i) == SRSRAN_SUCCESS);
  }
  return SRSRAN_SUCCESS;
}

} // namespace srsenb
