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
