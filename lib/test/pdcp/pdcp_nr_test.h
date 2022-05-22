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

#ifndef SRSRAN_PDCP_NR_TEST_H
#define SRSRAN_PDCP_NR_TEST_H

#include "pdcp_base_test.h"
#include "srsran/test/ue_test_interfaces.h"
#include "srsran/upper/pdcp_entity_nr.h"

struct pdcp_initial_state {
  uint32_t tx_next;
  uint32_t rx_next;
  uint32_t rx_deliv;
  uint32_t rx_reord;
};

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

// Test PDUs for rx (generated from SDU1)
uint8_t pdu1_count0_snlen12[]          = {0x80, 0x00, 0x8f, 0xe3, 0xc7, 0x1b, 0xad, 0x14};
uint8_t pdu1_count2048_snlen12[]       = {0x88, 0x00, 0x8d, 0x2c, 0xe5, 0x38, 0xc0, 0x42};
uint8_t pdu1_count4096_snlen12[]       = {0x80, 0x00, 0x97, 0xbe, 0xee, 0x62, 0xf5, 0xe0};
uint8_t pdu1_count4294967295_snlen12[] = {0x8f, 0xff, 0x1e, 0x47, 0xa9, 0x55, 0xa9, 0xd8};
uint8_t pdu1_count0_snlen18[]          = {0x80, 0x00, 0x00, 0x8f, 0xe3, 0x37, 0x33, 0xd5, 0x64};
uint8_t pdu1_count131072_snlen18[]     = {0x82, 0x00, 0x00, 0x15, 0x01, 0x99, 0x97, 0xe0, 0x4e};
uint8_t pdu1_count262144_snlen18[]     = {0x80, 0x00, 0x00, 0xc2, 0x47, 0xc2, 0xee, 0x46, 0xd9};
uint8_t pdu1_count4294967295_snlen18[] = {0x83, 0xff, 0xff, 0x1e, 0x47, 0x78, 0xb8, 0x7a, 0x9f};

// Test PDUs for rx (generated from SDU2)
uint8_t pdu2_count1_snlen12[] = {0x80, 0x01, 0x5e, 0x3d, 0x70, 0x6a, 0xa4, 0x90};
uint8_t pdu2_count1_snlen18[] = {0x80, 0x00, 0x01, 0x5e, 0x3d, 0x93, 0xfe, 0xcc, 0x2e};

// This is the normal initial state. All state variables are set to zero
pdcp_initial_state normal_init_state = {};

// Some tests regarding COUNT wraparound take really long.
// This puts the PCDC state closer to wraparound quickly.
pdcp_initial_state near_wraparound_init_state = {.tx_next  = 4294967295,
                                                 .rx_next  = 4294967295,
                                                 .rx_deliv = 4294967295,
                                                 .rx_reord = 0};

/*
 * Helper classes to reduce copy / pasting in setting up tests
 */
// PDCP helper to setup PDCP + Dummy
class pdcp_nr_test_helper
{
public:
  pdcp_nr_test_helper(srsran::pdcp_config_t cfg, srsran::as_security_config_t sec_cfg_, srslog::basic_logger& logger) :
    rlc(logger), rrc(logger), gw(logger), pdcp(&rlc, &rrc, &gw, &stack.task_sched, logger, 0)
  {
    pdcp.configure(cfg);
    pdcp.config_security(sec_cfg_);
    pdcp.enable_integrity(srsran::DIRECTION_TXRX);
    pdcp.enable_encryption(srsran::DIRECTION_TXRX);
  }

  void set_pdcp_initial_state(pdcp_initial_state init_state)
  {
    pdcp.set_tx_next(init_state.tx_next);
    pdcp.set_rx_next(init_state.rx_next);
    pdcp.set_rx_deliv(init_state.rx_deliv);
    pdcp.set_rx_reord(init_state.rx_reord);
  }

  rlc_dummy               rlc;
  rrc_dummy               rrc;
  gw_dummy                gw;
  srsue::stack_test_dummy stack;
  srsran::pdcp_entity_nr  pdcp;
};

// Helper function to generate PDUs
srsran::unique_byte_buffer_t gen_expected_pdu(const srsran::unique_byte_buffer_t& in_sdu,
                                              uint32_t                            count,
                                              uint8_t                             pdcp_sn_len,
                                              srsran::as_security_config_t        sec_cfg,
                                              srslog::basic_logger&               logger)
{
  srsran::pdcp_config_t cfg = {1,
                               srsran::PDCP_RB_IS_DRB,
                               srsran::SECURITY_DIRECTION_UPLINK,
                               srsran::SECURITY_DIRECTION_DOWNLINK,
                               pdcp_sn_len,
                               srsran::pdcp_t_reordering_t::ms500,
                               srsran::pdcp_discard_timer_t::infinity,
                               false,
                               srsran::srsran_rat_t::nr};

  pdcp_nr_test_helper     pdcp_hlp(cfg, sec_cfg, logger);
  srsran::pdcp_entity_nr* pdcp = &pdcp_hlp.pdcp;
  rlc_dummy*              rlc  = &pdcp_hlp.rlc;

  pdcp_initial_state init_state = {};
  init_state.tx_next            = count;
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
                                                        srsran::as_security_config_t        sec_cfg_,
                                                        srslog::basic_logger&               logger)
{
  std::vector<pdcp_test_event_t> pdu_vec;
  for (uint32_t tx_next : tx_nexts) {
    pdcp_test_event_t event;
    event.pkt   = gen_expected_pdu(in_sdu, tx_next, pdcp_sn_len, sec_cfg_, logger);
    event.ticks = 0;
    pdu_vec.push_back(std::move(event));
  }
  return pdu_vec;
}

#endif // SRSRAN_PDCP_NR_TEST_H
