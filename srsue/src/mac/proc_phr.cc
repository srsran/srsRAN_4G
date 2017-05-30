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

#define Error(fmt, ...)   log_h->error_line(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) log_h->warning_line(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define Info(fmt, ...)    log_h->info_line(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define Debug(fmt, ...)   log_h->debug_line(__FILE__, __LINE__, fmt, ##__VA_ARGS__)

#include "mac/proc_phr.h"
#include "mac/mac.h"
#include "mac/mux.h"
#include "common/phy_interface.h"


  namespace srsue {
    
phr_proc::phr_proc()
{
  initiated = false; 
}

void phr_proc::init(phy_interface_mac* phy_h_, srslte::log* log_h_, mac_interface_rrc::mac_cfg_t *mac_cfg_, srslte::timers *timers_db_)
{
  phy_h     = phy_h_;
  log_h     = log_h_; 
  mac_cfg   = mac_cfg_;
  timers_db = timers_db_; 
  initiated = true;
  reset();
}

void phr_proc::reset()
{
  phr_is_triggered = false; 
  timer_periodic = -2; 
  timer_prohibit = -2;
  dl_pathloss_change = -2; 
}

bool phr_proc::pathloss_changed() {
  
  int min_change      = liblte_rrc_dl_pathloss_change_num[mac_cfg->main.phr_cnfg.dl_pathloss_change];
  int cur_pathloss_db = (int) phy_h->get_pathloss_db(); 
  
  if (abs(cur_pathloss_db - last_pathloss_db) > min_change && min_change > 0) {
    last_pathloss_db = cur_pathloss_db;
    return true; 
  } else {
    return false;
  }
}

/* Trigger PHR when timers exire */
void phr_proc::timer_expired(uint32_t timer_id) {
  switch(timer_id) {
    case mac::PHR_TIMER_PERIODIC:
      timers_db->get(mac::PHR_TIMER_PERIODIC)->reset();    
      timers_db->get(mac::PHR_TIMER_PERIODIC)->run();    
      Debug("PHR:   Triggered by timer periodic (timer expired).\n");
      phr_is_triggered = true; 
      break;
    case mac::PHR_TIMER_PROHIBIT:
      int pathloss_db = liblte_rrc_dl_pathloss_change_num[mac_cfg->main.phr_cnfg.dl_pathloss_change];
      if (pathloss_changed()) {
        Info("PHR:   Triggered by pathloss difference. cur_pathloss_db=%f (timer expired)\n", last_pathloss_db);
        phr_is_triggered = true; 
      }
      break;      
  }
}

void phr_proc::step(uint32_t tti)
{
  if (!initiated) {
    return;
  }  
  
  if (mac_cfg->main.phr_cnfg.setup_present) {
    int cfg_timer_periodic = liblte_rrc_periodic_phr_timer_num[mac_cfg->main.phr_cnfg.periodic_phr_timer];
    
    // Setup timers and trigger PHR when configuration changed by higher layers
    if (timer_periodic != cfg_timer_periodic && cfg_timer_periodic > 0) 
    {
      timer_periodic = cfg_timer_periodic; 
      timers_db->get(mac::PHR_TIMER_PERIODIC)->set(this, timer_periodic);
      timers_db->get(mac::PHR_TIMER_PERIODIC)->run();
      phr_is_triggered = true; 
      Info("PHR:   Configured timer periodic %d ms\n", timer_periodic);
    }

  }

  int cfg_timer_prohibit = liblte_rrc_prohibit_phr_timer_num[mac_cfg->main.phr_cnfg.prohibit_phr_timer];

  if (timer_prohibit != cfg_timer_prohibit && cfg_timer_prohibit > 0) 
  {
    timer_prohibit = cfg_timer_prohibit; 
    timers_db->get(mac::PHR_TIMER_PROHIBIT)->set(this, timer_prohibit);
    timers_db->get(mac::PHR_TIMER_PROHIBIT)->run();
    Info("PHR:   Configured timer prohibit %d ms\n", timer_prohibit);
    phr_is_triggered = true; 
  }  
  if (pathloss_changed() && timers_db->get(mac::PHR_TIMER_PROHIBIT)->is_expired()) 
  {
    Info("PHR:   Triggered by pathloss difference. cur_pathloss_db=%f\n", last_pathloss_db);
    phr_is_triggered = true;        
  }
}

bool phr_proc::generate_phr_on_ul_grant(float *phr) 
{
  
  if (phr_is_triggered) {
    if (phr) {
      *phr = phy_h->get_phr();
    }
    
    Debug("PHR:   Generating PHR=%f\n", phr?*phr:0.0);
    
    timers_db->get(mac::PHR_TIMER_PERIODIC)->reset();
    timers_db->get(mac::PHR_TIMER_PROHIBIT)->reset();
    timers_db->get(mac::PHR_TIMER_PERIODIC)->run();
    timers_db->get(mac::PHR_TIMER_PROHIBIT)->run();
    
    phr_is_triggered = false; 
    
    return true; 
  } else {
    return false; 
  }
}

}
