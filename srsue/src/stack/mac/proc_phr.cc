/*
 * Copyright 2013-2019 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
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

#define Error(fmt, ...)   log_h->error(fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) log_h->warning(fmt, ##__VA_ARGS__)
#define Info(fmt, ...)    log_h->info(fmt, ##__VA_ARGS__)
#define Debug(fmt, ...)   log_h->debug(fmt, ##__VA_ARGS__)

#include "srsue/hdr/stack/mac/proc_phr.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "srsue/hdr/stack/mac/mac.h"
#include "srsue/hdr/stack/mac/mux.h"

namespace srsue {

phr_proc::phr_proc()
{
  initiated        = false;
  last_pathloss_db = 0;
  phr_cfg          = {};
}

void phr_proc::init(phy_interface_mac_lte* phy_h_, srslte::log* log_h_, srslte::timers* timers_db_)
{
  phy_h     = phy_h_;
  log_h     = log_h_;
  timers_db = timers_db_;
  initiated = true;

  timer_periodic_id = timers_db->get_unique_id();
  timer_prohibit_id = timers_db->get_unique_id();

  reset();
}

void phr_proc::reset()
{
  timers_db->get(timer_periodic_id)->stop();
  timers_db->get(timer_prohibit_id)->stop();
  phr_is_triggered     = false;
}

void phr_proc::set_config(srslte::phr_cfg_t& cfg)
{
  phr_cfg = cfg;

  // First stop timers. If enabled==false or value is Inf, won't be re-started
  timers_db->get(timer_periodic_id)->stop();
  timers_db->get(timer_prohibit_id)->stop();

  if (cfg.enabled) {
    // Setup timers and trigger PHR when configuration changed by higher layers
    if (phr_cfg.periodic_timer > 0) {
      timers_db->get(timer_periodic_id)->set(this, phr_cfg.periodic_timer);
      timers_db->get(timer_periodic_id)->run();
      phr_is_triggered = true;
      Info("PHR:   Configured timer periodic %d ms\n", phr_cfg.periodic_timer);
    }

    if (phr_cfg.prohibit_timer > 0) {
      timers_db->get(timer_prohibit_id)->set(this, phr_cfg.prohibit_timer);
      timers_db->get(timer_prohibit_id)->run();
      Info("PHR:   Configured timer prohibit %d ms\n", phr_cfg.prohibit_timer);
      phr_is_triggered = true;
    }
  }
}

bool phr_proc::pathloss_changed() {
  if (!phr_cfg.enabled) {
    return false;
  }

  int cur_pathloss_db = (int)phy_h->get_pathloss_db();

  if (abs(cur_pathloss_db - last_pathloss_db) > phr_cfg.db_pathloss_change && phr_cfg.db_pathloss_change > 0) {
    last_pathloss_db = cur_pathloss_db;
    return true;
  } else {
    return false;
  }
}

void phr_proc::start_timer() {
  timers_db->get(timer_periodic_id)->run();
}

/* Trigger PHR when timers exires */
void phr_proc::timer_expired(uint32_t timer_id) {
  if (!phr_cfg.enabled) {
    Warning("PHR:   Timer triggered but PHR has been disabled\n");
    return;
  }
  if (timer_id == timer_periodic_id) {
    timers_db->get(timer_periodic_id)->reset();
    timers_db->get(timer_periodic_id)->run();
    Debug("PHR:   Triggered by timer periodic (timer expired).\n");
    phr_is_triggered = true;
  } else if (timer_id == timer_prohibit_id) {
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
  if (phr_cfg.enabled && initiated) {
    if (pathloss_changed() && timers_db->get(timer_prohibit_id)->is_expired()) {
      Info("PHR:   Triggered by pathloss difference. cur_pathloss_db=%d\n", last_pathloss_db);
      phr_is_triggered = true;
    }
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

bool phr_proc::is_extended()
{
  return phr_cfg.extended;
}
}
