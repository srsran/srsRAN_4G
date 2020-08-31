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

#include "srslte/common/common.h"
#include "srslte/common/interfaces_common.h"
#include "srslte/common/logmap.h"
#include "srslte/common/mac_pcap.h"
#include "srslte/common/test_common.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "srslte/mac/pdu.h"
#include <bitset>
#include <iostream>
#include <map>
#include <random>

#define HAVE_PCAP 0

std::random_device                     rd;
std::mt19937                           rand_gen(rd());
std::uniform_int_distribution<uint8_t> uniform_dist_u8(0, 255);

static std::unique_ptr<srslte::mac_pcap> pcap_handle = nullptr;

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
  std::cout << rar_pdu_msg.to_string() << std::endl;

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
  std::cout << rar_pdu_msg.to_string() << std::endl;

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
  // Prepare RAR grant
  uint8_t                grant_buffer[64] = {};
  srslte_dci_rar_grant_t rar_grant        = {};
  rar_grant.tpc_pusch                     = 3;
  srslte_dci_rar_pack(&rar_grant, grant_buffer);

  // Create MAC PDU and add RAR subheader
  srslte::rar_pdu rar_pdu;

  byte_buffer_t tx_buffer;
  rar_pdu.init_tx(&tx_buffer, 64);
  if (rar_pdu.new_subh()) {
    rar_pdu.get()->set_rapid(RAPID_TV1);
    rar_pdu.get()->set_ta_cmd(TA_CMD_TV1);
    rar_pdu.get()->set_temp_crnti(CRNTI);
    rar_pdu.get()->set_sched_grant(grant_buffer);
  }
  rar_pdu.write_packet(tx_buffer.msg);

  // compare with TV1
  TESTASSERT(memcmp(tx_buffer.msg, rar_pdu_tv1, sizeof(rar_pdu_tv1)) == 0);

  return SRSLTE_SUCCESS;
}

int mac_rar_pdu_pack_test2()
{
  // Prepare RAR grant
  uint8_t                grant_buffer[64] = {};
  srslte_dci_rar_grant_t rar_grant        = {};
  rar_grant.tpc_pusch                     = 3;
  srslte_dci_rar_pack(&rar_grant, grant_buffer);

  // Create MAC PDU and add RAR subheader
  srslte::rar_pdu rar_pdu;
  byte_buffer_t   tx_buffer;
  rar_pdu.init_tx(&tx_buffer, 64);
  rar_pdu.set_backoff(BACKOFF_IND_TV2);
  if (rar_pdu.new_subh()) {
    rar_pdu.get()->set_rapid(RAPID_TV2);
    rar_pdu.get()->set_ta_cmd(TA_CMD_TV2);
    rar_pdu.get()->set_temp_crnti(CRNTI);
    rar_pdu.get()->set_sched_grant(grant_buffer);
  }
  rar_pdu.write_packet(tx_buffer.msg);

  // compare with TV2
  TESTASSERT(memcmp(tx_buffer.msg, rar_pdu_tv2, sizeof(rar_pdu_tv2)) == 0);

  return SRSLTE_SUCCESS;
}

// Helper class to provide read_pdu_interface
class rlc_dummy : public srslte::read_pdu_interface
{
public:
  int read_pdu(uint32_t lcid, uint8_t* payload, uint32_t nof_bytes)
  {
    uint32_t len = SRSLTE_MIN(ul_queues[lcid], nof_bytes);

    // set payload bytes to LCID so we can check later if the scheduling was correct
    memset(payload, lcid, len);

    // remove from UL queue
    ul_queues[lcid] -= len;

    return len;
  };

  void write_sdu(uint32_t lcid, uint32_t nof_bytes) { ul_queues[lcid] += nof_bytes; }

private:
  // UL queues where key is LCID and value the queue length
  std::map<uint32_t, uint32_t> ul_queues;
};

// Basic test to pack a MAC PDU with a two SDUs of short length (i.e < 128B for short length header) and multi-byte
// padding
int mac_sch_pdu_pack_test1()
{
  static uint8_t tv[] = {0x21, 0x08, 0x22, 0x08, 0x1f, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
                         0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00};

  srslte::log_filter rlc_log("RLC");
  rlc_log.set_level(srslte::LOG_LEVEL_DEBUG);
  rlc_log.set_hex_limit(100000);

  rlc_dummy rlc;

  srslte::log_ref mac_log = srslte::logmap::get("MAC");
  mac_log->set_level(srslte::LOG_LEVEL_DEBUG);
  mac_log->set_hex_limit(100000);

  // create RLC SDUs
  const uint32_t sdu_len = 8;
  rlc.write_sdu(1, sdu_len);
  rlc.write_sdu(2, sdu_len);

  const uint32_t  pdu_size = 25;
  srslte::sch_pdu pdu(10, mac_log);

  byte_buffer_t buffer;
  pdu.init_tx(&buffer, pdu_size, true);

  TESTASSERT(pdu.rem_size() == pdu_size);
  TESTASSERT(pdu.get_pdu_len() == pdu_size);
  TESTASSERT(pdu.get_sdu_space() == pdu_size - 1);
  TESTASSERT(pdu.get_current_sdu_ptr() == buffer.msg);

  // Add first subheader and SDU
  TESTASSERT(pdu.new_subh());
  TESTASSERT(pdu.get()->set_sdu(1, sdu_len, &rlc) == sdu_len);

  // Have used 8 B SDU plus 1 B subheader
  TESTASSERT(pdu.rem_size() == pdu_size - 8 - 1);

  // Add second SCH
  TESTASSERT(pdu.new_subh());
  TESTASSERT(pdu.get()->set_sdu(2, sdu_len, &rlc) == sdu_len);
  TESTASSERT(pdu.rem_size() == pdu_size - 16 - 3);

  // write PDU
  TESTASSERT(pdu.write_packet(mac_log) == buffer.msg);
  TESTASSERT(buffer.N_bytes == pdu_size);

  // log
  mac_log->info_hex(buffer.msg, buffer.N_bytes, "MAC PDU (%d B):\n", buffer.N_bytes);

#if HAVE_PCAP
  pcap_handle->write_ul_crnti(buffer.msg, buffer.N_bytes, 0x1001, true, 1, 0);
#endif

  // compare with TV
  TESTASSERT(memcmp(buffer.msg, tv, sizeof(tv)) == 0);

  return SRSLTE_SUCCESS;
}

// Basic test to pack a MAC PDU with a two SDUs of short length (i.e < 128B for short length header) and 2x single-byte
// padding
int mac_sch_pdu_pack_test2()
{
  static uint8_t tv[] = {0x3f, 0x3f, 0x21, 0x08, 0x02, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
                         0x01, 0x01, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02};

  srslte::log_filter rlc_log("RLC");
  rlc_log.set_level(srslte::LOG_LEVEL_DEBUG);
  rlc_log.set_hex_limit(100000);

  rlc_dummy rlc;

  srslte::log_ref mac_log = srslte::logmap::get("MAC");
  mac_log->set_level(srslte::LOG_LEVEL_DEBUG);
  mac_log->set_hex_limit(100000);

  // create RLC SDUs
  const uint32_t sdu_len = 8;
  rlc.write_sdu(1, sdu_len);
  rlc.write_sdu(2, sdu_len);

  const uint32_t pdu_size = 21;

  srslte::sch_pdu pdu(10, mac_log);

  byte_buffer_t buffer;
  pdu.init_tx(&buffer, pdu_size, true);

  TESTASSERT(pdu.rem_size() == pdu_size);
  TESTASSERT(pdu.get_pdu_len() == pdu_size);
  TESTASSERT(pdu.get_sdu_space() == pdu_size - 1);
  TESTASSERT(pdu.get_current_sdu_ptr() == buffer.msg);

  // Add first subheader and SDU
  TESTASSERT(pdu.new_subh());
  TESTASSERT(pdu.get()->set_sdu(1, sdu_len, &rlc) == sdu_len);

  // Have used 8 B SDU plus 1 B subheader
  TESTASSERT(pdu.rem_size() == pdu_size - 8 - 1);

  // Add second SCH
  TESTASSERT(pdu.new_subh());
  TESTASSERT(pdu.get()->set_sdu(2, sdu_len, &rlc) == sdu_len);
  TESTASSERT(pdu.rem_size() == pdu_size - 16 - 3);

  // write PDU
  pdu.write_packet(mac_log);

  // log
  mac_log->info_hex(buffer.msg, buffer.N_bytes, "MAC PDU (%d B):\n", buffer.N_bytes);

#if HAVE_PCAP
  pcap_handle->write_ul_crnti(buffer.msg, buffer.N_bytes, 0x1001, true, 1, 0);
#endif

  // compare with TV
  TESTASSERT(memcmp(buffer.msg, tv, sizeof(tv)) == 0);

  return SRSLTE_SUCCESS;
}

// Basic test to pack a MAC PDU with one short and one long SDU (i.e >= 128 B for 16bit length header)
int mac_sch_pdu_pack_test3()
{
  static uint8_t tv[] = {
      0x21, 0x08, 0x22, 0x80, 0x82, 0x1f, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x02, 0x02,
      0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
      0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
      0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
      0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
      0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
      0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
      0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  srslte::log_filter rlc_log("RLC");
  rlc_log.set_level(srslte::LOG_LEVEL_DEBUG);
  rlc_log.set_hex_limit(100000);

  rlc_dummy rlc;

  srslte::log_ref mac_log("MAC");

  // create RLC SDUs
  // const uint32_t sdu_len = 130;
  rlc.write_sdu(1, 8);
  rlc.write_sdu(2, 130);

  const uint32_t  pdu_size = 150;
  srslte::sch_pdu pdu(10, srslte::log_ref{"MAC"});

  byte_buffer_t buffer;
  pdu.init_tx(&buffer, pdu_size, true);

  TESTASSERT(pdu.rem_size() == pdu_size);
  TESTASSERT(pdu.get_pdu_len() == pdu_size);
  TESTASSERT(pdu.get_sdu_space() == pdu_size - 1);
  TESTASSERT(pdu.get_current_sdu_ptr() == buffer.msg);

  TESTASSERT(pdu.new_subh());
  TESTASSERT(pdu.get()->set_sdu(1, 8, &rlc));

  // Have used 8 B SDU plus 1 B subheader
  TESTASSERT(pdu.rem_size() == pdu_size - 8 - 1);

  TESTASSERT(pdu.new_subh());
  TESTASSERT(pdu.get()->set_sdu(2, 130, &rlc));

  // Have used 138 B SDU plus 3 B subheader
  TESTASSERT(pdu.rem_size() == pdu_size - 138 - 3);

  // write PDU
  pdu.write_packet(srslte::log_ref{"MAC"});

  // log
  mac_log->info_hex(buffer.msg, buffer.N_bytes, "MAC PDU (%d B):\n", buffer.N_bytes);

#if HAVE_PCAP
  pcap_handle->write_ul_crnti(buffer.msg, buffer.N_bytes, 0x1001, true, 1, 0);
#endif

  // compare with TV
  TESTASSERT(memcmp(buffer.msg, tv, sizeof(tv)) == 0);

  return SRSLTE_SUCCESS;
}

// Test for padding-only MAC PDU
int mac_sch_pdu_pack_test4()
{
  static uint8_t tv[] = {0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  srslte::log_filter rlc_log("RLC");
  rlc_log.set_level(srslte::LOG_LEVEL_DEBUG);
  rlc_log.set_hex_limit(100000);

  rlc_dummy rlc;

  srslte::log_ref mac_log("MAC");

  const uint32_t  pdu_size = 10;
  srslte::sch_pdu pdu(10, srslte::log_ref{"MAC"});

  byte_buffer_t buffer;
  pdu.init_tx(&buffer, pdu_size, true);

  TESTASSERT(pdu.rem_size() == pdu_size);
  TESTASSERT(pdu.get_pdu_len() == pdu_size);
  TESTASSERT(pdu.get_sdu_space() == pdu_size - 1);
  TESTASSERT(pdu.get_current_sdu_ptr() == buffer.msg);

  // Try to add SDU
  TESTASSERT(pdu.new_subh());
  TESTASSERT(pdu.get()->set_sdu(2, 5, &rlc) == 0);

  // Adding SDU failed, remove subheader again
  pdu.del_subh();

  // write PDU
  pdu.write_packet(srslte::log_ref{"MAC"});

  // make sure full PDU has been written
  TESTASSERT(buffer.N_bytes == pdu_size);

  // log
  mac_log->info_hex(buffer.msg, buffer.N_bytes, "MAC PDU (%d B):\n", buffer.N_bytes);

#if HAVE_PCAP
  pcap_handle->write_ul_crnti(buffer.msg, buffer.N_bytes, 0x1001, true, 1, 0);
#endif

  // compare with TV
  TESTASSERT(memcmp(buffer.msg, tv, sizeof(tv)) == 0);

  return SRSLTE_SUCCESS;
}

// Test for max. TBS MAC PDU
int mac_sch_pdu_pack_test5()
{
  rlc_dummy rlc;

  srslte::log_ref mac_log("MAC");

  // write big SDU
  rlc.write_sdu(2, 20000);

  const uint32_t  pdu_size = SRSLTE_MAX_TBSIZE_BITS / 8; // Max. DL allocation for a single TB using 256 QAM
  srslte::sch_pdu pdu(10, srslte::log_ref{"MAC"});

  byte_buffer_t buffer;
  pdu.init_tx(&buffer, pdu_size, true);

  TESTASSERT(pdu.rem_size() == pdu_size);
  TESTASSERT(pdu.get_pdu_len() == pdu_size);
  TESTASSERT(pdu.get_sdu_space() == pdu_size - 1);
  TESTASSERT(pdu.get_current_sdu_ptr() == buffer.msg);

  // Try to add SDU
  TESTASSERT(pdu.new_subh());
  TESTASSERT(pdu.get()->set_sdu(2, pdu_size - 1, &rlc) != 0);

  // write PDU
  pdu.write_packet(srslte::log_ref{"MAC"});

  // make sure full PDU has been written
  TESTASSERT(buffer.N_bytes == pdu_size);

  // log
  mac_log->info_hex(buffer.msg, buffer.N_bytes, "MAC PDU (%d B):\n", buffer.N_bytes);

#if HAVE_PCAP
  pcap_handle->write_ul_crnti(buffer.msg, buffer.N_bytes, 0x1001, true, 1, 0);
#endif

  return SRSLTE_SUCCESS;
}

// Test for Long BSR CE
int mac_sch_pdu_pack_test6()
{
  srslte::log_ref mac_log("MAC");

  const uint32_t  pdu_size = 8;
  srslte::sch_pdu pdu(10, srslte::log_ref{"MAC"});

  uint8_t tv[pdu_size] = {0x3e, 0x1f, 0x01, 0xfa, 0x7f, 0x00, 0x00, 0x00};

  byte_buffer_t buffer;
  pdu.init_tx(&buffer, pdu_size, true);

  TESTASSERT(pdu.rem_size() == pdu_size);
  TESTASSERT(pdu.get_pdu_len() == pdu_size);
  TESTASSERT(pdu.get_sdu_space() == pdu_size - 1);
  TESTASSERT(pdu.get_current_sdu_ptr() == buffer.msg);

  // Try to Long BSR CE
  uint32_t buff_size[4] = {0, 1000, 5000, 19200000};
  TESTASSERT(pdu.new_subh());
  TESTASSERT(pdu.get()->set_bsr(buff_size, srslte::ul_sch_lcid::LONG_BSR));

  // write PDU
  pdu.write_packet(srslte::log_ref{"MAC"});

  // compare with tv
  TESTASSERT(memcmp(buffer.msg, tv, buffer.N_bytes) == 0);

  // log
  mac_log->info_hex(buffer.msg, buffer.N_bytes, "MAC PDU (%d B):\n", buffer.N_bytes);

#if HAVE_PCAP
  pcap_handle->write_ul_crnti(buffer.msg, buffer.N_bytes, 0x1001, true, 1, 0);
#endif

  return SRSLTE_SUCCESS;
}

// Test for short MAC PDU containing padding only
int mac_sch_pdu_pack_test7()
{
  srslte::log_ref mac_log("MAC");

  rlc_dummy rlc;
  rlc.write_sdu(1, 8);

  const uint32_t  pdu_size = 2;
  srslte::sch_pdu pdu(10, srslte::log_ref{"MAC"});

  uint8_t tv[pdu_size] = {0x1f, 0x1f};

  byte_buffer_t buffer;
  pdu.init_tx(&buffer, pdu_size, true);

  TESTASSERT(pdu.rem_size() == pdu_size);
  TESTASSERT(pdu.get_pdu_len() == pdu_size);
  TESTASSERT(pdu.get_sdu_space() == pdu_size - 1);
  TESTASSERT(pdu.get_current_sdu_ptr() == buffer.msg);

  // Try to add SDU, subheader is ok
  TESTASSERT(pdu.new_subh());

  // adding SDU fails
  TESTASSERT(pdu.get()->set_sdu(2, 8, &rlc) == SRSLTE_ERROR);

  // remove subheader again
  pdu.del_subh();

  // write PDU
  pdu.write_packet(srslte::log_ref{"MAC"});

  // compare with tv
  TESTASSERT(memcmp(buffer.msg, tv, buffer.N_bytes) == 0);

  // log
  mac_log->info_hex(buffer.msg, buffer.N_bytes, "MAC PDU (%d B):\n", buffer.N_bytes);

#if HAVE_PCAP
  pcap_handle->write_ul_crnti(buffer.msg, buffer.N_bytes, 0x1001, true, 1, 0);
#endif

  return SRSLTE_SUCCESS;
}

// Test Packing of SCell Activation CE command
int mac_sch_pdu_pack_test8()
{
  srslte::log_ref log_h{"MAC"};

  const uint32_t  pdu_size = 2;
  srslte::sch_pdu pdu(10, log_h);
  std::bitset<8>  cc_mask(uniform_dist_u8(rand_gen));

  // subheader: R|F2|E|LCID = 0|0|0|11011
  uint8_t tv[pdu_size] = {0b00011011, (uint8_t)cc_mask.to_ulong()};
  // ensure reserved bit
  tv[1] &= ~(0x1u);
  // limit to max carriers
  tv[1] &= ((1u << (uint32_t)SRSLTE_MAX_CARRIERS) - 1u);

  byte_buffer_t buffer;
  pdu.init_tx(&buffer, pdu_size, true);

  TESTASSERT(pdu.rem_size() == pdu_size);
  TESTASSERT(pdu.get_pdu_len() == pdu_size);
  TESTASSERT(pdu.get_sdu_space() == pdu_size - 1);
  TESTASSERT(pdu.get_current_sdu_ptr() == buffer.msg);

  // Try SCell activation CE
  TESTASSERT(pdu.new_subh());
  std::array<bool, SRSLTE_MAX_CARRIERS> cc_activ_list = {};
  for (uint8_t i = 1; i < SRSLTE_MAX_CARRIERS; ++i) {
    cc_activ_list[i] = cc_mask.test(i);
  }
  TESTASSERT(pdu.get()->set_scell_activation_cmd(cc_activ_list));

  // write PDU
  pdu.write_packet(log_h);

  // compare with tv
  TESTASSERT(memcmp(buffer.msg, tv, buffer.N_bytes) == 0);

  // log
  log_h->info("Activation mask chosen was 0x%x", tv[1]);
  log_h->info_hex(buffer.msg, buffer.N_bytes, "MAC PDU with SCell Activation CE (%d B):\n", buffer.N_bytes);

#if HAVE_PCAP
  pcap_handle->write_dl_crnti(tv, sizeof(tv), 0x1001, true, 1, 0);
#endif

  return SRSLTE_SUCCESS;
}

// Test for Short BSR CE
int mac_sch_pdu_pack_test9()
{
  srslte::log_ref mac_log("MAC");

  const uint32_t  pdu_size = 3;
  srslte::sch_pdu pdu(10, srslte::log_ref{"MAC"});

  uint8_t tv[pdu_size] = {0x3f, 0x1d, 0x0a};

  byte_buffer_t buffer;
  pdu.init_tx(&buffer, pdu_size, true);

  TESTASSERT(pdu.rem_size() == pdu_size);
  TESTASSERT(pdu.get_pdu_len() == pdu_size);
  TESTASSERT(pdu.get_sdu_space() == pdu_size - 1);
  TESTASSERT(pdu.get_current_sdu_ptr() == buffer.msg);

  // Try to add short BSR CE
  uint32_t buff_size[4] = {36, 0, 0, 0};
  TESTASSERT(pdu.new_subh());
  TESTASSERT(pdu.get()->set_bsr(buff_size, srslte::ul_sch_lcid::SHORT_BSR));
  TESTASSERT(pdu.new_subh() == false);

  // write PDU
  pdu.write_packet(srslte::log_ref{"MAC"});

  // compare with tv
  TESTASSERT(memcmp(buffer.msg, tv, buffer.N_bytes) == 0);

  // log
  mac_log->info_hex(buffer.msg, buffer.N_bytes, "MAC PDU (%d B):\n", buffer.N_bytes);

#if HAVE_PCAP
  pcap_handle->write_ul_crnti(buffer.msg, buffer.N_bytes, 0x1001, true, 1, 0);
#endif

  return SRSLTE_SUCCESS;
}

// Test for Short BSR CE + PHR CE
int mac_sch_pdu_pack_test10()
{
  srslte::log_ref mac_log("MAC");

  const uint32_t  pdu_size = 4;
  srslte::sch_pdu pdu(10, srslte::log_ref{"MAC"});

  uint8_t tv[pdu_size] = {0x3d, 0x1a, 0x20, 0x21};

  byte_buffer_t buffer;
  pdu.init_tx(&buffer, pdu_size, true);

  TESTASSERT(pdu.rem_size() == pdu_size);
  TESTASSERT(pdu.get_pdu_len() == pdu_size);
  TESTASSERT(pdu.get_sdu_space() == pdu_size - 1);
  TESTASSERT(pdu.get_current_sdu_ptr() == buffer.msg);

  // Try to add short BSR CE
  uint32_t buff_size[4] = {1132, 0, 0, 0};
  TESTASSERT(pdu.new_subh());
  TESTASSERT(pdu.get()->set_bsr(buff_size, srslte::ul_sch_lcid::SHORT_BSR));

  // Try to add PHR CE
  TESTASSERT(pdu.new_subh());
  TESTASSERT(pdu.get()->set_phr(10.1));

  // write PDU
  pdu.write_packet(srslte::log_ref{"MAC"});

  // compare with tv
  TESTASSERT(memcmp(buffer.msg, tv, buffer.N_bytes) == 0);

  // log
  mac_log->info_hex(buffer.msg, buffer.N_bytes, "MAC PDU (%d B):\n", buffer.N_bytes);

#if HAVE_PCAP
  pcap_handle->write_ul_crnti(buffer.msg, buffer.N_bytes, 0x1001, true, 1, 0);
#endif

  return SRSLTE_SUCCESS;
}

// Pack test for short MAC PDU, trying to add long BSR but no space left
int mac_sch_pdu_pack_test11()
{
  srslte::log_ref mac_log("MAC");

  const uint32_t  pdu_size = 3;
  srslte::sch_pdu pdu(10, srslte::log_ref{"MAC"});

  uint8_t tv[pdu_size] = {0x1f, 0x00, 0x00};

  byte_buffer_t buffer;
  pdu.init_tx(&buffer, pdu_size, true);

  TESTASSERT(pdu.rem_size() == pdu_size);
  TESTASSERT(pdu.get_pdu_len() == pdu_size);
  TESTASSERT(pdu.get_sdu_space() == pdu_size - 1);
  TESTASSERT(pdu.get_current_sdu_ptr() == buffer.msg);

  // Try to Long BSR CE
  uint32_t buff_size[4] = {0, 1000, 5000, 19200000};
  TESTASSERT(pdu.new_subh());
  TESTASSERT(pdu.get()->set_bsr(buff_size, srslte::ul_sch_lcid::LONG_BSR) == false);

  // Adding BSR failed, remove subheader again
  pdu.del_subh();

  // write PDU
  pdu.write_packet(srslte::log_ref{"MAC"});

  // compare with tv
  TESTASSERT(memcmp(buffer.msg, tv, buffer.N_bytes) == 0);

  // log
  mac_log->info_hex(buffer.msg, buffer.N_bytes, "MAC PDU (%d B):\n", buffer.N_bytes);

#if HAVE_PCAP
  pcap_handle->write_ul_crnti(buffer.msg, buffer.N_bytes, 0x1001, true, 1, 0);
#endif

  return SRSLTE_SUCCESS;
}

// Test for checking error cases
int mac_sch_pdu_pack_error_test()
{
  srslte::log_filter rlc_log("RLC");
  rlc_log.set_level(srslte::LOG_LEVEL_DEBUG);
  rlc_log.set_hex_limit(100000);

  rlc_dummy rlc;

  srslte::log_ref mac_log("MAC");

  // create RLC SDUs
  rlc.write_sdu(1, 8);

  const uint32_t  pdu_size = 150;
  srslte::sch_pdu pdu(10, srslte::log_ref{"MAC"});

  byte_buffer_t buffer;
  pdu.init_tx(&buffer, pdu_size, true);

  TESTASSERT(pdu.rem_size() == pdu_size);
  TESTASSERT(pdu.get_pdu_len() == pdu_size);
  TESTASSERT(pdu.get_sdu_space() == pdu_size - 1);
  TESTASSERT(pdu.get_current_sdu_ptr() == buffer.msg);

  // set msg pointer almost to end of byte buffer
  int buffer_space = buffer.get_tailroom();
  buffer.msg += buffer_space - 2;

  // subheader can be added
  TESTASSERT(pdu.new_subh());

  // adding SDU fails
  TESTASSERT(pdu.get()->set_sdu(1, 8, &rlc) == SRSLTE_ERROR);

  // writing PDU fails
  TESTASSERT(pdu.write_packet(srslte::log_ref{"MAC"}) == nullptr);

  // reset buffer
  buffer.clear();

  // write SDU again
  TESTASSERT(pdu.get() != nullptr);
  TESTASSERT(pdu.get()->set_sdu(1, 100, &rlc) == 8); // only 8 bytes in RLC buffer

  // writing PDU fails
  TESTASSERT(pdu.write_packet(srslte::log_ref{"MAC"}));

  // log
  mac_log->info_hex(buffer.msg, buffer.N_bytes, "MAC PDU (%d B):\n", buffer.N_bytes);

#if HAVE_PCAP
  pcap_handle->write_ul_crnti(buffer.msg, buffer.N_bytes, 0x1001, true, 1,0);
#endif

  return SRSLTE_SUCCESS;
}

int mac_mch_pdu_pack_test1()
{
  static uint8_t tv[] = {0x3e, 0x02, 0x20, 0x05, 0x21, 0x0a, 0x1f, 0x0f, 0xff, 0x01, 0x02, 0x03, 0x04, 0x05, 0x02,
                         0x04, 0x06, 0x08, 0x0a, 0x0c, 0x0e, 0x10, 0x12, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  srslte::log_ref mac_log("MAC");

  const uint32_t  pdu_size = 30;
  srslte::mch_pdu mch_pdu(10, srslte::log_ref{"MAC"});
  byte_buffer_t   buffer;
  mch_pdu.init_tx(&buffer, pdu_size, true);

  TESTASSERT(mch_pdu.rem_size() == pdu_size);
  TESTASSERT(mch_pdu.get_pdu_len() == pdu_size);
  TESTASSERT(mch_pdu.get_sdu_space() == pdu_size - 1);
  TESTASSERT(mch_pdu.get_current_sdu_ptr() == buffer.msg);
  // Add first subheader and SDU
  TESTASSERT(mch_pdu.new_subh());
  TESTASSERT(mch_pdu.get()->set_next_mch_sched_info(1, 0));

  // Add second SCH
  TESTASSERT(mch_pdu.new_subh());
  uint8_t sdu[5] = {1, 2, 3, 4, 5};
  TESTASSERT(mch_pdu.get()->set_sdu(0, 5, sdu) == 5);

  TESTASSERT(mch_pdu.new_subh());
  uint8_t sdu1[10] = {2, 4, 6, 8, 10, 12, 14, 16, 18, 20};
  mch_pdu.get()->set_sdu(1, 10, sdu1);

  // write PDU
  TESTASSERT(mch_pdu.write_packet(srslte::log_ref{"MAC"}) == buffer.msg);

  // log
  mac_log->info_hex(buffer.msg, buffer.N_bytes, "MAC PDU (%d B):\n", buffer.N_bytes);

#if HAVE_PCAP
  pcap_handle->write_ul_crnti(buffer.msg, buffer.N_bytes, 0x1001, true, 1,0);
#endif

  // compare with TV
  TESTASSERT(memcmp(buffer.msg, tv, buffer.N_bytes) == 0);

#if HAVE_PCAP
  pcap_handle->write_ul_crnti(tv, sizeof(tv), 0x1001, true, 1, 0);
#endif

  return SRSLTE_SUCCESS;
}

// Parsing a corrupted MAC PDU and making sure the PDU is reset and not further processed
int mac_sch_pdu_unpack_test1()
{
  static uint8_t tv[] = {0x3f, 0x3f, 0x21, 0x3f, 0x03, 0x00, 0x04, 0x00, 0x04};

  srslte::sch_pdu pdu(10, srslte::log_ref{"MAC"});
  pdu.init_rx(sizeof(tv), false);
  pdu.parse_packet(tv);

  // make sure this PDU is reset and will not be further processed
  TESTASSERT(pdu.nof_subh() == 0);
  TESTASSERT(pdu.next() == false);

#if HAVE_PCAP
  pcap_handle->write_ul_crnti(tv, sizeof(tv), 0x1001, true, 1,0);
#endif

  return SRSLTE_SUCCESS;
}

// Parsing a (corrupted) MAC PDU that only contains padding
int mac_sch_pdu_unpack_test2()
{
  static uint8_t tv[] = {0x3f, 0x3f};

  srslte::sch_pdu pdu(20, srslte::log_ref{"MAC"});
  pdu.init_rx(sizeof(tv), false);
  pdu.parse_packet(tv);

  // make sure this PDU is reset and will not be further processed
  TESTASSERT(pdu.nof_subh() == 0);
  TESTASSERT(pdu.next() == false);

#if HAVE_PCAP
  pcap_handle->write_ul_crnti(tv, sizeof(tv), 0x1001, true, 1,0);
#endif

  return SRSLTE_SUCCESS;
}

int mac_slsch_pdu_unpack_test1()
{
  // SL-SCH PDU captures from UXM 5G CV2X
  static uint8_t tv[] = {0x30, 0xED, 0x0E, 0x03, 0x00, 0x00, 0x01, 0x21, 0x2B, 0x1F, 0x9F, 0x75, 0x76,
                         0x87, 0x00, 0x00, 0x8E, 0x9E, 0xA5, 0xFD, 0xA1, 0xA3, 0xA7, 0xA9, 0x7F, 0x68,
                         0x6C, 0xB8, 0xBF, 0x0F, 0x96, 0x89, 0x2E, 0xDC, 0x80, 0x64, 0x06, 0x40, 0x00,
                         0x18, 0x6A, 0x07, 0x20, 0x7C, 0xE1, 0xE0, 0x04, 0x40, 0x02, 0x04, 0x80, 0x60,
                         0x2C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  srslte::log_filter mac_log("MAC");
  mac_log.set_level(srslte::LOG_LEVEL_DEBUG);
  mac_log.set_hex_limit(100000);

#if HAVE_PCAP
  pcap_handle->write_sl_crnti(tv, sizeof(tv), CRNTI, true, 1, 0);
#endif

  return SRSLTE_SUCCESS;
}

int main(int argc, char** argv)
{
#if HAVE_PCAP
  pcap_handle = std::unique_ptr<srslte::mac_pcap>(new srslte::mac_pcap());
  pcap_handle->open("mac_pdu_test.pcap");
#endif
  logmap::set_default_hex_limit(100000);
  logmap::set_default_log_level(LOG_LEVEL_DEBUG);

  srslte::log_ref mac_log{"MAC"};
  mac_log->set_level(srslte::LOG_LEVEL_DEBUG);
  mac_log->set_hex_limit(100000);

  TESTASSERT(mac_rar_pdu_unpack_test1() == SRSLTE_SUCCESS);
  TESTASSERT(mac_rar_pdu_unpack_test2() == SRSLTE_SUCCESS);
  TESTASSERT(mac_rar_pdu_pack_test1() == SRSLTE_SUCCESS);
  TESTASSERT(mac_rar_pdu_pack_test2() == SRSLTE_SUCCESS);

  TESTASSERT(mac_sch_pdu_pack_test1() == SRSLTE_SUCCESS);
  TESTASSERT(mac_sch_pdu_pack_test2() == SRSLTE_SUCCESS);
  TESTASSERT(mac_sch_pdu_pack_test3() == SRSLTE_SUCCESS);
  TESTASSERT(mac_sch_pdu_pack_test4() == SRSLTE_SUCCESS);
  TESTASSERT(mac_sch_pdu_pack_test5() == SRSLTE_SUCCESS);
  TESTASSERT(mac_sch_pdu_pack_test6() == SRSLTE_SUCCESS);
  TESTASSERT(mac_sch_pdu_pack_test7() == SRSLTE_SUCCESS);
  TESTASSERT(mac_sch_pdu_pack_test8() == SRSLTE_SUCCESS);
  TESTASSERT(mac_sch_pdu_pack_test9() == SRSLTE_SUCCESS);
  TESTASSERT(mac_sch_pdu_pack_test10() == SRSLTE_SUCCESS);
  TESTASSERT(mac_sch_pdu_pack_test11() == SRSLTE_SUCCESS);

  TESTASSERT(mac_sch_pdu_pack_error_test() == SRSLTE_SUCCESS);

  TESTASSERT(mac_mch_pdu_pack_test1() == SRSLTE_SUCCESS);

  TESTASSERT(mac_sch_pdu_unpack_test1() == SRSLTE_SUCCESS);
  TESTASSERT(mac_sch_pdu_unpack_test2() == SRSLTE_SUCCESS);

  TESTASSERT(mac_slsch_pdu_unpack_test1() == SRSLTE_SUCCESS);

  return SRSLTE_SUCCESS;
}
