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

  int qid2 = multiqueue.add_queue();
  TESTASSERT(qid2 == 1)
  TESTASSERT(multiqueue.nof_queues() == 2 and multiqueue.is_queue_active(qid1))
  TESTASSERT(multiqueue.try_push(qid2, 3).first)
  TESTASSERT(multiqueue.size(qid2) == 1 and not multiqueue.empty(qid2))
  TESTASSERT(multiqueue.empty(qid1) and multiqueue.size(qid1) == 0)

  multiqueue.erase_queue(qid1);
  TESTASSERT(multiqueue.nof_queues() == 1 and not multiqueue.is_queue_active(qid1))
  qid1 = multiqueue.add_queue();
  TESTASSERT(qid1 == 0)
  TESTASSERT(multiqueue.empty(qid1) and multiqueue.is_queue_active(qid1))
  TESTASSERT(multiqueue.try_push(qid1, number))
  TESTASSERT(multiqueue.size(qid1) == 1)

  std::cout << "outcome: Success\n";
  std::cout << "===========================================\n";

  return 0;
}

int main()
{
  TESTASSERT(test_multiqueue() == 0);
}
