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

#ifndef SRSRAN_SCHED_NR_SIGNALLING_H
#define SRSRAN_SCHED_NR_SIGNALLING_H

#include "harq_softbuffer.h"
#include "sched_nr_cfg.h"
#include "sched_nr_interface.h"
#include "srsenb/hdr/stack/mac/sched_common.h"

namespace srsenb {
namespace sched_nr_impl {

class bwp_slot_allocator;

/// Schedule NZP-CSI-RS resources for given slot
void sched_nzp_csi_rs(srsran::const_span<srsran_csi_rs_nzp_set_t> nzp_csi_rs_sets,
                      const srsran_slot_cfg_t&                    slot_cfg,
                      nzp_csi_rs_list&                            csi_rs_list);

/**
 * @brief Schedule grant for SSB.
 *
 * The functions schedules the SSB according to a given periodicity. This function is a simplified version of an
 * SSB scheduler and has several hard-coded parameters.
 *
 * @param[in]  sl_point         Slot point carrying information about current slot.
 * @param[in]  ssb_periodicity  Periodicity of SSB in ms.
 * @param[in]  mib              MIB message content
 * @param[out]  ssb_list        List of SSB messages to be sent to PHY.
 *
 * @remark This function a is basic scheduling function that uses the following simplified assumption:
 * 1) Subcarrier spacing: 15kHz
 * 2) Frequency below 3GHz
 * 3) Position in Burst is 1000, i.e., Only the first SSB of the 4 opportunities gets scheduled
 */
void sched_ssb_basic(const slot_point&      sl_point,
                     uint32_t               ssb_periodicity,
                     const srsran_mib_nr_t& mib,
                     ssb_list&              ssb_list);

/// Fill DCI fields with SIB info
bool fill_dci_sib(prb_interval interv, uint32_t sib_idx, const bwp_params_t& bwp_cfg, srsran_dci_dl_nr_t& dci);

/// For a given BWP and slot, schedule SSB, NZP CSI RS and SIBs
void sched_dl_signalling(bwp_slot_allocator& bwp_alloc);

/// scheduler for SIBs
class si_sched
{
public:
  explicit si_sched(const bwp_params_t& bwp_cfg_);

  void run_slot(bwp_slot_allocator& slot_alloc);

private:
  const bwp_params_t*   bwp_cfg = nullptr;
  srslog::basic_logger& logger;

  struct si_msg_ctxt_t {
    // args
    uint32_t n             = 0; /// 0 for SIB1, n/index in schedulingInfoList in si-SchedulingInfo in SIB1
    uint32_t len_bytes     = 0; /// length in bytes of SIB1 / SI message
    uint32_t win_len_slots = 0; /// window length in slots
    uint32_t period_frames = 0; /// periodicity of SIB1/SI window in frames

    // state
    uint32_t                                    n_tx   = 0; /// nof transmissions of the same SIB1 / SI message
    alloc_result                                result = alloc_result::invalid_coderate; /// last attempt to schedule SI
    slot_point                                  win_start; /// start of SI window, invalid if outside
    srsran::unique_pool_ptr<tx_harq_softbuffer> si_softbuffer;
  };
  srsran::bounded_vector<si_msg_ctxt_t, 10> pending_sis; /// configured SIB1 and SI messages
};

} // namespace sched_nr_impl
} // namespace srsenb

#endif // SRSRAN_SCHED_NR_SIGNALLING_H
