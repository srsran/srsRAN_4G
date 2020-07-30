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
#include "srslte/common/buffer_pool.h"
#include "srslte/common/log_filter.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "srslte/radio/radio.h"
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
} general_args_t;

typedef struct {
  srslte::rf_args_t rf;
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

  int  init(const all_args_t& args_, srslte::logger* logger_);
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
  std::unique_ptr<srslte::radio_base> radio;
  std::unique_ptr<ue_stack_base>      stack;
  std::unique_ptr<gw>                 gw_inst;

  // Generic logger members
  srslte::logger*    logger = nullptr;
  srslte::log_filter log; // Own logger for UE

  all_args_t                args;
  srslte::byte_buffer_pool* pool = nullptr;

  // Helper functions
  int parse_args(const all_args_t& args); // parse and validate arguments

  std::string get_build_mode();
  std::string get_build_info();
  std::string get_build_string();
};

} // namespace srsue

#endif // SRSUE_UE_H
