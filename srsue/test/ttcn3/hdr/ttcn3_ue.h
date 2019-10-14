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

#ifndef SRSUE_TTCN3_UE_H
#define SRSUE_TTCN3_UE_H

#include "lte_ttcn3_phy.h"
#include "srsue/hdr/stack/ue_stack_lte.h"
#include <sstream>

class ttcn3_ue : public phy_interface_syssim, public gw_interface_stack
{
public:
  ttcn3_ue() {}

  virtual ~ttcn3_ue() {}

  int init(all_args_t args, srslte::logger* logger_, syssim_interface_phy* syssim_, const std::string tc_name_)
  {
    logger = logger_;

    // Init UE log
    log.init("UE  ", logger);
    log.set_level(srslte::LOG_LEVEL_INFO);
    log.info("Built in %s mode using %s.\n", srslte_get_build_mode(), srslte_get_build_info());

    // Patch args
    args.stack.nas.force_imsi_attach = true;
    args.stack.nas.eia               = "1,2,3";
    args.stack.nas.eea               = "0,1,2,3";

    // Configure default parameters
    args.stack.usim.algo = "xor";
    args.stack.usim.imei = "356092040793011";
    args.stack.usim.imsi = "001010123456789";                  // Anritsu test USIM
    args.stack.usim.k    = "000102030405060708090A0B0C0D0E0F"; // fixed as per TS 34.108 Sec. 8.2

    args.stack.rrc.feature_group       = 0xe6041000;
    args.stack.rrc.ue_category_str     = SRSLTE_UE_CATEGORY_DEFAULT;
    args.stack.rrc.ue_category         = atoi(args.stack.rrc.ue_category_str.c_str());
    args.stack.rrc.nof_supported_bands = 1;
    args.stack.rrc.supported_bands[0]  = 7;
    args.stack.rrc.release             = 8;
    args.stack.rrc.mbms_service_id     = -1;

    args.phy.dl_earfcn = "3400";
    args.rf.type       = "none";
    args.stack.type    = "lte";
    args.phy.type      = "lte_ttcn3";

    // Instantiate layers and stack together our UE
    if (args.stack.type == "lte") {
      stack = std::unique_ptr<ue_stack_lte>(new ue_stack_lte());
      if (!stack) {
        log.console("Error creating LTE stack instance.\n");
        return SRSLTE_ERROR;
      }

      phy = std::unique_ptr<srsue::lte_ttcn3_phy>(new srsue::lte_ttcn3_phy(logger));
      if (!phy) {
        log.console("Error creating LTE PHY instance.\n");
        return SRSLTE_ERROR;
      }
    } else {
      log.console("Invalid stack type %s. Supported values are [lte].\n", args.stack.type.c_str());
      return SRSLTE_ERROR;
    }

    // init layers
    if (phy->init(args.phy, stack.get(), syssim_)) {
      log.console("Error initializing PHY.\n");
      return SRSLTE_ERROR;
    }

    if (stack->init(args.stack, logger, phy.get(), this)) {
      log.console("Error initializing stack.\n");
      return SRSLTE_ERROR;
    }

    return SRSLTE_SUCCESS;
  }

  void stop()
  {
    // nothing to do here
  }

  bool switch_on() { return stack->switch_on(); }

  bool switch_off() { return stack->switch_off(); }

  bool enable_data() { return stack->enable_data(); }

  bool disable_data() { return stack->disable_data(); }

  // The interface for SYSSIM
  void set_cell_map(lte_ttcn3_phy::cell_list_t phy_cell_map) { phy->set_cell_map(phy_cell_map); }

  void new_grant_ul(const srsue::mac_interface_phy_lte::mac_grant_ul_t grant) { phy->new_grant_ul(grant); }

  void new_tb(const srsue::mac_interface_phy_lte::mac_grant_dl_t mac_grant, const uint8_t* data)
  {
    phy->new_tb(mac_grant, data);
  }

  void set_current_tti(uint32_t tti) { phy->set_current_tti(tti); }

  uint16_t get_dl_sched_rnti(uint32_t tti) { return stack->get_dl_sched_rnti(tti); }

  // GW interface
  void add_mch_port(uint32_t lcid, uint32_t port) {}
  void write_pdu(uint32_t lcid, srslte::unique_byte_buffer_t pdu) {}
  void write_pdu_mch(uint32_t lcid, srslte::unique_byte_buffer_t pdu) {}
  int setup_if_addr(uint32_t lcid, uint8_t pdn_type, uint32_t ip_addr, uint8_t* ipv6_if_id, char* err_str) { return 0; }
  int apply_traffic_flow_template(const uint8_t&                                 eps_bearer_id,
                                  const uint8_t&                                 lcid,
                                  const LIBLTE_MME_TRAFFIC_FLOW_TEMPLATE_STRUCT* tft)
  {
    return 0;
  }

private:
  std::unique_ptr<lte_ttcn3_phy> phy;
  std::unique_ptr<ue_stack_lte>  stack;

  // Generic logger members
  srslte::logger*    logger = nullptr;
  srslte::log_filter log; // Own logger for UE

  all_args_t args = {};
};

#endif // SRSUE_TTCN3_UE_H