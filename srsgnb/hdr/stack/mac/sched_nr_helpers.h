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
#include "srsran/adt/optional_array.h"

namespace srsenb {
namespace sched_nr_impl {

class slot_ue;
class ul_harq_proc;
struct bwp_res_grid;

/// Helper function to verify if RNTI type can be placed in specified search space
/// Based on 38.213, Section 10.1
inline bool is_rnti_type_valid_in_search_space(srsran_rnti_type_t rnti_type, srsran_search_space_type_t ss_type)
{
  switch (ss_type) {
    case srsran_search_space_type_common_0:  // fall-through
    case srsran_search_space_type_common_0A: // Other SIBs
      return rnti_type == srsran_rnti_type_si;
    case srsran_search_space_type_common_1:
      return rnti_type == srsran_rnti_type_ra or rnti_type == srsran_rnti_type_tc or
             /* in case of Pcell -> */ rnti_type == srsran_rnti_type_c;
    case srsran_search_space_type_common_2:
      return rnti_type == srsran_rnti_type_p;
    case srsran_search_space_type_common_3:
      return rnti_type == srsran_rnti_type_c; // TODO: Fix
    case srsran_search_space_type_ue:
      return rnti_type == srsran_rnti_type_c or rnti_type == srsran_rnti_type_cs or
             rnti_type == srsran_rnti_type_sp_csi;
    default:
      break;
  }
  return false;
}

/// In case of Common SearchSpace, not all PRBs might be available
void reduce_to_dl_coreset_bw(const bwp_params_t&    bwp_cfg,
                             uint32_t               ss_id,
                             srsran_dci_format_nr_t dci_fmt,
                             prb_grant&             grant);

bool fill_dci_sib(prb_interval        interv,
                  uint32_t            sib_idx,
                  uint32_t            si_ntx,
                  const bwp_params_t& bwp_cfg,
                  srsran_dci_dl_nr_t& dci);

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
