/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

#include <iostream>
#include <srslte/common/multiqueue.h>

#define TESTASSERT(cond)                                                                                               \
  {                                                                                                                    \
    if (!(cond)) {                                                                                                     \
      std::cout << "[" << __FUNCTION__ << "][Line " << __LINE__ << "]: FAIL at " << (#cond) << std::endl;              \
      return -1;                                                                                                       \
    }                                                                                                                  \
  }

using namespace srslte;

int test_multiqueue()
{
  std::cout << "\n======= TEST multiqueue test: start =======\n";

  int number = 2;

  multiqueue_handler<int> multiqueue;
  TESTASSERT(multiqueue.nof_queues() == 0)

  // test push/pop and size for one queue
  int qid1 = multiqueue.add_queue();
  TESTASSERT(qid1 == 0 and multiqueue.is_queue_active(qid1))
  TESTASSERT(multiqueue.size(qid1) == 0 and multiqueue.empty(qid1))
  TESTASSERT(multiqueue.nof_queues() == 1)
  TESTASSERT(multiqueue.try_push(qid1, 5).first)
  TESTASSERT(multiqueue.try_push(qid1, number))
  TESTASSERT(multiqueue.size(qid1) == 2 and not multiqueue.empty(qid1))
  TESTASSERT(multiqueue.wait_pop(&number) == qid1)
  TESTASSERT(number == 5)
  TESTASSERT(multiqueue.wait_pop(&number) == qid1)
  TESTASSERT(number == 2 and multiqueue.empty(qid1) and multiqueue.size(qid1) == 0)

  // test push/pop and size for two queues
  int qid2 = multiqueue.add_queue();
  TESTASSERT(qid2 == 1)
  TESTASSERT(multiqueue.nof_queues() == 2 and multiqueue.is_queue_active(qid1))
  TESTASSERT(multiqueue.try_push(qid2, 3).first)
  TESTASSERT(multiqueue.size(qid2) == 1 and not multiqueue.empty(qid2))
  TESTASSERT(multiqueue.empty(qid1) and multiqueue.size(qid1) == 0)

  // check if erasing a queue breaks anything
  multiqueue.erase_queue(qid1);
  TESTASSERT(multiqueue.nof_queues() == 1 and not multiqueue.is_queue_active(qid1))
  qid1 = multiqueue.add_queue();
  TESTASSERT(qid1 == 0)
  TESTASSERT(multiqueue.empty(qid1) and multiqueue.is_queue_active(qid1))
  multiqueue.wait_pop(&number);

  // check round-robin
  for (int i = 0; i < 10; ++i) {
    TESTASSERT(multiqueue.try_push(qid1, i))
  }
  for (int i = 20; i < 35; ++i) {
    TESTASSERT(multiqueue.try_push(qid2, i))
  }
  TESTASSERT(multiqueue.size(qid1) == 10)
  TESTASSERT(multiqueue.size(qid2) == 15)
  TESTASSERT(multiqueue.wait_pop(&number) == qid1 and number == 0)
  TESTASSERT(multiqueue.wait_pop(&number) == qid2 and number == 20)
  TESTASSERT(multiqueue.wait_pop(&number) == qid1 and number == 1)
  TESTASSERT(multiqueue.wait_pop(&number) == qid2 and number == 21)
  TESTASSERT(multiqueue.size(qid1) == 8)
  TESTASSERT(multiqueue.size(qid2) == 13)
  for (int i = 0; i < 8 * 2; ++i) {
    multiqueue.wait_pop(&number);
  }
  TESTASSERT(multiqueue.size(qid1) == 0)
  TESTASSERT(multiqueue.size(qid2) == 5)
  TESTASSERT(multiqueue.wait_pop(&number) == qid2 and number == 30)

  std::cout << "outcome: Success\n";
  std::cout << "===========================================\n";

  return 0;
}

int main()
{
  TESTASSERT(test_multiqueue() == 0);
}
