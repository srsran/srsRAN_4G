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

#ifndef SRSRAN_SCHED_NR_PHY_HELPERS_H
#define SRSRAN_SCHED_NR_PHY_HELPERS_H

#include "sched_nr_common.h"

namespace srsenb {
namespace sched_nr_impl {

uint32_t get_P(uint32_t bwp_nof_prb, bool config_1_or_2);
uint32_t get_nof_rbgs(uint32_t bwp_nof_prb, uint32_t bwp_start, bool config1_or_2);

void bitmap_to_prb_array(const rbgmask_t& bitmap, uint32_t bwp_nof_prb, srsran_sch_grant_nr_t& grant);

class slot_ue;
void fill_dci_ue_cfg(const slot_ue& ue, srsran_dci_dl_nr_t& dci);
void fill_dci_ue_cfg(const slot_ue& ue, srsran_dci_ul_nr_t& dci);

} // namespace sched_nr_impl
} // namespace srsenb

#endif // SRSRAN_SCHED_NR_PHY_HELPERS_H
