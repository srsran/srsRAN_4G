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

#include "srslte/common/log_filter.h"
#include "srslte/common/mac_nr_pcap.h"
#include "srslte/config.h"
#include "srslte/mac/mac_nr_pdu.h"

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

static std::unique_ptr<srslte::mac_nr_pcap> pcap_handle = nullptr;

int mac_dl_sch_pdu_unpack_and_pack_test1()
{
  // MAC PDU with DL-SCH subheader with 8-bit length field
  // Bit 1-8
  // |   |   |   |   |   |   |   |   |
  // | R |F=0|         LCID          |  Octet 1
  // |               L               |  Octet 1

  // TV1 - MAC PDU with short subheader for CCCH, MAC SDU length is 8 B, total PDU is 10 B
  uint8_t mac_dl_sch_pdu_1[] = {0x00, 0x08, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};

  if (pcap_handle) {
    pcap_handle->write_dl_crnti(mac_dl_sch_pdu_1, sizeof(mac_dl_sch_pdu_1), PCAP_CRNTI, true, PCAP_TTI);
  }

  srslte::mac_nr_sch_pdu pdu;
  pdu.unpack(mac_dl_sch_pdu_1, sizeof(mac_dl_sch_pdu_1));
  TESTASSERT(pdu.get_num_subpdus() == 1);

  mac_nr_sch_subpdu subpdu = pdu.get_subpdu(0);
  TESTASSERT(subpdu.get_total_length() == 10);
  TESTASSERT(subpdu.get_sdu_length() == 8);
  TESTASSERT(subpdu.get_lcid() == 0);

  // pack PDU again
  byte_buffer_t tx_buffer;

  srslte::mac_nr_sch_pdu tx_pdu;
  tx_pdu.init_tx(&tx_buffer, sizeof(mac_dl_sch_pdu_1));

  // Add SDU part of TV from above
  tx_pdu.add_sdu(0, &mac_dl_sch_pdu_1[2], 8);

  TESTASSERT(tx_pdu.get_remaing_len() == 0);
  TESTASSERT(tx_buffer.N_bytes == sizeof(mac_dl_sch_pdu_1));
  TESTASSERT(memcmp(tx_buffer.msg, mac_dl_sch_pdu_1, tx_buffer.N_bytes) == 0);

  if (pcap_handle) {
    pcap_handle->write_dl_crnti(tx_buffer.msg, tx_buffer.N_bytes, PCAP_CRNTI, true, PCAP_TTI);
  }

  srslte::log_filter log("MAC");
  log.set_level(srslte::LOG_LEVEL_DEBUG);
  log.set_hex_limit(100000);
  log.info_hex(tx_buffer.msg, tx_buffer.N_bytes, "Generated MAC PDU (%d B)\n", tx_buffer.N_bytes);

  return SRSLTE_SUCCESS;
}

int mac_dl_sch_pdu_unpack_test2()
{
  // MAC PDU with DL-SCH subheader with 16-bit length field
  // Bit 1-8
  // |   |   |   |   |   |   |   |   |
  // | R |F=1|         LCID          |  Octet 1
  // |               L               |  Octet 2
  // |               L               |  Octet 3

  // Note: Pack test is not working in this case as it would generate a PDU with the short length field only

  // TV2 - MAC PDU with long subheader for LCID=2, MAC SDU length is 8 B, total PDU is 11 B
  uint8_t mac_dl_sch_pdu_2[] = {0x42, 0x00, 0x08, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};

  if (pcap_handle) {
    pcap_handle->write_dl_crnti(mac_dl_sch_pdu_2, sizeof(mac_dl_sch_pdu_2), PCAP_CRNTI, true, PCAP_TTI);
  }

  srslte::mac_nr_sch_pdu pdu;
  pdu.unpack(mac_dl_sch_pdu_2, sizeof(mac_dl_sch_pdu_2));
  TESTASSERT(pdu.get_num_subpdus() == 1);
  mac_nr_sch_subpdu subpdu = pdu.get_subpdu(0);
  TESTASSERT(subpdu.get_total_length() == 11);
  TESTASSERT(subpdu.get_sdu_length() == 8);
  TESTASSERT(subpdu.get_lcid() == 2);

  return SRSLTE_SUCCESS;
}

int mac_dl_sch_pdu_pack_test3()
{
  // MAC PDU with DL-SCH subheader with 16-bit length field (512 B SDU + 3 B header)
  // Bit 1-8
  // |   |   |   |   |   |   |   |   |
  // | R |F=1|         LCID          |  Octet 1
  // |               L               |  Octet 2
  // |               L               |  Octet 3

  uint8_t sdu[512] = {};

  // populate SDU payload
  for (uint32_t i = 0; i < 512; i++) {
    sdu[i] = i % 256;
  }

  // pack buffer
  byte_buffer_t tx_buffer;

  srslte::mac_nr_sch_pdu tx_pdu;
  tx_pdu.init_tx(&tx_buffer, 1024);

  // Add SDU
  tx_pdu.add_sdu(4, sdu, sizeof(sdu));

  TESTASSERT(tx_pdu.get_remaing_len() == 509);
  TESTASSERT(tx_buffer.N_bytes == 515);

  if (pcap_handle) {
    pcap_handle->write_dl_crnti(tx_buffer.msg, tx_buffer.N_bytes, PCAP_CRNTI, true, PCAP_TTI);
  }

  srslte::log_filter log("MAC");
  log.set_level(srslte::LOG_LEVEL_DEBUG);
  log.set_hex_limit(100000);
  log.info_hex(tx_buffer.msg, tx_buffer.N_bytes, "Generated MAC PDU (%d B)\n", tx_buffer.N_bytes);

  return SRSLTE_SUCCESS;
}

int mac_dl_sch_pdu_pack_test4()
{
  // MAC PDU with only padding
  // Bit 1-8
  // |   |   |   |   |   |   |   |   |
  // | R | R |         LCID          |  Octet 1
  // |              zeros            |  Octet ..

  // TV2 - MAC PDU with a single LCID with padding only
  uint8_t tv[] = {0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  const uint32_t pdu_size = 8;
  byte_buffer_t  tx_buffer;

  // modify buffer (to be nulled during PDU packing
  tx_buffer.msg[4] = 0xaa;

  srslte::mac_nr_sch_pdu tx_pdu;
  tx_pdu.init_tx(&tx_buffer, pdu_size);

  TESTASSERT(tx_pdu.get_remaing_len() == pdu_size);
  tx_pdu.pack();
  TESTASSERT(tx_buffer.N_bytes == pdu_size);
  TESTASSERT(tx_buffer.N_bytes == sizeof(tv));

  TESTASSERT(memcmp(tx_buffer.msg, tv, tx_buffer.N_bytes) == 0);

  if (pcap_handle) {
    pcap_handle->write_dl_crnti(tx_buffer.msg, tx_buffer.N_bytes, PCAP_CRNTI, true, PCAP_TTI);
  }

  srslte::log_filter log("MAC");
  log.set_level(srslte::LOG_LEVEL_DEBUG);
  log.set_hex_limit(100000);
  log.info_hex(tx_buffer.msg, tx_buffer.N_bytes, "Generated MAC PDU (%d B)\n", tx_buffer.N_bytes);

  return SRSLTE_SUCCESS;
}

int mac_dl_sch_pdu_pack_test5()
{
  // MAC PDU with DL-SCH subheader with 8-bit length field and padding after that
  // Bit 1-8
  // |   |   |   |   |   |   |   |   |
  // | R |F=0|         LCID          |  Octet 1
  // |               L               |  Octet 2
  // |              zeros            |  Octet 3-10
  // | R | R |         LCID          |  Octet 11
  // |              zeros            |  Octet ..

  // TV2 - MAC PDU with a single LCID with padding only
  uint8_t tv[] = {0x01, 0x08, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00};

  const uint32_t pdu_size = 16;
  byte_buffer_t  tx_buffer;
  tx_buffer.clear();

  srslte::mac_nr_sch_pdu tx_pdu;
  tx_pdu.init_tx(&tx_buffer, pdu_size);

  // Add SDU part of TV from above
  tx_pdu.add_sdu(1, &tv[2], 8);

  TESTASSERT(tx_pdu.get_remaing_len() == 6);

  tx_pdu.pack();

  TESTASSERT(tx_pdu.get_remaing_len() == 0);
  TESTASSERT(tx_buffer.N_bytes == pdu_size);
  TESTASSERT(tx_buffer.N_bytes == sizeof(tv));
  TESTASSERT(memcmp(tx_buffer.msg, tv, tx_buffer.N_bytes) == 0);

  if (pcap_handle) {
    pcap_handle->write_dl_crnti(tx_buffer.msg, tx_buffer.N_bytes, PCAP_CRNTI, true, PCAP_TTI);
  }

  srslte::log_filter log("MAC");
  log.set_level(srslte::LOG_LEVEL_DEBUG);
  log.set_hex_limit(100000);
  log.info_hex(tx_buffer.msg, tx_buffer.N_bytes, "Generated MAC PDU (%d B)\n", tx_buffer.N_bytes);

  return SRSLTE_SUCCESS;
}

int mac_dl_sch_pdu_unpack_test6()
{
  // MAC PDU with DL-SCH subheader reserved LCID
  // Bit 1-8
  // |   |   |   |   |   |   |   |   |
  // | R |F=1|         LCID          |  Octet 1
  // |               L               |  Octet 2

  // TV2 - MAC PDU with reserved LCID (46=0x2e)
  uint8_t mac_dl_sch_pdu_2[] = {0x2e, 0x04, 0x11, 0x22, 0x33, 0x44};

  if (pcap_handle) {
    pcap_handle->write_dl_crnti(mac_dl_sch_pdu_2, sizeof(mac_dl_sch_pdu_2), PCAP_CRNTI, true, PCAP_TTI);
  }

  srslte::mac_nr_sch_pdu pdu;
  pdu.unpack(mac_dl_sch_pdu_2, sizeof(mac_dl_sch_pdu_2));
  TESTASSERT(pdu.get_num_subpdus() == 0);

  return SRSLTE_SUCCESS;
}

int mac_ul_sch_pdu_unpack_test1()
{
  // UL-SCH MAC PDU with fixed-size CE and DL-SCH subheader with 16-bit length field
  // Note: this is a malformed UL-SCH PDU has SDU is placed after the CE
  // Bit 1-8
  // |   |   |   |   |   |   |   |   |
  // | R | R |         LCID          |  Octet 1 (C-RNTI LCID = 0x )
  // |            C-RNTI             |  Octet 2
  // |            C-RNTI             |  Octet 3
  // | R |F=1|         LCID          |  Octet 4
  // |               L               |  Octet 5
  // |               L               |  Octet 6
  // |             data              |  Octet 7-x

  // TV3 - UL-SCH MAC PDU with long subheader for LCID=2, MAC SDU length is 4 B, total PDU is 9 B
  const uint8_t ul_sch_crnti[]     = {0x11, 0x22};
  uint8_t       mac_ul_sch_pdu_1[] = {0x3a, 0x11, 0x22, 0x43, 0x00, 0x04, 0x11, 0x22, 0x33, 0x44};

  if (pcap_handle) {
    pcap_handle->write_ul_crnti(mac_ul_sch_pdu_1, sizeof(mac_ul_sch_pdu_1), PCAP_CRNTI, true, PCAP_TTI);
  }

  srslte::mac_nr_sch_pdu pdu(true);
  pdu.unpack(mac_ul_sch_pdu_1, sizeof(mac_ul_sch_pdu_1));
  TESTASSERT(pdu.get_num_subpdus() == 2);

  // First subpdu is C-RNTI CE
  mac_nr_sch_subpdu subpdu0 = pdu.get_subpdu(0);
  TESTASSERT(subpdu0.get_total_length() == 3);
  TESTASSERT(subpdu0.get_sdu_length() == 2);
  TESTASSERT(subpdu0.get_lcid() == mac_nr_sch_subpdu::CRNTI);
  TESTASSERT(memcmp(subpdu0.get_sdu(), (uint8_t*)&ul_sch_crnti, sizeof(ul_sch_crnti)) == 0);

  // Second subpdu is UL-SCH
  mac_nr_sch_subpdu subpdu1 = pdu.get_subpdu(1);
  TESTASSERT(subpdu1.get_total_length() == 7);
  TESTASSERT(subpdu1.get_sdu_length() == 4);
  TESTASSERT(subpdu1.get_lcid() == 3);

  return SRSLTE_SUCCESS;
}

int mac_ul_sch_pdu_unpack_and_pack_test2()
{
  // MAC PDU with UL-SCH (for UL-CCCH) subheader
  // Bit 1-8
  // |   |   |   |   |   |   |   |   |
  // | R | R |         LCID          |  Octet 1

  // TV1 - MAC PDU with short subheader for CCCH, MAC SDU length is 8 B, total PDU is 10 B
  uint8_t mac_ul_sch_pdu_1[] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};

  if (pcap_handle) {
    pcap_handle->write_ul_crnti(mac_ul_sch_pdu_1, sizeof(mac_ul_sch_pdu_1), PCAP_CRNTI, true, PCAP_TTI);
  }

  srslte::mac_nr_sch_pdu pdu(true);
  pdu.unpack(mac_ul_sch_pdu_1, sizeof(mac_ul_sch_pdu_1));
  TESTASSERT(pdu.get_num_subpdus() == 1);

  mac_nr_sch_subpdu subpdu = pdu.get_subpdu(0);
  TESTASSERT(subpdu.get_total_length() == 9);
  TESTASSERT(subpdu.get_sdu_length() == 8);
  TESTASSERT(subpdu.get_lcid() == 0);

  // pack PDU again
  byte_buffer_t tx_buffer;

  srslte::mac_nr_sch_pdu tx_pdu;
  tx_pdu.init_tx(&tx_buffer, sizeof(mac_ul_sch_pdu_1), true);

  // Add SDU part of TV from above
  tx_pdu.add_sdu(0, &mac_ul_sch_pdu_1[1], 8);

  TESTASSERT(tx_pdu.get_remaing_len() == 0);
  TESTASSERT(tx_buffer.N_bytes == sizeof(mac_ul_sch_pdu_1));
  TESTASSERT(memcmp(tx_buffer.msg, mac_ul_sch_pdu_1, tx_buffer.N_bytes) == 0);

  if (pcap_handle) {
    pcap_handle->write_ul_crnti(tx_buffer.msg, tx_buffer.N_bytes, PCAP_CRNTI, true, PCAP_TTI);
  }

  srslte::log_filter log("MAC");
  log.set_level(srslte::LOG_LEVEL_DEBUG);
  log.set_hex_limit(100000);
  log.info_hex(tx_buffer.msg, tx_buffer.N_bytes, "Generated MAC PDU (%d B)\n", tx_buffer.N_bytes);

  return SRSLTE_SUCCESS;
}

int mac_ul_sch_pdu_unpack_and_pack_test3()
{
  // MAC PDU with UL-SCH (with normal LCID) subheader for short SDU
  // Bit 1-8
  // |   |   |   |   |   |   |   |   |
  // | R |F=0|         LCID          |  Octet 1
  // |                L              |  Octet 2

  // TV1 - MAC PDU with short subheader for CCCH, MAC SDU length is 8 B, total PDU is 10 B
  uint8_t mac_ul_sch_pdu_1[] = {0x02, 0x0a, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa};

  if (pcap_handle) {
    pcap_handle->write_ul_crnti(mac_ul_sch_pdu_1, sizeof(mac_ul_sch_pdu_1), PCAP_CRNTI, true, PCAP_TTI);
  }

  srslte::mac_nr_sch_pdu pdu(true);
  pdu.unpack(mac_ul_sch_pdu_1, sizeof(mac_ul_sch_pdu_1));
  TESTASSERT(pdu.get_num_subpdus() == 1);

  mac_nr_sch_subpdu subpdu = pdu.get_subpdu(0);
  TESTASSERT(subpdu.get_total_length() == 12);
  TESTASSERT(subpdu.get_sdu_length() == 10);
  TESTASSERT(subpdu.get_lcid() == 2);

  // pack PDU again
  byte_buffer_t tx_buffer;

  srslte::mac_nr_sch_pdu tx_pdu;
  tx_pdu.init_tx(&tx_buffer, sizeof(mac_ul_sch_pdu_1), true);

  // Add SDU part of TV from above
  tx_pdu.add_sdu(2, &mac_ul_sch_pdu_1[2], 10);

  TESTASSERT(tx_pdu.get_remaing_len() == 0);
  TESTASSERT(tx_buffer.N_bytes == sizeof(mac_ul_sch_pdu_1));
  TESTASSERT(memcmp(tx_buffer.msg, mac_ul_sch_pdu_1, tx_buffer.N_bytes) == 0);

  if (pcap_handle) {
    pcap_handle->write_ul_crnti(tx_buffer.msg, tx_buffer.N_bytes, PCAP_CRNTI, true, PCAP_TTI);
  }

  srslte::log_filter log("MAC");
  log.set_level(srslte::LOG_LEVEL_DEBUG);
  log.set_hex_limit(100000);
  log.info_hex(tx_buffer.msg, tx_buffer.N_bytes, "Generated MAC PDU (%d B)\n", tx_buffer.N_bytes);

  return SRSLTE_SUCCESS;
}

int mac_ul_sch_pdu_pack_test4()
{
  // MAC PDU with UL-SCH (with normal LCID) subheader for long SDU
  // Bit 1-8
  // |   |   |   |   |   |   |   |   |
  // | R |F=1|         LCID          |  Octet 1
  // |                L              |  Octet 2
  // |                L              |  Octet 3

  uint8_t sdu[512] = {};

  // populate SDU payload
  for (uint32_t i = 0; i < 512; i++) {
    sdu[i] = i % 256;
  }

  // pack PDU again
  byte_buffer_t tx_buffer;

  srslte::mac_nr_sch_pdu tx_pdu;
  tx_pdu.init_tx(&tx_buffer, sizeof(sdu) + 3, true);

  // Add SDU part of TV from above
  tx_pdu.add_sdu(2, sdu, sizeof(sdu));

  TESTASSERT(tx_pdu.get_remaing_len() == 0);
  TESTASSERT(tx_buffer.N_bytes == sizeof(sdu) + 3);

  if (pcap_handle) {
    pcap_handle->write_ul_crnti(tx_buffer.msg, tx_buffer.N_bytes, PCAP_CRNTI, true, PCAP_TTI);
  }

  srslte::log_filter log("MAC");
  log.set_level(srslte::LOG_LEVEL_DEBUG);
  log.set_hex_limit(100000);
  log.info_hex(tx_buffer.msg, tx_buffer.N_bytes, "Generated MAC PDU (%d B)\n", tx_buffer.N_bytes);

  return SRSLTE_SUCCESS;
}

int mac_ul_sch_pdu_unpack_test5()
{
  // MAC PDU with UL-SCH (with normal LCID) subheader for short SDU but reserved LCID
  // Bit 1-8
  // |   |   |   |   |   |   |   |   |
  // | R |F=0|         LCID          |  Octet 1
  // |                L              |  Octet 2

  // TV1 - MAC PDU with short subheader but reserved LCID for UL-SCH (LCID=33)
  uint8_t mac_ul_sch_pdu_1[] = {0x21, 0x0a, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa};

  if (pcap_handle) {
    pcap_handle->write_ul_crnti(mac_ul_sch_pdu_1, sizeof(mac_ul_sch_pdu_1), PCAP_CRNTI, true, PCAP_TTI);
  }

  srslte::mac_nr_sch_pdu pdu(true);
  pdu.unpack(mac_ul_sch_pdu_1, sizeof(mac_ul_sch_pdu_1));
  TESTASSERT(pdu.get_num_subpdus() == 0);

  return SRSLTE_SUCCESS;
}

int main(int argc, char** argv)
{
#if PCAP
  pcap_handle = std::unique_ptr<srslte::mac_nr_pcap>(new srslte::mac_nr_pcap());
  pcap_handle->open("mac_nr_pdu_test.pcap");
#endif

  if (mac_dl_sch_pdu_unpack_and_pack_test1()) {
    fprintf(stderr, "mac_dl_sch_pdu_unpack_and_pack_test1() failed.\n");
    return SRSLTE_ERROR;
  }

  if (mac_dl_sch_pdu_unpack_test2()) {
    fprintf(stderr, "mac_dl_sch_pdu_unpack_test2() failed.\n");
    return SRSLTE_ERROR;
  }

  if (mac_dl_sch_pdu_pack_test3()) {
    fprintf(stderr, "mac_dl_sch_pdu_pack_test3() failed.\n");
    return SRSLTE_ERROR;
  }

  if (mac_dl_sch_pdu_pack_test4()) {
    fprintf(stderr, "mac_dl_sch_pdu_pack_test4() failed.\n");
    return SRSLTE_ERROR;
  }

  if (mac_dl_sch_pdu_pack_test5()) {
    fprintf(stderr, "mac_dl_sch_pdu_pack_test5() failed.\n");
    return SRSLTE_ERROR;
  }

  if (mac_dl_sch_pdu_unpack_test6()) {
    fprintf(stderr, "mac_dl_sch_pdu_unpack_test6() failed.\n");
    return SRSLTE_ERROR;
  }

  if (mac_ul_sch_pdu_unpack_test1()) {
    fprintf(stderr, "mac_ul_sch_pdu_unpack_test1() failed.\n");
    return SRSLTE_ERROR;
  }

  if (mac_ul_sch_pdu_unpack_and_pack_test2()) {
    fprintf(stderr, "mac_ul_sch_pdu_unpack_and_pack_test2() failed.\n");
    return SRSLTE_ERROR;
  }

  if (mac_ul_sch_pdu_unpack_and_pack_test3()) {
    fprintf(stderr, "mac_ul_sch_pdu_unpack_and_pack_test3() failed.\n");
    return SRSLTE_ERROR;
  }

  if (mac_ul_sch_pdu_pack_test4()) {
    fprintf(stderr, "mac_ul_sch_pdu_pack_test4() failed.\n");
    return SRSLTE_ERROR;
  }

  if (mac_ul_sch_pdu_unpack_test5()) {
    fprintf(stderr, "mac_ul_sch_pdu_unpack_test5() failed.\n");
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}
