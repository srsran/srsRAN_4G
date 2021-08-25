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

#include "sched_nr_common_test.h"
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

} // namespace srsenb
