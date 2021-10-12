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

#ifndef SRSRAN_SCHED_NR_UE_DED_TEST_SUITE_H
#define SRSRAN_SCHED_NR_UE_DED_TEST_SUITE_H

#include "sched_nr_sim_ue.h"

namespace srsenb {

void test_dl_sched_result(const sim_nr_enb_ctxt_t& enb_ctxt, const sched_nr_cc_result_view& cc_out);

}

#endif // SRSRAN_SCHED_NR_UE_DED_TEST_SUITE_H
