/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

void rrc_nr::get_eutra_nr_capabilities(srslte::byte_buffer_t* eutra_nr_caps)
{
  uint8_t eutra_nr_cap_raw[] = {0x01, 0x1c, 0x04, 0x81, 0x60, 0x00, 0x1c, 0x4d, 0x00, 0x00, 0x00, 0x04,
                                0x00, 0x40, 0x04, 0x04, 0xd0, 0x10, 0x74, 0x06, 0x14, 0xe8, 0x1b, 0x10,
                                0x78, 0x00, 0x00, 0x20, 0x00, 0x10, 0x08, 0x08, 0x01, 0x00, 0x20};

  memcpy(eutra_nr_caps->msg, eutra_nr_cap_raw, sizeof(eutra_nr_cap_raw));
  eutra_nr_caps->N_bytes = sizeof(eutra_nr_cap_raw);
  log_h->debug_hex(
      eutra_nr_caps->msg, eutra_nr_caps->N_bytes, "EUTRA-NR capabilities (%u B)\n", eutra_nr_caps->N_bytes);
  return;
}

void rrc_nr::get_nr_capabilities(srslte::byte_buffer_t* nr_caps)
{
  uint8_t nr_cap_raw[] = {
      0xe1, 0x00, 0x00, 0x00, 0x01, 0x47, 0x7a, 0x03, 0x02, 0x00, 0x00, 0x01, 0x40, 0x48, 0x07, 0x06, 0x0e, 0x02, 0x0c,
      0x00, 0x02, 0x13, 0x60, 0x10, 0x73, 0xe4, 0x20, 0xf0, 0x00, 0x80, 0xc1, 0x30, 0x08, 0x0c, 0x00, 0x00, 0x0a, 0x05,
      0x89, 0xba, 0xc2, 0x19, 0x43, 0x40, 0x88, 0x10, 0x74, 0x18, 0x60, 0x4c, 0x04, 0x41, 0x6c, 0x90, 0x14, 0x06, 0x0c,
      0x78, 0xc7, 0x3e, 0x42, 0x0f, 0x00, 0x58, 0x0c, 0x0e, 0x0e, 0x02, 0x21, 0x3c, 0x84, 0xfc, 0x4d, 0xe0, 0x00, 0x12,
      0x00, 0x00, 0x00, 0x00, 0xe5, 0x4d, 0x00, 0x01, 0x00, 0x00, 0x04, 0x18, 0x60, 0x00, 0x34, 0xaa, 0x60};
  memcpy(nr_caps->msg, nr_cap_raw, sizeof(nr_cap_raw));
  nr_caps->N_bytes = sizeof(nr_cap_raw);
  log_h->debug_hex(nr_caps->msg, nr_caps->N_bytes, "NR capabilities (%u B)\n", nr_caps->N_bytes);
  return;
}

// RLC interface
void rrc_nr::max_retx_attempted() {}

// STACK interface
void rrc_nr::cell_search_completed(const rrc_interface_phy_lte::cell_search_ret_t& cs_ret, const phy_cell_t& found_cell)
{}

} // namespace srsue