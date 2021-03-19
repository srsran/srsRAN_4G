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

#ifndef SRSGNB_NR_VNF_PHY_H
#define SRSGNB_NR_VNF_PHY_H

#include "srsenb/hdr/phy/enb_phy_base.h"
#include "srsenb/hdr/phy/phy_common.h"
#include "srsran/common/basic_vnf.h"
#include "srsran/interfaces/enb_metrics_interface.h"
#include "srsran/interfaces/gnb_interfaces.h"
#include "srsran/interfaces/radio_interfaces.h"

namespace srsenb {

struct nr_phy_cfg_t {
  // TODO: add cell and RRC configs
};

class vnf_phy_nr : public srsenb::enb_phy_base, public srsenb::phy_interface_stack_nr
{
public:
  vnf_phy_nr() = default;
  ~vnf_phy_nr();

  int  init(const srsenb::phy_args_t& args, const nr_phy_cfg_t& cfg, srsenb::stack_interface_phy_nr* stack_);
  void stop() override;

  std::string get_type() override { return "vnf"; };

  void start_plot() override;

  void get_metrics(std::vector<srsenb::phy_metrics_t>& metrics) override;

  // MAC interface
  int dl_config_request(const dl_config_request_t& request) override;
  int tx_request(const tx_request_t& request) override;

  void cmd_cell_gain(uint32_t cell_idx, float gain_db) override
  {
    // Do nothing
  }

private:
  std::unique_ptr<srsran::srsran_basic_vnf> vnf = nullptr;

  bool initialized = false;

  void parse_config(const nr_phy_cfg_t& cfg);
};

} // namespace srsenb

#endif // SRSGNB_NR_VNF_PHY_H
