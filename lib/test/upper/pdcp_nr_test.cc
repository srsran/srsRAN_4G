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

#include "srslte/common/buffer_pool.h"
#include "srslte/common/log_filter.h"
#include "srslte/common/security.h"
#include "srslte/upper/pdcp_entity_nr.h"
#include <iostream>

#define TESTASSERT(cond)                                                                                               \
  {                                                                                                                    \
    if (!(cond)) {                                                                                                     \
      std::cout << "[" << __FUNCTION__ << "][Line " << __LINE__ << "]: FAIL at " << (#cond) << std::endl;              \
      return -1;                                                                                                       \
    }                                                                                                                  \
  }

// Encryption and Integrity Keys
uint8_t k_int[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
                   0x16, 0x17, 0x18, 0x19, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x30, 0x31};
uint8_t k_enc[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
                   0x16, 0x17, 0x18, 0x19, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x30, 0x31};

// Test SDUs for tx
uint8_t  sdu1[]   = {0x18, 0xE2};
uint32_t SDU1_LEN = 2;

// Test PDUs for rx
uint8_t  pdu1[]   = {0x80, 0x00, 0x8f, 0xe3, 0xe0, 0xdf, 0x82, 0x92};
uint32_t PDU1_LEN = 8;

uint8_t  pdu2[]   = {0x88, 0x00, 0x8d, 0x2c, 0x47, 0x5e, 0xb1, 0x5b};
uint32_t PDU2_LEN = 8;

uint8_t  pdu3[]   = {0x80, 0x00, 0x97, 0xbe, 0xa3, 0x32, 0xfa, 0x61};
uint32_t PDU3_LEN = 8;

uint8_t  pdu4[]   = {0x80, 0x00, 0x00, 0x8f, 0xe3, 0xe0, 0xdf, 0x82, 0x92};
uint32_t PDU4_LEN = 9;

uint8_t  pdu5[]   = {0x82, 0x00, 0x00, 0x15, 0x01, 0xf4, 0xb0, 0xfc, 0xc5};
uint32_t PDU5_LEN = 9;

uint8_t  pdu6[]   = {0x80, 0x00, 0x00, 0xc2, 0x47, 0xa8, 0xdd, 0xc0, 0x73};
uint32_t PDU6_LEN = 9;

// dummy classes
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
  }

private:
  srslte::log*                 log;
  srslte::unique_byte_buffer_t last_pdcp_pdu;

  bool rb_is_um(uint32_t lcid) { return false; }
};

class rrc_dummy : public srsue::rrc_interface_pdcp
{
public:
  rrc_dummy(srslte::log* log_) : log(log_) {}

  void write_pdu(uint32_t lcid, srslte::unique_byte_buffer_t pdu) {}
  void write_pdu_bcch_bch(srslte::unique_byte_buffer_t pdu) {}
  void write_pdu_bcch_dlsch(srslte::unique_byte_buffer_t pdu) {}
  void write_pdu_pcch(srslte::unique_byte_buffer_t pdu) {}
  void write_pdu_mch(uint32_t lcid, srslte::unique_byte_buffer_t pdu) {}

  std::string get_rb_name(uint32_t lcid) { return "None"; }

private:
  srslte::log* log;
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
 * Genric function to test transmission of in-sequence packets
 */
int test_tx(uint32_t                     n_packets,
            uint8_t                      pdcp_sn_len,
            srslte::unique_byte_buffer_t pdu_exp,
            srslte::byte_buffer_pool*    pool,
            srslte::log*                 log)
{
  srslte::pdcp_entity_nr pdcp;
  srslte::pdcp_config_t  cfg = {
      1, srslte::PDCP_RB_IS_DRB, srslte::SECURITY_DIRECTION_UPLINK, srslte::SECURITY_DIRECTION_DOWNLINK, pdcp_sn_len};

  rlc_dummy rlc(log);
  rrc_dummy rrc(log);
  gw_dummy  gw(log);

  pdcp.init(&rlc, &rrc, &gw, log, 0, cfg);
  pdcp.config_security(
      k_enc, k_int, k_enc, k_int, srslte::CIPHERING_ALGORITHM_ID_128_EEA2, srslte::INTEGRITY_ALGORITHM_ID_128_EIA2);
  pdcp.enable_integrity();
  pdcp.enable_encryption();

  // Test SDU
  srslte::unique_byte_buffer_t sdu = allocate_unique_buffer(*pool);
  memcpy(sdu->msg, sdu1, SDU1_LEN);
  sdu->N_bytes = SDU1_LEN;

  // Run test
  for (uint32_t i = 0; i < n_packets; ++i) {
    // Test SDU
    srslte::unique_byte_buffer_t sdu = allocate_unique_buffer(*pool);
    memcpy(sdu->msg, sdu1, SDU1_LEN);
    sdu->N_bytes = SDU1_LEN;
    pdcp.write_sdu(std::move(sdu), true);
  }
  srslte::unique_byte_buffer_t pdu_act = allocate_unique_buffer(*pool);
  rlc.get_last_sdu(pdu_act);

  TESTASSERT(pdu_act->N_bytes == pdu_exp->N_bytes);
  for (uint32_t i = 0; i < pdu_exp->N_bytes; ++i) {
    TESTASSERT(pdu_act->msg[i] == pdu_exp->msg[i]);
  }
  return 0;
}

/*
 * TX Test: PDCP Entity with SN LEN = 12 and 18. Tested COUNT = 0, 2048 and 4096
 * PDCP entity configured with EIA2 and EEA2
 */
int test_tx_all(srslte::byte_buffer_pool* pool, srslte::log* log)
{
  /*
   * TX Test 1: PDCP Entity with SN LEN = 12
   * PDCP entity configured with EIA2 and EEA2
   * TX_NEXT = 0.
   * Input: {0x18, 0xE2}
   * Output: PDCP Header {0x80, 0x00}, Ciphered Text {0x8f, 0xe3}, MAC-I {0xe0, 0xdf, 0x82, 0x92}
   */
  srslte::unique_byte_buffer_t pdu_exp_sn0_len12 = allocate_unique_buffer(*pool);
  memcpy(pdu_exp_sn0_len12->msg, pdu1, PDU1_LEN);
  pdu_exp_sn0_len12->N_bytes = PDU1_LEN;
  TESTASSERT(test_tx(1, srslte::PDCP_SN_LEN_12, std::move(pdu_exp_sn0_len12), pool, log) == 0);

  /*
   * TX Test 2: PDCP Entity with SN LEN = 12
   * PDCP entity configured with EIA2 and EEA2
   * TX_NEXT = 2048.
   * Input: {0x18, 0xE2}
   * Output: PDCP Header {0x88, 0x00}, Ciphered Text {0x8d, 0x2c}, MAC-I {0x47, 0x5e, 0xb1, 0x5b}
   */
  srslte::unique_byte_buffer_t pdu_exp_sn2048_len12 = allocate_unique_buffer(*pool);
  memcpy(pdu_exp_sn2048_len12->msg, pdu2, PDU2_LEN);
  pdu_exp_sn2048_len12->N_bytes = PDU2_LEN;
  TESTASSERT(test_tx(2049, srslte::PDCP_SN_LEN_12, std::move(pdu_exp_sn2048_len12), pool, log) == 0);

  /*
   * TX Test 3: PDCP Entity with SN LEN = 12
   * PDCP entity configured with EIA2 and EEA2
   * TX_NEXT = 4096.
   * Input: {0x18, 0xE2}
   * Output: PDCP Header {0x80,0x00}, Ciphered Text {0x97, 0xbe}, MAC-I {0xa3, 0x32, 0xfa, 0x61}
   */
  srslte::unique_byte_buffer_t pdu_exp_sn4096_len12 = allocate_unique_buffer(*pool);
  memcpy(pdu_exp_sn4096_len12->msg, pdu3, PDU3_LEN);
  pdu_exp_sn4096_len12->N_bytes = PDU3_LEN;
  TESTASSERT(test_tx(4097, srslte::PDCP_SN_LEN_12, std::move(pdu_exp_sn4096_len12), pool, log) == 0);

  /*
   * TX Test 4: PDCP Entity with SN LEN = 18
   * PDCP entity configured with EIA2 and EEA2
   * TX_NEXT = 0.
   * Input: {0x18, 0xE2}
   * Output: PDCP Header {0x80, 0x80, 0x00}, Ciphered Text {0x8f, 0xe3}, MAC-I {0xe0, 0xdf, 0x82, 0x92}
   */
  srslte::unique_byte_buffer_t pdu_exp_sn0_len18 = allocate_unique_buffer(*pool);
  memcpy(pdu_exp_sn0_len18->msg, pdu4, PDU4_LEN);
  pdu_exp_sn0_len18->N_bytes = PDU4_LEN;
  TESTASSERT(test_tx(1, srslte::PDCP_SN_LEN_18, std::move(pdu_exp_sn0_len18), pool, log) == 0);

  /*
   * TX Test 5: PDCP Entity with SN LEN = 18
   * PDCP entity configured with EIA2 and EEA2
   * TX_NEXT = 131072.
   * Input: {0x18, 0xE2}
   * Output: PDCP Header {0x82, 0x00, 0x00}, Ciphered Text {0x15, 0x01}, MAC-I {0xf4, 0xb0, 0xfc, 0xc5}
   */
  srslte::unique_byte_buffer_t pdu_exp_sn131072_len18 = allocate_unique_buffer(*pool);
  memcpy(pdu_exp_sn131072_len18->msg, pdu5, PDU5_LEN);
  pdu_exp_sn131072_len18->N_bytes = PDU5_LEN;
  TESTASSERT(test_tx(131073, srslte::PDCP_SN_LEN_18, std::move(pdu_exp_sn131072_len18), pool, log) == 0);

  /*
   * TX Test 6: PDCP Entity with SN LEN = 18
   * PDCP entity configured with EIA2 and EEA2
   * TX_NEXT = 262144.
   * Input: {0x18, 0xE2}
   * Output: PDCP Header {0x80, 0x00, 0x00}, Ciphered Text {0xc2, 0x47}, MAC-I {0xa8, 0xdd, 0xc0, 0x73}
   */
  srslte::unique_byte_buffer_t pdu_exp_sn262144_len18 = allocate_unique_buffer(*pool);
  memcpy(pdu_exp_sn262144_len18->msg, pdu6, PDU6_LEN);
  pdu_exp_sn262144_len18->N_bytes = PDU6_LEN;
  TESTASSERT(test_tx(262145, srslte::PDCP_SN_LEN_18, std::move(pdu_exp_sn262144_len18), pool, log) == 0);
  return 0;
}

/*
 * RX Test: PDCP Entity with SN LEN = 12 and 18. Tested 4097 packets received without losses.
 * PDCP entity configured with EIA2 and EEA2
 */
int test_rx_in_sequence(uint64_t n_packets, uint8_t pdcp_sn_len, srslte::byte_buffer_pool* pool, srslte::log* log)
{
  srslte::pdcp_entity_nr pdcp_tx;
  srslte::pdcp_entity_nr pdcp_rx;
  srslte::pdcp_config_t  cfg_tx = {
      1, srslte::PDCP_RB_IS_DRB, srslte::SECURITY_DIRECTION_UPLINK, srslte::SECURITY_DIRECTION_DOWNLINK, pdcp_sn_len};
  srslte::pdcp_config_t cfg_rx = {
      1, srslte::PDCP_RB_IS_DRB, srslte::SECURITY_DIRECTION_DOWNLINK, srslte::SECURITY_DIRECTION_UPLINK, pdcp_sn_len};

  rlc_dummy rlc_tx(log);
  rrc_dummy rrc_tx(log);
  gw_dummy  gw_tx(log);

  rlc_dummy rlc_rx(log);
  rrc_dummy rrc_rx(log);
  gw_dummy  gw_rx(log);

  pdcp_tx.init(&rlc_tx, &rrc_tx, &gw_tx, log, 0, cfg_tx);
  pdcp_tx.config_security(
      k_enc, k_int, k_enc, k_int, srslte::CIPHERING_ALGORITHM_ID_128_EEA2, srslte::INTEGRITY_ALGORITHM_ID_128_EIA2);
  pdcp_tx.enable_integrity();
  pdcp_tx.enable_encryption();

  pdcp_rx.init(&rlc_rx, &rrc_rx, &gw_rx, log, 0, cfg_rx);
  pdcp_rx.config_security(
      k_enc, k_int, k_enc, k_int, srslte::CIPHERING_ALGORITHM_ID_128_EEA2, srslte::INTEGRITY_ALGORITHM_ID_128_EIA2);
  pdcp_rx.enable_integrity();
  pdcp_rx.enable_encryption();

  srslte::unique_byte_buffer_t sdu_act = allocate_unique_buffer(*pool);
  srslte::unique_byte_buffer_t sdu_exp = allocate_unique_buffer(*pool);
  memcpy(sdu_exp->msg, sdu1, SDU1_LEN);
  sdu_exp->N_bytes = SDU1_LEN;

  // Generate test message and
  // decript and check matching SDUs
  for (uint64_t i = 0; i < n_packets; ++i) {
    srslte::unique_byte_buffer_t sdu = allocate_unique_buffer(*pool);
    srslte::unique_byte_buffer_t pdu = allocate_unique_buffer(*pool);
    memcpy(sdu->msg, sdu_exp->msg, SDU1_LEN);
    sdu->N_bytes = SDU1_LEN;

    // Generate encripted and integrity protected PDU
    pdcp_tx.write_sdu(std::move(sdu), true);
    rlc_tx.get_last_sdu(pdu);
    pdcp_rx.write_pdu(std::move(pdu));
    gw_rx.get_last_pdu(sdu_act);

    TESTASSERT(sdu_exp->N_bytes == sdu_act->N_bytes);
    for (uint32_t j = 0; j < sdu_act->N_bytes; ++j) {
      TESTASSERT(sdu_exp->msg[j] == sdu_act->msg[j]);
    }
  }
  return 0;
}

// Setup all tests
int run_all_tests(srslte::byte_buffer_pool* pool)
{
  // Setup log
  srslte::log_filter log("PDCP NR Test");
  log.set_level(srslte::LOG_LEVEL_DEBUG);
  log.set_hex_limit(128);

  TESTASSERT(test_tx_all(pool, &log) == 0);
  TESTASSERT(test_rx_in_sequence(4097, srslte::PDCP_SN_LEN_12, pool, &log) == 0);
  // TESTASSERT(test_rx_in_sequence(4294967297, srslte::PDCP_SN_LEN_12, pool, &log) == 0);
  TESTASSERT(test_rx_in_sequence(262145, srslte::PDCP_SN_LEN_18, pool, &log) == 0);
  return 0;
}

int main(int argc, char** argv)
{
  run_all_tests(srslte::byte_buffer_pool::get_instance());
  srslte::byte_buffer_pool::cleanup();
}
