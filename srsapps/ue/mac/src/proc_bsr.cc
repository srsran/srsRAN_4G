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

#include "srsapps/ue/mac/proc_bsr.h"
#include "srsapps/ue/mac/mac_params.h"
#include "srsapps/ue/mac/mac.h"
#include "srsapps/ue/mac/mux.h"

namespace srslte {
  namespace ue {
    
bsr_proc::bsr_proc()
{
  initiated = false; 
  timer_periodic = false; 
  timer_retx = false; 
  reset_sr = false; 
  for (int i=0;i<MAX_LCID;i++)  {
    lcg[i]        = -1; 
    priorities[i] = -1; 
    last_pending_data[i] = 0; 
  }        
  triggered_bsr_type=NONE; 
}

void bsr_proc::init(log* log_h_, timers *timers_db_, mac_params* params_db_, mac_io *mac_io_h_)
{
  log_h     = log_h_; 
  params_db = params_db_;
  mac_io_h  = mac_io_h_;
  timers_db = timers_db_; 
  initiated = true;
}

void bsr_proc::reset()
{
  triggered_bsr_type = NONE; 
}

/* Process Periodic BSR */
void bsr_proc::timer_expired(uint32_t timer_id) {
  switch(timer_id) {
    case mac::BSR_TIMER_PERIODIC:
      if (triggered_bsr_type == NONE) {
        // Check condition 4 in Sec 5.4.5 
        //triggered_bsr_type = PERIODIC; 
        Info("BSR PERIODIC disabled\n");
      }
      break;
    case mac::BSR_TIMER_RETX:
      // Enable reTx of SR 
      //triggered_bsr_type = REGULAR; 
      Info("BSR reTX disabled\n");
      sr_is_sent = false; 
      break;      
  }
}

// Checks if data is available for a a channel with higher priority than others 
bool bsr_proc::check_highest_channel() {
  int pending_data_lcid = -1; 
  
  for (int i=0;i<mac_io::NOF_UL_LCH && pending_data_lcid == -1;i++) {
    if (lcg[i] >= 0) {
      if (!mac_io_h->get(i+mac_io::MAC_LCH_CCCH_UL)->isempty()) {
        pending_data_lcid = i; 
        for (int j=0;j<mac_io::NOF_UL_LCH;j++) {
          if (!mac_io_h->get(j+mac_io::MAC_LCH_CCCH_UL)->isempty()) {
            if (priorities[j] > priorities[i]) {
              pending_data_lcid = -1; 
            }
          }
        }
      }      
    }
  }
  if (pending_data_lcid >= 0) {
    // If there is new data available for this logical channel 
    uint32_t nbytes = mac_io_h->get(pending_data_lcid+mac_io::MAC_LCH_CCCH_UL)->pending_data()/8;
    if (nbytes > last_pending_data[pending_data_lcid]) 
    {
      if (triggered_bsr_type != REGULAR) {        
        Info("Triggered REGULAR BSR for Max Priority LCID=%d\n", pending_data_lcid);
      }
      triggered_bsr_type = REGULAR; 
      return true; 
    } 
  }
  return false; 
}
    
// Checks if only one logical channel has data avaiable for Tx
bool bsr_proc::check_single_channel() {    
  uint32_t pending_data_lcid = 0; 
  uint32_t nof_nonzero_lcid = 0; 
  
  for (int i=0;i<mac_io::NOF_UL_LCH;i++) {
    if (lcg[i] >= 0) {
      if (!mac_io_h->get(i+mac_io::MAC_LCH_CCCH_UL)->isempty()) {
        pending_data_lcid = i;
        nof_nonzero_lcid++; 
      }
    }
  }
  if (nof_nonzero_lcid == 1) {
    uint32_t nbytes = mac_io_h->get(pending_data_lcid+mac_io::MAC_LCH_CCCH_UL)->pending_data()/8;
    // If there is new data available for this logical channel 
    if (nbytes > last_pending_data[pending_data_lcid]) {
      triggered_bsr_type = REGULAR; 
      Info("Triggered REGULAR BSR for single LCID=%d\n", pending_data_lcid);
      return true; 
    } 
  }

}

void bsr_proc::update_pending_data() {
  for (int i=0;i<mac_io_h->NOF_UL_LCH;i++) {
    last_pending_data[i] = mac_io_h->get(i+mac_io::MAC_LCH_CCCH_UL)->pending_data()/8; 
  }
}

bool bsr_proc::generate_bsr(bsr_t *bsr, uint32_t nof_padding_bytes) {
  bool ret = false; 
  uint32_t nof_lcg=0;
  bzero(bsr, sizeof(bsr_t));    
  for (int i=0;i<mac_io_h->NOF_UL_LCH;i++) {
    if (lcg[i] >= 0) {
      uint32_t n = mac_io_h->get(i+mac_io::MAC_LCH_CCCH_UL)->pending_data()/8;
      bsr->buff_size[lcg[i]] += n;
      if (n > 0) {
        nof_lcg++;
        ret = true; 
      }
    }
  }
  if (triggered_bsr_type == PADDING) {            
    if (nof_padding_bytes < 4) {
      // If space only for short  
      if (nof_lcg > 1) {
        bsr->format = TRUNC_BSR;
        uint32_t max_prio_ch = find_max_priority_lcid();
        for (int i=0;i<4;i++) {
          if (lcg[max_prio_ch] != i) {
            bsr->buff_size[i] = 0; 
          }
        }
      } else {
        bsr->format = SHORT_BSR;
      }
    } else {
      // If space for long BSR  
      bsr->format = LONG_BSR;
    }
  } else {
    bsr->format = SHORT_BSR;    
    if (nof_lcg > 1) {
      bsr->format = LONG_BSR;
    }  
  }     
  return ret; 
}

// Checks if Regular BSR must be assembled, as defined in 5.4.5 
// Padding BSR is assembled when called by mux_unit when UL grant is received
// Periodic BSR is triggered by the expiration of the timers 
void bsr_proc::step(uint32_t tti)
{
  if (!initiated) {
    return;
  }  
  
  if (!timer_periodic) {
    if (params_db->get_param(mac_params::BSR_TIMER_PERIODIC)) {
      timer_periodic = true; 
      timers_db->get(mac::BSR_TIMER_PERIODIC)->set(this, params_db->get_param(mac_params::BSR_TIMER_PERIODIC));
    }
  }

  if (!timer_retx) {
    if (params_db->get_param(mac_params::BSR_TIMER_RETX)) {
      timer_retx = true; 
      timers_db->get(mac::BSR_TIMER_RETX)->set(this, params_db->get_param(mac_params::BSR_TIMER_RETX));
    }
  }

  // Check condition 1 in Sec 5.4.5   
  if (triggered_bsr_type == NONE) {
    check_single_channel();
  }
  // Higher priority channel is reported regardless of a BSR being already triggered
  check_highest_channel();
  
  update_pending_data();
}

char* bsr_proc::bsr_type_tostring(triggered_bsr_type_t type) {
  switch(type) {
    case bsr_proc::REGULAR: 
      return (char*) "Regular";
    case bsr_proc::PADDING:
      return (char*) "Padding";
    case bsr_proc::PERIODIC: 
      return (char*) "Periodic";
  }
}

char* bsr_proc::bsr_format_tostring(bsr_format_t format) {
  switch(format) {
    case bsr_proc::LONG_BSR: 
      return (char*) "Long";
    case bsr_proc::SHORT_BSR:
      return (char*) "Short";
    case bsr_proc::TRUNC_BSR: 
      return (char*) "Truncated";
  }
}

uint32_t bsr_proc::need_to_send_bsr_on_ul_grant(uint32_t grant_size) 
{
  uint32_t bsr_sz = 0; 
  if (triggered_bsr_type == PERIODIC || triggered_bsr_type == REGULAR) {
    uint32_t total_data = 0; 
    /* Check if grant + MAC SDU headers is enough to accomodate all pending data */
    for (int i=0;i<mac_io_h->NOF_UL_LCH && total_data < grant_size;i++) {
      uint32_t idx = 0; 
      uint32_t sdu_len = 0; 
      while(mac_io_h->get(i+mac_io::MAC_LCH_CCCH_UL)->pop(&sdu_len, idx) && total_data < grant_size) {
        idx++;
        total_data += sch_pdu::size_plus_header_sdu(sdu_len/8);
      }
    }
    total_data--; // Because last SDU has no size header 
    
    /* All triggered BSRs shall be cancelled in case the UL grant can accommodate all pending data available for transmission
       but is not sufficient to additionally accommodate the BSR MAC control element plus its subheader.
     */
    bsr_t bsr;     
    generate_bsr(&bsr, 0);
    bsr_sz = bsr.format==LONG_BSR?3:1;
    if (total_data <= grant_size && total_data + 1 + bsr_sz > grant_size) {
      bsr_sz = 0; 
      Debug("Grant is not enough to accomodate the BSR MAC CE\n");
      triggered_bsr_type = NONE; 
    }
    Debug("Checking if Regular BSR is sent: grant_size=%d, total_data=%d, bsr_sz=%d\n", 
         grant_size, total_data, bsr_sz);
  }
  return bsr_sz; 
}

bool bsr_proc::generate_bsr_on_ul_grant(uint32_t nof_padding_bytes, bsr_t *bsr) 
{
  bool ret = false; 

  if (triggered_bsr_type != NONE || nof_padding_bytes >= 2) {


    if (triggered_bsr_type == NONE) {
      triggered_bsr_type = PADDING;      
    }
    generate_bsr(bsr, nof_padding_bytes);
    ret = true; 
    Debug("Sending BSR type %s, format %s, nof_padding_bytes=%d\n", 
           bsr_type_tostring(triggered_bsr_type), bsr_format_tostring(bsr->format), nof_padding_bytes);
    
    if (timer_periodic && bsr->format != TRUNC_BSR) {
      timers_db->get(mac::BSR_TIMER_PERIODIC)->reset();
      timers_db->get(mac::BSR_TIMER_PERIODIC)->run();
    }
    // Cancel all triggered BSR       
    triggered_bsr_type = NONE;     
    reset_sr = true;     
  }
  
  // Restart or Start ReTX timer
  if (timer_retx) {
    timers_db->get(mac::BSR_TIMER_RETX)->reset();
    timers_db->get(mac::BSR_TIMER_RETX)->run();
  }
  return ret; 
}

bool bsr_proc::need_to_reset_sr() {
  if (reset_sr) {
    reset_sr = false; 
    sr_is_sent = false; 
    return true; 
  } else {
    return false; 
  }
}

bool bsr_proc::need_to_send_sr() {
  if (!sr_is_sent && triggered_bsr_type == REGULAR) {
    reset_sr = false; 
    sr_is_sent = true; 
    return true; 
  } 
  return false; 
}

void bsr_proc::setup_lcg(uint32_t lcid, uint32_t new_lcg)
{
  if (lcid < MAX_LCID && new_lcg < 4) {
    lcg[lcid] = new_lcg; 
  }      
}

void bsr_proc::set_priority(uint32_t lcid, uint32_t priority) {
  if (lcid < MAX_LCID) {
    priorities[lcid] = priority;     
  }
}

uint32_t bsr_proc::find_max_priority_lcid() {
  uint32_t max_prio = 0, max_idx = 0; 
  for (int i=0;i<MAX_LCID;i++) {
    if (priorities[i] > max_prio) {
      max_prio = priorities[i]; 
      max_idx  = i; 
    }
  }
  return max_idx; 
}

}
}
