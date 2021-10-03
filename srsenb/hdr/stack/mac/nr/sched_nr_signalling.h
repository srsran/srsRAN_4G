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

/// For a given BWP and slot, schedule SSB, NZP CSI RS and SIBs
void sched_dl_signalling(const bwp_params& bwp_params,
                         slot_point        sl_pdcch,
                         ssb_list&         ssb_list,
                         nzp_csi_rs_list&  nzp_csi_rs);

} // namespace sched_nr_impl
} // namespace srsenb

#endif // SRSRAN_SCHED_NR_SIGNALLING_H
