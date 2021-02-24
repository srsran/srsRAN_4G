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

#include "srslte/common/common.h"
#include "srslte/common/log.h"
#include "srslte/common/logger_srslog_wrapper.h"
#include "srslte/common/test_common.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "srslte/srslog/srslog.h"
#include "srslte/srslte.h"
#include "srsue/hdr/stack/upper/gw.h"

class test_stack_dummy : public srsue::stack_interface_gw
{
public:
  bool is_registered() { return true; }
  bool start_service_request() { return true; };
  void write_sdu(uint32_t lcid, srslte::unique_byte_buffer_t sdu) { return; }
  bool is_lcid_enabled(uint32_t lcid) { return true; }
};

int gw_change_lcid_test()
{

  // Setup logging.
  srslog::sink* log_sink = srslog::create_stdout_sink();
  if (!log_sink) {
    return SRSLTE_ERROR;
  }

  srslog::log_channel* chan = srslog::create_log_channel("main_channel", *log_sink);
  if (!chan) {
    return SRSLTE_ERROR;
  }

  srslte::srslog_wrapper log_wrapper(*chan);

  srslte::log_filter log;
  log.init("TEST ", &log_wrapper);
  log.set_level("debug");
  log.set_hex_limit(10000);

  srslog::init();

  srsue::gw_args_t gw_args;
  gw_args.tun_dev_name     = "tun1";
  gw_args.log.gw_level     = "debug";
  gw_args.log.gw_hex_limit = 100000;
  test_stack_dummy stack;
  srsue::gw        gw;
  gw.init(gw_args, &log_wrapper, &stack);

  uint32_t eps_bearer_id              = 5;
  uint32_t non_existing_eps_bearer_id = 23;
  uint32_t old_lcid                   = 3;
  uint32_t new_lcid                   = 4;
  char*    err_str                    = nullptr;
  int      rtn                        = 0;

  rtn = gw.setup_if_addr(
      eps_bearer_id, old_lcid, LIBLTE_MME_PDN_TYPE_IPV4, htonl(inet_addr("192.168.56.32")), nullptr, err_str);

  if (rtn != SRSLTE_SUCCESS) {
    log.error("Failed to setup GW interface. Not possible to test function. Try to execute with sudo rights.");
    gw.stop();
    return SRSLTE_SUCCESS;
  }

  TESTASSERT(gw.update_lcid(eps_bearer_id, new_lcid) == SRSLTE_SUCCESS);
  TESTASSERT(gw.update_lcid(non_existing_eps_bearer_id, new_lcid) == SRSLTE_ERROR);
  gw.stop();
  return SRSLTE_SUCCESS;
}

int main(int argc, char** argv)
{
  TESTASSERT(gw_change_lcid_test() == SRSLTE_SUCCESS);
  return SRSLTE_SUCCESS;
}
