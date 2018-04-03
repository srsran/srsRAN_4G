
/******************************************************************************
 * File:        metrics_hub.h
 * Description: Centralizes metrics interfaces to allow different metrics clients
 *              to get metrics 
 *****************************************************************************/

#ifndef SRSLTE_METRICS_HUB_H
#define SRSLTE_METRICS_HUB_H

#include <vector>
#include "srslte/common/threads.h"
#include "srslte/srslte.h"

namespace srslte {

template<typename metrics_t>
class metrics_interface 
{
public:
  virtual bool get_metrics(metrics_t &m) = 0; 
}; 

template<typename metrics_t>
class metrics_listener
{
public: 
  virtual void set_metrics(metrics_t &m, const uint32_t period_usec) = 0;
  virtual void stop() = 0;
};

template<typename metrics_t>
class metrics_hub : public periodic_thread
{
public:
  metrics_hub()
    :m(NULL)
    ,sleep_period_start()
  {}
  bool init(metrics_interface<metrics_t> *m_, float report_period_secs_=1.0) {
    m = m_;
    // Start with user-default priority
    start_periodic(report_period_secs_*1e6, -2);
    return true;
  }
  void stop() {
    // stop all listeners
    for (uint32_t i=0;i<listeners.size();i++) {
      listeners[i]->stop();
    }
    thread_cancel();
    wait_thread_finish();
  }
  
  void add_listener(metrics_listener<metrics_t> *listener) {
    listeners.push_back(listener);
  }
  
private:
  void run_period(){
    // get current time and check how long we slept
    gettimeofday(&sleep_period_start[2], NULL);
    get_time_interval(sleep_period_start);
    uint32_t period = sleep_period_start[0].tv_sec*1e6 + sleep_period_start[0].tv_usec;
    if (m) {
      metrics_t metric = {};
      m->get_metrics(metric);
      for (uint32_t i=0;i<listeners.size();i++) {
        listeners[i]->set_metrics(metric, period);
      }
    }
    // store start of sleep period
    gettimeofday(&sleep_period_start[1], NULL);
  }
  metrics_interface<metrics_t> *m;
  std::vector<metrics_listener<metrics_t>*> listeners;
  struct timeval sleep_period_start[3];
};

} // namespace srslte

#endif // SRSLTE_METRICS_HUB_H
