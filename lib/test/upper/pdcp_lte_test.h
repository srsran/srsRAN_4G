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

#ifndef SRSLTE_PDCP_LTE_TEST_H
#define SRSLTE_PDCP_LTE_TEST_H

#include "pdcp_base_test.h"
#include "srslte/upper/pdcp_entity_lte.h"

struct pdcp_lte_initial_state {
  uint32_t tx_count;
  uint32_t rx_hfn;
  uint32_t next_pdcp_rx_sn;
  uint32_t last_submitted_pdcp_rx_sn;
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
uint8_t k_int[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
                   0x16, 0x17, 0x18, 0x19, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x30, 0x31};
uint8_t k_enc[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
                   0x16, 0x17, 0x18, 0x19, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x30, 0x31};

// Security Configuration, common to all tests.
pdcp_security_cfg sec_cfg = {
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

// This is the normal initial state. All state variables are set to zero
pdcp_lte_initial_state normal_init_state = {};

/*
 * Helper classes to reduce copy / pasting in setting up tests
 */
// PDCP helper to setup PDCP + Dummy
class pdcp_lte_test_helper
{
public:
  pdcp_lte_test_helper(srslte::pdcp_config_t cfg, pdcp_security_cfg sec_cfg, srslte::log* log) :
    rlc(log),
    rrc(log),
    gw(log),
    timers(64),
    pdcp(&rlc, &rrc, &gw, &timers, log)
  {
    pdcp.init(0, cfg);
    pdcp.config_security(
        sec_cfg.k_enc_rrc, sec_cfg.k_int_rrc, sec_cfg.k_enc_up, sec_cfg.k_int_up, sec_cfg.enc_algo, sec_cfg.int_algo);
    pdcp.enable_integrity();
    pdcp.enable_encryption();
  }

  void set_pdcp_initial_state(pdcp_lte_initial_state init_state)
  {

    pdcp.set_tx_count(init_state.tx_count);
    pdcp.set_rx_hfn(init_state.rx_hfn);
    pdcp.set_next_pdcp_rx_sn(init_state.next_pdcp_rx_sn);
    pdcp.set_last_submitted_pdcp_rx_sn(init_state.last_submitted_pdcp_rx_sn);
  }

  rlc_dummy               rlc;
  rrc_dummy               rrc;
  gw_dummy                gw;
  srslte::timer_handler   timers;
  srslte::pdcp_entity_lte pdcp;
};

// Helper function to generate PDUs
srslte::unique_byte_buffer_t gen_expected_pdu(const srslte::unique_byte_buffer_t& in_sdu,
                                              uint32_t                            count,
                                              uint8_t                             pdcp_sn_len,
                                              srslte::pdcp_rb_type_t              rb_type,
                                              pdcp_security_cfg                   sec_cfg,
                                              srslte::byte_buffer_pool*           pool,
                                              srslte::log*                        log)
{
  srslte::pdcp_config_t cfg = {1,
                               rb_type,
                               srslte::SECURITY_DIRECTION_UPLINK,
                               srslte::SECURITY_DIRECTION_DOWNLINK,
                               pdcp_sn_len,
                               srslte::pdcp_t_reordering_t::ms500,
                               srslte::pdcp_discard_timer_t::infinity};

  pdcp_lte_test_helper     pdcp_hlp(cfg, sec_cfg, log);
  srslte::pdcp_entity_lte* pdcp = &pdcp_hlp.pdcp;
  rlc_dummy*               rlc  = &pdcp_hlp.rlc;

  pdcp_lte_initial_state init_state = {};
  init_state.tx_count               = count;
  pdcp_hlp.set_pdcp_initial_state(init_state);

  srslte::unique_byte_buffer_t sdu = srslte::allocate_unique_buffer(*pool);
  *sdu                             = *in_sdu;
  pdcp->write_sdu(std::move(sdu), true);
  srslte::unique_byte_buffer_t out_pdu = srslte::allocate_unique_buffer(*pool);
  rlc->get_last_sdu(out_pdu);

  return out_pdu;
}

// Helper function to generate vector of PDU from a vector of TX_NEXTs for generating expected pdus
std::vector<pdcp_test_event_t> gen_expected_pdus_vector(const srslte::unique_byte_buffer_t& in_sdu,
                                                        const std::vector<uint32_t>&        tx_nexts,
                                                        uint8_t                             pdcp_sn_len,
                                                        srslte::pdcp_rb_type_t              rb_type,
                                                        pdcp_security_cfg                   sec_cfg,
                                                        srslte::byte_buffer_pool*           pool,
                                                        srslte::log*                        log)
{
  std::vector<pdcp_test_event_t> pdu_vec;
  for (uint32_t tx_next : tx_nexts) {
    pdcp_test_event_t event;
    event.pkt   = gen_expected_pdu(in_sdu, tx_next, pdcp_sn_len, rb_type, sec_cfg, pool, log);
    event.ticks = 0;
    pdu_vec.push_back(std::move(event));
  }
  return pdu_vec;
}

#endif // SRSLTE_PDCP_NR_TEST_H
