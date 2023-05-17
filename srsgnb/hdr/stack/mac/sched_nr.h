/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#ifndef SRSRAN_SCHED_NR_H
#define SRSRAN_SCHED_NR_H

#include "sched_nr_cfg.h"
#include "sched_nr_interface.h"
#include "sched_nr_ue.h"
#include "srsran/adt/pool/cached_alloc.h"
#include "srsran/adt/pool/circular_stack_pool.h"
#include "srsran/common/slot_point.h"
#include <array>
extern "C" {
#include "srsran/config.h"
}

namespace srsenb {

namespace sched_nr_impl {

class cc_worker;

} // namespace sched_nr_impl

class sched_nr final : public sched_nr_interface
{
public:
  explicit sched_nr();
  ~sched_nr() override;

  void stop();
  int  config(const sched_args_t& sched_cfg, srsran::const_span<sched_nr_cell_cfg_t> cell_list) override;
  void ue_cfg(uint16_t rnti, const ue_cfg_t& cfg) override;
  void ue_rem(uint16_t rnti) override;

  int dl_rach_info(const rar_info_t& rar_info);

  void dl_ack_info(uint16_t rnti, uint32_t cc, uint32_t pid, uint32_t tb_idx, bool ack) override;
  void ul_crc_info(uint16_t rnti, uint32_t cc, uint32_t pid, bool crc) override;
  void ul_sr_info(uint16_t rnti) override;
  void ul_bsr(uint16_t rnti, uint32_t lcg_id, uint32_t bsr) override;
  void dl_buffer_state(uint16_t rnti, uint32_t lcid, uint32_t newtx, uint32_t retx);
  void dl_mac_ce(uint16_t rnti, uint32_t ce_lcid) override;
  void dl_cqi_info(uint16_t rnti, uint32_t cc, uint32_t cqi_value);

  /// Called once per slot in a non-concurrent fashion
  void      slot_indication(slot_point slot_tx) override;
  dl_res_t* get_dl_sched(slot_point pdsch_tti, uint32_t cc) override;
  ul_res_t* get_ul_sched(slot_point pusch_tti, uint32_t cc) override;

  void get_metrics(mac_metrics_t& metrics);

private:
  int ue_cfg_impl(uint16_t rnti, const ue_cfg_t& cfg);
  int add_ue_impl(uint16_t rnti, sched_nr_impl::unique_ue_ptr u);

  // args
  sched_nr_impl::sched_params_t cfg;
  srslog::basic_logger*         logger = nullptr;

  // slot-specific
  slot_point       current_slot_tx;
  std::atomic<int> worker_count{0};

  using slot_cc_worker = sched_nr_impl::cc_worker;
  std::vector<std::unique_ptr<sched_nr_impl::cc_worker> > cc_workers;

  // UE Database
  std::unique_ptr<srsran::circular_stack_pool<SRSENB_MAX_UES> > ue_pool;
  using ue_map_t = sched_nr_impl::ue_map_t;
  ue_map_t ue_db;

  // Feedback management
  class event_manager;
  std::unique_ptr<event_manager> pending_events;

  // metrics extraction
  class ue_metrics_manager;
  std::unique_ptr<ue_metrics_manager> metrics_handler;
};

} // namespace srsenb

#endif // SRSRAN_SCHED_NR_H
