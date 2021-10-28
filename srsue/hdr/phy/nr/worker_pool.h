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

#ifndef SRSUE_NR_WORKER_POOL_H
#define SRSUE_NR_WORKER_POOL_H

#include "sf_worker.h"
#include "srsran/common/thread_pool.h"
#include "srsue/hdr/phy/prach.h"

namespace srsue {
namespace nr {

class worker_pool : public srsue::phy_interface_stack_nr
{
private:
  srslog::basic_logger&                    logger;
  srsran::thread_pool                      pool;
  std::vector<std::unique_ptr<sf_worker> > workers;
  state                                    phy_state;
  std::unique_ptr<prach>                   prach_buffer       = nullptr;
  uint32_t                                 prach_nof_sf       = 0;
  uint32_t                                 prach_sf_count     = 0;
  cf_t*                                    prach_ptr          = nullptr;
  float                                    prach_target_power = 0.0f;
  uint32_t                                 sf_sz              = 0;
  srsran::phy_cfg_nr_t                     cfg{};
  std::vector<bool>                        pending_cfgs;
  std::mutex                               cfg_mutex;

public:
  sf_worker* operator[](std::size_t pos) { return workers.at(pos).get(); }

  worker_pool(uint32_t max_workers);
  bool init(const phy_args_nr_t& args_, srsran::phy_common_interface& common, stack_interface_phy_nr* stack_, int prio);
  sf_worker* wait_worker(uint32_t tti);
  void       start_worker(sf_worker* w);
  void       stop();
  void       send_prach(const uint32_t prach_occasion,
                        const int      preamble_index,
                        const float    preamble_received_target_power,
                        const float    ta_base_sec = 0.0f) override;
  int        set_ul_grant(uint32_t                                       rx_tti,
                          std::array<uint8_t, SRSRAN_RAR_UL_GRANT_NBITS> array,
                          uint16_t                                       rnti,
                          srsran_rnti_type_t                             rnti_type) override;
  bool       set_config(const srsran::phy_cfg_nr_t& cfg) override;
  bool       has_valid_sr_resource(uint32_t sr_id) override;
  void       clear_pending_grants() override;
  void       get_metrics(phy_metrics_t& m);
  int        tx_request(const tx_request_t& request) override;

  /**
   * @brief Sets external CFO to compensate UL signal frequency offset
   * @param ext_cfo_hz External CFO in Hz
   */
  void set_ul_ext_cfo(float ext_cfo_hz) { phy_state.set_ul_ext_cfo(ext_cfo_hz); }
};

} // namespace nr
} // namespace srsue

#endif // SRSUE_NR_WORKER_POOL_H
