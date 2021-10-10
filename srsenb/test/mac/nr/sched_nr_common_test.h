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

#ifndef SRSRAN_SCHED_NR_COMMON_TEST_H
#define SRSRAN_SCHED_NR_COMMON_TEST_H

#include "srsenb/hdr/stack/mac/nr/sched_nr_pdcch.h"
#include "srsran/adt/span.h"

namespace srsenb {

void test_dl_pdcch_consistency(srsran::const_span<sched_nr_impl::pdcch_dl_t> dl_pdcch);
void test_pdsch_consistency(srsran::const_span<mac_interface_phy_nr::pdsch_t> dl_pdcch);
/// @brief Test whether the SSB grant gets scheduled with the correct periodicity.
void test_ssb_scheduled_grant(
    const srsran::slot_point&                                                                 sl_point,
    const sched_nr_interface::cell_cfg_t&                                                     cell_cfg,
    const srsran::bounded_vector<mac_interface_phy_nr::ssb_t, mac_interface_phy_nr::MAX_SSB>& ssb_list);

} // namespace srsenb

#endif // SRSRAN_SCHED_NR_COMMON_TEST_H
