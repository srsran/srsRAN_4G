/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#include "srsran/common/common.h"
#include "srsran/common/interfaces_common.h"
#include "srsran/common/mac_pcap.h"
#include "srsran/common/test_common.h"
#include "srsran/mac/pdu.h"

extern "C" {
#include "srsran/phy/phch/dci.h"
}

#include <bitset>
#include <iostream>
#include <map>
#include <random>

#define HAVE_PCAP 0

std::random_device                     rd;
std::mt19937                           rand_gen(rd());
std::uniform_int_distribution<uint8_t> uniform_dist_u8(0, 255);

static std::unique_ptr<srsran::mac_pcap> pcap_handle = nullptr;

using namespace srsran;

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
  srsran::rar_pdu rar_pdu_msg;
  rar_pdu_msg.init_rx(sizeof(rar_pdu_tv1));
  rar_pdu_msg.parse_packet(rar_pdu_tv1);
  fmt::memory_buffer buffer;
  rar_pdu_msg.to_string(buffer);
  std::cout << fmt::to_string(buffer) << std::endl;

  TESTASSERT(not rar_pdu_msg.has_backoff());
  TESTASSERT(rar_pdu_msg.nof_subh() == 1);
  while (rar_pdu_msg.next()) {
    TESTASSERT(rar_pdu_msg.get()->get_rapid() == RAPID_TV1);
    TESTASSERT(rar_pdu_msg.get()->get_ta_cmd() == TA_CMD_TV1);
    TESTASSERT(rar_pdu_msg.get()->get_temp_crnti() == CRNTI);
  }

  return SRSRAN_SUCCESS;
}

int mac_rar_pdu_unpack_test2()
{
  srsran::rar_pdu rar_pdu_msg;
  rar_pdu_msg.init_rx(sizeof(rar_pdu_tv2));
  rar_pdu_msg.parse_packet(rar_pdu_tv2);
  fmt::memory_buffer buffer;
  rar_pdu_msg.to_string(buffer);
  std::cout << fmt::to_string(buffer) << std::endl;

  TESTASSERT(rar_pdu_msg.nof_subh() == 2);
  TESTASSERT(rar_pdu_msg.has_backoff());
  TESTASSERT(rar_pdu_msg.get_backoff() == BACKOFF_IND_TV2);
  while (rar_pdu_msg.next()) {
    if (rar_pdu_msg.get()->has_rapid()) {
      TESTASSERT(rar_pdu_msg.get()->get_rapid() == RAPID_TV2);
      TESTASSERT(rar_pdu_msg.get()->get_ta_cmd() == TA_CMD_TV2);
      TESTASSERT(rar_pdu_msg.get()->get_temp_crnti() == CRNTI);
    }
  }

  return SRSRAN_SUCCESS;
}

// Malformed RAR PDU with two RAPIDs but incomplete content
int mac_rar_pdu_unpack_test3()
{
  // The last byte of the malformed RAR PDU is byte 11 (0x1a), we need to add 3 more bytes to the array to please ASAN
  // though
  uint8_t rar_pdu[]   = {0xd5, 0x4e, 0x02, 0x80, 0x1a, 0x0c, 0x00, 0x47, 0x00, 0x00, 0x1a, 0xff, 0xff, 0xff};
  uint8_t rar_pdu_len = 11;

  srsran::rar_pdu rar_pdu_msg;
  rar_pdu_msg.init_rx(rar_pdu_len); // only pass the 11 valid bytes
  TESTASSERT(rar_pdu_msg.parse_packet(rar_pdu) != SRSRAN_SUCCESS);
  TESTASSERT(rar_pdu_msg.nof_subh() == 0);

  fmt::memory_buffer buffer;
  rar_pdu_msg.to_string(buffer);
  std::cout << fmt::to_string(buffer) << std::endl;

  return SRSRAN_SUCCESS;
}

int mac_rar_pdu_pack_test1()
{
  // Prepare RAR grant
  uint8_t                grant_buffer[64] = {};
  srsran_dci_rar_grant_t rar_grant        = {};
  rar_grant.tpc_pusch                     = 3;
  srsran_dci_rar_pack(&rar_grant, grant_buffer);

  // Create MAC PDU and add RAR subheader
  srsran::rar_pdu rar_pdu;

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

  return SRSRAN_SUCCESS;
}

int mac_rar_pdu_pack_test2()
{
  // Prepare RAR grant
  uint8_t                grant_buffer[64] = {};
  srsran_dci_rar_grant_t rar_grant        = {};
  rar_grant.tpc_pusch                     = 3;
  srsran_dci_rar_pack(&rar_grant, grant_buffer);

  // Create MAC PDU and add RAR subheader
  srsran::rar_pdu rar_pdu;
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

  return SRSRAN_SUCCESS;
}

// Helper class to provide read_pdu_interface
class rlc_dummy : public srsran::read_pdu_interface
{
public:
  int read_pdu(uint32_t lcid, uint8_t* payload, uint32_t nof_bytes)
  {
    uint32_t len = std::min(ul_queues[lcid], nof_bytes);

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

  auto& mac_logger = srslog::fetch_basic_logger("MAC");

  rlc_dummy rlc;

  // create RLC SDUs
  const uint32_t sdu_len = 8;
  rlc.write_sdu(1, sdu_len);
  rlc.write_sdu(2, sdu_len);

  const uint32_t  pdu_size = 25;
  srsran::sch_pdu pdu(10, mac_logger);

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
  TESTASSERT(pdu.write_packet(mac_logger) == buffer.msg);
  TESTASSERT(buffer.N_bytes == pdu_size);

  // log
  mac_logger.info(buffer.msg, buffer.N_bytes, "MAC PDU (%d B):", buffer.N_bytes);

#if HAVE_PCAP
  pcap_handle->write_ul_crnti(buffer.msg, buffer.N_bytes, 0x1001, true, 1, 0);
#endif

  // compare with TV
  TESTASSERT(memcmp(buffer.msg, tv, sizeof(tv)) == 0);

  return SRSRAN_SUCCESS;
}

// Basic test to pack a MAC PDU with a two SDUs of short length (i.e < 128B for short length header) and 2x single-byte
// padding
int mac_sch_pdu_pack_test2()
{
  static uint8_t tv[] = {0x3f, 0x3f, 0x21, 0x08, 0x02, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
                         0x01, 0x01, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02};

  auto& mac_logger = srslog::fetch_basic_logger("MAC");

  rlc_dummy rlc;

  // create RLC SDUs
  const uint32_t sdu_len = 8;
  rlc.write_sdu(1, sdu_len);
  rlc.write_sdu(2, sdu_len);

  const uint32_t pdu_size = 21;

  srsran::sch_pdu pdu(10, mac_logger);

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
  pdu.write_packet(mac_logger);

  // log
  mac_logger.info(buffer.msg, buffer.N_bytes, "MAC PDU (%d B):", buffer.N_bytes);

#if HAVE_PCAP
  pcap_handle->write_ul_crnti(buffer.msg, buffer.N_bytes, 0x1001, true, 1, 0);
#endif

  // compare with TV
  TESTASSERT(memcmp(buffer.msg, tv, sizeof(tv)) == 0);

  return SRSRAN_SUCCESS;
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

  auto& mac_logger = srslog::fetch_basic_logger("MAC");

  rlc_dummy rlc;

  // create RLC SDUs
  // const uint32_t sdu_len = 130;
  rlc.write_sdu(1, 8);
  rlc.write_sdu(2, 130);

  const uint32_t  pdu_size = 150;
  srsran::sch_pdu pdu(10, mac_logger);

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
  pdu.write_packet(mac_logger);

  // log
  mac_logger.info(buffer.msg, buffer.N_bytes, "MAC PDU (%d B):", buffer.N_bytes);

#if HAVE_PCAP
  pcap_handle->write_ul_crnti(buffer.msg, buffer.N_bytes, 0x1001, true, 1, 0);
#endif

  // compare with TV
  TESTASSERT(memcmp(buffer.msg, tv, sizeof(tv)) == 0);

  return SRSRAN_SUCCESS;
}

// Test for padding-only MAC PDU
int mac_sch_pdu_pack_test4()
{
  static uint8_t tv[] = {0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  auto& mac_logger = srslog::fetch_basic_logger("MAC");

  rlc_dummy rlc;

  const uint32_t  pdu_size = 10;
  srsran::sch_pdu pdu(10, mac_logger);

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
  pdu.write_packet(mac_logger);

  // make sure full PDU has been written
  TESTASSERT(buffer.N_bytes == pdu_size);

  // log
  mac_logger.info(buffer.msg, buffer.N_bytes, "MAC PDU (%d B):", buffer.N_bytes);

#if HAVE_PCAP
  pcap_handle->write_ul_crnti(buffer.msg, buffer.N_bytes, 0x1001, true, 1, 0);
#endif

  // compare with TV
  TESTASSERT(memcmp(buffer.msg, tv, sizeof(tv)) == 0);

  return SRSRAN_SUCCESS;
}

// Test for max. TBS MAC PDU
int mac_sch_pdu_pack_test5()
{
  rlc_dummy rlc;

  auto& mac_logger = srslog::fetch_basic_logger("MAC");

  // write big SDU
  rlc.write_sdu(2, 20000);

  const uint32_t  pdu_size = SRSRAN_MAX_TBSIZE_BITS / 8; // Max. DL allocation for a single TB using 256 QAM
  srsran::sch_pdu pdu(10, mac_logger);

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
  pdu.write_packet(mac_logger);

  // make sure full PDU has been written
  TESTASSERT(buffer.N_bytes == pdu_size);

  // log
  mac_logger.info(buffer.msg, buffer.N_bytes, "MAC PDU (%d B):", buffer.N_bytes);

#if HAVE_PCAP
  pcap_handle->write_ul_crnti(buffer.msg, buffer.N_bytes, 0x1001, true, 1, 0);
#endif

  return SRSRAN_SUCCESS;
}

// Test for Long BSR CE
int mac_sch_pdu_pack_test6()
{
  auto& mac_logger = srslog::fetch_basic_logger("MAC");

  const uint32_t  pdu_size = 8;
  srsran::sch_pdu pdu(10, mac_logger);

  uint8_t tv[pdu_size]  = {0x3e, 0x1f, 0x86, 0x18, 0x61, 0x00, 0x00, 0x00}; // upper edge case
  uint8_t tv2[pdu_size] = {0x3e, 0x1f, 0x04, 0x10, 0x41, 0x00, 0x00, 0x00}; // lower edge case
  uint8_t tv3[pdu_size] = {0x3e, 0x1f, 0xf3, 0xdf, 0xbf, 0x00, 0x00, 0x00}; // max index case

  byte_buffer_t buffer;
  byte_buffer_t rx_buffer;
  pdu.init_tx(&buffer, pdu_size, true);

  TESTASSERT(pdu.rem_size() == pdu_size);
  TESTASSERT(pdu.get_pdu_len() == pdu_size);
  TESTASSERT(pdu.get_sdu_space() == pdu_size - 1);
  TESTASSERT(pdu.get_current_sdu_ptr() == buffer.msg);

  // Long BSR CE
  // 1552 B is the upper edge for BSR index 33 (1326 < BS <= 1552), which results in 0b100001 being
  // the bit-pattern reported for each LCG
  uint32_t buff_size_tx[4] = {1552, 1552, 1552, 1552};
  TESTASSERT(pdu.new_subh());
  TESTASSERT(pdu.get()->set_bsr(buff_size_tx, srsran::ul_sch_lcid::LONG_BSR));

  // write PDU
  pdu.write_packet(mac_logger);

  // compare with tv
  TESTASSERT(memcmp(buffer.msg, tv, buffer.N_bytes) == 0);

  // unpack again
  pdu.init_rx(sizeof(tv), true);
  pdu.parse_packet(tv);

  // check subheaders
  TESTASSERT(pdu.nof_subh() == 2);

  uint32_t buff_size_rx[4] = {};
  while (pdu.next()) {
    if (!pdu.get()->is_sdu() && pdu.get()->ul_sch_ce_type() == srsran::ul_sch_lcid::LONG_BSR) {
      uint32_t buff_size_idx[4] = {};
      uint32_t nonzero_lcg      = pdu.get()->get_bsr(buff_size_idx, buff_size_rx);
      for (uint32_t i = 0; i < 4; i++) {
        printf("buff_size_idx[%d]=%d buff_size_bytes=%d\n", i, buff_size_idx[i], buff_size_rx[i]);
      }
    }
  }

  // check received buff sizes match transmitted ones
  for (uint32_t i = 0; i < 4; i++) {
    TESTASSERT(buff_size_rx[i] == buff_size_tx[i]);
  }
  fmt::memory_buffer str_buffer;
  pdu.to_string(str_buffer);
  mac_logger.info("%s", fmt::to_string(str_buffer));

  // log
  mac_logger.info(buffer.msg, buffer.N_bytes, "MAC PDU (%d B):", buffer.N_bytes);

#if HAVE_PCAP
  pcap_handle->write_ul_crnti(buffer.msg, buffer.N_bytes, 0x1001, true, 1, 0);
#endif

  // test lower edge BSR (1 B available for each LCG)
  pdu.init_tx(&buffer, pdu_size, true);
  buffer.clear();

  TESTASSERT(pdu.rem_size() == pdu_size);
  TESTASSERT(pdu.get_pdu_len() == pdu_size);
  TESTASSERT(pdu.get_sdu_space() == pdu_size - 1);
  TESTASSERT(pdu.get_current_sdu_ptr() == buffer.msg);

  // Long BSR CE
  // 1 B is the lower edge for BSR index 1 (0 < BS <= 10) so the UE should report index 1 when there is one byte to
  // transmit One the receive side, the eNB should extract the maximum number of bytes for this index, i.e. 10 for each
  // LCG
  uint32_t buff_size_tx_low_edge[4]          = {1, 1, 1, 1};
  uint32_t buff_size_rx_expected_low_edge[4] = {10, 10, 10, 10};
  TESTASSERT(pdu.new_subh());
  TESTASSERT(pdu.get()->set_bsr(buff_size_tx_low_edge, srsran::ul_sch_lcid::LONG_BSR));

  // write PDU
  pdu.write_packet(mac_logger);
  str_buffer.clear();
  pdu.to_string(str_buffer);
  mac_logger.info("%s", fmt::to_string(str_buffer));

  TESTASSERT(memcmp(buffer.msg, tv2, buffer.N_bytes) == 0);

  // log
  mac_logger.info(buffer.msg, buffer.N_bytes, "MAC PDU (%d B):", buffer.N_bytes);

#if HAVE_PCAP
  pcap_handle->write_ul_crnti(buffer.msg, buffer.N_bytes, 0x1001, true, 1, 0);
#endif

  // read PDU again
  rx_buffer.clear();
  pdu.init_rx(sizeof(tv2), true);
  pdu.parse_packet(tv2);

  // check subheaders
  TESTASSERT(pdu.nof_subh() == 2);

  uint32_t buff_size_rx_low_edge[4] = {};
  while (pdu.next()) {
    if (!pdu.get()->is_sdu() && pdu.get()->ul_sch_ce_type() == srsran::ul_sch_lcid::LONG_BSR) {
      uint32_t buff_size_idx[4] = {};
      uint32_t nonzero_lcg      = pdu.get()->get_bsr(buff_size_idx, buff_size_rx_low_edge);
      for (uint32_t i = 0; i < 4; i++) {
        printf("buff_size_idx[%d]=%d buff_size_bytes=%d\n", i, buff_size_idx[i], buff_size_rx_low_edge[i]);
      }
    }
  }

  // check received buff sizes match transmitted ones
  for (uint32_t i = 0; i < 4; i++) {
    TESTASSERT(buff_size_rx_low_edge[i] == buff_size_rx_expected_low_edge[i]);
  }

  // test index 62 and 63 (max value) buffer states
  pdu.init_tx(&buffer, pdu_size, true);
  buffer.clear();

  TESTASSERT(pdu.rem_size() == pdu_size);
  TESTASSERT(pdu.get_pdu_len() == pdu_size);
  TESTASSERT(pdu.get_sdu_space() == pdu_size - 1);
  TESTASSERT(pdu.get_current_sdu_ptr() == buffer.msg);

  // Long BSR CE with index 60, 61, 62, and 63
  uint32_t buff_size_max_idx[4]               = {93480, 128125, 150000, 150001};
  uint32_t buff_size_rx_expected_max_idx[4]   = {60, 61, 62, 63};
  uint32_t buff_size_rx_expected_max_value[4] = {109439, 128125, 150000, 150000};
  TESTASSERT(pdu.new_subh());
  TESTASSERT(pdu.get()->set_bsr(buff_size_max_idx, srsran::ul_sch_lcid::LONG_BSR));

  // write PDU
  pdu.write_packet(mac_logger);
  str_buffer.clear();
  pdu.to_string(str_buffer);
  mac_logger.info("%s", fmt::to_string(str_buffer));

  TESTASSERT(memcmp(buffer.msg, tv3, buffer.N_bytes) == 0);

  // log
  mac_logger.info(buffer.msg, buffer.N_bytes, "MAC PDU (%d B):", buffer.N_bytes);

#if HAVE_PCAP
  pcap_handle->write_ul_crnti(buffer.msg, buffer.N_bytes, 0x1001, true, 1, 0);
#endif

  // read PDU again
  rx_buffer.clear();
  pdu.init_rx(sizeof(tv3), true);
  pdu.parse_packet(tv3);

  // check subheaders
  TESTASSERT(pdu.nof_subh() == 2);

  uint32_t buff_size_rx_max_idx[4] = {};
  while (pdu.next()) {
    if (!pdu.get()->is_sdu() && pdu.get()->ul_sch_ce_type() == srsran::ul_sch_lcid::LONG_BSR) {
      uint32_t buff_size[4] = {};
      uint32_t nonzero_lcg  = pdu.get()->get_bsr(buff_size_rx_max_idx, buff_size);
      for (uint32_t i = 0; i < 4; i++) {
        printf("buff_size_idx[%d]=%d buff_size_bytes=%d\n", i, buff_size_rx_max_idx[i], buff_size[i]);
      }
    }
  }

  // check received buff sizes match transmitted ones
  for (uint32_t i = 0; i < 4; i++) {
    TESTASSERT(buff_size_rx_max_idx[i] == buff_size_rx_expected_max_idx[i]);
  }

  return SRSRAN_SUCCESS;
}

// Test for short MAC PDU containing padding only
int mac_sch_pdu_pack_test7()
{
  auto& mac_logger = srslog::fetch_basic_logger("MAC");

  rlc_dummy rlc;
  rlc.write_sdu(1, 8);

  const uint32_t  pdu_size = 2;
  srsran::sch_pdu pdu(10, mac_logger);

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
  TESTASSERT(pdu.get()->set_sdu(2, 8, &rlc) == SRSRAN_ERROR);

  // remove subheader again
  pdu.del_subh();

  // write PDU
  pdu.write_packet(mac_logger);

  // compare with tv
  TESTASSERT(memcmp(buffer.msg, tv, buffer.N_bytes) == 0);

  // log
  mac_logger.info(buffer.msg, buffer.N_bytes, "MAC PDU (%d B):", buffer.N_bytes);

#if HAVE_PCAP
  pcap_handle->write_ul_crnti(buffer.msg, buffer.N_bytes, 0x1001, true, 1, 0);
#endif

  return SRSRAN_SUCCESS;
}

// Test Packing of SCell Activation CE command
int mac_sch_pdu_pack_test8()
{
  auto& mac_logger = srslog::fetch_basic_logger("MAC");

  const uint32_t  pdu_size = 2;
  srsran::sch_pdu pdu(10, mac_logger);
  std::bitset<8>  cc_mask(uniform_dist_u8(rand_gen));

  // subheader: R|F2|E|LCID = 0|0|0|11011
  uint8_t tv[pdu_size] = {0b00011011, (uint8_t)cc_mask.to_ulong()};
  // ensure reserved bit
  tv[1] &= ~(0x1u);
  // limit to max carriers
  tv[1] &= ((1u << (uint32_t)SRSRAN_MAX_CARRIERS) - 1u);

  byte_buffer_t buffer;
  pdu.init_tx(&buffer, pdu_size, true);

  TESTASSERT(pdu.rem_size() == pdu_size);
  TESTASSERT(pdu.get_pdu_len() == pdu_size);
  TESTASSERT(pdu.get_sdu_space() == pdu_size - 1);
  TESTASSERT(pdu.get_current_sdu_ptr() == buffer.msg);

  // Try SCell activation CE
  TESTASSERT(pdu.new_subh());
  std::array<bool, SRSRAN_MAX_CARRIERS> cc_activ_list = {};
  for (uint8_t i = 1; i < SRSRAN_MAX_CARRIERS; ++i) {
    cc_activ_list[i] = cc_mask.test(i);
  }
  TESTASSERT(pdu.get()->set_scell_activation_cmd(cc_activ_list));

  // write PDU
  pdu.write_packet(mac_logger);

  // compare with tv
  TESTASSERT(memcmp(buffer.msg, tv, buffer.N_bytes) == 0);

  // log
  mac_logger.info("Activation mask chosen was 0x%x", tv[1]);
  mac_logger.info(buffer.msg, buffer.N_bytes, "MAC PDU with SCell Activation CE (%d B):", buffer.N_bytes);

#if HAVE_PCAP
  pcap_handle->write_dl_crnti(tv, sizeof(tv), 0x1001, true, 1, 0);
#endif

  return SRSRAN_SUCCESS;
}

// Test for Short BSR CE
int mac_sch_pdu_pack_test9()
{
  auto& mac_logger = srslog::fetch_basic_logger("MAC");

  const uint32_t  pdu_size = 3;
  srsran::sch_pdu pdu(10, mac_logger);

  uint8_t tv[pdu_size] = {0x3f, 0x1d, 0x09};

  byte_buffer_t buffer;
  pdu.init_tx(&buffer, pdu_size, true);

  TESTASSERT(pdu.rem_size() == pdu_size);
  TESTASSERT(pdu.get_pdu_len() == pdu_size);
  TESTASSERT(pdu.get_sdu_space() == pdu_size - 1);
  TESTASSERT(pdu.get_current_sdu_ptr() == buffer.msg);

  // Try to add short BSR CE
  uint32_t buff_size[4] = {36, 0, 0, 0};
  TESTASSERT(pdu.new_subh());
  TESTASSERT(pdu.get()->set_bsr(buff_size, srsran::ul_sch_lcid::SHORT_BSR));
  TESTASSERT(pdu.new_subh() == false);

  // write PDU
  pdu.write_packet(mac_logger);

  // compare with tv
  TESTASSERT(memcmp(buffer.msg, tv, buffer.N_bytes) == 0);

  // log
  mac_logger.info(buffer.msg, buffer.N_bytes, "MAC PDU (%d B):", buffer.N_bytes);

#if HAVE_PCAP
  pcap_handle->write_ul_crnti(buffer.msg, buffer.N_bytes, 0x1001, true, 1, 0);
#endif

  return SRSRAN_SUCCESS;
}

// Test for Short BSR CE + PHR CE
int mac_sch_pdu_pack_test10()
{
  auto& mac_logger = srslog::fetch_basic_logger("MAC");

  const uint32_t  pdu_size = 4;
  srsran::sch_pdu pdu(10, mac_logger);

  uint8_t tv[pdu_size] = {0x3d, 0x1a, 0x1f, 0x21};

  byte_buffer_t buffer;
  pdu.init_tx(&buffer, pdu_size, true);

  TESTASSERT(pdu.rem_size() == pdu_size);
  TESTASSERT(pdu.get_pdu_len() == pdu_size);
  TESTASSERT(pdu.get_sdu_space() == pdu_size - 1);
  TESTASSERT(pdu.get_current_sdu_ptr() == buffer.msg);

  // Try to add short BSR CE
  uint32_t buff_size[4] = {1132, 0, 0, 0};
  TESTASSERT(pdu.new_subh());
  TESTASSERT(pdu.get()->set_bsr(buff_size, srsran::ul_sch_lcid::SHORT_BSR));

  // Try to add PHR CE
  TESTASSERT(pdu.new_subh());
  TESTASSERT(pdu.get()->set_phr(10.1));

  // write PDU
  pdu.write_packet(mac_logger);

  // compare with tv
  TESTASSERT(memcmp(buffer.msg, tv, buffer.N_bytes) == 0);

  // log
  mac_logger.info(buffer.msg, buffer.N_bytes, "MAC PDU (%d B):", buffer.N_bytes);

#if HAVE_PCAP
  pcap_handle->write_ul_crnti(buffer.msg, buffer.N_bytes, 0x1001, true, 1, 0);
#endif

  return SRSRAN_SUCCESS;
}

// Pack test for short MAC PDU, trying to add long BSR but no space left
int mac_sch_pdu_pack_test11()
{
  auto& mac_logger = srslog::fetch_basic_logger("MAC");

  const uint32_t  pdu_size = 3;
  srsran::sch_pdu pdu(10, mac_logger);

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
  TESTASSERT(pdu.get()->set_bsr(buff_size, srsran::ul_sch_lcid::LONG_BSR) == false);

  // Adding BSR failed, remove subheader again
  pdu.del_subh();

  // write PDU
  pdu.write_packet(mac_logger);

  // compare with tv
  TESTASSERT(memcmp(buffer.msg, tv, buffer.N_bytes) == 0);

  // log
  mac_logger.info(buffer.msg, buffer.N_bytes, "MAC PDU (%d B):", buffer.N_bytes);

#if HAVE_PCAP
  pcap_handle->write_ul_crnti(buffer.msg, buffer.N_bytes, 0x1001, true, 1, 0);
#endif

  return SRSRAN_SUCCESS;
}

// Test for checking error cases
int mac_sch_pdu_pack_error_test()
{
  rlc_dummy rlc;
  auto&     mac_logger = srslog::fetch_basic_logger("MAC");

  // create RLC SDUs
  rlc.write_sdu(1, 8);

  const uint32_t  pdu_size = 150;
  srsran::sch_pdu pdu(10, mac_logger);

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
  TESTASSERT(pdu.get()->set_sdu(1, 8, &rlc) == SRSRAN_ERROR);

  // writing PDU fails
  TESTASSERT(pdu.write_packet(mac_logger) == nullptr);

  // reset buffer
  buffer.clear();

  // write SDU again
  TESTASSERT(pdu.get() != nullptr);
  TESTASSERT(pdu.get()->set_sdu(1, 100, &rlc) == 8); // only 8 bytes in RLC buffer

  // writing PDU fails
  TESTASSERT(pdu.write_packet(mac_logger));

  // log
  mac_logger.info(buffer.msg, buffer.N_bytes, "MAC PDU (%d B):", buffer.N_bytes);

#if HAVE_PCAP
  pcap_handle->write_ul_crnti(buffer.msg, buffer.N_bytes, 0x1001, true, 1, 0);
#endif

  return SRSRAN_SUCCESS;
}

int mac_mch_pdu_pack_test1()
{
  static uint8_t tv[] = {0x3e, 0x02, 0x20, 0x05, 0x21, 0x0a, 0x1f, 0x0f, 0xff, 0x01, 0x02, 0x03, 0x04, 0x05, 0x02,
                         0x04, 0x06, 0x08, 0x0a, 0x0c, 0x0e, 0x10, 0x12, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  auto& mac_logger = srslog::fetch_basic_logger("MAC");

  const uint32_t  pdu_size = 30;
  srsran::mch_pdu mch_pdu(10, mac_logger);
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
  TESTASSERT(mch_pdu.write_packet(mac_logger) == buffer.msg);

  // log
  mac_logger.info(buffer.msg, buffer.N_bytes, "MAC PDU (%d B):", buffer.N_bytes);

#if HAVE_PCAP
  pcap_handle->write_ul_crnti(buffer.msg, buffer.N_bytes, 0x1001, true, 1, 0);
#endif

  // compare with TV
  TESTASSERT(memcmp(buffer.msg, tv, buffer.N_bytes) == 0);

#if HAVE_PCAP
  pcap_handle->write_ul_crnti(tv, sizeof(tv), 0x1001, true, 1, 0);
#endif

  return SRSRAN_SUCCESS;
}

// Parsing a corrupted MAC PDU and making sure the PDU is reset and not further processed
int mac_sch_pdu_unpack_test1()
{
  static uint8_t tv[] = {0x3f, 0x3f, 0x21, 0x3f, 0x03, 0x00, 0x04, 0x00, 0x04};

  srsran::sch_pdu pdu(10, srslog::fetch_basic_logger("MAC"));
  pdu.init_rx(sizeof(tv), false);
  pdu.parse_packet(tv);

  // make sure this PDU is reset and will not be further processed
  TESTASSERT(pdu.nof_subh() == 0);
  TESTASSERT(pdu.next() == false);

#if HAVE_PCAP
  pcap_handle->write_ul_crnti(tv, sizeof(tv), 0x1001, true, 1, 0);
#endif

  return SRSRAN_SUCCESS;
}

// Parsing a (corrupted) MAC PDU that only contains padding
int mac_sch_pdu_unpack_test2()
{
  static uint8_t tv[] = {0x3f, 0x3f};

  srsran::sch_pdu pdu(20, srslog::fetch_basic_logger("MAC"));
  pdu.init_rx(sizeof(tv), false);
  pdu.parse_packet(tv);

  // make sure this PDU is reset and will not be further processed
  TESTASSERT(pdu.nof_subh() == 0);
  TESTASSERT(pdu.next() == false);

#if HAVE_PCAP
  pcap_handle->write_ul_crnti(tv, sizeof(tv), 0x1001, true, 1, 0);
#endif

  return SRSRAN_SUCCESS;
}

// Unpacking of PDU containing Timing Advance (TA) CE
int mac_sch_pdu_unpack_test3()
{
  static uint8_t tv[] = {0x3d, 0x1f, 0x1f, 0x00};

  srsran::sch_pdu pdu(20, srslog::fetch_basic_logger("MAC"));
  pdu.init_rx(sizeof(tv), false);
  pdu.parse_packet(tv);

  TESTASSERT(pdu.nof_subh() == 2);
  while (pdu.next()) {
    if (!pdu.get()->is_sdu() && pdu.get()->dl_sch_ce_type() == srsran::dl_sch_lcid::TA_CMD) {
      TESTASSERT(pdu.get()->get_ta_cmd() == 31);
    }
  }

  fmt::memory_buffer buffer;
  pdu.to_string(buffer);
  std::cout << fmt::to_string(buffer) << std::endl;

#if HAVE_PCAP
  pcap_handle->write_dl_crnti(tv, sizeof(tv), 0x1001, true, 1, 0);
#endif

  return SRSRAN_SUCCESS;
}

int mac_slsch_pdu_unpack_test1()
{
  // SL-SCH PDU captures from UXM 5G CV2X
  static uint8_t tv[] = {0x30, 0xED, 0x0E, 0x03, 0x00, 0x00, 0x01, 0x21, 0x2B, 0x1F, 0x9F, 0x75, 0x76,
                         0x87, 0x00, 0x00, 0x8E, 0x9E, 0xA5, 0xFD, 0xA1, 0xA3, 0xA7, 0xA9, 0x7F, 0x68,
                         0x6C, 0xB8, 0xBF, 0x0F, 0x96, 0x89, 0x2E, 0xDC, 0x80, 0x64, 0x06, 0x40, 0x00,
                         0x18, 0x6A, 0x07, 0x20, 0x7C, 0xE1, 0xE0, 0x04, 0x40, 0x02, 0x04, 0x80, 0x60,
                         0x2C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

#if HAVE_PCAP
  pcap_handle->write_sl_crnti(tv, sizeof(tv), CRNTI, true, 1, 0);
#endif

  return SRSRAN_SUCCESS;
}

int main(int argc, char** argv)
{
#if HAVE_PCAP
  pcap_handle = std::unique_ptr<srsran::mac_pcap>(new srsran::mac_pcap());
  pcap_handle->open("mac_pdu_test.pcap");
#endif
  auto& mac_logger = srslog::fetch_basic_logger("MAC", false);
  mac_logger.set_level(srslog::basic_levels::debug);
  mac_logger.set_hex_dump_max_size(-1);
  auto& rlc_logger = srslog::fetch_basic_logger("RLC", false);
  rlc_logger.set_level(srslog::basic_levels::debug);
  rlc_logger.set_hex_dump_max_size(-1);

  srslog::init();

  TESTASSERT(mac_rar_pdu_unpack_test1() == SRSRAN_SUCCESS);
  TESTASSERT(mac_rar_pdu_unpack_test2() == SRSRAN_SUCCESS);
  TESTASSERT(mac_rar_pdu_unpack_test3() == SRSRAN_SUCCESS);
  TESTASSERT(mac_rar_pdu_pack_test1() == SRSRAN_SUCCESS);
  TESTASSERT(mac_rar_pdu_pack_test2() == SRSRAN_SUCCESS);

  TESTASSERT(mac_sch_pdu_pack_test1() == SRSRAN_SUCCESS);
  TESTASSERT(mac_sch_pdu_pack_test2() == SRSRAN_SUCCESS);
  TESTASSERT(mac_sch_pdu_pack_test3() == SRSRAN_SUCCESS);
  TESTASSERT(mac_sch_pdu_pack_test4() == SRSRAN_SUCCESS);
  TESTASSERT(mac_sch_pdu_pack_test5() == SRSRAN_SUCCESS);
  TESTASSERT(mac_sch_pdu_pack_test6() == SRSRAN_SUCCESS);
  TESTASSERT(mac_sch_pdu_pack_test7() == SRSRAN_SUCCESS);
  TESTASSERT(mac_sch_pdu_pack_test8() == SRSRAN_SUCCESS);
  TESTASSERT(mac_sch_pdu_pack_test9() == SRSRAN_SUCCESS);
  TESTASSERT(mac_sch_pdu_pack_test10() == SRSRAN_SUCCESS);
  TESTASSERT(mac_sch_pdu_pack_test11() == SRSRAN_SUCCESS);

  TESTASSERT(mac_sch_pdu_pack_error_test() == SRSRAN_SUCCESS);

  TESTASSERT(mac_mch_pdu_pack_test1() == SRSRAN_SUCCESS);

  TESTASSERT(mac_sch_pdu_unpack_test1() == SRSRAN_SUCCESS);
  TESTASSERT(mac_sch_pdu_unpack_test2() == SRSRAN_SUCCESS);
  TESTASSERT(mac_sch_pdu_unpack_test3() == SRSRAN_SUCCESS);

  TESTASSERT(mac_slsch_pdu_unpack_test1() == SRSRAN_SUCCESS);

  return SRSRAN_SUCCESS;
}
