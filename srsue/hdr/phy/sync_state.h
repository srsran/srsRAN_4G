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

#ifndef SRSUE_SYNC_STATE_H
#define SRSUE_SYNC_STATE_H

#include <condition_variable>
#include <mutex>

namespace srsue {

class sync_state
{
public:
  typedef enum {
    IDLE = 0,
    CELL_SEARCH,
    SFN_SYNC,
    CAMPING,
  } state_t;

  /* Run_state is called by the main thread at the start of each loop. It updates the state
   * and returns the current state
   */
  state_t run_state()
  {
    std::lock_guard<std::mutex> lock(mutex);
    cur_state = next_state;
    if (state_setting) {
      state_setting = false;
      state_running = true;
    }
    cvar.notify_all();
    return cur_state;
  }

  // Called by the main thread at the end of each state to indicate it has finished.
  void state_exit(bool exit_ok = true)
  {
    std::lock_guard<std::mutex> lock(mutex);
    if (cur_state == SFN_SYNC && exit_ok == true) {
      next_state = CAMPING;
    } else {
      next_state = IDLE;
    }
    state_running = false;
    cvar.notify_all();
  }
  void force_sfn_sync()
  {
    std::lock_guard<std::mutex> lock(mutex);
    next_state = SFN_SYNC;
  }

  state_t get_state()
  {
    std::lock_guard<std::mutex> lock(mutex);
    return cur_state;
  }

  /* Functions to be called from outside the STM thread to instruct the STM to switch state.
   * The functions change the state and wait until it has changed it.
   *
   * These functions are mutexed and only 1 can be called at a time
   */
  void go_idle()
  {
    // Do not wait when transitioning to IDLE to avoid blocking
    next_state = IDLE;
  }
  void run_cell_search()
  {
    go_state(CELL_SEARCH);
    wait_state_run();
    wait_state_next();
  }
  void run_sfn_sync()
  {
    go_state(SFN_SYNC);
    wait_state_run();
    wait_state_next();
  }

  /* Helpers below this */
  bool is_idle()
  {
    std::lock_guard<std::mutex> lock(mutex);
    return cur_state == IDLE;
  }
  bool is_camping()
  {
    std::lock_guard<std::mutex> lock(mutex);
    return cur_state == CAMPING;
  }
  bool wait_idle(uint32_t timeout_ms)
  {
    std::unique_lock<std::mutex> lock(mutex);

    // Avoid wasting time if the next state will not be IDLE
    if (next_state != IDLE) {
      return cur_state == IDLE;
    }

    // Calculate timeout
    std::chrono::system_clock::time_point expire_time = std::chrono::system_clock::now();
    expire_time += std::chrono::milliseconds(timeout_ms);

    // Wait until the state is IDLE
    while (cur_state != IDLE) {
      std::cv_status cv_status = cvar.wait_until(lock, expire_time);

      // Return if it timeouts
      if (cv_status != std::cv_status::timeout) {
        return cur_state == IDLE;
      }
    }

    // Return true if the state is IDLE
    return true;
  }

  const char* to_string()
  {
    std::lock_guard<std::mutex> lock(mutex);
    switch (cur_state) {
      case IDLE:
        return "IDLE";
      case CELL_SEARCH:
        return "SEARCH";
      case SFN_SYNC:
        return "SYNC";
      case CAMPING:
        return "CAMPING";
      default:
        return "UNKNOWN";
    }
  }

  sync_state() = default;

private:
  void go_state(state_t s)
  {
    std::unique_lock<std::mutex> ul(mutex);
    next_state    = s;
    state_setting = true;
    while (state_setting) {
      cvar.wait(ul);
    }
  }

  void go_state_nowait(state_t s)
  {
    std::unique_lock<std::mutex> ul(mutex);
    next_state    = s;
    state_setting = true;
  }

  /* Waits until there is a call to set_state() and then run_state(). Returns when run_state() returns */
  void wait_state_run()
  {
    std::unique_lock<std::mutex> ul(mutex);
    while (state_running) {
      cvar.wait(ul);
    }
  }
  void wait_state_next()
  {
    std::unique_lock<std::mutex> ul(mutex);
    while (cur_state != next_state) {
      cvar.wait(ul);
    }
  }

  bool                    state_running = false;
  bool                    state_setting = false;
  std::atomic<state_t>    next_state    = {IDLE}; // can be updated from outside (i.e. other thread)
  state_t                 cur_state     = IDLE;   // will only be accessed when holding the mutex
  std::mutex              mutex;
  std::condition_variable cvar;
};

}; // namespace srsue

#endif // SRSUE_SYNC_STATE_H
