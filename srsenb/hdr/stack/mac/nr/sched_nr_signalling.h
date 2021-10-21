/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#include "sched_nr_cfg.h"
#include "sched_nr_interface.h"

namespace srsenb {
namespace sched_nr_impl {

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

/// For a given BWP and slot, schedule SSB, NZP CSI RS and SIBs
void sched_dl_signalling(const bwp_params_t& bwp_params,
                         slot_point          sl_pdcch,
                         ssb_list&           ssb_list,
                         nzp_csi_rs_list&    nzp_csi_rs);

} // namespace sched_nr_impl
} // namespace srsenb

#endif // SRSRAN_SCHED_NR_SIGNALLING_H
