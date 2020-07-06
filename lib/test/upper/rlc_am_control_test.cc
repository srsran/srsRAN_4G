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

// Simple status PDU
int simple_status_pdu_test1()
{
  uint8_t  pdu1[]   = {0x00, 0x78};
  uint32_t PDU1_LEN = 2;

  srslte::rlc_status_pdu_t s1;
  srslte::byte_buffer_t    b1, b2;

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
  return SRSLTE_SUCCESS;
}

// Status PDU with 4 NACKs
int status_pdu_with_nacks_test1()
{
  uint8_t  pdu2[]   = {0x00, 0x22, 0x00, 0x40, 0x0C, 0x01, 0xC0, 0x20};
  uint32_t PDU2_LEN = 8;

  srslte::rlc_status_pdu_t s2;
  srslte::byte_buffer_t    b1, b2;

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
  return SRSLTE_SUCCESS;
}

int main(int argc, char** argv)
{
  TESTASSERT(simple_status_pdu_test1() == SRSLTE_SUCCESS);
  TESTASSERT(status_pdu_with_nacks_test1() == SRSLTE_SUCCESS);

  return SRSLTE_SUCCESS;
}
