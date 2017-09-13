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

#include "srslte/common/threads.h"
#include "srslte/common/log.h"

#include "phy/txrx.h"

#include <assert.h>
#include <string.h>

#define Error(fmt, ...)   if (SRSLTE_DEBUG_ENABLED) log_h->error_line(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) if (SRSLTE_DEBUG_ENABLED) log_h->warning_line(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define Info(fmt, ...)    if (SRSLTE_DEBUG_ENABLED) log_h->info_line(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define Debug(fmt, ...)   if (SRSLTE_DEBUG_ENABLED) log_h->debug_line(__FILE__, __LINE__, fmt, ##__VA_ARGS__)

using namespace std; 


namespace srsenb {

void phch_common::set_nof_mutex(uint32_t nof_mutex_) {
  nof_mutex = nof_mutex_; 
  assert(nof_mutex <= max_mutex);
}

void phch_common::reset() {
  bzero(ul_grants, sizeof(mac_interface_phy::ul_sched_t)*10);
  bzero(dl_grants, sizeof(mac_interface_phy::dl_sched_t)*10);
}

bool phch_common::init(srslte_cell_t *cell_, srslte::radio* radio_h_, mac_interface_phy *mac_)
{
  radio = radio_h_;
  mac   = mac_; 
  memcpy(&cell, cell_, sizeof(srslte_cell_t));

  is_first_of_burst = true; 
  is_first_tx = true; 
  for (uint32_t i=0;i<max_mutex;i++) {
    pthread_mutex_init(&tx_mutex[i], NULL);
  }
  reset(); 
  return true; 
}

void phch_common::stop() {
  for (uint32_t i=0;i<nof_mutex;i++) {
    pthread_mutex_trylock(&tx_mutex[i]);
    pthread_mutex_unlock(&tx_mutex[i]);
  }
}

void phch_common::worker_end(uint32_t tx_mutex_cnt, cf_t* buffer, uint32_t nof_samples, srslte_timestamp_t tx_time)
{

  // Wait previous TTIs to be transmitted 
  if (is_first_tx) {
    is_first_tx = false; 
  } else {
    pthread_mutex_lock(&tx_mutex[tx_mutex_cnt%nof_mutex]);
  }

  radio->set_tti(tx_mutex_cnt); 
  radio->tx(buffer, nof_samples, tx_time);
  
  // Trigger next transmission 
  pthread_mutex_unlock(&tx_mutex[(tx_mutex_cnt+1)%nof_mutex]);

  // Trigger MAC clock
  mac->tti_clock();
}

void phch_common::ack_clear(uint32_t sf_idx)
{
  for(std::map<uint16_t,pending_ack_t>::iterator iter=pending_ack.begin(); iter!=pending_ack.end(); ++iter) {
    pending_ack_t *p = (pending_ack_t*) &iter->second;
    p->is_pending[sf_idx] = false;     
  }
}

void phch_common::ack_add_rnti(uint16_t rnti)
{
  for (int sf_idx=0;sf_idx<10;sf_idx++) {
    pending_ack[rnti].is_pending[sf_idx] = false; 
  }
}

void phch_common::ack_rem_rnti(uint16_t rnti)
{
  pending_ack.erase(rnti);
}

void phch_common::ack_set_pending(uint32_t sf_idx, uint16_t rnti, uint32_t last_n_pdcch)
{
  if (pending_ack.count(rnti)) {
    pending_ack[rnti].is_pending[sf_idx] = true; 
    pending_ack[rnti].n_pdcch[sf_idx]    = last_n_pdcch;
  }
}

bool phch_common::ack_is_pending(uint32_t sf_idx, uint16_t rnti, uint32_t *last_n_pdcch)
{
  if (pending_ack.count(rnti)) {
    bool ret = pending_ack[rnti].is_pending[sf_idx];  
    pending_ack[rnti].is_pending[sf_idx] = false; 
    
    if (ret && last_n_pdcch) {
      *last_n_pdcch = pending_ack[rnti].n_pdcch[sf_idx];
    }
    return ret; 
  } else {
    return false; 
  }
}

}
