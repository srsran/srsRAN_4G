/*
 * Copyright 2013-2020 Software Radio Systems Limited
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
