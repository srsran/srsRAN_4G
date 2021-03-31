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

  srsran::bsr_cfg_nr_t bsr_cfg = {};
  bsr_cfg.periodic_timer       = 20;
  bsr_cfg.retx_timer           = 320;
  TESTASSERT(mac.set_config(bsr_cfg) == SRSRAN_SUCCESS);

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

  return SRSRAN_SUCCESS;
}
