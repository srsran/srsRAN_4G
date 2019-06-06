/*
 * Copyright 2013-2019 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
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

#ifndef SRSLTE_ENB_STACK_BASE_H
#define SRSLTE_ENB_STACK_BASE_H

#include <string>

namespace srsenb {

struct stack_metrics_t;

class enb_stack_base
{
public:
  virtual ~enb_stack_base() = default;

  virtual std::string get_type() = 0;

  virtual void stop() = 0;

  // eNB metrics interface
  virtual bool get_metrics(stack_metrics_t* metrics) = 0;
};

} // namespace srsenb

#endif // SRSLTE_ENB_STACK_BASE_H
