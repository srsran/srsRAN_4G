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
