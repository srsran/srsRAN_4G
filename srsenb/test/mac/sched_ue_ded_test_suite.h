/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSLTE_SCHED_UE_DED_TEST_SUITE_H
#define SRSLTE_SCHED_UE_DED_TEST_SUITE_H

#include "sched_common_test_suite.h"
#include "sched_sim_ue.h"

namespace srsenb {

/**
 * Checks DL grant content and whether it is consistent with the current UE context. Current checks:
 * - The DCI rv matches the nof DL harq retxs observed from the UE perspective
 * - The number of retxs per DL harq does not exceed its maximum set in the ue cfg
 * - HARQ pids are not reused too early (ACK hasn't arrive to the eNB yet)
 * @param ue_ctxt current simulation UE context
 * @param tti_rx TTI when scheduling decision was made
 * @param enb_cc_idx eNB carrier index
 * @param pdsch PDSCH grant data
 * @return error code
 */
int test_pdsch_grant(const sim_ue_ctxt_t&                    ue_ctxt,
                     srslte::tti_point                       tti_rx,
                     uint32_t                                enb_cc_idx,
                     const sched_interface::dl_sched_data_t& pdsch);

/**
 * Checks PHICH & PUSCH grant content and whether it is consistent with the current UE HARQ state. Current checks:
 * - All PHICH and PUSCH grants' rnti values are present in the eNB context
 * - UEs only get PUSCH grants in active CCs
 * - Active UE UL HARQs expect PHICH
 * - Active UE UL HARQs expect PUSCH grants except if it is the last retx
 * - The DCI rv matches the nof UL harq retxs observed from the UE perspective
 * - The number of retxs per UL harq does not exceed its maximum value set in the ue cfg
 * @param enb_ctxt current eNB state, including list of UEs
 * @param sf_out result of a subframe sched result
 * @return error code
 */
int test_ul_sched_result(const sim_enb_ctxt_t& enb_ctxt, const sf_output_res_t& sf_out);

/**
 * Call all tests that depend on the UE internal state for all registered UEs in the eNB
 * @param enb_ctxt current eNB state, including list of UEs
 * @param sf_out result of a subframe sched result
 * @return error code
 */
int test_all_ues(const sim_enb_ctxt_t& enb_ctxt, const sf_output_res_t& sf_out);

} // namespace srsenb

#endif // SRSLTE_SCHED_UE_DED_TEST_SUITE_H
