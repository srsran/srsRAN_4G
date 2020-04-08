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

#ifndef SRSLTE_MAC_INTERFACE_TYPES_H
#define SRSLTE_MAC_INTERFACE_TYPES_H

#include "rrc_interface_types.h"

namespace srslte {

/***************************
 *      MAC Config
 **************************/
struct bsr_cfg_t {
  int periodic_timer;
  int retx_timer;
  bsr_cfg_t() { reset(); }
  void reset()
  {
    periodic_timer = -1;
    retx_timer     = 2560;
  }
};

struct phr_cfg_t {
  bool enabled;
  int  periodic_timer;
  int  prohibit_timer;
  int  db_pathloss_change;
  bool extended;
  phr_cfg_t() { reset(); }
  void reset()
  {
    enabled            = false;
    periodic_timer     = -1;
    prohibit_timer     = -1;
    db_pathloss_change = -1;
    extended           = false;
  }
};

struct sr_cfg_t {
  bool enabled;
  int  dsr_transmax;
  sr_cfg_t() { reset(); }
  void reset()
  {
    enabled      = false;
    dsr_transmax = 0;
  }
};

struct ul_harq_cfg_t {
  uint32_t max_harq_msg3_tx;
  uint32_t max_harq_tx;
  ul_harq_cfg_t() { reset(); }
  void reset()
  {
    max_harq_msg3_tx = 5;
    max_harq_tx      = 5;
  }
};

struct rach_cfg_t {
  bool     enabled;
  uint32_t nof_preambles;
  uint32_t nof_groupA_preambles;
  int32_t  messagePowerOffsetGroupB;
  uint32_t messageSizeGroupA;
  uint32_t responseWindowSize;
  uint32_t powerRampingStep;
  uint32_t preambleTransMax;
  int32_t  iniReceivedTargetPower;
  uint32_t contentionResolutionTimer;
  uint32_t new_ra_msg_len;
  rach_cfg_t() { reset(); }
  void reset()
  {
    enabled                   = false;
    nof_preambles             = 0;
    nof_groupA_preambles      = 0;
    messagePowerOffsetGroupB  = 0;
    messageSizeGroupA         = 0;
    responseWindowSize        = 0;
    powerRampingStep          = 0;
    preambleTransMax          = 0;
    iniReceivedTargetPower    = 0;
    contentionResolutionTimer = 0;
    new_ra_msg_len            = 0;
  }
};

struct mac_cfg_t {
  // Default constructor with default values as in 36.331 9.2.2
  mac_cfg_t() { set_defaults(); }

  void set_defaults()
  {
    rach_cfg.reset();
    sr_cfg.reset();
    set_mac_main_cfg_default();
  }

  void set_mac_main_cfg_default()
  {
    bsr_cfg.reset();
    phr_cfg.reset();
    harq_cfg.reset();
    time_alignment_timer = -1;
  }

  bsr_cfg_t     bsr_cfg;
  phr_cfg_t     phr_cfg;
  sr_cfg_t      sr_cfg;
  rach_cfg_t    rach_cfg;
  ul_harq_cfg_t harq_cfg;
  int           time_alignment_timer = -1;
};

} // namespace srslte

#endif // SRSLTE_MAC_INTERFACE_TYPES_H
