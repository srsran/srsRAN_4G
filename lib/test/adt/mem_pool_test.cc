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

#include "srsran/adt/mem_pool.h"
#include "srsran/common/test_common.h"

class C
{
public:
  C() { default_ctor_counter++; }
  ~C() { dtor_counter++; }

  void* operator new(size_t sz);
  void  operator delete(void* ptr)noexcept;

  static int default_ctor_counter;
  static int dtor_counter;
};
int C::default_ctor_counter = 0;
int C::dtor_counter         = 0;

srsran::big_obj_pool<C> pool;

void* C::operator new(size_t sz)
{
  return pool.allocate_node(sz);
}

void C::operator delete(void* ptr)noexcept
{
  pool.deallocate_node(ptr);
}

int test_nontrivial_obj_pool()
{
  // No object creation on reservation
  {
    pool.reserve(10);
  }
  TESTASSERT(C::default_ctor_counter == 0);
  TESTASSERT(C::dtor_counter == 0);

  // default Ctor/Dtor are correctly called
  {
    pool.clear();
    pool.reserve(10);

    std::unique_ptr<C> c(new C{});
  }
  TESTASSERT(C::default_ctor_counter == 1);
  TESTASSERT(C::dtor_counter == 1);

  // move of unique_ptr is correctly called
  C::default_ctor_counter = 0;
  C::dtor_counter         = 0;
  {
    pool.clear();
    pool.reserve(10);

    std::unique_ptr<C> c(new C{});
    auto               c2 = std::move(c);
  }
  TESTASSERT(C::default_ctor_counter == 1);
  TESTASSERT(C::dtor_counter == 1);

  return SRSRAN_SUCCESS;
}

int main()
{
  TESTASSERT(test_nontrivial_obj_pool() == SRSRAN_SUCCESS);
  srsran::console("Success\n");
  return 0;
}