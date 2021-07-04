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

#ifndef SRSRAN_PDCP_LTE_TEST_H
#define SRSRAN_PDCP_LTE_TEST_H

#include "pdcp_base_test.h"
#include "srsran/test/ue_test_interfaces.h"
#include "srsran/upper/pdcp_entity_lte.h"

// Helper struct to hold a packet and the number of clock
// ticks to run after writing the packet to test timeouts.
struct pdcp_test_event_t {
  srsran::unique_byte_buffer_t pkt;
  uint32_t                     ticks = 0;
};

/*
 * Constant definitions that are common to multiple tests
 */
// Encryption and Integrity Keys
std::array<uint8_t, 32> k_int = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10,
                                 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x20, 0x21,
                                 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x30, 0x31};
std::array<uint8_t, 32> k_enc = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10,
                                 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x20, 0x21,
                                 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x30, 0x31};

// Security Configuration, common to all tests.
srsran::as_security_config_t sec_cfg = {
    k_int,
    k_enc,
    k_int,
    k_enc,
    srsran::INTEGRITY_ALGORITHM_ID_128_EIA2,
    srsran::CIPHERING_ALGORITHM_ID_128_EEA2,
};

// Test SDUs for tx
uint8_t sdu1[] = {0x18, 0xe2};
uint8_t sdu2[] = {0xde, 0xad};

// This is the normal initial state. All state variables are set to zero
srsran::pdcp_lte_state_t normal_init_state = {};

/*
 * Helper classes to reduce copy / pasting in setting up tests
 */
// PDCP helper to setup PDCP + Dummy
class pdcp_lte_test_helper
{
public:
  pdcp_lte_test_helper(srsran::pdcp_config_t cfg, srsran::as_security_config_t sec_cfg_, srslog::basic_logger& logger) :
    rlc(logger), rrc(logger), gw(logger), pdcp(&rlc, &rrc, &gw, &stack.task_sched, logger, 0)
  {
    pdcp.configure(cfg);
    pdcp.config_security(sec_cfg_);
    pdcp.enable_integrity(srsran::DIRECTION_TXRX);
    pdcp.enable_encryption(srsran::DIRECTION_TXRX);
  }

  void set_pdcp_initial_state(const srsran::pdcp_lte_state_t& init_state) { pdcp.set_bearer_state(init_state, false); }

  rlc_dummy               rlc;
  rrc_dummy               rrc;
  gw_dummy                gw;
  srsue::stack_test_dummy stack;
  srsran::pdcp_entity_lte pdcp;
};

// Helper function to generate PDUs
srsran::unique_byte_buffer_t gen_expected_pdu(const srsran::unique_byte_buffer_t& in_sdu,
                                              uint32_t                            count,
                                              uint8_t                             pdcp_sn_len,
                                              srsran::pdcp_rb_type_t              rb_type,
                                              srsran::as_security_config_t        sec_cfg,
                                              srslog::basic_logger&               logger)
{
  srsran::pdcp_config_t cfg = {1,
                               rb_type,
                               srsran::SECURITY_DIRECTION_UPLINK,
                               srsran::SECURITY_DIRECTION_DOWNLINK,
                               pdcp_sn_len,
                               srsran::pdcp_t_reordering_t::ms500,
                               srsran::pdcp_discard_timer_t::infinity,
                               false,
                               srsran::srsran_rat_t::lte};

  pdcp_lte_test_helper     pdcp_hlp(cfg, sec_cfg, logger);
  srsran::pdcp_entity_lte* pdcp = &pdcp_hlp.pdcp;
  rlc_dummy*               rlc  = &pdcp_hlp.rlc;

  srsran::pdcp_lte_state_t init_state = {};
  init_state.tx_hfn                   = pdcp->HFN(count);
  init_state.next_pdcp_tx_sn          = pdcp->SN(count);
  pdcp_hlp.set_pdcp_initial_state(init_state);

  srsran::unique_byte_buffer_t sdu = srsran::make_byte_buffer();
  *sdu                             = *in_sdu;
  pdcp->write_sdu(std::move(sdu));
  srsran::unique_byte_buffer_t out_pdu = srsran::make_byte_buffer();
  rlc->get_last_sdu(out_pdu);

  return out_pdu;
}

// Helper function to generate vector of PDU from a vector of TX_NEXTs for generating expected pdus
std::vector<pdcp_test_event_t> gen_expected_pdus_vector(const srsran::unique_byte_buffer_t& in_sdu,
                                                        const std::vector<uint32_t>&        tx_nexts,
                                                        uint8_t                             pdcp_sn_len,
                                                        srsran::pdcp_rb_type_t              rb_type,
                                                        srsran::as_security_config_t        sec_cfg_,
                                                        srslog::basic_logger&               logger)
{
  std::vector<pdcp_test_event_t> pdu_vec;
  for (uint32_t tx_next : tx_nexts) {
    pdcp_test_event_t event;
    event.pkt   = gen_expected_pdu(in_sdu, tx_next, pdcp_sn_len, rb_type, sec_cfg_, logger);
    event.ticks = 0;
    pdu_vec.push_back(std::move(event));
  }
  return pdu_vec;
}

#endif // SRSRAN_PDCP_NR_TEST_H
