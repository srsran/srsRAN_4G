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

#include "srsran/adt/scope_exit.h"
#include "srsran/common/test_common.h"

int test_scope_exit(int* value)
{
  auto test_exit = srsran::make_scope_exit([value]() {
    *value = SRSRAN_SUCCESS;
    printf("Finished successfully\n");
  });

  // TEST: simple scope_exit call
  int nof_calls = 0;
  {
    auto scope_exit = srsran::make_scope_exit([&]() {
      nof_calls++;
      printf("TEST1: Exited first scope\n");
    });
    printf("TEST1: Entered first scope\n");
  }
  TESTASSERT(nof_calls == 1);

  // TEST: Cancelling scope_exit call via release()
  nof_calls = 0;
  {
    auto scope_exit = srsran::make_scope_exit([&]() {
      nof_calls++;
      printf("TEST2: This should not be called\n");
    });
    scope_exit.release();
    printf("TEST2: Entered second scope\n");
  }
  TESTASSERT(nof_calls == 0);

  // TEST: move is safe
  nof_calls = 0;
  {
    printf("TEST3: Entered third scope\n");
    auto scope_exit = srsran::make_scope_exit([&]() { nof_calls++; });
    auto scope_exit2{std::move(scope_exit)};
  }
  TESTASSERT(nof_calls == 1);

  // TEST: lvalue callback is safe
  {
    nof_calls       = 0;
    auto task       = [&nof_calls]() { nof_calls += 2; };
    auto scope_exit = srsran::make_scope_exit(task);
  }
  TESTASSERT(nof_calls == 2);

  return SRSRAN_SUCCESS;
}

int main()
{
  int ret = SRSRAN_ERROR;
  TESTASSERT(test_scope_exit(&ret) == SRSRAN_SUCCESS);
  TESTASSERT(ret == SRSRAN_SUCCESS);
}