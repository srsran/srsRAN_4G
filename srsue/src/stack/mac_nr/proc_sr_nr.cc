/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#include "srsue/hdr/stack/mac_nr/proc_sr_nr.h"
#include "srsran/common/standard_streams.h"
#include "srsran/interfaces/ue_phy_interfaces.h"
#include "srsran/interfaces/ue_rrc_interfaces.h"
#include "srsue/hdr/stack/mac_nr/proc_ra_nr.h"

namespace srsue {

proc_sr_nr::proc_sr_nr(srslog::basic_logger& logger) : logger(logger) {}

int32_t proc_sr_nr::init(proc_ra_nr* ra_, phy_interface_mac_nr* phy_, rrc_interface_mac* rrc_)
{
  rrc        = rrc_;
  ra         = ra_;
  phy        = phy_;
  initiated  = true;
  sr_counter = 0;
  return SRSRAN_SUCCESS;
}

void proc_sr_nr::reset()
{
  is_pending_sr = false;
}

bool proc_sr_nr::need_tx(uint32_t tti)
{
  int last_tx_tti = 0; // FIXME: phy->sr_last_tx_tti();
  logger.debug("SR:    need_tx(): last_tx_tti=%d, tti=%d", last_tx_tti, tti);
  if (last_tx_tti >= 0) {
    // TODO: implement prohibit timer
    if (TTI_SUB(last_tx_tti, tti) >= 8) {
      return true;
    }
  }
  return false;
}

int32_t proc_sr_nr::set_config(const srsran::sr_cfg_nr_t& cfg_)
{
  // disable by default
  cfg.enabled = false;

  if (cfg_.num_items != 1) {
    logger.error("Only one SR config supported. Disabling SR.");
    return SRSRAN_ERROR;
  }

  if (cfg_.enabled && cfg_.item[0].trans_max == 0) {
    logger.error("Zero is an invalid value for sr-TransMax (n4, n8, n16, n32, n64 are supported). Disabling SR.");
    return SRSRAN_ERROR;
  }

  if (cfg_.enabled && cfg_.item[0].prohibit_timer > 0) {
    logger.error("sr-ProhibitTimer isn't supported. Disabling SR.");
    return SRSRAN_ERROR;
  }

  if (cfg_.enabled) {
    logger.info("SR:    Set sr-TransMax=%d", cfg_.item[0].trans_max);
  }

  // store config
  cfg = cfg_;

  return SRSRAN_SUCCESS;
}

void proc_sr_nr::step(uint32_t tti)
{
  if (initiated) {
    if (is_pending_sr) {
      if (cfg.enabled) {
        if (sr_counter < cfg.item[0].trans_max) {
          if (sr_counter == 0 || need_tx(tti)) {
            sr_counter++;
            logger.info("SR:    Signalling PHY sr_counter=%d", sr_counter);
            phy->sr_send(0); // @xavierarteaga what is the ID you expect here?
          }
        } else {
          if (need_tx(tti)) {
            logger.info("SR:    Releasing PUCCH/SRS resources, sr_counter=%d, sr-TransMax=%d",
                        sr_counter,
                        cfg.item[0].trans_max);
            srsran::console("Scheduling request failed: releasing RRC connection...\n");
            rrc->release_pucch_srs();
            // TODO:
            // - clear any configured downlink assignments and uplink grants;
            // - clear any PUSCH resources for semi-persistent CSI reporting;
            ra->start_by_mac();
            reset();
          }
        }
      } else {
        logger.info("SR:    PUCCH not configured. Starting RA procedure");
        ra->start_by_mac();
        reset();
      }
    }
  }
}

void proc_sr_nr::start()
{
  if (initiated) {
    if (!is_pending_sr) {
      sr_counter    = 0;
      is_pending_sr = true;
    }
  }
}

} // namespace srsue
