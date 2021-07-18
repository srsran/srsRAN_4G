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

#include "srsue/hdr/stack/upper/nas_5g_procedures.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <unistd.h>

using namespace srsran;

namespace srsue {

nas_5g::registration_procedure::registration_procedure(nas_5g_interface_procedures* parent_nas_) :
  parent_nas(parent_nas_)
{}

srsran::proc_outcome_t nas_5g::registration_procedure::init()
{
  parent_nas->send_registration_request();
  return srsran::proc_outcome_t::yield;
}
srsran::proc_outcome_t nas_5g::registration_procedure::step()
{
  return srsran::proc_outcome_t::success;
}

} // namespace srsue