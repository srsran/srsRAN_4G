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

#include "srsue/hdr/mac/proc_phr.h"
#include "srsue/hdr/mac/mac.h"
#include "srsue/hdr/mac/mux.h"
#include "srslte/interfaces/ue_interfaces.h"


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

  timer_periodic_id = timers_db->get_unique_id();
  timer_prohibit_id = timers_db->get_unique_id();

  reset();
}

void phr_proc::reset()
{
  phr_is_triggered = false; 
  timer_periodic_value = -2;
  timer_prohibit_value = -2;
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
    
void phr_proc::start_timer() {
  timers_db->get(timer_periodic_id)->run();
}    

/* Trigger PHR when timers exire */
void phr_proc::timer_expired(uint32_t timer_id) {
  if(timer_id == timer_periodic_id) {
    timers_db->get(timer_periodic_id)->reset();
    timers_db->get(timer_periodic_id)->run();
    Debug("PHR:   Triggered by timer periodic (timer expired).\n");
    phr_is_triggered = true;
  } else if (timer_id == timer_prohibit_id) {
    int pathloss_db = liblte_rrc_dl_pathloss_change_num[mac_cfg->main.phr_cnfg.dl_pathloss_change];
    if (pathloss_changed()) {
      Info("PHR:   Triggered by pathloss difference. cur_pathloss_db=%d (timer expired)\n", last_pathloss_db);
      phr_is_triggered = true;
    }
  } else {
    log_h->warning("Received timer callback from unknown timer_id=%d\n", timer_id);
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
    if (timer_periodic_value != cfg_timer_periodic && cfg_timer_periodic > 0)
    {
      timer_periodic_value = cfg_timer_periodic;
      timers_db->get(timer_periodic_id)->set(this, timer_periodic_value);
      timers_db->get(timer_periodic_id)->run();
      phr_is_triggered = true; 
      Info("PHR:   Configured timer periodic %d ms\n", timer_periodic_value);
    }

  }

  int cfg_timer_prohibit = liblte_rrc_prohibit_phr_timer_num[mac_cfg->main.phr_cnfg.prohibit_phr_timer];

  if (timer_prohibit_value != cfg_timer_prohibit && cfg_timer_prohibit > 0)
  {
    timer_prohibit_value = cfg_timer_prohibit;
    timers_db->get(timer_prohibit_id)->set(this, timer_prohibit_value);
    timers_db->get(timer_prohibit_id)->run();
    Info("PHR:   Configured timer prohibit %d ms\n", timer_prohibit_value);
    phr_is_triggered = true; 
  }  
  if (pathloss_changed() && timers_db->get(timer_prohibit_id)->is_expired())
  {
    Info("PHR:   Triggered by pathloss difference. cur_pathloss_db=%d\n", last_pathloss_db);
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
    
    timers_db->get(timer_periodic_id)->reset();
    timers_db->get(timer_prohibit_id)->reset();
    timers_db->get(timer_periodic_id)->run();
    timers_db->get(timer_prohibit_id)->run();
    
    phr_is_triggered = false; 
    
    return true; 
  } else {
    return false; 
  }
}

}
