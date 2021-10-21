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

#ifndef SRSRAN_SCHED_NR_HELPERS_H
#define SRSRAN_SCHED_NR_HELPERS_H

#include "sched_nr_cfg.h"
#include "sched_nr_ue.h"

namespace srsenb {
namespace sched_nr_impl {

class slot_ue;
class ul_harq_proc;
struct bwp_res_grid;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool fill_dci_rar(prb_interval interv, uint16_t ra_rnti, const bwp_params_t& bwp_cfg, srsran_dci_dl_nr_t& dci);

bool fill_dci_msg3(const slot_ue& ue, const bwp_params_t& bwp_cfg, srsran_dci_ul_nr_t& dci);

/// Generate PDCCH DL DCI fields
void fill_dl_dci_ue_fields(const slot_ue&        ue,
                           const bwp_params_t&   bwp_cfg,
                           uint32_t              ss_id,
                           srsran_dci_location_t dci_pos,
                           srsran_dci_dl_nr_t&   dci);

/// Generate PDCCH UL DCI fields
void fill_ul_dci_ue_fields(const slot_ue&        ue,
                           const bwp_params_t&   bwp_cfg,
                           uint32_t              ss_id,
                           srsran_dci_location_t dci_pos,
                           srsran_dci_ul_nr_t&   dci);

/// Log UE state for slot being scheduled
void log_sched_slot_ues(srslog::basic_logger& logger,
                        slot_point            pdcch_slot,
                        uint32_t              cc,
                        const slot_ue_map_t&  slot_ues);

/// Log Scheduling Result for a given BWP and slot
void log_sched_bwp_result(srslog::basic_logger& logger,
                          slot_point            pdcch_slot,
                          const bwp_res_grid&   res_grid,
                          const slot_ue_map_t&  slot_ues);

} // namespace sched_nr_impl
} // namespace srsenb

#endif // SRSRAN_SCHED_NR_HELPERS_H
