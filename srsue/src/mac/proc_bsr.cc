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

#define Error(fmt, ...)   log_h->error(fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) log_h->warning(fmt, ##__VA_ARGS__)
#define Info(fmt, ...)    log_h->info(fmt, ##__VA_ARGS__)
#define Debug(fmt, ...)   log_h->debug(fmt, ##__VA_ARGS__)

#include "srsue/hdr/mac/proc_bsr.h"
#include "srsue/hdr/mac/mac.h"
#include "srsue/hdr/mac/mux.h"


  namespace srsue {
    
bsr_proc::bsr_proc()
{
  initiated = false; 
  last_print = 0; 
  next_tx_tti = 0; 
  triggered_bsr_type=NONE; 
  
}

void bsr_proc::init(rlc_interface_mac *rlc_, srslte::log* log_h_, mac_interface_rrc::mac_cfg_t *mac_cfg_, srslte::timers *timers_db_)
{
  log_h     = log_h_; 
  rlc       = rlc_; 
  mac_cfg   = mac_cfg_;
  timers_db = timers_db_;

  timer_periodic_id = timers_db->get_unique_id();
  timer_retx_id     = timers_db->get_unique_id();

  reset();
  initiated = true;
}

void bsr_proc::reset()
{
  timers_db->get(timer_periodic_id)->stop();
  timers_db->get(timer_periodic_id)->reset();
  timers_db->get(timer_retx_id)->stop();
  timers_db->get(timer_retx_id)->reset();
  
  reset_sr = false; 
  sr_is_sent = false; 
  triggered_bsr_type = NONE; 
  for (int i=0;i<MAX_LCID;i++)  {
    lcg[i]        = -1; 
    priorities[i] = -1; 
    last_pending_data[i] = 0; 
  }        
  lcg[0] = 0; 
  priorities[0] = 99;   
  next_tx_tti = 0; 
}

/* Process Periodic BSR */
void bsr_proc::timer_expired(uint32_t timer_id) {
  if(timer_id == timer_periodic_id) {
    if (triggered_bsr_type == NONE) {
      // Check condition 4 in Sec 5.4.5
      triggered_bsr_type = PERIODIC;
      Debug("BSR:   Triggering Periodic BSR\n");
    }
  } else if (timer_id == timer_retx_id) {
    // Enable reTx of SR only if periodic timer is not infinity
    int periodic = liblte_rrc_periodic_bsr_timer_num[mac_cfg->main.ulsch_cnfg.periodic_bsr_timer];
    if (periodic >= 0) {
      triggered_bsr_type = REGULAR;
      Debug("BSR:   Triggering BSR reTX\n");
      sr_is_sent = false;
    }
  }
}

// Checks if data is available for a a channel with higher priority than others 
bool bsr_proc::check_highest_channel() {
  int pending_data_lcid = -1; 
  
  for (int i=0;i<MAX_LCID && pending_data_lcid == -1;i++) {
    if (lcg[i] >= 0) {
      if (rlc->get_buffer_state(i) > 0) {
        pending_data_lcid = i; 
        for (int j=0;j<MAX_LCID;j++) {
          if (rlc->get_buffer_state(j) > 0) {
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
    uint32_t nbytes = rlc->get_buffer_state(pending_data_lcid);
    if (nbytes > last_pending_data[pending_data_lcid]) 
    {
      if (triggered_bsr_type != REGULAR) {        
        Debug("BSR:   Triggered REGULAR BSR for Max Priority LCID=%d\n", pending_data_lcid);
      }
      triggered_bsr_type = REGULAR; 
      return true; 
    } 
  }
  return false; 
}

uint32_t bsr_proc::get_buffer_state() {
  uint32_t buffer = 0; 
  for (int i=0;i<MAX_LCID;i++) {
    if (lcg[i] >= 0) {
      buffer += rlc->get_buffer_state(i);
    }
  }
  return buffer; 
}
    
// Checks if only one logical channel has data avaiable for Tx
bool bsr_proc::check_single_channel() {    
  uint32_t pending_data_lcid = 0; 
  uint32_t nof_nonzero_lcid = 0; 
  
  for (int i=0;i<MAX_LCID;i++) {
    if (lcg[i] >= 0) {
      if (rlc->get_buffer_state(i) > 0) {
        pending_data_lcid = i;
        nof_nonzero_lcid++; 
      }
    }
  }
  if (nof_nonzero_lcid == 1) {
    uint32_t nbytes = rlc->get_buffer_state(pending_data_lcid);
    // If there is new data available for this logical channel 
    if (nbytes > last_pending_data[pending_data_lcid]) {
      triggered_bsr_type = REGULAR; 
      Debug("BSR:   Triggered REGULAR BSR for single LCID=%d\n", pending_data_lcid);
      return true; 
    } 
  }
  return false;
}

void bsr_proc::update_pending_data() {
  for (int i=0;i<MAX_LCID;i++) {
    last_pending_data[i] = rlc->get_buffer_state(i); 
  }
}

bool bsr_proc::generate_bsr(bsr_t *bsr, uint32_t nof_padding_bytes) {
  bool ret = false; 
  uint32_t nof_lcg=0;
  bzero(bsr, sizeof(bsr_t));    
  for (int i=0;i<MAX_LCID;i++) {
    if (lcg[i] >= 0) {
      uint32_t n = rlc->get_buffer_state(i);
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
  
  int periodic = liblte_rrc_periodic_bsr_timer_num[mac_cfg->main.ulsch_cnfg.periodic_bsr_timer];
  if (periodic > 0 && (uint32_t)periodic != timers_db->get(timer_periodic_id)->get_timeout())
  {
    timers_db->get(timer_periodic_id)->set(this, periodic);
    timers_db->get(timer_periodic_id)->run();
    Info("BSR:   Configured timer periodic %d ms\n", periodic);    
  }      
  int retx = liblte_rrc_retransmission_bsr_timer_num[mac_cfg->main.ulsch_cnfg.retx_bsr_timer];
  if (retx > 0 && (uint32_t)retx != timers_db->get(timer_retx_id)->get_timeout())
  {
    timers_db->get(timer_retx_id)->set(this, retx);
    timers_db->get(timer_retx_id)->run();
    Info("BSR:   Configured timer reTX %d ms\n", retx);
  }

  // Check condition 1 in Sec 5.4.5   
  if (triggered_bsr_type == NONE) {
    check_single_channel();
  }
  // Higher priority channel is reported regardless of a BSR being already triggered
  check_highest_channel();
  
  update_pending_data();
  
  
  if ((tti - last_print)%10240 > QUEUE_STATUS_PERIOD_MS) {
    char str[128];
    bzero(str, 128);
    for (int i=0;i<MAX_LCID;i++) {
      sprintf(str, "%s%d (%d), ", str, rlc->get_buffer_state(i), last_pending_data[i]);
    }
    Info("BSR:   QUEUE status: %s\n", str);
    last_print = tti; 
  }
  
}

char* bsr_proc::bsr_type_tostring(triggered_bsr_type_t type) {
  switch(type) {
    case bsr_proc::REGULAR: 
      return (char*) "Regular";
    case bsr_proc::PADDING:
      return (char*) "Padding";
    case bsr_proc::PERIODIC: 
      return (char*) "Periodic";
    default:
      return (char*) "Regular";
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
    default:
      return (char*) "Short";
  }
}

bool bsr_proc::need_to_send_bsr_on_ul_grant(uint32_t grant_size, bsr_t *bsr) 
{
  bool ret = false; 

  uint32_t bsr_sz = 0; 
  if (triggered_bsr_type == PERIODIC || triggered_bsr_type == REGULAR) {
    /* Check if grant + MAC SDU headers is enough to accomodate all pending data */
    int total_data = 0; 
    for (int i=0;i<MAX_LCID && total_data < (int)grant_size;i++) {
      total_data += srslte::sch_pdu::size_header_sdu(rlc->get_buffer_state(i))+rlc->get_buffer_state(i);      
    }
    total_data--; // Because last SDU has no size header 
    
    /* All triggered BSRs shall be cancelled in case the UL grant can accommodate all pending data available for transmission
       but is not sufficient to additionally accommodate the BSR MAC control element plus its subheader.
     */
    generate_bsr(bsr, 0);
    bsr_sz = bsr->format==LONG_BSR?3:1;
    if (total_data <= (int)grant_size && total_data + 1 + bsr_sz > grant_size) {
      Debug("Grant is not enough to accomodate the BSR MAC CE\n");
    } else {
      Debug("BSR:   Including Regular BSR: grant_size=%d, total_data=%d, bsr_sz=%d\n", 
          grant_size, total_data, bsr_sz);
      ret = true; 
    }    
    if (timers_db->get(timer_periodic_id)->get_timeout() && bsr->format != TRUNC_BSR) {
      timers_db->get(timer_periodic_id)->reset();
      timers_db->get(timer_periodic_id)->run();
    }
  }
  // Cancel all triggered BSR and SR     
  triggered_bsr_type = NONE; 
  reset_sr = true;     
  // Restart or Start ReTX timer
  if (timers_db->get(timer_retx_id)->get_timeout()) {
    timers_db->get(timer_retx_id)->reset();
    timers_db->get(timer_retx_id)->run();
  }
  return ret;   
}

bool bsr_proc::generate_padding_bsr(uint32_t nof_padding_bytes, bsr_t *bsr) 
{
  bool ret = false; 

  if (triggered_bsr_type != NONE || nof_padding_bytes >= 2) {

    if (triggered_bsr_type == NONE) {
      triggered_bsr_type = PADDING;      
    }
    generate_bsr(bsr, nof_padding_bytes);
    ret = true; 
    Info("BSR:   Type %s, Format %s, Value=%d,%d,%d,%d\n", 
         bsr_type_tostring(triggered_bsr_type), bsr_format_tostring(bsr->format), 
         bsr->buff_size[0], bsr->buff_size[1], bsr->buff_size[2], bsr->buff_size[3]);
    
    if (timers_db->get(timer_periodic_id)->get_timeout() && bsr->format != TRUNC_BSR) {
      timers_db->get(timer_periodic_id)->reset();
      timers_db->get(timer_periodic_id)->run();
    }    
    
  }
  return ret; 
}

void bsr_proc::set_tx_tti(uint32_t tti) {
  Debug("BSR:   Set next_tx_tti=%d\n", tti);
  next_tx_tti = tti;  
}

bool bsr_proc::need_to_reset_sr() {
  if (reset_sr) {
    reset_sr = false; 
    sr_is_sent = false; 
    Debug("BSR:   SR reset. sr_is_sent and reset_rs false\n");
    return true; 
  } else {
    return false; 
  }
}

bool bsr_proc::need_to_send_sr(uint32_t tti) {
  if (!sr_is_sent && triggered_bsr_type == REGULAR) {
    if (srslte_tti_interval(tti,next_tx_tti)>0 && srslte_tti_interval(tti,next_tx_tti) < 10240-HARQ_DELAY_MS) {
      reset_sr = false; 
      sr_is_sent = true; 
      Debug("BSR:   Need to send sr: sr_is_sent=true, reset_sr=false, tti=%d, next_tx_tti=%d\n", tti, next_tx_tti);
      return true; 
    } else {
      Debug("BSR:   Not sending SR because tti=%d, next_tx_tti=%d\n", tti, next_tx_tti);
    }
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
  int32_t max_prio = 0;
  uint32_t max_idx = 0;
  for (int i=0;i<MAX_LCID;i++) {
    if (priorities[i] > max_prio) {
      max_prio = priorities[i]; 
      max_idx  = i; 
    }
  }
  return max_idx; 
}

}
