// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

#include "ttcn3_ip_ctrl_interface.h"
#include "ttcn3_ip_sock_interface.h"
#include "ttcn3_srb_interface.h"
#include "ttcn3_sys_interface.h"
#include "ttcn3_ut_interface.h"

int if_handler_test()
{
  auto&                   logger = srslog::fetch_basic_logger("TEST", false);
  ttcn3_ut_interface      ut(logger);
  ttcn3_sys_interface     sys(logger);
  ttcn3_ip_sock_interface ip_sock(logger);
  ttcn3_ip_ctrl_interface ip_ctrl(logger);
  ttcn3_srb_interface     srb(logger);

  return SRSRAN_SUCCESS;
}

int main(int argc, char** argv)
{
  srslog::init();

  if_handler_test();

  return SRSRAN_SUCCESS;
}
