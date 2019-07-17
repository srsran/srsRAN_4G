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

// Encription and Integrity Keys
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

// fake classes
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
  srslte::log* log;
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

  void write_pdu_mch(uint32_t lcid, srslte::unique_byte_buffer_t pdu) {}
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
  srslte::log* log;
  srslte::unique_byte_buffer_t last_pdu;
};

/*
 * Test 1: PDCP Entity TX
 * Configure PDCP entity with EIA2 and EEA2
 * TX_NEXT initially at 0.
 * Input: {0x18, 0xE2}
 * Output: PDCP Header {0x80,0x00}, Ciphered Text {0x8f, 0xe3}, MAC-I {0xe0, 0xdf, 0x82, 0x92}
 */
int test_tx_basic(srslte::byte_buffer_pool* pool, srslte::log* log)
{
  srslte::pdcp_entity_nr pdcp;
  srslte::srslte_pdcp_config_t cfg = {1, srslte::PDCP_RB_IS_DRB, SECURITY_DIRECTION_UPLINK, SECURITY_DIRECTION_DOWNLINK, srslte::PDCP_SN_LEN_12};

  rlc_dummy rlc(log);
  rrc_dummy rrc(log);
  gw_dummy gw(log);

  pdcp.init(&rlc, &rrc, &gw, log, 0, cfg);
  pdcp.config_security(k_enc, k_int, k_enc, k_int, srslte::CIPHERING_ALGORITHM_ID_128_EEA2, srslte::INTEGRITY_ALGORITHM_ID_128_EIA2);
  pdcp.enable_integrity();
  pdcp.enable_encryption();

  // Test SDU
  srslte::unique_byte_buffer_t sdu = allocate_unique_buffer(*pool);
  memcpy(sdu->msg, sdu1, SDU1_LEN);
  sdu->N_bytes = SDU1_LEN;

  // Expected PDCP PDU
  srslte::unique_byte_buffer_t pdu_exp = allocate_unique_buffer(*pool);
  memcpy(pdu_exp->msg, pdu1, PDU1_LEN);
  pdu_exp->N_bytes = PDU1_LEN;

  // Run test
  pdcp.write_sdu(std::move(sdu), true);
  srslte::unique_byte_buffer_t pdu_act = allocate_unique_buffer(*pool);
  rlc.get_last_sdu(pdu_act);

  TESTASSERT(pdu_act->N_bytes == pdu_exp->N_bytes);
  for (uint32_t i = 0; i < pdu_exp->N_bytes; ++i) {
    TESTASSERT(pdu_act->msg[i] == pdu_exp->msg[i]);
  }
  return 0;
}

/*
 * Test 2: PDCP Entity RX
 * Configure PDCP entity with EIA2 and EEA2
 * TX_NEXT initially at 0.
 * Input: {0x80, 0x00, 0x8f, 0xe3, 0xe0, 0xdf 0x82, 0x92}
 * Output: {0x18, 0xE2}
 */
bool test_rx_basic(srslte::byte_buffer_pool* pool, srslte::log* log)
{
  srslte::pdcp_entity_nr pdcp;
  srslte::srslte_pdcp_config_t cfg = {1, srslte::PDCP_RB_IS_DRB, SECURITY_DIRECTION_DOWNLINK, SECURITY_DIRECTION_UPLINK, srslte::PDCP_SN_LEN_12};

  rlc_dummy rlc(log);
  rrc_dummy rrc(log);
  gw_dummy  gw(log);

  pdcp.init(&rlc, &rrc, &gw, log, 0, cfg);
  pdcp.config_security(k_enc, k_int, k_enc, k_int, srslte::CIPHERING_ALGORITHM_ID_128_EEA2, srslte::INTEGRITY_ALGORITHM_ID_128_EIA2);
  pdcp.enable_integrity();
  pdcp.enable_encryption();

  uint8_t mac_exp[4];
  srslte::unique_byte_buffer_t pdu     = allocate_unique_buffer(*pool);
  srslte::unique_byte_buffer_t sdu_exp = allocate_unique_buffer(*pool);
  memcpy(pdu->msg, pdu1, PDU1_LEN);
  pdu->N_bytes = PDU1_LEN;
  memcpy(sdu_exp->msg, sdu1, PDU1_LEN);
  sdu_exp->N_bytes = SDU1_LEN;


  // Run test
  pdcp.write_pdu(std::move(pdu));
  
  // Check integrity check OK
  TESTASSERT(gw.rx_count == 1); 

  // Check decrypting OK
  srslte::unique_byte_buffer_t sdu_act = allocate_unique_buffer(*pool);
  gw.get_last_pdu(sdu_act);
  TESTASSERT(sdu_act->N_bytes == sdu_exp->N_bytes);
  for (uint32_t i = 0; i < sdu_exp->N_bytes; ++i) {
    TESTASSERT(sdu_act->msg[i] == sdu_exp->msg[i]);
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

  TESTASSERT(test_tx_basic(pool, &log) == 0);
  TESTASSERT(test_rx_basic(pool, &log) == 0);
  return 0;
}


int main(int argc, char** argv)
{
  run_all_tests(srslte::byte_buffer_pool::get_instance());
  srslte::byte_buffer_pool::cleanup();
}
