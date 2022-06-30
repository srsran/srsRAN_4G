/**
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
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

namespace srsue {

proc_sr_nr::proc_sr_nr(srslog::basic_logger& logger) : logger(logger) {}

int32_t proc_sr_nr::init(mac_interface_sr_nr* mac_, phy_interface_mac_nr* phy_, rrc_interface_mac* rrc_)
{
  std::lock_guard<std::mutex> lock(mutex);
  rrc        = rrc_;
  mac        = mac_;
  phy        = phy_;
  sr_counter = 0;

  initiated = true;

  return SRSRAN_SUCCESS;
}

void proc_sr_nr::reset()
{
  std::lock_guard<std::mutex> lock(mutex);
  reset_nolock();
}

void proc_sr_nr::reset_nolock()
{
  is_pending_sr = false;
}

int32_t proc_sr_nr::set_config(const srsran::sr_cfg_nr_t& cfg_)
{
  {
    std::lock_guard<std::mutex> lock(mutex);
    // disable by default
    cfg.enabled = false;
  }

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
  } else {
    logger.info("SR:    Disabling procedure");
  }

  {
    std::lock_guard<std::mutex> lock(mutex);
    // store config
    cfg = cfg_;
  }

  return SRSRAN_SUCCESS;
}

void proc_sr_nr::step(uint32_t tti)
{
  std::lock_guard<std::mutex> lock(mutex);
  if (!initiated) {
    return;
  }

  // As long as at least one SR is pending, the MAC entity shall for each pending SR:
  if (!is_pending_sr) {
    return;
  }

  // 1> if the MAC entity has no valid PUCCH resource configured for the pending SR:
  if (not phy->has_valid_sr_resource(cfg.item[0].sched_request_id)) {
    // 2> initiate a Random Access procedure (see clause 5.1) on the SpCell and cancel the pending SR.
    logger.info("SR:    PUCCH not configured. Starting RA procedure");
    mac->start_ra();
    reset_nolock();
    return;
  }

  // Handle
  if (sr_counter >= cfg.item[0].trans_max) {
    logger.info(
        "SR:    Releasing PUCCH/SRS resources, sr_counter=%d, sr-TransMax=%d", sr_counter, cfg.item[0].trans_max);
    srsran::console("Scheduling request failed: releasing RRC connection...\n");
    rrc->release_pucch_srs();

    // 4> clear any configured downlink assignments and uplink grants;
    phy->clear_pending_grants();

    // 4> clear any PUSCH resources for semi-persistent CSI reporting;
    // ... TODO

    mac->start_ra();
    reset_nolock();
  }
}

bool proc_sr_nr::sr_opportunity(uint32_t tti, uint32_t sr_id, bool meas_gap, bool ul_sch_tx)
{
  std::lock_guard<std::mutex> lock(mutex);
  // 2> when the MAC entity has an SR transmission occasion on the valid PUCCH resource for SR configured; and
  if (!initiated || !cfg.enabled || !is_pending_sr) {
    return false;
  }

  // 2> if sr-ProhibitTimer is not running at the time of the SR transmission occasion; and
  // ... TODO

  // 2> if the PUCCH resource for the SR transmission occasion does not overlap with a measurement gap; and
  if (meas_gap) {
    return false;
  }

  // 2> if the PUCCH resource for the SR transmission occasion does not overlap with a UL-SCH resource:
  if (ul_sch_tx) {
    return false;
  }

  // 3> if SR_COUNTER < sr-TransMax:
  if (sr_counter < cfg.item[0].trans_max) { //
    // 4> increment SR_COUNTER by 1;
    sr_counter += 1;

    // 4> start the sr-ProhibitTimer.
    // ... TODO

    // 4> instruct the physical layer to signal the SR on one valid PUCCH resource for SR;
    logger.info("SR:    Signalling PHY sr_counter=%d", sr_counter);
    return true;
  }

  // 3> else:
  // step will execute

  return false;
}

void proc_sr_nr::start()
{
  std::lock_guard<std::mutex> lock(mutex);
  if (initiated) {
    if (not is_pending_sr) {
      logger.info("SR:    Starting procedure");
      sr_counter    = 0;
      is_pending_sr = true;
    } else {
      logger.debug("SR:    Already pending for Tx");
    }
  } else {
    logger.warning("SR:    Procedure not initiated");
  }
}

} // namespace srsue
