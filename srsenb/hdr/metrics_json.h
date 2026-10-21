// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

/******************************************************************************
 * File:        metrics_json.h
 * Description: Metrics class printing to a json file.
 *****************************************************************************/

#ifndef SRSENB_METRICS_JSON_H
#define SRSENB_METRICS_JSON_H

#include "srsran/interfaces/enb_metrics_interface.h"
#include "srsran/srslog/log_channel.h"

namespace srsenb {

class metrics_json : public srsran::metrics_listener<enb_metrics_t>
{
public:
  metrics_json(srslog::log_channel& c, enb_metrics_interface* enb_) : log_c(c), enb(enb_) {}

  void set_metrics(const enb_metrics_t& m, const uint32_t period_usec) override;
  void stop() override {}

private:
  srslog::log_channel&   log_c;
  enb_metrics_interface* enb;
};

} // namespace srsenb

#endif // SRSENB_METRICS_JSON_H
