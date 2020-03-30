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

#include "srslte/asn1/rrc_asn1.h"
#include "srslte/asn1/rrc_asn1_utils.h"
#include "srslte/common/log_filter.h"
#include "srslte/common/mac_pcap.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "srslte/test/ue_test_interfaces.h"
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

srslte::log_ref mac_log{"MAC"};

namespace srslte {

// fake classes
class rlc_dummy : public srsue::rlc_dummy_interface
{
public:
  rlc_dummy(srslte::log_filter* log_) : received_bytes(0), log(log_) {}
  bool     has_data(const uint32_t lcid) final { return ul_queues[lcid] > 0; }
  uint32_t get_buffer_state(const uint32_t lcid) final { return ul_queues[lcid]; }
  int      read_pdu(uint32_t lcid, uint8_t* payload, uint32_t nof_bytes) final
  {
    if (!read_enable) {
      return 0;
    }
    uint32_t len = SRSLTE_MIN(ul_queues[lcid], nof_bytes);

    // set payload bytes to LCID so we can check later if the scheduling was correct
    memset(payload, lcid > 0 ? lcid : 0xf, len);

    // remove from UL queue
    ul_queues[lcid] -= len;

    return len;
  };
  void write_pdu(uint32_t lcid, uint8_t* payload, uint32_t nof_bytes) final
  {
    log->debug_hex(payload, nof_bytes, "Received %d B on LCID %d\n", nof_bytes, lcid);
    received_bytes += nof_bytes;
  }

  void     write_sdu(uint32_t lcid, uint32_t nof_bytes) { ul_queues[lcid] += nof_bytes; }
  uint32_t get_received_bytes() { return received_bytes; }

  void disable_read() { read_enable = false; }

private:
  bool                read_enable = true;
  uint32_t            received_bytes;
  srslte::log_filter* log;
  // UL queues where key is LCID and value the queue length
  std::map<uint32_t, uint32_t> ul_queues;
};

class phy_dummy : public phy_interface_mac_lte
{
public:
  phy_dummy() : scell_cmd(0){};

  void set_log(srslte::log* log_h_) { log_h = log_h_; }
  void reset()
  {
    last_preamble_idx = 0;
    last_target_power = 0;
    prach_delay_cnt   = 0;
    prach_tti         = 0;
    nof_rar_grants    = 0;
    rar_temp_rnti     = 0;
    rar_time_adv      = 0;
    last_crnti        = 0;
    prach_transmitted = false;
    prach_info_tx     = false;
  }

  void set_prach_tti(uint32_t tti, bool reset_transmitted = true)
  {
    this->prach_tti = tti;
    if (reset_transmitted) {
      prach_transmitted = false;
    }
  }

  // phy_interface_mac_lte
  void configure_prach_params(){};

  void prach_send(uint32_t preamble_idx, int allowed_subframe, float target_power_dbm, float ta_base_sec)
  {
    prach_delay_cnt   = 0;
    last_preamble_idx = preamble_idx;
    last_target_power = target_power_dbm;
    prach_transmitted = true;
    prach_info_tx     = true;
    log_h->info("PRACH will be transmitted at tti=%d, preamble_idx=%d\n", prach_tti, preamble_idx);
  }

  prach_info_t prach_get_info()
  {
    prach_info_t info = {};
    if (prach_info_tx) {
      prach_delay_cnt++;
      if (prach_delay_cnt > prach_delay) {
        info.tti_ra         = prach_tti;
        prach_info_tx       = false;
        info.is_transmitted = true;
        log_h->info("PRACH has been transmitted\n");
      }
    }
    return info;
  };

  void sr_send(){};
  int  sr_last_tx_tti() { return 0; };
  void set_mch_period_stop(uint32_t stop){};

  // phy_interface_mac_common
  void set_crnti(uint16_t rnti) { last_crnti = rnti; }
  void set_timeadv_rar(uint32_t ta_cmd) { rar_time_adv = ta_cmd; }
  void set_timeadv(uint32_t ta_cmd){};
  void set_activation_deactivation_scell(uint32_t cmd) { scell_cmd = cmd; };
  void set_rar_grant(uint8_t grant_payload[SRSLTE_RAR_GRANT_LEN], uint16_t rnti)
  {
    memcpy(rar_payload, grant_payload, SRSLTE_RAR_GRANT_LEN);
    rar_temp_rnti = rnti;
    nof_rar_grants++;
  }

  uint32_t get_current_tti() { return 0; }
  float    get_phr() { return 0; };
  float    get_pathloss_db() { return 0; };

  // getter for test execution
  uint32_t get_scell_cmd() { return scell_cmd; }

  // Testing methods
  int dl_grant(mac* mac_h, bool ack, uint16_t rnti, uint32_t len, const uint8_t* payload)
  {

    bool ack_v[SRSLTE_MAX_CODEWORDS] = {ack, 0};

    mac_interface_phy_lte::tb_action_dl_t dl_action    = {};
    mac_interface_phy_lte::mac_grant_dl_t dl_mac_grant = {};

    // Send grant to MAC
    dl_mac_grant.rnti              = rnti;
    dl_mac_grant.tb[0].ndi         = dl_ndi;
    dl_mac_grant.tb[0].ndi_present = true;
    dl_mac_grant.tb[0].tbs         = len;
    mac_h->new_grant_dl(0, dl_mac_grant, &dl_action);

    if (ack && !SRSLTE_RNTI_ISRAR(rnti)) {
      dl_ndi = !dl_ndi;
    }

    TESTASSERT(dl_action.tb[0].enabled);
    TESTASSERT((int)dl_action.tb[0].rv == dl_mac_grant.tb[0].rv);

    // Copy data and send tb_decoded
    memcpy(dl_action.tb[0].payload, payload, len);

    // print generated PDU
    log_h->info_hex(
        dl_action.tb[0].payload, dl_mac_grant.tb[0].tbs, "Generated DL PDU (%d B)\n", dl_mac_grant.tb[0].tbs);

#if HAVE_PCAP
    pcap_handle->write_dl_crnti(dl_action.tb[0].payload, dl_mac_grant.tb[0].tbs, rnti, true, 1);
#endif

    mac_h->tb_decoded(0, dl_mac_grant, ack_v);

    return 0;
  }

  int rar_and_check(mac* mac_h, bool preamble_matches, uint32_t temp_rnti)
  {

    // Generate RAR to MAC
    uint8_t grant[SRSLTE_RAR_GRANT_LEN] = {1};

    uint32_t rar_timeadv = 16;

    srslte::rar_pdu rar_pdu_msg;
    byte_buffer.clear();
    rar_pdu_msg.init_tx(&byte_buffer, 7);
    if (rar_pdu_msg.new_subh()) {
      rar_pdu_msg.get()->set_rapid(preamble_matches ? last_preamble_idx : (last_preamble_idx + 1));
      rar_pdu_msg.get()->set_ta_cmd(rar_timeadv);
      rar_pdu_msg.get()->set_temp_crnti(temp_rnti);
      rar_pdu_msg.get()->set_sched_grant(grant);
    }
    rar_pdu_msg.write_packet(byte_buffer.msg);

    // Send RAR grant to MAC
    dl_grant(mac_h, true, get_rar_rnti(), 7, byte_buffer.msg);

    // Check MAC passes RAR grant and TA cmd to PHY
    if (preamble_matches) {
      TESTASSERT(!memcmp(rar_payload, grant, SRSLTE_RAR_GRANT_LEN));
      TESTASSERT(rar_temp_rnti == temp_rnti);
      TESTASSERT(rar_time_adv == rar_timeadv);
    }

    return 0;
  }

  int ul_grant_and_check_tv(mac*           mac_h,
                            bool           ack,
                            uint16_t       rnti,
                            uint32_t       len,
                            const uint8_t* tv,
                            bool           is_rar        = false,
                            bool           adaptive_retx = false)
  {

    mac_interface_phy_lte::tb_action_ul_t ul_action    = {};
    mac_interface_phy_lte::mac_grant_ul_t ul_mac_grant = {};

    if (ack) {
      ul_ndi = !ul_ndi;
    }

    // Generate UL Grant
    if (!adaptive_retx) {
      ul_mac_grant.phich_available = !ack;
      ul_mac_grant.tb.ndi          = ul_ndi;
      ul_mac_grant.tb.ndi_present  = ack;
    } else {
      ul_mac_grant.hi_value        = true;
      ul_mac_grant.phich_available = true;
      ul_mac_grant.tb.ndi          = ul_ndi;
      ul_mac_grant.tb.ndi_present  = true;
    }

    ul_mac_grant.is_rar = is_rar;
    ul_mac_grant.rnti   = rnti;
    ul_mac_grant.tb.tbs = len;

    // Send grant to MAC and get action for this TB, then call tb_decoded to unlock MAC
    mac_h->new_grant_ul(0, ul_mac_grant, &ul_action);

    // print generated PDU
    log_h->info_hex(ul_action.tb.payload, ul_mac_grant.tb.tbs, "Generated UL PDU (%d B)\n", ul_mac_grant.tb.tbs);

#if HAVE_PCAP
    pcap_handle->write_ul_crnti(ul_action.tb.payload, ul_mac_grant.tb.tbs, rnti, true, 1);
#endif

    if (tv && ul_action.tb.payload) {
      return memcmp(ul_action.tb.payload, tv, len);
    } else {
      return 0;
    }
  }

  int get_last_preamble() { return last_preamble_idx; }

  uint32_t is_prach_transmitted() { return prach_transmitted; }

  uint32_t get_rar_rnti() { return (prach_tti % 10) + 1; }

  uint16_t get_crnti() { return last_crnti; }

  const static uint32_t prach_delay = 5;

private:
  uint32_t scell_cmd = 0;

  uint32_t prach_delay_cnt   = 0;
  uint32_t prach_tti         = 0;
  bool     prach_info_tx     = false;
  bool     prach_transmitted = false;
  float    last_target_power = 0;
  int      last_preamble_idx = -1;

  uint16_t last_crnti = 0;

  srslte::log* log_h;

  bool ul_ndi = false;
  bool dl_ndi = false;

  byte_buffer_t byte_buffer;

  uint32_t nof_rar_grants = 0;
  uint32_t rar_time_adv   = 0;
  uint16_t rar_temp_rnti  = 0;
  uint8_t  rar_payload[SRSLTE_RAR_GRANT_LEN];
};

class rrc_dummy : public rrc_interface_mac
{
public:
  void ho_ra_completed(bool ra_successful)
  {
    ho_finish            = true;
    ho_finish_successful = ra_successful;
  }
  void     release_pucch_srs() { printf("%s\n", __FUNCTION__); }
  void     run_tti(uint32_t tti) { printf("%s\n", __FUNCTION__); }
  void     ra_problem() { rach_problem++; }
  bool     ho_finish            = false;
  bool     ho_finish_successful = false;
  uint32_t rach_problem         = 0;
};

class stack_dummy : public stack_test_dummy
{
public:
  void init(mac* mac_, phy_interface_mac_lte* phy_)
  {
    mac_h = mac_;
    phy_h = phy_;
  }
  bool events_exist()
  {
    for (int i = 0; i < pending_tasks.nof_queues(); ++i) {
      if (not pending_tasks.empty(i)) {
        return true;
      }
    }
    return false;
  }
  void run_tti(uint32_t tti)
  {
    mac_h->run_tti(tti);
    // flush all events
    if (events_exist()) {
      srslte::move_task_t task{};
      if (pending_tasks.wait_pop(&task) >= 0) {
        task();
      }
    }
    timers.step_all();
  }

private:
  phy_interface_mac_lte* phy_h = nullptr;
  mac*                   mac_h = nullptr;
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

  rlc_log.set_level(srslte::LOG_LEVEL_DEBUG);
  rlc_log.set_hex_limit(100000);

  // dummy layers
  phy_dummy   phy;
  rlc_dummy   rlc(&rlc_log);
  rrc_dummy   rrc;
  stack_dummy stack;

  // the actual MAC
  mac mac("MAC");
  stack.init(&mac, &phy);
  mac.init(&phy, &rlc, &rrc, &stack);

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
  stack.run_tti(0);
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

  srslte::log_filter rlc_log("RLC");
  rlc_log.set_level(srslte::LOG_LEVEL_DEBUG);
  rlc_log.set_hex_limit(100000);

  // dummy layers
  phy_dummy   phy;
  rlc_dummy   rlc(&rlc_log);
  rrc_dummy   rrc;
  stack_dummy stack;

  // the actual MAC
  mac mac("MAC");
  stack.init(&mac, &phy);
  mac.init(&phy, &rlc, &rrc, &stack);
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
    mac_log->info_hex(ul_action.tb.payload, mac_grant.tb.tbs, "Generated PDU (%d B)\n", mac_grant.tb.tbs);
#if HAVE_PCAP
    pcap_handle->write_ul_crnti(ul_action.tb.payload, mac_grant.tb.tbs, 0x1001, true, 1);
#endif

    TESTASSERT(memcmp(ul_action.tb.payload, tv, sizeof(tv)) == 0);
  }

  // make sure MAC PDU thread picks up before stopping
  sleep(1);
  stack.run_tti(0);
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

  srslte::log_filter rlc_log("RLC");
  rlc_log.set_level(srslte::LOG_LEVEL_DEBUG);
  rlc_log.set_hex_limit(100000);

  // dummy layers
  phy_dummy   phy;
  rlc_dummy   rlc(&rlc_log);
  rrc_dummy   rrc;
  stack_dummy stack;

  // the actual MAC
  mac mac("MAC");
  stack.init(&mac, &phy);
  mac.init(&phy, &rlc, &rrc, &stack);
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

  // run TTI to setup Bj, no UL data available yet, so no BSR should be triggered
  stack.run_tti(0);
  usleep(200);

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
    mac_log->info_hex(ul_action.tb.payload, mac_grant.tb.tbs, "Generated PDU (%d B)\n", mac_grant.tb.tbs);
#if HAVE_PCAP
    pcap_handle->write_ul_crnti(ul_action.tb.payload, mac_grant.tb.tbs, 0x1001, true, 1);
#endif

    TESTASSERT(memcmp(ul_action.tb.payload, tv, sizeof(tv)) == 0);
  }

  // make sure MAC PDU thread picks up before stopping
  sleep(1);
  stack.run_tti(0);
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
  const uint8_t tv[] = {0x3d, 0x21, 0x32, 0x22, 0x28, 0x23, 0x14, 0x1f, 0x51, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
                        0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
                        0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
                        0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02,
                        0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
                        0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
                        0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
                        0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00};

  srslte::log_filter rlc_log("RLC");
  rlc_log.set_level(srslte::LOG_LEVEL_DEBUG);
  rlc_log.set_hex_limit(100000);

  // dummy layers
  phy_dummy   phy;
  rlc_dummy   rlc(&rlc_log);
  rrc_dummy   rrc;
  stack_dummy stack;

  // the actual MAC
  mac mac("MAC");
  stack.init(&mac, &phy);
  mac.init(&phy, &rlc, &rrc, &stack);
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

  // run TTI to setup Bj, BSR should be generated
  stack.run_tti(0);
  usleep(100);

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
    mac_log->info_hex(ul_action.tb.payload, mac_grant.tb.tbs, "Generated PDU (%d B)\n", mac_grant.tb.tbs);
#if HAVE_PCAP
    pcap_handle->write_ul_crnti(ul_action.tb.payload, mac_grant.tb.tbs, 0x1001, true, 1);
#endif

    TESTASSERT(memcmp(ul_action.tb.payload, tv, sizeof(tv)) == 0);
  }

  // make sure MAC PDU thread picks up before stopping
  sleep(1);
  stack.run_tti(0);
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

  srslte::log_filter rlc_log("RLC");
  rlc_log.set_level(srslte::LOG_LEVEL_DEBUG);
  rlc_log.set_hex_limit(100000);

  // dummy layers
  phy_dummy   phy;
  rlc_dummy   rlc(&rlc_log);
  rrc_dummy   rrc;
  stack_dummy stack;

  // the actual MAC
  mac mac("MAC");
  stack.init(&mac, &phy);
  mac.init(&phy, &rlc, &rrc, &stack);
  const uint16_t crnti = 0x1001;
  mac.set_ho_rnti(crnti, 0);

  // generate configs for two LCIDs with different priority and PBR
  std::vector<logical_channel_config_t> lcids;
  logical_channel_config_t              config = {};
  // The config of DRB1
  config.lcid     = 3;
  config.lcg      = 3;
  config.PBR      = 8;   // 8 kByte/s
  config.BSD      = 100; // 100ms
  config.priority = 15;
  lcids.push_back(config);

  // DRB2
  config.lcid     = 4;
  config.lcg      = 1;
  config.PBR      = 0; // no PBR
  config.priority = 7; // higher prio
  lcids.push_back(config);

  // setup LCIDs in MAC
  for (auto& channel : lcids) {
    mac.setup_lcid(channel.lcid, channel.lcg, channel.priority, channel.PBR, channel.BSD);
  }

  // run TTI to setup Bj
  stack.run_tti(0);
  sleep(1);

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
    mac_log->info_hex(ul_action.tb.payload, mac_grant.tb.tbs, "Generated PDU (%d B)\n", mac_grant.tb.tbs);
#if HAVE_PCAP
    pcap_handle->write_ul_crnti(ul_action.tb.payload, mac_grant.tb.tbs, 0x1001, true, 1);
#endif

    TESTASSERT(memcmp(ul_action.tb.payload, tv, sizeof(tv)) == 0);
  }

  // make sure MAC PDU thread picks up before stopping
  sleep(1);
  stack.run_tti(0);
  mac.stop();

  return SRSLTE_SUCCESS;
}

// PDU with single SDU and short BSR
int mac_ul_sch_pdu_with_short_bsr_test()
{
  const uint8_t tv[] = {0x3f, 0x3d, 0x01, 0x02, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01};

  srslte::log_filter rlc_log("RLC");
  rlc_log.set_level(srslte::LOG_LEVEL_DEBUG);
  rlc_log.set_hex_limit(100000);

  // dummy layers
  phy_dummy   phy;
  rlc_dummy   rlc(&rlc_log);
  rrc_dummy   rrc;
  stack_dummy stack;

  // the actual MAC
  mac mac("MAC");
  stack.init(&mac, &phy);
  mac.init(&phy, &rlc, &rrc, &stack);
  const uint16_t crnti = 0x1001;
  mac.set_ho_rnti(crnti, 0);

  // generate configs for two LCIDs with different priority and PBR
  std::vector<logical_channel_config_t> lcids;
  logical_channel_config_t              config = {};
  // The config of DRB1
  config.lcid     = 3;
  config.lcg      = 3;
  config.PBR      = 8;
  config.BSD      = 100; // 100ms
  config.priority = 15;
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

  // write dummy data
  rlc.write_sdu(1, 10);

  // generate TTI
  uint32 tti = 0;
  stack.run_tti(tti++);
  usleep(100);

  // create UL action and grant and push MAC PDU
  {
    mac_interface_phy_lte::tb_action_ul_t ul_action = {};
    mac_interface_phy_lte::mac_grant_ul_t mac_grant = {};

    mac_grant.rnti           = crnti; // make sure MAC picks it up as valid UL grant
    mac_grant.tb.ndi_present = true;
    mac_grant.tb.ndi         = true;
    mac_grant.tb.tbs         = 14; // give room for MAC subheader, SDU and short BSR
    int cc_idx               = 0;

    // Send grant to MAC and get action for this TB, then call tb_decoded to unlock MAC
    mac.new_grant_ul(cc_idx, mac_grant, &ul_action);

    // print generated PDU
    mac_log->info_hex(ul_action.tb.payload, mac_grant.tb.tbs, "Generated PDU (%d B)\n", mac_grant.tb.tbs);
#if HAVE_PCAP
    pcap_handle->write_ul_crnti(ul_action.tb.payload, mac_grant.tb.tbs, 0x1001, true, 1);
#endif

    TESTASSERT(memcmp(ul_action.tb.payload, tv, sizeof(tv)) == 0);
  }

  // make sure MAC PDU thread picks up before stopping
  sleep(1);
  stack.run_tti(tti);
  mac.stop();

  return SRSLTE_SUCCESS;
}

// PDU with only padding BSR (long BSR) and the rest padding
int mac_ul_sch_pdu_with_padding_bsr_test()
{
  srslte::log_filter rlc_log("RLC");
  rlc_log.set_level(srslte::LOG_LEVEL_DEBUG);
  rlc_log.set_hex_limit(100000);

  // dummy layers
  phy_dummy   phy;
  rlc_dummy   rlc(&rlc_log);
  rrc_dummy   rrc;
  stack_dummy stack;

  // the actual MAC
  mac mac("MAC");
  stack.init(&mac, &phy);
  mac.init(&phy, &rlc, &rrc, &stack);
  const uint16_t crnti = 0x1001;
  mac.set_ho_rnti(crnti, 0);

  // create UL action and grant and push MAC PDU
  {

    const uint8_t tv[] = {0x3e, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    mac_interface_phy_lte::tb_action_ul_t ul_action = {};
    mac_interface_phy_lte::mac_grant_ul_t mac_grant = {};

    mac_grant.rnti           = crnti; // make sure MAC picks it up as valid UL grant
    mac_grant.tb.ndi_present = true;
    mac_grant.tb.ndi         = true;
    mac_grant.tb.tbs         = 10; // give enough room for Padding BSR
    int cc_idx               = 0;

    // Send grant to MAC and get action for this TB, then call tb_decoded to unlock MAC
    mac.new_grant_ul(cc_idx, mac_grant, &ul_action);

    // print generated PDU
    mac_log->info_hex(ul_action.tb.payload, mac_grant.tb.tbs, "Generated PDU (%d B)\n", mac_grant.tb.tbs);
#if HAVE_PCAP
    pcap_handle->write_ul_crnti(ul_action.tb.payload, mac_grant.tb.tbs, 0x1001, true, 1);
#endif

    TESTASSERT(memcmp(ul_action.tb.payload, tv, sizeof(tv)) == 0);
  }

  // create UL action and grant and push MAC PDU
  {

    const uint8_t tv[] = {0x1c, 0x42};

    mac_interface_phy_lte::tb_action_ul_t ul_action = {};
    mac_interface_phy_lte::mac_grant_ul_t mac_grant = {};

    mac_grant.rnti           = crnti; // make sure MAC picks it up as valid UL grant
    mac_grant.tb.ndi_present = true;
    mac_grant.tb.ndi         = true;
    mac_grant.pid            = 2;
    mac_grant.tb.tbs         = 2; // give enough room for Padding BSR
    int cc_idx               = 0;

    // Add data to multiple LCID
    mac.setup_lcid(1, 1, 1, -1, 0);
    mac.setup_lcid(2, 2, 2, -1, 0);
    rlc.disable_read();
    rlc.write_sdu(1, 10);
    rlc.write_sdu(2, 100);
    stack.run_tti(1);

    // Send grant to MAC and get action for this TB, then call tb_decoded to unlock MAC
    mac.new_grant_ul(cc_idx, mac_grant, &ul_action);

    // print generated PDU
    mac_log->info_hex(ul_action.tb.payload, mac_grant.tb.tbs, "Generated PDU (%d B)\n", mac_grant.tb.tbs);
#if HAVE_PCAP
    pcap_handle->write_ul_crnti(ul_action.tb.payload, mac_grant.tb.tbs, 0x1001, true, 1);
#endif

    TESTASSERT(memcmp(ul_action.tb.payload, tv, sizeof(tv)) == 0);
  }

  // make sure MAC PDU thread picks up before stopping
  sleep(1);
  stack.run_tti(0);
  mac.stop();

  return SRSLTE_SUCCESS;
}

// Single byte MAC PDU
int mac_ul_sch_pdu_one_byte_test()
{
  const uint8_t tv[] = {0x1f};

  srslte::log_filter rlc_log("RLC");
  rlc_log.set_level(srslte::LOG_LEVEL_DEBUG);
  rlc_log.set_hex_limit(100000);

  // dummy layers
  phy_dummy   phy;
  rlc_dummy   rlc(&rlc_log);
  rrc_dummy   rrc;
  stack_dummy stack;

  // the actual MAC
  mac mac("MAC");
  stack.init(&mac, &phy);
  mac.init(&phy, &rlc, &rrc, &stack);
  const uint16_t crnti = 0x1001;
  mac.set_ho_rnti(crnti, 0);

  // write dummy data
  rlc.write_sdu(0, 10);

  // create UL action and grant and push MAC PDU
  {
    mac_interface_phy_lte::tb_action_ul_t ul_action = {};
    mac_interface_phy_lte::mac_grant_ul_t mac_grant = {};

    mac_grant.rnti           = crnti; // make sure MAC picks it up as valid UL grant
    mac_grant.tb.ndi_present = true;
    mac_grant.tb.ndi         = true;
    mac_grant.tb.tbs         = 1;
    int cc_idx               = 0;

    // Send grant to MAC and get action for this TB, then call tb_decoded to unlock MAC
    mac.new_grant_ul(cc_idx, mac_grant, &ul_action);

    // print generated PDU
    mac_log->info_hex(ul_action.tb.payload, mac_grant.tb.tbs, "Generated PDU (%d B)\n", mac_grant.tb.tbs);
#if HAVE_PCAP
    pcap_handle->write_ul_crnti(ul_action.tb.payload, mac_grant.tb.tbs, 0x1001, true, 1);
#endif

    TESTASSERT(memcmp(ul_action.tb.payload, tv, sizeof(tv)) == 0);
  }

  // make sure MAC PDU thread picks up before stopping
  sleep(1);
  stack.run_tti(0);
  mac.stop();

  return SRSLTE_SUCCESS;
}

// Two byte MAC PDU
int mac_ul_sch_pdu_two_byte_test()
{
  const uint8_t tv[] = {0x01, 0x01};

  srslte::log_filter rlc_log("RLC");
  rlc_log.set_level(srslte::LOG_LEVEL_DEBUG);
  rlc_log.set_hex_limit(100000);

  // dummy layers
  phy_dummy   phy;
  rlc_dummy   rlc(&rlc_log);
  rrc_dummy   rrc;
  stack_dummy stack;

  // the actual MAC
  mac mac("MAC");
  stack.init(&mac, &phy);
  mac.init(&phy, &rlc, &rrc, &stack);
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
    mac_grant.tb.tbs         = 2;
    int cc_idx               = 0;

    // Send grant to MAC and get action for this TB, then call tb_decoded to unlock MAC
    mac.new_grant_ul(cc_idx, mac_grant, &ul_action);

    // print generated PDU
    mac_log->info_hex(ul_action.tb.payload, mac_grant.tb.tbs, "Generated PDU (%d B)\n", mac_grant.tb.tbs);
#if HAVE_PCAP
    pcap_handle->write_ul_crnti(ul_action.tb.payload, mac_grant.tb.tbs, 0x1001, true, 1);
#endif

    TESTASSERT(memcmp(ul_action.tb.payload, tv, sizeof(tv)) == 0);
  }

  // make sure MAC PDU thread picks up before stopping
  sleep(1);
  stack.run_tti(0);
  mac.stop();

  return SRSLTE_SUCCESS;
}

// Three byte MAC PDU (Single byte padding, SDU header, 1 B SDU)
int mac_ul_sch_pdu_three_byte_test()
{
  const uint8_t tv[] = {0x3f, 0x01, 0x01};

  srslte::log_filter rlc_log("RLC");
  rlc_log.set_level(srslte::LOG_LEVEL_DEBUG);
  rlc_log.set_hex_limit(100000);

  // dummy layers
  phy_dummy   phy;
  rlc_dummy   rlc(&rlc_log);
  rrc_dummy   rrc;
  stack_dummy stack;

  // the actual MAC
  mac mac("MAC");
  stack.init(&mac, &phy);
  mac.init(&phy, &rlc, &rrc, &stack);
  const uint16_t crnti = 0x1001;
  mac.set_ho_rnti(crnti, 0);

  // write dummy data
  rlc.write_sdu(1, 1);

  // create UL action and grant and push MAC PDU
  {
    mac_interface_phy_lte::tb_action_ul_t ul_action = {};
    mac_interface_phy_lte::mac_grant_ul_t mac_grant = {};

    mac_grant.rnti           = crnti; // make sure MAC picks it up as valid UL grant
    mac_grant.tb.ndi_present = true;
    mac_grant.tb.ndi         = true;
    mac_grant.tb.tbs         = 3;
    int cc_idx               = 0;

    // Send grant to MAC and get action for this TB, then call tb_decoded to unlock MAC
    mac.new_grant_ul(cc_idx, mac_grant, &ul_action);

    // print generated PDU
    mac_log->info_hex(ul_action.tb.payload, mac_grant.tb.tbs, "Generated PDU (%d B)\n", mac_grant.tb.tbs);
#if HAVE_PCAP
    pcap_handle->write_ul_crnti(ul_action.tb.payload, mac_grant.tb.tbs, 0x1001, true, 1);
#endif

    TESTASSERT(memcmp(ul_action.tb.payload, tv, sizeof(tv)) == 0);
  }

  // make sure MAC PDU thread picks up before stopping
  sleep(1);
  stack.run_tti(0);
  mac.stop();

  return SRSLTE_SUCCESS;
}

struct ra_test {
  uint32_t                     nof_prachs;
  uint32_t                     rar_nof_rapid; // set to zero to don't transmit RAR
  uint32_t                     rar_nof_invalid_rapid;
  uint16_t                     crnti;
  uint16_t                     temp_rnti;
  uint32_t                     nof_msg3_retx;
  uint32_t                     preamble_idx;
  int                          assume_prach_transmitted;
  bool                         send_valid_ul_grant;
  bool                         msg4_enable;
  bool                         msg4_valid_conres;
  bool                         check_ra_successful;
  asn1::rrc::rach_cfg_common_s rach_cfg;
};

struct ra_test test;

int run_mac_ra_test(struct ra_test test, mac* mac, phy_dummy* phy, uint32_t* tti_state, srslte::stack_dummy* stack)
{
  uint32_t tti = *tti_state;

  const uint8_t tv_msg3[]    = {0x3c, 0x00, 0x01, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f};
  const uint8_t tv_msg3_ce[] = {0x1b, 0x00, 0x65};

  uint32_t      msg4_len            = 7;
  const uint8_t tv_msg4_nocontres[] = {0x1f, 0x1f};
  const uint8_t tv_msg4_valid[]     = {0x1c, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f};
  const uint8_t tv_msg4_invalid[]   = {0x1c, 0x0f, 0x0a, 0x0f, 0x0f, 0x0f, 0x0f};

  uint16_t temp_rnti = test.temp_rnti;

  bool new_prach = false;

  for (uint32_t j = 0; j < test.nof_prachs; j++) {

    // In the next TTI, a BSR shall be triggered which triggers SR which triggers PRACH
    if (test.assume_prach_transmitted != (int)j) {
      phy->set_prach_tti(tti + phy->prach_delay);
      stack->run_tti(tti++);
    }

    // Check MAC instructs PHY to transmit PRACH
    TESTASSERT(phy->is_prach_transmitted());

    // Test preamble index
    if (test.preamble_idx) {
      TESTASSERT(phy->get_last_preamble() == (int)test.preamble_idx);
    } else {
      TESTASSERT(phy->get_last_preamble() < test.rach_cfg.preamb_info.nof_ra_preambs.to_number());
    }

    // TODO: Test power ramping

    // Check MAC does not schedule RA-RNTI before window starts
    for (uint32_t i = 0; i < phy->prach_delay + 3 - 1; i++) {
      stack->run_tti(tti);
      TESTASSERT(!SRSLTE_RNTI_ISRAR(mac->get_dl_sched_rnti(tti)));
      tti++;
    }

    bool rapid_found = false;
    // Check MAC schedules correct RA-RNTI during window
    for (uint32_t i = 0; i < test.rach_cfg.ra_supervision_info.ra_resp_win_size.to_number() && !rapid_found; i++) {
      stack->run_tti(tti);
      TESTASSERT(mac->get_dl_sched_rnti(tti) == phy->get_rar_rnti());
      tti++;

      // Receive RAR
      if (test.rar_nof_rapid > 0) {
        rapid_found = i >= test.rar_nof_invalid_rapid;
        if (phy->rar_and_check(mac, rapid_found, temp_rnti)) {
          return -1;
        }
      }
    }

    // Run Contention Resolution if received correct RAPID
    if (rapid_found) {
      // Skip Contention resolution if preamble chosen by network
      if (test.preamble_idx) {
        break;
      }
      // Request Msg3 (re)-transmission
      for (uint32_t i = 0; i < test.nof_msg3_retx + 1; i++) {

        // Step to contention resolution. Make sure timer does not start until Msg3 is transmitted
        // and restarts on every retx
        for (int j = 0; j < test.rach_cfg.ra_supervision_info.mac_contention_resolution_timer.to_number() - 1; j++) {
          stack->run_tti(tti);
          TESTASSERT(mac->get_dl_sched_rnti(tti) == (test.crnti ? test.crnti : test.temp_rnti));
          tti++;
        }

        if (i == test.rach_cfg.max_harq_msg3_tx) {
          phy->set_prach_tti(tti + phy->prach_delay, false);
        }

        if (test.crnti) {
          TESTASSERT(!phy->ul_grant_and_check_tv(mac, i == 0, temp_rnti, 3, tv_msg3_ce, i == 0));
        } else {
          TESTASSERT(!phy->ul_grant_and_check_tv(mac, i == 0, temp_rnti, 9, tv_msg3, i == 0, i == 1));
        }
      }

      if (test.nof_msg3_retx == test.rach_cfg.max_harq_msg3_tx) {
        TESTASSERT(mac->get_dl_sched_rnti(tti) != temp_rnti);
        break;
      }

      for (int i = 0; i < test.rach_cfg.ra_supervision_info.mac_contention_resolution_timer.to_number() - 1; i++) {
        stack->run_tti(tti);
        TESTASSERT(mac->get_dl_sched_rnti(tti) == (test.crnti ? test.crnti : test.temp_rnti));
        tti++;

        if (test.msg4_enable) {
          if (test.crnti) {
            // Test a DL grant does not resolve the contention resolution
            if (phy->dl_grant(mac, true, test.crnti, 2, tv_msg4_nocontres)) {
              return -1;
            }
            TESTASSERT(phy->get_crnti() != test.crnti);

            // UL grant is checked later
            if (test.send_valid_ul_grant) {
              if (phy->ul_grant_and_check_tv(mac, true, test.crnti, 2, NULL)) {
                return -1;
              }
              break;
            } else if ((int)i == test.rach_cfg.ra_supervision_info.mac_contention_resolution_timer.to_number() - 2) {
              new_prach = true;
            }
          } else {
            if (phy->dl_grant(
                    mac, true, temp_rnti, msg4_len, test.msg4_valid_conres ? tv_msg4_valid : tv_msg4_invalid)) {
              return -1;
            }
            if (!test.msg4_valid_conres) {
              new_prach = true;
            }
            break;
          }
        }
      }
    }
    if (new_prach) {
      test.assume_prach_transmitted = (int)j + 1;
      phy->set_prach_tti(tti + phy->prach_delay, false);
      TESTASSERT(mac->get_dl_sched_rnti(tti) != temp_rnti);
      stack->run_tti(tti++);
    }
  }

  // RA procedure should be completed here
  if (test.check_ra_successful) {
    stack->run_tti(tti);
    TESTASSERT(phy->get_crnti() == (test.crnti ? test.crnti : test.temp_rnti));
    TESTASSERT(mac->get_dl_sched_rnti(tti) == (test.crnti ? test.crnti : test.temp_rnti));
    tti++;
  }

  *tti_state = tti;
  return 0;
}

/* Tests MAC RA procedure specified in 5.1 of 36.321
 * Currently not covered:
 *  - Selection of groupA/groupB sequences
 *  - Backoff timer
 *  - PDCCH order RACH initiation
 *  - Ignore RAR TA cmd when TA-Timer is running
 */
int mac_random_access_test()
{
  uint64_t contention_id = 0xf0f0f0f0f0f;

  srslte::log_filter phy_log("PHY");
  phy_log.set_level(srslte::LOG_LEVEL_DEBUG);
  phy_log.set_hex_limit(100000);

  srslte::log_filter rlc_log("RLC");
  rlc_log.set_level(srslte::LOG_LEVEL_DEBUG);
  rlc_log.set_hex_limit(100000);

  // dummy layers
  phy_dummy phy;
  phy.set_log(&phy_log);
  rlc_dummy   rlc(&rlc_log);
  rrc_dummy   rrc;
  stack_dummy stack;

  // Configure default RACH parameters
  asn1::rrc::rach_cfg_common_s rach_cfg = {};
  rach_cfg.preamb_info.nof_ra_preambs   = asn1::rrc::rach_cfg_common_s::preamb_info_s_::nof_ra_preambs_opts::n12;
  rach_cfg.ra_supervision_info.preamb_trans_max = asn1::rrc::preamb_trans_max_opts::n8;
  rach_cfg.ra_supervision_info.ra_resp_win_size =
      asn1::rrc::rach_cfg_common_s::ra_supervision_info_s_::ra_resp_win_size_e_::sf4;
  rach_cfg.max_harq_msg3_tx = 2;
  rach_cfg.ra_supervision_info.mac_contention_resolution_timer =
      asn1::rrc::rach_cfg_common_s::ra_supervision_info_s_::mac_contention_resolution_timer_opts::sf8;

  // Configure MAC
  mac mac("MAC");
  stack.init(&mac, &phy);
  mac.init(&phy, &rlc, &rrc, &stack);
  srslte::mac_cfg_t mac_cfg;
  set_mac_cfg_t_rach_cfg_common(&mac_cfg, rach_cfg);
  mac.set_config(mac_cfg);

  // generate config for LCIDs in different LCGs than CCCH
  std::vector<logical_channel_config_t> lcids;
  logical_channel_config_t              config = {};
  // The config of DRB1
  config.lcid     = 3;
  config.lcg      = 3;
  config.PBR      = 8;
  config.BSD      = 100; // 100ms
  config.priority = 15;
  lcids.push_back(config);

  // setup LCIDs in MAC
  for (auto& channel : lcids) {
    mac.setup_lcid(channel.lcid, channel.lcg, channel.priority, channel.PBR, channel.BSD);
  }

  // Generate Msg3
  mac.set_contention_id(contention_id);
  rlc.write_sdu(0, 6);   // UL-CCCH with Msg3
  rlc.write_sdu(3, 100); // DRB data on other LCG

  uint32 tti = 0;

  // Structure that defines the test to be executed
  struct ra_test my_test           = {};
  uint32_t       test_id           = 1;
  my_test.temp_rnti                = 100;
  my_test.assume_prach_transmitted = -1;

  // Test 1: No RAR is received.
  //         According to end of 5.1.5, UE sends up to preamb_trans_max upon which indicates RA problem to higher layers
  mac_log->info("\n=========== Test %d =============\n", test_id++);
  my_test.rach_cfg   = rach_cfg;
  my_test.nof_prachs = rach_cfg.ra_supervision_info.preamb_trans_max.to_number();
  TESTASSERT(!run_mac_ra_test(my_test, &mac, &phy, &tti, &stack));

  // Make sure it triggers RRC signal
  stack.run_tti(tti++);
  TESTASSERT(rrc.rach_problem == 1);

  // Reset MAC
  mac.reset();
  phy.reset();
  mac.set_contention_id(contention_id);

  // Test 2: RAR received but no matching RAPID
  //         The UE receives a RAR without a matching RAPID on every RAR response window TTI.
  //         According to 5.1.5, the RA procedure is considered non successful and tries again
  mac_log->info("\n=========== Test %d =============\n", test_id++);
  my_test.rar_nof_rapid         = 1;
  my_test.nof_prachs            = 1;
  my_test.rar_nof_invalid_rapid = rach_cfg.ra_supervision_info.ra_resp_win_size.to_number();
  TESTASSERT(!run_mac_ra_test(my_test, &mac, &phy, &tti, &stack));

  // Test 3: RAR received but no matching RAPID. Test Msg3 retransmissions
  //         On each HARQ retx, contention resolution timer must be restarted (5.1.5)
  //         When max-HARQ-msg3-retx, contention not successful
  mac_log->info("\n=========== Test %d =============\n", test_id++);
  my_test.rar_nof_invalid_rapid = 0;
  my_test.nof_msg3_retx         = rach_cfg.max_harq_msg3_tx;
  TESTASSERT(!run_mac_ra_test(my_test, &mac, &phy, &tti, &stack));

  // Test 4: RAR with valid RAPID. Msg3 transmitted, Msg4 received but invalid ConRes
  //         Contention resolution is defined in 5.1.5. If ConResID does not match, the ConRes is considered
  //         not successful and tries again
  mac_log->info("\n=========== Test %d =============\n", test_id++);
  phy.reset();
  my_test.nof_msg3_retx = 0;
  my_test.msg4_enable   = true;
  TESTASSERT(!run_mac_ra_test(my_test, &mac, &phy, &tti, &stack));

  // Test 5: Msg4 received and valid ConRes. In this case a valid ConResID is received and RA procedure is successful
  mac_log->info("\n=========== Test %d =============\n", test_id++);
  my_test.temp_rnti++; // Temporal C-RNTI has to change to avoid duplicate
  my_test.msg4_valid_conres        = true;
  my_test.check_ra_successful      = true;
  my_test.assume_prach_transmitted = 0;
  TESTASSERT(!run_mac_ra_test(my_test, &mac, &phy, &tti, &stack));

  // Test 6: RA with existing C-RNTI (Sends C-RNTI MAC CE)
  //         The transmission of C-RNTI MAC CE is only done if no CCCH is present (5.1.4).
  //         To trigger a new RA we have to either generate more data for DRB or wait until BSR-reTX is triggered
  rlc.write_sdu(3, 100);
  phy.set_crnti(0);
  mac_log->info("\n=========== Test %d =============\n", test_id++);
  my_test.crnti = my_test.temp_rnti;
  my_test.temp_rnti++; // Temporal C-RNTI has to change to avoid duplicate
  my_test.assume_prach_transmitted = -1;
  my_test.send_valid_ul_grant      = true;
  TESTASSERT(!run_mac_ra_test(my_test, &mac, &phy, &tti, &stack));

  // Test 7: Test Contention based Random Access. This is used eg in HO where preamble is chosen by UE.
  //         It is similar to Test 5 because C-RNTI is available to the UE when start the RA but
  //         In this case we will let the procedure expire the Contention Resolution window and make sure
  //         and RRC HO fail signal is sent to RRC.
  mac_log->info("\n=========== Test %d =============\n", test_id++);
  phy.set_prach_tti(tti + phy.prach_delay);
  phy.set_crnti(0);
  mac.start_cont_ho();
  stack.run_tti(tti++);
  rrc.ho_finish      = false;
  my_test.nof_prachs = rach_cfg.ra_supervision_info.preamb_trans_max.to_number();
  my_test.temp_rnti++; // Temporal C-RNTI has to change to avoid duplicate
  my_test.msg4_valid_conres        = false;
  my_test.assume_prach_transmitted = 0;
  my_test.check_ra_successful      = false;
  my_test.send_valid_ul_grant      = false;
  TESTASSERT(!run_mac_ra_test(my_test, &mac, &phy, &tti, &stack));
  TESTASSERT(!rrc.ho_finish_successful && rrc.ho_finish);

  // Test 8: Test Contention based Random Access. Same as above but we let the procedure finish successfully.
  mac_log->info("\n=========== Test %d =============\n", test_id++);
  phy.set_prach_tti(tti + phy.prach_delay);
  phy.set_crnti(0);
  mac.start_cont_ho();
  stack.run_tti(tti++);
  rrc.ho_finish      = false;
  my_test.nof_prachs = 1;
  my_test.temp_rnti++; // Temporal C-RNTI has to change to avoid duplicate
  my_test.send_valid_ul_grant = true;
  TESTASSERT(!run_mac_ra_test(my_test, &mac, &phy, &tti, &stack));
  TESTASSERT(rrc.ho_finish_successful && rrc.ho_finish);

  // Test 9: Test non-Contention based HO. Used in HO but preamble is given by the network. In addition to checking
  //         that the given preamble is correctly passed to the PHY, in this case there is no contention.
  //         In this first test, no RAR is received and RA procedure fails
  mac_log->info("\n=========== Test %d =============\n", test_id++);
  phy.set_prach_tti(tti + phy.prach_delay);
  stack.run_tti(tti++);
  phy.set_crnti(0);
  rrc.ho_finish        = false;
  my_test.preamble_idx = 3;
  mac.start_noncont_ho(my_test.preamble_idx, 0);
  my_test.nof_prachs            = rach_cfg.ra_supervision_info.preamb_trans_max.to_number();
  my_test.rar_nof_invalid_rapid = rach_cfg.ra_supervision_info.ra_resp_win_size.to_number();
  my_test.temp_rnti++; // Temporal C-RNTI has to change to avoid duplicate
  TESTASSERT(!run_mac_ra_test(my_test, &mac, &phy, &tti, &stack));
  stack.run_tti(tti++);
  TESTASSERT(!rrc.ho_finish_successful && rrc.ho_finish);

  // Test 10: Test non-Contention based HO. Used in HO but preamble is given by the network. We check that
  //         the procedure is considered successful without waiting for contention
  mac_log->info("\n=========== Test %d =============\n", test_id++);
  phy.set_prach_tti(tti + phy.prach_delay);
  stack.run_tti(tti++);
  phy.set_crnti(0);
  rrc.ho_finish        = false;
  my_test.preamble_idx = 3;
  mac.start_noncont_ho(my_test.preamble_idx, 0);
  my_test.nof_prachs            = 1;
  my_test.rar_nof_invalid_rapid = 0;
  my_test.check_ra_successful   = true;
  my_test.temp_rnti++; // Temporal C-RNTI has to change to avoid duplicate
  TESTASSERT(!run_mac_ra_test(my_test, &mac, &phy, &tti, &stack));
  stack.run_tti(tti++);
  TESTASSERT(rrc.ho_finish_successful && rrc.ho_finish);

  mac.stop();

  return SRSLTE_SUCCESS;
}

int main(int argc, char** argv)
{
#if HAVE_PCAP
  pcap_handle = std::unique_ptr<srslte::mac_pcap>(new srslte::mac_pcap());
  pcap_handle->open("mac_test.pcap");
#endif

  mac_log->set_level(srslte::LOG_LEVEL_DEBUG);
  mac_log->set_hex_limit(100000);

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

  if (mac_ul_sch_pdu_with_short_bsr_test()) {
    printf("mac_ul_sch_pdu_with_long_bsr_test() test failed.\n");
    return -1;
  }

  if (mac_ul_sch_pdu_with_padding_bsr_test()) {
    printf("mac_ul_sch_pdu_with_padding_bsr_test() test failed.\n");
    return -1;
  }

  if (mac_ul_sch_pdu_one_byte_test()) {
    printf("mac_ul_sch_pdu_one_byte_test() test failed.\n");
    return -1;
  }

  if (mac_ul_sch_pdu_two_byte_test()) {
    printf("mac_ul_sch_pdu_two_byte_test() test failed.\n");
    return -1;
  }

  if (mac_ul_sch_pdu_three_byte_test()) {
    printf("mac_ul_sch_pdu_three_byte_test() test failed.\n");
    return -1;
  }

  if (mac_random_access_test()) {
    printf("mac_random_access_test() test failed.\n");
    return -1;
  }

  return 0;
}
