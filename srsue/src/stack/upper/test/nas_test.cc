/**
 * Copyright 2013-2023 Software Radio Systems Limited
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
#include "srsran/common/tsan_options.h"
#include "srsran/interfaces/ue_pdcp_interfaces.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/srsran_test.h"
#include "srsran/test/ue_test_interfaces.h"
#include "srsue/hdr/stack/upper/gw.h"
#include "srsue/hdr/stack/upper/nas.h"
#include "srsue/hdr/stack/upper/test/nas_test_common.h"
#include "srsue/hdr/stack/upper/usim.h"
#include "srsue/hdr/stack/upper/usim_base.h"

using namespace srsue;
using namespace srsran;

static_assert(alignof(LIBLTE_BYTE_MSG_STRUCT) == alignof(byte_buffer_t),
              "liblte buffer and byte buffer members misaligned");
static_assert(offsetof(LIBLTE_BYTE_MSG_STRUCT, N_bytes) == offsetof(byte_buffer_t, N_bytes),
              "liblte buffer and byte buffer members misaligned");
static_assert(offsetof(LIBLTE_BYTE_MSG_STRUCT, header) == offsetof(byte_buffer_t, buffer),
              "liblte buffer and byte buffer members misaligned");
static_assert(sizeof(LIBLTE_BYTE_MSG_STRUCT) <= offsetof(byte_buffer_t, msg),
              "liblte buffer and byte buffer members misaligned");

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
    nas_cfg.eia               = "1,2,3";
    nas_cfg.eea               = "0,1,2,3";
    nas_cfg.apn_name          = "test123";

    test_stack_dummy<srsue::nas> stack(&pdcp_dummy);
    srsue::nas                   nas(srslog::fetch_basic_logger("NAS"), &stack.task_sched);
    srsue::gw                    gw(srslog::fetch_basic_logger("GW"));

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
    srsue::nas nas(srslog::fetch_basic_logger("NAS"), &stack.task_sched);
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
    srsue::nas nas(srslog::fetch_basic_logger("NAS-5G"), &stack.task_sched);
    nas_args_t cfg;
    cfg.apn_name          = "srsran";
    cfg.apn_user          = "srsuser";
    cfg.apn_pass          = "srspass";
    cfg.eia               = "1,2,3";
    cfg.eea               = "0,1,2,3";
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

  srsue::nas nas(srslog::fetch_basic_logger("NAS"), &stack.task_sched);
  nas_args_t cfg        = {};
  cfg.force_imsi_attach = true; // make sure we get a fresh security context
  cfg.eia               = "1,2,3";
  cfg.eea               = "0,1,2,3";
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

  TESTASSERT(mme_attach_request_test() == SRSRAN_SUCCESS);
  TESTASSERT(security_command_test() == SRSRAN_SUCCESS);
  TESTASSERT(esm_info_request_test() == SRSRAN_SUCCESS);
  TESTASSERT(dedicated_eps_bearer_test() == SRSRAN_SUCCESS);

  return SRSRAN_SUCCESS;
}
