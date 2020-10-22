/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSENB_PHCH_COMMON_H
#define SRSENB_PHCH_COMMON_H

#include "phy_interfaces.h"
#include "srsenb/hdr/phy/phy_ue_db.h"
#include "srslte/common/gen_mch_tables.h"
#include "srslte/common/interfaces_common.h"
#include "srslte/common/log.h"
#include "srslte/common/thread_pool.h"
#include "srslte/common/threads.h"
#include "srslte/interfaces/enb_interfaces.h"
#include "srslte/interfaces/enb_metrics_interface.h"
#include "srslte/interfaces/radio_interfaces.h"
#include "srslte/phy/channel/channel.h"
#include "srslte/radio/radio.h"
#include <map>
#include <srslte/common/tti_sempahore.h>
#include <string.h>

namespace srsenb {

class phy_common
{
public:
  phy_common() = default;

  bool
       init(const phy_cell_cfg_list_t& cell_list_, srslte::radio_interface_phy* radio_handler, stack_interface_phy_lte* mac);
  void reset();
  void stop();

  /**
   * TTI transmission semaphore, used for ensuring that PHY workers transmit following start order
   */
  srslte::tti_semaphore<void*> semaphore;

  /**
   * Performs common end worker transmission tasks such as transmission and stack TTI execution
   *
   * @param tx_sem_id Semaphore identifier, the worker thread pointer is used
   * @param buffer baseband IQ sample buffer
   * @param tx_time timestamp to transmit samples
   */
  void worker_end(void* tx_sem_id, srslte::rf_buffer_t& buffer, srslte::rf_timestamp_t& tx_time);

  // Common objects
  phy_args_t params = {};

  uint32_t get_nof_carriers() { return static_cast<uint32_t>(cell_list.size()); };
  uint32_t get_nof_prb(uint32_t cc_idx)
  {
    uint32_t ret = 0;

    if (cc_idx < cell_list.size()) {
      ret = cell_list[cc_idx].cell.nof_prb;
    }

    return ret;
  };
  uint32_t get_nof_ports(uint32_t cc_idx)
  {
    uint32_t ret = 0;

    if (cc_idx < cell_list.size()) {
      ret = cell_list[cc_idx].cell.nof_ports;
    }

    return ret;
  };
  uint32_t get_nof_rf_channels()
  {
    uint32_t count = 0;

    for (auto& cell : cell_list) {
      count += cell.cell.nof_ports;
    }

    return count;
  }
  double get_ul_freq_hz(uint32_t cc_idx)
  {
    double ret = 0.0;

    if (cc_idx < cell_list.size()) {
      ret = cell_list[cc_idx].ul_freq_hz;
    }

    return ret;
  };
  double get_dl_freq_hz(uint32_t cc_idx)
  {
    double ret = 0.0;

    if (cc_idx < cell_list.size()) {
      ret = cell_list[cc_idx].dl_freq_hz;
    }

    return ret;
  };
  uint32_t get_rf_port(uint32_t cc_idx)
  {
    uint32_t ret = 0;

    if (cc_idx < cell_list.size()) {
      ret = cell_list[cc_idx].rf_port;
    }

    return ret;
  };
  srslte_cell_t get_cell(uint32_t cc_idx)
  {
    srslte_cell_t c = {};
    if (cc_idx < cell_list.size()) {
      c = cell_list[cc_idx].cell;
    }
    return c;
  };

  void set_cell_gain(uint32_t cell_id, float gain_db)
  {
    auto it =
        std::find_if(cell_list.begin(), cell_list.end(), [cell_id](phy_cell_cfg_t& x) { return x.cell_id == cell_id; });

    // Check if the cell was found;
    if (it == cell_list.end()) {
      srslte::console("cell ID %d not found\n", cell_id);
      return;
    }

    it->gain_db = gain_db;
  }

  float get_cell_gain(uint32_t cc_idx)
  {
    if (cc_idx < cell_list.size()) {
      return cell_list.at(cc_idx).gain_db;
    }
    return 0.0f;
  }

  // Common Physical Uplink DMRS configuration
  srslte_refsignal_dmrs_pusch_cfg_t dmrs_pusch_cfg = {};

  srslte::radio_interface_phy* radio      = nullptr;
  stack_interface_phy_lte*     stack      = nullptr;
  srslte::channel_ptr          dl_channel = nullptr;

  /**
   * UE Database object, direct public access, all PHY threads should be able to access this attribute directly
   */
  phy_ue_db ue_db;

  void configure_mbsfn(phy_interface_stack_lte::phy_cfg_mbsfn_t* cfg);
  void build_mch_table();
  void build_mcch_table();
  bool is_mbsfn_sf(srslte_mbsfn_cfg_t* cfg, uint32_t phy_tti);
  void set_mch_period_stop(uint32_t stop);

  // Getters and setters for ul grants which need to be shared between workers
  const stack_interface_phy_lte::ul_sched_list_t& get_ul_grants(uint32_t tti);
  void set_ul_grants(uint32_t tti, const stack_interface_phy_lte::ul_sched_list_t& ul_grants);
  void clear_grants(uint16_t rnti);

private:
  // Common objects for scheduling grants
  srslte::circular_array<stack_interface_phy_lte::ul_sched_list_t, TTIMOD_SZ> ul_grants   = {};
  std::mutex                                                                  grant_mutex = {};

  phy_cell_cfg_list_t cell_list;

  bool                                     have_mtch_stop   = false;
  pthread_mutex_t                          mtch_mutex       = {};
  pthread_cond_t                           mtch_cvar        = {};
  phy_interface_stack_lte::phy_cfg_mbsfn_t mbsfn            = {};
  bool                                     sib13_configured = false;
  bool                                     mcch_configured  = false;
  uint8_t                                  mch_table[40]    = {};
  uint8_t                                  mcch_table[10]   = {};
  uint32_t                                 mch_period_stop  = 0;
  bool                                     is_mch_subframe(srslte_mbsfn_cfg_t* cfg, uint32_t phy_tti);
  bool                                     is_mcch_subframe(srslte_mbsfn_cfg_t* cfg, uint32_t phy_tti);
};

} // namespace srsenb

#endif // SRSENB_PHCH_COMMON_H
