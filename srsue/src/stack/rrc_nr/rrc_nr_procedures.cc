/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#include "srsue/hdr/stack/rrc_nr/rrc_nr_procedures.h"
#include "srsran/common/standard_streams.h"

#define Error(fmt, ...) rrc_ptr->logger.error("Proc \"%s\" - " fmt, name(), ##__VA_ARGS__)
#define Warning(fmt, ...) rrc_ptr->logger.warning("Proc \"%s\" - " fmt, name(), ##__VA_ARGS__)
#define Info(fmt, ...) rrc_ptr->logger.info("Proc \"%s\" - " fmt, name(), ##__VA_ARGS__)
#define Debug(fmt, ...) rrc_ptr->logger.debug("Proc \"%s\" - " fmt, name(), ##__VA_ARGS__)

using namespace asn1::rrc_nr;
using namespace asn1;
using namespace srsran;

namespace srsue {

rrc_nr::connection_reconf_no_ho_proc::connection_reconf_no_ho_proc(rrc_nr* parent_) : rrc_ptr(parent_), initiator(nr) {}

proc_outcome_t rrc_nr::connection_reconf_no_ho_proc::init(const reconf_initiator_t initiator_,
                                                          const bool               endc_release_and_add_r15,
                                                          const bool nr_secondary_cell_group_cfg_r15_present,
                                                          const asn1::dyn_octstring nr_secondary_cell_group_cfg_r15,
                                                          const bool                sk_counter_r15_present,
                                                          const uint32_t            sk_counter_r15,
                                                          const bool                nr_radio_bearer_cfg1_r15_present,
                                                          const asn1::dyn_octstring nr_radio_bearer_cfg1_r15)
{
  Info("Starting...");
  initiator = initiator_;

  rrc_recfg_s        rrc_recfg;
  cell_group_cfg_s   cell_group_cfg;
  radio_bearer_cfg_s radio_bearer_cfg;
  asn1::SRSASN_CODE  err;

  if (nr_secondary_cell_group_cfg_r15_present) {
    cbit_ref bref(nr_secondary_cell_group_cfg_r15.data(), nr_secondary_cell_group_cfg_r15.size());
    err = rrc_recfg.unpack(bref);
    if (err != asn1::SRSASN_SUCCESS) {
      Error("Could not unpack NR reconfiguration message.");
      return proc_outcome_t::error;
    }

#if 0
    rrc_ptr->log_rrc_message(
        "RRC NR Reconfiguration", Rx, nr_secondary_cell_group_cfg_r15, rrc_recfg, "NR Secondary Cell Group Cfg R15");
#endif
    if (rrc_recfg.crit_exts.type() != asn1::rrc_nr::rrc_recfg_s::crit_exts_c_::types::rrc_recfg) {
      Error("Reconfiguration does not contain Secondary Cell Group Config");
      return proc_outcome_t::error;
    }

    if (not rrc_recfg.crit_exts.rrc_recfg().secondary_cell_group_present) {
      Error("Reconfiguration does not contain Secondary Cell Group Config");
      return proc_outcome_t::error;
    }

    cbit_ref bref0(rrc_recfg.crit_exts.rrc_recfg().secondary_cell_group.data(),
                   rrc_recfg.crit_exts.rrc_recfg().secondary_cell_group.size());

    err = cell_group_cfg.unpack(bref0);
    if (err != asn1::SRSASN_SUCCESS) {
      Error("Could not unpack cell group message message.");
      return proc_outcome_t::error;
    }

#if 0
    rrc_ptr->log_rrc_message("RRC NR Reconfiguration",
                             Rx,
                             rrc_recfg.crit_exts.rrc_recfg().secondary_cell_group,
                             cell_group_cfg,
                             "Secondary Cell Group Config");
#endif

    Info("Applying Cell Group Cfg");
    if (!rrc_ptr->apply_cell_group_cfg(cell_group_cfg)) {
      return proc_outcome_t::error;
    }
  }

  if (sk_counter_r15_present) {
    Info("Applying Cell Group Cfg");
    if (!rrc_ptr->configure_sk_counter((uint16_t)sk_counter_r15)) {
      return proc_outcome_t::error;
    }
  }

  if (nr_radio_bearer_cfg1_r15_present) {
    cbit_ref bref1(nr_radio_bearer_cfg1_r15.data(), nr_radio_bearer_cfg1_r15.size());

    err = radio_bearer_cfg.unpack(bref1);
    if (err != asn1::SRSASN_SUCCESS) {
      Error("Could not unpack radio bearer config.");
      return proc_outcome_t::error;
    }

#if 0
    rrc_ptr->log_rrc_message(
        "RRC NR Reconfiguration", Rx, nr_radio_bearer_cfg1_r15, radio_bearer_cfg, "Radio Bearer Config R15");
#endif

    Info("Applying Radio Bearer Cfg");
    if (!rrc_ptr->apply_radio_bearer_cfg(radio_bearer_cfg)) {
      return proc_outcome_t::error;
    }
  }

  return proc_outcome_t::success;
}

proc_outcome_t rrc_nr::connection_reconf_no_ho_proc::react(const bool& config_complete)
{
  if (not config_complete) {
    Error("NR reconfiguration failed");
    return proc_outcome_t::error;
  }

  // TODO phy ctrl
  // in case there are scell to configure, wait for second phy configuration
  // if (not rrc_ptr->phy_ctrl->is_config_pending()) {
  //   return proc_outcome_t::yield;
  // }

  Info("Reconfig NR return successful");
  return proc_outcome_t::success;
}

void rrc_nr::connection_reconf_no_ho_proc::then(const srsran::proc_state_t& result)
{
  if (result.is_success()) {
    Info("Finished %s successfully", name());
    srsran::console("RRC NR reconfiguration successful.\n");
    rrc_ptr->rrc_eutra->nr_rrc_con_reconfig_complete(true);
  } else {
    // 5.3.5.8.2 Inability to comply with RRCReconfiguration
    switch (initiator) {
      case reconf_initiator_t::mcg_srb1:
        rrc_ptr->rrc_eutra->nr_notify_reconfiguration_failure();
        break;
      default:
        Warning("Reconfiguration failure not implemented for initiator %d", initiator);
        break;
    }
    srsran::console("RRC NR reconfiguration failed.\n");
    Warning("Finished %s with failure", name());
  }
  return;
}

} // namespace srsue
