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
#include "srslte/upper/rlc_um_nr.h"

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
#include "srslte/mac/mac_nr_pdu.h"
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

// RLC UM PDU with complete SDU
int rlc_um_nr_pdu_test1()
{
  const int                                     header_len = 1, payload_len = 4;
  std::array<uint8_t, header_len + payload_len> tv  = {0x00, 0x11, 0x22, 0x33, 0x44};
  srslte::byte_buffer_t                         pdu = make_pdu_and_log(tv);

  // unpack PDU
  rlc_um_nr_pdu_header_t header = {};
  TESTASSERT(rlc_um_nr_read_data_pdu_header(&pdu, srslte::rlc_um_nr_sn_size_t::size6bits, &header) != 0);
  TESTASSERT(header.si == rlc_nr_si_field_t::full_sdu);

  // clear header
  corrupt_pdu_header(pdu, header_len, payload_len);

  // pack again
  TESTASSERT(rlc_um_nr_write_data_pdu_header(header, &pdu) == header_len);
  TESTASSERT(pdu.N_bytes == tv.size());
  TESTASSERT(memcmp(pdu.msg, tv.data(), pdu.N_bytes) == 0);

  return SRSLTE_SUCCESS;
}

// RLC UM PDU with 6 Bit SN carrying the last segment of an SDU
int rlc_um_nr_pdu_test2()
{
  // SN = 1
  const int                                           header_len = 3, payload_len = 4;
  const std::array<uint8_t, header_len + payload_len> tv  = {0x81, 0x01, 0x02, 0x11, 0x22, 0x33, 0x44};
  srslte::byte_buffer_t                               pdu = make_pdu_and_log(tv);

  // unpack PDU
  rlc_um_nr_pdu_header_t header = {};
  TESTASSERT(rlc_um_nr_read_data_pdu_header(&pdu, srslte::rlc_um_nr_sn_size_t::size6bits, &header) != 0);

  TESTASSERT(header.si == rlc_nr_si_field_t::last_segment);
  TESTASSERT(header.sn == 1);
  TESTASSERT(header.so == 258);
  TESTASSERT(header.sn_size == srslte::rlc_um_nr_sn_size_t::size6bits);

  // clear header
  corrupt_pdu_header(pdu, header_len, payload_len);

  // pack again
  TESTASSERT(rlc_um_nr_write_data_pdu_header(header, &pdu) == header_len);
  TESTASSERT(pdu.N_bytes == tv.size());
  TESTASSERT(memcmp(pdu.msg, tv.data(), pdu.N_bytes) == 0);

  return SRSLTE_SUCCESS;
}

// RLC UM PDU with 6 Bit SN carrying a middle segment of an SDU
int rlc_um_nr_pdu_test3()
{
  // SN = 3
  const int                                           header_len = 3, payload_len = 4;
  const std::array<uint8_t, header_len + payload_len> tv  = {0xc3, 0x01, 0x02, 0x11, 0x22, 0x33, 0x44};
  srslte::byte_buffer_t                               pdu = make_pdu_and_log(tv);

  // unpack PDU
  rlc_um_nr_pdu_header_t header = {};
  TESTASSERT(rlc_um_nr_read_data_pdu_header(&pdu, srslte::rlc_um_nr_sn_size_t::size6bits, &header) != 0);

  TESTASSERT(header.si == rlc_nr_si_field_t::neither_first_nor_last_segment);
  TESTASSERT(header.so == 258);
  TESTASSERT(header.sn == 3);
  TESTASSERT(header.sn_size == srslte::rlc_um_nr_sn_size_t::size6bits);

  // clear header
  corrupt_pdu_header(pdu, header_len, payload_len);

  // pack again
  TESTASSERT(rlc_um_nr_write_data_pdu_header(header, &pdu) == header_len);
  TESTASSERT(pdu.N_bytes == tv.size());
  TESTASSERT(memcmp(pdu.msg, tv.data(), pdu.N_bytes) == 0);

  return SRSLTE_SUCCESS;
}

// RLC UM PDU with 6 Bit SN carrying a first segment of an SDU
int rlc_um_nr_pdu_test4()
{
  // SN = 31
  const int                                     header_len = 1, payload_len = 4;
  std::array<uint8_t, header_len + payload_len> tv  = {0x5f, 0x11, 0x22, 0x33, 0x44};
  srslte::byte_buffer_t                         pdu = make_pdu_and_log(tv);

  // unpack PDU
  rlc_um_nr_pdu_header_t header = {};
  TESTASSERT(rlc_um_nr_read_data_pdu_header(&pdu, srslte::rlc_um_nr_sn_size_t::size6bits, &header) != 0);

  TESTASSERT(header.si == rlc_nr_si_field_t::first_segment);
  TESTASSERT(header.so == 0);
  TESTASSERT(header.sn == 31);
  TESTASSERT(header.sn_size == srslte::rlc_um_nr_sn_size_t::size6bits);

  // clear header
  corrupt_pdu_header(pdu, header_len, payload_len);

  // pack again
  TESTASSERT(rlc_um_nr_write_data_pdu_header(header, &pdu) == header_len);
  TESTASSERT(pdu.N_bytes == tv.size());
  TESTASSERT(memcmp(pdu.msg, tv.data(), pdu.N_bytes) == 0);

  return SRSLTE_SUCCESS;
}

// This should fail unpacking because the PDU has reserved bits set
int rlc_um_nr_pdu_unpack_test5()
{
  std::array<uint8_t, 7> tv  = {0x33, 0x01, 0x02, 0x11, 0x22, 0x33, 0x44};
  srslte::byte_buffer_t  pdu = make_pdu_and_log(tv);

  // unpack PDU
  rlc_um_nr_pdu_header_t header = {};
  TESTASSERT(rlc_um_nr_read_data_pdu_header(&pdu, srslte::rlc_um_nr_sn_size_t::size6bits, &header) == 0);

  return SRSLTE_SUCCESS;
}

int main(int argc, char** argv)
{
#if PCAP
  pcap_handle = std::unique_ptr<srslte::mac_nr_pcap>(new srslte::mac_nr_pcap());
  pcap_handle->open("rlc_um_nr_pdu_test.pcap");
#endif

  if (rlc_um_nr_pdu_test1()) {
    fprintf(stderr, "rlc_um_nr_pdu_test1() failed.\n");
    return SRSLTE_ERROR;
  }

  if (rlc_um_nr_pdu_test2()) {
    fprintf(stderr, "rlc_um_nr_pdu_test2() failed.\n");
    return SRSLTE_ERROR;
  }

  if (rlc_um_nr_pdu_test3()) {
    fprintf(stderr, "rlc_um_nr_pdu_test3() failed.\n");
    return SRSLTE_ERROR;
  }

  if (rlc_um_nr_pdu_test4()) {
    fprintf(stderr, "rlc_um_nr_pdu_test4() failed.\n");
    return SRSLTE_ERROR;
  }

  if (rlc_um_nr_pdu_unpack_test5()) {
    fprintf(stderr, "rlc_um_nr_pdu_unpack_test5() failed.\n");
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}
