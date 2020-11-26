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
