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

#include "srsran/adt/circular_buffer.h"
#include "srsran/common/test_common.h"

namespace srsran {

int test_static_circular_buffer()
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
  for (int& it : circ_buffer) {
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

  return SRSRAN_SUCCESS;
}

int test_queue_block_api()
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
  return SRSRAN_SUCCESS;
}

int test_queue_block_api_2()
{
  std::thread t;

  {
    dyn_blocking_queue<int> queue(100);

    t = std::thread([&queue]() {
      int count = 0;
      while (queue.push_blocking(count++)) {
      }
    });

    for (int i = 0; i < 10000; ++i) {
      TESTASSERT(queue.pop_blocking() == i);
    }

    // queue dtor called
  }

  t.join();
  return SRSRAN_SUCCESS;
}

} // namespace srsran

int main()
{
  TESTASSERT(srsran::test_static_circular_buffer() == SRSRAN_SUCCESS);
  TESTASSERT(srsran::test_queue_block_api() == SRSRAN_SUCCESS);
  TESTASSERT(srsran::test_queue_block_api_2() == SRSRAN_SUCCESS);
  srsran::console("Success\n");
  return SRSRAN_SUCCESS;
}