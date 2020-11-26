/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ttcn3_ip_ctrl_interface.h"
#include "ttcn3_ip_sock_interface.h"
#include "ttcn3_srb_interface.h"
#include "ttcn3_sys_interface.h"
#include "ttcn3_ut_interface.h"

int if_handler_test()
{
  ttcn3_ut_interface      ut;
  ttcn3_sys_interface     sys;
  ttcn3_ip_sock_interface ip_sock;
  ttcn3_ip_ctrl_interface ip_ctrl;
  ttcn3_srb_interface     srb;

  return SRSLTE_SUCCESS;
}

int main(int argc, char** argv)
{
  if_handler_test();

  return SRSLTE_SUCCESS;
}
