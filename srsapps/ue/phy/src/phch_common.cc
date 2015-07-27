/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
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



#include <string.h>
#include "srslte/srslte.h"
#include "srsapps/ue/phy/phch_common.h"

namespace srslte {                                                                                                                                                                                                                                                                                                                                                                                                                                                     
namespace ue {

phch_common::phch_common()
{
  params_db = NULL; 
  log_h     = NULL; 
  radio_h   = NULL; 
  mac       = NULL; 
  sr_enabled        = false; 
  is_first_of_burst = true; 
  is_first_tx       = true; 
  rar_grant_pending = false; 
}
  
void phch_common::init(phy_params *_params, log *_log, radio *_radio, mac_interface_phy *_mac) 
{
  params_db = _params;
  log_h     = _log; 
  radio_h   = _radio; 
  mac       = _mac; 
  is_first_tx = true; 
}

bool phch_common::ul_rnti_active(uint32_t tti) {
  if ((tti >= ul_rnti_start && ul_rnti_start >= 0 || ul_rnti_start < 0) && 
      (tti <  ul_rnti_end   && ul_rnti_end   >= 0 || ul_rnti_end   < 0)) 
  {
    return true; 
  } else {
    return false; 
  }
}

bool phch_common::dl_rnti_active(uint32_t tti) {
  if (((tti >= dl_rnti_start && dl_rnti_start >= 0)  || dl_rnti_start < 0) && 
      ((tti <  dl_rnti_end   && dl_rnti_end   >= 0)  || dl_rnti_end   < 0))
  {
    return true; 
  } else {
    return false; 
  }
}

// Unpack RAR grant as defined in Section 6.2 of 36.213 
void phch_common::set_rar_grant(uint32_t tti, uint8_t grant_payload[SRSLTE_RAR_GRANT_LEN])
{
  srslte_dci_rar_grant_unpack(&rar_grant, grant_payload);
  rar_grant_pending = true; 
  Info("Setting RAR grant: \n");
  srslte_dci_rar_grant_fprint(stdout, &rar_grant);
  // PUSCH is at n+6 or n+7 and phch_worker assumes default delay of 4 ttis
  if (rar_grant.ul_delay) {
    rar_grant_tti     = (tti + 3) % 10240; 
  } else {
    rar_grant_tti     = (tti + 2) % 10240; 
  }
}

bool phch_common::get_pending_rar(uint32_t tti, srslte_dci_rar_grant_t *rar_grant_)
{
  if (rar_grant_pending && (tti >= rar_grant_tti || (tti < 10 && rar_grant_pending > 10235))) {
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
  if (rnti_value) {
    Debug("Set DL rnti: start=%d, end=%d, value=0x%x\n", tti_start, tti_end, rnti_value);
  }
}

void phch_common::reset_pending_ack(uint32_t tti) {
  pending_ack[tti%10].enabled = false; 
}

void phch_common::set_pending_ack(uint32_t tti, uint32_t I_lowest, uint32_t n_dmrs) {
  pending_ack[tti%10].enabled  = true; 
  pending_ack[tti%10].I_lowest = I_lowest;       
  pending_ack[tti%10].n_dmrs = n_dmrs;            
  Debug("Set pending ACK for tti=%d I_lowest=%d, n_dmrs=%d\n", tti, I_lowest, n_dmrs);
}

bool phch_common::get_pending_ack(uint32_t tti) {
  return get_pending_ack(tti, NULL, NULL); 
}

bool phch_common::get_pending_ack(uint32_t tti, uint32_t *I_lowest, uint32_t *n_dmrs) {
  if (I_lowest) {
    *I_lowest = pending_ack[tti%10].I_lowest;
  }
  if (n_dmrs) {
    *n_dmrs = pending_ack[tti%10].n_dmrs;
  }
  return pending_ack[tti%10].enabled;
}

/* The transmisison of UL subframes must be in sequence. Each worker uses this function to indicate
 * that all processing is done and data is ready for transmission or there is no transmission at all (tx_enable). 
 * In that case, the end of burst message will be send to the radio 
 */
void phch_common::worker_end(uint32_t tti, bool tx_enable, 
                                   cf_t *buffer, uint32_t nof_samples, 
                                   srslte_timestamp_t tx_time) 
{
  pthread_mutex_lock(&tx_mutex);
  
  // Wait previous TTIs to be transmitted 
  if (is_first_tx) {
    tx_tti_cnt = tti; 
    is_first_tx = false; 
  } else {
    while(tti != tx_tti_cnt) {
      pthread_cond_wait(&tx_cvar, &tx_mutex);
    }
  }
  if (tx_enable) {
    radio_h->tx(buffer, nof_samples, tx_time);
    is_first_of_burst = false; 
  } else if (!is_first_of_burst) {
    radio_h->tx_end();
    is_first_of_burst = true;   
  } /* else do nothing, just update tti counter */
  
  tx_tti_cnt = (tx_tti_cnt + 1) % 10240;
  
  pthread_cond_signal(&tx_cvar);
  pthread_mutex_unlock(&tx_mutex);
}    

}
}