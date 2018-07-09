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

#include <unistd.h>

#include "srslte/common/threads.h"
#include "srslte/common/log.h"

#include "srsenb/hdr/phy/txrx.h"
#include "srsenb/hdr/phy/phch_worker.h"

#define Error(fmt, ...)   if (SRSLTE_DEBUG_ENABLED) log_h->error(fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) if (SRSLTE_DEBUG_ENABLED) log_h->warning(fmt, ##__VA_ARGS__)
#define Info(fmt, ...)    if (SRSLTE_DEBUG_ENABLED) log_h->info(fmt, ##__VA_ARGS__)
#define Debug(fmt, ...)   if (SRSLTE_DEBUG_ENABLED) log_h->debug(fmt, ##__VA_ARGS__)

using namespace std; 


namespace srsenb {

txrx::txrx() : tx_mutex_cnt(0), nof_tx_mutex(0), tti(0) {
  running = false;   
  radio_h = NULL; 
  log_h   = NULL; 
  workers_pool = NULL; 
  worker_com   = NULL;
  prach = NULL;
}

bool txrx::init(srslte::radio* radio_h_, srslte::thread_pool* workers_pool_, phch_common* worker_com_, prach_worker *prach_, srslte::log* log_h_, uint32_t prio_)
{
  radio_h      = radio_h_;
  log_h        = log_h_;     
  workers_pool = workers_pool_;
  worker_com   = worker_com_;
  prach        = prach_; 
  tx_mutex_cnt = 0; 
  running      = true; 
  
  nof_tx_mutex = MUTEX_X_WORKER*workers_pool->get_nof_workers();
  worker_com->set_nof_mutex(nof_tx_mutex);
    
  start(prio_);
  return true; 
}

void txrx::stop()
{
  running = false; 
  wait_thread_finish();
}

void txrx::run_thread()
{
  phch_worker *worker = NULL;
  cf_t *buffer[SRSLTE_MAX_PORTS] = {NULL};
  srslte_timestamp_t rx_time, tx_time; 
  uint32_t sf_len = SRSLTE_SF_LEN_PRB(worker_com->cell.nof_prb);
  
  float samp_rate = srslte_sampling_freq_hz(worker_com->cell.nof_prb);
#if 0
  if (30720%((int) samp_rate/1000) == 0) {
    radio_h->set_master_clock_rate(30.72e6);        
  } else {
    radio_h->set_master_clock_rate(23.04e6);        
  }
#else
  if (samp_rate < 10e6) {
    radio_h->set_master_clock_rate(4 * samp_rate);
  } else {
    radio_h->set_master_clock_rate(samp_rate);
  }
#endif
  
  log_h->console("Setting Sampling frequency %.2f MHz\n", (float) samp_rate/1000000);

  // Configure radio 
  radio_h->set_rx_srate(samp_rate);
  radio_h->set_tx_srate(samp_rate);  
  
  log_h->info("Starting RX/TX thread nof_prb=%d, sf_len=%d\n",worker_com->cell.nof_prb, sf_len);


  // Set TTI so that first TX is at tti=0
  tti = 10235; 
    
  printf("\n==== eNodeB started ===\n");
  printf("Type <t> to view trace\n");
  // Main loop
  while (running) {
    tti = (tti+1)%10240;        
    worker = (phch_worker*) workers_pool->wait_worker(tti);
    if (worker) {
      for (int p = 0; p < SRSLTE_MAX_PORTS; p++){
        buffer[p] = worker->get_buffer_rx(p);
      }
      
      radio_h->rx_now((void **) buffer, sf_len, &rx_time);
                    
      /* Compute TX time: Any transmission happens in TTI+4 thus advance 4 ms the reception time */
      srslte_timestamp_copy(&tx_time, &rx_time);
      srslte_timestamp_add(&tx_time, 0, HARQ_DELAY_MS*1e-3);
      
      Debug("Settting TTI=%d, tx_mutex=%d, tx_time=%ld:%f to worker %d\n", 
            tti, tx_mutex_cnt, 
            tx_time.full_secs, tx_time.frac_secs,
            worker->get_id());
      
      worker->set_time(tti, tx_mutex_cnt, tx_time);
      tx_mutex_cnt = (tx_mutex_cnt+1)%nof_tx_mutex;
      
      // Trigger phy worker execution
      workers_pool->start_worker(worker);       

      // Trigger prach worker execution 
      prach->new_tti(tti, buffer[0]);
      
    } else {
      // wait_worker() only returns NULL if it's being closed. Quit now to avoid unnecessary loops here
      running = false; 
    }
  }
}


  
}
