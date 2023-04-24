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
  uint32_t read_pdu(uint32_t lcid, uint8_t* payload, uint32_t nof_bytes)
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

// Unpacking of UL-SCH PDU containing Padding, PHR and two SDUs for same LCID
int mac_sch_pdu_unpack_test4()
{
  static uint8_t tv[] = {
      0x3f, 0x3a, 0x23, 0x09, 0x03, 0x1e, 0xd9, 0xd0, 0x84, 0x1d, 0xc4, 0x77, 0xc7, 0x3b, 0xf0, 0x9a, 0x31, 0xc6, 0xb7,
      0x48, 0x61, 0x6e, 0xc3, 0x97, 0x48, 0xa6, 0xe3, 0xac, 0xc0, 0x75, 0x9f, 0xb2, 0xc8, 0xed, 0xb0, 0xad, 0xcc, 0xc2,
      0x0d, 0x28, 0xdd, 0xbb, 0x4b, 0x4c, 0xc5, 0xfc, 0x52, 0x40, 0xc2, 0x09, 0x89, 0x23, 0x77, 0x4c, 0xba, 0xbf, 0xf2,
      0x9d, 0x6b, 0xb2, 0x12, 0x0b, 0x64, 0xda, 0xf8, 0x14, 0x88, 0xc4, 0xd7, 0x95, 0xec, 0xb4, 0x50, 0x37, 0x00, 0x15,
      0x33, 0x52, 0x56, 0xf5, 0x5b, 0xdf, 0x18, 0xd2, 0x2b, 0xd2, 0x92, 0x1d, 0x6f, 0xfd, 0xcf, 0x82, 0x08, 0x33, 0x5c,
      0x00, 0x48, 0xe4, 0xc4, 0x1f, 0x79, 0xb0, 0xd3, 0xca, 0xe8, 0xd3, 0xdf, 0x1b, 0x25, 0x35, 0x11, 0x80, 0x14, 0x29,
      0x52, 0x3f, 0xfc, 0xe4, 0x5c, 0x6b, 0xe2, 0x2b, 0xed, 0xea, 0x5f, 0x4a, 0xeb, 0xa7, 0x2e, 0xaf, 0xc6, 0xa8, 0x60,
      0x99, 0x72, 0x48, 0x6c, 0x51, 0x63, 0x91, 0x87, 0x74, 0x11, 0x9b, 0x9e, 0x63, 0xdb, 0x9a, 0x48, 0x37, 0x05, 0x2a,
      0x63, 0xf3, 0x14, 0xc2, 0x3d, 0xff, 0x69, 0x6b, 0xaf, 0x2f, 0x13, 0x0f, 0xc8, 0x85, 0x57, 0x34, 0xd7, 0xba, 0xc5,
      0x5e, 0x2f, 0xd6, 0xf9, 0xcd, 0x39, 0xd4, 0x67, 0x81, 0x0c, 0x6c, 0xab, 0x6f, 0x5f, 0xc0, 0x31, 0x9c, 0xbf, 0x9a,
      0x08, 0x6e, 0xc9, 0x1b, 0x7d, 0x91, 0xa1, 0xd4, 0xc5, 0x78, 0x8f, 0x8a, 0xd6, 0xbe, 0x60, 0xcf, 0x8b, 0x99, 0xa4,
      0xf2, 0x1b, 0xb0, 0x5e, 0xc6, 0x1f, 0xbe, 0x86, 0x50, 0x5a, 0x46, 0xea, 0x62, 0xb4, 0xb3, 0x7e, 0x32, 0x44, 0x1f,
      0x06, 0x09, 0x97, 0x95, 0x93, 0x6d, 0x53, 0xf3, 0x3c, 0xde, 0x8c, 0xe0, 0xd0, 0xa7, 0x90, 0x2f, 0x6e, 0xaf, 0xed,
      0xf4, 0xff, 0x47, 0x3a, 0xe9, 0xaa, 0xef, 0x9c, 0x28, 0x21, 0xe0, 0x47, 0x27, 0xe9, 0xde, 0xbd, 0x7c, 0x4b, 0x10,
      0x6f, 0x87, 0xef, 0xfc, 0x68, 0xbf, 0xa3, 0xf8, 0xee, 0x11, 0xa8, 0xdb, 0x06, 0xa7, 0x23, 0x40, 0x91, 0xcd, 0x2f,
      0x2d, 0xf5, 0x50, 0x0e, 0x3c, 0x78, 0xf7, 0x1a, 0x35, 0x74, 0x65, 0x45, 0xe3, 0xec, 0x34, 0xdf, 0x54, 0xf4, 0x83,
      0x4d, 0xe2, 0x94, 0xf5, 0xbe, 0x9a, 0x9c, 0xe1, 0xdb, 0x2d, 0xae, 0x0a, 0x5b, 0xa3, 0x5b, 0x69, 0xdf, 0xd3, 0x60,
      0xf9, 0x08, 0xd4, 0x5e, 0x4d, 0xb8, 0x4a, 0x82, 0x97, 0x9f, 0x76, 0x1a, 0xec, 0x58, 0xaf, 0xe1, 0x16, 0x49, 0x7d,
      0xf7, 0x24, 0xab, 0xa5, 0x2f, 0x06, 0x48, 0x8a, 0x6f, 0x27, 0x5d, 0xcf, 0x20, 0x65, 0xa4, 0x7e, 0xb2, 0x5c, 0xc9,
      0x34, 0xf3, 0x68, 0xaa, 0x0e, 0x54, 0x03, 0xbd, 0x35, 0x19, 0x06, 0xb2, 0x11, 0x2b, 0x5d, 0xb6, 0x5a, 0x63, 0xff,
      0xe4, 0xd2, 0x26, 0x41, 0xa2, 0x47, 0xa6, 0x46, 0xc5, 0x58, 0xa2, 0x8e, 0x8d, 0x95, 0xf6, 0x37, 0xa3, 0x4a, 0x3a,
      0x60, 0x7f, 0x54, 0x67, 0x32, 0x65, 0x92, 0x8f, 0x1b, 0xec, 0xf3, 0x1a, 0xd0, 0xc5, 0x41, 0x11, 0x67, 0x88, 0xb7,
      0xad, 0x4d, 0x0f, 0x4f, 0xdc, 0x9c, 0xe5, 0xd2, 0xd4, 0x88, 0x1d, 0x0e, 0xe9, 0x9c, 0x62, 0x50, 0xce, 0xc7, 0xe2,
      0x5e, 0xe3, 0xce, 0x51, 0xfd, 0x9e, 0x16, 0x3e, 0xaf, 0x7e, 0xc6, 0x66, 0x2b, 0x14, 0x75, 0x7b, 0xf0, 0x12, 0x60,
      0xc2, 0xe6, 0xe8, 0xdf, 0xf4, 0xd1, 0x7c, 0x57, 0x21, 0x4a, 0x1e, 0x03, 0xa8, 0x01, 0xd1, 0xf9, 0xff, 0x6f, 0x10,
      0x3d, 0x1e, 0x8e, 0x04, 0x84, 0xb9, 0x18, 0xfa, 0x34, 0x08, 0x0c, 0x94, 0xca, 0xf2, 0x7d, 0xaa, 0xe6, 0x4e, 0x26,
      0x3d, 0x70, 0x70, 0x5c, 0x73, 0x19, 0x5d, 0x45, 0x12, 0x5c, 0xb4, 0x22, 0x9a, 0xd3, 0xb0, 0x9e, 0x57, 0x6a, 0xb6,
      0x51, 0x9e, 0xbe, 0x5d, 0x33, 0x88, 0x4f, 0xb0, 0x32, 0x36, 0xfe, 0x58, 0x73, 0x6e, 0xc9, 0xcf, 0xe2, 0xe2, 0x2d,
      0x27, 0xf4, 0x89, 0xdb, 0x17, 0x23, 0xae, 0xc7, 0xc1, 0x06, 0x31, 0x77, 0x57, 0xd0, 0x35, 0xb5, 0x03, 0xbe, 0x04,
      0xb3, 0xf0, 0x3a, 0xb1, 0x49, 0xae, 0x20, 0x12, 0x7d, 0x02, 0xf4, 0xaa, 0x29, 0xe8, 0x34, 0x04, 0xff, 0x57, 0xb3,
      0xc7, 0x19, 0xb9, 0xf8, 0x90, 0x10, 0xc8, 0xc6, 0xc5, 0xcb, 0x84, 0xca, 0x7e, 0x74, 0x04, 0x30, 0xbd, 0xb2, 0x50,
      0xcf, 0x30, 0x52, 0xc3, 0xda, 0x7b, 0xac, 0x0e, 0x7f, 0xab, 0x66, 0x32, 0x72, 0x7f, 0xeb, 0x6b, 0x0f, 0xfc, 0x33,
      0xd5, 0xc1, 0xff, 0x59, 0x8b, 0x7d, 0xce, 0x90, 0xad, 0x8b, 0x42, 0xfd, 0x5b, 0x72, 0x4f, 0x1e, 0x4d, 0xca, 0xca,
      0x5b, 0x4a, 0x76, 0xc1, 0x7c, 0xe8, 0x40, 0x68, 0x53, 0x50, 0x64, 0x87, 0x25, 0x25, 0x86, 0x7f, 0xb1, 0x03, 0x4d,
      0x41, 0xb1, 0xd8, 0x83, 0xae, 0x33, 0xf6, 0xfe, 0x52, 0x43, 0xc8, 0x1c, 0x9e, 0x12, 0x92, 0x60, 0x8f, 0x7b, 0xa0,
      0xf7, 0xce, 0xf0, 0x5b, 0x55, 0x16, 0x80, 0xdb, 0x95, 0x31, 0xdf, 0xe2, 0x72, 0x90, 0xba, 0xf6, 0x3e, 0xee, 0xec,
      0x3c, 0x40, 0x2f, 0x05, 0x5c, 0xcd, 0x17, 0xef, 0x2d, 0xa6, 0x6a, 0xce, 0x9d, 0x38, 0xbe, 0xf8, 0x8e, 0xd4, 0x79,
      0x69, 0x69, 0xaa, 0x48, 0x4b, 0x1d, 0xd8, 0x06, 0x13, 0x17, 0xf4, 0xff, 0x53, 0x34, 0x2e, 0x58, 0x90, 0xfb, 0x70,
      0x7f, 0x29, 0x16, 0xe9, 0xf7, 0xb4, 0x22, 0xb5, 0xac, 0xb0, 0x8a, 0x25, 0x19, 0xf3, 0xd0, 0x62, 0x3f, 0xed, 0x3a,
      0x45, 0x00, 0x51, 0x39, 0xff, 0xa5, 0x6d, 0x2d, 0xfd, 0xfd, 0x28, 0x6d, 0x7d, 0x51, 0x84, 0x66, 0x48, 0x38, 0x88,
      0xfe, 0xe4, 0x38, 0x88, 0x0a, 0x52, 0x7b, 0xda, 0xb4, 0xba, 0xc7, 0xee, 0xff, 0xc7, 0x40, 0x38, 0xc6, 0xe5, 0xa5,
      0xf3, 0xe2, 0xa3, 0x1b, 0x50, 0x20, 0x6d, 0xd4, 0x86, 0xb5, 0x0c, 0x0f, 0xb3, 0xf0, 0x47, 0x3b, 0xfa, 0x99, 0xb7,
      0xd4, 0x4d, 0x71, 0x9b, 0x3c, 0x71, 0x62, 0x7c, 0xa9, 0x28, 0x61, 0x4f, 0x1b, 0x43, 0xf2, 0x37, 0x93, 0x12, 0xa4,
      0x67, 0x98, 0x59, 0x73, 0xa7, 0x0d, 0x64, 0xef, 0x48, 0x5e, 0x88, 0xff, 0x33, 0xd6, 0x71, 0xce, 0x12, 0xe2, 0x31,
      0x8e, 0x8b, 0x59, 0xef, 0xda, 0x75, 0x32, 0xcc, 0xac, 0xc6, 0xde, 0x50, 0x2d, 0x77, 0xa9, 0xa1, 0x1e, 0xb6, 0x05,
      0x0d, 0xff, 0x63, 0x96, 0xfe, 0x96, 0x6c, 0x6f, 0x65, 0x7e, 0x51, 0x96, 0x0c, 0xdd, 0xef, 0xfb, 0xb7, 0x64, 0x2d,
      0x84, 0x10, 0xf3, 0x62, 0x60, 0x21, 0xd9, 0x0a, 0xc2, 0xf8, 0xc0, 0xc7, 0x05, 0xbf, 0x2a, 0x9b, 0xbe, 0xc1, 0x07,
      0x2d, 0x26, 0x85, 0x7f, 0xbc, 0x91, 0x5c, 0xab, 0x8c, 0x13, 0x10, 0xba, 0x97, 0x20, 0xad, 0xfa, 0x81, 0xce, 0xd3,
      0x8b, 0x90, 0xcb, 0x4b, 0x57, 0xd1, 0x0b, 0x82, 0x6c, 0xc9, 0x43, 0x74, 0xf6, 0x69, 0xf9, 0x75, 0x25, 0x8b, 0xd1,
      0xd0, 0x17, 0xe5, 0xe0, 0xd1, 0x7c, 0x01, 0x7f, 0x76, 0x82, 0x4d, 0x4a, 0x0d, 0xde, 0x15, 0x58, 0x35, 0xe6, 0x63,
      0xb7, 0x53, 0x2c, 0xfa, 0xc7, 0x23, 0x63, 0xc0, 0x98, 0x88, 0x4b, 0x6a, 0x59, 0x63, 0x4f, 0x39, 0x34, 0xcb, 0x3a,
      0xb3, 0x42, 0xbc, 0x01, 0x8c, 0xc9, 0xdf, 0xa1, 0x22, 0x14, 0x88, 0x85, 0xcc, 0xdb, 0xb2, 0xc6, 0xa2, 0xd5, 0x2a,
      0x62, 0x6d, 0xb2, 0xae, 0xd7, 0x0b, 0x11, 0x26, 0x45, 0x45, 0xf2, 0x7f, 0xf9, 0x34, 0x3c, 0xfa, 0xc0, 0x05, 0xd9,
      0x61, 0x27, 0xed, 0xe9, 0xad, 0xb9, 0xc4, 0x5f, 0x80, 0x66, 0x34, 0xaa, 0xc9, 0xa1, 0x5c, 0x77, 0x79, 0x68, 0x88,
      0x9f, 0xad, 0xcd, 0x91, 0x2c, 0xc6, 0xc5, 0x68, 0xc0, 0x85, 0x6e, 0x99, 0xe7, 0x95, 0x87, 0xd0, 0x42, 0x40, 0x95,
      0xa1, 0xc0, 0xfb, 0xd5, 0x6a, 0xc4, 0x77, 0xc7, 0x3b, 0xf0, 0x2f, 0xc3, 0x8f, 0xdc, 0x91, 0x21, 0x08, 0x57};

  uint8_t* sdu1     = &tv[6];
  uint32_t sdu1_len = 9;

  uint8_t* sdu2     = &tv[15];
  uint32_t sdu2_len = 1048;

  srsran::sch_pdu pdu(5, srslog::fetch_basic_logger("MAC"));
  pdu.init_rx(sizeof(tv), true);
  pdu.parse_packet(tv);

  TESTASSERT(pdu.nof_subh() == 4);

  // Padding
  TESTASSERT(pdu.next());
  TESTASSERT(pdu.get()->is_sdu() == false);
  TESTASSERT(pdu.get()->ul_sch_ce_type() == srsran::ul_sch_lcid::PADDING);

  // PHR
  TESTASSERT(pdu.next());
  TESTASSERT(pdu.get()->is_sdu() == false);
  TESTASSERT(pdu.get()->ul_sch_ce_type() == srsran::ul_sch_lcid::PHR_REPORT);
  TESTASSERT(pdu.get()->get_phr() == 7);

  // SDU1
  TESTASSERT(pdu.next());
  TESTASSERT(pdu.get()->is_sdu() == true);
  TESTASSERT(pdu.get()->get_sdu_lcid() == 3);
  TESTASSERT(pdu.get()->get_payload_size() == sdu1_len);
  TESTASSERT(memcmp(sdu1, pdu.get()->get_sdu_ptr(), sdu1_len) == 0);

  // SDU2
  TESTASSERT(pdu.next());
  TESTASSERT(pdu.get()->is_sdu() == true);
  TESTASSERT(pdu.get()->get_sdu_lcid() == 3);
  TESTASSERT(pdu.get()->get_payload_size() == sdu2_len);
  TESTASSERT(memcmp(sdu2, pdu.get()->get_sdu_ptr(), sdu2_len) == 0);

  // end of PDU
  TESTASSERT(pdu.next() == false);

  fmt::memory_buffer buffer;
  pdu.to_string(buffer);
  std::cout << fmt::to_string(buffer) << std::endl;

#if HAVE_PCAP
  pcap_handle->write_ul_crnti(tv, sizeof(tv), 0x1001, true, 1, 0);
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
  TESTASSERT(mac_sch_pdu_unpack_test4() == SRSRAN_SUCCESS);

  TESTASSERT(mac_slsch_pdu_unpack_test1() == SRSRAN_SUCCESS);

  return SRSRAN_SUCCESS;
}
