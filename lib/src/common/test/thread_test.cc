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

#include <iostream>
#include <srsran/common/threads.h>

class thread_test : public srsran::thread
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