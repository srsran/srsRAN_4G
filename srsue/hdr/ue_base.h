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
 * File:        ue_base.h
 * Description: Base class for UEs.
 *****************************************************************************/

#ifndef UE_BASE_H
#define UE_BASE_H

#include <stdarg.h>
#include <string>
#include <pthread.h>
#include "srslte/radio/radio_multi.h"
#include "phy/phy.h"
#include "upper/usim.h"
#include "upper/rrc.h"
#include "srslte/interfaces/ue_interfaces.h"

#include "srslte/common/logger.h"
#include "srslte/common/log_filter.h"

#include "ue_metrics_interface.h"

namespace srsue {

/*******************************************************************************
  UE Parameters
*******************************************************************************/

typedef struct {
  uint32_t      dl_earfcn;
  float         dl_freq;
  float         ul_freq;
  float         freq_offset;
  float         rx_gain;
  float         tx_gain;
  uint32_t      nof_rx_ant;
  std::string   device_name;
  std::string   device_args;
  std::string   time_adv_nsamples;
  std::string   burst_preamble;
}rf_args_t;

typedef struct {
  bool          enable;
  std::string   filename;
  bool          nas_enable;
  std::string   nas_filename;
}pcap_args_t;

typedef struct {
  bool          enable;
  std::string   phy_filename;
  std::string   radio_filename;
}trace_args_t;

typedef struct {
  std::string   phy_level;
  std::string   phy_lib_level;
  std::string   mac_level;
  std::string   rlc_level;
  std::string   pdcp_level;
  std::string   rrc_level;
  std::string   gw_level;
  std::string   nas_level;
  std::string   usim_level;
  std::string   all_level;
  int           phy_hex_limit;
  int           mac_hex_limit;
  int           rlc_hex_limit;
  int           pdcp_hex_limit;
  int           rrc_hex_limit;
  int           gw_hex_limit;
  int           nas_hex_limit;
  int           usim_hex_limit;
  int           all_hex_limit;
  int           file_max_size;
  std::string   filename;
}log_args_t;

typedef struct {
  bool          enable;
}gui_args_t;

typedef struct {
  std::string   ip_netmask;
  phy_args_t    phy;
  float         metrics_period_secs;
  bool          pregenerate_signals;
  bool          metrics_csv_enable;
  std::string   metrics_csv_filename;
}expert_args_t;

typedef struct {
  rf_args_t     rf;
  rf_cal_t      rf_cal; 
  pcap_args_t   pcap;
  trace_args_t  trace;
  log_args_t    log;
  gui_args_t    gui;
  usim_args_t   usim;
  rrc_args_t    rrc;
  std::string   ue_category_str;
  std::string   apn;
  expert_args_t expert;
}all_args_t;

typedef enum {
  LTE = 0,
  SRSUE_INSTANCE_TYPE_NITEMS
} srsue_instance_type_t;
static const char srsue_instance_type_text[SRSUE_INSTANCE_TYPE_NITEMS][10] = { "LTE" };


/*******************************************************************************
  Main UE class
*******************************************************************************/

class ue_base
    :public ue_interface
    ,public ue_metrics_interface
{
public:
  ue_base();
  virtual ~ue_base() {}

  static ue_base* get_instance(srsue_instance_type_t type);

  void cleanup(void);

  virtual bool init(all_args_t *args_) = 0;
  virtual void stop() = 0;
  virtual bool is_attached() = 0;
  virtual void start_plot() = 0;

  virtual void radio_overflow() = 0;
  
  void handle_rf_msg(srslte_rf_error_t error);

  // UE metrics interface
  virtual bool get_metrics(ue_metrics_t &m) = 0;

  virtual void pregenerate_signals(bool enable) = 0;

  srslte::log_filter rf_log;
  rf_metrics_t     rf_metrics;
  srslte::LOG_LEVEL_ENUM level(std::string l);

  std::string get_build_mode();
  std::string get_build_info();
  std::string get_build_string();
};

} // namespace srsue

#endif // UE_BASE_H
  
