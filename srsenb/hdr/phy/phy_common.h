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

#include "srslte/common/gen_mch_tables.h"
#include "srslte/common/interfaces_common.h"
#include "srslte/common/log.h"
#include "srslte/common/thread_pool.h"
#include "srslte/common/threads.h"
#include "srslte/interfaces/common_interfaces.h"
#include "srslte/interfaces/enb_interfaces.h"
#include "srslte/interfaces/enb_metrics_interface.h"
#include "srslte/radio/radio.h"
#include <map>
#include <semaphore.h>
#include <string.h>

namespace srsenb {

typedef struct {
  std::string            type;
  srslte::phy_log_args_t log;

  float       max_prach_offset_us;
  int         pusch_max_its;
  bool        pusch_8bit_decoder;
  float       tx_amplitude;
  int         nof_phy_threads;
  std::string equalizer_mode;
  float       estimator_fil_w;
  bool        pregenerate_signals;
} phy_args_t;

class phy_common
{
public:
  phy_common(uint32_t nof_workers);
  ~phy_common();

  void set_nof_workers(uint32_t nof_workers);

  bool init(const srslte_cell_t& cell_, srslte::radio_interface_phy* radio_handler, stack_interface_phy_lte* mac);
  void reset(); 
  void stop();
  
  void worker_end(uint32_t tx_mutex_cnt, cf_t *buffer[SRSLTE_MAX_PORTS], uint32_t nof_samples, srslte_timestamp_t tx_time);

  // Common objects
  srslte_cell_t cell;
  phy_args_t    params;

  // Physical Uplink Config common
  srslte_ul_cfg_t ul_cfg_com;

  // Physical Downlink Config common
  srslte_dl_cfg_t dl_cfg_com;

  srslte::radio_interface_phy* radio;
  stack_interface_phy_lte* stack;

  // Common objects for schedulign grants
  stack_interface_phy_lte::ul_sched_t ul_grants[TTIMOD_SZ];
  stack_interface_phy_lte::dl_sched_t dl_grants[TTIMOD_SZ];

  // Map of pending ACKs for each user 
  typedef struct {
    bool is_pending[TTIMOD_SZ][SRSLTE_MAX_TB];
    uint16_t n_pdcch[TTIMOD_SZ];
  } pending_ack_t;

  class common_ue {
   public:
    pending_ack_t pending_ack;
    uint8_t ri;
    srslte_ra_tb_t last_tb[SRSLTE_MAX_HARQ_PROC];
  };

  std::map<uint16_t, common_ue> common_ue_db;
  
  void ue_db_add_rnti(uint16_t rnti);
  void ue_db_rem_rnti(uint16_t rnti);
  void    ue_db_clear(uint32_t tti);
  void    ue_db_set_ack_pending(uint32_t tti, uint16_t rnti, uint32_t tb_idx, uint32_t n_pdcch);
  bool    ue_db_is_ack_pending(uint32_t tti, uint16_t rnti, uint32_t tb_idx, uint32_t* last_n_pdcch = NULL);
  void ue_db_set_ri(uint16_t rnti, uint8_t ri);
  uint8_t ue_db_get_ri(uint16_t rnti);

  void           ue_db_set_last_ul_tb(uint16_t rnti, uint32_t pid, srslte_ra_tb_t tb);
  srslte_ra_tb_t ue_db_get_last_ul_tb(uint16_t rnti, uint32_t pid);

  void configure_mbsfn(phy_interface_stack_lte::phy_cfg_mbsfn_t* cfg);
  void build_mch_table();
  void build_mcch_table();
  bool is_mbsfn_sf(srslte_mbsfn_cfg_t* cfg, uint32_t phy_tti);
  void set_mch_period_stop(uint32_t stop);

private:
  std::vector<sem_t>    tx_sem;
  bool            is_first_tx;
  bool            is_first_of_burst; 

  uint32_t        nof_workers;
  uint32_t        max_workers;

  pthread_mutex_t user_mutex;

  bool                                have_mtch_stop;
  pthread_mutex_t                     mtch_mutex;
  pthread_cond_t                      mtch_cvar;
  phy_interface_stack_lte::phy_cfg_mbsfn_t mbsfn;
  bool sib13_configured;
  bool mcch_configured;
  uint8_t                                  mch_table[40]  = {};
  uint8_t                                  mcch_table[10] = {};
  uint32_t                            mch_period_stop;
  uint8_t                                  mch_sf_idx_lut[10] = {};
  bool    is_mch_subframe(srslte_mbsfn_cfg_t* cfg, uint32_t phy_tti);
  bool    is_mcch_subframe(srslte_mbsfn_cfg_t* cfg, uint32_t phy_tti);

  void add_rnti(uint16_t rnti);
  
};

} // namespace srsenb

#endif // SRSENB_PHCH_COMMON_H
