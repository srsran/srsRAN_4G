/**
 * Copyright 2013-2022 Software Radio Systems Limited
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
 * File:        metrics_stdout.h
 * Description: Metrics class printing to stdout.
 *****************************************************************************/

#ifndef SRSENB_METRICS_STDOUT_H
#define SRSENB_METRICS_STDOUT_H

#include <pthread.h>
#include <stdint.h>
#include <string>

#include "srsran/interfaces/enb_metrics_interface.h"

namespace srsenb {

class metrics_stdout : public srsran::metrics_listener<enb_metrics_t>
{
public:
  metrics_stdout();

  void toggle_print(bool b);
  void set_metrics(const enb_metrics_t& m, const uint32_t period_usec);
  void set_handle(enb_metrics_interface* enb_);
  void stop(){};

private:
  void set_metrics_helper(uint32_t num_ue, const mac_metrics_t& mac, const std::vector<phy_metrics_t>& phy, bool is_nr);
  std::string float_to_string(float f, int digits, int field_width = 6);
  std::string float_to_eng_string(float f, int digits);

  std::atomic<bool>      do_print  = {false};
  uint8_t                n_reports = 0;
  enb_metrics_interface* enb       = nullptr;
};

} // namespace srsenb

#endif // SRSENB_METRICS_STDOUT_H
