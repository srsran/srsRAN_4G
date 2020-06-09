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

#ifndef SRSUE_VNF_PHY_NR_H
#define SRSUE_VNF_PHY_NR_H

#include "srsenb/hdr/phy/phy_common.h"
#include "srslte/common/basic_vnf.h"
#include "srslte/common/log.h"
#include "srslte/common/log_filter.h"
#include "srslte/interfaces/enb_metrics_interface.h"
#include "srslte/interfaces/radio_interfaces.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "srslte/interfaces/ue_nr_interfaces.h"
#include "srsue/hdr/phy/ue_phy_nr_base.h"

namespace srsue {

class vnf_phy_nr : public srsue::ue_phy_base, public srsue::phy_interface_stack_nr
{
public:
  vnf_phy_nr(srslte::logger* logger_);
  ~vnf_phy_nr();

  int init(const srsue::phy_args_t& args, srsue::stack_interface_phy_nr* stack_);

  int init(const srsue::phy_args_t& args_);

  void set_earfcn(std::vector<uint32_t> earfcns);

  void stop();

  void wait_initialize();
  void get_metrics(phy_metrics_t* m);

  std::string get_type() { return "vnf_nr"; };

  void start_plot();

  // MAC interface
  int tx_request(const tx_request_t& request);

private:
  srslte::logger* logger = nullptr;

  std::unique_ptr<srslte::srslte_basic_vnf> vnf;

  srsue::stack_interface_phy_nr* stack = nullptr;

  bool initialized = false;
};

} // namespace srsue

#endif // SRSUE_VNF_PHY_NR_H