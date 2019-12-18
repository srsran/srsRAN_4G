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

#include <assert.h>
#include <condition_variable>
#include <deque>
#include <inttypes.h>
#include <mutex>

#ifndef SRSLTE_TTI_SEMPAHORE_H_
#define SRSLTE_TTI_SEMPAHORE_H_

namespace srslte {
template <class T>
class tti_semaphore
{
private:
  std::mutex              mutex;
  std::condition_variable cvar;
  std::deque<T>           fifo;

public:
  void wait(T id)
  {
    std::unique_lock<std::mutex> lock(mutex);
    while (!fifo.empty() && fifo.front() != id) {
      cvar.wait(lock);
    }
  }

  void push(T id)
  {
    std::unique_lock<std::mutex> lock(mutex);
    fifo.push_back(id);
  }

  void release()
  {
    std::unique_lock<std::mutex> lock(mutex);
    fifo.pop_front();
    cvar.notify_all();
  }

  void wait_all()
  {
    while (!fifo.empty()) {
      wait(fifo.front());
    }
  }
};
} // namespace srslte

#endif // SRSLTE_TTI_SEMPAHORE_H_
