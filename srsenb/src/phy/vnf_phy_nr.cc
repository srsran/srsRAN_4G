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

#include <string>
#include <sys/mman.h>

#include "srsenb/hdr/phy/vnf_phy_nr.h"
#include "srsran/common/basic_vnf_api.h"

using namespace std;

namespace srsenb {

vnf_phy_nr::~vnf_phy_nr()
{
  stop();
}

void vnf_phy_nr::parse_config(const nr_phy_cfg_t& cfg) {}

int vnf_phy_nr::init(const srsenb::phy_args_t& args, const nr_phy_cfg_t& cfg, srsenb::stack_interface_phy_nr* stack_)
{
  mlockall(MCL_CURRENT | MCL_FUTURE);

  // create VNF
  vnf = std::unique_ptr<srsran::srsran_basic_vnf>(new srsran::srsran_basic_vnf(args.vnf_args, stack_));

  initialized = true;

  return SRSRAN_SUCCESS;
}

void vnf_phy_nr::stop()
{
  if (initialized) {
    vnf->stop();
    initialized = false;
  }
}

// Start GUI
void vnf_phy_nr::start_plot() {}

void vnf_phy_nr::get_metrics(std::vector<srsenb::phy_metrics_t>& metrics) {}

int vnf_phy_nr::dl_config_request(const dl_config_request_t& request)
{
  // prepare DL config request over basic API and send
  return vnf->dl_config_request(request);
}

int vnf_phy_nr::tx_request(const tx_request_t& request)
{
  // send Tx request over basic API
  return vnf->tx_request(request);
}

} // namespace srsenb
