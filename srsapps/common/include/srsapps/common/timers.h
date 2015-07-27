/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
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


#include <stdint.h>
#include <vector>


#ifndef TIMERS_H
#define TIMERS_H

namespace srslte {
  
class timer_callback 
{
  public: 
    virtual void timer_expired(uint32_t timer_id) = 0; 
}; 
  
class timers
{
public:
  class timer
  {
  public:
    timer() { counter = 0; timeout = 0; running = false; }; 
    void set(timer_callback *callback_, uint32_t timeout_) {
      callback = callback_; 
      timeout = timeout_; 
    }
    bool is_running() {
      return counter < timeout; 
    }
    bool is_expired() {
      return counter == timeout; 
    }
    void reset() {
      counter = 0; 
    }
    void step() {
      if (running) {
        counter++; 
        if (is_expired()) {
          callback->timer_expired(id); 
        }        
      }
    }
    void stop() {
      running = false; 
    }
    void run() {
      running = true; 
    }
    uint32_t id; 
  private: 
    timer_callback *callback; 
    uint32_t timeout; 
    uint32_t counter; 
    bool running; 
  };
  
  timers(uint32_t nof_timers_) : timer_list(nof_timers_) {
    nof_timers = nof_timers_; 
    for (uint32_t i=0;i<nof_timers;i++) {
      timer_list[i].id = i; 
    }
  }
  
  void step_all() {
    for (int i=0;i<nof_timers;i++) {
      get(i)->step();
    }
  }
  void stop_all() {
    for (int i=0;i<nof_timers;i++) {
      get(i)->stop();
    }
  }
  void run_all() {
    for (int i=0;i<nof_timers;i++) {
      get(i)->run();
    }
  }
  void reset_all() {
    for (int i=0;i<nof_timers;i++) {
      get(i)->reset();
    }
  }
  timer *get(uint32_t i) {
    if (i < nof_timers) {
      return &timer_list[i];       
    } else {
      printf("Error accessing invalid timer %d (Only %d timers available)\n", i, nof_timers);
      return NULL; 
    }
  }
private:
  uint32_t nof_timers; 
  std::vector<timer>   timer_list;   
};
}
  
#endif