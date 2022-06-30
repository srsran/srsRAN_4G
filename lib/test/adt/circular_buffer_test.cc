/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#include "srsran/adt/circular_buffer.h"
#include "srsran/common/test_common.h"

namespace srsran {

struct C {
  C() : val_ptr(new int(5)) { count++; }
  ~C() { count--; }
  C(C&& other) : val_ptr(move(other.val_ptr)) { count++; }
  C& operator=(C&&) = default;

  std::unique_ptr<int> val_ptr;

  static size_t count;
};
size_t C::count = 0;

struct D {
  D() { count++; }
  ~D() { count--; }
  D(const D&) { count++; }
  D(D&&)     = delete;
  D& operator=(D&&) = delete;
  D& operator=(const D&) = default;

  static size_t count;
};
size_t D::count = 0;

int test_static_circular_buffer()
{
  {
    static_circular_buffer<int, 10> circ_buffer;
    TESTASSERT(circ_buffer.max_size() == 10);
    TESTASSERT(circ_buffer.empty() and not circ_buffer.full() and circ_buffer.size() == 0);

    // push until full
    for (size_t i = 0; i < circ_buffer.max_size(); ++i) {
      TESTASSERT(circ_buffer.size() == i and not circ_buffer.full());
      circ_buffer.push(i);
      TESTASSERT(not circ_buffer.empty());
    }
    TESTASSERT(circ_buffer.size() == 10 and circ_buffer.full());

    // test iterator
    int count = 0;
    for (int it : circ_buffer) {
      TESTASSERT(it == count);
      count++;
    }
    TESTASSERT(*circ_buffer.begin() == circ_buffer.top());

    // pop until empty
    for (size_t i = 0; i < circ_buffer.max_size(); ++i) {
      TESTASSERT(circ_buffer.size() == circ_buffer.max_size() - i and not circ_buffer.empty());
      TESTASSERT(circ_buffer.top() == (int)i);
      circ_buffer.pop();
    }
    TESTASSERT(circ_buffer.empty() and circ_buffer.size() == 0);

    // test iteration with wrap-around in memory
    for (size_t i = 0; i < circ_buffer.max_size(); ++i) {
      circ_buffer.push(i);
    }
    for (size_t i = 0; i < circ_buffer.max_size() / 2; ++i) {
      circ_buffer.pop();
    }
    circ_buffer.push(circ_buffer.max_size());
    circ_buffer.push(circ_buffer.max_size() + 1);
    TESTASSERT(circ_buffer.size() == circ_buffer.max_size() / 2 + 2);
    count = circ_buffer.max_size() / 2;
    for (int& it : circ_buffer) {
      TESTASSERT(it == count);
      count++;
    }
  }

  // TEST: move-only types
  {
    static_circular_buffer<C, 5> circbuffer;
    circbuffer.push(C{});
    circbuffer.push(C{});
    circbuffer.push(C{});
    circbuffer.push(C{});
    circbuffer.push(C{});
    TESTASSERT(circbuffer.full() and C::count == 5);
    C c = std::move(circbuffer.top());
    TESTASSERT(circbuffer.full() and C::count == 6);
    circbuffer.pop();
    TESTASSERT(not circbuffer.full() and C::count == 5);

    static_circular_buffer<C, 5> circbuffer2(std::move(circbuffer));
    TESTASSERT(circbuffer.empty() and circbuffer2.size() == 4);
    TESTASSERT(C::count == 5);
    circbuffer.push(C{});
    TESTASSERT(C::count == 6);
    circbuffer = std::move(circbuffer2);
    TESTASSERT(C::count == 5);
  }

  TESTASSERT(C::count == 0);
  return SRSRAN_SUCCESS;
}

void test_dyn_circular_buffer()
{
  {
    dyn_circular_buffer<int> circ_buffer(10);
    TESTASSERT(circ_buffer.max_size() == 10);
    TESTASSERT(circ_buffer.empty() and not circ_buffer.full() and circ_buffer.size() == 0);

    // push until full
    for (size_t i = 0; i < circ_buffer.max_size(); ++i) {
      TESTASSERT(circ_buffer.size() == i and not circ_buffer.full());
      circ_buffer.push(i);
      TESTASSERT(not circ_buffer.empty());
    }
    TESTASSERT(circ_buffer.size() == 10 and circ_buffer.full());

    // test iterator
    int count = 0;
    for (int it : circ_buffer) {
      TESTASSERT(it == count);
      count++;
    }
    TESTASSERT(*circ_buffer.begin() == circ_buffer.top());

    // pop until empty
    for (size_t i = 0; i < circ_buffer.max_size(); ++i) {
      TESTASSERT(circ_buffer.size() == circ_buffer.max_size() - i and not circ_buffer.empty());
      TESTASSERT(circ_buffer.top() == (int)i);
      circ_buffer.pop();
    }
    TESTASSERT(circ_buffer.empty() and circ_buffer.size() == 0);

    // test iteration with wrap-around in memory
    for (size_t i = 0; i < circ_buffer.max_size(); ++i) {
      circ_buffer.push(i);
    }
    for (size_t i = 0; i < circ_buffer.max_size() / 2; ++i) {
      circ_buffer.pop();
    }
    circ_buffer.push(circ_buffer.max_size());
    circ_buffer.push(circ_buffer.max_size() + 1);
    TESTASSERT(circ_buffer.size() == circ_buffer.max_size() / 2 + 2);
    count = circ_buffer.max_size() / 2;
    for (int& it : circ_buffer) {
      TESTASSERT(it == count);
      count++;
    }
  }

  // TEST: move-only types
  {
    dyn_circular_buffer<C> circbuffer(5);
    circbuffer.push(C{});
    circbuffer.push(C{});
    circbuffer.push(C{});
    circbuffer.push(C{});
    circbuffer.push(C{});
    TESTASSERT(circbuffer.full() and C::count == 5);
    C c = std::move(circbuffer.top());
    TESTASSERT(circbuffer.full() and C::count == 6);
    circbuffer.pop();
    TESTASSERT(not circbuffer.full() and C::count == 5);

    dyn_circular_buffer<C> circbuffer2(std::move(circbuffer));
    TESTASSERT(circbuffer.empty() and circbuffer2.size() == 4);
    TESTASSERT(C::count == 5);
    circbuffer.set_size(5);
    circbuffer.push(C{});
    TESTASSERT(C::count == 6);
    circbuffer = std::move(circbuffer2);
    TESTASSERT(C::count == 5);
  }

  // TEST: copy-only types
  {
    dyn_circular_buffer<D> circbuffer(3);
    D                      d{};
    circbuffer.push(d);
    circbuffer.push(d);
    circbuffer.push(d);
    TESTASSERT(circbuffer.full() and D::count == 4);

    dyn_circular_buffer<D> circbuffer2(circbuffer);
    TESTASSERT(circbuffer2.full() and circbuffer.full());
    TESTASSERT(D::count == 7);
    circbuffer.pop();
    circbuffer.pop();
    TESTASSERT(D::count == 5);
    circbuffer = circbuffer2;
    TESTASSERT(D::count == 7);
  }
  TESTASSERT(C::count == 0);
}

void test_queue_block_api()
{
  dyn_blocking_queue<int> queue(100);

  std::thread t([&queue]() {
    int count = 0;
    while (true) {
      int val = queue.pop_blocking();
      if (queue.is_stopped()) {
        break;
      }
      assert(val == count);
      count++;
    }
  });

  for (int i = 0; i < 10000; ++i) {
    queue.push_blocking(i);
  }

  queue.stop();
  t.join();
}

void test_queue_block_api_2()
{
  std::thread t;

  dyn_blocking_queue<int> queue(100);

  t = std::thread([&queue]() {
    int count = 0;
    while (queue.push_blocking(count++)) {
    }
  });

  for (int i = 0; i < 10000; ++i) {
    TESTASSERT(queue.pop_blocking() == i);
  }

  queue.stop();
  t.join();
}

} // namespace srsran

int main(int argc, char** argv)
{
  auto& test_log = srslog::fetch_basic_logger("TEST");
  test_log.set_level(srslog::basic_levels::info);

  srsran::test_init(argc, argv);

  TESTASSERT(srsran::test_static_circular_buffer() == SRSRAN_SUCCESS);
  srsran::test_dyn_circular_buffer();
  srsran::test_queue_block_api();
  srsran::test_queue_block_api_2();
  srsran::console("Success\n");
  return SRSRAN_SUCCESS;
}