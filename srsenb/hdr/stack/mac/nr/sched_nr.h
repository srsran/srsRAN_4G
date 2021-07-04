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

#include "sched_nr_common.h"
#include "sched_nr_interface.h"
#include "sched_nr_ue.h"
#include "srsran/adt/pool/cached_alloc.h"
#include "srsran/common/tti_point.h"
#include <array>
extern "C" {
#include "srsran/config.h"
}

namespace srsenb {

namespace sched_nr_impl {
class sched_worker_manager;
}

class ue_event_manager;

class sched_nr final : public sched_nr_interface
{
public:
  explicit sched_nr(const sched_cfg_t& sched_cfg);
  ~sched_nr() override;
  int  cell_cfg(srsran::const_span<cell_cfg_t> cell_list) override;
  void ue_cfg(uint16_t rnti, const ue_cfg_t& cfg) override;

  void slot_indication(tti_point tti_rx) override;
  int  generate_sched_result(tti_point tti_rx, uint32_t cc, tti_request_t& tti_req) override;

  void dl_ack_info(uint16_t rnti, uint32_t cc, uint32_t pid, uint32_t tb_idx, bool ack) override;
  void ul_sr_info(tti_point tti_rx, uint16_t rnti) override;

private:
  void ue_cfg_impl(uint16_t rnti, const ue_cfg_t& cfg);

  // args
  sched_nr_impl::sched_params cfg;

  using sched_worker_manager = sched_nr_impl::sched_worker_manager;
  std::unique_ptr<sched_worker_manager> sched_workers;

  using ue_map_t = sched_nr_impl::ue_map_t;
  std::mutex ue_db_mutex;
  ue_map_t   ue_db;

  // management of PHY UE feedback
  std::unique_ptr<ue_event_manager> pending_events;
};

} // namespace srsenb

#endif // SRSRAN_SCHED_NR_H
