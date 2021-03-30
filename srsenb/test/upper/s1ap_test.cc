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

#include "srsenb/hdr/stack/upper/s1ap.h"
#include "srsenb/test/common/dummy_classes.h"
#include "srsran/common/test_common.h"

using namespace srsenb;

class stack_dummy : public srsenb::stack_interface_s1ap_lte
{
public:
  void add_mme_socket(int fd) {}
  void remove_mme_socket(int fd) {}
};

void test_s1ap_erab_setup()
{
  srsran::task_scheduler task_sched;
  srslog::basic_logger&  logger = srslog::fetch_basic_logger("S1AP");
  s1ap                   s1ap_obj(&task_sched, logger);
  rrc_dummy              rrc;
  stack_dummy            stack;

  s1ap_args_t args   = {};
  args.cell_id       = 0x01;
  args.enb_id        = 0x19B;
  args.mcc           = 907;
  args.mnc           = 70;
  args.s1c_bind_addr = "127.0.0.100";
  args.tac           = 7;
  args.gtp_bind_addr = "127.0.0.100";
  args.mme_addr      = "127.0.0.1";

  TESTASSERT(s1ap_obj.init(args, &rrc, &stack) == SRSRAN_SUCCESS);
}

int main(int argc, char** argv)
{
  // Setup logging.
  auto& logger = srslog::fetch_basic_logger("S1AP");
  logger.set_level(srslog::basic_levels::debug);
  logger.set_hex_dump_max_size(-1);

  // Start the log backend.
  srsran::test_init(argc, argv);

  test_s1ap_erab_setup();
}