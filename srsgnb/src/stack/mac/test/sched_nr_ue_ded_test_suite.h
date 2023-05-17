/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#ifndef SRSRAN_SCHED_NR_UE_DED_TEST_SUITE_H
#define SRSRAN_SCHED_NR_UE_DED_TEST_SUITE_H

#include "sched_nr_sim_ue.h"

namespace srsenb {

void test_dl_sched_result(const sim_nr_enb_ctxt_t& enb_ctxt, const sched_nr_cc_result_view& cc_out);

}

#endif // SRSRAN_SCHED_NR_UE_DED_TEST_SUITE_H
