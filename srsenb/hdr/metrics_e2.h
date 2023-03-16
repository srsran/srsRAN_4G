/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/******************************************************************************
 * File:        metrics_stdout.h
 * Description: Metrics class printing to stdout.
 *****************************************************************************/

#ifndef SRSENB_METRICS_E2_H
#define SRSENB_METRICS_E2_H

#include "srsran/interfaces/e2_metrics_interface.h"
#include "srsran/interfaces/enb_metrics_interface.h"
#include <pthread.h>
#include <queue>
#include <stdint.h>
#include <string>

#define METRICS_BUFFER_SIZE 20
namespace srsenb {

class metrics_e2 : public srsran::metrics_listener<enb_metrics_t>, public e2_interface_metrics
{
public:
  metrics_e2(enb_metrics_interface* enb_) : do_print(false) {}
  void set_metrics(const enb_metrics_t& m, const uint32_t period_usec);
  bool pull_metrics(enb_metrics_t* m) override;
  void stop(){};

private:
  std::atomic<bool>         do_print  = {false};
  uint8_t                   n_reports = 0;
  std::queue<enb_metrics_t> metrics_queue;
  enb_metrics_interface*    enb = nullptr;
};

} // namespace srsenb

#endif // SRSENB_METRICS_STDOUT_H
