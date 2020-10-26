/*
 * Copyright 2013-2020 Software Radio Systems Limited
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

#define Error(fmt, ...) log_h->error(fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) log_h->warning(fmt, ##__VA_ARGS__)
#define Info(fmt, ...) log_h->info(fmt, ##__VA_ARGS__)
#define Debug(fmt, ...) log_h->debug(fmt, ##__VA_ARGS__)

#include "srsue/hdr/stack/mac/proc_sr.h"
#include "srsue/hdr/stack/mac/proc_ra.h"

namespace srsue {

sr_proc::sr_proc()
{
  initiated = false;
}

void sr_proc::init(ra_proc* ra_, phy_interface_mac_lte* phy_h_, rrc_interface_mac* rrc_, srslte::log_ref log_h_)
{
  log_h      = log_h_;
  rrc        = rrc_;
  ra         = ra_;
  phy_h      = phy_h_;
  initiated  = true;
  sr_counter = 0;
}

void sr_proc::reset()
{
  is_pending_sr = false;
}

bool sr_proc::need_tx(uint32_t tti)
{
  int last_tx_tti = phy_h->sr_last_tx_tti();
  Debug("SR:    need_tx(): last_tx_tti=%d, tti=%d\n", last_tx_tti, tti);
  if (last_tx_tti >= 0) {
    if (tti > (uint32_t)last_tx_tti) {
      if (tti - last_tx_tti > 8) {
        return true;
      }
    } else {
      uint32_t interval = 10240 - last_tx_tti + tti;
      if (interval > 8 && tti < 8) {
        return true;
      }
    }
  }
  return false;
}

void sr_proc::set_config(srslte::sr_cfg_t& cfg)
{
  if (cfg.enabled && cfg.dsr_transmax == 0) {
    Error("Zero is an invalid value for dsr-TransMax (n4, n8, n16, n32, n64 are supported). Disabling SR.\n");
    return;
  }
  if (cfg.enabled) {
    Info("SR:    Set dsr_transmax=%d\n", sr_cfg.dsr_transmax);
  }
  sr_cfg = cfg;
}

void sr_proc::step(uint32_t tti)
{
  if (initiated) {
    if (is_pending_sr) {
      if (sr_cfg.enabled) {
        if (sr_counter < sr_cfg.dsr_transmax) {
          if (sr_counter == 0 || need_tx(tti)) {
            sr_counter++;
            Info("SR:    Signalling PHY sr_counter=%d\n", sr_counter);
            phy_h->sr_send();
          }
        } else {
          if (need_tx(tti)) {
            Info("SR:    Releasing PUCCH/SRS resources, sr_counter=%d, dsr_transmax=%d\n",
                 sr_counter,
                 sr_cfg.dsr_transmax);
            srslte::console("Scheduling request failed: releasing RRC connection...\n");
            rrc->release_pucch_srs();
            ra->start_mac_order();
            reset();
          }
        }
      } else if (ra->is_idle()) {
        Info("SR:    PUCCH not configured. Starting RA procedure\n");
        ra->start_mac_order();
        reset();
      }
    }
  }
}

void sr_proc::start()
{
  if (initiated) {
    if (!is_pending_sr) {
      sr_counter    = 0;
      is_pending_sr = true;
    }
  }
}

} // namespace srsue
