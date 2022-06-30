/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

/******************************************************************************
 * File:        ue.h
 * Description: Top-level UE class. Creates and links all
 *              layers and helpers.
 *****************************************************************************/

#ifndef SRSUE_UE_H
#define SRSUE_UE_H

#include <pthread.h>
#include <stdarg.h>
#include <string>

#include "phy/ue_phy_base.h"
#include "srsran/common/buffer_pool.h"
#include "srsran/radio/radio.h"
#include "srsran/srslog/srslog.h"
#include "srsran/system/sys_metrics_processor.h"
#include "stack/ue_stack_base.h"

#include "ue_metrics_interface.h"

namespace srsue {

/*******************************************************************************
  UE Parameters
*******************************************************************************/

typedef struct {
  bool        enable;
  std::string phy_filename;
  std::string radio_filename;
} trace_args_t;

typedef struct {
  std::string all_level;
  int         all_hex_limit;
  int         file_max_size;
  std::string filename;
} log_args_t;

typedef struct {
  bool enable;
} gui_args_t;

typedef struct {
  float       metrics_period_secs;
  bool        metrics_csv_enable;
  bool        metrics_csv_append;
  int         metrics_csv_flush_period_sec;
  std::string metrics_csv_filename;
  bool        metrics_json_enable;
  std::string metrics_json_filename;
  bool        tracing_enable;
  std::string tracing_filename;
  std::size_t tracing_buffcapacity;
} general_args_t;

typedef struct {
  srsran::rf_args_t rf;
  trace_args_t      trace;
  log_args_t        log;
  gui_args_t        gui;

  phy_args_t   phy;
  stack_args_t stack;
  gw_args_t    gw;

  general_args_t general;
} all_args_t;

/*******************************************************************************
  Main UE class
*******************************************************************************/

class ue : public ue_metrics_interface
{
public:
  ue();
  ~ue();

  int  init(const all_args_t& args_);
  void stop();
  bool switch_on();
  bool switch_off();
  void start_plot();

  // UE metrics interface
  bool get_metrics(ue_metrics_t* m);

  void radio_overflow();

private:
  // UE consists of a radio, a PHY and a stack element
  std::unique_ptr<ue_phy_base>        phy;
  std::unique_ptr<ue_phy_base>        dummy_phy;
  std::unique_ptr<srsran::radio_base> radio;
  std::unique_ptr<ue_stack_base>      stack;
  std::unique_ptr<gw>                 gw_inst;

  // Generic logger members
  srslog::basic_logger& logger;

  // System metrics processor.
  srsran::sys_metrics_processor sys_proc;

  all_args_t args;

  // Helper functions
  int parse_args(const all_args_t& args); // parse and validate arguments

  std::string get_build_mode();
  std::string get_build_info();
  std::string get_build_string();
};

} // namespace srsue

#endif // SRSUE_UE_H
