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

#ifndef SRSUE_TTCN3_UE_H
#define SRSUE_TTCN3_UE_H

#include "lte_ttcn3_phy.h"
#include "srsue/hdr/stack/ue_stack_lte.h"
#include <sstream>

class ttcn3_ue : public phy_interface_syssim, public gw_interface_stack
{
public:
  ttcn3_ue();

  virtual ~ttcn3_ue();

  int init(all_args_t args, srslte::logger* logger_, syssim_interface_phy* syssim_, const std::string tc_name_);

  void stop();

  bool switch_on();

  bool switch_off();

  bool enable_data();

  bool disable_data();

  // The interface for SYSSIM
  void set_cell_map(lte_ttcn3_phy::cell_list_t phy_cell_map);

  void new_grant_ul(const srsue::mac_interface_phy_lte::mac_grant_ul_t grant);

  void new_tb(const srsue::mac_interface_phy_lte::mac_grant_dl_t mac_grant, const uint8_t* data);

  void set_current_tti(uint32_t tti);

  uint16_t get_dl_sched_rnti(uint32_t tti);

  // GW interface
  void add_mch_port(uint32_t lcid, uint32_t port);
  void write_pdu(uint32_t lcid, srslte::unique_byte_buffer_t pdu);
  void write_pdu_mch(uint32_t lcid, srslte::unique_byte_buffer_t pdu);
  int  setup_if_addr(uint32_t lcid, uint8_t pdn_type, uint32_t ip_addr, uint8_t* ipv6_if_id, char* err_str);

  int apply_traffic_flow_template(const uint8_t&                                 eps_bearer_id,
                                  const uint8_t&                                 lcid,
                                  const LIBLTE_MME_TRAFFIC_FLOW_TEMPLATE_STRUCT* tft);

  void set_test_loop_mode(const test_loop_mode_state_t mode, const uint32_t ip_pdu_delay_ms_ = 0);

  void timer_expired(uint32_t timeout_id);

  void send_queued_data();

  void loop_back_pdu_with_tft(uint32_t input_lcid, srslte::unique_byte_buffer_t pdu);

private:
  std::unique_ptr<lte_ttcn3_phy> phy;
  std::unique_ptr<ue_stack_lte>  stack;

  // Generic logger members
  srslte::logger*    logger = nullptr;
  srslte::log_filter log; // Own logger for UE

  test_loop_mode_state_t                                         test_loop_mode = TEST_LOOP_INACTIVE;
  srslte::timer_handler::unique_timer                            pdu_delay_timer;
  std::map<uint32_t, block_queue<srslte::unique_byte_buffer_t> > pdu_queue; // A PDU queue for each DRB
  tft_pdu_matcher                                                tft_matcher;

  all_args_t args = {};
};

#endif // SRSUE_TTCN3_UE_H
