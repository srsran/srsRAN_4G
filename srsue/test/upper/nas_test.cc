/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#include "srsran/common/bcd_helpers.h"
#include "srsran/common/test_common.h"
#include "srsran/interfaces/ue_pdcp_interfaces.h"
#include "srsran/srslog/srslog.h"
#include "srsran/test/ue_test_interfaces.h"
#include "srsue/hdr/stack/upper/gw.h"
#include "srsue/hdr/stack/upper/nas.h"
#include "srsue/hdr/stack/upper/usim.h"
#include "srsue/hdr/stack/upper/usim_base.h"

using namespace srsue;

static_assert(alignof(LIBLTE_BYTE_MSG_STRUCT) == alignof(byte_buffer_t),
              "liblte buffer and byte buffer members misaligned");
static_assert(offsetof(LIBLTE_BYTE_MSG_STRUCT, N_bytes) == offsetof(byte_buffer_t, N_bytes),
              "liblte buffer and byte buffer members misaligned");
static_assert(offsetof(LIBLTE_BYTE_MSG_STRUCT, header) == offsetof(byte_buffer_t, buffer),
              "liblte buffer and byte buffer members misaligned");
static_assert(sizeof(LIBLTE_BYTE_MSG_STRUCT) <= offsetof(byte_buffer_t, msg),
              "liblte buffer and byte buffer members misaligned");

#define LCID 1

uint8_t auth_request_pdu[] = {0x07, 0x52, 0x01, 0x0c, 0x63, 0xa8, 0x54, 0x13, 0xe6, 0xa4, 0xce, 0xd9,
                              0x86, 0xfb, 0xe5, 0xce, 0x9b, 0x62, 0x5e, 0x10, 0x67, 0x57, 0xb3, 0xc2,
                              0xb9, 0x70, 0x90, 0x01, 0x0c, 0x72, 0x8a, 0x67, 0x57, 0x92, 0x52, 0xb8};

uint8_t sec_mode_command_pdu[] = {0x37, 0x4e, 0xfd, 0x57, 0x11, 0x00, 0x07, 0x5d, 0x02, 0x01, 0x02, 0xf0, 0x70, 0xc1};

uint8_t attach_accept_pdu[] = {0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x42, 0x01, 0x3e, 0x06, 0x00, 0x00,
                               0xf1, 0x10, 0x00, 0x01, 0x00, 0x2a, 0x52, 0x01, 0xc1, 0x01, 0x04, 0x1b, 0x07,
                               0x74, 0x65, 0x73, 0x74, 0x31, 0x32, 0x33, 0x06, 0x6d, 0x6e, 0x63, 0x30, 0x30,
                               0x31, 0x06, 0x6d, 0x63, 0x63, 0x30, 0x30, 0x31, 0x04, 0x67, 0x70, 0x72, 0x73,
                               0x05, 0x01, 0xc0, 0xa8, 0x05, 0x02, 0x27, 0x01, 0x80, 0x50, 0x0b, 0xf6, 0x00,
                               0xf1, 0x10, 0x80, 0x01, 0x01, 0x35, 0x16, 0x6d, 0xbc, 0x64, 0x01, 0x00};

uint8_t esm_info_req_pdu[] = {0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x5a, 0xd9};

uint8_t activate_dedicated_eps_bearer_pdu[] = {0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x62, 0x00, 0xc5, 0x05,
                                               0x01, 0x01, 0x07, 0x21, 0x31, 0x00, 0x03, 0x40, 0x08, 0xae,
                                               0x5d, 0x02, 0x00, 0xc2, 0x81, 0x34, 0x01, 0x4d};

uint8_t deactivate_eps_bearer_pdu[] = {0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x62, 0x00, 0xcd, 0x24};

uint16 mcc = 61441;
uint16 mnc = 65281;

using namespace srsran;

namespace srsran {

// fake classes
class pdcp_dummy : public rrc_interface_pdcp, public pdcp_interface_gw
{
public:
  void        write_pdu(uint32_t lcid, unique_byte_buffer_t pdu) {}
  void        write_pdu_bcch_bch(unique_byte_buffer_t pdu) {}
  void        write_pdu_bcch_dlsch(unique_byte_buffer_t pdu) {}
  void        write_pdu_pcch(unique_byte_buffer_t pdu) {}
  void        write_pdu_mch(uint32_t lcid, srsran::unique_byte_buffer_t sdu) {}
  const char* get_rb_name(uint32_t lcid) { return "lcid"; }
  void        write_sdu(uint32_t lcid, srsran::unique_byte_buffer_t sdu) {}
  bool        is_lcid_enabled(uint32_t lcid) { return false; }
};

class rrc_dummy : public rrc_interface_nas
{
public:
  rrc_dummy() : last_sdu_len(0)
  {
    plmns[0].plmn_id.from_number(mcc, mnc);
    plmns[0].tac = 0xffff;
  }
  void init(nas* nas_) { nas_ptr = nas_; }
  void write_sdu(unique_byte_buffer_t sdu)
  {
    last_sdu_len = sdu->N_bytes;
    // printf("NAS generated SDU (len=%d):\n", sdu->N_bytes);
    // srsran_vec_fprint_byte(stdout, sdu->msg, sdu->N_bytes);
  }
  const char* get_rb_name(uint32_t lcid) { return "lcid"; }
  uint32_t    get_last_sdu_len() { return last_sdu_len; }
  void        reset() { last_sdu_len = 0; }

  bool plmn_search()
  {
    nas_ptr->plmn_search_completed(plmns, 1);
    return true;
  }
  void plmn_select(srsran::plmn_id_t plmn_id){};
  void set_ue_identity(srsran::s_tmsi_t s_tmsi) {}
  bool connection_request(srsran::establishment_cause_t cause, srsran::unique_byte_buffer_t sdu)
  {
    printf("NAS generated SDU (len=%d):\n", sdu->N_bytes);
    last_sdu_len = sdu->N_bytes;
    srsran_vec_fprint_byte(stdout, sdu->msg, sdu->N_bytes);
    is_connected_flag = true;
    nas_ptr->connection_request_completed(true);
    return true;
  }
  bool is_connected() { return is_connected_flag; }

  uint16_t get_mcc() { return mcc; }
  uint16_t get_mnc() { return mnc; }
  void     enable_capabilities() {}
  uint32_t get_lcid_for_eps_bearer(const uint32_t& eps_bearer_id) { return 0; }
  void     paging_completed(bool outcome) {}
  bool     has_nr_dc() { return false; }

private:
  nas*                            nas_ptr;
  uint32_t                        last_sdu_len;
  nas_interface_rrc::found_plmn_t plmns[nas_interface_rrc::MAX_FOUND_PLMNS];
  bool                            is_connected_flag = false;
};

class test_stack_dummy : public srsue::stack_test_dummy, public stack_interface_gw, public thread
{
public:
  test_stack_dummy(pdcp_interface_gw* pdcp_) : pdcp(pdcp_), thread("DUMMY STACK") {}
  void init(srsue::nas* nas_)
  {
    nas = nas_;
    start(-1);
  }
  bool switch_on() { return true; }
  void write_sdu(uint32_t lcid, srsran::unique_byte_buffer_t sdu) { pdcp->write_sdu(lcid, std::move(sdu)); }
  bool is_lcid_enabled(uint32_t lcid) { return pdcp->is_lcid_enabled(lcid); }

  bool is_registered() { return true; }

  bool start_service_request() { return true; }

  void run_thread()
  {
    std::unique_lock<std::mutex> lk(init_mutex);
    running = true;
    init_cv.notify_all();
    lk.unlock();
    while (running) {
      task_sched.tic();
      task_sched.run_pending_tasks();
      nas->run_tti();
    }
  }
  void stop()
  {
    std::unique_lock<std::mutex> lk(init_mutex);
    while (not running) {
      init_cv.wait(lk);
    }
    running = false;
    wait_thread_finish();
  }
  pdcp_interface_gw*      pdcp    = nullptr;
  srsue::nas*             nas     = nullptr;
  bool                    running = false;
  std::mutex              init_mutex;
  std::condition_variable init_cv;
};

class gw_dummy : public gw_interface_nas, public gw_interface_pdcp
{
  int setup_if_addr(uint32_t eps_bearer_id,
                    uint32_t lcid,
                    uint8_t  pdn_type,
                    uint32_t ip_addr,
                    uint8_t* ipv6_if_id,
                    char*    err_str)
  {
    return SRSRAN_SUCCESS;
  }
  int apply_traffic_flow_template(const uint8_t&                                 eps_bearer_id,
                                  const uint8_t&                                 lcid,
                                  const LIBLTE_MME_TRAFFIC_FLOW_TEMPLATE_STRUCT* tft)
  {
    return SRSRAN_SUCCESS;
  }
  void write_pdu(uint32_t lcid, unique_byte_buffer_t pdu) {}
  void write_pdu_mch(uint32_t lcid, srsran::unique_byte_buffer_t sdu) {}
  void set_test_loop_mode(const test_loop_mode_state_t mode, const uint32_t ip_pdu_delay_ms = 0) {}
};

} // namespace srsran

int security_command_test()
{
  int              ret = SRSRAN_ERROR;
  stack_test_dummy stack;

  rrc_dummy rrc_dummy;
  gw_dummy  gw;

  usim_args_t args;
  args.algo     = "xor";
  args.imei     = "353490069873319";
  args.imsi     = "001010123456789";
  args.k        = "00112233445566778899aabbccddeeff";
  args.op       = "63BFA50EE6523365FF14C1F45F88737D";
  args.using_op = true;

  // init USIM
  srsue::usim usim(srslog::fetch_basic_logger("USIM"));
  usim.init(&args);

  {
    srsue::nas nas(&stack.task_sched);
    nas_args_t cfg;
    cfg.eia = "1,2,3";
    cfg.eea = "0,1,2,3";
    nas.init(&usim, &rrc_dummy, &gw, cfg);
    rrc_dummy.init(&nas);

    // push auth request PDU to NAS to generate security context
    byte_buffer_pool*    pool = byte_buffer_pool::get_instance();
    unique_byte_buffer_t tmp  = srsran::make_byte_buffer();
    TESTASSERT(tmp != nullptr);
    memcpy(tmp->msg, auth_request_pdu, sizeof(auth_request_pdu));
    tmp->N_bytes = sizeof(auth_request_pdu);
    nas.write_pdu(LCID, std::move(tmp));

    // TODO: add check for authentication response
    rrc_dummy.reset();

    // reuse buffer for security mode command
    tmp = srsran::make_byte_buffer();
    TESTASSERT(tmp != nullptr);
    memcpy(tmp->msg, sec_mode_command_pdu, sizeof(sec_mode_command_pdu));
    tmp->N_bytes = sizeof(sec_mode_command_pdu);
    nas.write_pdu(LCID, std::move(tmp));

    // check length of generated NAS SDU
    if (rrc_dummy.get_last_sdu_len() > 3) {
      ret = SRSRAN_SUCCESS;
    }
  }

  return ret;
}

int mme_attach_request_test()
{
  int ret = SRSRAN_ERROR;

  rrc_dummy  rrc_dummy;
  pdcp_dummy pdcp_dummy;

  srsue::usim usim(srslog::fetch_basic_logger("USIM"));
  usim_args_t args;
  args.mode = "soft";
  args.algo = "xor";
  args.imei = "353490069873319";
  args.imsi = "001010123456789";
  args.k    = "00112233445566778899aabbccddeeff";
  args.op   = "63BFA50EE6523365FF14C1F45F88737D";
  usim.init(&args);

  {
    nas_args_t nas_cfg;
    nas_cfg.force_imsi_attach = true;
    nas_cfg.apn_name          = "test123";

    test_stack_dummy stack(&pdcp_dummy);
    srsue::nas       nas(&stack.task_sched);
    srsue::gw        gw;

    nas.init(&usim, &rrc_dummy, &gw, nas_cfg);
    rrc_dummy.init(&nas);

    gw_args_t gw_args;
    gw_args.tun_dev_name     = "tun0";
    gw_args.log.gw_level     = "debug";
    gw_args.log.gw_hex_limit = 100000;

    gw.init(gw_args, &stack);
    stack.init(&nas);
    // trigger test
    stack.switch_on();
    stack.stop();

    // this will time out in the first place

    // reset length of last received NAS PDU
    rrc_dummy.reset();

    // finally push attach accept
    byte_buffer_pool*    pool = byte_buffer_pool::get_instance();
    unique_byte_buffer_t tmp  = srsran::make_byte_buffer();
    TESTASSERT(tmp != nullptr);
    memcpy(tmp->msg, attach_accept_pdu, sizeof(attach_accept_pdu));
    tmp->N_bytes = sizeof(attach_accept_pdu);
    nas.write_pdu(LCID, std::move(tmp));
    nas_metrics_t metrics;
    nas.get_metrics(&metrics);
    TESTASSERT(metrics.nof_active_eps_bearer == 1);

    // check length of generated NAS SDU (attach complete)
    if (rrc_dummy.get_last_sdu_len() > 3) {
      ret = SRSRAN_SUCCESS;
    }
    // ensure buffers are deleted before pool cleanup
    gw.stop();
  }

  return ret;
}

int esm_info_request_test()
{
  int ret = SRSRAN_ERROR;

  srsue::stack_test_dummy stack{};

  rrc_dummy rrc_dummy;
  gw_dummy  gw;

  usim_args_t args;
  args.algo = "xor";
  args.imei = "353490069873319";
  args.imsi = "001010123456789";
  args.k    = "00112233445566778899aabbccddeeff";
  args.op   = "63BFA50EE6523365FF14C1F45F88737D";

  // init USIM
  srsue::usim usim(srslog::fetch_basic_logger("USIM"));
  usim.init(&args);

  {
    srsue::nas nas(&stack.task_sched);
    nas_args_t cfg;
    cfg.apn_name          = "srsran";
    cfg.apn_user          = "srsuser";
    cfg.apn_pass          = "srspass";
    cfg.force_imsi_attach = true;
    nas.init(&usim, &rrc_dummy, &gw, cfg);

    // push ESM info request PDU to NAS to generate response
    unique_byte_buffer_t tmp = srsran::make_byte_buffer();
    TESTASSERT(tmp != nullptr);
    memcpy(tmp->msg, esm_info_req_pdu, sizeof(esm_info_req_pdu));
    tmp->N_bytes = sizeof(esm_info_req_pdu);
    nas.write_pdu(LCID, std::move(tmp));

    // check length of generated NAS SDU
    if (rrc_dummy.get_last_sdu_len() > 3) {
      ret = SRSRAN_SUCCESS;
    }
  }

  return ret;
}

int dedicated_eps_bearer_test()
{
  srsue::stack_test_dummy stack;

  rrc_dummy rrc_dummy;
  gw_dummy  gw;

  usim_args_t args;
  args.algo = "xor";
  args.imei = "353490069873319";
  args.imsi = "001010123456789";
  args.k    = "00112233445566778899aabbccddeeff";
  args.op   = "63BFA50EE6523365FF14C1F45F88737D";

  // init USIM
  srsue::usim usim(srslog::fetch_basic_logger("USIM"));
  usim.init(&args);

  srsue::nas nas(&stack.task_sched);
  nas_args_t cfg        = {};
  cfg.force_imsi_attach = true; // make sure we get a fresh security context
  nas.init(&usim, &rrc_dummy, &gw, cfg);

  // push dedicated EPS bearer PDU to NAS
  unique_byte_buffer_t tmp = srsran::make_byte_buffer();
  TESTASSERT(tmp != nullptr);
  memcpy(tmp->msg, activate_dedicated_eps_bearer_pdu, sizeof(activate_dedicated_eps_bearer_pdu));
  tmp->N_bytes = sizeof(activate_dedicated_eps_bearer_pdu);
  nas.write_pdu(LCID, std::move(tmp));

  // This should fail since no default bearer has been created yet
  nas_metrics_t metrics;
  nas.get_metrics(&metrics);
  TESTASSERT(metrics.nof_active_eps_bearer == 0);

  // add default EPS beaerer
  unique_byte_buffer_t attach_with_default_bearer = srsran::make_byte_buffer();
  TESTASSERT(attach_with_default_bearer != nullptr);
  memcpy(attach_with_default_bearer->msg, attach_accept_pdu, sizeof(attach_accept_pdu));
  attach_with_default_bearer->N_bytes = sizeof(attach_accept_pdu);
  nas.write_pdu(LCID, std::move(attach_with_default_bearer));

  // This should fail since no default bearer has been created yet
  nas.get_metrics(&metrics);
  TESTASSERT(metrics.nof_active_eps_bearer == 1);

  // push dedicated bearer activation and check that it was added
  tmp = srsran::make_byte_buffer();
  TESTASSERT(tmp != nullptr);
  memcpy(tmp->msg, activate_dedicated_eps_bearer_pdu, sizeof(activate_dedicated_eps_bearer_pdu));
  tmp->N_bytes = sizeof(activate_dedicated_eps_bearer_pdu);
  nas.write_pdu(LCID, std::move(tmp));
  nas.get_metrics(&metrics);
  TESTASSERT(metrics.nof_active_eps_bearer == 2);

  // tear-down dedicated bearer
  tmp = srsran::make_byte_buffer();
  TESTASSERT(tmp != nullptr);
  memcpy(tmp->msg, deactivate_eps_bearer_pdu, sizeof(deactivate_eps_bearer_pdu));
  tmp->N_bytes = sizeof(deactivate_eps_bearer_pdu);
  nas.write_pdu(LCID, std::move(tmp));
  nas.get_metrics(&metrics);
  TESTASSERT(metrics.nof_active_eps_bearer == 1);

  // try to tear-down dedicated bearer again
  tmp = srsran::make_byte_buffer();
  TESTASSERT(tmp != nullptr);
  memcpy(tmp->msg, deactivate_eps_bearer_pdu, sizeof(deactivate_eps_bearer_pdu));
  tmp->N_bytes = sizeof(deactivate_eps_bearer_pdu);
  nas.write_pdu(LCID, std::move(tmp));
  nas.get_metrics(&metrics);
  TESTASSERT(metrics.nof_active_eps_bearer == 1);

  return SRSRAN_SUCCESS;
}

int main(int argc, char** argv)
{
  // Setup logging.
  auto& rrc_logger = srslog::fetch_basic_logger("RRC", false);
  rrc_logger.set_level(srslog::basic_levels::debug);
  rrc_logger.set_hex_dump_max_size(100000);
  auto& nas_logger = srslog::fetch_basic_logger("NAS", false);
  nas_logger.set_level(srslog::basic_levels::debug);
  nas_logger.set_hex_dump_max_size(100000);
  auto& usim_logger = srslog::fetch_basic_logger("USIM", false);
  usim_logger.set_level(srslog::basic_levels::debug);
  usim_logger.set_hex_dump_max_size(100000);
  auto& gw_logger = srslog::fetch_basic_logger("GW", false);
  gw_logger.set_level(srslog::basic_levels::debug);
  gw_logger.set_hex_dump_max_size(100000);

  // Start the log backend.
  srslog::init();

  if (security_command_test()) {
    printf("Security command test failed.\n");
    return -1;
  }

  if (mme_attach_request_test()) {
    printf("Attach request test failed.\n");
    return -1;
  }

  if (esm_info_request_test()) {
    printf("ESM info request test failed.\n");
    return -1;
  }

  if (dedicated_eps_bearer_test()) {
    printf("Dedicated EPS bearer test failed.\n");
    return -1;
  }

  return 0;
}
