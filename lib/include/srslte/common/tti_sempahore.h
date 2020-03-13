/*
 * Copyright 2013-2020 Software Radio Systems Limited
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

/**
 * Implements priority semaphore based on a FIFO queue wait . This class enqueues T type element identifiers (method
 * push) and waits until the enqueued object is the first (method wait). The first element is released by method
 * release. The method release_all waits for all the elements to be released.
 *
 * @tparam T Object identifier type
 */
template <class T>
class tti_semaphore
{
private:
  std::mutex              mutex; ///< Used for scope mutexes
  std::condition_variable cvar;  ///< Used for notifying element identifier releases
  std::deque<T>           fifo;  ///< Queue to keep order

public:
  /**
   * Waits for the first element of the queue match the element identifier provided.
   *
   * @param id the element identifier
   */
  void wait(T id)
  {
    std::unique_lock<std::mutex> lock(mutex);

    // While the FIFO is not empty and the front ID does not match the provided element identifier, keep waiting
    while (not fifo.empty() and fifo.front() != id) {
      // Wait for a release
      cvar.wait(lock);
    }
  }

  /**
   * Enqueue (push) an element identifier to the queue
   *
   * @param id the element identifier
   */
  void push(T id)
  {
    std::unique_lock<std::mutex> lock(mutex);

    // Append the element identifier
    fifo.push_back(id);
  }

  /**
   * Releases (pops) the first element
   */
  void release()
  {
    std::unique_lock<std::mutex> lock(mutex);

    // Pop first element
    fifo.pop_front();

    // Notify release
    cvar.notify_all();
  }

  /**
   * Waits for all the elements to be released
   */
  void wait_all()
  {
    std::unique_lock<std::mutex> lock(mutex);

    // Wait until the FIFO is empty
    while (not fifo.empty()) {
      cvar.wait(lock);
    }
  }
};
} // namespace srslte

#endif // SRSLTE_TTI_SEMPAHORE_H_
