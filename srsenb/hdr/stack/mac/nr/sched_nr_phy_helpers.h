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

#ifndef SRSRAN_SCHED_NR_PHY_HELPERS_H
#define SRSRAN_SCHED_NR_PHY_HELPERS_H

#include "sched_nr_common.h"

namespace srsenb {
namespace sched_nr_impl {

uint32_t get_P(uint32_t bwp_nof_prb, bool config_1_or_2);
uint32_t get_nof_rbgs(uint32_t bwp_nof_prb, uint32_t bwp_start, bool config1_or_2);

class slot_ue;
void fill_dci_ue_cfg(const slot_ue&           ue,
                     const rbgmask_t&         rbgmask,
                     const sched_cell_params& cc_cfg,
                     srsran_dci_dl_nr_t&      dci);
void fill_dci_ue_cfg(const slot_ue&           ue,
                     const rbgmask_t&         rbgmask,
                     const sched_cell_params& cc_cfg,
                     srsran_dci_ul_nr_t&      dci);
void fill_pdsch_ue(const slot_ue&           ue,
                   const rbgmask_t&         rbgmask,
                   const sched_cell_params& cc_cfg,
                   srsran_sch_cfg_nr_t&     sch);
void fill_pusch_ue(const slot_ue&           ue,
                   const rbgmask_t&         rbgmask,
                   const sched_cell_params& cc_cfg,
                   srsran_sch_cfg_nr_t&     sch);

} // namespace sched_nr_impl
} // namespace srsenb

#endif // SRSRAN_SCHED_NR_PHY_HELPERS_H
