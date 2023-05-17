/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#include "srsran/adt/pool/cached_alloc.h"
#include "srsran/common/test_common.h"
#include <chrono>

void test_cached_deque_basic_operations()
{
  srsran::deque<int> my_deque;
  TESTASSERT(my_deque.empty() and my_deque.size() == 0);
  my_deque.push_front(0);
  my_deque.push_back(1);
  TESTASSERT(my_deque.front() == 0 and my_deque.back() == 1);
  TESTASSERT(my_deque.size() == 2);

  srsran::deque<int> my_deque2(my_deque);
  TESTASSERT(my_deque == my_deque2);
  my_deque.clear();
  TESTASSERT(my_deque != my_deque2);
  TESTASSERT(my_deque2.size() == 2 and my_deque2.back() == 1);
  TESTASSERT(my_deque.empty());

  my_deque = my_deque2;
  TESTASSERT(my_deque == my_deque2);
  my_deque2.clear();
  TESTASSERT(my_deque2.empty());

  my_deque2 = std::move(my_deque);
  TESTASSERT(my_deque.empty() and my_deque2.size() == 2);
}

struct C {
  C()             = default;
  C(C&&) noexcept = default;
  C(const C&)     = delete;
  C& operator=(C&&) noexcept = default;
  C& operator=(const C&) = delete;

  bool operator==(const C& other) { return true; }
};

void test_cached_queue_basic_operations()
{
  srsran::queue<C> my_queue;
  TESTASSERT(my_queue.empty());
  my_queue.push(C{});
  TESTASSERT(my_queue.size() == 1);

  srsran::queue<C> my_queue2(std::move(my_queue));
  TESTASSERT(my_queue2.size() == 1);
}

void cached_deque_benchmark()
{
  using std::chrono::high_resolution_clock;
  using std::chrono::microseconds;

  srsran::queue<int>                my_deque;
  std::queue<int>                   std_deque;
  high_resolution_clock::time_point tp;

  size_t N = 10000000, n_elems = 10;

  for (size_t i = 0; i < n_elems; ++i) {
    my_deque.push(i);
    std_deque.push(i);
  }

  // NOTE: this benchmark doesnt account for when memory is fragmented
  tp = high_resolution_clock::now();
  for (size_t i = n_elems; i < N; ++i) {
    std_deque.push(i);
    std_deque.pop();
  }
  microseconds t_std = std::chrono::duration_cast<microseconds>(high_resolution_clock::now() - tp);

  tp = high_resolution_clock::now();
  for (size_t i = n_elems; i < N; ++i) {
    my_deque.push(i);
    my_deque.pop();
  }
  microseconds t_cached = std::chrono::duration_cast<microseconds>(high_resolution_clock::now() - tp);

  fmt::print("Time elapsed: cached alloc={} usec, std alloc={} usec\n", t_cached.count(), t_std.count());
  fmt::print("queue sizes: {} {}\n", my_deque.size(), std_deque.size());
}

int main()
{
  test_cached_deque_basic_operations();
  test_cached_queue_basic_operations();
  cached_deque_benchmark();
  return 0;
}