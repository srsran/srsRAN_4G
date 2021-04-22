/**
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "srsran/adt/pool/fixed_size_pool.h"
#include "srsran/adt/pool/mem_pool.h"
#include "srsran/adt/pool/obj_pool.h"
#include "srsran/common/test_common.h"

class C
{
public:
  C() { default_ctor_counter++; }
  ~C() { dtor_counter++; }

  void* operator new(size_t sz);
  void* operator new(size_t sz, void*& ptr) { return ptr; }
  void  operator delete(void* ptr)noexcept;

  static std::atomic<int> default_ctor_counter;
  static std::atomic<int> dtor_counter;
};
std::atomic<int> C::default_ctor_counter(0);
std::atomic<int> C::dtor_counter(0);

srsran::big_obj_pool<C> pool;

void* C::operator new(size_t sz)
{
  return pool.allocate_node(sz);
}

void C::operator delete(void* ptr)noexcept
{
  pool.deallocate_node(ptr);
}

void test_nontrivial_obj_pool()
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
}

struct BigObj {
  C                        c;
  std::array<uint8_t, 500> space;

  using pool_t = srsran::concurrent_fixed_memory_pool<512, true>;

  void* operator new(size_t sz)
  {
    srsran_assert(sz == sizeof(BigObj), "Allocated node size and object size do not match");
    return pool_t::get_instance()->allocate_node(sizeof(BigObj));
  }
  void* operator new(size_t sz, const std::nothrow_t& nothrow_value) noexcept
  {
    srsran_assert(sz == sizeof(BigObj), "Allocated node size and object size do not match");
    return pool_t::get_instance()->allocate_node(sizeof(BigObj));
  }
  void operator delete(void* ptr) { pool_t::get_instance()->deallocate_node(ptr); }
};

void test_fixedsize_pool()
{
  size_t pool_size  = 1024;
  auto*  fixed_pool = BigObj::pool_t::get_instance(pool_size);
  fixed_pool->print_all_buffers();
  {
    std::vector<std::unique_ptr<BigObj> > vec(pool_size);
    for (size_t i = 0; i < pool_size; ++i) {
      vec[i].reset(new BigObj());
      TESTASSERT(vec[i].get() != nullptr);
    }
    std::unique_ptr<BigObj> obj(new (std::nothrow) BigObj());
    TESTASSERT(obj == nullptr);
    vec.clear();
    obj = std::unique_ptr<BigObj>(new (std::nothrow) BigObj());
    TESTASSERT(obj != nullptr);
    obj.reset();
    fixed_pool->print_all_buffers();
  }
  fixed_pool->print_all_buffers();
  TESTASSERT(C::default_ctor_counter == C::dtor_counter);

  // TEST: one thread allocates, and the other deallocates
  {
    std::unique_ptr<BigObj>                              obj;
    std::atomic<bool>                                    stop(false);
    srsran::dyn_blocking_queue<std::unique_ptr<BigObj> > queue(pool_size / 2);
    std::thread                                          t([&queue, &stop]() {
      while (not stop.load(std::memory_order_relaxed)) {
        std::unique_ptr<BigObj> obj(new (std::nothrow) BigObj());
        TESTASSERT(obj != nullptr);
        queue.try_push(std::move(obj));
      }
    });

    for (size_t i = 0; i < pool_size * 8; ++i) {
      obj = queue.pop_blocking();
      TESTASSERT(obj != nullptr);
    }
    stop.store(true);
    fixed_pool->print_all_buffers();
    t.join();
  }
  fixed_pool->print_all_buffers();
  TESTASSERT(C::default_ctor_counter == C::dtor_counter);
}

struct D : public C {
  char val = '\0';
};

void test_background_pool()
{
  C::default_ctor_counter = 0;
  C::dtor_counter         = 0;
  {
    auto init_D_val = [](void* ptr) {
      new (ptr) D();
      static_cast<D*>(ptr)->val = 'c';
    };
    srsran::background_obj_pool<D> obj_pool(16, 4, 16, init_D_val);
    TESTASSERT(obj_pool.cache_size() == 16);
    std::vector<srsran::unique_pool_ptr<D> > objs;

    for (size_t i = 0; i < 16 - 4; ++i) {
      objs.push_back(obj_pool.make());
    }
    TESTASSERT(
        std::all_of(objs.begin(), objs.end(), [](const srsran::unique_pool_ptr<D>& d) { return d->val == 'c'; }));
    TESTASSERT(C::default_ctor_counter == 16);

    // This will trigger a new batch allocation in the background
    objs.push_back(obj_pool.make());
  }
  TESTASSERT(C::dtor_counter == C::default_ctor_counter);
}

int main(int argc, char** argv)
{
  srsran::test_init(argc, argv);

  test_nontrivial_obj_pool();
  test_fixedsize_pool();
  test_background_pool();

  printf("Success\n");
  return 0;
}
