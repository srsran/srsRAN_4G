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

#include "sched_nr_common_test.h"
#include "srsenb/hdr/stack/mac/nr/sched_nr_cfg.h"
#include "srsran/support/srsran_test.h"

namespace srsenb {

void test_dl_pdcch_consistency(srsran::const_span<sched_nr_impl::pdcch_dl_t> dl_pdcchs)
{
  for (const auto& pdcch : dl_pdcchs) {
    if (pdcch.dci.ctx.rnti_type == srsran_rnti_type_ra) {
      TESTASSERT_EQ(pdcch.dci.ctx.format, srsran_dci_format_nr_1_0);
      TESTASSERT_EQ(pdcch.dci.ctx.ss_type, srsran_search_space_type_common_1);
      TESTASSERT(pdcch.dci.ctx.location.L > 0);
    } else if (pdcch.dci.ctx.rnti_type == srsran_rnti_type_c) {
      TESTASSERT(pdcch.dci.ctx.format == srsran_dci_format_nr_1_0 or pdcch.dci.ctx.format == srsran_dci_format_nr_1_1);
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
    const sched_nr_interface::cell_cfg_t&                                                     cell_cfg,
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
