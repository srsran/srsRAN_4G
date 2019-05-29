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

#include "srslte/common/pdu.h"
#include <iostream>

#define TESTASSERT(cond)                                                                                               \
  {                                                                                                                    \
    if (!(cond)) {                                                                                                     \
      std::cout << "[" << __FUNCTION__ << "][Line " << __LINE__ << "]: FAIL at " << (#cond) << std::endl;              \
      return -1;                                                                                                       \
    }                                                                                                                  \
  }

using namespace srslte;

#define CRNTI (0x1001)

// TV1 contains a RAR PDU for a single RAPID and no backoff indication
#define RAPID_TV1 (42)
#define TA_CMD_TV1 (8)
uint8_t rar_pdu_tv1[] = {0x6a, 0x00, 0x80, 0x00, 0x0c, 0x10, 0x01};

// TV2 contains a RAR PDU for a single RAPID and also includes a backoff indication subheader
#define RAPID_TV2 (22)
#define BACKOFF_IND_TV2 (2)
#define TA_CMD_TV2 (0)
uint8_t rar_pdu_tv2[] = {0x82, 0x56, 0x00, 0x00, 0x00, 0x0c, 0x10, 0x01};

int mac_rar_pdu_unpack_test1()
{
  srslte::rar_pdu rar_pdu_msg;
  rar_pdu_msg.init_rx(sizeof(rar_pdu_tv1));
  rar_pdu_msg.parse_packet(rar_pdu_tv1);
  rar_pdu_msg.fprint(stdout);

  TESTASSERT(not rar_pdu_msg.has_backoff());
  while (rar_pdu_msg.next()) {
    TESTASSERT(rar_pdu_msg.get()->get_rapid() == RAPID_TV1);
    TESTASSERT(rar_pdu_msg.get()->get_ta_cmd() == TA_CMD_TV1);
    TESTASSERT(rar_pdu_msg.get()->get_temp_crnti() == CRNTI);
  }

  return SRSLTE_SUCCESS;
}

int mac_rar_pdu_unpack_test2()
{
  srslte::rar_pdu rar_pdu_msg;
  rar_pdu_msg.init_rx(sizeof(rar_pdu_tv2));
  rar_pdu_msg.parse_packet(rar_pdu_tv2);
  rar_pdu_msg.fprint(stdout);

  TESTASSERT(rar_pdu_msg.has_backoff());
  TESTASSERT(rar_pdu_msg.get_backoff() == BACKOFF_IND_TV2);
  while (rar_pdu_msg.next()) {
    if (rar_pdu_msg.get()->has_rapid()) {
      TESTASSERT(rar_pdu_msg.get()->get_rapid() == RAPID_TV2);
      TESTASSERT(rar_pdu_msg.get()->get_ta_cmd() == TA_CMD_TV2);
      TESTASSERT(rar_pdu_msg.get()->get_temp_crnti() == CRNTI);
    }
  }

  return SRSLTE_SUCCESS;
}

int mac_rar_pdu_pack_test1()
{
  uint8_t rar_buffer[64] = {};

  // Prepare RAR grant
  uint8_t                grant_buffer[64] = {};
  srslte_dci_rar_grant_t rar_grant        = {};
  rar_grant.tpc_pusch                     = 3;
  srslte_dci_rar_pack(&rar_grant, grant_buffer);

  // Create MAC PDU and add RAR subheader
  srslte::rar_pdu rar_pdu;
  rar_pdu.init_tx(rar_buffer, 64);
  if (rar_pdu.new_subh()) {
    rar_pdu.get()->set_rapid(RAPID_TV1);
    rar_pdu.get()->set_ta_cmd(TA_CMD_TV1);
    rar_pdu.get()->set_temp_crnti(CRNTI);
    rar_pdu.get()->set_sched_grant(grant_buffer);
  }
  rar_pdu.write_packet(rar_buffer);

  // compare with TV1
  TESTASSERT(memcmp(rar_buffer, rar_pdu_tv1, sizeof(rar_pdu_tv1)) == 0);

  return SRSLTE_SUCCESS;
}

int mac_rar_pdu_pack_test2()
{
  uint8_t rar_buffer[64] = {};

  // Prepare RAR grant
  uint8_t                grant_buffer[64] = {};
  srslte_dci_rar_grant_t rar_grant        = {};
  rar_grant.tpc_pusch                     = 3;
  srslte_dci_rar_pack(&rar_grant, grant_buffer);

  // Create MAC PDU and add RAR subheader
  srslte::rar_pdu rar_pdu;
  rar_pdu.init_tx(rar_buffer, 64);
  rar_pdu.set_backoff(BACKOFF_IND_TV2);
  if (rar_pdu.new_subh()) {
    rar_pdu.get()->set_rapid(RAPID_TV2);
    rar_pdu.get()->set_ta_cmd(TA_CMD_TV2);
    rar_pdu.get()->set_temp_crnti(CRNTI);
    rar_pdu.get()->set_sched_grant(grant_buffer);
  }
  rar_pdu.write_packet(rar_buffer);

  // compare with TV2
  TESTASSERT(memcmp(rar_buffer, rar_pdu_tv2, sizeof(rar_pdu_tv2)) == 0);

  return SRSLTE_SUCCESS;
}

int main(int argc, char** argv)
{
  if (mac_rar_pdu_unpack_test1()) {
    fprintf(stderr, "mac_rar_pdu_unpack_test1 failed.\n");
    return SRSLTE_ERROR;
  }

  if (mac_rar_pdu_unpack_test2()) {
    fprintf(stderr, "mac_rar_pdu_unpack_test2 failed.\n");
    return SRSLTE_ERROR;
  }

  if (mac_rar_pdu_pack_test1()) {
    fprintf(stderr, "mac_rar_pdu_pack_test1 failed.\n");
    return SRSLTE_ERROR;
  }

  if (mac_rar_pdu_pack_test2()) {
    fprintf(stderr, "mac_rar_pdu_pack_test2 failed.\n");
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}
