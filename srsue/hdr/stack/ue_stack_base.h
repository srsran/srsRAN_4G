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

#ifndef SRSUE_UE_STACK_BASE_H
#define SRSUE_UE_STACK_BASE_H

#include "srslte/common/logger.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "srsue/hdr/ue_metrics_interface.h"
#include "srsue/hdr/ue_stack_interface.h"
#include <memory>

namespace srsue {

class ue_stack_base : public ue_interface
{
public:
  ue_stack_base(){};
  virtual ~ue_stack_base(){};

  virtual std::string get_type() = 0;

  virtual int  init(const stack_args_t& args_, srslte::logger* logger_) = 0;
  virtual void stop()                                                   = 0;
  virtual bool switch_on()                                              = 0;
  virtual bool switch_off()                                             = 0;
  virtual bool is_rrc_connected()                                       = 0;

  // UE metrics interface
  virtual bool get_metrics(stack_metrics_t* metrics) = 0;
};

} // namespace srsue

#endif // SRSUE_UE_BASE_H
