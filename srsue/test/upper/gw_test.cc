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

#include "srslte/common/logger_srslog_wrapper.h"
#include "srslte/common/test_common.h"
#include "srslte/interfaces/ue_pdcp_interfaces.h"
#include "srslte/srslog/srslog.h"
#include "srsue/hdr/stack/upper/gw.h"

#include <arpa/inet.h>

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
  srsue::gw_args_t gw_args;
  gw_args.tun_dev_name     = "tun1";
  gw_args.log.gw_level     = "debug";
  gw_args.log.gw_hex_limit = 100000;
  test_stack_dummy stack;
  srsue::gw        gw;
  gw.init(gw_args, &stack);

  uint32_t eps_bearer_id              = 5;
  uint32_t non_existing_eps_bearer_id = 23;
  uint32_t old_lcid                   = 3;
  uint32_t new_lcid                   = 4;
  char*    err_str                    = nullptr;
  int      rtn                        = 0;

  rtn = gw.setup_if_addr(
      eps_bearer_id, old_lcid, LIBLTE_MME_PDN_TYPE_IPV4, htonl(inet_addr("192.168.56.32")), nullptr, err_str);

  if (rtn != SRSLTE_SUCCESS) {
    srslog::fetch_basic_logger("TEST", false)
        .error("Failed to setup GW interface. Not possible to test function. Try to execute with sudo rights.");
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
  srslog::init();

  TESTASSERT(gw_change_lcid_test() == SRSLTE_SUCCESS);

  return SRSLTE_SUCCESS;
}
