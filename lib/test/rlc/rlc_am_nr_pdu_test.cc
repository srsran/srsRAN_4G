/**
 * Copyright 2013-2022 Software Radio Systems Limited
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
#include "srsran/config.h"
#include "srsran/rlc/rlc.h"
#include "srsran/rlc/rlc_am_nr_packing.h"

#include <array>
#include <getopt.h>
#include <iostream>
#include <memory>
#include <vector>

#define PCAP_CRNTI (0x1001)
#define PCAP_TTI (666)

using namespace srsran;

#include "srsran/common/mac_pcap.h"
#include "srsran/mac/mac_rar_pdu_nr.h"
#include "srsran/mac/mac_sch_pdu_nr.h"
static std::unique_ptr<srsran::mac_pcap> pcap_handle = nullptr;

int write_pdu_to_pcap(const uint32_t lcid, const uint8_t* payload, const uint32_t len)
{
  if (pcap_handle) {
    byte_buffer_t          tx_buffer;
    srsran::mac_sch_pdu_nr tx_pdu;
    tx_pdu.init_tx(&tx_buffer, len + 10);
    tx_pdu.add_sdu(lcid, payload, len);
    tx_pdu.pack();
    pcap_handle->write_dl_crnti_nr(tx_buffer.msg, tx_buffer.N_bytes, PCAP_CRNTI, true, PCAP_TTI);
    return SRSRAN_SUCCESS;
  }
  return SRSRAN_ERROR;
}

template <std::size_t N>
srsran::byte_buffer_t make_pdu_and_log(const std::array<uint8_t, N>& tv)
{
  srsran::byte_buffer_t pdu;
  memcpy(pdu.msg, tv.data(), tv.size());
  pdu.N_bytes = tv.size();
  write_pdu_to_pcap(4, tv.data(), tv.size());
  return pdu;
}

void corrupt_pdu_header(srsran::byte_buffer_t& pdu, const uint32_t header_len, const uint32_t payload_len)
{
  // clear header only
  for (uint32_t i = 0; i < header_len; i++) {
    pdu.msg[i] = 0xaa;
  }
  pdu.msg += header_len;
  pdu.N_bytes = payload_len;
}

// RLC AM PDU 12bit with complete SDU
int rlc_am_nr_pdu_test1()
{
  test_delimit_logger                           delimiter("PDU test 1");
  const int                                     header_len = 2, payload_len = 4;
  std::array<uint8_t, header_len + payload_len> tv  = {0x80, 0x00, 0x11, 0x22, 0x33, 0x44};
  srsran::byte_buffer_t                         pdu = make_pdu_and_log(tv);

  // unpack PDU
  rlc_am_nr_pdu_header_t header = {};
  TESTASSERT(rlc_am_nr_read_data_pdu_header(&pdu, srsran::rlc_am_nr_sn_size_t::size12bits, &header) != 0);
  TESTASSERT(header.si == rlc_nr_si_field_t::full_sdu);

  // clear header
  corrupt_pdu_header(pdu, header_len, payload_len);

  // pack again
  TESTASSERT(rlc_am_nr_write_data_pdu_header(header, &pdu) == header_len);
  TESTASSERT(pdu.N_bytes == tv.size());
  TESTASSERT(memcmp(pdu.msg, tv.data(), pdu.N_bytes) == 0);

  return SRSRAN_SUCCESS;
}

// RLC AM PDU 12bit first segment of SDU with P flag and SN 511
int rlc_am_nr_pdu_test2()
{
  test_delimit_logger                           delimiter("PDU test 2");
  const int                                     header_len = 2, payload_len = 4;
  std::array<uint8_t, header_len + payload_len> tv  = {0xd1, 0xff, 0x11, 0x22, 0x33, 0x44};
  srsran::byte_buffer_t                         pdu = make_pdu_and_log(tv);

  // unpack PDU
  rlc_am_nr_pdu_header_t header = {};
  TESTASSERT(rlc_am_nr_read_data_pdu_header(&pdu, srsran::rlc_am_nr_sn_size_t::size12bits, &header) != 0);
  TESTASSERT(header.si == rlc_nr_si_field_t::first_segment);
  TESTASSERT(header.sn == 511);
  TESTASSERT(header.so == 0);

  // clear header
  corrupt_pdu_header(pdu, header_len, payload_len);

  // pack again
  TESTASSERT(rlc_am_nr_write_data_pdu_header(header, &pdu) == header_len);
  TESTASSERT(pdu.N_bytes == tv.size());

  write_pdu_to_pcap(4, pdu.msg, pdu.N_bytes);

  TESTASSERT(memcmp(pdu.msg, tv.data(), pdu.N_bytes) == 0);

  return SRSRAN_SUCCESS;
}

// RLC AM PDU 12bit last segment of SDU without P flag and SN 0x0404 and SO 0x0404 (1028)
int rlc_am_nr_pdu_test3()
{
  test_delimit_logger                           delimiter("PDU test 3");
  const int                                     header_len = 4, payload_len = 4;
  std::array<uint8_t, header_len + payload_len> tv  = {0xa4, 0x04, 0x04, 0x04, 0x11, 0x22, 0x33, 0x44};
  srsran::byte_buffer_t                         pdu = make_pdu_and_log(tv);

  // unpack PDU
  rlc_am_nr_pdu_header_t header = {};
  TESTASSERT(rlc_am_nr_read_data_pdu_header(&pdu, srsran::rlc_am_nr_sn_size_t::size12bits, &header) != 0);
  TESTASSERT(header.si == rlc_nr_si_field_t::last_segment);
  TESTASSERT(header.sn == 1028);
  TESTASSERT(header.so == 1028);

  // clear header
  corrupt_pdu_header(pdu, header_len, payload_len);

  // pack again
  TESTASSERT(rlc_am_nr_write_data_pdu_header(header, &pdu) == header_len);
  TESTASSERT(pdu.N_bytes == tv.size());

  write_pdu_to_pcap(4, pdu.msg, pdu.N_bytes);

  TESTASSERT(memcmp(pdu.msg, tv.data(), pdu.N_bytes) == 0);

  return SRSRAN_SUCCESS;
}

// RLC AM PDU 18bit full SDU with P flag and SN 0x100000001000000010 (131586)
int rlc_am_nr_pdu_test4()
{
  test_delimit_logger                           delimiter("PDU test 4");
  const int                                     header_len = 3, payload_len = 4;
  std::array<uint8_t, header_len + payload_len> tv  = {0xc2, 0x02, 0x02, 0x11, 0x22, 0x33, 0x44};
  srsran::byte_buffer_t                         pdu = make_pdu_and_log(tv);

  // unpack PDU
  rlc_am_nr_pdu_header_t header = {};
  TESTASSERT(rlc_am_nr_read_data_pdu_header(&pdu, srsran::rlc_am_nr_sn_size_t::size18bits, &header) != 0);
  TESTASSERT(header.si == rlc_nr_si_field_t::full_sdu);
  TESTASSERT(header.sn == 131586);
  TESTASSERT(header.so == 0);

  // clear header
  corrupt_pdu_header(pdu, header_len, payload_len);

  // pack again
  TESTASSERT(rlc_am_nr_write_data_pdu_header(header, &pdu) == header_len);
  TESTASSERT(pdu.N_bytes == tv.size());

  write_pdu_to_pcap(4, pdu.msg, pdu.N_bytes);

  TESTASSERT(memcmp(pdu.msg, tv.data(), pdu.N_bytes) == 0);

  return SRSRAN_SUCCESS;
}

// RLC AM PDU 18bit middle part of SDU (SO 514) without P flag and SN 131327
int rlc_am_nr_pdu_test5()
{
  test_delimit_logger                           delimiter("PDU test 5");
  const int                                     header_len = 5, payload_len = 4;
  std::array<uint8_t, header_len + payload_len> tv  = {0xb2, 0x00, 0xff, 0x02, 0x02, 0x11, 0x22, 0x33, 0x44};
  srsran::byte_buffer_t                         pdu = make_pdu_and_log(tv);

  // unpack PDU
  rlc_am_nr_pdu_header_t header = {};
  TESTASSERT(rlc_am_nr_read_data_pdu_header(&pdu, srsran::rlc_am_nr_sn_size_t::size18bits, &header) != 0);
  TESTASSERT(header.si == rlc_nr_si_field_t::neither_first_nor_last_segment);
  TESTASSERT(header.sn == 131327);
  TESTASSERT(header.so == 514);

  // clear header
  corrupt_pdu_header(pdu, header_len, payload_len);

  // pack again
  TESTASSERT(rlc_am_nr_write_data_pdu_header(header, &pdu) == header_len);
  TESTASSERT(pdu.N_bytes == tv.size());

  write_pdu_to_pcap(4, pdu.msg, pdu.N_bytes);

  TESTASSERT(memcmp(pdu.msg, tv.data(), pdu.N_bytes) == 0);

  return SRSRAN_SUCCESS;
}

// Malformed RLC AM PDU 18bit with reserved bits set
int rlc_am_nr_pdu_test6()
{
  test_delimit_logger                           delimiter("PDU test 6");
  const int                                     header_len = 5, payload_len = 4;
  std::array<uint8_t, header_len + payload_len> tv  = {0xb7, 0x00, 0xff, 0x02, 0x02, 0x11, 0x22, 0x33, 0x44};
  srsran::byte_buffer_t                         pdu = make_pdu_and_log(tv);

  // unpack PDU
  rlc_am_nr_pdu_header_t header = {};
  TESTASSERT(rlc_am_nr_read_data_pdu_header(&pdu, srsran::rlc_am_nr_sn_size_t::size18bits, &header) == 0);
  TESTASSERT(header.sn == 0);

  return SRSRAN_SUCCESS;
}

///< Control PDU tests (12bit SN)
// Status PDU for 12bit SN with ACK_SN=2065 and no further NACK_SN (E1 bit not set)
int rlc_am_nr_control_pdu_12bit_sn_test1()
{
  test_delimit_logger      delimiter("Control PDU (12bit SN) test 1");
  const int                len = 3;
  std::array<uint8_t, len> tv  = {0x08, 0x11, 0x00};
  srsran::byte_buffer_t    pdu = make_pdu_and_log(tv);

  TESTASSERT(rlc_am_is_control_pdu(pdu.msg) == true);

  // unpack PDU
  rlc_am_nr_status_pdu_t status_pdu(srsran::rlc_am_nr_sn_size_t::size12bits);
  TESTASSERT(rlc_am_nr_read_status_pdu(&pdu, srsran::rlc_am_nr_sn_size_t::size12bits, &status_pdu) == SRSRAN_SUCCESS);
  TESTASSERT(status_pdu.ack_sn == 2065);
  TESTASSERT(status_pdu.nacks.size() == 0);

  // reset status PDU
  pdu.clear();

  // pack again
  TESTASSERT(rlc_am_nr_write_status_pdu(status_pdu, srsran::rlc_am_nr_sn_size_t::size12bits, &pdu) == SRSRAN_SUCCESS);
  TESTASSERT(pdu.N_bytes == tv.size());

  write_pdu_to_pcap(4, pdu.msg, pdu.N_bytes);

  TESTASSERT(memcmp(pdu.msg, tv.data(), pdu.N_bytes) == 0);

  return SRSRAN_SUCCESS;
}

// Status PDU for 12bit SN with ACK_SN=2065 and NACK_SN=273 (E1 bit set)
int rlc_am_nr_control_pdu_12bit_sn_test2()
{
  test_delimit_logger      delimiter("Control PDU (12bit SN) test 2");
  const int                len = 5;
  std::array<uint8_t, len> tv  = {0x08, 0x11, 0x80, 0x11, 0x10};
  srsran::byte_buffer_t    pdu = make_pdu_and_log(tv);

  TESTASSERT(rlc_am_is_control_pdu(pdu.msg) == true);

  // unpack PDU
  rlc_am_nr_status_pdu_t status_pdu(srsran::rlc_am_nr_sn_size_t::size12bits);
  TESTASSERT(rlc_am_nr_read_status_pdu(&pdu, srsran::rlc_am_nr_sn_size_t::size12bits, &status_pdu) == SRSRAN_SUCCESS);
  TESTASSERT(status_pdu.ack_sn == 2065);
  TESTASSERT(status_pdu.nacks.size() == 1);
  TESTASSERT(status_pdu.nacks[0].nack_sn == 273);

  // reset status PDU
  pdu.clear();

  // pack again
  TESTASSERT(rlc_am_nr_write_status_pdu(status_pdu, srsran::rlc_am_nr_sn_size_t::size12bits, &pdu) == SRSRAN_SUCCESS);
  TESTASSERT(pdu.N_bytes == tv.size());

  write_pdu_to_pcap(4, pdu.msg, pdu.N_bytes);

  TESTASSERT(memcmp(pdu.msg, tv.data(), pdu.N_bytes) == 0);

  return SRSRAN_SUCCESS;
}

// Status PDU for 12bit SN with ACK_SN=2065, NACK_SN=273, SO_START=2, SO_END=5, NACK_SN=275, SO_START=5, SO_END=0xFFFF
// E1 and E2 bit set on first NACK, only E2 on second.
int rlc_am_nr_control_pdu_12bit_sn_test3()
{
  test_delimit_logger      delimiter("Control PDU (12bit SN) test 3");
  const int                len = 15;
  std::array<uint8_t, len> tv  = {
      0x08, 0x11, 0x80, 0x11, 0x1c, 0x00, 0x02, 0x00, 0x05, 0x11, 0x34, 0x00, 0x05, 0xFF, 0xFF};
  srsran::byte_buffer_t pdu = make_pdu_and_log(tv);

  TESTASSERT(rlc_am_is_control_pdu(pdu.msg) == true);

  // unpack PDU
  rlc_am_nr_status_pdu_t status_pdu(srsran::rlc_am_nr_sn_size_t::size12bits);
  TESTASSERT(rlc_am_nr_read_status_pdu(&pdu, srsran::rlc_am_nr_sn_size_t::size12bits, &status_pdu) == SRSRAN_SUCCESS);
  TESTASSERT(status_pdu.ack_sn == 2065);
  TESTASSERT(status_pdu.nacks.size() == 2);
  TESTASSERT(status_pdu.nacks[0].nack_sn == 273);
  TESTASSERT(status_pdu.nacks[0].so_start == 2);
  TESTASSERT(status_pdu.nacks[0].so_end == 5);
  TESTASSERT(status_pdu.nacks[1].nack_sn == 275);
  TESTASSERT(status_pdu.nacks[1].so_start == 5);
  TESTASSERT(status_pdu.nacks[1].so_end == 0xFFFF);

  // reset status PDU
  pdu.clear();

  // pack again
  TESTASSERT(rlc_am_nr_write_status_pdu(status_pdu, srsran::rlc_am_nr_sn_size_t::size12bits, &pdu) == SRSRAN_SUCCESS);
  TESTASSERT(pdu.N_bytes == tv.size());

  write_pdu_to_pcap(4, pdu.msg, pdu.N_bytes);
  TESTASSERT(memcmp(pdu.msg, tv.data(), pdu.N_bytes) == 0);

  return SRSRAN_SUCCESS;
}

// Status PDU for 12bit SN with ACK_SN=2065, NACK_SN=273, SO_START=2, SO_END=5, NACK_SN=275
// E1 and E2 bit set on first NACK, neither E1 or E2 on the second.
int rlc_am_nr_control_pdu_12bit_sn_test4()
{
  test_delimit_logger      delimiter("Control PDU (12bit SN) test 4");
  const int                len = 11;
  std::array<uint8_t, len> tv  = {0x08, 0x11, 0x80, 0x11, 0x1c, 0x00, 0x02, 0x00, 0x05, 0x11, 0x30};
  srsran::byte_buffer_t    pdu = make_pdu_and_log(tv);

  TESTASSERT(rlc_am_is_control_pdu(pdu.msg) == true);

  // unpack PDU
  rlc_am_nr_status_pdu_t status_pdu(srsran::rlc_am_nr_sn_size_t::size12bits);
  TESTASSERT(rlc_am_nr_read_status_pdu(&pdu, srsran::rlc_am_nr_sn_size_t::size12bits, &status_pdu) == SRSRAN_SUCCESS);
  TESTASSERT(status_pdu.ack_sn == 2065);
  TESTASSERT(status_pdu.nacks.size() == 2);
  TESTASSERT(status_pdu.nacks[0].nack_sn == 273);
  TESTASSERT(status_pdu.nacks[0].has_so == true);
  TESTASSERT(status_pdu.nacks[0].so_start == 2);
  TESTASSERT(status_pdu.nacks[0].so_end == 5);
  TESTASSERT(status_pdu.nacks[1].nack_sn == 275);
  TESTASSERT(status_pdu.nacks[1].has_so == false);

  // reset status PDU
  pdu.clear();

  // pack again
  TESTASSERT(rlc_am_nr_write_status_pdu(status_pdu, srsran::rlc_am_nr_sn_size_t::size12bits, &pdu) == SRSRAN_SUCCESS);
  TESTASSERT(pdu.N_bytes == tv.size());

  write_pdu_to_pcap(4, pdu.msg, pdu.N_bytes);
  TESTASSERT(memcmp(pdu.msg, tv.data(), pdu.N_bytes) == 0);

  return SRSRAN_SUCCESS;
}

// Malformed Status PDU, with E1 still set at the end of the PDU
// 12bit SN with ACK_SN=2065, NACK_SN=273, SO_START=2, SO_END=5, NACK_SN=275, SO_START=5, SO_END=0xFFFF
// E1 and E2 bit set on first NACK, only E2 on second.
int rlc_am_nr_control_pdu_12bit_sn_test5()
{
  test_delimit_logger      delimiter("Control PDU (12bit SN) test 5");
  const int                len = 15;
  std::array<uint8_t, len> tv  = {
      0x08, 0x11, 0x80, 0x11, 0x1c, 0x00, 0x02, 0x00, 0x05, 0x11, 0x3c, 0x00, 0x05, 0xFF, 0xFF};
  srsran::byte_buffer_t pdu = make_pdu_and_log(tv);

  TESTASSERT(rlc_am_is_control_pdu(pdu.msg) == true);

  // unpack PDU
  rlc_am_nr_status_pdu_t status_pdu(srsran::rlc_am_nr_sn_size_t::size12bits);
  TESTASSERT(rlc_am_nr_read_status_pdu(&pdu, srsran::rlc_am_nr_sn_size_t::size12bits, &status_pdu) == 0);

  return SRSRAN_SUCCESS;
}

// Status PDU for 12bit SN with ACK_SN=2065,
// NACK range0: 3 full SDUs, NACK_SN=273..275
// NACK range1: missing segment sequence across 4 SDUs
//              starting at NACK_SN=276, SO_START=2,
//              ending at NACK_SN=279, SO_END=5
// E1 and E3 bit set on first NACK, E2 and E3 bit set on the second.
int rlc_am_nr_control_pdu_12bit_sn_test_nack_range()
{
  test_delimit_logger      delimiter("Control PDU (12bit SN) test NACK range");
  const int                len = 13;
  std::array<uint8_t, len> tv  = {0x08,  // D/C | 3CPT | 4ACK_SN_upper
                                 0x11,  // 8ACK_SN_lower
                                 0x80,  // E1 | 7R
                                 0x11,  // 8NACK_SN_upper
                                 0x1a,  // 4NACK_SN_lower | E1 | E2 | E3 | R
                                 0x03,  // 8NACK_range
                                 0x11,  // 8NACK_SN_upper
                                 0x46,  // 4NACK_SN_lower | E1 | E2 | E3 | R
                                 0x00,  // 8SO_START_upper
                                 0x02,  // 8SO_START_lower
                                 0x00,  // 8SO_END_upper
                                 0x05,  // 8SO_END_lower
                                 0x04}; // 8NACK_range
  srsran::byte_buffer_t    pdu = make_pdu_and_log(tv);

  TESTASSERT(rlc_am_is_control_pdu(pdu.msg) == true);

  // unpack PDU
  rlc_am_nr_status_pdu_t status_pdu(srsran::rlc_am_nr_sn_size_t::size12bits);
  TESTASSERT(rlc_am_nr_read_status_pdu(&pdu, srsran::rlc_am_nr_sn_size_t::size12bits, &status_pdu) == SRSRAN_SUCCESS);
  TESTASSERT(status_pdu.ack_sn == 2065);
  TESTASSERT(status_pdu.nacks.size() == 2);
  TESTASSERT(status_pdu.nacks[0].nack_sn == 273);
  TESTASSERT(status_pdu.nacks[0].has_so == false);
  TESTASSERT(status_pdu.nacks[0].has_nack_range == true);
  TESTASSERT(status_pdu.nacks[0].nack_range == 3);

  TESTASSERT(status_pdu.nacks[1].nack_sn == 276);
  TESTASSERT(status_pdu.nacks[1].has_so == true);
  TESTASSERT(status_pdu.nacks[1].so_start == 2);
  TESTASSERT(status_pdu.nacks[1].so_end == 5);
  TESTASSERT(status_pdu.nacks[1].has_nack_range == true);
  TESTASSERT(status_pdu.nacks[1].nack_range == 4);

  // reset status PDU
  pdu.clear();

  // pack again
  TESTASSERT(rlc_am_nr_write_status_pdu(status_pdu, srsran::rlc_am_nr_sn_size_t::size12bits, &pdu) == SRSRAN_SUCCESS);
  TESTASSERT(pdu.N_bytes == tv.size());

  write_pdu_to_pcap(4, pdu.msg, pdu.N_bytes);
  TESTASSERT(memcmp(pdu.msg, tv.data(), pdu.N_bytes) == 0);

  return SRSRAN_SUCCESS;
}

// Test merge of NACKs upon status PDU creation -- previous NACK: non-range; next NACK: non-range
int rlc_am_nr_control_pdu_test_nack_merge_sdu_sdu(rlc_am_nr_sn_size_t sn_size)
{
  test_delimit_logger delimiter("Control PDU ({} bit SN) test NACK merge: SDU + SDU", to_number(sn_size));

  const uint16_t so_end_of_sdu = rlc_status_nack_t::so_end_of_sdu;
  const uint32_t mod_nr        = cardinality(sn_size);
  const uint32_t min_size      = 3;
  const uint32_t nack_size     = sn_size == rlc_am_nr_sn_size_t::size12bits ? 2 : 3;
  const uint32_t so_size       = 4;
  const uint32_t range_size    = 1;

  // Case: [...][NACK SDU] + [NACK SDU] (continuous: merge with previous element)
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK SDU]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size, status_pdu.packed_size);

    // Add next NACK: [NACK SDU]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 1001;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + range_size, status_pdu.packed_size);
    TESTASSERT_EQ(prev_nack.nack_sn, status_pdu.nacks.back().nack_sn);
    TESTASSERT_EQ(false, status_pdu.nacks.back().has_so);
    TESTASSERT_EQ(0, status_pdu.nacks.back().so_start);
    TESTASSERT_EQ(0, status_pdu.nacks.back().so_end);
    TESTASSERT_EQ(true, status_pdu.nacks.back().has_nack_range);
    TESTASSERT_EQ(2, status_pdu.nacks.back().nack_range);
  }

  // Case: [...][NACK SDU] + [NACK SDU] (non-continuous, SN gap: append as is)
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK SDU]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size, status_pdu.packed_size);

    // Add next NACK: [NACK SDU]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 1002;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + 2 * nack_size, status_pdu.packed_size);
    TESTASSERT(prev_nack == status_pdu.nacks.front());
    TESTASSERT(next_nack == status_pdu.nacks.back());
  }

  // Case: [...][NACK SDU] + [NACK SDU] (continuous: merge with previous element) -- with SN overflow
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK SDU]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = mod_nr - 1;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size, status_pdu.packed_size);

    // Add next NACK: [NACK SDU]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 0;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + range_size, status_pdu.packed_size);
    TESTASSERT_EQ(prev_nack.nack_sn, status_pdu.nacks.back().nack_sn);
    TESTASSERT_EQ(false, status_pdu.nacks.back().has_so);
    TESTASSERT_EQ(0, status_pdu.nacks.back().so_start);
    TESTASSERT_EQ(0, status_pdu.nacks.back().so_end);
    TESTASSERT_EQ(true, status_pdu.nacks.back().has_nack_range);
    TESTASSERT_EQ(2, status_pdu.nacks.back().nack_range);
  }

  // Case: [...][NACK SDU] + [NACK SDU] (non-continuous, SN gap: append as is) -- with SN overflow
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK SDU]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = mod_nr - 1;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size, status_pdu.packed_size);

    // Add next NACK: [NACK SDU]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 1;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + 2 * nack_size, status_pdu.packed_size);
    TESTASSERT(prev_nack == status_pdu.nacks.front());
    TESTASSERT(next_nack == status_pdu.nacks.back());
  }

  // Case: [...][NACK SDU] + [NACK segm] (continuous: merge with previous element)
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK SDU]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size, status_pdu.packed_size);

    // Add next NACK: [NACK segm]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 1001;
    next_nack.has_so         = true;
    next_nack.so_start       = 0;
    next_nack.so_end         = 50;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + so_size + range_size, status_pdu.packed_size);
    TESTASSERT_EQ(prev_nack.nack_sn, status_pdu.nacks.back().nack_sn);
    TESTASSERT_EQ(true, status_pdu.nacks.back().has_so);
    TESTASSERT_EQ(0, status_pdu.nacks.back().so_start);
    TESTASSERT_EQ(next_nack.so_end, status_pdu.nacks.back().so_end);
    TESTASSERT_EQ(true, status_pdu.nacks.back().has_nack_range);
    TESTASSERT_EQ(2, status_pdu.nacks.back().nack_range);
  }

  // Case: [...][NACK SDU] + [NACK segm] (non-continuous, SN gap: append as is)
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK SDU]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size, status_pdu.packed_size);

    // Add next NACK: [NACK segm]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 1002;
    next_nack.has_so         = true;
    next_nack.so_start       = 0;
    next_nack.so_end         = 50;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + 2 * nack_size + so_size, status_pdu.packed_size);
    TESTASSERT(status_pdu.nacks.front() == prev_nack);
    TESTASSERT(status_pdu.nacks.back() == next_nack);
  }

  // Case: [...][NACK SDU] + [NACK segm] (non-continuous, SO gap: append as is)
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK SDU]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size, status_pdu.packed_size);

    // Add next NACK: [NACK segm]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 1001;
    next_nack.has_so         = true;
    next_nack.so_start       = 1;
    next_nack.so_end         = 50;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + 2 * nack_size + so_size, status_pdu.packed_size);
    TESTASSERT(status_pdu.nacks.front() == prev_nack);
    TESTASSERT(status_pdu.nacks.back() == next_nack);
  }

  // Case: [...][NACK segm] + [NACK SDU] (continuous: merge with previous element)
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK segm]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = so_end_of_sdu;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + so_size, status_pdu.packed_size);

    // Add next NACK: [NACK SDU]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 1001;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + so_size + range_size, status_pdu.packed_size);
    TESTASSERT_EQ(prev_nack.nack_sn, status_pdu.nacks.back().nack_sn);
    TESTASSERT_EQ(true, status_pdu.nacks.back().has_so);
    TESTASSERT_EQ(prev_nack.so_start, status_pdu.nacks.back().so_start);
    TESTASSERT_EQ(so_end_of_sdu, status_pdu.nacks.back().so_end);
    TESTASSERT_EQ(true, status_pdu.nacks.back().has_nack_range);
    TESTASSERT_EQ(2, status_pdu.nacks.back().nack_range);
  }

  // Case: [...][NACK segm] + [NACK SDU] (non-continuous, SN gap: append as is)
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK segm]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = so_end_of_sdu;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());

    // Add next NACK: [NACK SDU]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 1002;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + 2 * nack_size + so_size, status_pdu.packed_size);
    TESTASSERT(status_pdu.nacks.front() == prev_nack);
    TESTASSERT(status_pdu.nacks.back() == next_nack);
  }

  // Case: [...][NACK segm] + [NACK SDU] (non-continuous, SO gap: append as is)
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK segm]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = 99;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + so_size, status_pdu.packed_size);

    // Add next NACK: [NACK SDU]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 1001;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + 2 * nack_size + so_size, status_pdu.packed_size);
    TESTASSERT(status_pdu.nacks.front() == prev_nack);
    TESTASSERT(status_pdu.nacks.back() == next_nack);
  }

  // Case: [...][NACK segm] + [NACK segm] (continuous: merge with previous element)
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK segm]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = so_end_of_sdu;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + so_size, status_pdu.packed_size);

    // Add next NACK: [NACK segm]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 1001;
    next_nack.has_so         = true;
    next_nack.so_start       = 0;
    next_nack.so_end         = 22;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + so_size + range_size, status_pdu.packed_size);
    TESTASSERT_EQ(prev_nack.nack_sn, status_pdu.nacks.back().nack_sn);
    TESTASSERT_EQ(true, status_pdu.nacks.back().has_so);
    TESTASSERT_EQ(prev_nack.so_start, status_pdu.nacks.back().so_start);
    TESTASSERT_EQ(next_nack.so_end, status_pdu.nacks.back().so_end);
    TESTASSERT_EQ(true, status_pdu.nacks.back().has_nack_range);
    TESTASSERT_EQ(2, status_pdu.nacks.back().nack_range);
  }

  // Case: [...][NACK segm] + [NACK segm] (non-continuous, SN gap: append as is)
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK segm]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = so_end_of_sdu;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + so_size, status_pdu.packed_size);

    // Add next NACK: [NACK segm]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 1002;
    next_nack.has_so         = true;
    next_nack.so_start       = 0;
    next_nack.so_end         = 22;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + 2 * nack_size + 2 * so_size, status_pdu.packed_size);
    TESTASSERT(status_pdu.nacks.front() == prev_nack);
    TESTASSERT(status_pdu.nacks.back() == next_nack);
  }

  // Case: [...][NACK segm] + [NACK segm] (non-continuous, SO gap (left): append as is)
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK segm]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = 99;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + so_size, status_pdu.packed_size);

    // Add next NACK: [NACK segm]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 1001;
    next_nack.has_so         = true;
    next_nack.so_start       = 0;
    next_nack.so_end         = 22;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + 2 * nack_size + 2 * so_size, status_pdu.packed_size);
    TESTASSERT(status_pdu.nacks.front() == prev_nack);
    TESTASSERT(status_pdu.nacks.back() == next_nack);
  }

  // Case: [...][NACK segm] + [NACK segm] (non-continuous, SO gap (right): append as is)
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK segm]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = so_end_of_sdu;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + so_size, status_pdu.packed_size);

    // Add next NACK: [NACK segm]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 1001;
    next_nack.has_so         = true;
    next_nack.so_start       = 5;
    next_nack.so_end         = 22;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + 2 * nack_size + 2 * so_size, status_pdu.packed_size);
    TESTASSERT(status_pdu.nacks.front() == prev_nack);
    TESTASSERT(status_pdu.nacks.back() == next_nack);
  }

  return SRSRAN_SUCCESS;
}

// Test merge of NACKs upon status PDU creation -- previous NACK: range; next NACK: non-range
int rlc_am_nr_control_pdu_test_nack_merge_range_sdu(rlc_am_nr_sn_size_t sn_size)
{
  test_delimit_logger delimiter("Control PDU ({} bit SN) test NACK merge: range + SDU", to_number(sn_size));

  const uint16_t so_end_of_sdu = rlc_status_nack_t::so_end_of_sdu;
  const uint32_t mod_nr        = cardinality(sn_size);
  const uint32_t min_size      = 3;
  const uint32_t nack_size     = sn_size == rlc_am_nr_sn_size_t::size12bits ? 2 : 3;
  const uint32_t so_size       = 4;
  const uint32_t range_size    = 1;

  // Case: [...][NACK range] + [NACK SDU] (continuous: merge with previous element)
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK range]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + range_size, status_pdu.packed_size);

    // Add next NACK: [NACK SDU]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 1005;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + range_size, status_pdu.packed_size);
    TESTASSERT_EQ(prev_nack.nack_sn, status_pdu.nacks.back().nack_sn);
    TESTASSERT_EQ(false, status_pdu.nacks.back().has_so);
    TESTASSERT_EQ(0, status_pdu.nacks.back().so_start);
    TESTASSERT_EQ(0, status_pdu.nacks.back().so_end);
    TESTASSERT_EQ(true, status_pdu.nacks.back().has_nack_range);
    TESTASSERT_EQ(prev_nack.nack_range + 1, status_pdu.nacks.back().nack_range);
  }

  // Case: [...][NACK range] + [NACK SDU] (non-continuous, SN gap: append as is)
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK range]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + range_size, status_pdu.packed_size);

    // Add next NACK: [NACK SDU]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 1006;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + 2 * nack_size + range_size, status_pdu.packed_size);
    TESTASSERT(prev_nack == status_pdu.nacks.front());
    TESTASSERT(next_nack == status_pdu.nacks.back());
  }

  // Case: [...][NACK range] + [NACK SDU] (continuous: merge with previous element) -- with SN overflow
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK range]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = mod_nr - 1;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + range_size, status_pdu.packed_size);

    // Add next NACK: [NACK SDU]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 4;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + range_size, status_pdu.packed_size);
    TESTASSERT_EQ(prev_nack.nack_sn, status_pdu.nacks.back().nack_sn);
    TESTASSERT_EQ(false, status_pdu.nacks.back().has_so);
    TESTASSERT_EQ(0, status_pdu.nacks.back().so_start);
    TESTASSERT_EQ(0, status_pdu.nacks.back().so_end);
    TESTASSERT_EQ(true, status_pdu.nacks.back().has_nack_range);
    TESTASSERT_EQ(prev_nack.nack_range + 1, status_pdu.nacks.back().nack_range);
  }

  // Case: [...][NACK range] + [NACK SDU] (non-continuous, SN gap: append as is) -- with SN overflow
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK range]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = mod_nr - 1;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + range_size, status_pdu.packed_size);

    // Add next NACK: [NACK SDU]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 5;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + 2 * nack_size + range_size, status_pdu.packed_size);
    TESTASSERT(prev_nack == status_pdu.nacks.front());
    TESTASSERT(next_nack == status_pdu.nacks.back());
  }

  // Case: [...][NACK range] + [NACK segm] (continuous: merge with previous element)
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK range]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + range_size, status_pdu.packed_size);

    // Add next NACK: [NACK segm]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 1005;
    next_nack.has_so         = true;
    next_nack.so_start       = 0;
    next_nack.so_end         = 50;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + so_size + range_size, status_pdu.packed_size);
    TESTASSERT_EQ(prev_nack.nack_sn, status_pdu.nacks.back().nack_sn);
    TESTASSERT_EQ(true, status_pdu.nacks.back().has_so);
    TESTASSERT_EQ(0, status_pdu.nacks.back().so_start);
    TESTASSERT_EQ(next_nack.so_end, status_pdu.nacks.back().so_end);
    TESTASSERT_EQ(true, status_pdu.nacks.back().has_nack_range);
    TESTASSERT_EQ(prev_nack.nack_range + 1, status_pdu.nacks.back().nack_range);
  }

  // Case: [...][NACK range] + [NACK segm] (non-continuous, SN gap: append as is)
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK range]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + range_size, status_pdu.packed_size);

    // Add next NACK: [NACK segm]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 1006;
    next_nack.has_so         = true;
    next_nack.so_start       = 0;
    next_nack.so_end         = 50;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + 2 * nack_size + range_size + so_size, status_pdu.packed_size);
    TESTASSERT(status_pdu.nacks.front() == prev_nack);
    TESTASSERT(status_pdu.nacks.back() == next_nack);
  }

  // Case: [...][NACK range] + [NACK segm] (non-continuous, SO gap: append as is)
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK range]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + range_size, status_pdu.packed_size);

    // Add next NACK: [NACK segm]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 1005;
    next_nack.has_so         = true;
    next_nack.so_start       = 1;
    next_nack.so_end         = 50;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + 2 * nack_size + range_size + so_size, status_pdu.packed_size);
    TESTASSERT(status_pdu.nacks.front() == prev_nack);
    TESTASSERT(status_pdu.nacks.back() == next_nack);
  }

  // Case: [...][NACK range+segm] + [NACK SDU] (continuous: merge with previous element)
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK range+segm]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = so_end_of_sdu;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + so_size + range_size, status_pdu.packed_size);

    // Add next NACK: [NACK SDU]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 1005;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + so_size + range_size, status_pdu.packed_size);
    TESTASSERT_EQ(prev_nack.nack_sn, status_pdu.nacks.back().nack_sn);
    TESTASSERT_EQ(true, status_pdu.nacks.back().has_so);
    TESTASSERT_EQ(prev_nack.so_start, status_pdu.nacks.back().so_start);
    TESTASSERT_EQ(so_end_of_sdu, status_pdu.nacks.back().so_end);
    TESTASSERT_EQ(true, status_pdu.nacks.back().has_nack_range);
    TESTASSERT_EQ(prev_nack.nack_range + 1, status_pdu.nacks.back().nack_range);
  }

  // Case: [...][NACK range+segm] + [NACK SDU] (non-continuous, SN gap: append as is)
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK range+segm]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = so_end_of_sdu;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());

    // Add next NACK: [NACK SDU]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 1006;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(min_size + 2 * nack_size + so_size + range_size, status_pdu.packed_size);
    TESTASSERT_EQ(2, status_pdu.nacks.size());
    TESTASSERT(status_pdu.nacks.front() == prev_nack);
    TESTASSERT(status_pdu.nacks.back() == next_nack);
  }

  // Case: [...][NACK range+segm] + [NACK SDU] (non-continuous, SO gap: append as is)
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK range+segm]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = 99;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + so_size + range_size, status_pdu.packed_size);

    // Add next NACK: [NACK SDU]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 1005;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + 2 * nack_size + so_size + range_size, status_pdu.packed_size);
    TESTASSERT(status_pdu.nacks.front() == prev_nack);
    TESTASSERT(status_pdu.nacks.back() == next_nack);
  }

  // Case: [...][NACK range+segm] + [NACK segm] (continuous: merge with previous element)
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK range+segm]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = so_end_of_sdu;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + so_size + range_size, status_pdu.packed_size);

    // Add next NACK: [NACK segm]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 1005;
    next_nack.has_so         = true;
    next_nack.so_start       = 0;
    next_nack.so_end         = 22;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + so_size + range_size, status_pdu.packed_size);
    TESTASSERT_EQ(prev_nack.nack_sn, status_pdu.nacks.back().nack_sn);
    TESTASSERT_EQ(true, status_pdu.nacks.back().has_so);
    TESTASSERT_EQ(prev_nack.so_start, status_pdu.nacks.back().so_start);
    TESTASSERT_EQ(next_nack.so_end, status_pdu.nacks.back().so_end);
    TESTASSERT_EQ(true, status_pdu.nacks.back().has_nack_range);
    TESTASSERT_EQ(prev_nack.nack_range + 1, status_pdu.nacks.back().nack_range);
  }

  // Case: [...][NACK range+segm] + [NACK segm] (non-continuous, SN gap: append as is)
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK range+segm]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = so_end_of_sdu;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + so_size + range_size, status_pdu.packed_size);

    // Add next NACK: [NACK segm]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 1006;
    next_nack.has_so         = true;
    next_nack.so_start       = 0;
    next_nack.so_end         = 22;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + 2 * nack_size + 2 * so_size + range_size, status_pdu.packed_size);
    TESTASSERT(status_pdu.nacks.front() == prev_nack);
    TESTASSERT(status_pdu.nacks.back() == next_nack);
  }

  // Case: [...][NACK range+segm] + [NACK segm] (non-continuous, SO gap (left): append as is)
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK range+segm]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = 99;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + so_size + range_size, status_pdu.packed_size);

    // Add next NACK: [NACK segm]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 1005;
    next_nack.has_so         = true;
    next_nack.so_start       = 0;
    next_nack.so_end         = 22;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + 2 * nack_size + 2 * so_size + range_size, status_pdu.packed_size);
    TESTASSERT(status_pdu.nacks.front() == prev_nack);
    TESTASSERT(status_pdu.nacks.back() == next_nack);
  }

  // Case: [...][NACK range+segm] + [NACK segm] (non-continuous, SO gap (right): append as is)
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK range+segm]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = so_end_of_sdu;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + so_size + range_size, status_pdu.packed_size);

    // Add next NACK: [NACK segm]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 1005;
    next_nack.has_so         = true;
    next_nack.so_start       = 5;
    next_nack.so_end         = 22;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + 2 * nack_size + 2 * so_size + range_size, status_pdu.packed_size);
    TESTASSERT(status_pdu.nacks.front() == prev_nack);
    TESTASSERT(status_pdu.nacks.back() == next_nack);
  }

  return SRSRAN_SUCCESS;
}

// Test merge of NACKs upon status PDU creation -- previous NACK: non-range; next NACK: range
int rlc_am_nr_control_pdu_test_nack_merge_sdu_range(rlc_am_nr_sn_size_t sn_size)
{
  test_delimit_logger delimiter("Control PDU ({} bit SN) test NACK merge: SDU + range", to_number(sn_size));

  const uint16_t so_end_of_sdu = rlc_status_nack_t::so_end_of_sdu;
  const uint32_t mod_nr        = cardinality(sn_size);
  const uint32_t min_size      = 3;
  const uint32_t nack_size     = sn_size == rlc_am_nr_sn_size_t::size12bits ? 2 : 3;
  const uint32_t so_size       = 4;
  const uint32_t range_size    = 1;

  // Case: [...][NACK SDU] + [NACK range] (continuous: merge with previous element)
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK SDU]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size, status_pdu.packed_size);

    // Add next NACK: [NACK range]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 1001;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + range_size, status_pdu.packed_size);
    TESTASSERT_EQ(prev_nack.nack_sn, status_pdu.nacks.back().nack_sn);
    TESTASSERT_EQ(false, status_pdu.nacks.back().has_so);
    TESTASSERT_EQ(0, status_pdu.nacks.back().so_start);
    TESTASSERT_EQ(0, status_pdu.nacks.back().so_end);
    TESTASSERT_EQ(true, status_pdu.nacks.back().has_nack_range);
    TESTASSERT_EQ(3, status_pdu.nacks.back().nack_range);
  }

  // Case: [...][NACK SDU] + [NACK range] (non-continuous, SN gap: append as is)
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK SDU]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size, status_pdu.packed_size);

    // Add next NACK: [NACK range]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 1002;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + 2 * nack_size + range_size, status_pdu.packed_size);
    TESTASSERT(prev_nack == status_pdu.nacks.front());
    TESTASSERT(next_nack == status_pdu.nacks.back());
  }

  // Case: [...][NACK SDU] + [NACK range] (continuous: merge with previous element) -- with SN overflow
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK SDU]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = mod_nr - 1;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size, status_pdu.packed_size);

    // Add next NACK: [NACK range]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 0;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + range_size, status_pdu.packed_size);
    TESTASSERT_EQ(prev_nack.nack_sn, status_pdu.nacks.back().nack_sn);
    TESTASSERT_EQ(false, status_pdu.nacks.back().has_so);
    TESTASSERT_EQ(0, status_pdu.nacks.back().so_start);
    TESTASSERT_EQ(0, status_pdu.nacks.back().so_end);
    TESTASSERT_EQ(true, status_pdu.nacks.back().has_nack_range);
    TESTASSERT_EQ(3, status_pdu.nacks.back().nack_range);
  }

  // Case: [...][NACK SDU] + [NACK range] (non-continuous, SN gap: append as is) -- with SN overflow
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK SDU]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = mod_nr - 1;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size, status_pdu.packed_size);

    // Add next NACK: [NACK range]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 1;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + 2 * nack_size + range_size, status_pdu.packed_size);
    TESTASSERT(prev_nack == status_pdu.nacks.front());
    TESTASSERT(next_nack == status_pdu.nacks.back());
  }

  // Case: [...][NACK SDU] + [NACK range+segm] (continuous: merge with previous element)
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK SDU]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size, status_pdu.packed_size);

    // Add next NACK: [NACK range+segm]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 1001;
    next_nack.has_so         = true;
    next_nack.so_start       = 0;
    next_nack.so_end         = 50;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + so_size + range_size, status_pdu.packed_size);
    TESTASSERT_EQ(prev_nack.nack_sn, status_pdu.nacks.back().nack_sn);
    TESTASSERT_EQ(true, status_pdu.nacks.back().has_so);
    TESTASSERT_EQ(0, status_pdu.nacks.back().so_start);
    TESTASSERT_EQ(next_nack.so_end, status_pdu.nacks.back().so_end);
    TESTASSERT_EQ(true, status_pdu.nacks.back().has_nack_range);
    TESTASSERT_EQ(3, status_pdu.nacks.back().nack_range);
  }

  // Case: [...][NACK SDU] + [NACK range+segm] (non-continuous, SN gap: append as is)
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK SDU]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size, status_pdu.packed_size);

    // Add next NACK: [NACK range+segm]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 1002;
    next_nack.has_so         = true;
    next_nack.so_start       = 0;
    next_nack.so_end         = 50;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + 2 * nack_size + so_size + range_size, status_pdu.packed_size);
    TESTASSERT(status_pdu.nacks.front() == prev_nack);
    TESTASSERT(status_pdu.nacks.back() == next_nack);
  }

  // Case: [...][NACK SDU] + [NACK range+segm] (non-continuous, SO gap: append as is)
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK SDU]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size, status_pdu.packed_size);

    // Add next NACK: [NACK range+segm]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 1001;
    next_nack.has_so         = true;
    next_nack.so_start       = 1;
    next_nack.so_end         = 50;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + 2 * nack_size + so_size + range_size, status_pdu.packed_size);
    TESTASSERT(status_pdu.nacks.front() == prev_nack);
    TESTASSERT(status_pdu.nacks.back() == next_nack);
  }

  // Case: [...][NACK segm] + [NACK range] (continuous: merge with previous element)
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK segm]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = so_end_of_sdu;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + so_size, status_pdu.packed_size);

    // Add next NACK: [NACK range]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 1001;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + so_size + range_size, status_pdu.packed_size);
    TESTASSERT_EQ(prev_nack.nack_sn, status_pdu.nacks.back().nack_sn);
    TESTASSERT_EQ(true, status_pdu.nacks.back().has_so);
    TESTASSERT_EQ(prev_nack.so_start, status_pdu.nacks.back().so_start);
    TESTASSERT_EQ(so_end_of_sdu, status_pdu.nacks.back().so_end);
    TESTASSERT_EQ(true, status_pdu.nacks.back().has_nack_range);
    TESTASSERT_EQ(3, status_pdu.nacks.back().nack_range);
  }

  // Case: [...][NACK segm] + [NACK range] (non-continuous, SN gap: append as is)
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK segm]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = so_end_of_sdu;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());

    // Add next NACK: [NACK range]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 1002;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + 2 * nack_size + so_size + range_size, status_pdu.packed_size);
    TESTASSERT(status_pdu.nacks.front() == prev_nack);
    TESTASSERT(status_pdu.nacks.back() == next_nack);
  }

  // Case: [...][NACK segm] + [NACK range] (non-continuous, SO gap: append as is)
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK segm]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = 99;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + so_size, status_pdu.packed_size);

    // Add next NACK: [NACK range]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 1001;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + 2 * nack_size + so_size + range_size, status_pdu.packed_size);
    TESTASSERT(status_pdu.nacks.front() == prev_nack);
    TESTASSERT(status_pdu.nacks.back() == next_nack);
  }

  // Case: [...][NACK segm] + [NACK range+segm] (continuous: merge with previous element)
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK segm]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = so_end_of_sdu;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + so_size, status_pdu.packed_size);

    // Add next NACK: [NACK range+segm]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 1001;
    next_nack.has_so         = true;
    next_nack.so_start       = 0;
    next_nack.so_end         = 22;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + so_size + range_size, status_pdu.packed_size);
    TESTASSERT_EQ(prev_nack.nack_sn, status_pdu.nacks.back().nack_sn);
    TESTASSERT_EQ(true, status_pdu.nacks.back().has_so);
    TESTASSERT_EQ(prev_nack.so_start, status_pdu.nacks.back().so_start);
    TESTASSERT_EQ(next_nack.so_end, status_pdu.nacks.back().so_end);
    TESTASSERT_EQ(true, status_pdu.nacks.back().has_nack_range);
    TESTASSERT_EQ(3, status_pdu.nacks.back().nack_range);
  }

  // Case: [...][NACK segm] + [NACK range+segm] (non-continuous, SN gap: append as is)
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK segm]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = so_end_of_sdu;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + so_size, status_pdu.packed_size);

    // Add next NACK: [NACK range+segm]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 1002;
    next_nack.has_so         = true;
    next_nack.so_start       = 0;
    next_nack.so_end         = 22;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + 2 * nack_size + 2 * so_size + range_size, status_pdu.packed_size);
    TESTASSERT(status_pdu.nacks.front() == prev_nack);
    TESTASSERT(status_pdu.nacks.back() == next_nack);
  }

  // Case: [...][NACK segm] + [NACK range+segm] (non-continuous, SO gap (left): append as is)
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK segm]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = 99;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + so_size, status_pdu.packed_size);

    // Add next NACK: [NACK range+segm]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 1001;
    next_nack.has_so         = true;
    next_nack.so_start       = 0;
    next_nack.so_end         = 22;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + 2 * nack_size + 2 * so_size + range_size, status_pdu.packed_size);
    TESTASSERT(status_pdu.nacks.front() == prev_nack);
    TESTASSERT(status_pdu.nacks.back() == next_nack);
  }

  // Case: [...][NACK segm] + [NACK range+segm] (non-continuous, SO gap (right): append as is)
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK segm]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = so_end_of_sdu;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + so_size, status_pdu.packed_size);

    // Add next NACK: [NACK range+segm]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 1001;
    next_nack.has_so         = true;
    next_nack.so_start       = 5;
    next_nack.so_end         = 22;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + 2 * nack_size + 2 * so_size + range_size, status_pdu.packed_size);
    TESTASSERT(status_pdu.nacks.front() == prev_nack);
    TESTASSERT(status_pdu.nacks.back() == next_nack);
  }

  return SRSRAN_SUCCESS;
}

// Test merge of NACKs upon status PDU creation -- previous NACK: range; next NACK: range
int rlc_am_nr_control_pdu_test_nack_merge_range_range(rlc_am_nr_sn_size_t sn_size)
{
  test_delimit_logger delimiter("Control PDU ({} bit SN) test NACK merge: range + SDU", to_number(sn_size));

  const uint16_t so_end_of_sdu = rlc_status_nack_t::so_end_of_sdu;
  const uint32_t mod_nr        = cardinality(sn_size);
  const uint32_t min_size      = 3;
  const uint32_t nack_size     = sn_size == rlc_am_nr_sn_size_t::size12bits ? 2 : 3;
  const uint32_t so_size       = 4;
  const uint32_t range_size    = 1;

  // Case: [...][NACK range] + [NACK range] (continuous: merge with previous element)
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK range]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + range_size, status_pdu.packed_size);

    // Add next NACK: [NACK range]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 1005;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + range_size, status_pdu.packed_size);
    TESTASSERT_EQ(prev_nack.nack_sn, status_pdu.nacks.back().nack_sn);
    TESTASSERT_EQ(false, status_pdu.nacks.back().has_so);
    TESTASSERT_EQ(0, status_pdu.nacks.back().so_start);
    TESTASSERT_EQ(0, status_pdu.nacks.back().so_end);
    TESTASSERT_EQ(true, status_pdu.nacks.back().has_nack_range);
    TESTASSERT_EQ(prev_nack.nack_range + next_nack.nack_range, status_pdu.nacks.back().nack_range);
  }

  // Case: [...][NACK range] + [NACK range] (non-continuous, SN gap: append as is)
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK range]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + range_size, status_pdu.packed_size);

    // Add next NACK: [NACK range]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 1006;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + 2 * nack_size + 2 * range_size, status_pdu.packed_size);
    TESTASSERT(prev_nack == status_pdu.nacks.front());
    TESTASSERT(next_nack == status_pdu.nacks.back());
  }

  // Case: [...][NACK range] + [NACK range] (continuous: merge with previous element) -- with SN overflow
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK range]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = mod_nr - 1;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + range_size, status_pdu.packed_size);

    // Add next NACK: [NACK range]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 4;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + range_size, status_pdu.packed_size);
    TESTASSERT_EQ(prev_nack.nack_sn, status_pdu.nacks.back().nack_sn);
    TESTASSERT_EQ(false, status_pdu.nacks.back().has_so);
    TESTASSERT_EQ(0, status_pdu.nacks.back().so_start);
    TESTASSERT_EQ(0, status_pdu.nacks.back().so_end);
    TESTASSERT_EQ(true, status_pdu.nacks.back().has_nack_range);
    TESTASSERT_EQ(prev_nack.nack_range + next_nack.nack_range, status_pdu.nacks.back().nack_range);
  }

  // Case: [...][NACK range] + [NACK range] (non-continuous, SN gap: append as is) -- with SN overflow
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK range]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = mod_nr - 1;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + range_size, status_pdu.packed_size);

    // Add next NACK: [NACK range]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 5;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + 2 * nack_size + 2 * range_size, status_pdu.packed_size);
    TESTASSERT(prev_nack == status_pdu.nacks.front());
    TESTASSERT(next_nack == status_pdu.nacks.back());
  }

  // Case: [...][NACK range] + [NACK range+segm] (continuous: merge with previous element)
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK range]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + range_size, status_pdu.packed_size);

    // Add next NACK: [NACK range+segm]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 1005;
    next_nack.has_so         = true;
    next_nack.so_start       = 0;
    next_nack.so_end         = 50;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + so_size + range_size, status_pdu.packed_size);
    TESTASSERT_EQ(prev_nack.nack_sn, status_pdu.nacks.back().nack_sn);
    TESTASSERT_EQ(true, status_pdu.nacks.back().has_so);
    TESTASSERT_EQ(0, status_pdu.nacks.back().so_start);
    TESTASSERT_EQ(next_nack.so_end, status_pdu.nacks.back().so_end);
    TESTASSERT_EQ(true, status_pdu.nacks.back().has_nack_range);
    TESTASSERT_EQ(prev_nack.nack_range + next_nack.nack_range, status_pdu.nacks.back().nack_range);
  }

  // Case: [...][NACK range] + [NACK range+segm] (non-continuous, SN gap: append as is)
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK range]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + range_size, status_pdu.packed_size);

    // Add next NACK: [NACK range+segm]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 1006;
    next_nack.has_so         = true;
    next_nack.so_start       = 0;
    next_nack.so_end         = 50;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + 2 * nack_size + 2 * range_size + so_size, status_pdu.packed_size);
    TESTASSERT(status_pdu.nacks.front() == prev_nack);
    TESTASSERT(status_pdu.nacks.back() == next_nack);
  }

  // Case: [...][NACK range] + [NACK range+segm] (non-continuous, SO gap: append as is)
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK range]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + range_size, status_pdu.packed_size);

    // Add next NACK: [NACK range+segm]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 1005;
    next_nack.has_so         = true;
    next_nack.so_start       = 1;
    next_nack.so_end         = 50;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + 2 * nack_size + 2 * range_size + so_size, status_pdu.packed_size);
    TESTASSERT(status_pdu.nacks.front() == prev_nack);
    TESTASSERT(status_pdu.nacks.back() == next_nack);
  }

  // Case: [...][NACK range+segm] + [NACK range] (continuous: merge with previous element)
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK range+segm]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = so_end_of_sdu;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + so_size + range_size, status_pdu.packed_size);

    // Add next NACK: [NACK range]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 1005;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + so_size + range_size, status_pdu.packed_size);
    TESTASSERT_EQ(prev_nack.nack_sn, status_pdu.nacks.back().nack_sn);
    TESTASSERT_EQ(true, status_pdu.nacks.back().has_so);
    TESTASSERT_EQ(prev_nack.so_start, status_pdu.nacks.back().so_start);
    TESTASSERT_EQ(so_end_of_sdu, status_pdu.nacks.back().so_end);
    TESTASSERT_EQ(true, status_pdu.nacks.back().has_nack_range);
    TESTASSERT_EQ(prev_nack.nack_range + next_nack.nack_range, status_pdu.nacks.back().nack_range);
  }

  // Case: [...][NACK range+segm] + [NACK range] (non-continuous, SN gap: append as is)
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK range+segm]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = so_end_of_sdu;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());

    // Add next NACK: [NACK range]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 1006;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(min_size + 2 * nack_size + so_size + 2 * range_size, status_pdu.packed_size);
    TESTASSERT_EQ(2, status_pdu.nacks.size());
    TESTASSERT(status_pdu.nacks.front() == prev_nack);
    TESTASSERT(status_pdu.nacks.back() == next_nack);
  }

  // Case: [...][NACK range+segm] + [NACK range] (non-continuous, SO gap: append as is)
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK range+segm]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = 99;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + so_size + range_size, status_pdu.packed_size);

    // Add next NACK: [NACK range]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 1005;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + 2 * nack_size + so_size + 2 * range_size, status_pdu.packed_size);
    TESTASSERT(status_pdu.nacks.front() == prev_nack);
    TESTASSERT(status_pdu.nacks.back() == next_nack);
  }

  // Case: [...][NACK range+segm] + [NACK range+segm] (continuous: merge with previous element)
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK range+segm]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = so_end_of_sdu;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + so_size + range_size, status_pdu.packed_size);

    // Add next NACK: [NACK range+segm]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 1005;
    next_nack.has_so         = true;
    next_nack.so_start       = 0;
    next_nack.so_end         = 22;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + so_size + range_size, status_pdu.packed_size);
    TESTASSERT_EQ(prev_nack.nack_sn, status_pdu.nacks.back().nack_sn);
    TESTASSERT_EQ(true, status_pdu.nacks.back().has_so);
    TESTASSERT_EQ(prev_nack.so_start, status_pdu.nacks.back().so_start);
    TESTASSERT_EQ(next_nack.so_end, status_pdu.nacks.back().so_end);
    TESTASSERT_EQ(true, status_pdu.nacks.back().has_nack_range);
    TESTASSERT_EQ(prev_nack.nack_range + next_nack.nack_range, status_pdu.nacks.back().nack_range);
  }

  // Case: [...][NACK range+segm] + [NACK range+segm] (non-continuous, SN gap: append as is)
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK range+segm]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = so_end_of_sdu;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + so_size + range_size, status_pdu.packed_size);

    // Add next NACK: [NACK range+segm]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 1006;
    next_nack.has_so         = true;
    next_nack.so_start       = 0;
    next_nack.so_end         = 22;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + 2 * nack_size + 2 * so_size + 2 * range_size, status_pdu.packed_size);
    TESTASSERT(status_pdu.nacks.front() == prev_nack);
    TESTASSERT(status_pdu.nacks.back() == next_nack);
  }

  // Case: [...][NACK range+segm] + [NACK range+segm] (non-continuous, SO gap (left): append as is)
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK range+segm]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = 99;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + so_size + range_size, status_pdu.packed_size);

    // Add next NACK: [NACK range+segm]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 1005;
    next_nack.has_so         = true;
    next_nack.so_start       = 0;
    next_nack.so_end         = 22;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + 2 * nack_size + 2 * so_size + 2 * range_size, status_pdu.packed_size);
    TESTASSERT(status_pdu.nacks.front() == prev_nack);
    TESTASSERT(status_pdu.nacks.back() == next_nack);
  }

  // Case: [...][NACK range+segm] + [NACK range+segm] (non-continuous, SO gap (right): append as is)
  {
    rlc_am_nr_status_pdu_t status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.nacks.size());

    // Prepare status_pdu.nacks: [...][NACK range+segm]
    rlc_status_nack_t prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = so_end_of_sdu;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + nack_size + so_size + range_size, status_pdu.packed_size);

    // Add next NACK: [NACK range+segm]
    rlc_status_nack_t next_nack;
    next_nack.nack_sn        = 1005;
    next_nack.has_so         = true;
    next_nack.so_start       = 5;
    next_nack.so_end         = 22;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.nacks.size());
    TESTASSERT_EQ(min_size + 2 * nack_size + 2 * so_size + 2 * range_size, status_pdu.packed_size);
    TESTASSERT(status_pdu.nacks.front() == prev_nack);
    TESTASSERT(status_pdu.nacks.back() == next_nack);
  }

  return SRSRAN_SUCCESS;
}

// Test status PDU for correct trimming and estimation of packed size
// 1) Test init, copy and reset
// 2) Test step-wise growth and trimming of status PDU while covering several corner cases
int rlc_am_nr_control_pdu_test_trimming(rlc_am_nr_sn_size_t sn_size)
{
  test_delimit_logger delimiter("Control PDU ({} bit SN) test trimming", to_number(sn_size));

  // status PDU with no NACKs
  {
    constexpr uint32_t     min_size = 3;
    srsran::byte_buffer_t  pdu;
    rlc_am_nr_status_pdu_t status_pdu(sn_size);

    status_pdu.ack_sn = 99;
    TESTASSERT_EQ(status_pdu.packed_size, min_size); // minimum size
    TESTASSERT_EQ(rlc_am_nr_write_status_pdu(status_pdu, sn_size, &pdu), SRSRAN_SUCCESS);
    TESTASSERT_EQ(pdu.N_bytes, min_size);

    rlc_am_nr_status_pdu_t status_pdu_copy = status_pdu;
    TESTASSERT_EQ(status_pdu_copy.ack_sn, 99);
    TESTASSERT_EQ(status_pdu_copy.packed_size, min_size); // minimum size
    TESTASSERT_EQ(rlc_am_nr_write_status_pdu(status_pdu_copy, sn_size, &pdu), SRSRAN_SUCCESS);
    TESTASSERT_EQ(pdu.N_bytes, min_size);

    status_pdu.reset();

    status_pdu.ack_sn = 77;
    TESTASSERT_EQ(status_pdu.packed_size, min_size); // should still have minimum size
    TESTASSERT_EQ(rlc_am_nr_write_status_pdu(status_pdu, sn_size, &pdu), SRSRAN_SUCCESS);
    TESTASSERT_EQ(pdu.N_bytes, min_size);

    TESTASSERT_EQ(status_pdu_copy.ack_sn, 99);            // shouldn't have changed
    TESTASSERT_EQ(status_pdu_copy.packed_size, min_size); // minimum size
    TESTASSERT_EQ(rlc_am_nr_write_status_pdu(status_pdu_copy, sn_size, &pdu), SRSRAN_SUCCESS);
    TESTASSERT_EQ(pdu.N_bytes, min_size);
  }

  // status PDU with multiple NACKs
  // expect: ACK=77, NACKs=[12][14][17 50:99][17 150:199][17 250:299][19][21 333:111 r5][27 444:666 r3]
  {
    constexpr uint32_t     min_size      = 3;
    const uint32_t         nack_size     = sn_size == rlc_am_nr_sn_size_t::size12bits ? 2 : 3;
    constexpr uint32_t     so_size       = 4;
    constexpr uint32_t     range_size    = 1;
    uint32_t               expected_size = min_size;
    srsran::byte_buffer_t  pdu;
    rlc_am_nr_status_pdu_t status_pdu(sn_size);

    status_pdu.ack_sn = 77;
    {
      rlc_status_nack_t nack;
      nack.nack_sn = 12;
      status_pdu.push_nack(nack);
    }
    expected_size += nack_size;
    TESTASSERT_EQ(status_pdu.packed_size, expected_size);
    TESTASSERT_EQ(rlc_am_nr_write_status_pdu(status_pdu, sn_size, &pdu), SRSRAN_SUCCESS);
    TESTASSERT_EQ(pdu.N_bytes, expected_size);
    {
      rlc_status_nack_t nack;
      nack.nack_sn = 14;
      status_pdu.push_nack(nack);
    }
    expected_size += nack_size;
    TESTASSERT_EQ(status_pdu.packed_size, expected_size);
    TESTASSERT_EQ(rlc_am_nr_write_status_pdu(status_pdu, sn_size, &pdu), SRSRAN_SUCCESS);
    TESTASSERT_EQ(pdu.N_bytes, expected_size);
    {
      rlc_status_nack_t nack;
      nack.nack_sn  = 17;
      nack.has_so   = true;
      nack.so_start = 50;
      nack.so_end   = 99;
      status_pdu.push_nack(nack);
    }
    expected_size += nack_size + so_size;
    TESTASSERT_EQ(status_pdu.packed_size, expected_size);
    TESTASSERT_EQ(rlc_am_nr_write_status_pdu(status_pdu, sn_size, &pdu), SRSRAN_SUCCESS);
    TESTASSERT_EQ(pdu.N_bytes, expected_size);
    {
      rlc_status_nack_t nack;
      nack.nack_sn  = 17;
      nack.has_so   = true;
      nack.so_start = 150;
      nack.so_end   = 199;
      status_pdu.push_nack(nack);
    }
    expected_size += nack_size + so_size;
    TESTASSERT_EQ(status_pdu.packed_size, expected_size);
    TESTASSERT_EQ(rlc_am_nr_write_status_pdu(status_pdu, sn_size, &pdu), SRSRAN_SUCCESS);
    TESTASSERT_EQ(pdu.N_bytes, expected_size);
    {
      rlc_status_nack_t nack;
      nack.nack_sn  = 17;
      nack.has_so   = true;
      nack.so_start = 250;
      nack.so_end   = 299;
      status_pdu.push_nack(nack);
    }
    expected_size += nack_size + so_size;
    TESTASSERT_EQ(status_pdu.packed_size, expected_size);
    TESTASSERT_EQ(rlc_am_nr_write_status_pdu(status_pdu, sn_size, &pdu), SRSRAN_SUCCESS);
    TESTASSERT_EQ(pdu.N_bytes, expected_size);
    {
      rlc_status_nack_t nack;
      nack.nack_sn = 19;
      status_pdu.push_nack(nack);
    }
    expected_size += nack_size;
    TESTASSERT_EQ(status_pdu.packed_size, expected_size);
    TESTASSERT_EQ(rlc_am_nr_write_status_pdu(status_pdu, sn_size, &pdu), SRSRAN_SUCCESS);
    TESTASSERT_EQ(pdu.N_bytes, expected_size);
    {
      rlc_status_nack_t nack;
      nack.nack_sn        = 21;
      nack.has_so         = true;
      nack.so_start       = 333;
      nack.so_end         = 111;
      nack.has_nack_range = true;
      nack.nack_range     = 5;
      status_pdu.push_nack(nack);
    }
    expected_size += nack_size + so_size + range_size;
    TESTASSERT_EQ(status_pdu.packed_size, expected_size);
    TESTASSERT_EQ(rlc_am_nr_write_status_pdu(status_pdu, sn_size, &pdu), SRSRAN_SUCCESS);
    TESTASSERT_EQ(pdu.N_bytes, expected_size);
    {
      rlc_status_nack_t nack;
      nack.nack_sn        = 27;
      nack.has_so         = true;
      nack.so_start       = 444;
      nack.so_end         = 666;
      nack.has_nack_range = true;
      nack.nack_range     = 3;
      status_pdu.push_nack(nack);
    }
    expected_size += nack_size + so_size + range_size;
    TESTASSERT_EQ(status_pdu.ack_sn, 77);
    TESTASSERT_EQ(status_pdu.packed_size, expected_size);
    TESTASSERT_EQ(rlc_am_nr_write_status_pdu(status_pdu, sn_size, &pdu), SRSRAN_SUCCESS);
    TESTASSERT_EQ(pdu.N_bytes, expected_size);

    // current state: ACK=77, NACKs=[12][14][17 50:99][17 150:199][17 250:299][19][21 333:111 r5][27 444:666 r3]

    // create a copy, check content
    rlc_am_nr_status_pdu_t status_pdu_copy = status_pdu;
    TESTASSERT_EQ(status_pdu_copy.ack_sn, 77);
    TESTASSERT_EQ(status_pdu_copy.packed_size, expected_size);
    TESTASSERT_EQ(rlc_am_nr_write_status_pdu(status_pdu_copy, sn_size, &pdu), SRSRAN_SUCCESS);
    TESTASSERT_EQ(pdu.N_bytes, expected_size);

    // current state: ACK=77, NACKs=[12][14][17 50:99][17 150:199][17 250:299][19][21 333:111 r5][27 444:666 r3]

    // trim to much larger size: nothing should change
    TESTASSERT_EQ(status_pdu.trim(expected_size * 2), true);
    TESTASSERT_EQ(status_pdu.ack_sn, 77);
    TESTASSERT_EQ(status_pdu.packed_size, expected_size);
    TESTASSERT_EQ(rlc_am_nr_write_status_pdu(status_pdu, sn_size, &pdu), SRSRAN_SUCCESS);
    TESTASSERT_EQ(pdu.N_bytes, expected_size);

    // trim to exact size: nothing should change
    TESTASSERT_EQ(status_pdu.trim(expected_size), true);
    TESTASSERT_EQ(status_pdu.ack_sn, 77);
    TESTASSERT_EQ(status_pdu.packed_size, expected_size);
    TESTASSERT_EQ(rlc_am_nr_write_status_pdu(status_pdu, sn_size, &pdu), SRSRAN_SUCCESS);
    TESTASSERT_EQ(pdu.N_bytes, expected_size);

    // trim to (expected_size - 1): this should remove the last NACK and update ACK accordingly
    TESTASSERT_EQ(status_pdu.trim(expected_size - 1), true);
    expected_size -= nack_size + so_size + range_size;
    TESTASSERT_EQ(status_pdu.ack_sn, 27);
    TESTASSERT_EQ(status_pdu.packed_size, expected_size);
    TESTASSERT_EQ(rlc_am_nr_write_status_pdu(status_pdu, sn_size, &pdu), SRSRAN_SUCCESS);
    TESTASSERT_EQ(pdu.N_bytes, expected_size);

    // current state: ACK=27, NACKs=[12][14][17 50:99][17 150:199][17 250:299][19][21 333:111 r5]

    // trim to (expected_size - last two NACKs): this should remove the last NACK and update ACK accordingly
    TESTASSERT_EQ(status_pdu.trim(expected_size - (2 * nack_size + so_size + range_size)), true);
    expected_size -= 2 * nack_size + so_size + range_size;
    TESTASSERT_EQ(status_pdu.ack_sn, 19);
    TESTASSERT_EQ(status_pdu.packed_size, expected_size);
    TESTASSERT_EQ(rlc_am_nr_write_status_pdu(status_pdu, sn_size, &pdu), SRSRAN_SUCCESS);
    TESTASSERT_EQ(pdu.N_bytes, expected_size);

    // current state: ACK=19, NACKs=[12][14][17 50:99][17 150:199][17 250:299]

    // trim to (expected_size - 1): this should remove the last NACK and all other NACKs with the same SN
    TESTASSERT_EQ(status_pdu.trim(expected_size - 1), true);
    expected_size -= 3 * (nack_size + so_size);
    TESTASSERT_EQ(status_pdu.ack_sn, 17);
    TESTASSERT_EQ(status_pdu.packed_size, expected_size);
    TESTASSERT_EQ(rlc_am_nr_write_status_pdu(status_pdu, sn_size, &pdu), SRSRAN_SUCCESS);
    TESTASSERT_EQ(pdu.N_bytes, expected_size);

    // current state: ACK=17, NACKs=[12][14]

    // trim to impossible size = 1: this should report a failure without changes of the PDU
    TESTASSERT_EQ(status_pdu.trim(1), false);
    TESTASSERT_EQ(status_pdu.ack_sn, 17);
    TESTASSERT_EQ(status_pdu.packed_size, expected_size);
    TESTASSERT_EQ(rlc_am_nr_write_status_pdu(status_pdu, sn_size, &pdu), SRSRAN_SUCCESS);
    TESTASSERT_EQ(pdu.N_bytes, expected_size);

    // current state: ACK=17, NACKs=[12][14]

    // trim to minimum size: this should remove all NACKs and update ACK to the SN of the first NACK
    expected_size = min_size;
    TESTASSERT_EQ(status_pdu.trim(expected_size), true);
    TESTASSERT_EQ(status_pdu.ack_sn, 12);
    TESTASSERT_EQ(status_pdu.packed_size, expected_size);
    TESTASSERT_EQ(rlc_am_nr_write_status_pdu(status_pdu, sn_size, &pdu), SRSRAN_SUCCESS);
    TESTASSERT_EQ(pdu.N_bytes, expected_size);

    // current state: ACK=12, NACKs empty

    // check the copy again - should be unchanged if not a shallow copy
    TESTASSERT_EQ(status_pdu_copy.ack_sn, 77);
    TESTASSERT_EQ(status_pdu_copy.packed_size, expected_size);
    TESTASSERT_EQ(rlc_am_nr_write_status_pdu(status_pdu_copy, sn_size, &pdu), SRSRAN_SUCCESS);
    TESTASSERT_EQ(pdu.N_bytes, expected_size);
  }

  return SRSRAN_SUCCESS;
}

///< Control PDU tests (18bit SN)
// Status PDU for 18bit SN with ACK_SN=235929=0x39999=0b11 1001 1001 1001 1001 and no further NACK_SN (E1 bit not set)
int rlc_am_nr_control_pdu_18bit_sn_test1()
{
  test_delimit_logger      delimiter("Control PDU (18bit SN) test 1");
  const int                len = 3;
  std::array<uint8_t, len> tv  = {0x0E, 0x66, 0x64};
  srsran::byte_buffer_t    pdu = make_pdu_and_log(tv);

  TESTASSERT(rlc_am_is_control_pdu(pdu.msg) == true);

  // unpack PDU
  rlc_am_nr_status_pdu_t status_pdu(srsran::rlc_am_nr_sn_size_t::size18bits);
  TESTASSERT(rlc_am_nr_read_status_pdu(&pdu, srsran::rlc_am_nr_sn_size_t::size18bits, &status_pdu) == SRSRAN_SUCCESS);
  TESTASSERT(status_pdu.ack_sn == 235929);
  TESTASSERT(status_pdu.nacks.size() == 0);

  // reset status PDU
  pdu.clear();

  // pack again
  TESTASSERT(rlc_am_nr_write_status_pdu(status_pdu, srsran::rlc_am_nr_sn_size_t::size18bits, &pdu) == SRSRAN_SUCCESS);
  TESTASSERT(pdu.N_bytes == tv.size());

  write_pdu_to_pcap(4, pdu.msg, pdu.N_bytes);

  TESTASSERT(memcmp(pdu.msg, tv.data(), pdu.N_bytes) == 0);

  return SRSRAN_SUCCESS;
}

// Status PDU for 18bit SN with ACK_SN=235929=0x39999=0b11 1001 1001 1001 1001 (E1 bit set)
//                         and NACK_SN=222822=0x36666=0b11 0110 0110 0110 0110
int rlc_am_nr_control_pdu_18bit_sn_test2()
{
  test_delimit_logger      delimiter("Control PDU (18bit SN) test 2");
  const int                len = 6;
  std::array<uint8_t, len> tv  = {0x0E, 0x66, 0x66, 0xD9, 0x99, 0x80};
  srsran::byte_buffer_t    pdu = make_pdu_and_log(tv);

  TESTASSERT(rlc_am_is_control_pdu(pdu.msg) == true);

  // unpack PDU
  rlc_am_nr_status_pdu_t status_pdu(srsran::rlc_am_nr_sn_size_t::size18bits);
  TESTASSERT(rlc_am_nr_read_status_pdu(&pdu, srsran::rlc_am_nr_sn_size_t::size18bits, &status_pdu) == SRSRAN_SUCCESS);
  TESTASSERT(status_pdu.ack_sn == 235929);
  TESTASSERT(status_pdu.nacks.size() == 1);
  TESTASSERT(status_pdu.nacks[0].nack_sn == 222822);

  // reset status PDU
  pdu.clear();

  // pack again
  TESTASSERT(rlc_am_nr_write_status_pdu(status_pdu, srsran::rlc_am_nr_sn_size_t::size18bits, &pdu) == SRSRAN_SUCCESS);
  TESTASSERT(pdu.N_bytes == tv.size());

  write_pdu_to_pcap(4, pdu.msg, pdu.N_bytes);

  TESTASSERT(memcmp(pdu.msg, tv.data(), pdu.N_bytes) == 0);

  return SRSRAN_SUCCESS;
}

// Status PDU for 18bit SN with ACK_SN=235929=0x39999=0b11 1001 1001 1001 1001 (E1 bit set),
//                             NACK_SN=222822=0x36666=0b11 0110 0110 0110 0110 (E1 and E2 bit set),
//                             SO_START=2, SO_END=5,
//                             NACK_SN=222975=0x366ff=0b11 0110 0110 1111 1111 (E2 bit set),
//                             SO_START=5, SO_END=0xFFFF
int rlc_am_nr_control_pdu_18bit_sn_test3()
{
  test_delimit_logger      delimiter("Control PDU (18bit SN) test 3");
  const int                len = 17;
  std::array<uint8_t, len> tv  = {0b00001110, // D/C | 3CPT | 4ACK_SN_upper
                                 0b01100110, // 8ACK_SN_center
                                 0b01100110, // 6ACK_SN_lower | E1 | R
                                 0b11011001, // 8NACK_SN_upper
                                 0b10011001, // 8NACK_SN_center
                                 0b10110000, // 2NACK_SN_lower | E1 | E2 | E3 | 3R
                                 0x00,       // 8SO_START_upper
                                 0x02,       // 8SO_START_lower
                                 0x00,       // 8SO_END_upper
                                 0x05,       // 8SO_END_lower
                                 0b11011001, // 8NACK_SN_upper
                                 0b10111111, // 8NACK_SN_center
                                 0b11010000, // 2NACK_SN_lower | E1 | E2 | E3 | 3R
                                 0x00,       // 8SO_START_upper
                                 0x05,       // 8SO_START_lower
                                 0xFF,       // 8SO_END_upper
                                 0xFF};      // 8SO_END_lower
  srsran::byte_buffer_t    pdu = make_pdu_and_log(tv);

  TESTASSERT(rlc_am_is_control_pdu(pdu.msg) == true);

  // unpack PDU
  rlc_am_nr_status_pdu_t status_pdu(srsran::rlc_am_nr_sn_size_t::size18bits);
  TESTASSERT(rlc_am_nr_read_status_pdu(&pdu, srsran::rlc_am_nr_sn_size_t::size18bits, &status_pdu) == SRSRAN_SUCCESS);
  TESTASSERT(status_pdu.ack_sn == 235929);
  TESTASSERT(status_pdu.nacks.size() == 2);
  TESTASSERT(status_pdu.nacks[0].nack_sn == 222822);
  TESTASSERT(status_pdu.nacks[0].has_so == true);
  TESTASSERT(status_pdu.nacks[0].so_start == 2);
  TESTASSERT(status_pdu.nacks[0].so_end == 5);
  TESTASSERT(status_pdu.nacks[1].nack_sn == 222975);
  TESTASSERT(status_pdu.nacks[1].has_so == true);
  TESTASSERT(status_pdu.nacks[1].so_start == 5);
  TESTASSERT(status_pdu.nacks[1].so_end == 0xFFFF);

  // reset status PDU
  pdu.clear();

  // pack again
  TESTASSERT(rlc_am_nr_write_status_pdu(status_pdu, srsran::rlc_am_nr_sn_size_t::size18bits, &pdu) == SRSRAN_SUCCESS);
  TESTASSERT(pdu.N_bytes == tv.size());

  write_pdu_to_pcap(4, pdu.msg, pdu.N_bytes);
  TESTASSERT(memcmp(pdu.msg, tv.data(), pdu.N_bytes) == 0);

  return SRSRAN_SUCCESS;
}

// Status PDU for 18bit SN with ACK_SN=235929=0x39999=0b11 1001 1001 1001 1001 (E1 bit set),
//                             NACK_SN=222822=0x36666=0b11 0110 0110 0110 0110 (E1 and E2 bit set),
//                             SO_START=2, SO_END=5,
//                             NACK_SN=222975=0x366ff=0b11 0110 0110 1111 1111 (E1 and E2 bit not set),
int rlc_am_nr_control_pdu_18bit_sn_test4()
{
  test_delimit_logger      delimiter("Control PDU (18bit SN) test 4");
  const int                len = 13;
  std::array<uint8_t, len> tv  = {0b00001110,  // D/C | 3CPT | 4ACK_SN_upper
                                 0b01100110,  // 8ACK_SN_center
                                 0b01100110,  // 6ACK_SN_lower | E1 | R
                                 0b11011001,  // 8NACK_SN_upper
                                 0b10011001,  // 8NACK_SN_center
                                 0b10110000,  // 2NACK_SN_lower | E1 | E2 | E3 | 3R
                                 0x00,        // 8SO_START_upper
                                 0x02,        // 8SO_START_lower
                                 0x00,        // 8SO_END_upper
                                 0x05,        // 8SO_END_lower
                                 0b11011001,  // 8NACK_SN_upper
                                 0b10111111,  // 8NACK_SN_center
                                 0b11000000}; // 2NACK_SN_lower | E1 | E2 | E3 | 3R
  srsran::byte_buffer_t    pdu = make_pdu_and_log(tv);

  TESTASSERT(rlc_am_is_control_pdu(pdu.msg) == true);

  // unpack PDU
  rlc_am_nr_status_pdu_t status_pdu(srsran::rlc_am_nr_sn_size_t::size18bits);
  TESTASSERT(rlc_am_nr_read_status_pdu(&pdu, srsran::rlc_am_nr_sn_size_t::size18bits, &status_pdu) == SRSRAN_SUCCESS);
  TESTASSERT(status_pdu.ack_sn == 235929);
  TESTASSERT(status_pdu.nacks.size() == 2);
  TESTASSERT(status_pdu.nacks[0].nack_sn == 222822);
  TESTASSERT(status_pdu.nacks[0].has_so == true);
  TESTASSERT(status_pdu.nacks[0].so_start == 2);
  TESTASSERT(status_pdu.nacks[0].so_end == 5);
  TESTASSERT(status_pdu.nacks[1].nack_sn == 222975);
  TESTASSERT(status_pdu.nacks[1].has_so == false);

  // reset status PDU
  pdu.clear();

  // pack again
  TESTASSERT(rlc_am_nr_write_status_pdu(status_pdu, srsran::rlc_am_nr_sn_size_t::size18bits, &pdu) == SRSRAN_SUCCESS);
  TESTASSERT(pdu.N_bytes == tv.size());

  write_pdu_to_pcap(4, pdu.msg, pdu.N_bytes);
  TESTASSERT(memcmp(pdu.msg, tv.data(), pdu.N_bytes) == 0);

  return SRSRAN_SUCCESS;
}

// Malformed Status PDU, similar to test3 but with E1 still set at the end of the PDU
// Status PDU for 18bit SN with ACK_SN=235929=0x39999=0b11 1001 1001 1001 1001 (E1 bit set),
//                             NACK_SN=222822=0x36666=0b11 0110 0110 0110 0110 (E1 and E2 bit set),
//                             SO_START=2, SO_END=5,
//                             NACK_SN=222975=0x366ff=0b11 0110 0110 1111 1111 ([!E1!] and E2 bit set),
//                             SO_START=5, SO_END=0xFFFF
int rlc_am_nr_control_pdu_18bit_sn_test5()
{
  test_delimit_logger      delimiter("Control PDU (18bit SN) test 5");
  const int                len = 17;
  std::array<uint8_t, len> tv  = {0b00001110, // D/C | 3CPT | 4ACK_SN_upper
                                 0b01100110, // 8ACK_SN_center
                                 0b01100110, // 6ACK_SN_lower | E1 | R
                                 0b11011001, // 8NACK_SN_upper
                                 0b10011001, // 8NACK_SN_center
                                 0b10110000, // 2NACK_SN_lower | E1 | E2 | E3 | 3R
                                 0x00,       // 8SO_START_upper
                                 0x02,       // 8SO_START_lower
                                 0x00,       // 8SO_END_upper
                                 0x05,       // 8SO_END_lower
                                 0b11011001, // 8NACK_SN_upper
                                 0b10111111, // 8NACK_SN_center
                                 0b11110000, // 2NACK_SN_lower | [!E1!] | E2 | E3 | 3R
                                 0x00,       // 8SO_START_upper
                                 0x05,       // 8SO_START_lower
                                 0xFF,       // 8SO_END_upper
                                 0xFF};      // 8SO_END_lower
  srsran::byte_buffer_t    pdu = make_pdu_and_log(tv);

  TESTASSERT(rlc_am_is_control_pdu(pdu.msg) == true);

  // unpack PDU
  rlc_am_nr_status_pdu_t status_pdu(srsran::rlc_am_nr_sn_size_t::size18bits);
  TESTASSERT(rlc_am_nr_read_status_pdu(&pdu, srsran::rlc_am_nr_sn_size_t::size18bits, &status_pdu) == 0);

  return SRSRAN_SUCCESS;
}

// Status PDU for 18bit SN with ACK_SN=200977=0x31111=0b11 0001 0001 0001 0001,
// NACK range0: 3 full SDUs,    NACK_SN=69905=0x11111=0b01 0001 0001 0001 0001
// NACK range1: missing segment sequence across 4 SDUs
//              starting at     NACK_SN=69913=0x11119=0b01 0001 0001 0001 1001, SO_START=2,
//              ending at       NACK_SN=69916, SO_END=5
// E1 and E3 bit set on first NACK, E2 and E3 bit set on the second.
int rlc_am_nr_control_pdu_18bit_sn_test_nack_range()
{
  test_delimit_logger      delimiter("Control PDU (18bit SN) test NACK range");
  const int                len = 15;
  std::array<uint8_t, len> tv  = {0b00001100, // D/C | 3CPT | 4ACK_SN_upper
                                 0b01000100, // 8ACK_SN_center
                                 0b01000110, // 6ACK_SN_lower | E1 | R
                                 0b01000100, // 8NACK_SN_upper
                                 0b01000100, // 8NACK_SN_center
                                 0b01101000, // 2NACK_SN_lower | E1 | E2 | E3 | 3R
                                 0x03,       // 8NACK_range
                                 0b01000100, // 8NACK_SN_upper
                                 0b01000110, // 8NACK_SN_center
                                 0b01011000, // 2NACK_SN_lower | E1 | E2 | E3 | 3R
                                 0x00,       // 8SO_START_upper
                                 0x02,       // 8SO_START_lower
                                 0x00,       // 8SO_END_upper
                                 0x05,       // 8SO_END_lower
                                 0x04};      // 8NACK_range
  srsran::byte_buffer_t    pdu = make_pdu_and_log(tv);

  TESTASSERT(rlc_am_is_control_pdu(pdu.msg) == true);

  // unpack PDU
  rlc_am_nr_status_pdu_t status_pdu(srsran::rlc_am_nr_sn_size_t::size18bits);
  TESTASSERT(rlc_am_nr_read_status_pdu(&pdu, srsran::rlc_am_nr_sn_size_t::size18bits, &status_pdu) == SRSRAN_SUCCESS);
  TESTASSERT(status_pdu.ack_sn == 200977);
  TESTASSERT(status_pdu.nacks.size() == 2);
  TESTASSERT(status_pdu.nacks[0].nack_sn == 69905);
  TESTASSERT(status_pdu.nacks[0].has_so == false);
  TESTASSERT(status_pdu.nacks[0].has_nack_range == true);
  TESTASSERT(status_pdu.nacks[0].nack_range == 3);

  TESTASSERT(status_pdu.nacks[1].nack_sn == 69913);
  TESTASSERT(status_pdu.nacks[1].has_so == true);
  TESTASSERT(status_pdu.nacks[1].so_start == 2);
  TESTASSERT(status_pdu.nacks[1].so_end == 5);
  TESTASSERT(status_pdu.nacks[1].has_nack_range == true);
  TESTASSERT(status_pdu.nacks[1].nack_range == 4);

  // reset status PDU
  pdu.clear();

  // pack again
  TESTASSERT(rlc_am_nr_write_status_pdu(status_pdu, srsran::rlc_am_nr_sn_size_t::size18bits, &pdu) == SRSRAN_SUCCESS);
  TESTASSERT(pdu.N_bytes == tv.size());

  write_pdu_to_pcap(4, pdu.msg, pdu.N_bytes);
  TESTASSERT(memcmp(pdu.msg, tv.data(), pdu.N_bytes) == 0);

  return SRSRAN_SUCCESS;
}

int main(int argc, char** argv)
{
  static const struct option long_options[] = {{"pcap", no_argument, nullptr, 'p'}, {nullptr, 0, nullptr, 0}};

  // Parse arguments
  while (true) {
    int option_index = 0;
    int c            = getopt_long(argc, argv, "p", long_options, &option_index);
    if (c == -1) {
      break;
    }

    switch (c) {
      case 'p':
        printf("Setting up PCAP\n");
        pcap_handle = std::unique_ptr<srsran::mac_pcap>(new srsran::mac_pcap());
        pcap_handle->open("rlc_am_nr_pdu_test.pcap");
        break;
      default:
        fprintf(stderr, "error parsing arguments\n");
        return SRSRAN_ERROR;
    }
  }

  srslog::init();

  if (rlc_am_nr_pdu_test1()) {
    fprintf(stderr, "rlc_am_nr_pdu_test1() failed.\n");
    return SRSRAN_ERROR;
  }

  if (rlc_am_nr_pdu_test2()) {
    fprintf(stderr, "rlc_am_nr_pdu_test2() failed.\n");
    return SRSRAN_ERROR;
  }

  if (rlc_am_nr_pdu_test3()) {
    fprintf(stderr, "rlc_am_nr_pdu_test3() failed.\n");
    return SRSRAN_ERROR;
  }

  if (rlc_am_nr_pdu_test4()) {
    fprintf(stderr, "rlc_am_nr_pdu_test4() failed.\n");
    return SRSRAN_ERROR;
  }

  if (rlc_am_nr_pdu_test5()) {
    fprintf(stderr, "rlc_am_nr_pdu_test5() failed.\n");
    return SRSRAN_ERROR;
  }

  if (rlc_am_nr_pdu_test6()) {
    fprintf(stderr, "rlc_am_nr_pdu_test6() failed.\n");
    return SRSRAN_ERROR;
  }

  if (rlc_am_nr_control_pdu_12bit_sn_test1()) {
    fprintf(stderr, "rlc_am_nr_control_pdu_12bit_sn_test1() failed.\n");
    return SRSRAN_ERROR;
  }

  if (rlc_am_nr_control_pdu_12bit_sn_test2()) {
    fprintf(stderr, "rlc_am_nr_control_pdu_12bit_sn_test2() failed.\n");
    return SRSRAN_ERROR;
  }

  if (rlc_am_nr_control_pdu_12bit_sn_test3()) {
    fprintf(stderr, "rlc_am_nr_control_pdu_12bit_sn_test3() failed.\n");
    return SRSRAN_ERROR;
  }

  if (rlc_am_nr_control_pdu_12bit_sn_test4()) {
    fprintf(stderr, "rlc_am_nr_control_pdu_12bit_sn_test4() failed.\n");
    return SRSRAN_ERROR;
  }

  if (rlc_am_nr_control_pdu_12bit_sn_test5()) {
    fprintf(stderr, "rlc_am_nr_control_pdu_12bit_sn_test5() failed.\n");
    return SRSRAN_ERROR;
  }

  if (rlc_am_nr_control_pdu_12bit_sn_test_nack_range()) {
    fprintf(stderr, "rlc_am_nr_control_pdu_12bit_sn_test_nack_range() failed.\n");
    return SRSRAN_ERROR;
  }

  if (rlc_am_nr_control_pdu_test_nack_merge_sdu_sdu(rlc_am_nr_sn_size_t::size12bits)) {
    fprintf(stderr, "rlc_am_nr_control_pdu_test_nack_merge_sdu_sdu(size12bits) failed.\n");
    return SRSRAN_ERROR;
  }

  if (rlc_am_nr_control_pdu_test_nack_merge_range_sdu(rlc_am_nr_sn_size_t::size12bits)) {
    fprintf(stderr, "rlc_am_nr_control_pdu_test_nack_merge_range_sdu(size12bits) failed.\n");
    return SRSRAN_ERROR;
  }

  if (rlc_am_nr_control_pdu_test_nack_merge_sdu_range(rlc_am_nr_sn_size_t::size12bits)) {
    fprintf(stderr, "rlc_am_nr_control_pdu_test_nack_merge_sdu_range(size12bits) failed.\n");
    return SRSRAN_ERROR;
  }

  if (rlc_am_nr_control_pdu_test_nack_merge_range_range(rlc_am_nr_sn_size_t::size12bits)) {
    fprintf(stderr, "rlc_am_nr_control_pdu_test_nack_merge_range_range(size12bits) failed.\n");
    return SRSRAN_ERROR;
  }

  if (rlc_am_nr_control_pdu_test_trimming(rlc_am_nr_sn_size_t::size12bits)) {
    fprintf(stderr, "rlc_am_nr_control_pdu_test_trimming(size12bits) failed.\n");
    return SRSRAN_ERROR;
  }

  if (rlc_am_nr_control_pdu_18bit_sn_test1()) {
    fprintf(stderr, "rlc_am_nr_control_pdu_18bit_sn_test1() failed.\n");
    return SRSRAN_ERROR;
  }

  if (rlc_am_nr_control_pdu_18bit_sn_test2()) {
    fprintf(stderr, "rlc_am_nr_control_pdu_18bit_sn_test2() failed.\n");
    return SRSRAN_ERROR;
  }

  if (rlc_am_nr_control_pdu_18bit_sn_test3()) {
    fprintf(stderr, "rlc_am_nr_control_pdu_18bit_sn_test3() failed.\n");
    return SRSRAN_ERROR;
  }

  if (rlc_am_nr_control_pdu_18bit_sn_test4()) {
    fprintf(stderr, "rlc_am_nr_control_pdu_18bit_sn_test4() failed.\n");
    return SRSRAN_ERROR;
  }

  if (rlc_am_nr_control_pdu_18bit_sn_test5()) {
    fprintf(stderr, "rlc_am_nr_control_pdu_18bit_sn_test5() failed.\n");
    return SRSRAN_ERROR;
  }

  if (rlc_am_nr_control_pdu_18bit_sn_test_nack_range()) {
    fprintf(stderr, "rlc_am_nr_control_pdu_18bit_sn_test_nack_range() failed.\n");
    return SRSRAN_ERROR;
  }

  if (rlc_am_nr_control_pdu_test_nack_merge_sdu_sdu(rlc_am_nr_sn_size_t::size18bits)) {
    fprintf(stderr, "rlc_am_nr_control_pdu_test_nack_merge_sdu_sdu(size18bits) failed.\n");
    return SRSRAN_ERROR;
  }

  if (rlc_am_nr_control_pdu_test_nack_merge_range_sdu(rlc_am_nr_sn_size_t::size18bits)) {
    fprintf(stderr, "rlc_am_nr_control_pdu_test_nack_merge_range_sdu(size18bits) failed.\n");
    return SRSRAN_ERROR;
  }

  if (rlc_am_nr_control_pdu_test_nack_merge_sdu_range(rlc_am_nr_sn_size_t::size18bits)) {
    fprintf(stderr, "rlc_am_nr_control_pdu_test_nack_merge_sdu_range(size18bits) failed.\n");
    return SRSRAN_ERROR;
  }

  if (rlc_am_nr_control_pdu_test_nack_merge_range_range(rlc_am_nr_sn_size_t::size18bits)) {
    fprintf(stderr, "rlc_am_nr_control_pdu_test_nack_merge_range_range(size18bits) failed.\n");
    return SRSRAN_ERROR;
  }

  if (rlc_am_nr_control_pdu_test_trimming(rlc_am_nr_sn_size_t::size18bits)) {
    fprintf(stderr, "rlc_am_nr_control_pdu_test_trimming(size18bits) failed.\n");
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}
