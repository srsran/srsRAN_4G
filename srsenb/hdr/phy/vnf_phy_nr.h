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

#ifndef SRSGNB_NR_VNF_PHY_H
#define SRSGNB_NR_VNF_PHY_H

#include "srsenb/hdr/phy/enb_phy_base.h"
#include "srsenb/hdr/phy/phy_common.h"
#include "srslte/common/basic_vnf.h"
#include "srslte/common/log.h"
#include "srslte/common/log_filter.h"
#include "srslte/interfaces/enb_metrics_interface.h"
#include "srslte/interfaces/gnb_interfaces.h"
#include "srslte/interfaces/radio_interfaces.h"

namespace srsenb {

struct nr_phy_cfg_t {
  // TODO: add cell and RRC configs
};

class vnf_phy_nr : public srsenb::enb_phy_base, public srsenb::phy_interface_stack_nr
{
public:
  vnf_phy_nr(srslte::logger* logger_);
  ~vnf_phy_nr();

  int  init(const srsenb::phy_args_t& args, const nr_phy_cfg_t& cfg, srsenb::stack_interface_phy_nr* stack_);
  void stop() override;

  std::string get_type() override { return "vnf"; };

  void start_plot() override;

  void get_metrics(srsenb::phy_metrics_t metrics[ENB_METRICS_MAX_USERS]) override;

  // MAC interface
  int dl_config_request(const dl_config_request_t& request) override;
  int tx_request(const tx_request_t& request) override;

  void cmd_cell_gain(uint32_t cell_idx, float gain_db) override
  {
    // Do nothing
  }

private:
  srslte::logger* logger = nullptr;

  std::unique_ptr<srslte::srslte_basic_vnf> vnf = nullptr;

  bool initialized = false;

  void parse_config(const nr_phy_cfg_t& cfg);
};

} // namespace srsenb

#endif // SRSGNB_NR_VNF_PHY_H