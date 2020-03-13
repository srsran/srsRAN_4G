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

#include "srslte/config.h"
#include "srslte/upper/rlc.h"
#include "srslte/upper/rlc_am_nr.h"

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

#define PCAP 0
#define PCAP_CRNTI (0x1001)
#define PCAP_TTI (666)

using namespace srslte;

#if PCAP
#include "srslte/common/mac_nr_pcap.h"
#include "srslte/common/mac_nr_pdu.h"
static std::unique_ptr<srslte::mac_nr_pcap> pcap_handle = nullptr;
#endif

int write_pdu_to_pcap(const uint32_t lcid, const uint8_t* payload, const uint32_t len)
{
#if PCAP
  if (pcap_handle) {
    byte_buffer_t          tx_buffer;
    srslte::mac_nr_sch_pdu tx_pdu;
    tx_pdu.init_tx(&tx_buffer, len + 10);
    tx_pdu.add_sdu(lcid, payload, len);
    tx_pdu.pack();
    pcap_handle->write_dl_crnti(tx_buffer.msg, tx_buffer.N_bytes, PCAP_CRNTI, true, PCAP_TTI);
    return SRSLTE_SUCCESS;
  }
#endif
  return SRSLTE_ERROR;
}

template <std::size_t N>
srslte::byte_buffer_t make_pdu_and_log(const std::array<uint8_t, N>& tv)
{
  srslte::byte_buffer_t pdu;
  memcpy(pdu.msg, tv.data(), tv.size());
  pdu.N_bytes = tv.size();
  write_pdu_to_pcap(4, tv.data(), tv.size());
  return pdu;
}

void corrupt_pdu_header(srslte::byte_buffer_t& pdu, const uint32_t header_len, const uint32_t payload_len)
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
  const int                                     header_len = 2, payload_len = 4;
  std::array<uint8_t, header_len + payload_len> tv  = {0x80, 0x00, 0x11, 0x22, 0x33, 0x44};
  srslte::byte_buffer_t                         pdu = make_pdu_and_log(tv);

  // unpack PDU
  rlc_am_nr_pdu_header_t header = {};
  TESTASSERT(rlc_am_nr_read_data_pdu_header(&pdu, srslte::rlc_am_nr_sn_size_t::size12bits, &header) != 0);
  TESTASSERT(header.si == rlc_nr_si_field_t::full_sdu);

  // clear header
  corrupt_pdu_header(pdu, header_len, payload_len);

  // pack again
  TESTASSERT(rlc_am_nr_write_data_pdu_header(header, &pdu) == header_len);
  TESTASSERT(pdu.N_bytes == tv.size());
  TESTASSERT(memcmp(pdu.msg, tv.data(), pdu.N_bytes) == 0);

  return SRSLTE_SUCCESS;
}

// RLC AM PDU 12bit first segment of SDU with P flag and SN 511
int rlc_am_nr_pdu_test2()
{
  const int                                     header_len = 2, payload_len = 4;
  std::array<uint8_t, header_len + payload_len> tv  = {0xd1, 0xff, 0x11, 0x22, 0x33, 0x44};
  srslte::byte_buffer_t                         pdu = make_pdu_and_log(tv);

  // unpack PDU
  rlc_am_nr_pdu_header_t header = {};
  TESTASSERT(rlc_am_nr_read_data_pdu_header(&pdu, srslte::rlc_am_nr_sn_size_t::size12bits, &header) != 0);
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

  return SRSLTE_SUCCESS;
}

// RLC AM PDU 12bit last segment of SDU without P flag and SN 0x0404 and SO 0x0404 (1028)
int rlc_am_nr_pdu_test3()
{
  const int                                     header_len = 4, payload_len = 4;
  std::array<uint8_t, header_len + payload_len> tv  = {0xa4, 0x04, 0x04, 0x04, 0x11, 0x22, 0x33, 0x44};
  srslte::byte_buffer_t                         pdu = make_pdu_and_log(tv);

  // unpack PDU
  rlc_am_nr_pdu_header_t header = {};
  TESTASSERT(rlc_am_nr_read_data_pdu_header(&pdu, srslte::rlc_am_nr_sn_size_t::size12bits, &header) != 0);
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

  return SRSLTE_SUCCESS;
}

// RLC AM PDU 18bit full SDU with P flag and SN 0x100000001000000010 (131586)
int rlc_am_nr_pdu_test4()
{
  const int                                     header_len = 3, payload_len = 4;
  std::array<uint8_t, header_len + payload_len> tv  = {0xc2, 0x02, 0x02, 0x11, 0x22, 0x33, 0x44};
  srslte::byte_buffer_t                         pdu = make_pdu_and_log(tv);

  // unpack PDU
  rlc_am_nr_pdu_header_t header = {};
  TESTASSERT(rlc_am_nr_read_data_pdu_header(&pdu, srslte::rlc_am_nr_sn_size_t::size18bits, &header) != 0);
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

  return SRSLTE_SUCCESS;
}

// RLC AM PDU 18bit middle part of SDU (SO 514) without P flag and SN 131327
int rlc_am_nr_pdu_test5()
{
  const int                                     header_len = 5, payload_len = 4;
  std::array<uint8_t, header_len + payload_len> tv  = {0xb2, 0x00, 0xff, 0x02, 0x02, 0x11, 0x22, 0x33, 0x44};
  srslte::byte_buffer_t                         pdu = make_pdu_and_log(tv);

  // unpack PDU
  rlc_am_nr_pdu_header_t header = {};
  TESTASSERT(rlc_am_nr_read_data_pdu_header(&pdu, srslte::rlc_am_nr_sn_size_t::size18bits, &header) != 0);
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

  return SRSLTE_SUCCESS;
}

// Malformed RLC AM PDU 18bit with reserved bits set
int rlc_am_nr_pdu_test6()
{
  const int                                     header_len = 5, payload_len = 4;
  std::array<uint8_t, header_len + payload_len> tv  = {0xb7, 0x00, 0xff, 0x02, 0x02, 0x11, 0x22, 0x33, 0x44};
  srslte::byte_buffer_t                         pdu = make_pdu_and_log(tv);

  // unpack PDU
  rlc_am_nr_pdu_header_t header = {};
  TESTASSERT(rlc_am_nr_read_data_pdu_header(&pdu, srslte::rlc_am_nr_sn_size_t::size18bits, &header) == 0);
  TESTASSERT(header.sn == 0);

  return SRSLTE_SUCCESS;
}

///< Control PDU tests
// Status PDU for 12bit SN with ACK_SN=2065 and no further NACK_SN (E1 bit not set)
int rlc_am_nr_control_pdu_test1()
{
  const int                len = 3;
  std::array<uint8_t, len> tv  = {0x08, 0x11, 0x00};
  srslte::byte_buffer_t    pdu = make_pdu_and_log(tv);

  TESTASSERT(rlc_am_is_control_pdu(pdu.msg) == true);

  // unpack PDU
  rlc_am_nr_status_pdu_t status_pdu = {};
  TESTASSERT(rlc_am_nr_read_status_pdu(&pdu, srslte::rlc_am_nr_sn_size_t::size12bits, &status_pdu) == SRSLTE_SUCCESS);
  TESTASSERT(status_pdu.ack_sn == 2065);
  TESTASSERT(status_pdu.N_nack == 0);

  // reset status PDU
  pdu.clear();

  // pack again
  TESTASSERT(rlc_am_nr_write_status_pdu(status_pdu, srslte::rlc_am_nr_sn_size_t::size12bits, &pdu) == SRSLTE_SUCCESS);
  TESTASSERT(pdu.N_bytes == tv.size());

  write_pdu_to_pcap(4, pdu.msg, pdu.N_bytes);

  TESTASSERT(memcmp(pdu.msg, tv.data(), pdu.N_bytes) == 0);

  return SRSLTE_SUCCESS;
}

// Status PDU for 12bit SN with ACK_SN=2065 and NACK_SN=273 (E1 bit set)
int rlc_am_nr_control_pdu_test2()
{
  const int                len = 5;
  std::array<uint8_t, len> tv  = {0x08, 0x11, 0x80, 0x11, 0x10};
  srslte::byte_buffer_t    pdu = make_pdu_and_log(tv);

  TESTASSERT(rlc_am_is_control_pdu(pdu.msg) == true);

  // unpack PDU
  rlc_am_nr_status_pdu_t status_pdu = {};
  TESTASSERT(rlc_am_nr_read_status_pdu(&pdu, srslte::rlc_am_nr_sn_size_t::size12bits, &status_pdu) == SRSLTE_SUCCESS);
  TESTASSERT(status_pdu.ack_sn == 2065);
  TESTASSERT(status_pdu.N_nack == 1);
  TESTASSERT(status_pdu.nacks[0].nack_sn == 273);

  // reset status PDU
  pdu.clear();

  // pack again
  TESTASSERT(rlc_am_nr_write_status_pdu(status_pdu, srslte::rlc_am_nr_sn_size_t::size12bits, &pdu) == SRSLTE_SUCCESS);
  // TESTASSERT(pdu.N_bytes == tv.size());

  write_pdu_to_pcap(4, pdu.msg, pdu.N_bytes);

  TESTASSERT(memcmp(pdu.msg, tv.data(), pdu.N_bytes) == 0);

  return SRSLTE_SUCCESS;
}

int main(int argc, char** argv)
{
#if PCAP
  pcap_handle = std::unique_ptr<srslte::mac_nr_pcap>(new srslte::mac_nr_pcap());
  pcap_handle->open("rlc_am_nr_pdu_test.pcap");
#endif

  if (rlc_am_nr_pdu_test1()) {
    fprintf(stderr, "rlc_am_nr_pdu_test1() failed.\n");
    return SRSLTE_ERROR;
  }

  if (rlc_am_nr_pdu_test2()) {
    fprintf(stderr, "rlc_am_nr_pdu_test2() failed.\n");
    return SRSLTE_ERROR;
  }

  if (rlc_am_nr_pdu_test3()) {
    fprintf(stderr, "rlc_am_nr_pdu_test3() failed.\n");
    return SRSLTE_ERROR;
  }

  if (rlc_am_nr_pdu_test4()) {
    fprintf(stderr, "rlc_am_nr_pdu_test4() failed.\n");
    return SRSLTE_ERROR;
  }

  if (rlc_am_nr_pdu_test5()) {
    fprintf(stderr, "rlc_am_nr_pdu_test5() failed.\n");
    return SRSLTE_ERROR;
  }

  if (rlc_am_nr_pdu_test6()) {
    fprintf(stderr, "rlc_am_nr_pdu_test6() failed.\n");
    return SRSLTE_ERROR;
  }

  if (rlc_am_nr_control_pdu_test1()) {
    fprintf(stderr, "rlc_am_nr_control_pdu_test1() failed.\n");
    return SRSLTE_ERROR;
  }

  if (rlc_am_nr_control_pdu_test2()) {
    fprintf(stderr, "rlc_am_nr_control_pdu_test2() failed.\n");
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}
