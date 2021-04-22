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

#ifndef SRSUE_VNF_PHY_NR_H
#define SRSUE_VNF_PHY_NR_H

#include "srsenb/hdr/phy/phy_common.h"
#include "srsran/common/basic_vnf.h"
#include "srsran/interfaces/enb_metrics_interface.h"
#include "srsran/interfaces/radio_interfaces.h"
#include "srsran/interfaces/ue_interfaces.h"
#include "srsran/interfaces/ue_nr_interfaces.h"
#include "srsue/hdr/phy/ue_nr_phy_base.h"

namespace srsue {

class vnf_phy_nr : public srsue::ue_phy_base, public srsue::phy_interface_stack_nr
{
public:
  vnf_phy_nr() = default;
  ~vnf_phy_nr();

  int init(const srsue::phy_args_t& args, srsue::stack_interface_phy_nr* stack_);

  int init(const srsue::phy_args_t& args_) override;

  void set_earfcn(std::vector<uint32_t> earfcns);

  void stop() override;

  void wait_initialize() override;
  void get_metrics(const srsran::srsran_rat_t& rat, phy_metrics_t* m) override;

  std::string get_type() override { return "vnf_nr"; };

  void start_plot() override;

  // RRC interface
  bool set_config(const srsran::phy_cfg_nr_t& cfg) override;

  // MAC interface
  int tx_request(const tx_request_t& request) override;
  int set_ul_grant(std::array<uint8_t, SRSRAN_RAR_UL_GRANT_NBITS>, uint16_t rnti, srsran_rnti_type_t rnti_type) override
  {
    return SRSRAN_SUCCESS;
  };
  void send_prach(const uint32_t preamble_idx,
                  const int      prach_occasion,
                  const float    target_power_dbm,
                  const float    ta_base_sec = 0.0f) override{};
  bool has_valid_sr_resource(uint32_t sr_id) override;
  void clear_pending_grants() override;

private:
  std::unique_ptr<srsran::srsran_basic_vnf> vnf;

  srsue::stack_interface_phy_nr* stack = nullptr;

  bool initialized = false;
};

} // namespace srsue

#endif // SRSUE_VNF_PHY_NR_H
