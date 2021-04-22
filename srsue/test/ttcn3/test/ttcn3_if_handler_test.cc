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
