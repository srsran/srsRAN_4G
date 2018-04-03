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

#include "srslte/srslte.h"
#include "srsenb/hdr/phy/prach_worker.h"

namespace srsenb {

int prach_worker::init(srslte_cell_t *cell_, srslte_prach_cfg_t *prach_cfg_, mac_interface_phy* mac_, srslte::log* log_h_, int priority)
{
  log_h = log_h_; 
  mac   = mac_; 
  memcpy(&prach_cfg, prach_cfg_, sizeof(srslte_prach_cfg_t));
  memcpy(&cell, cell_, sizeof(srslte_cell_t));
  
  max_prach_offset_us = 50; 
  
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&cvar, NULL);

  if (srslte_prach_init_cfg(&prach, &prach_cfg, cell.nof_prb)) {
    fprintf(stderr, "Error initiating PRACH\n");
    return -1; 
  }

  srslte_prach_set_detect_factor(&prach, 60);    

  nof_sf = (uint32_t) ceilf(prach.T_tot*1000); 

  signal_buffer_rx = (cf_t*) srslte_vec_malloc(sizeof(cf_t)*nof_sf*SRSLTE_SF_LEN_PRB(cell.nof_prb));
  if (!signal_buffer_rx) {
    perror("malloc");
    return -1;
  }
  
  start(priority);
  initiated = true;

  sf_cnt = 0;
  pending_tti   = 0; 
  processed_tti = 0; 
  return 0; 
}

void prach_worker::stop()
{
  srslte_prach_free(&prach);

  if (signal_buffer_rx) {
    free(signal_buffer_rx);
  }
  pthread_mutex_lock(&mutex);
  processed_tti = 99999; 
  running = false; 
  pthread_cond_signal(&cvar);
  pthread_mutex_unlock(&mutex);
  
  wait_thread_finish();
}

void prach_worker::set_max_prach_offset_us(float delay_us)
{
  max_prach_offset_us = delay_us; 
}

int prach_worker::new_tti(uint32_t tti_rx, cf_t* buffer_rx)
{
  // Save buffer only if it's a PRACH TTI
  if (srslte_prach_tti_opportunity(&prach, tti_rx, -1) || sf_cnt) {
    memcpy(&signal_buffer_rx[sf_cnt*SRSLTE_SF_LEN_PRB(cell.nof_prb)], buffer_rx, sizeof(cf_t)*SRSLTE_SF_LEN_PRB(cell.nof_prb));
    sf_cnt++;
    if (sf_cnt == nof_sf) {
      sf_cnt = 0; 
      if ((int) pending_tti != processed_tti) {
        log_h->warning("PRACH thread did not finish processing TTI=%d\n", pending_tti);
      }
      pthread_mutex_lock(&mutex);
      if (tti_rx+1 > nof_sf) {
        pending_tti = tti_rx+1-nof_sf;       
      } else {
        pending_tti = 10240+(tti_rx+1-nof_sf);
      }
      pthread_cond_signal(&cvar);
      pthread_mutex_unlock(&mutex);
    }
  }
  return 0; 
}


int prach_worker::run_tti(uint32_t tti_rx)
{
  if (srslte_prach_tti_opportunity(&prach, tti_rx, -1)) 
  {
    // Detect possible PRACHs
    if (srslte_prach_detect_offset(&prach,
                                   prach_cfg.freq_offset,
                                   &signal_buffer_rx[prach.N_cp],
                                   nof_sf*SRSLTE_SF_LEN_PRB(cell.nof_prb)-prach.N_cp,
                                   prach_indices, 
                                   prach_offsets,
                                   prach_p2avg,
                                   &prach_nof_det)) 
    {
      log_h->error("Error detecting PRACH\n");
      return SRSLTE_ERROR; 
    }
    
    if (prach_nof_det) {
      for (uint32_t i=0;i<prach_nof_det;i++) {
        log_h->info("PRACH: %d/%d, preamble=%d, offset=%.1f us, peak2avg=%.1f, max_offset=%.1f us\n", 
            i, prach_nof_det, prach_indices[i], prach_offsets[i]*1e6, prach_p2avg[i], max_prach_offset_us);
        
        if (prach_offsets[i]*1e6 < max_prach_offset_us) {
          mac->rach_detected(tti_rx, prach_indices[i], (uint32_t) (prach_offsets[i]*1e6));            
        }
      }
    }
  }
  return 0; 
}

void prach_worker::run_thread()
{
  running = true; 
  while(running) {
   pthread_mutex_lock(&mutex);
   while(processed_tti == (int) pending_tti) {
    pthread_cond_wait(&cvar, &mutex);
   }
   pthread_mutex_unlock(&mutex);
   log_h->debug("Processing pending_tti=%d\n", pending_tti);
   if (running) {
    if (run_tti(pending_tti)) {
      running = false; 
    }
    processed_tti = pending_tti;
   }
  }
}

  
}
