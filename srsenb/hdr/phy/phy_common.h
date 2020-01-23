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
  phy_common(uint32_t nof_workers);
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
  uint32_t get_ul_earfcn(uint32_t cc_idx)
  {
    uint32_t ret = 0;

    if (cc_idx < cell_list.size()) {
      ret = cell_list[cc_idx].ul_earfcn;

      // If there is no UL-EARFCN, deduce it from DL-EARFCN
      if (ret == 0) {
        ret = srslte_band_ul_earfcn(cell_list[cc_idx].dl_earfcn);
      }
    }

    return ret;
  };
  uint32_t get_dl_earfcn(uint32_t cc_idx)
  {
    uint32_t ret = 0;

    if (cc_idx < cell_list.size()) {
      ret = cell_list[cc_idx].dl_earfcn;
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

  // Common objects for schedulign grants
  stack_interface_phy_lte::ul_sched_list_t ul_grants[TTIMOD_SZ] = {};
  stack_interface_phy_lte::dl_sched_list_t dl_grants[TTIMOD_SZ] = {};

  // Map of pending ACKs for each user
  typedef struct {
    bool     is_pending[TTIMOD_SZ][SRSLTE_MAX_TB];
    uint16_t n_pdcch[TTIMOD_SZ];
  } pending_ack_t;

  class common_ue
  {
  public:
    pending_ack_t  pending_ack                   = {};
    uint8_t        ri                            = 0;
    srslte_ra_tb_t last_tb[SRSLTE_MAX_HARQ_PROC] = {};
  };

  std::map<uint16_t, common_ue> common_ue_db;

  void    ue_db_add_rnti(uint16_t rnti);
  void    ue_db_rem_rnti(uint16_t rnti);
  void    ue_db_clear(uint32_t tti);
  void    ue_db_set_ack_pending(uint32_t tti, uint16_t rnti, uint32_t tb_idx, uint32_t n_pdcch);
  bool    ue_db_is_ack_pending(uint32_t tti, uint16_t rnti, uint32_t tb_idx, uint32_t* last_n_pdcch = nullptr);
  void    ue_db_set_ri(uint16_t rnti, uint8_t ri);
  uint8_t ue_db_get_ri(uint16_t rnti);

  void           ue_db_set_last_ul_tb(uint16_t rnti, uint32_t pid, srslte_ra_tb_t tb);
  srslte_ra_tb_t ue_db_get_last_ul_tb(uint16_t rnti, uint32_t pid);

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

  std::mutex user_mutex = {};

  bool                                     have_mtch_stop     = false;
  pthread_mutex_t                          mtch_mutex         = {};
  pthread_cond_t                           mtch_cvar          = {};
  phy_interface_stack_lte::phy_cfg_mbsfn_t mbsfn              = {};
  bool                                     sib13_configured   = false;
  bool                                     mcch_configured    = false;
  uint8_t                                  mch_table[40]      = {};
  uint8_t                                  mcch_table[10]     = {};
  uint32_t                                 mch_period_stop    = 0;
  uint8_t                                  mch_sf_idx_lut[10] = {};
  bool                                     is_mch_subframe(srslte_mbsfn_cfg_t* cfg, uint32_t phy_tti);
  bool                                     is_mcch_subframe(srslte_mbsfn_cfg_t* cfg, uint32_t phy_tti);

  void add_rnti(uint16_t rnti);
};

} // namespace srsenb

#endif // SRSENB_PHCH_COMMON_H
