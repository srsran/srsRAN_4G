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

#include "srsran/config.h"
#include "srsran/rlc/rlc.h"
#include "srsran/rlc/rlc_um_nr.h"

#include <array>
#include <iostream>
#include <memory>
#include <vector>

#define TESTASSERT(cond)                                                                                               \
  {                                                                                                                    \
    if (!(cond)) {                                                                                                     \
      std::cout << "[" << __FUNCTION__ << "][Line " << __LINE__ << "]: FAIL at " << (#cond) << std::endl;              \
      return -1;                                                                                                       \
    }                                                                                                                  \
  }

#define PCAP 1
#define PCAP_CRNTI (0x1001)
#define PCAP_TTI (666)

using namespace srsran;

#if PCAP
#include "srsran/common/mac_pcap.h"
#include "srsran/mac/mac_sch_pdu_nr.h"
static std::unique_ptr<srsran::mac_pcap> pcap_handle = nullptr;
#endif

int write_pdu_to_pcap(const uint32_t lcid, const uint8_t* payload, const uint32_t len)
{
#if PCAP
  if (pcap_handle) {
    byte_buffer_t          tx_buffer;
    srsran::mac_sch_pdu_nr tx_pdu;
    tx_pdu.init_tx(&tx_buffer, len + 10);
    tx_pdu.add_sdu(lcid, payload, len);
    tx_pdu.pack();
    pcap_handle->write_dl_crnti_nr(tx_buffer.msg, tx_buffer.N_bytes, PCAP_CRNTI, true, PCAP_TTI);
    return SRSRAN_SUCCESS;
  }
#endif
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

// RLC UM PDU with complete SDU
int rlc_um_nr_pdu_test1()
{
  const int                                     header_len = 1, payload_len = 4;
  std::array<uint8_t, header_len + payload_len> tv  = {0x00, 0x11, 0x22, 0x33, 0x44};
  srsran::byte_buffer_t                         pdu = make_pdu_and_log(tv);

  // unpack PDU
  rlc_um_nr_pdu_header_t header = {};
  TESTASSERT(rlc_um_nr_read_data_pdu_header(&pdu, srsran::rlc_um_nr_sn_size_t::size6bits, &header) != 0);
  TESTASSERT(header.si == rlc_nr_si_field_t::full_sdu);

  // clear header
  corrupt_pdu_header(pdu, header_len, payload_len);

  // pack again
  TESTASSERT(rlc_um_nr_write_data_pdu_header(header, &pdu) == header_len);
  TESTASSERT(pdu.N_bytes == tv.size());
  TESTASSERT(memcmp(pdu.msg, tv.data(), pdu.N_bytes) == 0);

  return SRSRAN_SUCCESS;
}

// RLC UM PDU with 6 Bit SN carrying the last segment of an SDU
int rlc_um_nr_pdu_test2()
{
  // SN = 1
  const int                                           header_len = 3, payload_len = 4;
  const std::array<uint8_t, header_len + payload_len> tv  = {0x81, 0x01, 0x02, 0x11, 0x22, 0x33, 0x44};
  srsran::byte_buffer_t                               pdu = make_pdu_and_log(tv);

  // unpack PDU
  rlc_um_nr_pdu_header_t header = {};
  TESTASSERT(rlc_um_nr_read_data_pdu_header(&pdu, srsran::rlc_um_nr_sn_size_t::size6bits, &header) != 0);

  TESTASSERT(header.si == rlc_nr_si_field_t::last_segment);
  TESTASSERT(header.sn == 1);
  TESTASSERT(header.so == 258);
  TESTASSERT(header.sn_size == srsran::rlc_um_nr_sn_size_t::size6bits);

  // clear header
  corrupt_pdu_header(pdu, header_len, payload_len);

  // pack again
  TESTASSERT(rlc_um_nr_write_data_pdu_header(header, &pdu) == header_len);
  TESTASSERT(pdu.N_bytes == tv.size());
  TESTASSERT(memcmp(pdu.msg, tv.data(), pdu.N_bytes) == 0);

  return SRSRAN_SUCCESS;
}

// RLC UM PDU with 6 Bit SN carrying a middle segment of an SDU
int rlc_um_nr_pdu_test3()
{
  // SN = 3
  const int                                           header_len = 3, payload_len = 4;
  const std::array<uint8_t, header_len + payload_len> tv  = {0xc3, 0x01, 0x02, 0x11, 0x22, 0x33, 0x44};
  srsran::byte_buffer_t                               pdu = make_pdu_and_log(tv);

  // unpack PDU
  rlc_um_nr_pdu_header_t header = {};
  TESTASSERT(rlc_um_nr_read_data_pdu_header(&pdu, srsran::rlc_um_nr_sn_size_t::size6bits, &header) != 0);

  TESTASSERT(header.si == rlc_nr_si_field_t::neither_first_nor_last_segment);
  TESTASSERT(header.so == 258);
  TESTASSERT(header.sn == 3);
  TESTASSERT(header.sn_size == srsran::rlc_um_nr_sn_size_t::size6bits);

  // clear header
  corrupt_pdu_header(pdu, header_len, payload_len);

  // pack again
  TESTASSERT(rlc_um_nr_write_data_pdu_header(header, &pdu) == header_len);
  TESTASSERT(pdu.N_bytes == tv.size());
  TESTASSERT(memcmp(pdu.msg, tv.data(), pdu.N_bytes) == 0);

  return SRSRAN_SUCCESS;
}

// RLC UM PDU with 6 Bit SN carrying a first segment of an SDU
int rlc_um_nr_pdu_test4()
{
  // SN = 31
  const int                                     header_len = 1, payload_len = 4;
  std::array<uint8_t, header_len + payload_len> tv  = {0x5f, 0x11, 0x22, 0x33, 0x44};
  srsran::byte_buffer_t                         pdu = make_pdu_and_log(tv);

  // unpack PDU
  rlc_um_nr_pdu_header_t header = {};
  TESTASSERT(rlc_um_nr_read_data_pdu_header(&pdu, srsran::rlc_um_nr_sn_size_t::size6bits, &header) != 0);

  TESTASSERT(header.si == rlc_nr_si_field_t::first_segment);
  TESTASSERT(header.so == 0);
  TESTASSERT(header.sn == 31);
  TESTASSERT(header.sn_size == srsran::rlc_um_nr_sn_size_t::size6bits);

  // clear header
  corrupt_pdu_header(pdu, header_len, payload_len);

  // pack again
  TESTASSERT(rlc_um_nr_write_data_pdu_header(header, &pdu) == header_len);
  TESTASSERT(pdu.N_bytes == tv.size());
  TESTASSERT(memcmp(pdu.msg, tv.data(), pdu.N_bytes) == 0);

  return SRSRAN_SUCCESS;
}

// This should fail unpacking because the PDU has reserved bits set
int rlc_um_nr_pdu_unpack_test5()
{
  std::array<uint8_t, 7> tv  = {0x33, 0x01, 0x02, 0x11, 0x22, 0x33, 0x44};
  srsran::byte_buffer_t  pdu = make_pdu_and_log(tv);

  // unpack PDU
  rlc_um_nr_pdu_header_t header = {};
  TESTASSERT(rlc_um_nr_read_data_pdu_header(&pdu, srsran::rlc_um_nr_sn_size_t::size6bits, &header) == 0);

  return SRSRAN_SUCCESS;
}

// Unpack RLC UM 12bit SN PDU with PDCP and ICMP
int rlc_um_nr_pdu_unpack_test6()
{
  std::array<uint8_t, 88> tv = {
      0x00, 0x80, 0x00, 0x01, 0x45, 0x00, 0x00, 0x54, 0x34, 0xee, 0x40, 0x00, 0x40, 0x01, 0x80, 0x67, 0xc0, 0xa8,
      0x02, 0x01, 0xc0, 0xa8, 0x02, 0x02, 0x08, 0x00, 0xf0, 0x38, 0x56, 0x9b, 0x00, 0x02, 0x74, 0x40, 0x35, 0x60,
      0x00, 0x00, 0x00, 0x00, 0x3e, 0xb6, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
      0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
      0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37};
  srsran::byte_buffer_t pdu = make_pdu_and_log(tv);

  // unpack PDU
  rlc_um_nr_pdu_header_t header = {};
  TESTASSERT(rlc_um_nr_read_data_pdu_header(&pdu, srsran::rlc_um_nr_sn_size_t::size12bits, &header) != 0);

  TESTASSERT(header.si == rlc_nr_si_field_t::full_sdu);
  TESTASSERT(header.so == 0);
  TESTASSERT(header.sn == 0);

  TESTASSERT(rlc_um_nr_packed_length(header) == 1);

  return SRSRAN_SUCCESS;
}

// Unpack RLC UM 12bit SN middle segment PDU SI=11 SN=5 SO=360
int rlc_um_nr_pdu_unpack_test7()
{
  std::array<uint8_t, 183> tv = {
      0xc0, 0x05, 0x01, 0x68, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
      0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a,
      0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d,
      0x7e, 0x7f, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x90,
      0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, 0xa0, 0xa1, 0xa2, 0xa3,
      0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, 0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6,
      0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9,
      0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc,
      0xdd, 0xde, 0xdf, 0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
      0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb};
  const uint32_t        header_len = 4;
  srsran::byte_buffer_t pdu        = make_pdu_and_log(tv);

  // unpack PDU
  rlc_um_nr_pdu_header_t header = {};
  TESTASSERT(rlc_um_nr_read_data_pdu_header(&pdu, srsran::rlc_um_nr_sn_size_t::size12bits, &header) != 0);

  TESTASSERT(header.si == rlc_nr_si_field_t::neither_first_nor_last_segment);
  TESTASSERT(header.so == 360);
  TESTASSERT(header.sn == 5);

  TESTASSERT(rlc_um_nr_packed_length(header) == header_len);

  // clear header
  corrupt_pdu_header(pdu, header_len, tv.size() - header_len);

  // pack again
  TESTASSERT(rlc_um_nr_write_data_pdu_header(header, &pdu) == header_len);
  TESTASSERT(pdu.N_bytes == tv.size());
  TESTASSERT(memcmp(pdu.msg, tv.data(), pdu.N_bytes) == 0);

  return SRSRAN_SUCCESS;
}

// Unpack RLC UM 12bit SN first segment PDU SI=01 SN=5
int rlc_um_nr_pdu_unpack_test8()
{
  std::array<uint8_t, 183> tv = {
      0x40, 0x05, 0x80, 0x00, 0x05, 0x45, 0x00, 0x04, 0x04, 0xf1, 0xaf, 0x40, 0x00, 0x40, 0x01, 0xbd, 0xf5, 0xc0, 0xa8,
      0x03, 0x01, 0xc0, 0xa8, 0x03, 0x02, 0x08, 0x00, 0xbc, 0xea, 0x0b, 0x0d, 0x00, 0x07, 0x81, 0x10, 0x5b, 0x60, 0x00,
      0x00, 0x00, 0x00, 0xed, 0x4c, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
      0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a,
      0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d,
      0x3e, 0x3f, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50,
      0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x60, 0x61, 0x62, 0x63,
      0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76,
      0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
      0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95};
  const uint32_t        header_len = 2;
  srsran::byte_buffer_t pdu        = make_pdu_and_log(tv);

  // unpack PDU
  rlc_um_nr_pdu_header_t header = {};
  TESTASSERT(rlc_um_nr_read_data_pdu_header(&pdu, srsran::rlc_um_nr_sn_size_t::size12bits, &header) != 0);

  TESTASSERT(header.si == rlc_nr_si_field_t::first_segment);
  TESTASSERT(header.so == 0);
  TESTASSERT(header.sn == 5);

  TESTASSERT(rlc_um_nr_packed_length(header) == header_len);

  // clear header
  corrupt_pdu_header(pdu, header_len, tv.size() - header_len);

  // pack again
  TESTASSERT(rlc_um_nr_write_data_pdu_header(header, &pdu) == header_len);
  TESTASSERT(pdu.N_bytes == tv.size());
  TESTASSERT(memcmp(pdu.msg, tv.data(), pdu.N_bytes) == 0);

  return SRSRAN_SUCCESS;
}

// Unpack/pack RLC UM 12bit SN last segment PDU SI=10 SN=1542 SO=972
int rlc_um_nr_pdu_unpack_test9()
{
  std::array<uint8_t, 106> tv         = {0x86, 0x06, 0x03, 0xcc, 0xad, 0xae, 0xaf, 0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5,
                                 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf, 0xc0, 0xc1, 0xc2,
                                 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
                                 0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc,
                                 0xdd, 0xde, 0xdf, 0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7};
  const uint32_t           header_len = 4;

  srsran::byte_buffer_t pdu = make_pdu_and_log(tv);

  // unpack PDU
  rlc_um_nr_pdu_header_t header = {};
  TESTASSERT(rlc_um_nr_read_data_pdu_header(&pdu, srsran::rlc_um_nr_sn_size_t::size12bits, &header) != 0);

  TESTASSERT(header.si == rlc_nr_si_field_t::last_segment);
  TESTASSERT(header.so == 972);
  TESTASSERT(header.sn == 1542);

  TESTASSERT(rlc_um_nr_packed_length(header) == header_len);

  // clear header
  corrupt_pdu_header(pdu, header_len, tv.size() - header_len);

  // pack again
  TESTASSERT(rlc_um_nr_write_data_pdu_header(header, &pdu) == header_len);
  TESTASSERT(pdu.N_bytes == tv.size());
  TESTASSERT(memcmp(pdu.msg, tv.data(), pdu.N_bytes) == 0);

  return SRSRAN_SUCCESS;
}

int main(int argc, char** argv)
{
#if PCAP
  pcap_handle = std::unique_ptr<srsran::mac_pcap>(new srsran::mac_pcap());
  pcap_handle->open("rlc_um_nr_pdu_test.pcap");
#endif

  srslog::init();

  if (rlc_um_nr_pdu_test1()) {
    fprintf(stderr, "rlc_um_nr_pdu_test1() failed.\n");
    return SRSRAN_ERROR;
  }

  if (rlc_um_nr_pdu_test2()) {
    fprintf(stderr, "rlc_um_nr_pdu_test2() failed.\n");
    return SRSRAN_ERROR;
  }

  if (rlc_um_nr_pdu_test3()) {
    fprintf(stderr, "rlc_um_nr_pdu_test3() failed.\n");
    return SRSRAN_ERROR;
  }

  if (rlc_um_nr_pdu_test4()) {
    fprintf(stderr, "rlc_um_nr_pdu_test4() failed.\n");
    return SRSRAN_ERROR;
  }

  if (rlc_um_nr_pdu_unpack_test5()) {
    fprintf(stderr, "rlc_um_nr_pdu_unpack_test5() failed.\n");
    return SRSRAN_ERROR;
  }

  if (rlc_um_nr_pdu_unpack_test6()) {
    fprintf(stderr, "rlc_um_nr_pdu_unpack_test6() failed.\n");
    return SRSRAN_ERROR;
  }

  if (rlc_um_nr_pdu_unpack_test7()) {
    fprintf(stderr, "rlc_um_nr_pdu_unpack_test7() failed.\n");
    return SRSRAN_ERROR;
  }

  if (rlc_um_nr_pdu_unpack_test8()) {
    fprintf(stderr, "rlc_um_nr_pdu_unpack_test8() failed.\n");
    return SRSRAN_ERROR;
  }

  if (rlc_um_nr_pdu_unpack_test9()) {
    fprintf(stderr, "rlc_um_nr_pdu_unpack_test9() failed.\n");
    return SRSRAN_ERROR;
  }

#if PCAP
  pcap_handle->close();
#endif

  return SRSRAN_SUCCESS;
}
