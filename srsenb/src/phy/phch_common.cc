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

#include "srsenb/hdr/phy/txrx.h"

#include <assert.h>

#define Error(fmt, ...)   if (SRSLTE_DEBUG_ENABLED) log_h->error(fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) if (SRSLTE_DEBUG_ENABLED) log_h->warning(fmt, ##__VA_ARGS__)
#define Info(fmt, ...)    if (SRSLTE_DEBUG_ENABLED) log_h->info(fmt, ##__VA_ARGS__)
#define Debug(fmt, ...)   if (SRSLTE_DEBUG_ENABLED) log_h->debug(fmt, ##__VA_ARGS__)

using namespace std; 


namespace srsenb {

void phch_common::set_nof_mutex(uint32_t nof_mutex_) {
  nof_mutex = nof_mutex_; 
  assert(nof_mutex <= max_mutex);
}

void phch_common::reset() {
  bzero(ul_grants, sizeof(mac_interface_phy::ul_sched_t)*TTIMOD_SZ);
  bzero(dl_grants, sizeof(mac_interface_phy::dl_sched_t)*TTIMOD_SZ);
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

void phch_common::worker_end(uint32_t tx_mutex_cnt, cf_t* buffer[SRSLTE_MAX_PORTS], uint32_t nof_samples, srslte_timestamp_t tx_time)
{

  // Wait previous TTIs to be transmitted 
  if (is_first_tx) {
    is_first_tx = false; 
  } else {
    pthread_mutex_lock(&tx_mutex[tx_mutex_cnt%nof_mutex]);
  }

  radio->set_tti(tx_mutex_cnt);
  radio->tx((void **) buffer, nof_samples, tx_time);
  
  // Trigger next transmission 
  pthread_mutex_unlock(&tx_mutex[(tx_mutex_cnt+1)%nof_mutex]);

  // Trigger MAC clock
  mac->tti_clock();
}

void phch_common::ue_db_clear(uint32_t sf_idx)
{
  for(std::map<uint16_t,common_ue>::iterator iter=common_ue_db.begin(); iter!=common_ue_db.end(); ++iter) {
    pending_ack_t *p = &((common_ue*)&iter->second)->pending_ack;
    for (uint32_t tb_idx = 0; tb_idx < SRSLTE_MAX_TB; tb_idx++) {
      p->is_pending[sf_idx][tb_idx] = false;
    }
  }
}

void phch_common::ue_db_add_rnti(uint16_t rnti)
{
  for (int sf_idx=0;sf_idx<TTIMOD_SZ;sf_idx++) {
    for (uint32_t tb_idx = 0; tb_idx < SRSLTE_MAX_TB; tb_idx++) {
      common_ue_db[rnti].pending_ack.is_pending[sf_idx][tb_idx] = false;
    }
  }
}

void phch_common::ue_db_rem_rnti(uint16_t rnti)
{
  common_ue_db.erase(rnti);
}

void phch_common::ue_db_set_ack_pending(uint32_t sf_idx, uint16_t rnti, uint32_t tb_idx, uint32_t last_n_pdcch)
{
  if (common_ue_db.count(rnti)) {
    common_ue_db[rnti].pending_ack.is_pending[sf_idx][tb_idx] = true;
    common_ue_db[rnti].pending_ack.n_pdcch[sf_idx]            = (uint16_t) last_n_pdcch;
  }
}

bool phch_common::ue_db_is_ack_pending(uint32_t sf_idx, uint16_t rnti, uint32_t tb_idx, uint32_t *last_n_pdcch) {
  if (common_ue_db.count(rnti)) {
    bool ret = common_ue_db[rnti].pending_ack.is_pending[sf_idx][tb_idx];
    common_ue_db[rnti].pending_ack.is_pending[sf_idx][tb_idx] = false;

    if (ret && last_n_pdcch) {
      *last_n_pdcch = common_ue_db[rnti].pending_ack.n_pdcch[sf_idx];
    }
    return ret; 
  } else {
    return false;
  }
}

void phch_common::ue_db_set_ri(uint16_t rnti, uint8_t ri) {
  if (common_ue_db.count(rnti)) {
    common_ue_db[rnti].ri = ri;
  }
}
uint8_t phch_common::ue_db_get_ri(uint16_t rnti) {
  uint8_t ret = 0;
  if (common_ue_db.count(rnti)) {
    ret = common_ue_db[rnti].ri;
  }
  return ret;
}
void phch_common::ue_db_set_last_ul_mod(uint16_t rnti, uint32_t tti, srslte_mod_t mcs) {
  if (!common_ue_db.count(rnti)) {
    ue_db_add_rnti(rnti);
  }
  common_ue_db[rnti].last_ul_mod[TTI_RX(tti)%(2*HARQ_DELAY_MS)] = mcs;
}
srslte_mod_t phch_common::ue_db_get_last_ul_mod(uint16_t rnti, uint32_t tti) {
  srslte_mod_t ret = SRSLTE_MOD_BPSK;
  if (common_ue_db.count(rnti)) {
    ret = common_ue_db[rnti].last_ul_mod[TTI_RX(tti)%(2*HARQ_DELAY_MS)];
  }
  return ret;
}
void phch_common::ue_db_set_last_ul_tbs(uint16_t rnti, uint32_t tti, int tbs) {
  if (!common_ue_db.count(rnti)) {
    ue_db_add_rnti(rnti);
  }
  common_ue_db[rnti].last_ul_tbs[TTI_RX(tti)%(2*HARQ_DELAY_MS)] = tbs;
}
int phch_common::ue_db_get_last_ul_tbs(uint16_t rnti, uint32_t tti) {
  int ret = -1;
  if (common_ue_db.count(rnti)) {
    ret = common_ue_db[rnti].last_ul_tbs[TTI_RX(tti)%(2*HARQ_DELAY_MS)];
  }
  return ret;
}

}
