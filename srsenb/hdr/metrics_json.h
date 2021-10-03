/**
 * Copyright 2013-2021 Software Radio Systems Limited
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
