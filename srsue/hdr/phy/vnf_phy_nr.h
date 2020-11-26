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