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

#ifndef SRSUE_RRC_NR_H
#define SRSUE_RRC_NR_H

#include "srslte/asn1/rrc_nr_asn1.h"
#include "srslte/common/block_queue.h"
#include "srslte/common/buffer_pool.h"
#include "srslte/interfaces/nr_common_interface_types.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "srslte/interfaces/ue_nr_interfaces.h"
#include "srsue/hdr/stack/upper/gw.h"

namespace srsue {

// Expert arguments to create GW without proper RRC
struct core_less_args_t {
  std::string      ip_addr;
  srsue::gw_args_t gw_args;
  uint8_t          drb_lcid;
};

struct rrc_nr_args_t {
  core_less_args_t coreless;
  std::string      log_level;
  uint32_t         log_hex_limit;
};

struct rrc_nr_metrics_t {};

class rrc_nr final : public rrc_interface_phy_nr,
                     public rrc_interface_pdcp,
                     public rrc_interface_rlc,
                     public srslte::timer_callback
{
public:
  rrc_nr();
  ~rrc_nr();

  void init(phy_interface_rrc_nr*  phy_,
            mac_interface_rrc_nr*  mac_,
            rlc_interface_rrc*     rlc_,
            pdcp_interface_rrc*    pdcp_,
            gw_interface_rrc*      gw_,
            srslte::timer_handler* timers_,
            stack_interface_rrc*   stack_,
            const rrc_nr_args_t&   args_);

  void stop();

  void get_metrics(rrc_nr_metrics_t& m);

  // Timeout callback interface
  void timer_expired(uint32_t timeout_id) final;
  void srslte_rrc_log(const char* str);

  enum direction_t { Rx = 0, Tx };
  template <class T>
  void log_rrc_message(const std::string& source, direction_t dir, const srslte::byte_buffer_t* pdu, const T& msg);

  // PHY interface
  void in_sync() final;
  void out_of_sync() final;

  // MAC interface
  void run_tti(uint32_t tti) final;

  // RLC interface
  void max_retx_attempted() final;

  // PDCP interface
  void write_pdu(uint32_t lcid, srslte::unique_byte_buffer_t pdu) final;
  void write_pdu_bcch_bch(srslte::unique_byte_buffer_t pdu) final;
  void write_pdu_bcch_dlsch(srslte::unique_byte_buffer_t pdu) final;
  void write_pdu_pcch(srslte::unique_byte_buffer_t pdu) final;
  void write_pdu_mch(uint32_t lcid, srslte::unique_byte_buffer_t pdu) final;

  // STACK interface
  void cell_search_completed(const rrc_interface_phy_lte::cell_search_ret_t& cs_ret, const phy_cell_t& found_cell);

private:
  struct cmd_msg_t {
    enum { PDU, PCCH, PDU_MCH, RLF, PDU_BCCH_DLSCH, STOP } command;
    srslte::unique_byte_buffer_t pdu;
    uint16_t                     lcid;
  };

  bool                           running = false;
  srslte::block_queue<cmd_msg_t> cmd_q;

  phy_interface_rrc_nr* phy = nullptr;
  //  mac_interface_rrc*    mac   = nullptr;
  rlc_interface_rrc*   rlc   = nullptr;
  pdcp_interface_rrc*  pdcp  = nullptr;
  gw_interface_rrc*    gw    = nullptr;
  stack_interface_rrc* stack = nullptr;

  srslte::log_ref log_h;

  /// RRC states (3GPP 38.331 v15.5.1 Sec 4.2.1)
  enum rrc_nr_state_t {
    RRC_NR_STATE_IDLE = 0,
    RRC_NR_STATE_CONNECTED,
    RRC_NR_STATE_CONNECTED_INACTIVE,
    RRC_NR_STATE_N_ITEMS,
  };
  const static char* rrc_nr_state_text[RRC_NR_STATE_N_ITEMS];

  //  rrc_nr_state_t state = RRC_NR_STATE_IDLE;

  rrc_nr_args_t args = {};

  // RRC constants and timers
  srslte::timer_handler* timers = nullptr;

  std::string get_rb_name(uint32_t lcid) final { return srslte::to_string((srslte::rb_id_nr_t)lcid); }
};

} // namespace srsue

#endif // SRSUE_RRC_NR_H