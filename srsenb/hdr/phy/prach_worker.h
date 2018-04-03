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

#ifndef SRSENB_PRACH_WORKER_H
#define SRSENB_PRACH_WORKER_H

#include "srslte/interfaces/enb_interfaces.h"
#include "srslte/common/log.h"
#include "srslte/common/threads.h"

namespace srsenb {
  
class prach_worker : thread
{
public:
  prach_worker() : initiated(false), prach_nof_det(0), max_prach_offset_us(0), pending_tti(0), processed_tti(0),
                   running(false), nof_sf(0), sf_cnt(0) {
    log_h = NULL;
    mac = NULL;
    signal_buffer_rx = NULL;
    bzero(&prach, sizeof(srslte_prach_t));
    bzero(&prach_indices, sizeof(prach_indices));
    bzero(&prach_offsets, sizeof(prach_offsets));
    bzero(&prach_p2avg, sizeof(prach_p2avg));
    bzero(&cell, sizeof(cell));
    bzero(&prach_cfg, sizeof(prach_cfg));
    bzero(&mutex, sizeof(mutex));
    bzero(&cvar, sizeof(cvar));
  }
  
  int  init(srslte_cell_t *cell, srslte_prach_cfg_t *prach_cfg, mac_interface_phy *mac, srslte::log *log_h, int priority);
  int  new_tti(uint32_t tti, cf_t *buffer);
  void set_max_prach_offset_us(float delay_us);
  void stop();
  
private:
  void run_thread();
  int run_tti(uint32_t tti); 
  
  uint32_t prach_nof_det; 
  uint32_t prach_indices[165]; 
  float    prach_offsets[165]; 
  float    prach_p2avg[165];
 
  srslte_cell_t cell; 
  srslte_prach_cfg_t prach_cfg;
  srslte_prach_t  prach;

  pthread_mutex_t mutex;
  pthread_cond_t  cvar;

  cf_t *signal_buffer_rx;
  
  srslte::log* log_h;
  mac_interface_phy *mac;
  float max_prach_offset_us;
  bool initiated;
  uint32_t pending_tti;
  int processed_tti;
  bool running;
  uint32_t nof_sf;
  uint32_t sf_cnt;
};
}
#endif // SRSENB_PRACH_WORKER_H
