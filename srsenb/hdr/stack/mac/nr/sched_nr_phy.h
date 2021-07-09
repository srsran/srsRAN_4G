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

#ifndef SRSRAN_SCHED_NR_PHY_H
#define SRSRAN_SCHED_NR_PHY_H

#include "sched_nr_cfg.h"

namespace srsenb {
namespace sched_nr_impl {

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool fill_dci_rar(prb_interval interv, const bwp_params& bwp_cfg, srsran_dci_dl_nr_t& dci);

class slot_ue;

/// Generate PDCCH DL DCI fields
void fill_dl_dci_ue_fields(const slot_ue&        ue,
                           const bwp_params&     bwp_cfg,
                           uint32_t              ss_id,
                           srsran_dci_location_t dci_pos,
                           srsran_dci_dl_nr_t&   dci);

/// Generate PDCCH UL DCI fields
void fill_ul_dci_ue_fields(const slot_ue&        ue,
                           const bwp_params&     bwp_cfg,
                           uint32_t              ss_id,
                           srsran_dci_location_t dci_pos,
                           srsran_dci_ul_nr_t&   dci);

} // namespace sched_nr_impl
} // namespace srsenb

#endif // SRSRAN_SCHED_NR_PHY_H
