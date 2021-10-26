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

#ifndef SRSRAN_SCHED_NR_H
#define SRSRAN_SCHED_NR_H

#include "sched_nr_cfg.h"
#include "sched_nr_interface.h"
#include "sched_nr_ue.h"
#include "srsran/adt/pool/cached_alloc.h"
#include "srsran/common/slot_point.h"
#include <array>
extern "C" {
#include "srsran/config.h"
}

namespace srsenb {

namespace sched_nr_impl {
class sched_worker_manager;
class serv_cell_manager;
} // namespace sched_nr_impl

class ul_sched_result_buffer;

class sched_nr final : public sched_nr_interface
{
public:
  explicit sched_nr();
  ~sched_nr() override;
  int  config(const sched_args_t& sched_cfg, srsran::const_span<cell_cfg_t> cell_list) override;
  void ue_cfg(uint16_t rnti, const ue_cfg_t& cfg) override;
  void ue_rem(uint16_t rnti) override;
  bool ue_exists(uint16_t rnti) override;

  int dl_rach_info(uint32_t cc, const rar_info_t& rar_info);

  void dl_ack_info(uint16_t rnti, uint32_t cc, uint32_t pid, uint32_t tb_idx, bool ack) override;
  void ul_crc_info(uint16_t rnti, uint32_t cc, uint32_t pid, bool crc) override;
  void ul_sr_info(uint16_t rnti) override;
  void ul_bsr(uint16_t rnti, uint32_t lcg_id, uint32_t bsr) override;
  void dl_buffer_state(uint16_t rnti, uint32_t lcid, uint32_t newtx, uint32_t retx);

  int run_slot(slot_point pdsch_tti, uint32_t cc, dl_res_t& result) override;
  int get_ul_sched(slot_point pusch_tti, uint32_t cc, ul_res_t& result) override;

  void get_metrics(mac_metrics_t& metrics);

private:
  void ue_cfg_impl(uint16_t rnti, const ue_cfg_t& cfg);

  // args
  sched_nr_impl::sched_params cfg;
  srslog::basic_logger*       logger = nullptr;

  using sched_worker_manager = sched_nr_impl::sched_worker_manager;
  std::unique_ptr<sched_worker_manager> sched_workers;

  using ue_map_t = sched_nr_impl::ue_map_t;
  std::mutex ue_db_mutex;
  ue_map_t   ue_db;

  // management of Sched Result buffering
  std::unique_ptr<ul_sched_result_buffer> pending_results;

  // management of cell resources
  std::vector<std::unique_ptr<sched_nr_impl::serv_cell_manager> > cells;
};

} // namespace srsenb

#endif // SRSRAN_SCHED_NR_H
