/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

#ifndef SRSENB_PHY_H
#define SRSENB_PHY_H

#include "phy_common.h"
#include "sf_worker.h"
#include "srsenb/hdr/phy/enb_phy_base.h"
#include "srslte/common/log.h"
#include "srslte/common/log_filter.h"
#include "srslte/common/trace.h"
#include "srslte/interfaces/common_interfaces.h"
#include "srslte/interfaces/enb_interfaces.h"
#include "srslte/interfaces/enb_metrics_interface.h"
#include "srslte/radio/radio.h"
#include "txrx.h"

namespace srsenb {
 
typedef struct {
  srslte_cell_t                  cell;
  asn1::rrc::prach_cfg_sib_s     prach_cnfg;
  asn1::rrc::pdsch_cfg_common_s  pdsch_cnfg;
  asn1::rrc::pusch_cfg_common_s  pusch_cnfg;
  asn1::rrc::pucch_cfg_common_s  pucch_cnfg;
  asn1::rrc::srs_ul_cfg_common_c srs_ul_cnfg;
} phy_cfg_t;

class phy : public enb_phy_base, public phy_interface_stack_lte, public srslte::phy_interface_radio
{
public:
  phy(srslte::logger* logger_);
  ~phy();

  int  init(const phy_args_t&            args,
            const phy_cfg_t&             cfg,
            srslte::radio_interface_phy* radio_,
            stack_interface_phy_lte*     stack_);
  void stop();

  std::string get_type() { return "lte"; };

  /* MAC->PHY interface */
  int  add_rnti(uint16_t rnti, bool is_temporal = false);
  void rem_rnti(uint16_t rnti);
  void set_mch_period_stop(uint32_t stop);

  /*RRC-PHY interface*/
  void configure_mbsfn(asn1::rrc::sib_type2_s* sib2, asn1::rrc::sib_type13_r9_s* sib13, asn1::rrc::mcch_msg_s mcch);

  static uint32_t tti_to_SFN(uint32_t tti);
  static uint32_t tti_to_subf(uint32_t tti);
  
  void start_plot();
  void set_config_dedicated(uint16_t rnti, asn1::rrc::phys_cfg_ded_s* dedicated);

  void get_metrics(phy_metrics_t metrics[ENB_METRICS_MAX_USERS]);

  void radio_overflow(){};
  void radio_failure(){};

private:
  phy_rrc_cfg_t phy_rrc_config = {};
  uint32_t      nof_workers    = 0;

  const static int MAX_WORKERS         = 4;
  const static int DEFAULT_WORKERS     = 2;
  
  const static int PRACH_WORKER_THREAD_PRIO = 3;
  const static int SF_RECV_THREAD_PRIO = 1;
  const static int WORKERS_THREAD_PRIO = 2;

  srslte::radio_interface_phy* radio = nullptr;

  srslte::logger*                                   logger = nullptr;
  std::vector<std::unique_ptr<srslte::log_filter> > log_vec;
  srslte::log*                                      log_h = nullptr;

  srslte::thread_pool      workers_pool;
  std::vector<sf_worker>    workers;
  phy_common                workers_common;
  prach_worker             prach;
  txrx                      tx_rx;

  bool initialized = false;

  srslte_prach_cfg_t prach_cfg = {};

  void parse_config(const phy_cfg_t& cfg);
};

} // namespace srsenb

#endif // SRSENB_PHY_H
