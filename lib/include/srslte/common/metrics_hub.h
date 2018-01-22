
/******************************************************************************
 * File:        metrics_hub.h
 * Description: Centralizes metrics interfaces to allow different metrics clients
 *              to get metrics 
 *****************************************************************************/

#ifndef METRICS_HUB_H
#define METRICS_HUB_H

#include <vector>
#include "srslte/common/threads.h"

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
  virtual void set_metrics(metrics_t &m) = 0;
};

template<typename metrics_t>
class metrics_hub : public periodic_thread
{
public:
  metrics_hub()
    :m(NULL)
    ,report_period_secs(1)
  {}
  bool init(metrics_interface<metrics_t> *m_, float report_period_secs_=1.0) {
    m = m_;
    report_period_secs = report_period_secs_;
    start_periodic(report_period_secs*1e6);
    return true;
  }
  void stop() {
    thread_cancel();
    wait_thread_finish();
  }
  
  void add_listener(metrics_listener<metrics_t> *listener) {
    listeners.push_back(listener);
  }
  
private:
  void run_period() {
    if (m) {
      metrics_t metric = {};
      m->get_metrics(metric);
      for (uint32_t i=0;i<listeners.size();i++) {
        listeners[i]->set_metrics(metric);
      }
    }
  }
  metrics_interface<metrics_t> *m;
  std::vector<metrics_listener<metrics_t>*> listeners;
  float report_period_secs;
};

} // namespace srslte

#endif // METRICS_HUB_H
