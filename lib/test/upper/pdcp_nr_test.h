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

#ifndef SRSLTE_PDCP_NR_TEST_H
#define SRSLTE_PDCP_NR_TEST_H

#include "srslte/common/buffer_pool.h"
#include "srslte/common/log_filter.h"
#include "srslte/common/security.h"
#include "srslte/upper/pdcp_entity_nr.h"
#include <iostream>

/*
 * Functions and macros for comparisions
 */
#define TESTASSERT(cond)                                                                                               \
  {                                                                                                                    \
    if (!(cond)) {                                                                                                     \
      std::cout << "[" << __FUNCTION__ << "][Line " << __LINE__ << "]: FAIL at " << (#cond) << std::endl;              \
      return -1;                                                                                                       \
    }                                                                                                                  \
  }

int compare_two_packets(const srslte::unique_byte_buffer_t& msg1, const srslte::unique_byte_buffer_t& msg2)
{
  TESTASSERT(msg1->N_bytes == msg2->N_bytes);
  for (uint32_t i = 0; i < msg1->N_bytes; ++i) {
    TESTASSERT(msg1->msg[i] == msg2->msg[i]);
  }
  return 0;
}

/*
 * Definition of helpful structs for testing
 */
struct pdcp_security_cfg {
  uint8_t*                            k_int_rrc;
  uint8_t*                            k_enc_rrc;
  uint8_t*                            k_int_up;
  uint8_t*                            k_enc_up;
  srslte::INTEGRITY_ALGORITHM_ID_ENUM int_algo;
  srslte::CIPHERING_ALGORITHM_ID_ENUM enc_algo;
};

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
 * Dummy classes
 */
class rlc_dummy : public srsue::rlc_interface_pdcp
{
public:
  rlc_dummy(srslte::log* log_) : log(log_) {}

  void get_last_sdu(const srslte::unique_byte_buffer_t& pdu)
  {
    memcpy(pdu->msg, last_pdcp_pdu->msg, last_pdcp_pdu->N_bytes);
    pdu->N_bytes = last_pdcp_pdu->N_bytes;
    return;
  }
  void write_sdu(uint32_t lcid, srslte::unique_byte_buffer_t sdu, bool blocking = true)
  {
    log->info_hex(sdu->msg, sdu->N_bytes, "RLC SDU");
    last_pdcp_pdu.swap(sdu);
    rx_count++;
  }
  void discard_sdu(uint32_t lcid, uint32_t discard_sn)
  {
    log->info("Notifing RLC to discard SDU (SN=%" PRIu32 ")\n", discard_sn);
    discard_count++;
    log->info("Discard_count=%" PRIu64 "\n", discard_count);
  }

  uint64_t rx_count      = 0;
  uint64_t discard_count = 0;

private:
  srslte::log*                 log;
  srslte::unique_byte_buffer_t last_pdcp_pdu;

  bool rb_is_um(uint32_t lcid) { return false; }
};

class rrc_dummy : public srsue::rrc_interface_pdcp
{
public:
  rrc_dummy(srslte::log* log_) {}

  void write_pdu(uint32_t lcid, srslte::unique_byte_buffer_t pdu) {}
  void write_pdu_bcch_bch(srslte::unique_byte_buffer_t pdu) {}
  void write_pdu_bcch_dlsch(srslte::unique_byte_buffer_t pdu) {}
  void write_pdu_pcch(srslte::unique_byte_buffer_t pdu) {}
  void write_pdu_mch(uint32_t lcid, srslte::unique_byte_buffer_t pdu) {}

  std::string get_rb_name(uint32_t lcid) { return "None"; }
};

class gw_dummy : public srsue::gw_interface_pdcp
{
public:
  gw_dummy(srslte::log* log_) : log(log_) {}

  void     write_pdu_mch(uint32_t lcid, srslte::unique_byte_buffer_t pdu) {}
  uint32_t rx_count = 0;

  void get_last_pdu(const srslte::unique_byte_buffer_t& pdu)
  {
    memcpy(pdu->msg, last_pdu->msg, last_pdu->N_bytes);
    pdu->N_bytes = last_pdu->N_bytes;
    return;
  }
  void write_pdu(uint32_t lcid, srslte::unique_byte_buffer_t pdu)
  {
    log->info_hex(pdu->msg, pdu->N_bytes, "GW PDU");
    rx_count++;
    last_pdu.swap(pdu);
  }

private:
  srslte::log*                 log;
  srslte::unique_byte_buffer_t last_pdu;
};

/*
 * Helper classes to reduce copy / pasting in setting up tests
 */
// PDCP helper to setup PDCP + Dummy
class pdcp_nr_test_helper
{
public:
  pdcp_nr_test_helper(srslte::pdcp_config_t cfg, pdcp_security_cfg sec_cfg, srslte::log* log) :
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

  void set_pdcp_initial_state(pdcp_initial_state init_state)
  {
    pdcp.set_tx_next(init_state.tx_next);
    pdcp.set_rx_next(init_state.rx_next);
    pdcp.set_rx_deliv(init_state.rx_deliv);
    pdcp.set_rx_reord(init_state.rx_reord);
  }

  rlc_dummy              rlc;
  rrc_dummy              rrc;
  gw_dummy               gw;
  srslte::timer_handler  timers;
  srslte::pdcp_entity_nr pdcp;
};

// Helper function to generate PDUs
srslte::unique_byte_buffer_t gen_expected_pdu(const srslte::unique_byte_buffer_t& in_sdu,
                                              uint32_t                            count,
                                              uint8_t                             pdcp_sn_len,
                                              pdcp_security_cfg                   sec_cfg,
                                              srslte::byte_buffer_pool*           pool,
                                              srslte::log*                        log)
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
  pdcp->write_sdu(std::move(sdu), true);
  srslte::unique_byte_buffer_t out_pdu = srslte::allocate_unique_buffer(*pool);
  rlc->get_last_sdu(out_pdu);

  return out_pdu;
}

// Helper function to generate vector of PDU from a vector of TX_NEXTs for generating expected pdus
std::vector<pdcp_test_event_t> gen_expected_pdus_vector(const srslte::unique_byte_buffer_t& in_sdu,
                                                        const std::vector<uint32_t>&        tx_nexts,
                                                        uint8_t                             pdcp_sn_len,
                                                        pdcp_security_cfg                   sec_cfg,
                                                        srslte::byte_buffer_pool*           pool,
                                                        srslte::log*                        log)
{
  std::vector<pdcp_test_event_t> pdu_vec;
  for (uint32_t tx_next : tx_nexts) {
    pdcp_test_event_t event;
    event.pkt   = gen_expected_pdu(in_sdu, tx_next, pdcp_sn_len, sec_cfg, pool, log);
    event.ticks = 0;
    pdu_vec.push_back(std::move(event));
  }
  return pdu_vec;
}

// Helper to print packets
void print_packet_array(const srslte::unique_byte_buffer_t& msg)
{
  printf("uint8_t msg[] = {\n");
  for (uint64_t i = 0; i < msg->N_bytes; ++i) {
    printf("0x%02x, ", msg->msg[i]);
  }
  printf("\n};\n");
}
#endif // SRSLTE_PDCP_NR_TEST_H
