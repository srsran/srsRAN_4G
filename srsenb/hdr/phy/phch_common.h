/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2017 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of srsLTE.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
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

#include <map>
#include "srslte/interfaces/enb_interfaces.h"
#include "srslte/interfaces/enb_metrics_interface.h"
#include "srslte/common/gen_mch_tables.h"
#include "srslte/common/log.h"
#include "srslte/common/threads.h"
#include "srslte/common/thread_pool.h"
#include "srslte/radio/radio.h"
#include <string.h>
namespace srsenb {

typedef struct {
  float max_prach_offset_us; 
  int pusch_max_its;
  float tx_amplitude; 
  int nof_phy_threads;  
  std::string equalizer_mode; 
  float estimator_fil_w;   
  bool       pregenerate_signals;
} phy_args_t; 

typedef enum{
  SUBFRAME_TYPE_REGULAR = 0,
  SUBFRAME_TYPE_MBSFN,
  SUBFRAME_TYPE_N_ITEMS,
} subframe_type_t;
static const char subframe_type_text[SUBFRAME_TYPE_N_ITEMS][20] = {"Regular", "MBSFN"};

/* Subframe config */

typedef struct {
  subframe_type_t sf_type;
  uint8_t         mbsfn_area_id;
  uint8_t         non_mbsfn_region_length;
  uint8_t         mbsfn_mcs;
  bool            mbsfn_encode;
  bool            is_mcch;
} subframe_cfg_t;



class phch_common
{
public:
 
  
  phch_common(uint32_t max_mutex_) : tx_mutex(max_mutex_) {
    nof_mutex = 0;
    max_mutex = max_mutex_; 
    params.max_prach_offset_us = 20;
    radio = NULL;
    mac = NULL;
    is_first_tx = false;
    is_first_of_burst = false;
    pdsch_p_b = 0;
    nof_workers = 0;
    bzero(&pusch_cfg, sizeof(pusch_cfg));
    bzero(&hopping_cfg, sizeof(hopping_cfg));
    bzero(&pucch_cfg, sizeof(pucch_cfg));
    bzero(&ul_grants, sizeof(ul_grants));
  }
  
  bool init(srslte_cell_t *cell, srslte::radio *radio_handler, mac_interface_phy *mac);  
  void reset(); 
  void stop();
  
  void set_nof_mutex(uint32_t nof_mutex); 

  void worker_end(uint32_t tx_mutex_cnt, cf_t *buffer[SRSLTE_MAX_PORTS], uint32_t nof_samples, srslte_timestamp_t tx_time);

  // Common objects
  srslte_cell_t                     cell; 
  srslte_refsignal_dmrs_pusch_cfg_t pusch_cfg; 
  srslte_pusch_hopping_cfg_t        hopping_cfg;
  srslte_pucch_cfg_t                pucch_cfg; 
  uint8_t                           pdsch_p_b;
  phy_args_t                        params; 

  srslte::radio     *radio;
  mac_interface_phy *mac; 
  
  // Common objects for schedulign grants 
  mac_interface_phy::ul_sched_t ul_grants[TTIMOD_SZ];
  mac_interface_phy::dl_sched_t dl_grants[TTIMOD_SZ];
  
  // Map of pending ACKs for each user 
  typedef struct {
    bool is_pending[TTIMOD_SZ][SRSLTE_MAX_TB];
    uint16_t n_pdcch[TTIMOD_SZ];
  } pending_ack_t;

  class common_ue {
   public:
    pending_ack_t pending_ack;
    uint8_t ri;
    int last_ul_tbs[2*HARQ_DELAY_MS];
    srslte_mod_t last_ul_mod[2*HARQ_DELAY_MS];
  };

  std::map<uint16_t, common_ue> common_ue_db;
  
  void ue_db_add_rnti(uint16_t rnti);
  void ue_db_rem_rnti(uint16_t rnti);
  void ue_db_clear(uint32_t sf_idx);
  void ue_db_set_ack_pending(uint32_t sf_idx, uint16_t rnti, uint32_t tb_idx, uint32_t n_pdcch);
  bool ue_db_is_ack_pending(uint32_t sf_idx, uint16_t rnti, uint32_t tb_idx, uint32_t *last_n_pdcch = NULL);
  void ue_db_set_ri(uint16_t rnti, uint8_t ri);
  uint8_t ue_db_get_ri(uint16_t rnti);
  void ue_db_set_last_ul_mod(uint16_t rnti, uint32_t tti, srslte_mod_t mcs);
  srslte_mod_t ue_db_get_last_ul_mod(uint16_t rnti, uint32_t tti);
  void ue_db_set_last_ul_tbs(uint16_t rnti, uint32_t tti, int tbs);
  int ue_db_get_last_ul_tbs(uint16_t rnti, uint32_t tti);
  
  void configure_mbsfn(phy_interface_rrc::phy_cfg_mbsfn_t *cfg);
  void build_mch_table();
  void build_mcch_table();
  void get_sf_config(subframe_cfg_t *cfg, uint32_t phy_tti);
  

private:
  std::vector<pthread_mutex_t>    tx_mutex; 
  bool            is_first_tx;
  bool            is_first_of_burst; 

  uint32_t        nof_workers;
  uint32_t        nof_mutex;
  uint32_t        max_mutex;

  pthread_mutex_t user_mutex;
  
  phy_interface_rrc::phy_cfg_mbsfn_t  mbsfn;
  bool sib13_configured;
  bool mcch_configured;
  uint8_t mch_table[40];
  uint8_t mcch_table[10];
  
  uint8_t mch_sf_idx_lut[10];
  bool is_mch_subframe(subframe_cfg_t *cfg, uint32_t phy_tti);
  bool is_mcch_subframe(subframe_cfg_t *cfg, uint32_t phy_tti);

  void add_rnti(uint16_t rnti);
  
};

} // namespace srsenb

#endif // SRSENB_PHCH_COMMON_H
