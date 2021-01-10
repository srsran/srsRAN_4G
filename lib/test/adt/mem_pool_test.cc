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

#include "srslte/adt/mem_pool.h"
#include "srslte/common/test_common.h"

class C
{
public:
  C() { default_ctor_counter++; }
  ~C() { dtor_counter++; }

  static int default_ctor_counter;
  static int dtor_counter;
};
int C::default_ctor_counter = 0;
int C::dtor_counter         = 0;

int test_nontrivial_obj_pool()
{
  // No object creation on reservation
  {
    srslte::obj_pool<C> pool;
    pool.reserve(10);
  }
  TESTASSERT(C::default_ctor_counter == 0);
  TESTASSERT(C::dtor_counter == 0);

  // default Ctor/Dtor are correctly called
  {
    srslte::obj_pool<C> pool;
    pool.reserve(10);

    srslte::unique_pool_obj<C> c = pool.make();
  }
  TESTASSERT(C::default_ctor_counter == 1);
  TESTASSERT(C::dtor_counter == 1);

  // move of unique_ptr is correctly called
  C::default_ctor_counter = 0;
  C::dtor_counter         = 0;
  {
    srslte::obj_pool<C> pool;
    pool.reserve(10);

    srslte::unique_pool_obj<C> c  = pool.make();
    srslte::unique_pool_obj<C> c2 = std::move(c);
  }
  TESTASSERT(C::default_ctor_counter == 1);
  TESTASSERT(C::dtor_counter == 1);

  return SRSLTE_SUCCESS;
}

int main()
{
  TESTASSERT(test_nontrivial_obj_pool() == SRSLTE_SUCCESS);
  srslte::console("Success\n");
  return 0;
}