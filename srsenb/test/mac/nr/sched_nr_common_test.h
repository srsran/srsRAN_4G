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

void test_pdcch_consistency(srsran::const_span<sched_nr_impl::pdcch_dl_t> dl_pdcch);

}

#endif // SRSRAN_SCHED_NR_COMMON_TEST_H
