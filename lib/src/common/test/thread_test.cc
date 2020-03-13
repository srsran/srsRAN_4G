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
#include <srslte/common/threads.h>

class thread_test : public srslte::thread
{
public:
  thread_test() : thread("Thread Test") {}

protected:
  void run_thread() override { std::cout << "Hello world!" << std::endl; }
};

int main(int argc, char** argv)
{
  // Declare thread
  thread_test mythread;

  // Start
  mythread.start(0);

  // Wait to finish, join
  mythread.wait_thread_finish();

  return 0;
}