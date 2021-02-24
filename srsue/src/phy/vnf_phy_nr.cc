/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
  vnf         = std::unique_ptr<srslte::srslte_basic_vnf>(new srslte::srslte_basic_vnf(args_.vnf_args, stack));
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
bool vnf_phy_nr::set_config(const srslte::phy_cfg_nr_t& cfg)
{
  return false;
}
void vnf_phy_nr::sr_send(uint32_t sr_id) {}

} // namespace srsue