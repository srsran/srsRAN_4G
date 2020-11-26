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

#include "srslte/adt/scope_exit.h"
#include "srslte/common/test_common.h"

int test_scope_exit(int* value)
{
  auto test_exit = srslte::make_scope_exit([value]() {
    *value = SRSLTE_SUCCESS;
    printf("Finished successfully\n");
  });

  // TEST: simple scope_exit call
  int nof_calls = 0;
  {
    auto scope_exit = srslte::make_scope_exit([&]() {
      nof_calls++;
      printf("TEST1: Exited first scope\n");
    });
    printf("TEST1: Entered first scope\n");
  }
  TESTASSERT(nof_calls == 1);

  // TEST: Cancelling scope_exit call via release()
  nof_calls = 0;
  {
    auto scope_exit = srslte::make_scope_exit([&]() {
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
    auto scope_exit = srslte::make_scope_exit([&]() { nof_calls++; });
    auto scope_exit2{std::move(scope_exit)};
  }
  TESTASSERT(nof_calls == 1);

  // TEST: lvalue callback is safe
  {
    nof_calls       = 0;
    auto task       = [&nof_calls]() { nof_calls += 2; };
    auto scope_exit = srslte::make_scope_exit(task);
  }
  TESTASSERT(nof_calls == 2);

  return SRSLTE_SUCCESS;
}

int main()
{
  int ret = SRSLTE_ERROR;
  TESTASSERT(test_scope_exit(&ret) == SRSLTE_SUCCESS);
  TESTASSERT(ret == SRSLTE_SUCCESS);
}