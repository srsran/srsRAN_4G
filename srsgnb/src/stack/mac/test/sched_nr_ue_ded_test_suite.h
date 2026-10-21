// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

#ifndef SRSRAN_SCHED_NR_UE_DED_TEST_SUITE_H
#define SRSRAN_SCHED_NR_UE_DED_TEST_SUITE_H

#include "sched_nr_sim_ue.h"

namespace srsenb {

void test_dl_sched_result(const sim_nr_enb_ctxt_t& enb_ctxt, const sched_nr_cc_result_view& cc_out);

}

#endif // SRSRAN_SCHED_NR_UE_DED_TEST_SUITE_H
