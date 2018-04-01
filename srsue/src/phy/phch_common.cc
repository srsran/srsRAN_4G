/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
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

#include <assert.h>
#include <string.h>
#include "srslte/srslte.h"
#include "srsue/hdr/phy/phch_common.h"

#define Error(fmt, ...)   if (SRSLTE_DEBUG_ENABLED) log_h->error(fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) if (SRSLTE_DEBUG_ENABLED) log_h->warning(fmt, ##__VA_ARGS__)
#define Info(fmt, ...)    if (SRSLTE_DEBUG_ENABLED) log_h->info(fmt, ##__VA_ARGS__)
#define Debug(fmt, ...)   if (SRSLTE_DEBUG_ENABLED) log_h->debug(fmt, ##__VA_ARGS__)

namespace srsue {

cf_t zeros[50000];

phch_common::phch_common(uint32_t max_mutex_) : tx_mutex(max_mutex_)
{
  config    = NULL; 
  args      = NULL; 
  log_h     = NULL; 
  radio_h   = NULL; 
  mac       = NULL; 
  max_mutex = max_mutex_;
  nof_mutex = 0;
  rx_gain_offset = 0;
  last_ri = 0;
  last_pmi = 0;

  bzero(&dl_metrics, sizeof(dl_metrics_t));
  dl_metrics_read = true;
  dl_metrics_count = 0;
  bzero(&ul_metrics, sizeof(ul_metrics_t));
  ul_metrics_read = true;
  ul_metrics_count = 0;
  bzero(&sync_metrics, sizeof(sync_metrics_t));
  sync_metrics_read = true;
  sync_metrics_count = 0;

  bzero(zeros, 50000*sizeof(cf_t));

  // FIXME: This is an ugly fix to avoid the TX filters to empty
  /*
  for (int i=0;i<50000;i++) {
    zeros[i] = 0.01*cexpf(((float) i/50000)*0.1*_Complex_I);
  }*/

  reset();

}
  
void phch_common::init(phy_interface_rrc::phy_cfg_t *_config, phy_args_t *_args, srslte::log *_log, srslte::radio *_radio, rrc_interface_phy *_rrc, mac_interface_phy *_mac)
{
  log_h     = _log; 
  radio_h   = _radio;
  rrc       = _rrc;
  mac       = _mac; 
  config    = _config;     
  args      = _args; 
  is_first_tx = true; 
  sr_last_tx_tti = -1;
  
  for (uint32_t i=0;i<nof_mutex;i++) {
    pthread_mutex_init(&tx_mutex[i], NULL);
  }
}

void phch_common::set_nof_mutex(uint32_t nof_mutex_) {
  nof_mutex = nof_mutex_; 
  assert(nof_mutex <= max_mutex);
}

bool phch_common::ul_rnti_active(uint32_t tti) {
  if ((((int)tti >= ul_rnti_start && ul_rnti_start >= 0) || ul_rnti_start < 0) &&
      (((int)tti <  ul_rnti_end   && ul_rnti_end   >= 0) || ul_rnti_end   < 0))
  {
    return true; 
  } else {
    return false; 
  }
}

bool phch_common::dl_rnti_active(uint32_t tti) {
  Debug("tti=%d, dl_rnti_start=%d, dl_rnti_end=%d, dl_rnti=0x%x\n", tti, dl_rnti_start, dl_rnti_end, dl_rnti);
  if ((((int)tti >= dl_rnti_start && dl_rnti_start >= 0)  || dl_rnti_start < 0) &&
      (((int)tti <  dl_rnti_end   && dl_rnti_end   >= 0)  || dl_rnti_end   < 0))
  {
    bool ret = true; 
    // FIXME: This scheduling decision belongs to RRC
    if (dl_rnti_type == SRSLTE_RNTI_SI) {
      if (dl_rnti_end - dl_rnti_start > 1) { // This is not a SIB1        
        if ((tti/10)%2 == 0 && (tti%10) == 5) { // Skip subframe #5 for which SFN mod 2 = 0
          ret = false; 
        }
      }
    }
    return ret; 
  } else {
    return false; 
  }
}

srslte::radio* phch_common::get_radio()
{
  return radio_h;
}

// Unpack RAR grant as defined in Section 6.2 of 36.213 
void phch_common::set_rar_grant(uint32_t tti, uint8_t grant_payload[SRSLTE_RAR_GRANT_LEN])
{
  srslte_dci_rar_grant_unpack(&rar_grant, grant_payload);
  rar_grant_pending = true;
  if (MSG3_DELAY_MS < 0) {
    fprintf(stderr, "Error MSG3_DELAY_MS can't be negative\n");
  }
  if (rar_grant.ul_delay) {
    rar_grant_tti     = (tti + MSG3_DELAY_MS + 1) % 10240;
  } else {
    rar_grant_tti     = (tti + MSG3_DELAY_MS) % 10240;
  }
}

bool phch_common::get_pending_rar(uint32_t tti, srslte_dci_rar_grant_t *rar_grant_)
{
  if (rar_grant_pending && tti >= rar_grant_tti) {
    if (rar_grant_) {
      rar_grant_pending = false; 
      memcpy(rar_grant_, &rar_grant, sizeof(srslte_dci_rar_grant_t));
    }
    return true; 
  }
  return false; 
}

/* Common variables used by all phy workers */
uint16_t phch_common::get_ul_rnti(uint32_t tti) {
  if (ul_rnti_active(tti)) {
    return ul_rnti; 
  } else {
    return 0; 
  }
}
srslte_rnti_type_t phch_common::get_ul_rnti_type() {
  return ul_rnti_type; 
}
void phch_common::set_ul_rnti(srslte_rnti_type_t type, uint16_t rnti_value, int tti_start, int tti_end) {
  ul_rnti = rnti_value;
  ul_rnti_type = type;
  ul_rnti_start = tti_start;
  ul_rnti_end   = tti_end;
}
uint16_t phch_common::get_dl_rnti(uint32_t tti) {
  if (dl_rnti_active(tti)) {
    return dl_rnti; 
  } else {
    return 0; 
  }
}
srslte_rnti_type_t phch_common::get_dl_rnti_type() {
  return dl_rnti_type; 
}
void phch_common::set_dl_rnti(srslte_rnti_type_t type, uint16_t rnti_value, int tti_start, int tti_end) {
  dl_rnti       = rnti_value;
  dl_rnti_type  = type;
  dl_rnti_start = tti_start;
  dl_rnti_end   = tti_end;
  if (log_h) {
    Debug("Set DL rnti: start=%d, end=%d, value=0x%x\n", tti_start, tti_end, rnti_value);
  }
}

void phch_common::reset_pending_ack(uint32_t tti) {
  pending_ack[TTIMOD(tti)].enabled = false;
}

void phch_common::set_pending_ack(uint32_t tti, uint32_t I_lowest, uint32_t n_dmrs) {
  pending_ack[TTIMOD(tti)].enabled  = true;
  pending_ack[TTIMOD(tti)].I_lowest = I_lowest;
  pending_ack[TTIMOD(tti)].n_dmrs = n_dmrs;
  Debug("Set pending ACK for tti=%d I_lowest=%d, n_dmrs=%d\n", tti, I_lowest, n_dmrs);
}

bool phch_common::get_pending_ack(uint32_t tti) {
  return get_pending_ack(tti, NULL, NULL); 
}

bool phch_common::get_pending_ack(uint32_t tti, uint32_t *I_lowest, uint32_t *n_dmrs) {
  if (I_lowest) {
    *I_lowest = pending_ack[TTIMOD(tti)].I_lowest;
  }
  if (n_dmrs) {
    *n_dmrs = pending_ack[TTIMOD(tti)].n_dmrs;
  }
  return pending_ack[TTIMOD(tti)].enabled;
}

bool phch_common::is_any_pending_ack() {
  for (int i=0;i<TTIMOD_SZ;i++) {
    if (pending_ack[i].enabled) {
      return true;
    }
  }
  return false;
}

/* The transmisison of UL subframes must be in sequence. Each worker uses this function to indicate
 * that all processing is done and data is ready for transmission or there is no transmission at all (tx_enable). 
 * In that case, the end of burst message will be send to the radio 
 */
void phch_common::worker_end(uint32_t tti, bool tx_enable, 
                                   cf_t *buffer, uint32_t nof_samples, 
                                   srslte_timestamp_t tx_time) 
{

  // Wait previous TTIs to be transmitted 
  if (is_first_tx) {
    is_first_tx = false; 
  } else {
    pthread_mutex_lock(&tx_mutex[tti%nof_mutex]);
  }

  radio_h->set_tti(tti); 
  if (tx_enable) {
    radio_h->tx_single(buffer, nof_samples, tx_time);
    is_first_of_burst = false; 
  } else {
    if (TX_MODE_CONTINUOUS) {
      if (!is_first_of_burst) {
        radio_h->tx_single(zeros, nof_samples, tx_time);
      }
    } else {
      if (!is_first_of_burst) {
        radio_h->tx_end();
        is_first_of_burst = true;   
      }
    }
  }
  // Trigger next transmission 
  pthread_mutex_unlock(&tx_mutex[(tti+1)%nof_mutex]);
}    


void phch_common::set_cell(const srslte_cell_t &c) {
  cell = c;
}

uint32_t phch_common::get_nof_prb() {
  return cell.nof_prb;
}

void phch_common::set_dl_metrics(const dl_metrics_t &m) {
  if(dl_metrics_read) {
    dl_metrics       = m;
    dl_metrics_count = 1;
    dl_metrics_read  = false;
  } else {
    dl_metrics_count++;
    dl_metrics.mcs  = dl_metrics.mcs + (m.mcs - dl_metrics.mcs)/dl_metrics_count;
    dl_metrics.n    = dl_metrics.n + (m.n - dl_metrics.n)/dl_metrics_count;
    dl_metrics.rsrp = dl_metrics.rsrp + (m.rsrp - dl_metrics.rsrp)/dl_metrics_count;
    dl_metrics.rsrq = dl_metrics.rsrq + (m.rsrq - dl_metrics.rsrq)/dl_metrics_count;
    dl_metrics.rssi = dl_metrics.rssi + (m.rssi - dl_metrics.rssi)/dl_metrics_count;
    dl_metrics.sinr = dl_metrics.sinr + (m.sinr - dl_metrics.sinr)/dl_metrics_count;
    dl_metrics.pathloss = dl_metrics.pathloss + (m.pathloss - dl_metrics.pathloss)/dl_metrics_count;
    dl_metrics.turbo_iters = dl_metrics.turbo_iters + (m.turbo_iters - dl_metrics.turbo_iters)/dl_metrics_count;
  }
}

void phch_common::get_dl_metrics(dl_metrics_t &m) {
  m = dl_metrics;
  dl_metrics_read = true;
}

void phch_common::set_ul_metrics(const ul_metrics_t &m) {
  if(ul_metrics_read) {
    ul_metrics       = m;
    ul_metrics_count = 1;
    ul_metrics_read  = false;
  } else {
    ul_metrics_count++;
    ul_metrics.mcs   = ul_metrics.mcs + (m.mcs - ul_metrics.mcs)/ul_metrics_count;
    ul_metrics.power = ul_metrics.power + (m.power - ul_metrics.power)/ul_metrics_count;
  }
}

void phch_common::get_ul_metrics(ul_metrics_t &m) {
  m = ul_metrics;
  ul_metrics_read = true;
}

void phch_common::set_sync_metrics(const sync_metrics_t &m) {

  if(sync_metrics_read) {
    sync_metrics = m;
    sync_metrics_count = 1;
    sync_metrics_read  = false;
  } else {
    sync_metrics_count++;
    sync_metrics.cfo = sync_metrics.cfo + (m.cfo - sync_metrics.cfo)/sync_metrics_count;
    sync_metrics.sfo = sync_metrics.sfo + (m.sfo - sync_metrics.sfo)/sync_metrics_count;
  }
}

void phch_common::get_sync_metrics(sync_metrics_t &m) {
  m = sync_metrics;
  sync_metrics_read = true;
}

void phch_common::reset() {
  sr_enabled        = false;
  is_first_of_burst = true;
  is_first_tx       = true;
  rar_grant_pending = false;
  pathloss = 0;
  cur_pathloss = 0;
  cur_pusch_power = 0;
  p0_preamble = 0;
  cur_radio_power = 0;
  sr_last_tx_tti = -1;
  cur_pusch_power = 0;
  avg_rsrp = 0;
  avg_rsrp_dbm = 0;
  avg_rsrq_db = 0;

  pcell_meas_enabled  = false;
  pcell_report_period = 20;

  bzero(pending_ack, sizeof(pending_ack_t)*TTIMOD_SZ);

}

void phch_common::reset_ul()
{
  /*
  is_first_tx = true;
  is_first_of_burst = true;

  for (uint32_t i=0;i<nof_mutex;i++) {
    pthread_mutex_trylock(&tx_mutex[i]);
    pthread_mutex_unlock(&tx_mutex[i]);
  }
  radio_h->tx_end();
   */
}

}
