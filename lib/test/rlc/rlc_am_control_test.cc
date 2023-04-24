/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#include "srsran/common/test_common.h"
#include "srsran/rlc/rlc_am_lte.h"
#include <iostream>

// Simple status PDU
int simple_status_pdu_test1()
{
  uint8_t  pdu1[]   = {0x00, 0x78};
  uint32_t PDU1_LEN = 2;

  srsran::rlc_status_pdu_t s1;
  srsran::byte_buffer_t    b1, b2;

  memcpy(b1.msg, &pdu1[0], PDU1_LEN);
  b1.N_bytes = PDU1_LEN;
  rlc_am_read_status_pdu(&b1, &s1);
  TESTASSERT(s1.ack_sn == 30);
  TESTASSERT(s1.N_nack == 0);
  rlc_am_write_status_pdu(&s1, &b2);
  TESTASSERT(b2.N_bytes == PDU1_LEN);
  for (uint32_t i = 0; i < b2.N_bytes; i++) {
    TESTASSERT(b2.msg[i] == b1.msg[i]);
  }
  TESTASSERT(rlc_am_is_valid_status_pdu(s1));
  return SRSRAN_SUCCESS;
}

// Status PDU with 4 NACKs
int status_pdu_with_nacks_test1()
{
  uint8_t  pdu2[]   = {0x00, 0x22, 0x00, 0x40, 0x0C, 0x01, 0xC0, 0x20};
  uint32_t PDU2_LEN = 8;

  srsran::rlc_status_pdu_t s2;
  srsran::byte_buffer_t    b1, b2;

  memcpy(b1.msg, &pdu2[0], PDU2_LEN);
  b1.N_bytes = PDU2_LEN;
  rlc_am_read_status_pdu(&b1, &s2);
  TESTASSERT(s2.ack_sn == 8);
  TESTASSERT(s2.N_nack == 4);
  TESTASSERT(s2.nacks[0].nack_sn == 0);
  TESTASSERT(s2.nacks[1].nack_sn == 1);
  TESTASSERT(s2.nacks[2].nack_sn == 3);
  TESTASSERT(s2.nacks[3].nack_sn == 4);
  rlc_am_write_status_pdu(&s2, &b2);
  TESTASSERT(b2.N_bytes == PDU2_LEN);
  for (uint32_t i = 0; i < b2.N_bytes; i++) {
    TESTASSERT(b2.msg[i] == b1.msg[i]);
  }

  TESTASSERT(rlc_am_is_valid_status_pdu(s2));
  return SRSRAN_SUCCESS;
}

int malformed_status_pdu_test()
{
  uint8_t pdu[] = {0x0b, 0x77, 0x6d, 0xd6, 0xe5, 0x6f, 0x56, 0xf8};

  srsran::rlc_status_pdu_t s1;
  srsran::byte_buffer_t    b1, b2;

  memcpy(b1.msg, pdu, sizeof(pdu));
  b1.N_bytes = sizeof(pdu);
  rlc_am_read_status_pdu(&b1, &s1);
  TESTASSERT(rlc_am_is_valid_status_pdu(s1) == false);
  return SRSRAN_SUCCESS;
}

// Malformed PDU captured in field-test
// 22:48:03.509077 [RLC    ] [I] DRB1 Tx status PDU - ACK_SN = 205, N_nack = 98, NACK_SN =
// [752][986][109][110][111][112][113][114][115][116][117][118][119][120][121][122][123][124][125][126][127][128][129]
int malformed_status_pdu_test2()
{
  uint32_t vr_a = 293;

  // Construct a status PDU that ACKs SN 205, which is outside the rx window
  srsran::rlc_status_pdu_t status_pdu = {};
  status_pdu.ack_sn                   = 205;
  status_pdu.N_nack                   = 2;
  status_pdu.nacks[0].nack_sn         = 752;
  status_pdu.nacks[1].nack_sn         = 986;
  TESTASSERT(rlc_am_is_valid_status_pdu(status_pdu, vr_a) == false);

  // 1 SN after upper edge of Rx window will fail
  status_pdu.ack_sn = 806;
  TESTASSERT(rlc_am_is_valid_status_pdu(status_pdu, vr_a) == false);

  // The exact upper edge of Rx window should work
  status_pdu.ack_sn = 805;
  status_pdu.N_nack = 0;
  TESTASSERT(rlc_am_is_valid_status_pdu(status_pdu, vr_a) == true);

  // ACK_SN is again outside of rx_window
  vr_a              = 0;
  status_pdu.ack_sn = 742;
  status_pdu.N_nack = 0;
  TESTASSERT(rlc_am_is_valid_status_pdu(status_pdu, vr_a) == false);

  // ACK_SN is well within rx window
  vr_a              = 300;
  status_pdu.ack_sn = 742;
  status_pdu.N_nack = 0;
  TESTASSERT(rlc_am_is_valid_status_pdu(status_pdu, vr_a) == true);

  return SRSRAN_SUCCESS;
}

int main(int argc, char** argv)
{
  srslog::init();

  TESTASSERT(simple_status_pdu_test1() == SRSRAN_SUCCESS);
  TESTASSERT(status_pdu_with_nacks_test1() == SRSRAN_SUCCESS);
  TESTASSERT(malformed_status_pdu_test() == SRSRAN_SUCCESS);
  TESTASSERT(malformed_status_pdu_test2() == SRSRAN_SUCCESS);

  return SRSRAN_SUCCESS;
}
