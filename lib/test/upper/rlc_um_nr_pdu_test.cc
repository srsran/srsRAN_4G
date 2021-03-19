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

#include "srsran/config.h"
#include "srsran/upper/rlc.h"
#include "srsran/upper/rlc_um_nr.h"

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

#if PCAP
  pcap_handle->close();
#endif

  return SRSRAN_SUCCESS;
}
