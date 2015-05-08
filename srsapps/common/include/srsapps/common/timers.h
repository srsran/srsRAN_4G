
#include <stdint.h>
#include "srslte/config.h"

#ifndef TIMERS_H
#define TIMERS_H

namespace srslte {
  
class SRSLTE_API timer_callback 
{
  public: 
    virtual void timer_expired(uint32_t timer_id) = 0; 
}; 
  
class SRSLTE_API timers
{
public:
  class SRSLTE_API timer
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
  
  timers(uint32_t nof_timers_) {
    nof_timers = nof_timers_; 
    timer_list = new timer[nof_timers];
    for (uint32_t i=0;i<nof_timers;i++) {
      timer_list[i].id = i; 
    }
  }
 ~timers() {
   delete timer_list; 
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
      return NULL; 
    }
  }
private:
  uint32_t nof_timers; 
  timer   *timer_list;   
};
}
  
#endif