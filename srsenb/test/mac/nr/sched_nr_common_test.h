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
