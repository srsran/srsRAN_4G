/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
  rlc_am_nr_status_pdu_t status_pdu = {};
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
  rlc_am_nr_status_pdu_t status_pdu = {};
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
  rlc_am_nr_status_pdu_t status_pdu = {};
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
  rlc_am_nr_status_pdu_t status_pdu = {};
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
  rlc_am_nr_status_pdu_t status_pdu = {};
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
  rlc_am_nr_status_pdu_t status_pdu = {};
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
  rlc_am_nr_status_pdu_t status_pdu = {};
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
  rlc_am_nr_status_pdu_t status_pdu = {};
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
  rlc_am_nr_status_pdu_t status_pdu = {};
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
  rlc_am_nr_status_pdu_t status_pdu = {};
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
  rlc_am_nr_status_pdu_t status_pdu = {};
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
  rlc_am_nr_status_pdu_t status_pdu = {};
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

  return SRSRAN_SUCCESS;
}
