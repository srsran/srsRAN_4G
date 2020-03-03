/*
 * Copyright 2013-2019 Software Radio Systems Limited
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
#include "srslte/interfaces/common_interfaces.h"
#include "srslte/interfaces/enb_interfaces.h"
#include "srslte/interfaces/enb_metrics_interface.h"
#include "srslte/phy/channel/channel.h"
#include "srslte/radio/radio.h"
#include <map>
#include <semaphore.h>
#include <string.h>

namespace srsenb {

class phy_common
{
public:
  explicit phy_common(uint32_t nof_workers);
  ~phy_common();

  void set_nof_workers(uint32_t nof_workers);

  bool
       init(const phy_cell_cfg_list_t& cell_list_, srslte::radio_interface_phy* radio_handler, stack_interface_phy_lte* mac);
  void reset();
  void stop();

  void
  worker_end(uint32_t tx_mutex_cnt, cf_t* buffer[SRSLTE_MAX_PORTS], uint32_t nof_samples, srslte_timestamp_t tx_time);

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
  float get_ul_freq_hz(uint32_t cc_idx)
  {
    float ret = 0.0f;

    if (cc_idx < cell_list.size()) {
      ret = cell_list[cc_idx].ul_freq_hz;
    }

    return ret;
  };
  float get_dl_freq_hz(uint32_t cc_idx)
  {
    float ret = 0.0f;

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

  // Physical Uplink Config common
  srslte_ul_cfg_t ul_cfg_com = {};

  // Physical Downlink Config common
  srslte_dl_cfg_t dl_cfg_com = {};

  srslte::radio_interface_phy* radio      = nullptr;
  stack_interface_phy_lte*     stack      = nullptr;
  srslte::channel_ptr          dl_channel = nullptr;

  // Common objects for scheduling grants
  stack_interface_phy_lte::ul_sched_list_t ul_grants[TTIMOD_SZ] = {};
  stack_interface_phy_lte::dl_sched_list_t dl_grants[TTIMOD_SZ] = {};

  /**
   * UE Database object, direct public access, all PHY threads should be able to access this attribute directly
   */
  phy_ue_db ue_db;

  void configure_mbsfn(phy_interface_stack_lte::phy_cfg_mbsfn_t* cfg);
  void build_mch_table();
  void build_mcch_table();
  bool is_mbsfn_sf(srslte_mbsfn_cfg_t* cfg, uint32_t phy_tti);
  void set_mch_period_stop(uint32_t stop);

private:
  phy_cell_cfg_list_t cell_list;
  std::vector<sem_t>  tx_sem;
  bool                is_first_tx = false;

  uint32_t nof_workers = 0;
  uint32_t max_workers = 0;

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
