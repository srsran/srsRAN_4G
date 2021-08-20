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

#ifndef SRSRAN_SCHED_NR_CELL_H
#define SRSRAN_SCHED_NR_CELL_H

#include "sched_nr_cfg.h"
#include "sched_nr_grant_allocator.h"
#include "sched_nr_time_rr.h"
#include "srsran/adt/pool/cached_alloc.h"

namespace srsenb {
namespace sched_nr_impl {

/// SIB scheduler
class si_sched
{
public:
  explicit si_sched(const bwp_params& bwp_cfg_);

  void run_slot(bwp_slot_allocator& slot_alloc);

private:
  const bwp_params*     bwp_cfg = nullptr;
  srslog::basic_logger& logger;

  struct sched_si_t {
    uint32_t     n       = 0;
    uint32_t     len     = 0;
    uint32_t     win_len = 0;
    uint32_t     period  = 0;
    uint32_t     n_tx    = 0;
    alloc_result result  = alloc_result::invalid_coderate;
    slot_point   win_start;
  };
  srsran::bounded_vector<sched_si_t, 10> pending_sis;
};

using dl_sched_rar_info_t = sched_nr_interface::dl_sched_rar_info_t;

/// RAR/Msg3 scheduler
class ra_sched
{
public:
  explicit ra_sched(const bwp_params& bwp_cfg_);

  int  dl_rach_info(const dl_sched_rar_info_t& rar_info);
  void run_slot(bwp_slot_allocator& slot_grid, slot_ue_map_t& slot_ues);
  bool empty() const { return pending_rars.empty(); }

private:
  struct pending_rar_t {
    uint16_t                                                                   ra_rnti = 0;
    slot_point                                                                 prach_slot;
    slot_interval                                                              rar_win;
    srsran::bounded_vector<dl_sched_rar_info_t, sched_interface::MAX_RAR_LIST> msg3_grant;
  };

  alloc_result allocate_pending_rar(bwp_slot_allocator&  slot_grid,
                                    const pending_rar_t& rar,
                                    slot_ue_map_t&       slot_ues,
                                    uint32_t&            nof_grants_alloc);

  const bwp_params*     bwp_cfg = nullptr;
  srslog::basic_logger& logger;

  srsran::deque<pending_rar_t> pending_rars;
};

class bwp_ctxt
{
public:
  explicit bwp_ctxt(const bwp_params& bwp_cfg);

  const bwp_params* cfg;

  // channel-specific schedulers
  ra_sched                       ra;
  std::unique_ptr<sched_nr_base> data_sched;

  // Stores pending allocations and PRB bitmaps
  bwp_res_grid grid;
};

class serv_cell_manager
{
public:
  using feedback_callback_t = srsran::move_callback<void(ue_carrier&)>;

  explicit serv_cell_manager(const sched_cell_params& cell_cfg_);

  srsran::bounded_vector<bwp_ctxt, SCHED_NR_MAX_BWP_PER_CELL> bwps;
  const sched_cell_params&                                    cfg;

private:
  srslog::basic_logger& logger;
};

} // namespace sched_nr_impl
} // namespace srsenb

#endif // SRSRAN_SCHED_NR_CELL_H
