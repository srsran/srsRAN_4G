/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

/******************************************************************************
 * File:        ue.h
 * Description: Top-level UE class. Creates and links all
 *              layers and helpers.
 *****************************************************************************/

#ifndef SRSUE_UE_H
#define SRSUE_UE_H

#include <stdarg.h>
#include <string>
#include <pthread.h>

#include "ue_base.h"
#include "srslte/radio/radio_multi.h"
#include "phy/phy.h"
#include "mac/mac.h"
#include "srslte/upper/rlc.h"
#include "srslte/upper/pdcp.h"
#include "upper/rrc.h"
#include "upper/nas.h"
#include "upper/gw.h"
#include "upper/usim.h"

#include "srslte/common/buffer_pool.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "srslte/common/logger_file.h"
#include "srslte/common/log_filter.h"

#include "ue_metrics_interface.h"

namespace srsue {

/*******************************************************************************
  Main UE class
*******************************************************************************/

class ue
    :public ue_base
{
public:
  ue();

  bool init(all_args_t *args_);
  void stop();
  bool attach();
  bool deattach();
  bool is_attached();
  void start_plot();
  void print_mbms();
  bool mbms_service_start(uint32_t serv, uint32_t port);

  void print_pool();

  static void rf_msg(srslte_rf_error_t error);

  // UE metrics interface
  bool get_metrics(ue_metrics_t &m);

  void pregenerate_signals(bool enable);

  void radio_overflow();

private:
  virtual ~ue();

  srslte::radio_multi radio;
  srsue::phy         phy;
  srsue::mac         mac;
  srslte::mac_pcap   mac_pcap;
  srslte::nas_pcap   nas_pcap;
  srslte::rlc        rlc;
  srslte::pdcp       pdcp;
  srsue::rrc         rrc;
  srsue::nas         nas;
  srsue::gw          gw;
  srsue::usim_base*  usim;

  srslte::logger_stdout logger_stdout;
  srslte::logger_file   logger_file;
  srslte::logger        *logger;

  // rf_log is on ue_base
  std::vector<srslte::log_filter*>  phy_log;
  srslte::log_filter  mac_log;
  srslte::log_filter  rlc_log;
  srslte::log_filter  pdcp_log;
  srslte::log_filter  rrc_log;
  srslte::log_filter  nas_log;
  srslte::log_filter  gw_log;
  srslte::log_filter  usim_log;
  srslte::log_filter  pool_log;

  all_args_t       *args;
  bool              started;

  bool check_srslte_version();
};

} // namespace srsue

#endif // SRSUE_UE_H
  
