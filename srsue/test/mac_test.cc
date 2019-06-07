/*
 * Copyright 2013-2019 Software Radio Systems Limited
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
#include "srslte/common/mac_pcap.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "srsue/hdr/stack/mac/mac.h"
#include "srsue/hdr/stack/mac/mux.h"
#include <assert.h>
#include <iostream>
#include <string.h>

using namespace srsue;
using namespace srslte;

#define HAVE_PCAP 0

static std::unique_ptr<srslte::mac_pcap> pcap_handle = nullptr;

#define TESTASSERT(cond)                                                                                               \
  {                                                                                                                    \
    if (!(cond)) {                                                                                                     \
      std::cout << "[" << __FUNCTION__ << "][Line " << __LINE__ << "]: FAIL at " << (#cond) << std::endl;              \
      return SRSLTE_ERROR;                                                                                             \
    }                                                                                                                  \
  }

namespace srslte {

// fake classes
class rlc_dummy : public srsue::rlc_interface_mac
{
public:
  rlc_dummy(srslte::log_filter* log_) : received_bytes(0), log(log_) {}
  bool     has_data(const uint32_t lcid) { return ul_queues[lcid] > 0; }
  uint32_t get_buffer_state(const uint32_t lcid) { return ul_queues[lcid]; }
  int      read_pdu(uint32_t lcid, uint8_t* payload, uint32_t nof_bytes)
  {
    uint32_t len = SRSLTE_MIN(ul_queues[lcid], nof_bytes);

    // set payload bytes to LCID so we can check later if the scheduling was correct
    memset(payload, lcid, len);

    // remove from UL queue
    ul_queues[lcid] -= len;

    return len;
  };
  void write_pdu(uint32_t lcid, uint8_t* payload, uint32_t nof_bytes)
  {
    log->debug_hex(payload, nof_bytes, "Received %d B on LCID %d\n", nof_bytes, lcid);
    received_bytes += nof_bytes;
  };

  void     write_sdu(uint32_t lcid, uint32_t nof_bytes) { ul_queues[lcid] += nof_bytes; }
  void     write_pdu_bcch_bch(uint8_t* payload, uint32_t nof_bytes){};
  void     write_pdu_bcch_dlsch(uint8_t* payload, uint32_t nof_bytes){};
  void     write_pdu_pcch(uint8_t* payload, uint32_t nof_bytes){};
  void     write_pdu_mch(uint32_t lcid, uint8_t* payload, uint32_t nof_bytes){};
  uint32_t get_received_bytes() { return received_bytes; }

private:
  uint32_t            received_bytes;
  srslte::log_filter* log;
  // UL queues where key is LCID and value the queue length
  std::map<uint32_t, uint32_t> ul_queues;
};

class phy_dummy : public phy_interface_mac_lte
{
public:
  phy_dummy() : scell_cmd(0){};
  // phy_interface_mac_lte
  void         configure_prach_params(){};
  virtual void prach_send(uint32_t preamble_idx, int allowed_subframe, float target_power_dbm){};
  prach_info_t prach_get_info()
  {
    prach_info_t info = {};
    return info;
  };
  void sr_send(){};
  int  sr_last_tx_tti() { return 0; };
  void set_mch_period_stop(uint32_t stop){};

  // phy_interface_mac_common
  void     set_crnti(uint16_t rnti){};
  void     set_timeadv_rar(uint32_t ta_cmd){};
  void     set_timeadv(uint32_t ta_cmd){};
  void     set_activation_deactivation_scell(uint32_t cmd) { scell_cmd = cmd; };
  void     set_rar_grant(uint8_t grant_payload[SRSLTE_RAR_GRANT_LEN], uint16_t rnti){};
  uint32_t get_current_tti() { return 0; }
  float    get_phr() { return 0; };
  float    get_pathloss_db() { return 0; };

  // getter for test execution
  uint32_t get_scell_cmd() { return scell_cmd; }

private:
  uint32_t scell_cmd;
};

class rrc_dummy : public rrc_interface_mac
{
public:
  void ho_ra_completed(bool ra_successful) { printf("%s\n", __FUNCTION__); }
  void release_pucch_srs() { printf("%s\n", __FUNCTION__); }
  void run_tti(uint32_t tti) { printf("%s\n", __FUNCTION__); }
  void ra_problem() { printf("%s\n", __FUNCTION__); }
};

} // namespace srslte

int mac_unpack_test()
{
  // This MAC PDU contains three subheaders
  const uint32_t mac_header_len = 4;
  // Subheader 1 is SCell Activation/Deactivation CE
  //  - 1 byte SDU payload 0x02
  const uint32_t mac_pdu1_len = 1;
  // Subheader 2 is for LCID 1
  //  - 2 bytes SDU payload 0x00 0x08
  const uint32_t mac_pdu2_len = 2;
  // Subheader 3 is for LCID 3 (RLC AM PDU with 2 B header and 54 B data)
  //  - 56 bytes SDU payload 0x98 .. 0x89, 0x00, 0x00
  const uint32_t mac_pdu3_len = 56;

  uint8_t dl_sch_pdu[] = {0x3b, 0x21, 0x02, 0x03, 0x02, 0x00, 0x08, 0x98, 0x1b, 0x45, 0x00, 0x05, 0xda,
                          0xc7, 0x23, 0x40, 0x00, 0x40, 0x11, 0xe6, 0x9b, 0xc0, 0xa8, 0x03, 0x01, 0xc0,
                          0xa8, 0x03, 0x02, 0xd8, 0x29, 0x13, 0x89, 0x05, 0xc6, 0x2b, 0x73, 0x00, 0x0d,
                          0xc3, 0xb3, 0x5c, 0xa3, 0x23, 0xad, 0x00, 0x03, 0x20, 0x1b, 0x00, 0x00, 0x00,
                          0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x13, 0x89, 0x00, 0x00};

  srslte::log_filter rlc_log("RLC");
  srslte::log_filter mac_log("MAC");

  mac_log.set_level(srslte::LOG_LEVEL_DEBUG);
  mac_log.set_hex_limit(100000);
  rlc_log.set_level(srslte::LOG_LEVEL_DEBUG);
  rlc_log.set_hex_limit(100000);

  // dummy layers
  phy_dummy phy;
  rlc_dummy rlc(&rlc_log);
  rrc_dummy rrc;

  // the actual MAC
  mac mac;
  mac.init(&phy, &rlc, &rrc, &mac_log);

  // create dummy DL action and grant and push MAC PDU
  mac_interface_phy_lte::tb_action_dl_t dl_action;
  mac_interface_phy_lte::mac_grant_dl_t mac_grant;
  bzero(&dl_action, sizeof(dl_action));
  bzero(&mac_grant, sizeof(mac_grant));
  mac_grant.rnti      = 0xbeaf;
  mac_grant.tb[0].tbs = sizeof(dl_sch_pdu);
  int cc_idx          = 0;

  // Send grant to MAC and get action for this TB, then call tb_decoded to unlock MAC
  mac.new_grant_dl(cc_idx, mac_grant, &dl_action);

  // Copy PDU into provided buffer
  bool dl_ack[SRSLTE_MAX_CODEWORDS] = {true, false};
  memcpy(dl_action.tb[0].payload, dl_sch_pdu, sizeof(dl_sch_pdu));
  dl_action.tb[0].enabled = true;
  mac.tb_decoded(cc_idx, mac_grant, dl_ack);

  // make sure MAC PDU thread picks up before stopping
  sleep(1);
  mac.run_tti(0);
  mac.stop();

  // check length of both received RLC PDUs
  TESTASSERT(rlc.get_received_bytes() == mac_pdu2_len + mac_pdu3_len);

  // check received SCell activation command
  TESTASSERT(phy.get_scell_cmd() == 2);

  return SRSLTE_SUCCESS;
}

// Basic test with a single padding byte and a 10B SCH SDU
int mac_ul_sch_pdu_test1()
{
  const uint8_t tv[] = {0x3f, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01};

  srslte::log_filter mac_log("MAC");
  mac_log.set_level(srslte::LOG_LEVEL_DEBUG);
  mac_log.set_hex_limit(100000);

  srslte::log_filter rlc_log("RLC");
  rlc_log.set_level(srslte::LOG_LEVEL_DEBUG);
  rlc_log.set_hex_limit(100000);

  // dummy layers
  phy_dummy phy;
  rlc_dummy rlc(&rlc_log);
  rrc_dummy rrc;

  // the actual MAC
  mac mac;
  mac.init(&phy, &rlc, &rrc, &mac_log);
  const uint16_t crnti = 0x1001;
  mac.set_ho_rnti(crnti, 0);

  // write dummy data
  rlc.write_sdu(1, 10);

  // create UL action and grant and push MAC PDU
  {
    mac_interface_phy_lte::tb_action_ul_t ul_action = {};
    mac_interface_phy_lte::mac_grant_ul_t mac_grant = {};

    mac_grant.rnti           = crnti; // make sure MAC picks it up as valid UL grant
    mac_grant.tb.ndi_present = true;
    mac_grant.tb.ndi         = true;
    mac_grant.tb.tbs         = 12; // give room for MAC subheader, SDU and one padding byte
    int cc_idx               = 0;

    // Send grant to MAC and get action for this TB, then call tb_decoded to unlock MAC
    mac.new_grant_ul(cc_idx, mac_grant, &ul_action);

    // print generated PDU
    mac_log.info_hex(ul_action.tb.payload, mac_grant.tb.tbs, "Generated PDU (%d B)\n", mac_grant.tb.tbs);
#if HAVE_PCAP
    pcap_handle->write_ul_crnti(ul_action.tb.payload, mac_grant.tb.tbs, 0x1001, true, 1);
#endif

    TESTASSERT(memcmp(ul_action.tb.payload, tv, sizeof(tv)) == 0);
  }

  // make sure MAC PDU thread picks up before stopping
  sleep(1);
  mac.run_tti(0);
  mac.stop();

  return SRSLTE_SUCCESS;
}

// Basic logical channel prioritization test with 3 SCH SDUs
int mac_ul_logical_channel_prioritization_test1()
{
  // PDU layout (21 B in total)
  // -  2 B MAC subheader for SCH LCID=1
  // -  2 B MAC subheader for SCH LCID=2
  // -  1 B MAC subheader for SCH LCID=3
  // - 10 B MAC SDU for LCID=1
  // -  4 B MAC SDU for LCID=2
  // -  2 B MAC SDU for LCID=3
  const uint8_t tv[] = {0x21, 0x0a, 0x22, 0x04, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
                        0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x02, 0x03, 0x03};

  srslte::log_filter mac_log("MAC");
  mac_log.set_level(srslte::LOG_LEVEL_DEBUG);
  mac_log.set_hex_limit(100000);

  srslte::log_filter rlc_log("RLC");
  rlc_log.set_level(srslte::LOG_LEVEL_DEBUG);
  rlc_log.set_hex_limit(100000);

  // dummy layers
  phy_dummy phy;
  rlc_dummy rlc(&rlc_log);
  rrc_dummy rrc;

  // the actual MAC
  mac mac;
  mac.init(&phy, &rlc, &rrc, &mac_log);
  const uint16_t crnti = 0x1001;
  mac.set_ho_rnti(crnti, 0);

  // generate configs for three LCIDs with different priority and PBR
  std::vector<logical_channel_config_t> lcids;
  logical_channel_config_t              config = {};
  config.lcid                                  = 1;
  config.lcg                                   = 1;
  config.PBR                                   = 10;
  config.BSD                                   = 1000; // 1000ms
  config.priority                              = 1;    // highest prio
  lcids.push_back(config);

  config.lcid     = 2;
  config.lcg      = 1;
  config.PBR      = 4;
  config.priority = 2;
  lcids.push_back(config);

  config.lcid     = 3;
  config.lcg      = 1;
  config.PBR      = 2;
  config.priority = 3;
  lcids.push_back(config);

  // setup LCIDs in MAC
  for (auto& channel : lcids) {
    mac.setup_lcid(channel.lcid, channel.lcg, channel.priority, channel.PBR, channel.BSD);
  }

  // write dummy data for each LCID (except CCCH)
  rlc.write_sdu(1, 50);
  rlc.write_sdu(2, 40);
  rlc.write_sdu(3, 20);

  // create UL action and grant and push MAC PDU
  {
    mac_interface_phy_lte::tb_action_ul_t ul_action = {};
    mac_interface_phy_lte::mac_grant_ul_t mac_grant = {};

    mac_grant.rnti           = crnti; // make sure MAC picks it up as valid UL grant
    mac_grant.tb.ndi_present = true;
    mac_grant.tb.ndi         = true;
    mac_grant.tb.tbs         = 21; // each LCID has more data to transmit than is available
    int cc_idx               = 0;

    // Send grant to MAC and get action for this TB, then call tb_decoded to unlock MAC
    mac.new_grant_ul(cc_idx, mac_grant, &ul_action);

    // print generated PDU
    mac_log.info_hex(ul_action.tb.payload, mac_grant.tb.tbs, "Generated PDU (%d B)\n", mac_grant.tb.tbs);
#if HAVE_PCAP
    pcap_handle->write_ul_crnti(ul_action.tb.payload, mac_grant.tb.tbs, 0x1001, true, 1);
#endif

    TESTASSERT(memcmp(ul_action.tb.payload, tv, sizeof(tv)) == 0);
  }

  // make sure MAC PDU thread picks up before stopping
  sleep(1);
  mac.run_tti(0);
  mac.stop();

  return SRSLTE_SUCCESS;
}

// Similar test like above but with a much larger UL grant, we expect that each LCID is fully served
int mac_ul_logical_channel_prioritization_test2()
{
  // PDU layout (120 B in total)
  // -  1 B MAC subheader for Short BSR
  // -  2 B MAC subheader for SCH LCID=1
  // -  2 B MAC subheader for SCH LCID=2
  // -  2 B MAC subheader for SCH LCID=3
  // -  1 B MAC subheader for Padding
  //
  // -  1 B Short BSR
  // - 50 B MAC SDU for LCID=1
  // - 40 B MAC SDU for LCID=2
  // - 20 B MAC SDU for LCID=3
  // -  1 B Padding
  // =120 N
  const uint8_t tv[] = {0x3d, 0x21, 0x32, 0x22, 0x28, 0x23, 0x14, 0x1f, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
                        0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
                        0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
                        0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02,
                        0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
                        0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
                        0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
                        0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00};

  srslte::log_filter mac_log("MAC");
  mac_log.set_level(srslte::LOG_LEVEL_DEBUG);
  mac_log.set_hex_limit(100000);

  srslte::log_filter rlc_log("RLC");
  rlc_log.set_level(srslte::LOG_LEVEL_DEBUG);
  rlc_log.set_hex_limit(100000);

  // dummy layers
  phy_dummy phy;
  rlc_dummy rlc(&rlc_log);
  rrc_dummy rrc;

  // the actual MAC
  mac mac;
  mac.init(&phy, &rlc, &rrc, &mac_log);
  const uint16_t crnti = 0x1001;
  mac.set_ho_rnti(crnti, 0);

  // generate configs for three LCIDs with different priority and PBR
  std::vector<logical_channel_config_t> lcids;
  logical_channel_config_t              config = {};
  config.lcid                                  = 1;
  config.lcg                                   = 1;
  config.PBR                                   = 10;
  config.BSD                                   = 1000; // 1000ms
  config.priority                              = 1;    // highest prio
  lcids.push_back(config);

  config.lcid     = 2;
  config.lcg      = 1;
  config.PBR      = 4;
  config.priority = 2;
  lcids.push_back(config);

  config.lcid     = 3;
  config.lcg      = 1;
  config.PBR      = 2;
  config.priority = 3;
  lcids.push_back(config);

  // setup LCIDs in MAC
  for (auto& channel : lcids) {
    mac.setup_lcid(channel.lcid, channel.lcg, channel.priority, channel.PBR, channel.BSD);
  }

  // write dummy data for each LCID (except CCCH)
  rlc.write_sdu(1, 50);
  rlc.write_sdu(2, 40);
  rlc.write_sdu(3, 20);

  // create UL action and grant and push MAC PDU
  {
    mac_interface_phy_lte::tb_action_ul_t ul_action = {};
    mac_interface_phy_lte::mac_grant_ul_t mac_grant = {};

    mac_grant.rnti           = crnti; // make sure MAC picks it up as valid UL grant
    mac_grant.tb.ndi_present = true;
    mac_grant.tb.ndi         = true;
    mac_grant.tb.tbs         = 120; // each LCID has more data to transmit than is available
    int cc_idx               = 0;

    // Send grant to MAC and get action for this TB, then call tb_decoded to unlock MAC
    mac.new_grant_ul(cc_idx, mac_grant, &ul_action);

    // print generated PDU
    mac_log.info_hex(ul_action.tb.payload, mac_grant.tb.tbs, "Generated PDU (%d B)\n", mac_grant.tb.tbs);
#if HAVE_PCAP
    pcap_handle->write_ul_crnti(ul_action.tb.payload, mac_grant.tb.tbs, 0x1001, true, 1);
#endif

    TESTASSERT(memcmp(ul_action.tb.payload, tv, sizeof(tv)) == 0);
  }

  // make sure MAC PDU thread picks up before stopping
  sleep(1);
  mac.run_tti(0);
  mac.stop();

  return SRSLTE_SUCCESS;
}

// Basic logical channel prioritization test with 2 SCH SDUs
// Using default setting for dedicated bearer
int mac_ul_logical_channel_prioritization_test3()
{
  // PDU layout (21 B in total)
  // -  2 B MAC subheader for SCH LCID=4
  // -  1 B MAC subheader for SCH LCID=3
  // - 10 B MAC SDU for LCID=4
  // -  8 B MAC SDU for LCID=3
  const uint8_t tv[] = {0x24, 0x0a, 0x03, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
                        0x04, 0x04, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03};

  srslte::log_filter mac_log("MAC");
  mac_log.set_level(srslte::LOG_LEVEL_DEBUG);
  mac_log.set_hex_limit(100000);

  srslte::log_filter rlc_log("RLC");
  rlc_log.set_level(srslte::LOG_LEVEL_DEBUG);
  rlc_log.set_hex_limit(100000);

  // dummy layers
  phy_dummy phy;
  rlc_dummy rlc(&rlc_log);
  rrc_dummy rrc;

  // the actual MAC
  mac mac;
  mac.init(&phy, &rlc, &rrc, &mac_log);
  const uint16_t crnti = 0x1001;
  mac.set_ho_rnti(crnti, 0);

  // generate configs for two LCIDs with different priority and PBR
  std::vector<logical_channel_config_t> lcids;
  logical_channel_config_t              config = {};
  // The config of DRB1
  config.lcid     = 3;
  config.lcg      = 3;
  config.PBR      = 8;
  config.BSD      = 100; // 1000ms
  config.priority = 15;  // highest prio
  lcids.push_back(config);

  // DRB2
  config.lcid     = 4;
  config.lcg      = 1;
  config.PBR      = 0;
  config.priority = 7;
  lcids.push_back(config);

  // setup LCIDs in MAC
  for (auto& channel : lcids) {
    mac.setup_lcid(channel.lcid, channel.lcg, channel.priority, channel.PBR, channel.BSD);
  }

  // write dummy data for each LCID
  rlc.write_sdu(3, 50);
  rlc.write_sdu(4, 50);

  // create UL action and grant and push MAC PDU
  {
    mac_interface_phy_lte::tb_action_ul_t ul_action = {};
    mac_interface_phy_lte::mac_grant_ul_t mac_grant = {};

    mac_grant.rnti           = crnti; // make sure MAC picks it up as valid UL grant
    mac_grant.tb.ndi_present = true;
    mac_grant.tb.ndi         = true;
    mac_grant.tb.tbs         = 21; // each LCID has more data to transmit than is available
    int cc_idx               = 0;

    // Send grant to MAC and get action for this TB, then call tb_decoded to unlock MAC
    mac.new_grant_ul(cc_idx, mac_grant, &ul_action);

    // print generated PDU
    mac_log.info_hex(ul_action.tb.payload, mac_grant.tb.tbs, "Generated PDU (%d B)\n", mac_grant.tb.tbs);
#if HAVE_PCAP
    pcap_handle->write_ul_crnti(ul_action.tb.payload, mac_grant.tb.tbs, 0x1001, true, 1);
#endif

    TESTASSERT(memcmp(ul_action.tb.payload, tv, sizeof(tv)) == 0);
  }

  // make sure MAC PDU thread picks up before stopping
  sleep(1);
  mac.run_tti(0);
  mac.stop();

  return SRSLTE_SUCCESS;
}

int main(int argc, char** argv)
{
#if HAVE_PCAP
  pcap_handle = std::unique_ptr<srslte::mac_pcap>(new srslte::mac_pcap());
  pcap_handle->open("mac_test.pcap");
#endif

  if (mac_unpack_test()) {
    printf("MAC PDU unpack test failed.\n");
    return -1;
  }

  if (mac_ul_sch_pdu_test1()) {
    printf("mac_ul_sch_pdu_test1() test failed.\n");
    return -1;
  }

  if (mac_ul_logical_channel_prioritization_test1()) {
    printf("mac_ul_logical_channel_prioritization_test1() test failed.\n");
    return -1;
  }

  if (mac_ul_logical_channel_prioritization_test2()) {
    printf("mac_ul_logical_channel_prioritization_test2() test failed.\n");
    return -1;
  }

  if (mac_ul_logical_channel_prioritization_test3()) {
    printf("mac_ul_logical_channel_prioritization_test3() test failed.\n");
    return -1;
  }

  return 0;
}
