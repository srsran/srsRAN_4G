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

#ifndef SRSLTE_PDCP_NR_TEST_H
#define SRSLTE_PDCP_NR_TEST_H

#include "pdcp_base_test.h"
#include "srslte/test/ue_test_interfaces.h"
#include "srslte/upper/pdcp_entity_nr.h"

struct pdcp_initial_state {
  uint32_t tx_next;
  uint32_t rx_next;
  uint32_t rx_deliv;
  uint32_t rx_reord;
};

// Helper struct to hold a packet and the number of clock
// ticks to run after writing the packet to test timeouts.
struct pdcp_test_event_t {
  srslte::unique_byte_buffer_t pkt;
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
srslte::as_security_config_t sec_cfg = {
    k_int,
    k_enc,
    k_int,
    k_enc,
    srslte::INTEGRITY_ALGORITHM_ID_128_EIA2,
    srslte::CIPHERING_ALGORITHM_ID_128_EEA2,
};

// Test SDUs for tx
uint8_t sdu1[] = {0x18, 0xe2};
uint8_t sdu2[] = {0xde, 0xad};

// Test PDUs for rx (generated from SDU1)
uint8_t pdu1_count0_snlen12[]          = {0x80, 0x00, 0x8f, 0xe3, 0xe0, 0xdf, 0x82, 0x92};
uint8_t pdu1_count2048_snlen12[]       = {0x88, 0x00, 0x8d, 0x2c, 0x47, 0x5e, 0xb1, 0x5b};
uint8_t pdu1_count4096_snlen12[]       = {0x80, 0x00, 0x97, 0xbe, 0xa3, 0x32, 0xfa, 0x61};
uint8_t pdu1_count4294967295_snlen12[] = {0x8f, 0xff, 0x1e, 0x47, 0xe6, 0x86, 0x28, 0x6c};
uint8_t pdu1_count0_snlen18[]          = {0x80, 0x00, 0x00, 0x8f, 0xe3, 0xe0, 0xdf, 0x82, 0x92};
uint8_t pdu1_count131072_snlen18[]     = {0x82, 0x00, 0x00, 0x15, 0x01, 0xf4, 0xb0, 0xfc, 0xc5};
uint8_t pdu1_count262144_snlen18[]     = {0x80, 0x00, 0x00, 0xc2, 0x47, 0xa8, 0xdd, 0xc0, 0x73};
uint8_t pdu1_count4294967295_snlen18[] = {0x83, 0xff, 0xff, 0x1e, 0x47, 0xe6, 0x86, 0x28, 0x6c};

// Test PDUs for rx (generated from SDU2)
uint8_t pdu2_count1_snlen12[] = {0x80, 0x01, 0x5e, 0x3d, 0x64, 0xaf, 0xac, 0x7c};
uint8_t pdu2_count1_snlen18[] = {0x80, 0x00, 0x01, 0x5e, 0x3d, 0x64, 0xaf, 0xac, 0x7c};

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
  pdcp_nr_test_helper(srslte::pdcp_config_t cfg, srslte::as_security_config_t sec_cfg_, srslte::log_ref log) :
    rlc(log),
    rrc(log),
    gw(log),
    pdcp(&rlc, &rrc, &gw, &stack.task_sched, log, 0, cfg)
  {
    pdcp.config_security(sec_cfg_);
    pdcp.enable_integrity(srslte::DIRECTION_TXRX);
    pdcp.enable_encryption(srslte::DIRECTION_TXRX);
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
  srslte::pdcp_entity_nr  pdcp;
};

// Helper function to generate PDUs
srslte::unique_byte_buffer_t gen_expected_pdu(const srslte::unique_byte_buffer_t& in_sdu,
                                              uint32_t                            count,
                                              uint8_t                             pdcp_sn_len,
                                              srslte::as_security_config_t        sec_cfg,
                                              srslte::byte_buffer_pool*           pool,
                                              srslte::log_ref                     log)
{
  srslte::pdcp_config_t cfg = {1,
                               srslte::PDCP_RB_IS_DRB,
                               srslte::SECURITY_DIRECTION_UPLINK,
                               srslte::SECURITY_DIRECTION_DOWNLINK,
                               pdcp_sn_len,
                               srslte::pdcp_t_reordering_t::ms500,
                               srslte::pdcp_discard_timer_t::infinity};

  pdcp_nr_test_helper     pdcp_hlp(cfg, sec_cfg, log);
  srslte::pdcp_entity_nr* pdcp = &pdcp_hlp.pdcp;
  rlc_dummy*              rlc  = &pdcp_hlp.rlc;

  pdcp_initial_state init_state = {};
  init_state.tx_next            = count;
  pdcp_hlp.set_pdcp_initial_state(init_state);

  srslte::unique_byte_buffer_t sdu = srslte::allocate_unique_buffer(*pool);
  *sdu                             = *in_sdu;
  pdcp->write_sdu(std::move(sdu));
  srslte::unique_byte_buffer_t out_pdu = srslte::allocate_unique_buffer(*pool);
  rlc->get_last_sdu(out_pdu);

  return out_pdu;
}

// Helper function to generate vector of PDU from a vector of TX_NEXTs for generating expected pdus
std::vector<pdcp_test_event_t> gen_expected_pdus_vector(const srslte::unique_byte_buffer_t& in_sdu,
                                                        const std::vector<uint32_t>&        tx_nexts,
                                                        uint8_t                             pdcp_sn_len,
                                                        srslte::as_security_config_t        sec_cfg_,
                                                        srslte::byte_buffer_pool*           pool,
                                                        srslte::log_ref                     log)
{
  std::vector<pdcp_test_event_t> pdu_vec;
  for (uint32_t tx_next : tx_nexts) {
    pdcp_test_event_t event;
    event.pkt   = gen_expected_pdu(in_sdu, tx_next, pdcp_sn_len, sec_cfg_, pool, log);
    event.ticks = 0;
    pdu_vec.push_back(std::move(event));
  }
  return pdu_vec;
}

#endif // SRSLTE_PDCP_NR_TEST_H
