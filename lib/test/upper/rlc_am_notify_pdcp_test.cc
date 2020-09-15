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

#include "srslte/common/test_common.h"
#include "srslte/upper/rlc_am_lte.h"
#include <iostream>

srslte::log_ref rlc_log("RLC");

class pdcp_tester : public srsue::pdcp_interface_rlc
{
public:
  std::map<uint32_t, uint32_t> notified_counts; // Map of PDCP Tx count to acknoledged bytes
  // PDCP interface
  void write_pdu(uint32_t lcid, srslte::unique_byte_buffer_t sdu) {}
  void write_pdu_bcch_bch(srslte::unique_byte_buffer_t sdu) {}
  void write_pdu_bcch_dlsch(srslte::unique_byte_buffer_t sdu) {}
  void write_pdu_pcch(srslte::unique_byte_buffer_t sdu) {}
  void write_pdu_mch(uint32_t lcid, srslte::unique_byte_buffer_t pdu) {}
  void notify_delivery(uint32_t lcid, const std::vector<uint32_t>& pdcp_sn_vec)
  {
    for (uint32_t pdcp_sn : pdcp_sn_vec) {
      if (notified_counts.find(pdcp_sn) == notified_counts.end()) {
        notified_counts[pdcp_sn] = 0;
      }
      assert(lcid == 1);
      notified_counts[pdcp_sn] += 1;
      // std::cout << "Notified " << notified_counts[tx_count] << "Tx count" << tx_count << "nof_bytes" << nof_bytes
      //          << std::endl;
    }
  }
};

class rrc_tester : public srsue::rrc_interface_rlc
{
  void        max_retx_attempted() {}
  std::string get_rb_name(uint32_t lcid) { return "DRB1"; }
  void        write_pdu(uint32_t lcid, srslte::unique_byte_buffer_t pdu) {}
};

// Simple test of a single TX PDU and an imediate ACK
int simple_sdu_notify_test()
{

  srslte::byte_buffer_pool* pool = srslte::byte_buffer_pool::get_instance();
  pdcp_tester               pdcp;
  rrc_tester                rrc;
  srslte::timer_handler     timers(8);
  srslte::rlc_am_lte        rlc(rlc_log, 1, &pdcp, &rrc, &timers);

  uint8_t  sdu[]   = {0x22, 0x40};
  uint32_t SDU_LEN = 2;

  srslte::unique_byte_buffer_t sdu_buf = allocate_unique_buffer(*pool);
  srslte::unique_byte_buffer_t pdu_buf = allocate_unique_buffer(*pool);
  srslte::unique_byte_buffer_t sta_buf = allocate_unique_buffer(*pool);

  if (not rlc.configure(srslte::rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  // Write SDU into RLC entity
  memcpy(sdu_buf->msg, &sdu[0], SDU_LEN);
  sdu_buf->N_bytes    = SDU_LEN;
  sdu_buf->md.pdcp_sn = 10;
  rlc.write_sdu(std::move(sdu_buf));

  TESTASSERT(4 == rlc.get_buffer_state()); // 2 bytes for header + 2 bytes for payload

  // Read 1 PDUs from RLC1 (4 bytes)
  uint32_t len     = rlc.read_pdu(pdu_buf->msg, 4);
  pdu_buf->N_bytes = len;
  TESTASSERT(0 == rlc.get_buffer_state());

  // Feed ack to PDU
  srslte::rlc_status_pdu_t s1;
  s1.ack_sn = 1;
  s1.N_nack = 0;

  sta_buf->N_bytes = srslte::rlc_am_write_status_pdu(&s1, sta_buf->msg);
  rlc.write_pdu(sta_buf->msg, sta_buf->N_bytes);

  // Check PDCP notifications
  TESTASSERT(pdcp.notified_counts.size() == 1);
  TESTASSERT(pdcp.notified_counts.find(10) != pdcp.notified_counts.end());
  TESTASSERT(pdcp.notified_counts[10] == 1);

  return SRSLTE_SUCCESS;
}

// Test of a single SDU transmitted over 2 PDUs.
// Both PDUs are ACKed in the same status PDU.
int two_pdus_notify_test()
{

  srslte::byte_buffer_pool* pool = srslte::byte_buffer_pool::get_instance();
  pdcp_tester               pdcp;
  rrc_tester                rrc;
  srslte::timer_handler     timers(8);
  srslte::rlc_am_lte        rlc(rlc_log, 1, &pdcp, &rrc, &timers);

  uint8_t  sdu[]   = {0x22, 0x40, 0x30, 0x21, 0x50};
  uint32_t SDU_LEN = 5;

  srslte::unique_byte_buffer_t sdu_buf  = allocate_unique_buffer(*pool);
  srslte::unique_byte_buffer_t pdu1_buf = allocate_unique_buffer(*pool);
  srslte::unique_byte_buffer_t pdu2_buf = allocate_unique_buffer(*pool);
  srslte::unique_byte_buffer_t sta_buf  = allocate_unique_buffer(*pool);

  if (not rlc.configure(srslte::rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  // Write SDU into RLC entity
  memcpy(sdu_buf->msg, &sdu[0], SDU_LEN);
  sdu_buf->N_bytes    = SDU_LEN;
  sdu_buf->md.pdcp_sn = 10;
  rlc.write_sdu(std::move(sdu_buf));

  TESTASSERT(7 == rlc.get_buffer_state()); // 2 bytes for header + 5 for payload

  // Read first PDU from RLC1 (3 bytes of data)
  pdu1_buf->N_bytes = rlc.read_pdu(pdu1_buf->msg, 5); // Read
  TESTASSERT(4 == rlc.get_buffer_state());            // 2 bytes for header + 2 for payload

  // Read second PDUs from RLC1 (3 bytes of data)
  pdu2_buf->N_bytes = rlc.read_pdu(pdu2_buf->msg, 4); // 2 bytes for header + 2 for payload
  TESTASSERT(0 == rlc.get_buffer_state());

  // Feed ack of PDU1 to RLC
  srslte::rlc_status_pdu_t s1;
  s1.ack_sn = 1;
  s1.N_nack = 0;

  sta_buf->N_bytes = srslte::rlc_am_write_status_pdu(&s1, sta_buf->msg);
  rlc.write_pdu(sta_buf->msg, sta_buf->N_bytes);

  // Feed ack of PDU2 to RLC
  srslte::rlc_status_pdu_t s2;
  s2.ack_sn = 2;
  s2.N_nack = 0;

  sta_buf->N_bytes = srslte::rlc_am_write_status_pdu(&s2, sta_buf->msg);
  rlc.write_pdu(sta_buf->msg, sta_buf->N_bytes);

  // Check PDCP notifications
  TESTASSERT(pdcp.notified_counts.size() == 1);
  TESTASSERT(pdcp.notified_counts.find(10) != pdcp.notified_counts.end());
  TESTASSERT(pdcp.notified_counts[10] == 1);

  return SRSLTE_SUCCESS;
}

// Test of a two SDUs transmitted over a single PDU.
// Two SDUs -> K=1 (even number of LIs)
// The PDU is ACKed imediatly.
int two_sdus_notify_test()
{

  srslte::byte_buffer_pool* pool = srslte::byte_buffer_pool::get_instance();
  pdcp_tester               pdcp;
  rrc_tester                rrc;
  srslte::timer_handler     timers(8);
  srslte::rlc_am_lte        rlc(rlc_log, 1, &pdcp, &rrc, &timers);

  uint8_t  sdu1[]   = {0x22, 0x40, 0x30, 0x21, 0x50};
  uint32_t SDU1_LEN = 5;

  uint8_t                      sdu2[]   = {0x22, 0x40};
  uint32_t                     SDU2_LEN = 2;
  srslte::unique_byte_buffer_t sdu1_buf = allocate_unique_buffer(*pool);
  srslte::unique_byte_buffer_t sdu2_buf = allocate_unique_buffer(*pool);
  srslte::unique_byte_buffer_t pdu_buf  = allocate_unique_buffer(*pool);
  srslte::unique_byte_buffer_t sta_buf  = allocate_unique_buffer(*pool);

  if (not rlc.configure(srslte::rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  // Write SDU into RLC entity
  memcpy(sdu1_buf->msg, &sdu1[0], SDU1_LEN);
  sdu1_buf->N_bytes    = SDU1_LEN;
  sdu1_buf->md.pdcp_sn = 10;
  rlc.write_sdu(std::move(sdu1_buf));
  TESTASSERT(7 == rlc.get_buffer_state());

  memcpy(sdu2_buf->msg, &sdu2[0], SDU2_LEN);
  sdu2_buf->N_bytes    = SDU2_LEN;
  sdu2_buf->md.pdcp_sn = 13;
  rlc.write_sdu(std::move(sdu2_buf));
  TESTASSERT(11 == rlc.get_buffer_state()); // 2 bytes for header, 2 bytes for Li, and 7 for data

  // Read PDU from RLC1 (7 bytes of data)
  pdu_buf->N_bytes = rlc.read_pdu(pdu_buf->msg, 11); // 2 bytes for fixed header + 2 bytes for one LI
                                                     // + 7 bytes for payload

  TESTASSERT(0 == rlc.get_buffer_state());

  // Feed ack of PDU1 to RLC
  srslte::rlc_status_pdu_t s1;
  s1.ack_sn = 1;
  s1.N_nack = 0;

  sta_buf->N_bytes = srslte::rlc_am_write_status_pdu(&s1, sta_buf->msg);
  rlc.write_pdu(sta_buf->msg, sta_buf->N_bytes);

  // Check PDCP notifications
  TESTASSERT(pdcp.notified_counts.size() == 2);
  TESTASSERT(pdcp.notified_counts.find(10) != pdcp.notified_counts.end());
  TESTASSERT(pdcp.notified_counts[10] == 1);
  TESTASSERT(pdcp.notified_counts.find(13) != pdcp.notified_counts.end());
  TESTASSERT(pdcp.notified_counts[13] == 1);

  return SRSLTE_SUCCESS;
}

// Test of a three SDUs transmitted over a single PDU.
// The PDU is ACKed imediatly.
int three_sdus_notify_test()
{

  srslte::byte_buffer_pool* pool = srslte::byte_buffer_pool::get_instance();
  pdcp_tester               pdcp;
  rrc_tester                rrc;
  srslte::timer_handler     timers(8);
  srslte::rlc_am_lte        rlc(rlc_log, 1, &pdcp, &rrc, &timers);

  uint8_t  sdu1[]   = {0x22, 0x40, 0x30, 0x21, 0x50};
  uint32_t SDU1_LEN = 5;

  uint8_t  sdu2[]   = {0x22, 0x40};
  uint32_t SDU2_LEN = 2;

  uint8_t  sdu3[]   = {0x22, 0x40, 0x00};
  uint32_t SDU3_LEN = 3;

  srslte::unique_byte_buffer_t sdu1_buf = allocate_unique_buffer(*pool);
  srslte::unique_byte_buffer_t sdu2_buf = allocate_unique_buffer(*pool);
  srslte::unique_byte_buffer_t sdu3_buf = allocate_unique_buffer(*pool);
  srslte::unique_byte_buffer_t pdu_buf  = allocate_unique_buffer(*pool);
  srslte::unique_byte_buffer_t sta_buf  = allocate_unique_buffer(*pool);

  if (not rlc.configure(srslte::rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  // Write SDU into RLC entity
  memcpy(sdu1_buf->msg, &sdu1[0], SDU1_LEN);
  sdu1_buf->N_bytes    = SDU1_LEN;
  sdu1_buf->md.pdcp_sn = 10;
  rlc.write_sdu(std::move(sdu1_buf));
  TESTASSERT(7 == rlc.get_buffer_state());

  memcpy(sdu2_buf->msg, &sdu2[0], SDU2_LEN);
  sdu2_buf->N_bytes    = SDU2_LEN;
  sdu2_buf->md.pdcp_sn = 13;
  rlc.write_sdu(std::move(sdu2_buf));

  memcpy(sdu3_buf->msg, &sdu3[0], SDU3_LEN);
  sdu3_buf->N_bytes    = SDU3_LEN;
  sdu3_buf->md.pdcp_sn = 14;
  rlc.write_sdu(std::move(sdu3_buf));

  TESTASSERT(15 == rlc.get_buffer_state()); // 2 bytes for fixed header, 3 bytes for two LIs, 10 for data

  // Read PDU from RLC1 (10 bytes of data)
  pdu_buf->N_bytes = rlc.read_pdu(pdu_buf->msg, 15); // 2 bytes for fixed header + 5 bytes for three LIs
                                                     // + 10 bytes for payload

  TESTASSERT(0 == rlc.get_buffer_state());

  // Feed ack of PDU1 to RLC
  srslte::rlc_status_pdu_t s1;
  s1.ack_sn = 1;
  s1.N_nack = 0;

  sta_buf->N_bytes = srslte::rlc_am_write_status_pdu(&s1, sta_buf->msg);
  rlc.write_pdu(sta_buf->msg, sta_buf->N_bytes);

  // Check PDCP notifications
  TESTASSERT(pdcp.notified_counts.size() == 3);
  TESTASSERT(pdcp.notified_counts.find(10) != pdcp.notified_counts.end());
  TESTASSERT(pdcp.notified_counts.find(13) != pdcp.notified_counts.end());
  TESTASSERT(pdcp.notified_counts.find(14) != pdcp.notified_counts.end());
  TESTASSERT(pdcp.notified_counts[10] == 1);
  TESTASSERT(pdcp.notified_counts[13] == 1);
  TESTASSERT(pdcp.notified_counts[14] == 1);

  return SRSLTE_SUCCESS;
}
// Test notification of an RTXED SDU.
int rtxed_sdu_notify_test()
{

  srslte::byte_buffer_pool* pool = srslte::byte_buffer_pool::get_instance();
  pdcp_tester               pdcp;
  rrc_tester                rrc;
  srslte::timer_handler     timers(8);
  srslte::rlc_am_lte        rlc(rlc_log, 1, &pdcp, &rrc, &timers);

  uint8_t  sdu[]   = {0x22, 0x40, 0x30, 0x21, 0x50};
  uint32_t SDU_LEN = 5;

  srslte::unique_byte_buffer_t sdu_buf = allocate_unique_buffer(*pool);
  srslte::unique_byte_buffer_t pdu_buf = allocate_unique_buffer(*pool);
  srslte::unique_byte_buffer_t sta_buf = allocate_unique_buffer(*pool);

  if (not rlc.configure(srslte::rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  // Write SDU into RLC entity
  memcpy(sdu_buf->msg, &sdu[0], SDU_LEN);
  sdu_buf->N_bytes    = SDU_LEN;
  sdu_buf->md.pdcp_sn = 10;
  rlc.write_sdu(std::move(sdu_buf));
  TESTASSERT(7 == rlc.get_buffer_state());

  // Read first PDU from RLC (5 bytes of data)
  pdu_buf->N_bytes = rlc.read_pdu(pdu_buf->msg, 7); // 2 bytes for fixed header + 5 bytes for payload
  TESTASSERT(0 == rlc.get_buffer_state());

  // Feed Nack of PDU1 to RLC
  srslte::rlc_status_pdu_t s1;
  s1.ack_sn           = 1;
  s1.N_nack           = 1;
  s1.nacks[0].nack_sn = 0;

  sta_buf->N_bytes = srslte::rlc_am_write_status_pdu(&s1, sta_buf->msg);
  rlc.write_pdu(sta_buf->msg, sta_buf->N_bytes);
  TESTASSERT(7 == rlc.get_buffer_state());

  // Check PDCP notifications
  TESTASSERT(pdcp.notified_counts.empty());

  // Read rtxed PDU from RLC (5 bytes of data)
  pdu_buf->N_bytes = rlc.read_pdu(pdu_buf->msg, 7); // 2 bytes for fixed header + 5 bytes for payload
  TESTASSERT(0 == rlc.get_buffer_state());

  // Feed ack of PDU1 to RLC
  srslte::rlc_status_pdu_t s2;
  s2.ack_sn        = 1;
  s2.N_nack        = 0;
  sta_buf->N_bytes = srslte::rlc_am_write_status_pdu(&s2, sta_buf->msg);
  rlc.write_pdu(sta_buf->msg, sta_buf->N_bytes);
  TESTASSERT(0 == rlc.get_buffer_state());

  // Check PDCP notifications
  TESTASSERT(pdcp.notified_counts.size() == 1);
  TESTASSERT(pdcp.notified_counts.find(10) != pdcp.notified_counts.end());
  TESTASSERT(pdcp.notified_counts[10] == 1);

  return SRSLTE_SUCCESS;
}

// Test out of order ACK for SDU.
// Two sdus are transmitted, and ack arrives out of order
int two_sdus_out_of_order_ack_notify_test()
{
  srslte::byte_buffer_pool* pool = srslte::byte_buffer_pool::get_instance();
  pdcp_tester               pdcp;
  rrc_tester                rrc;
  srslte::timer_handler     timers(8);
  srslte::rlc_am_lte        rlc(rlc_log, 1, &pdcp, &rrc, &timers);

  uint8_t  sdu[]   = {0x22, 0x40, 0x30, 0x21, 0x50};
  uint32_t SDU_LEN = 5;

  srslte::unique_byte_buffer_t sdu_buf  = allocate_unique_buffer(*pool);
  srslte::unique_byte_buffer_t pdu1_buf = allocate_unique_buffer(*pool);
  srslte::unique_byte_buffer_t pdu2_buf = allocate_unique_buffer(*pool);
  srslte::unique_byte_buffer_t sta_buf  = allocate_unique_buffer(*pool);

  if (not rlc.configure(srslte::rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  // Write SDU into RLC entity
  memcpy(sdu_buf->msg, &sdu[0], SDU_LEN);
  sdu_buf->N_bytes    = SDU_LEN;
  sdu_buf->md.pdcp_sn = 10;
  rlc.write_sdu(std::move(sdu_buf));

  TESTASSERT(7 == rlc.get_buffer_state()); // 2 bytes for header + 5 for payload

  // Read first PDU from RLC1 (3 bytes of data)
  pdu1_buf->N_bytes = rlc.read_pdu(pdu1_buf->msg, 5); // Read
  TESTASSERT(4 == rlc.get_buffer_state());            // 2 bytes for header + 2 for payload

  // Read second PDUs from RLC1 (3 bytes of data)
  pdu2_buf->N_bytes = rlc.read_pdu(pdu2_buf->msg, 4); // 2 bytes for header + 2 for payload
  TESTASSERT(0 == rlc.get_buffer_state());

  // Feed ack of PDU1 to RLC
  srslte::rlc_status_pdu_t s1;
  s1.ack_sn = 1;
  s1.N_nack = 0;

  // Intentionally do not write first ack to RLC

  // Feed ack of PDU2 to RLC
  srslte::rlc_status_pdu_t s2;
  s2.ack_sn = 2;
  s2.N_nack = 0;

  // Write second ack
  sta_buf->N_bytes = srslte::rlc_am_write_status_pdu(&s2, sta_buf->msg);
  rlc.write_pdu(sta_buf->msg, sta_buf->N_bytes);

  // Check PDCP notifications
  TESTASSERT(pdcp.notified_counts.size() == 1);
  TESTASSERT(pdcp.notified_counts.find(10) != pdcp.notified_counts.end());
  TESTASSERT(pdcp.notified_counts[10] == 1);

  // Write first ack (out of order)
  sta_buf->N_bytes = srslte::rlc_am_write_status_pdu(&s1, sta_buf->msg);
  rlc.write_pdu(sta_buf->msg, sta_buf->N_bytes);

  // Check PDCP notifications
  TESTASSERT(pdcp.notified_counts.size() == 1);
  TESTASSERT(pdcp.notified_counts.find(10) != pdcp.notified_counts.end());
  TESTASSERT(pdcp.notified_counts[10] == 1);
  return SRSLTE_SUCCESS;
}

// Test out-of-order ack of a single SDU transmitted over 2 PDUs.
int two_pdus_out_of_order_ack_notify_test()
{

  srslte::byte_buffer_pool* pool = srslte::byte_buffer_pool::get_instance();
  pdcp_tester               pdcp;
  rrc_tester                rrc;
  srslte::timer_handler     timers(8);
  srslte::rlc_am_lte        rlc(rlc_log, 1, &pdcp, &rrc, &timers);

  uint8_t  sdu[]   = {0x22, 0x40, 0x30, 0x21, 0x50};
  uint32_t SDU_LEN = 5;

  srslte::unique_byte_buffer_t sdu_buf  = allocate_unique_buffer(*pool);
  srslte::unique_byte_buffer_t pdu1_buf = allocate_unique_buffer(*pool);
  srslte::unique_byte_buffer_t pdu2_buf = allocate_unique_buffer(*pool);
  srslte::unique_byte_buffer_t sta_buf  = allocate_unique_buffer(*pool);

  if (not rlc.configure(srslte::rlc_config_t::default_rlc_am_config())) {
    return -1;
  }

  // Write SDU into RLC entity
  memcpy(sdu_buf->msg, &sdu[0], SDU_LEN);
  sdu_buf->N_bytes    = SDU_LEN;
  sdu_buf->md.pdcp_sn = 10;
  rlc.write_sdu(std::move(sdu_buf));

  TESTASSERT(7 == rlc.get_buffer_state()); // 2 bytes for header + 5 for payload

  // Read first PDU from RLC1 (3 bytes of data)
  pdu1_buf->N_bytes = rlc.read_pdu(pdu1_buf->msg, 5); // Read
  TESTASSERT(4 == rlc.get_buffer_state());            // 2 bytes for header + 2 for payload

  // Read second PDUs from RLC1 (3 bytes of data)
  pdu2_buf->N_bytes = rlc.read_pdu(pdu2_buf->msg, 4); // 2 bytes for header + 2 for payload
  TESTASSERT(0 == rlc.get_buffer_state());

  // Feed ack of PDU1 to RLC
  srslte::rlc_status_pdu_t s1;
  s1.ack_sn = 1;
  s1.N_nack = 0;

  sta_buf->N_bytes = srslte::rlc_am_write_status_pdu(&s1, sta_buf->msg);
  rlc.write_pdu(sta_buf->msg, sta_buf->N_bytes);

  // Feed ack of PDU2 to RLC
  srslte::rlc_status_pdu_t s2;
  s2.ack_sn = 2;
  s2.N_nack = 0;

  sta_buf->N_bytes = srslte::rlc_am_write_status_pdu(&s2, sta_buf->msg);
  rlc.write_pdu(sta_buf->msg, sta_buf->N_bytes);

  // Check PDCP notifications
  TESTASSERT(pdcp.notified_counts.size() == 1);
  TESTASSERT(pdcp.notified_counts.find(10) != pdcp.notified_counts.end());
  TESTASSERT(pdcp.notified_counts[10] == 1);

  return SRSLTE_SUCCESS;
}
int main(int argc, char** argv)
{
  srslte::byte_buffer_pool::get_instance();
  TESTASSERT(simple_sdu_notify_test() == SRSLTE_SUCCESS);
  TESTASSERT(two_pdus_notify_test() == SRSLTE_SUCCESS);
  TESTASSERT(two_sdus_notify_test() == SRSLTE_SUCCESS);
  TESTASSERT(three_sdus_notify_test() == SRSLTE_SUCCESS);
  TESTASSERT(rtxed_sdu_notify_test() == SRSLTE_SUCCESS);
  TESTASSERT(two_sdus_out_of_order_ack_notify_test() == SRSLTE_SUCCESS);
  srslte::byte_buffer_pool::cleanup();
  return SRSLTE_SUCCESS;
}
