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