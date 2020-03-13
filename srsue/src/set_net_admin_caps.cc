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

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

int main(int argc, char* argv[])
{
  if (argc != 2) {
    std::cout << "Please call with the binary to provide net admin capabilities to as a parameter." << std::endl;
    std::cout << "E.g. ./set_net_admin_caps myprogCalling " << std::endl;
    return -1;
  }

  std::string command("setcap 'cap_net_admin=eip' ");
  command += argv[1];

  std::cout << "Calling " << command << " with root rights." << std::endl;
  setuid(0);
  system(command.c_str());

  return 0;
}
