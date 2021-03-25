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

#include "srsran/adt/pool/fixed_size_pool.h"
#include "srsran/adt/pool/mem_pool.h"
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

struct BigObj {
  C                        c;
  std::array<uint8_t, 500> space;

  using pool_t = srsran::concurrent_fixed_memory_pool<1024, 512>;

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
  {
    std::vector<std::unique_ptr<BigObj> > vec(BigObj::pool_t::size());
    for (size_t i = 0; i < BigObj::pool_t::size(); ++i) {
      vec[i].reset(new BigObj());
      TESTASSERT(vec[i].get() != nullptr);
    }
    std::unique_ptr<BigObj> obj(new (std::nothrow) BigObj());
    TESTASSERT(obj == nullptr);
    vec.clear();
    obj = std::unique_ptr<BigObj>(new (std::nothrow) BigObj());
    TESTASSERT(obj != nullptr);
    obj.reset();
  }

  // TEST: one thread allocates, and the other deallocates
  {
    std::unique_ptr<BigObj>                              obj;
    std::atomic<bool>                                    stop(false);
    srsran::dyn_blocking_queue<std::unique_ptr<BigObj> > queue(BigObj::pool_t::size() / 2);
    std::thread                                          t([&queue, &stop]() {
      while (not stop.load(std::memory_order_relaxed)) {
        std::unique_ptr<BigObj> obj(new (std::nothrow) BigObj());
        TESTASSERT(obj != nullptr);
        queue.push_blocking(std::move(obj));
      }
    });

    for (size_t i = 0; i < BigObj::pool_t::size() * 8; ++i) {
      obj = queue.pop_blocking();
      TESTASSERT(obj != nullptr);
    }
    stop.store(true);
    t.join();
  }
}

int main()
{
  TESTASSERT(test_nontrivial_obj_pool() == SRSRAN_SUCCESS);
  test_fixedsize_pool();

  srsran::console("Success\n");
  return 0;
}