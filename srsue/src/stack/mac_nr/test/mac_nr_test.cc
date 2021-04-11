/**
 * Copyright 2013-2021 Software Radio Systems Limited
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
#include "srsran/common/buffer_pool.h"
#include "srsran/common/common.h"
#include "srsran/common/test_common.h"
#include "srsran/test/ue_test_interfaces.h"
#include "srsue/hdr/stack/mac_nr/mac_nr.h"

using namespace srsue;

#define HAVE_PCAP 0
#define UE_ID 0

static std::unique_ptr<srsran::mac_pcap> pcap_handle = nullptr;

class dummy_phy : public phy_interface_mac_nr
{
public:
  dummy_phy() {}
  void send_prach(const uint32_t prach_occasion_,
                  const int      preamble_index_,
                  const float    preamble_received_target_power_,
                  const float    ta_base_sec_ = 0.0f) override
  {
    prach_occasion                 = prach_occasion_;
    preamble_index                 = preamble_index_;
    preamble_received_target_power = preamble_received_target_power_;
  }
  int tx_request(const tx_request_t& request) override { return 0; }
  int set_ul_grant(std::array<uint8_t, SRSRAN_RAR_UL_GRANT_NBITS>, uint16_t rnti, srsran_rnti_type_t rnti_type) override
  {
    return 0;
  }

  void get_last_send_prach(uint32_t* prach_occasion_, uint32_t* preamble_index_, int* preamble_received_target_power_)
  {
    *prach_occasion_                 = prach_occasion;
    *preamble_index_                 = preamble_index;
    *preamble_received_target_power_ = preamble_received_target_power;
  }
  bool has_valid_sr_resource(uint32_t sr_id) override { return false; }
  void clear_pending_grants() override {}

private:
  uint32_t prach_occasion                 = 0;
  uint32_t preamble_index                 = 0;
  int      preamble_received_target_power = 0;
};

class rrc_dummy : public rrc_interface_mac
{
public:
  rrc_dummy() {}
  virtual void ra_completed() {}
  virtual void ra_problem() {}
  virtual void release_pucch_srs() {}
};

class stack_dummy : public stack_test_dummy
{
public:
  void init(mac_nr* mac_, phy_interface_mac_nr* phy_)
  {
    mac_h = mac_;
    phy_h = phy_;
  }
  void run_tti(uint32_t tti)
  {
    mac_h->run_tti(tti);
    // flush all events
    stack_test_dummy::run_tti();
  }

private:
  phy_interface_mac_nr* phy_h = nullptr;
  mac_nr*               mac_h = nullptr;
};

// TODO: refactor to common test dummy components
class rlc_dummy : public srsue::rlc_dummy_interface
{
public:
  rlc_dummy() : received_bytes(0) {}
  bool     has_data_locked(const uint32_t lcid) final { return ul_queues[lcid] > 0; }
  uint32_t get_buffer_state(const uint32_t lcid) final { return ul_queues[lcid]; }
  int      read_pdu(uint32_t lcid, uint8_t* payload, uint32_t nof_bytes) final
  {
    if (!read_enable || nof_bytes < read_min) {
      return 0;
    }

    if (read_len > 0 && read_len < (int32_t)nof_bytes) {
      nof_bytes = read_len;
    }

    uint32_t len = SRSRAN_MIN(ul_queues[lcid], nof_bytes);

    // set payload bytes to LCID so we can check later if the scheduling was correct
    memset(payload, lcid > 0 ? lcid : 0xf, len);

    // remove from UL queue
    ul_queues[lcid] -= len;

    return len;
  };
  void write_pdu(uint32_t lcid, uint8_t* payload, uint32_t nof_bytes) final
  {
    logger.debug(payload, nof_bytes, "Received %d B on LCID %d", nof_bytes, lcid);
    received_bytes += nof_bytes;
  }

  void     write_sdu(uint32_t lcid, uint32_t nof_bytes) { ul_queues[lcid] += nof_bytes; }
  uint32_t get_received_bytes() { return received_bytes; }

  void disable_read() { read_enable = false; }
  void set_read_len(uint32_t len) { read_len = len; }
  void set_read_min(uint32_t len) { read_min = len; }
  void reset_queues()
  {
    for (auto& q : ul_queues) {
      q.second = 0;
    }
  }

private:
  bool                  read_enable = true;
  int32_t               read_len    = -1; // read all
  uint32_t              read_min    = 0;  // minimum "grant size" for read_pdu() to return data
  uint32_t              received_bytes;
  srslog::basic_logger& logger = srslog::fetch_basic_logger("RLC");
  // UL queues where key is LCID and value the queue length
  std::map<uint32_t, uint32_t> ul_queues;
};

// TODO: Add test
int msg3_test()
{
  return SRSRAN_SUCCESS;
}

// Basic PDU generation test
int mac_nr_ul_logical_channel_prioritization_test1()
{
  // PDU layout (20B in total)
  // -  2 B MAC subheader for SCH LCID=4
  // - 10 B sduPDU
  // -  1 B subheader padding
  // -  7 B padding
  const uint8_t tv[] = {0x04, 0x0a, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
                        0x04, 0x04, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  // dummy layers
  dummy_phy   phy;
  rlc_dummy   rlc;
  rrc_dummy   rrc;
  stack_dummy stack;

  // the actual MAC
  mac_nr mac(&stack.task_sched);

  mac_nr_args_t args = {};
  mac.init(args, &phy, &rlc, &rrc);

  stack.init(&mac, &phy);
  const uint16_t crnti = 0x1001;

  // generate config (default DRB2 config for EN-DC)
  std::vector<srsran::logical_channel_config_t> lcids;
  srsran::logical_channel_config_t              config = {};
  config.lcid                                          = 4;
  config.lcg                                           = 6;
  config.PBR                                           = 0;
  config.BSD                                           = 1000; // 1000ms
  config.priority                                      = 11;
  lcids.push_back(config);

  // setup LCIDs in MAC
  for (auto& channel : lcids) {
    mac.setup_lcid(channel);
  }

  // write dummy data to DRB2
  rlc.write_sdu(4, 10);

  // run TTI to setup Bj, BSR should be generated
  stack.run_tti(0);
  usleep(100);

  // create UL action and grant and read MAC PDU
  {
    mac_interface_phy_nr::tb_action_ul_t    ul_action = {};
    mac_interface_phy_nr::mac_nr_grant_ul_t mac_grant = {};

    mac_grant.rnti = crnti; // make sure MAC picks it up as valid UL grant
    mac_grant.pid  = 0;
    mac_grant.rnti = 0x1001;
    mac_grant.tti  = 0;
    mac_grant.tbs  = 20;
    int cc_idx     = 0;

    // Send grant to MAC and get action for this TB, 0x
    mac.new_grant_ul(cc_idx, mac_grant, &ul_action);

    // print generated PDU
    srslog::fetch_basic_logger("MAC").info(
        ul_action.tb.payload->msg, mac_grant.tbs, "Generated PDU (%d B)", mac_grant.tbs);
#if HAVE_PCAP
    pcap_handle->write_ul_crnti_nr(
        ul_action.tb.payload->msg, mac_grant.tbs, mac_grant.rnti, UE_ID, mac_grant.pid, mac_grant.tti);
#endif

    TESTASSERT(memcmp(ul_action.tb.payload->msg, tv, sizeof(tv)) == 0);
  }

  // make sure MAC PDU thread picks up before stopping
  stack.run_tti(0);
  mac.stop();

  return SRSRAN_SUCCESS;
}

// Correct packing of MAC PDU with subHeader with 16bit L field
int mac_nr_ul_logical_channel_prioritization_test2()
{
  // PDU layout (260 B in total)
  // -   3 B MAC subheader for SCH LCID=4 for 16bit L field
  // - 257 B sduPDU
  const uint8_t tv[] = {
      0x44, 0x01, 0x01, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
      0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
      0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
      0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
      0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
      0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
      0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
      0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
      0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
      0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
      0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
      0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
      0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
      0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
  };

  // dummy layers
  dummy_phy   phy;
  rlc_dummy   rlc;
  rrc_dummy   rrc;
  stack_dummy stack;

  // the actual MAC
  mac_nr mac(&stack.task_sched);

  mac_nr_args_t args = {};
  mac.init(args, &phy, &rlc, &rrc);

  stack.init(&mac, &phy);
  const uint16_t crnti = 0x1001;

  // generate config (default DRB2 config for EN-DC)
  std::vector<srsran::logical_channel_config_t> lcids;
  srsran::logical_channel_config_t              config = {};
  config.lcid                                          = 4;
  config.lcg                                           = 6;
  config.PBR                                           = 0;
  config.BSD                                           = 1000; // 1000ms
  config.priority                                      = 11;
  lcids.push_back(config);

  // setup LCIDs in MAC
  for (auto& channel : lcids) {
    mac.setup_lcid(channel);
  }

  // write dummy data to DRB2
  rlc.write_sdu(4, 1000);

  // run TTI to setup Bj, BSR should be generated
  stack.run_tti(0);
  usleep(100);

  // create UL action and grant and read MAC PDU
  {
    mac_interface_phy_nr::tb_action_ul_t    ul_action = {};
    mac_interface_phy_nr::mac_nr_grant_ul_t mac_grant = {};

    mac_grant.rnti = crnti; // make sure MAC picks it up as valid UL grant
    mac_grant.pid  = 0;
    mac_grant.rnti = 0x1001;
    mac_grant.tti  = 0;
    mac_grant.tbs  = 260;
    int cc_idx     = 0;

    // Send grant to MAC and get action for this TB, 0x
    mac.new_grant_ul(cc_idx, mac_grant, &ul_action);

    // print generated PDU
    srslog::fetch_basic_logger("MAC").info(
        ul_action.tb.payload->msg, mac_grant.tbs, "Generated PDU (%d B)", mac_grant.tbs);
#if HAVE_PCAP
    pcap_handle->write_ul_crnti_nr(
        ul_action.tb.payload->msg, mac_grant.tbs, mac_grant.rnti, UE_ID, mac_grant.pid, mac_grant.tti);
#endif

    TESTASSERT(memcmp(ul_action.tb.payload->msg, tv, sizeof(tv)) == 0);
  }

  // make sure MAC PDU thread picks up before stopping
  stack.run_tti(0);
  mac.stop();

  return SRSRAN_SUCCESS;
}

// Basic test for periodic BSR transmission
int mac_nr_ul_periodic_bsr_test()
{
  // PDU layout (10 B in total)
  // - 6B LCID=4 (+2B header, 8 B total)
  // - 2B SBSR
  const uint8_t tv1[] = {0x04, 0x06, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x3d, 0xd1};

  // PDU layout (10 B in total)
  // - 8B LCID=4
  const uint8_t tv2[] = {0x04, 0x08, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04};

  // dummy layers
  dummy_phy   phy;
  rlc_dummy   rlc;
  rrc_dummy   rrc;
  stack_dummy stack;

  // the actual MAC
  mac_nr mac(&stack.task_sched);

  mac_nr_args_t args = {};
  mac.init(args, &phy, &rlc, &rrc);

  stack.init(&mac, &phy);
  const uint16_t crnti = 0x1001;

  // generate config (default DRB2 config for EN-DC)
  std::vector<srsran::logical_channel_config_t> lcids;
  srsran::logical_channel_config_t              config = {};
  config.lcid                                          = 4;
  config.lcg                                           = 6;
  config.PBR                                           = 0;
  config.BSD                                           = 1000; // 1000ms
  config.priority                                      = 11;
  lcids.push_back(config);

  // setup LCIDs in MAC
  for (auto& channel : lcids) {
    mac.setup_lcid(channel);
  }

  srsran::bsr_cfg_nr_t bsr_cfg = {};
  bsr_cfg.periodic_timer       = 20;
  bsr_cfg.retx_timer           = 320;
  TESTASSERT(mac.set_config(bsr_cfg) == SRSRAN_SUCCESS);

  // run TTI to establish LCGs old buffer states at BSR
  uint32_t tti = 0;
  stack.run_tti(tti++);

  // write large amount of dummy data to DRB2
  rlc.write_sdu(4, 2000);

  // run TTI to setup Bj, BSR should be generated
  stack.run_tti(tti++);
  usleep(100);

  // create UL action and grant and read MAC PDU
  {
    mac_interface_phy_nr::tb_action_ul_t    ul_action = {};
    mac_interface_phy_nr::mac_nr_grant_ul_t mac_grant = {};

    mac_grant.rnti = crnti; // make sure MAC picks it up as valid UL grant
    mac_grant.pid  = 0;
    mac_grant.rnti = 0x1001;
    mac_grant.tti  = 0;
    mac_grant.tbs  = 10;
    int cc_idx     = 0;

    // Send grant to MAC and get action for this TB
    mac.new_grant_ul(cc_idx, mac_grant, &ul_action);

    // print generated PDU
    srslog::fetch_basic_logger("MAC").info(
        ul_action.tb.payload->msg, mac_grant.tbs, "Generated PDU (%d B)", mac_grant.tbs);
#if HAVE_PCAP
    pcap_handle->write_ul_crnti_nr(
        ul_action.tb.payload->msg, mac_grant.tbs, mac_grant.rnti, UE_ID, mac_grant.pid, mac_grant.tti);
#endif

    TESTASSERT(memcmp(ul_action.tb.payload->msg, tv1, sizeof(tv1)) == 0);
  }

  // for the next 19 TTI, until the periodic BSR is triggered again, no BSR should be included in the MAC PDU
  for (int i = 0; i < bsr_cfg.periodic_timer - 1; ++i) {
    stack.run_tti(tti++);
    usleep(100);

    // create UL action and grant and read MAC PDU
    {
      mac_interface_phy_nr::tb_action_ul_t    ul_action = {};
      mac_interface_phy_nr::mac_nr_grant_ul_t mac_grant = {};

      mac_grant.rnti = crnti; // make sure MAC picks it up as valid UL grant
      mac_grant.pid  = 0;
      mac_grant.rnti = 0x1001;
      mac_grant.tti  = 0;
      mac_grant.tbs  = 10;
      int cc_idx     = 0;

      // Send grant to MAC and get action for this TB
      mac.new_grant_ul(cc_idx, mac_grant, &ul_action);

      // print generated PDU
      srslog::fetch_basic_logger("MAC").info(
          ul_action.tb.payload->msg, mac_grant.tbs, "Generated PDU (%d B)", mac_grant.tbs);
#if HAVE_PCAP
      pcap_handle->write_ul_crnti_nr(
          ul_action.tb.payload->msg, mac_grant.tbs, mac_grant.rnti, UE_ID, mac_grant.pid, mac_grant.tti);
#endif

      TESTASSERT(memcmp(ul_action.tb.payload->msg, tv2, sizeof(tv2)) == 0);
    }
  }

  // run TTI to setup Bj, the same BSR should be generated again
  stack.run_tti(tti++);
  usleep(100);

  // create UL action and grant and read MAC PDU
  {
    mac_interface_phy_nr::tb_action_ul_t    ul_action = {};
    mac_interface_phy_nr::mac_nr_grant_ul_t mac_grant = {};

    mac_grant.rnti = crnti; // make sure MAC picks it up as valid UL grant
    mac_grant.pid  = 0;
    mac_grant.rnti = 0x1001;
    mac_grant.tti  = 0;
    mac_grant.tbs  = 10;
    int cc_idx     = 0;

    // Send grant to MAC and get action for this TB
    mac.new_grant_ul(cc_idx, mac_grant, &ul_action);

    // print generated PDU
    srslog::fetch_basic_logger("MAC").info(
        ul_action.tb.payload->msg, mac_grant.tbs, "Generated PDU (%d B)", mac_grant.tbs);
#if HAVE_PCAP
    pcap_handle->write_ul_crnti_nr(
        ul_action.tb.payload->msg, mac_grant.tbs, mac_grant.rnti, UE_ID, mac_grant.pid, mac_grant.tti);
#endif

    TESTASSERT(memcmp(ul_action.tb.payload->msg, tv1, sizeof(tv1)) == 0);
  }

  // make sure MAC PDU thread picks up before stopping
  stack.run_tti(tti++);
  mac.stop();

  return SRSRAN_SUCCESS;
}

int main()
{
#if HAVE_PCAP
  pcap_handle = std::unique_ptr<srsran::mac_pcap>(new srsran::mac_pcap());
  pcap_handle->open("mac_test_nr.pcap");
#endif

  auto& mac_logger = srslog::fetch_basic_logger("MAC");
  mac_logger.set_level(srslog::basic_levels::debug);
  mac_logger.set_hex_dump_max_size(-1);
  srslog::init();

  TESTASSERT(msg3_test() == SRSRAN_SUCCESS);
  TESTASSERT(mac_nr_ul_logical_channel_prioritization_test1() == SRSRAN_SUCCESS);
  TESTASSERT(mac_nr_ul_logical_channel_prioritization_test2() == SRSRAN_SUCCESS);
  TESTASSERT(mac_nr_ul_periodic_bsr_test() == SRSRAN_SUCCESS);

  return SRSRAN_SUCCESS;
}
