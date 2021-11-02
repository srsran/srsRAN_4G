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

#ifndef SRSRAN_SCHED_NR_SIGNALLING_H
#define SRSRAN_SCHED_NR_SIGNALLING_H

#include "../sched_common.h"
#include "sched_nr_cfg.h"
#include "sched_nr_interface.h"

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
 * @param[out]  ssb_list         List of SSB messages to be sent to PHY.
 *
 * @remark This function a is basic scheduling function that uses the following simplified assumption:
 * 1) Subcarrier spacing: 15kHz
 * 2) Frequency below 3GHz
 * 3) Position in Burst is 1000, i.e., Only the first SSB of the 4 opportunities gets scheduled
 */
void sched_ssb_basic(const slot_point& sl_point, uint32_t ssb_periodicity, ssb_list& ssb_list);

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
    uint32_t     n       = 0; /// index in schedulingInfoList in si-SchedulingInfo in SIB1
    uint32_t     len     = 0;
    uint32_t     win_len = 0;
    uint32_t     period  = 0;
    uint32_t     n_tx    = 0;
    alloc_result result  = alloc_result::invalid_coderate; /// last attempt to schedule SI
    slot_point   win_start;                                /// start of SI window, invalid if outside
  };
  srsran::bounded_vector<si_msg_ctxt_t, 10> pending_sis;
};

} // namespace sched_nr_impl
} // namespace srsenb

#endif // SRSRAN_SCHED_NR_SIGNALLING_H
