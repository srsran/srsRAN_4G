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

void test_pdcch_consistency(srsran::const_span<sched_nr_impl::pdcch_dl_t> dl_pdcchs)
{
  for (const auto& pdcch : dl_pdcchs) {
    if (pdcch.dci.ctx.rnti_type == srsran_rnti_type_ra) {
      TESTASSERT_EQ(pdcch.dci.ctx.format, srsran_dci_format_nr_1_0);
      TESTASSERT_EQ(pdcch.dci.ctx.ss_type, srsran_search_space_type_common_1);
      TESTASSERT(pdcch.dci.ctx.location.L > 0);
    }
  }
}

} // namespace srsenb
