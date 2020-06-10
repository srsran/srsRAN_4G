/*
 * Copyright 2013-2020 Software Radio Systems Limited
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

#include <pthread.h>
#include <sstream>
#include <string.h>
#include <string>
#include <strings.h>
#include <sys/mman.h>
#include <unistd.h>

#include "srslte/common/basic_vnf_api.h"
#include "srslte/common/log.h"
#include "srslte/common/test_common.h"
#include "srslte/common/threads.h"
#include "srsue/hdr/phy/vnf_phy_nr.h"

using namespace std;

namespace srsue {

vnf_phy_nr::vnf_phy_nr(srslte::logger* logger_) : logger(logger_) {}

vnf_phy_nr::~vnf_phy_nr()
{
  stop();
}

int vnf_phy_nr::init(const srsue::phy_args_t& args_, srsue::stack_interface_phy_nr* stack_)
{
  stack = stack_;
  return init(args_);
}

int vnf_phy_nr::init(const srsue::phy_args_t& args_)
{
  // create VNF
  vnf         = std::unique_ptr<srslte::srslte_basic_vnf>(new srslte::srslte_basic_vnf(args_.vnf_args, logger, stack));
  initialized = true;
  return SRSLTE_SUCCESS;
}

void vnf_phy_nr::set_earfcn(std::vector<uint32_t> earfcns) {}

void vnf_phy_nr::stop()
{
  if (initialized) {
    vnf->stop();
    initialized = false;
  }
}

// Start GUI
void vnf_phy_nr::start_plot() {}

void vnf_phy_nr::wait_initialize() {}

void vnf_phy_nr::get_metrics(phy_metrics_t* m) {}

int vnf_phy_nr::tx_request(const tx_request_t& request)
{
  // send Tx request over basic API
  return vnf->tx_request(request);
}

} // namespace srsue