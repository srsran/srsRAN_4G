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

#include "srsenb/hdr/stack/mac/nr/sched_nr_bwp.h"
#include "srsran/common/standard_streams.h"
#include "srsran/common/string_helpers.h"

namespace srsenb {
namespace sched_nr_impl {

ra_sched::ra_sched(const sched_cell_params& cell_cfg_) : cell_cfg(&cell_cfg_), logger(srslog::fetch_basic_logger("MAC"))
{}

alloc_result
ra_sched::allocate_pending_rar(bwp_slot_allocator& slot_grid, const pending_rar_t& rar, uint32_t& nof_grants_alloc)
{
  const uint32_t rar_aggr_level = 2;
  auto&          pdsch_bitmap   = slot_grid.res_grid()[slot_grid.get_pdcch_tti()].dl_rbgs;

  alloc_result ret = alloc_result::other_cause;
  for (nof_grants_alloc = rar.msg3_grant.size(); nof_grants_alloc > 0; nof_grants_alloc--) {
    ret = alloc_result::invalid_coderate;
    for (uint32_t nrbg = 1; nrbg < cell_cfg->cell_cfg.nof_rbg and ret == alloc_result::invalid_coderate; ++nrbg) {
      rbg_interval rbg_interv = find_empty_rbg_interval(pdsch_bitmap, nrbg);
      if (rbg_interv.length() == nrbg) {
        ret = slot_grid.alloc_rar(rar_aggr_level, rar, rbg_interv, nof_grants_alloc);
      } else {
        ret = alloc_result::no_sch_space;
      }
    }

    // If allocation was not successful because there were not enough RBGs, try allocating fewer Msg3 grants
    if (ret != alloc_result::invalid_coderate and ret != alloc_result::no_sch_space) {
      break;
    }
  }
  if (ret != alloc_result::success) {
    logger.info("SCHED: RAR allocation for L=%d was postponed. Cause=%s", rar_aggr_level, to_string(ret));
  }
  return ret;
}

void ra_sched::run_slot(bwp_slot_allocator& slot_grid)
{
  static const uint32_t PRACH_RAR_OFFSET = 3;
  tti_point             pdcch_tti        = slot_grid.get_pdcch_tti();

  for (auto it = pending_rars.begin(); it != pending_rars.end();) {
    pending_rar_t& rar = *it;

    // In case of RAR outside RAR window:
    // - if window has passed, discard RAR
    // - if window hasn't started, stop loop, as RARs are ordered by TTI
    tti_interval rar_window{rar.prach_tti + PRACH_RAR_OFFSET,
                            rar.prach_tti + PRACH_RAR_OFFSET + cell_cfg->cell_cfg.rar_window_size};
    if (not rar_window.contains(pdcch_tti)) {
      if (pdcch_tti >= rar_window.stop()) {
        fmt::memory_buffer str_buffer;
        fmt::format_to(str_buffer,
                       "SCHED: Could not transmit RAR within the window (RA={}, Window={}, RAR={}",
                       rar.prach_tti,
                       rar_window,
                       pdcch_tti);
        srsran::console("%s\n", srsran::to_c_str(str_buffer));
        logger.warning("%s", srsran::to_c_str(str_buffer));
        it = pending_rars.erase(it);
        continue;
      }
      return;
    }

    // Try to schedule DCI + RBGs for RAR Grant
    uint32_t     nof_rar_allocs = 0;
    alloc_result ret            = allocate_pending_rar(slot_grid, rar, nof_rar_allocs);

    if (ret == alloc_result::success) {
      // If RAR allocation was successful:
      // - in case all Msg3 grants were allocated, remove pending RAR, and continue with following RAR
      // - otherwise, erase only Msg3 grants that were allocated, and stop iteration

      if (nof_rar_allocs == rar.msg3_grant.size()) {
        it = pending_rars.erase(it);
      } else {
        std::copy(rar.msg3_grant.begin() + nof_rar_allocs, rar.msg3_grant.end(), rar.msg3_grant.begin());
        rar.msg3_grant.resize(rar.msg3_grant.size() - nof_rar_allocs);
        break;
      }
    } else {
      // If RAR allocation was not successful:
      // - in case of unavailable PDCCH space, try next pending RAR allocation
      // - otherwise, stop iteration
      if (ret != alloc_result::no_cch_space) {
        break;
      }
      ++it;
    }
  }
}

int ra_sched::dl_rach_info(const dl_sched_rar_info_t& rar_info)
{
  logger.info("SCHED: New PRACH tti=%d, preamble=%d, temp_crnti=0x%x, ta_cmd=%d, msg3_size=%d",
              rar_info.prach_tti,
              rar_info.preamble_idx,
              rar_info.temp_crnti,
              rar_info.ta_cmd,
              rar_info.msg3_size);

  // RA-RNTI = 1 + t_id + f_id
  // t_id = index of first subframe specified by PRACH (0<=t_id<10)
  // f_id = index of the PRACH within subframe, in ascending order of freq domain (0<=f_id<6) (for FDD, f_id=0)
  uint16_t ra_rnti = 1 + (uint16_t)(rar_info.prach_tti % 10u);

  // find pending rar with same RA-RNTI
  for (pending_rar_t& r : pending_rars) {
    if (r.prach_tti.to_uint() == rar_info.prach_tti and ra_rnti == r.ra_rnti) {
      if (r.msg3_grant.size() >= sched_interface::MAX_RAR_LIST) {
        logger.warning("PRACH ignored, as the the maximum number of RAR grants per tti has been reached");
        return SRSRAN_ERROR;
      }
      r.msg3_grant.push_back(rar_info);
      return SRSRAN_SUCCESS;
    }
  }

  // create new RAR
  pending_rar_t p;
  p.ra_rnti   = ra_rnti;
  p.prach_tti = tti_point{rar_info.prach_tti};
  p.msg3_grant.push_back(rar_info);
  pending_rars.push_back(p);

  return SRSRAN_SUCCESS;
}

bwp_sched::bwp_sched(const sched_cell_params& cell_cfg_, uint32_t bwp_id_) :
  cell_cfg(&cell_cfg_), bwp_id(bwp_id_), ra(cell_cfg_), grid(cell_cfg_, bwp_id_)
{}

cell_sched::cell_sched(const sched_cell_params& cell_cfg_) : cfg(&cell_cfg_)
{
  for (uint32_t bwp_id = 0; bwp_id < cfg->cell_cfg.bwps.size(); ++bwp_id) {
    bwps.emplace_back(cell_cfg_, bwp_id);
  }
}

} // namespace sched_nr_impl
} // namespace srsenb
