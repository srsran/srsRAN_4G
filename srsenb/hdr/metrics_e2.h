/**
 * Copyright 2013-2023 Software Radio Systems Limited
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
 * File:        metrics_e2.h
 * Description: Metrics class passing to E2 agent.
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
  void set_metrics(const enb_metrics_t& m, const uint32_t period_usec) override;
  bool pull_metrics(enb_metrics_t* m) override;
  void stop() override{};

  bool register_e2sm(e2sm* sm) override;
  bool unregister_e2sm(e2sm* sm) override;

private:
  std::atomic<bool>         do_print = {false};
  std::queue<enb_metrics_t> metrics_queue;
  enb_metrics_interface*    enb = nullptr;
  std::vector<e2sm*>        e2sm_vec;
};

} // namespace srsenb

#endif // SRSENB_METRICS_STDOUT_H
