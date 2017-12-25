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

#include "phy/txrx.h"
#include "phy/phch_worker.h"

#define Error(fmt, ...)   if (SRSLTE_DEBUG_ENABLED) log_h->error_line(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) if (SRSLTE_DEBUG_ENABLED) log_h->warning_line(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define Info(fmt, ...)    if (SRSLTE_DEBUG_ENABLED) log_h->info_line(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define Debug(fmt, ...)   if (SRSLTE_DEBUG_ENABLED) log_h->debug_line(__FILE__, __LINE__, fmt, ##__VA_ARGS__)

using namespace std; 

extern uint32_t       g_tti;
extern struct timeval g_tv_next;

namespace srsenb {

txrx::txrx()
{
  X_TRACE("TXRX:BEGIN");
  running = false;   
  radio_h = NULL; 
  log_h   = NULL; 
  workers_pool = NULL; 
  worker_com   = NULL; 
}

bool txrx::init(srslte::radio* radio_h_, srslte::thread_pool* workers_pool_, phch_common* worker_com_, prach_worker *prach_, srslte::log* log_h_, uint32_t prio_)
{
  X_TRACE("TXRX:BEGIN");
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
  X_TRACE("TXRX:BEGIN");
  running = false; 
  wait_thread_finish();
}

void txrx::run_thread()
{
  phch_worker *worker = NULL;
  cf_t *buffer = NULL;
  srslte_timestamp_t rx_time, tx_time; 
  uint32_t sf_len = SRSLTE_SF_LEN_PRB(worker_com->cell.nof_prb);
  
  float samp_rate = srslte_sampling_freq_hz(worker_com->cell.nof_prb);
  if (30720%((int) samp_rate/1000) == 0) {
    radio_h->set_master_clock_rate(30.72e6);        
  } else {
    radio_h->set_master_clock_rate(23.04e6);        
  }
  
  log_h->console("Setting Sampling frequency %.2f MHz\n", (float) samp_rate/1000000);

  // Configure radio 
  radio_h->set_rx_srate(samp_rate);
  radio_h->set_tx_srate(samp_rate);  
  
  log_h->info("Starting RX/TX thread nof_prb=%d, sf_len=%d\n",worker_com->cell.nof_prb, sf_len);

  // Start streaming RX samples
  radio_h->start_rx();
  
  // Set TTI so that first TX is at tti=0
  tti = 10235; 
   
  struct timeval tv_in, tv_out, tv_diff, tv_start, tv_step = {0, 1000}, tv_zero = {0, 0};

  threads_print_self();

  gettimeofday(&tv_start, NULL);

  // try to aligin on the top of the second
  usleep(1000000 - tv_start.tv_usec);

  tv_start.tv_sec += 1; 
  tv_start.tv_usec = 0;

  g_tv_next = tv_start;
 
  I_TRACE("tti %u, time_0 %ld:%06ld", tti, tv_start.tv_sec, tv_start.tv_usec);

  printf("\n==== eNodeB started ===\n");
  printf("Type <t> to view trace\n");
  // Main loop
  while (running) {
    timeradd(&g_tv_next, &tv_step, &g_tv_next);

    gettimeofday(&tv_in, NULL);

    g_tti = tti = (tti+1)%10240;        

    I_TRACE("----------------- tti %u, time_in %ld:%06ld ---------------------", 
            tti, tv_in.tv_sec, tv_in.tv_usec);

    worker = (phch_worker*) workers_pool->wait_worker(tti);
    if (worker) {          
      buffer = worker->get_buffer_rx();
      
      radio_h->rx_now(buffer, sf_len, &rx_time);
                    
      /* Compute TX time: Any transmission happens in TTI+4 thus advance 4 ms the reception time */
      srslte_timestamp_copy(&tx_time, &rx_time);
      srslte_timestamp_add(&tx_time, 0, 4e-3);
      
      worker->set_time(tti, tx_mutex_cnt, tx_time);
      tx_mutex_cnt = (tx_mutex_cnt+1)%nof_tx_mutex;
      
      // Trigger phy worker execution
      workers_pool->start_worker(worker);       

      // Trigger prach worker execution 
      prach->new_tti(tti, buffer);
      
      gettimeofday(&tv_out, NULL);

      timersub(&g_tv_next, &tv_out, &tv_diff);

      I_TRACE("tti %u, time_out %ld:%06ld, delta_t_next %ld:%06ld",
              tti,
              tv_out.tv_sec,
              tv_out.tv_usec,
              tv_diff.tv_sec,
              tv_diff.tv_usec);

      if(timercmp(&tv_diff, &tv_zero, >))
        {
          select(0, NULL, NULL, NULL, &tv_diff);
        }
    } else {
      // wait_worker() only returns NULL if it's being closed. Quit now to avoid unnecessary loops here
      running = false; 
    }
  }
}


  
}
