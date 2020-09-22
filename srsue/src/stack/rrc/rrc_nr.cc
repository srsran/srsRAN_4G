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

#include "srsue/hdr/stack/rrc/rrc_nr.h"

using namespace asn1::rrc_nr;

namespace srsue {

const char* rrc_nr::rrc_nr_state_text[] = {"IDLE", "CONNECTED", "CONNECTED-INACTIVE"};

rrc_nr::rrc_nr() : log_h("RRC") {}

rrc_nr::~rrc_nr() = default;

void rrc_nr::init(phy_interface_rrc_nr*  phy_,
                  mac_interface_rrc_nr*  mac_,
                  rlc_interface_rrc*     rlc_,
                  pdcp_interface_rrc*    pdcp_,
                  gw_interface_rrc*      gw_,
                  srslte::timer_handler* timers_,
                  stack_interface_rrc*   stack_,
                  const rrc_nr_args_t&   args_)
{
  phy    = phy_;
  rlc    = rlc_;
  pdcp   = pdcp_;
  gw     = gw_;
  timers = timers_;
  stack  = stack_;
  args   = args_;

  log_h->info("Creating dummy DRB on LCID=%d\n", args.coreless.drb_lcid);
  srslte::rlc_config_t rlc_cnfg = srslte::rlc_config_t::default_rlc_um_nr_config(6);
  rlc->add_bearer(args.coreless.drb_lcid, rlc_cnfg);

  srslte::pdcp_config_t pdcp_cnfg{args.coreless.drb_lcid,
                                  srslte::PDCP_RB_IS_DRB,
                                  srslte::SECURITY_DIRECTION_DOWNLINK,
                                  srslte::SECURITY_DIRECTION_UPLINK,
                                  srslte::PDCP_SN_LEN_18,
                                  srslte::pdcp_t_reordering_t::ms500,
                                  srslte::pdcp_discard_timer_t ::ms100};

  pdcp->add_bearer(args.coreless.drb_lcid, pdcp_cnfg);

  running = true;
}

void rrc_nr::stop()
{
  running = false;
}

void rrc_nr::get_metrics(rrc_nr_metrics_t& m) {}

// Timeout callback interface
void rrc_nr::timer_expired(uint32_t timeout_id) {}

void rrc_nr::srslte_rrc_log(const char* str) {}

template <class T>
void rrc_nr::log_rrc_message(const std::string& source, direction_t dir, const srslte::byte_buffer_t* pdu, const T& msg)
{}

// PHY interface
void rrc_nr::in_sync() {}
void rrc_nr::out_of_sync() {}

// MAC interface
void rrc_nr::run_tti(uint32_t tti) {}

// PDCP interface
void rrc_nr::write_pdu(uint32_t lcid, srslte::unique_byte_buffer_t pdu) {}
void rrc_nr::write_pdu_bcch_bch(srslte::unique_byte_buffer_t pdu) {}
void rrc_nr::write_pdu_bcch_dlsch(srslte::unique_byte_buffer_t pdu) {}
void rrc_nr::write_pdu_pcch(srslte::unique_byte_buffer_t pdu) {}
void rrc_nr::write_pdu_mch(uint32_t lcid, srslte::unique_byte_buffer_t pdu) {}

// RLC interface
void rrc_nr::max_retx_attempted() {}

// STACK interface
void rrc_nr::cell_search_completed(const rrc_interface_phy_lte::cell_search_ret_t& cs_ret, const phy_cell_t& found_cell)
{}

} // namespace srsue