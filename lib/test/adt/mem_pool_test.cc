/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#include "srslte/adt/mem_pool.h"
#include "srslte/common/test_common.h"

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

srslte::big_obj_pool<C> pool;

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

  return SRSLTE_SUCCESS;
}

int main()
{
  TESTASSERT(test_nontrivial_obj_pool() == SRSLTE_SUCCESS);
  srslte::console("Success\n");
  return 0;
}