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

/******************************************************************************
 * File:        ue_stack_interface.h
 * Description: Pure virtual interface to the UE stack.
 *****************************************************************************/

#ifndef SRSUE_UE_STACK_INTERFACE_H
#define SRSUE_UE_STACK_INTERFACE_H

#include "srslte/common/logger.h"
#include "stack/rrc/rrc.h"
#include "stack/upper/gw.h"
#include "stack/upper/nas.h"
#include "stack/upper/usim.h"
#include "ue_metrics_interface.h"

namespace srsue {

typedef struct {
  bool        enable;
  std::string filename;
  bool        nas_enable;
  std::string nas_filename;
} pcap_args_t;

typedef struct {
  std::string mac_level;
  std::string rlc_level;
  std::string pdcp_level;
  std::string rrc_level;
  std::string gw_level;
  std::string nas_level;
  std::string usim_level;

  int mac_hex_limit;
  int rlc_hex_limit;
  int pdcp_hex_limit;
  int rrc_hex_limit;
  int gw_hex_limit;
  int nas_hex_limit;
  int usim_hex_limit;
} stack_log_args_t;

typedef struct {
  std::string      type;
  pcap_args_t      pcap;
  stack_log_args_t log;
  usim_args_t      usim;
  rrc_args_t       rrc;
  std::string      ue_category_str;
  nas_args_t       nas;
  gw_args_t        gw;
} stack_args_t;

class ue_stack_interface
{
public:
  virtual bool init(stack_args_t args_, srslte::logger* logger_) = 0;
  virtual void stop()                                            = 0;
  virtual bool switch_on()                                       = 0;
  virtual bool switch_off()                                      = 0;

  virtual bool get_metrics(ue_metrics_t* metrics) = 0;

  virtual bool is_rrc_connected() = 0;
};

} // namespace srsue

#endif // SRSUE_UE_STACK_INTERFACE_H
