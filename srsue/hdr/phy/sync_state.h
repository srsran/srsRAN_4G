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

#ifndef SRSUE_SYNC_STATE_H
#define SRSUE_SYNC_STATE_H

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
    std::lock_guard<std::mutex> lock(inside);
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
    std::lock_guard<std::mutex> lock(inside);
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
    std::lock_guard<std::mutex> lock(inside);
    next_state = SFN_SYNC;
  }

  /* Functions to be called from outside the STM thread to instruct the STM to switch state.
   * The functions change the state and wait until it has changed it.
   *
   * These functions are mutexed and only 1 can be called at a time
   */
  void go_idle()
  {
    std::lock_guard<std::mutex> lock(outside);
    // Do not wait when transitioning to IDLE to avoid blocking
    go_state_nowait(IDLE);
  }
  void run_cell_search()
  {
    std::lock_guard<std::mutex> lock(outside);
    go_state(CELL_SEARCH);
    wait_state_run();
    wait_state_next();
  }
  void run_sfn_sync()
  {
    std::lock_guard<std::mutex> lock(outside);
    go_state(SFN_SYNC);
    wait_state_run();
    wait_state_next();
  }

  /* Helpers below this */
  bool is_idle() { return cur_state == IDLE; }
  bool is_camping() { return cur_state == CAMPING; }

  const char* to_string()
  {
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
    std::unique_lock<std::mutex> ul(inside);
    next_state    = s;
    state_setting = true;
    while (state_setting) {
      cvar.wait(ul);
    }
  }

  void go_state_nowait(state_t s)
  {
    std::unique_lock<std::mutex> ul(inside);
    next_state    = s;
    state_setting = true;
  }

  /* Waits until there is a call to set_state() and then run_state(). Returns when run_state() returns */
  void wait_state_run()
  {
    std::unique_lock<std::mutex> ul(inside);
    while (state_running) {
      cvar.wait(ul);
    }
  }
  void wait_state_next()
  {
    std::unique_lock<std::mutex> ul(inside);
    while (cur_state != next_state) {
      cvar.wait(ul);
    }
  }

  bool                    state_running = false;
  bool                    state_setting = false;
  state_t                 cur_state     = IDLE;
  state_t                 next_state    = IDLE;
  std::mutex              inside;
  std::mutex              outside;
  std::condition_variable cvar;
};

}; // namespace srsue

#endif // SRSUE_SYNC_STATE_H
