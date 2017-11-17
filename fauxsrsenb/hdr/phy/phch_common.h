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

#ifndef ENBPHCHCOMMON_H
#define ENBPHCHCOMMON_H

#include <map>
#include "srslte/interfaces/enb_interfaces.h"
#include "srslte/interfaces/enb_metrics_interface.h"

#include "srslte/common/log.h"
#include "srslte/common/threads.h"
#include "srslte/common/thread_pool.h"
#include "srslte/radio/radio.h"

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

class phch_common
{
public:
 
  
  phch_common(uint32_t max_mutex_) : tx_mutex(max_mutex_) {
    nof_mutex = 0;
    max_mutex = max_mutex_; 
    params.max_prach_offset_us = 20; 
  }
  
  bool init(srslte_cell_t *cell, srslte::radio *radio_handler, mac_interface_phy *mac);  
  void reset(); 
  void stop();
  
  void set_nof_mutex(uint32_t nof_mutex); 

  void worker_end(uint32_t tx_mutex_cnt, cf_t *buffer, uint32_t nof_samples, srslte_timestamp_t tx_time);

  // Common objects
  srslte_cell_t                     cell; 
  srslte_refsignal_dmrs_pusch_cfg_t pusch_cfg; 
  srslte_pusch_hopping_cfg_t        hopping_cfg;
  srslte_pucch_cfg_t                pucch_cfg; 
  phy_args_t                        params; 

  srslte::radio     *radio;
  mac_interface_phy *mac; 
  
  // Common objects for schedulign grants 
  mac_interface_phy::ul_sched_t ul_grants[10];
  mac_interface_phy::dl_sched_t dl_grants[10];
  
  // Map of pending ACKs for each user 
  typedef struct {
    bool is_pending[10]; 
    uint16_t n_pdcch[10];
  } pending_ack_t;
  std::map<uint16_t,pending_ack_t> pending_ack;
  
  void ack_add_rnti(uint16_t rnti);
  void ack_rem_rnti(uint16_t rnti);
  void ack_clear(uint32_t sf_idx); 
  void ack_set_pending(uint32_t sf_idx, uint16_t rnti, uint32_t n_pdcch);
  bool ack_is_pending(uint32_t sf_idx, uint16_t rnti, uint32_t *last_n_pdcch = NULL);
        
private:
  std::vector<pthread_mutex_t>    tx_mutex; 
  bool            is_first_tx;
  bool            is_first_of_burst; 

  uint32_t        nof_workers;
  uint32_t        nof_mutex;
  uint32_t        max_mutex;
  
};

} // namespace srsenb

#endif // UEPHY_H
