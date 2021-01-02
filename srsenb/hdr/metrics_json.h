/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/******************************************************************************
 * File:        metrics_json.h
 * Description: Metrics class printing to a json file.
 *****************************************************************************/

#ifndef SRSENB_METRICS_JSON_H
#define SRSENB_METRICS_JSON_H

#include "srslte/interfaces/enb_metrics_interface.h"
#include "srslte/srslog/log_channel.h"

namespace srsenb {

class metrics_json : public srslte::metrics_listener<enb_metrics_t>
{
public:
  metrics_json(srslog::log_channel& c) : log_c(c) {}

  void set_metrics(const enb_metrics_t& m, const uint32_t period_usec) override;
  void set_handle(enb_metrics_interface* enb_);
  void stop() override {}

private:
  srslog::log_channel&   log_c;
  enb_metrics_interface* enb;
};

} // namespace srsenb

#endif // SRSENB_METRICS_JSON_H
