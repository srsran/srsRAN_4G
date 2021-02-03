/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srslte/common/log_filter.h"
#include "srslte/common/mac_pcap.h"
#include "srslte/common/test_common.h"
#include "srslte/config.h"
#include "srslte/mac/mac_rar_pdu_nr.h"
#include "srslte/mac/mac_sch_pdu_nr.h"

#include <array>
#include <iostream>
#include <memory>
#include <vector>

#define PCAP 0
#define PCAP_CRNTI (0x1001)
#define PCAP_TTI (666)

using namespace srslte;

static std::unique_ptr<srslte::mac_pcap> pcap_handle = nullptr;

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
    pcap_handle->write_dl_crnti_nr(mac_dl_sch_pdu_1, sizeof(mac_dl_sch_pdu_1), PCAP_CRNTI, true, PCAP_TTI);
  }

  srslte::mac_sch_pdu_nr pdu;
  pdu.unpack(mac_dl_sch_pdu_1, sizeof(mac_dl_sch_pdu_1));
  TESTASSERT(pdu.get_num_subpdus() == 1);

  mac_sch_subpdu_nr subpdu = pdu.get_subpdu(0);
  TESTASSERT(subpdu.get_total_length() == 10);
  TESTASSERT(subpdu.get_sdu_length() == 8);
  TESTASSERT(subpdu.get_lcid() == 0);

  // pack PDU again
  byte_buffer_t tx_buffer;

  srslte::mac_sch_pdu_nr tx_pdu;
  tx_pdu.init_tx(&tx_buffer, sizeof(mac_dl_sch_pdu_1));

  // Add SDU part of TV from above
  tx_pdu.add_sdu(0, &mac_dl_sch_pdu_1[2], 8);

  TESTASSERT(tx_pdu.get_remaing_len() == 0);
  TESTASSERT(tx_buffer.N_bytes == sizeof(mac_dl_sch_pdu_1));
  TESTASSERT(memcmp(tx_buffer.msg, mac_dl_sch_pdu_1, tx_buffer.N_bytes) == 0);

  if (pcap_handle) {
    pcap_handle->write_dl_crnti_nr(tx_buffer.msg, tx_buffer.N_bytes, PCAP_CRNTI, true, PCAP_TTI);
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
    pcap_handle->write_dl_crnti_nr(mac_dl_sch_pdu_2, sizeof(mac_dl_sch_pdu_2), PCAP_CRNTI, true, PCAP_TTI);
  }

  srslte::mac_sch_pdu_nr pdu;
  pdu.unpack(mac_dl_sch_pdu_2, sizeof(mac_dl_sch_pdu_2));
  TESTASSERT(pdu.get_num_subpdus() == 1);
  mac_sch_subpdu_nr subpdu = pdu.get_subpdu(0);
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

  srslte::mac_sch_pdu_nr tx_pdu;
  tx_pdu.init_tx(&tx_buffer, 1024);

  // Add SDU
  tx_pdu.add_sdu(4, sdu, sizeof(sdu));

  TESTASSERT(tx_pdu.get_remaing_len() == 509);
  TESTASSERT(tx_buffer.N_bytes == 515);

  if (pcap_handle) {
    pcap_handle->write_dl_crnti_nr(tx_buffer.msg, tx_buffer.N_bytes, PCAP_CRNTI, true, PCAP_TTI);
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

  srslte::mac_sch_pdu_nr tx_pdu;
  tx_pdu.init_tx(&tx_buffer, pdu_size);

  TESTASSERT(tx_pdu.get_remaing_len() == pdu_size);
  tx_pdu.pack();
  TESTASSERT(tx_buffer.N_bytes == pdu_size);
  TESTASSERT(tx_buffer.N_bytes == sizeof(tv));

  TESTASSERT(memcmp(tx_buffer.msg, tv, tx_buffer.N_bytes) == 0);

  if (pcap_handle) {
    pcap_handle->write_dl_crnti_nr(tx_buffer.msg, tx_buffer.N_bytes, PCAP_CRNTI, true, PCAP_TTI);
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

  srslte::mac_sch_pdu_nr tx_pdu;
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
    pcap_handle->write_dl_crnti_nr(tx_buffer.msg, tx_buffer.N_bytes, PCAP_CRNTI, true, PCAP_TTI);
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
    pcap_handle->write_dl_crnti_nr(mac_dl_sch_pdu_2, sizeof(mac_dl_sch_pdu_2), PCAP_CRNTI, true, PCAP_TTI);
  }

  srslte::mac_sch_pdu_nr pdu;
  pdu.unpack(mac_dl_sch_pdu_2, sizeof(mac_dl_sch_pdu_2));
  TESTASSERT(pdu.get_num_subpdus() == 0);

  return SRSLTE_SUCCESS;
}

int mac_rar_pdu_unpack_test7()
{
  // MAC PDU with RAR PDU with single RAPID=0
  // rapid=0
  // ta=180
  // ul_grant:
  //   hopping_flag=0
  //   riv=0x1
  //   time_domain_rsc=1
  //   mcs=4
  //   tpc_command=3
  //   csi_request=0
  // tc-rnti=0x4616

  // Bit 1-8
  // |   |   |   |   |   |   |   |   |
  // | R |T=1|        RAPID=0        |  Octet 1
  // |              RAR              |  Octet 2-8
  const uint32_t tv_rapid                                       = 0;
  const uint32_t tv_ta                                          = 180;
  const uint16_t tv_tcrnti                                      = 0x4616;
  const uint8_t  tv_msg3_grant[mac_rar_subpdu_nr::UL_GRANT_NBITS] = {
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00}; // unpacked UL grant

  uint8_t mac_dl_rar_pdu[] = {0x40, 0x05, 0xa0, 0x00, 0x11, 0x46, 0x46, 0x16, 0x00, 0x00, 0x00};

  if (pcap_handle) {
    pcap_handle->write_dl_ra_rnti_nr(mac_dl_rar_pdu, sizeof(mac_dl_rar_pdu), 0x0016, true, PCAP_TTI);
  }

  srslte::mac_rar_pdu_nr pdu;
  TESTASSERT(pdu.unpack(mac_dl_rar_pdu, sizeof(mac_dl_rar_pdu)) == true);

  std::cout << pdu.to_string() << std::endl;

  TESTASSERT(pdu.get_num_subpdus() == 1);

  mac_rar_subpdu_nr subpdu = pdu.get_subpdu(0);
  TESTASSERT(subpdu.has_rapid() == true);
  TESTASSERT(subpdu.has_backoff() == false);
  TESTASSERT(subpdu.get_temp_crnti() == tv_tcrnti);
  TESTASSERT(subpdu.get_ta() == tv_ta);
  TESTASSERT(subpdu.get_rapid() == tv_rapid);

  std::array<uint8_t, mac_rar_subpdu_nr::UL_GRANT_NBITS> msg3_grant;
  subpdu.get_ul_grant(msg3_grant);
  TESTASSERT(memcmp(msg3_grant.data(), tv_msg3_grant, msg3_grant.size()) == 0);

  return SRSLTE_SUCCESS;
}

int mac_rar_pdu_unpack_test8()
{
  // Malformed MAC PDU, says it has RAR PDU but is too short to include MAC RAR

  // Bit 1-8
  // |   |   |   |   |   |   |   |   |
  // | E |T=1|        RAPID=0        |  Octet 1
  // |            RAR_fragment       |  Octet 2
  uint8_t mac_dl_rar_pdu[] = {0x40, 0x05};

  if (pcap_handle) {
    pcap_handle->write_dl_ra_rnti_nr(mac_dl_rar_pdu, sizeof(mac_dl_rar_pdu), 0x0016, true, PCAP_TTI);
  }

  // unpacking should fail
  srslte::mac_rar_pdu_nr pdu;
  TESTASSERT(pdu.unpack(mac_dl_rar_pdu, sizeof(mac_dl_rar_pdu)) == false);
  TESTASSERT(pdu.get_num_subpdus() == 0);

  // Malformed PDU with reserved bits set
  // Bit 1-8
  // |   |   |   |   |   |   |   |   |
  // | E |T=0| R | R |      BI       |  Octet 1
  uint8_t mac_dl_rar_pdu2[] = {0x10};
  TESTASSERT(pdu.unpack(mac_dl_rar_pdu2, sizeof(mac_dl_rar_pdu2)) == false);
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
    pcap_handle->write_ul_crnti_nr(mac_ul_sch_pdu_1, sizeof(mac_ul_sch_pdu_1), PCAP_CRNTI, true, PCAP_TTI);
  }

  srslte::mac_sch_pdu_nr pdu(true);
  pdu.unpack(mac_ul_sch_pdu_1, sizeof(mac_ul_sch_pdu_1));
  TESTASSERT(pdu.get_num_subpdus() == 2);

  // First subpdu is C-RNTI CE
  mac_sch_subpdu_nr subpdu0 = pdu.get_subpdu(0);
  TESTASSERT(subpdu0.get_total_length() == 3);
  TESTASSERT(subpdu0.get_sdu_length() == 2);
  TESTASSERT(subpdu0.get_lcid() == mac_sch_subpdu_nr::CRNTI);
  TESTASSERT(memcmp(subpdu0.get_sdu(), (uint8_t*)&ul_sch_crnti, sizeof(ul_sch_crnti)) == 0);

  // Second subpdu is UL-SCH
  mac_sch_subpdu_nr subpdu1 = pdu.get_subpdu(1);
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
    pcap_handle->write_ul_crnti_nr(mac_ul_sch_pdu_1, sizeof(mac_ul_sch_pdu_1), PCAP_CRNTI, true, PCAP_TTI);
  }

  srslte::mac_sch_pdu_nr pdu(true);
  pdu.unpack(mac_ul_sch_pdu_1, sizeof(mac_ul_sch_pdu_1));
  TESTASSERT(pdu.get_num_subpdus() == 1);

  mac_sch_subpdu_nr subpdu = pdu.get_subpdu(0);
  TESTASSERT(subpdu.get_total_length() == 9);
  TESTASSERT(subpdu.get_sdu_length() == 8);
  TESTASSERT(subpdu.get_lcid() == 0);

  // pack PDU again
  byte_buffer_t tx_buffer;

  srslte::mac_sch_pdu_nr tx_pdu;
  tx_pdu.init_tx(&tx_buffer, sizeof(mac_ul_sch_pdu_1), true);

  // Add SDU part of TV from above
  tx_pdu.add_sdu(0, &mac_ul_sch_pdu_1[1], 8);

  TESTASSERT(tx_pdu.get_remaing_len() == 0);
  TESTASSERT(tx_buffer.N_bytes == sizeof(mac_ul_sch_pdu_1));
  TESTASSERT(memcmp(tx_buffer.msg, mac_ul_sch_pdu_1, tx_buffer.N_bytes) == 0);

  if (pcap_handle) {
    pcap_handle->write_ul_crnti_nr(tx_buffer.msg, tx_buffer.N_bytes, PCAP_CRNTI, true, PCAP_TTI);
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
    pcap_handle->write_ul_crnti_nr(mac_ul_sch_pdu_1, sizeof(mac_ul_sch_pdu_1), PCAP_CRNTI, true, PCAP_TTI);
  }

  srslte::mac_sch_pdu_nr pdu(true);
  pdu.unpack(mac_ul_sch_pdu_1, sizeof(mac_ul_sch_pdu_1));
  TESTASSERT(pdu.get_num_subpdus() == 1);

  mac_sch_subpdu_nr subpdu = pdu.get_subpdu(0);
  TESTASSERT(subpdu.get_total_length() == 12);
  TESTASSERT(subpdu.get_sdu_length() == 10);
  TESTASSERT(subpdu.get_lcid() == 2);

  // pack PDU again
  byte_buffer_t tx_buffer;

  srslte::mac_sch_pdu_nr tx_pdu;
  tx_pdu.init_tx(&tx_buffer, sizeof(mac_ul_sch_pdu_1), true);

  // Add SDU part of TV from above
  tx_pdu.add_sdu(2, &mac_ul_sch_pdu_1[2], 10);

  TESTASSERT(tx_pdu.get_remaing_len() == 0);
  TESTASSERT(tx_buffer.N_bytes == sizeof(mac_ul_sch_pdu_1));
  TESTASSERT(memcmp(tx_buffer.msg, mac_ul_sch_pdu_1, tx_buffer.N_bytes) == 0);

  if (pcap_handle) {
    pcap_handle->write_ul_crnti_nr(tx_buffer.msg, tx_buffer.N_bytes, PCAP_CRNTI, true, PCAP_TTI);
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

  srslte::mac_sch_pdu_nr tx_pdu;
  tx_pdu.init_tx(&tx_buffer, sizeof(sdu) + 3, true);

  // Add SDU part of TV from above
  tx_pdu.add_sdu(2, sdu, sizeof(sdu));

  TESTASSERT(tx_pdu.get_remaing_len() == 0);
  TESTASSERT(tx_buffer.N_bytes == sizeof(sdu) + 3);

  if (pcap_handle) {
    pcap_handle->write_ul_crnti_nr(tx_buffer.msg, tx_buffer.N_bytes, PCAP_CRNTI, true, PCAP_TTI);
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
    pcap_handle->write_ul_crnti_nr(mac_ul_sch_pdu_1, sizeof(mac_ul_sch_pdu_1), PCAP_CRNTI, true, PCAP_TTI);
  }

  srslte::mac_sch_pdu_nr pdu(true);
  pdu.unpack(mac_ul_sch_pdu_1, sizeof(mac_ul_sch_pdu_1));
  TESTASSERT(pdu.get_num_subpdus() == 0);

  return SRSLTE_SUCCESS;
}

int main(int argc, char** argv)
{
#if PCAP
  pcap_handle = std::unique_ptr<srslte::mac_pcap>(new srslte::mac_pcap(srslte::srslte_rat_t::nr));
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

  if (mac_rar_pdu_unpack_test7()) {
    fprintf(stderr, "mac_rar_pdu_unpack_test7() failed.\n");
    return SRSLTE_ERROR;
  }

  if (mac_rar_pdu_unpack_test8()) {
    fprintf(stderr, "mac_rar_pdu_unpack_test8() failed.\n");
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

  if (pcap_handle) {
    pcap_handle->close();
  }

  return SRSLTE_SUCCESS;
}
