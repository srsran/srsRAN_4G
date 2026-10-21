// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

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