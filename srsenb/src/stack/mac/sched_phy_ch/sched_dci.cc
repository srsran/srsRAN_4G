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

#include "srsenb/hdr/stack/mac/sched_phy_ch/sched_dci.h"
#include "srsenb/hdr/stack/mac/sched_common.h"
#include "srsenb/hdr/stack/mac/sched_helpers.h"
#include "srsran/common/string_helpers.h"

#include <cmath>
#include <cstdint>

namespace srsenb {

static srslog::basic_logger& get_mac_logger()
{
  static srslog::basic_logger& logger = srslog::fetch_basic_logger("MAC");
  return logger;
}

/// Compute max TBS based on max coderate
int coderate_to_tbs(float max_coderate, uint32_t nof_re)
{
  return static_cast<int>(floorf(nof_re * max_coderate - 24));
}

/// Compute {mcs, tbs_idx} based on {max_tbs, nof_prb}
int compute_mcs_from_max_tbs(uint32_t nof_prb,
                             uint32_t max_tbs,
                             uint32_t max_mcs,
                             bool     is_ul,
                             bool     use_tbs_index_alt,
                             int&     mcs,
                             int&     tbs_idx)
{
  constexpr static std::array<int, 6> forbidden_tbs_idx_alt{1, 3, 5, 7, 9, 26};

  // Compute I_TBS based on max TBS
  uint32_t max_tbs_idx = (use_tbs_index_alt) ? 33 : 26;
  tbs_idx              = srsran_ra_tbs_to_table_idx(max_tbs, nof_prb, max_tbs_idx);
  if (tbs_idx <= 0) {
    return SRSRAN_ERROR;
  }
  --tbs_idx; // get TBS index lower bound
  if (use_tbs_index_alt and
      std::find(forbidden_tbs_idx_alt.begin(), forbidden_tbs_idx_alt.end(), tbs_idx) != forbidden_tbs_idx_alt.end()) {
    // some tbs_idx are invalid for 256QAM. See TS 36.213 - Table 7.1.7.1-1A
    --tbs_idx;
  }

  // Compute I_mcs based on I_TBS. Reverse of TS 36.213 - Table 7.1.7.1-1/1A
  mcs = srsran_ra_mcs_from_tbs_idx(tbs_idx, use_tbs_index_alt, is_ul);
  if (mcs < 0) {
    return SRSRAN_ERROR;
  }
  if (mcs > (int)max_mcs) {
    // bound mcs
    mcs     = max_mcs;
    tbs_idx = srsran_ra_tbs_idx_from_mcs(mcs, use_tbs_index_alt, is_ul);
  }

  return SRSRAN_SUCCESS;
}

tbs_info compute_mcs_and_tbs(uint32_t nof_prb,
                             uint32_t nof_re,
                             uint32_t cqi,
                             uint32_t max_mcs,
                             bool     is_ul,
                             bool     ulqam64_enabled,
                             bool     use_tbs_index_alt)
{
  assert((not is_ul or not use_tbs_index_alt) && "UL cannot use Alt CQI Table");
  assert((is_ul or not ulqam64_enabled) && "DL cannot use UL-QAM64 enable flag");

  float    max_coderate = srsran_cqi_to_coderate(std::min(cqi + 1U, 15U), use_tbs_index_alt);
  uint32_t max_Qm       = (is_ul) ? (ulqam64_enabled ? 6 : 4) : (use_tbs_index_alt ? 8 : 6);
  max_coderate          = std::min(max_coderate, 0.932F * max_Qm);

  int   mcs               = 0;
  float prev_max_coderate = 0;
  do {
    // update max TBS based on max coderate
    int max_tbs = coderate_to_tbs(max_coderate, nof_re);
    if (max_tbs < 16) {
      return tbs_info{};
    }

    // Compute max {MCS,I_TBS} based on given max_tbs, nof_prb
    int tbs_idx = 0;
    if (compute_mcs_from_max_tbs(nof_prb, max_tbs, max_mcs, is_ul, use_tbs_index_alt, mcs, tbs_idx) != SRSRAN_SUCCESS) {
      return tbs_info{};
    }

    if (mcs == 6 and nof_prb == 1) {
      // Avoid the unusual case n_prb=1, mcs=6 tbs=328 (used in voip)
      max_mcs = mcs - 1;
      continue;
    }

    // compute real TBS and coderate based on maximum achievable MCS
    int   tbs      = srsran_ra_tbs_from_idx(tbs_idx, nof_prb);
    float coderate = srsran_coderate(tbs, nof_re);

    // update max coderate based on mcs
    srsran_mod_t mod = (is_ul) ? srsran_ra_ul_mod_from_mcs(mcs) : srsran_ra_dl_mod_from_mcs(mcs, use_tbs_index_alt);
    uint32_t     Qm  = srsran_mod_bits_x_symbol(mod);
    max_coderate     = std::min(0.932F * Qm, max_coderate);

    if (coderate <= max_coderate) {
      // solution was found
      tbs_info tb;
      tb.tbs_bytes = tbs / 8;
      tb.mcs       = mcs;
      return tb;
    }

    // start with smaller max mcs in next iteration
    max_mcs = mcs - 1;
  } while (mcs > 0 and max_coderate != prev_max_coderate);

  return tbs_info{};
}

tbs_info compute_min_mcs_and_tbs_from_required_bytes(uint32_t nof_prb,
                                                     uint32_t nof_re,
                                                     uint32_t cqi,
                                                     uint32_t max_mcs,
                                                     uint32_t req_bytes,
                                                     bool     is_ul,
                                                     bool     ulqam64_enabled,
                                                     bool     use_tbs_index_alt)
{
  // get max MCS/TBS that meets max coderate requirements
  tbs_info tb_max = compute_mcs_and_tbs(nof_prb, nof_re, cqi, max_mcs, is_ul, ulqam64_enabled, use_tbs_index_alt);
  if (tb_max.tbs_bytes + 8 <= (int)req_bytes or tb_max.mcs == 0 or req_bytes <= 0) {
    // if mcs cannot be lowered or a decrease in TBS index won't meet req_bytes requirement
    return tb_max;
  }

  // get maximum MCS that leads to tbs < req_bytes (used as max_tbs argument)
  int mcs_min     = 0;
  int tbs_idx_min = 0;
  // Note: we subtract -1 to required data to get an exclusive lower bound for maximum MCS. This works ok because
  //       req_bytes * 8 is always even
  if (compute_mcs_from_max_tbs(nof_prb, req_bytes * 8U - 1, max_mcs, is_ul, use_tbs_index_alt, mcs_min, tbs_idx_min) !=
      SRSRAN_SUCCESS) {
    // Failed to compute maximum MCS that leads to TBS < req bytes. MCS=0 is likely a valid solution
    tbs_info tb2 = compute_mcs_and_tbs(nof_prb, nof_re, cqi, 0, is_ul, ulqam64_enabled, use_tbs_index_alt);
    if (tb2.tbs_bytes >= (int)req_bytes) {
      return tb2;
    }
    return tb_max;
  }

  // Iterate from min to max MCS until a solution is found
  for (int mcs = mcs_min + 1; mcs < tb_max.mcs; ++mcs) {
    tbs_info tb2 = compute_mcs_and_tbs(nof_prb, nof_re, cqi, mcs, is_ul, ulqam64_enabled, use_tbs_index_alt);
    if (tb2.tbs_bytes >= (int)req_bytes) {
      return tb2;
    }
  }
  return tb_max;
}

int generate_ra_bc_dci_format1a_common(srsran_dci_dl_t&           dci,
                                       uint16_t                   rnti,
                                       tti_point                  tti_tx_dl,
                                       uint32_t                   req_bytes,
                                       rbg_interval               rbg_range,
                                       const sched_cell_params_t& cell_params,
                                       uint32_t                   current_cfi)
{
  static const uint32_t Qm = 2, bc_rar_cqi = 4;
  static const float    max_ctrl_coderate = std::min(srsran_cqi_to_coderate(bc_rar_cqi + 1, false), 0.932F * Qm);

  // Calculate I_tbs for this TBS
  int tbs = static_cast<int>(req_bytes) * 8;
  int mcs = -1;
  for (uint32_t i = 0; i < 27; i++) {
    if (srsran_ra_tbs_from_idx(i, 2) >= tbs) {
      dci.type2_alloc.n_prb1a = srsran_ra_type2_t::SRSRAN_RA_TYPE2_NPRB1A_2;
      mcs                     = i;
      tbs                     = srsran_ra_tbs_from_idx(i, 2);
      break;
    }
    if (srsran_ra_tbs_from_idx(i, 3) >= tbs) {
      dci.type2_alloc.n_prb1a = srsran_ra_type2_t::SRSRAN_RA_TYPE2_NPRB1A_3;
      mcs                     = i;
      tbs                     = srsran_ra_tbs_from_idx(i, 3);
      break;
    }
  }
  if (mcs < 0) {
    //    logger.error("Can't allocate Format 1A for TBS=%d", tbs);
    return -1;
  }

  // Generate remaining DCI Format1A content
  dci.alloc_type         = SRSRAN_RA_ALLOC_TYPE2;
  dci.type2_alloc.mode   = srsran_ra_type2_t::SRSRAN_RA_TYPE2_LOC;
  prb_interval prb_range = prb_interval::rbgs_to_prbs(rbg_range, cell_params.nof_prb());
  dci.type2_alloc.riv    = srsran_ra_type2_to_riv(prb_range.length(), prb_range.start(), cell_params.nof_prb());
  dci.pid                = 0;
  dci.tb[0].mcs_idx      = mcs;
  dci.tb[0].rv           = 0; // used for SIBs
  dci.format             = SRSRAN_DCI_FORMAT1A;
  dci.rnti               = rnti;
  dci.ue_cc_idx          = std::numeric_limits<uint32_t>::max();

  // Compute effective code rate and verify it doesn't exceed max code rate
  uint32_t nof_re = cell_params.get_dl_nof_res(tti_tx_dl, dci, current_cfi);
  if (srsran_coderate(tbs, nof_re) >= max_ctrl_coderate) {
    return -1;
  }

  get_mac_logger().debug("ra_tbs=%d/%d, tbs_bytes=%d, tbs=%d, mcs=%d",
                         srsran_ra_tbs_from_idx(mcs, 2),
                         srsran_ra_tbs_from_idx(mcs, 3),
                         req_bytes,
                         tbs,
                         mcs);

  return tbs;
}

bool generate_sib_dci(sched_interface::dl_sched_bc_t& bc,
                      tti_point                       tti_tx_dl,
                      uint32_t                        sib_idx,
                      uint32_t                        sib_ntx,
                      rbg_interval                    rbg_range,
                      const sched_cell_params_t&      cell_params,
                      uint32_t                        current_cfi)
{
  bc           = {};
  int tbs_bits = generate_ra_bc_dci_format1a_common(
      bc.dci, SRSRAN_SIRNTI, tti_tx_dl, cell_params.cfg.sibs[sib_idx].len, rbg_range, cell_params, current_cfi);
  if (tbs_bits < 0) {
    return false;
  }

  // generate SIB-specific fields
  bc.index = sib_idx;
  bc.type  = sched_interface::dl_sched_bc_t::BCCH;
  //  bc.tbs          = sib_len;
  bc.tbs          = tbs_bits / 8;
  bc.dci.tb[0].rv = get_rvidx(sib_ntx);

  return true;
}

bool generate_paging_dci(sched_interface::dl_sched_bc_t& bc,
                         tti_point                       tti_tx_dl,
                         uint32_t                        req_bytes,
                         rbg_interval                    rbg_range,
                         const sched_cell_params_t&      cell_params,
                         uint32_t                        current_cfi)
{
  bc           = {};
  int tbs_bits = generate_ra_bc_dci_format1a_common(
      bc.dci, SRSRAN_PRNTI, tti_tx_dl, req_bytes, rbg_range, cell_params, current_cfi);
  if (tbs_bits < 0) {
    return false;
  }

  // generate Paging-specific fields
  bc.type = sched_interface::dl_sched_bc_t::PCCH;
  bc.tbs  = tbs_bits / 8;

  return true;
}

bool generate_rar_dci(sched_interface::dl_sched_rar_t& rar,
                      tti_point                        tti_tx_dl,
                      const pending_rar_t&             pending_rar,
                      rbg_interval                     rbg_range,
                      uint32_t                         nof_grants,
                      uint32_t                         start_msg3_prb,
                      const sched_cell_params_t&       cell_params,
                      uint32_t                         current_cfi)
{
  const uint32_t msg3_Lcrb = 3;
  uint32_t       req_bytes = 7 * nof_grants + 1; // 1+6 bytes per RAR subheader+body and 1 byte for Backoff

  rar          = {};
  int tbs_bits = generate_ra_bc_dci_format1a_common(
      rar.dci, pending_rar.ra_rnti, tti_tx_dl, req_bytes, rbg_range, cell_params, current_cfi);
  if (tbs_bits < 0) {
    return false;
  }

  rar.msg3_grant.resize(nof_grants);
  for (uint32_t i = 0; i < nof_grants; ++i) {
    rar.msg3_grant[i].data            = pending_rar.msg3_grant[i];
    rar.msg3_grant[i].grant.tpc_pusch = 3;
    rar.msg3_grant[i].grant.trunc_mcs = 0;
    rar.msg3_grant[i].grant.rba       = srsran_ra_type2_to_riv(msg3_Lcrb, start_msg3_prb, cell_params.nof_prb());

    start_msg3_prb += msg3_Lcrb;
  }
  //  rar.tbs = tbs_bits / 8;
  rar.tbs = req_bytes;

  return true;
}

void log_broadcast_allocation(const sched_interface::dl_sched_bc_t& bc,
                              rbg_interval                          rbg_range,
                              const sched_cell_params_t&            cell_params)
{
  if (not get_mac_logger().info.enabled()) {
    return;
  }

  fmt::memory_buffer str_buffer;
  fmt::format_to(str_buffer, "{}", rbg_range);

  if (bc.type == sched_interface::dl_sched_bc_t::bc_type::BCCH) {
    get_mac_logger().debug("SCHED: SIB%d, cc=%d, rbgs=(%d,%d), dci=(%d,%d), rv=%d, len=%d, period=%d, mcs=%d",
                           bc.index + 1,
                           cell_params.enb_cc_idx,
                           rbg_range.start(),
                           rbg_range.stop(),
                           bc.dci.location.L,
                           bc.dci.location.ncce,
                           bc.dci.tb[0].rv,
                           cell_params.cfg.sibs[bc.index].len,
                           cell_params.cfg.sibs[bc.index].period_rf,
                           bc.dci.tb[0].mcs_idx);
  } else {
    get_mac_logger().info("SCHED: PCH, cc=%d, rbgs=%s, dci=(%d,%d), tbs=%d, mcs=%d",
                          cell_params.enb_cc_idx,
                          srsran::to_c_str(str_buffer),
                          bc.dci.location.L,
                          bc.dci.location.ncce,
                          bc.tbs,
                          bc.dci.tb[0].mcs_idx);
  }
}

void log_rar_allocation(const sched_interface::dl_sched_rar_t& rar, rbg_interval rbg_range)
{
  if (not get_mac_logger().info.enabled()) {
    return;
  }

  fmt::memory_buffer str_buffer;
  fmt::format_to(str_buffer, "{}", rbg_range);

  fmt::memory_buffer str_buffer2;
  for (size_t i = 0; i < rar.msg3_grant.size(); ++i) {
    fmt::format_to(str_buffer2,
                   "{}{{c-rnti=0x{:x}, rba={}, mcs={}}}",
                   i > 0 ? ", " : "",
                   rar.msg3_grant[i].data.temp_crnti,
                   rar.msg3_grant[i].grant.rba,
                   rar.msg3_grant[i].grant.trunc_mcs);
  }

  get_mac_logger().info("SCHED: RAR, ra-rnti=%d, rbgs=%s, dci=(%d,%d), msg3 grants=[%s]",
                        rar.dci.rnti,
                        srsran::to_c_str(str_buffer),
                        rar.dci.location.L,
                        rar.dci.location.ncce,
                        srsran::to_c_str(str_buffer2));
}

} // namespace srsenb
