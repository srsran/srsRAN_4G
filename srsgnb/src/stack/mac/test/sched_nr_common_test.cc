/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#include "sched_nr_common_test.h"
#include "srsgnb/hdr/stack/mac/sched_nr_cfg.h"
#include "srsgnb/hdr/stack/mac/sched_nr_helpers.h"
#include "srsran/support/srsran_test.h"

namespace srsenb {

using namespace sched_nr_impl;

void test_dci_ctx_consistency(const srsran_pdcch_cfg_nr_t& pdcch_cfg, const srsran_dci_ctx_t& dci_ctx)
{
  TESTASSERT(dci_ctx.coreset_id < SRSRAN_UE_DL_NR_MAX_NOF_CORESET);
  TESTASSERT(pdcch_cfg.coreset_present[dci_ctx.coreset_id]);
  const srsran_coreset_t& coreset = pdcch_cfg.coreset[dci_ctx.coreset_id];

  // DCI location
  TESTASSERT(dci_ctx.location.L < SRSRAN_SEARCH_SPACE_NOF_AGGREGATION_LEVELS_NR);
  TESTASSERT(dci_ctx.location.ncce + (1U << dci_ctx.location.L) <= coreset_nof_cces(coreset));
  // RNTI type, SearchSpace type, DCI format
  TESTASSERT(sched_nr_impl::is_rnti_type_valid_in_search_space(dci_ctx.rnti_type, dci_ctx.ss_type));
  switch (dci_ctx.rnti_type) {
    case srsran_rnti_type_si:
      TESTASSERT_EQ(srsran_dci_format_nr_1_0, dci_ctx.format);
      TESTASSERT_EQ(srsran_search_space_type_common_0, dci_ctx.ss_type);
      TESTASSERT_EQ(SRSRAN_SIRNTI, dci_ctx.rnti);
      break;
    case srsran_rnti_type_ra:
      TESTASSERT_EQ(dci_ctx.format, srsran_dci_format_nr_1_0);
      TESTASSERT_EQ(dci_ctx.ss_type, srsran_search_space_type_common_1);
      TESTASSERT(pdcch_cfg.ra_search_space_present);
      TESTASSERT_EQ(pdcch_cfg.ra_search_space.coreset_id, dci_ctx.coreset_id);
      TESTASSERT(pdcch_cfg.ra_search_space.nof_candidates[dci_ctx.location.L] > 0);
      break;
    case srsran_rnti_type_tc:
      TESTASSERT_EQ(srsran_dci_format_nr_1_0, dci_ctx.format);
      TESTASSERT_EQ(srsran_search_space_type_common_1, dci_ctx.ss_type);
      break;
    case srsran_rnti_type_c:
      TESTASSERT(dci_ctx.format == srsran_dci_format_nr_1_0 or dci_ctx.format == srsran_dci_format_nr_1_1 or
                 dci_ctx.format == srsran_dci_format_nr_0_0 or dci_ctx.format == srsran_dci_format_nr_0_1);
      break;
    default:
      srsran_terminate("rnti type=%d not supported", dci_ctx.rnti_type);
  }
  // CORESET position
  TESTASSERT_EQ(srsran_coreset_start_rb(&coreset), dci_ctx.coreset_start_rb);
}

void test_pdcch_collisions(const srsran_pdcch_cfg_nr_t&   pdcch_cfg,
                           srsran::const_span<pdcch_dl_t> dl_pdcchs,
                           srsran::const_span<pdcch_ul_t> ul_pdcchs)
{
  srsran::optional_vector<coreset_bitmap> coreset_bitmaps;
  for (const srsran_coreset_t& coreset : view_active_coresets(pdcch_cfg)) {
    coreset_bitmaps.emplace(coreset.id, coreset_bitmap(coreset_nof_cces(coreset)));
  }

  for (const pdcch_dl_t& pdcch : dl_pdcchs) {
    coreset_bitmap& total_bitmap = coreset_bitmaps[pdcch.dci.ctx.coreset_id];
    coreset_bitmap  alloc(total_bitmap.size());
    alloc.fill(pdcch.dci.ctx.location.ncce, pdcch.dci.ctx.location.ncce + (1U << pdcch.dci.ctx.location.L));
    TESTASSERT((alloc & total_bitmap).none());
    total_bitmap |= alloc;
  }
  for (const pdcch_ul_t& pdcch : ul_pdcchs) {
    coreset_bitmap& total_bitmap = coreset_bitmaps[pdcch.dci.ctx.coreset_id];
    coreset_bitmap  alloc(total_bitmap.size());
    alloc.fill(pdcch.dci.ctx.location.ncce, pdcch.dci.ctx.location.ncce + (1U << pdcch.dci.ctx.location.L));
    TESTASSERT((alloc & total_bitmap).none());
    total_bitmap |= alloc;
  }
}

void test_dl_pdcch_consistency(const cell_config_manager&                    cell_cfg,
                               srsran::const_span<sched_nr_impl::pdcch_dl_t> dl_pdcchs)
{
  for (const auto& pdcch : dl_pdcchs) {
    TESTASSERT(pdcch.dci.bwp_id < cell_cfg.bwps.size());
    const srsran_pdcch_cfg_nr_t& pdcch_cfg = cell_cfg.bwps[pdcch.dci.bwp_id].cfg.pdcch;
    test_dci_ctx_consistency(pdcch_cfg, pdcch.dci.ctx);

    const srsran_coreset_t& coreset = pdcch_cfg.coreset[pdcch.dci.ctx.coreset_id];
    if (pdcch.dci.ctx.coreset_id == 0) {
      TESTASSERT_EQ(srsran_coreset_get_bw(&pdcch_cfg.coreset[0]), pdcch.dci.coreset0_bw);
    }

    switch (pdcch.dci.ctx.rnti_type) {
      case srsran_rnti_type_si:
        TESTASSERT(pdcch.dci.sii != 0 or pdcch.dci.ctx.coreset_id == 0); // sii=0 must go in CORESET#0
        break;
      default:
        break;
    }
  }
}

void test_pdsch_consistency(srsran::const_span<mac_interface_phy_nr::pdsch_t> pdschs)
{
  for (const mac_interface_phy_nr::pdsch_t& pdsch : pdschs) {
    TESTASSERT(pdsch.sch.grant.nof_layers > 0);
    if (pdsch.sch.grant.rnti_type == srsran_rnti_type_c) {
      TESTASSERT(pdsch.sch.grant.tb[0].softbuffer.tx != nullptr);
      TESTASSERT(pdsch.sch.grant.tb[0].softbuffer.tx->buffer_b != nullptr);
      TESTASSERT(pdsch.sch.grant.tb[0].softbuffer.tx->max_cb > 0);
    }
  }
}

void test_ssb_scheduled_grant(
    const srsran::slot_point&                                                                 sl_point,
    const sched_nr_impl::cell_config_manager&                                                 cell_cfg,
    const srsran::bounded_vector<mac_interface_phy_nr::ssb_t, mac_interface_phy_nr::MAX_SSB>& ssb_list)
{
  /*
   * Verify that, with correct SSB periodicity, dl_res has:
   * 1) SSB grant
   * 2) 4 LSBs of SFN in packed MIB message are correct
   * 3) SSB index is 0
   */
  if (sl_point.to_uint() % (cell_cfg.ssb.periodicity_ms * (uint32_t)sl_point.nof_slots_per_subframe()) == 0) {
    TESTASSERT(ssb_list.size() == 1);
    auto& ssb_item = ssb_list.back();
    TESTASSERT(ssb_item.pbch_msg.sfn_4lsb == ((uint8_t)sl_point.sfn() & 0b1111));
    bool expected_hrf = sl_point.slot_idx() % SRSRAN_NSLOTS_PER_FRAME_NR(srsran_subcarrier_spacing_15kHz) >=
                        SRSRAN_NSLOTS_PER_FRAME_NR(srsran_subcarrier_spacing_15kHz) / 2;
    TESTASSERT(ssb_item.pbch_msg.hrf == expected_hrf);
    TESTASSERT(ssb_item.pbch_msg.ssb_idx == 0);
  }
  // Verify that, outside SSB periodicity, there is NO SSB grant
  else {
    TESTASSERT(ssb_list.size() == 0);
  }
}

} // namespace srsenb
