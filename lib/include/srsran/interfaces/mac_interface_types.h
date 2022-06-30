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

#ifndef SRSRAN_MAC_INTERFACE_TYPES_H
#define SRSRAN_MAC_INTERFACE_TYPES_H

#include "rrc_interface_types.h"

namespace srsran {

/***************************
 *      MAC Config
 **************************/

/* Logical Channel Multiplexing and Prioritization + Msg3 Buffer */

class logical_channel_config_t
{
public:
  uint8_t  lcid;
  uint8_t  lcg;
  int32_t  Bj;
  int32_t  PBR; // in kByte/s, -1 sets to infinity
  uint32_t bucket_size;
  uint32_t BSD;
  uint32_t priority;
  int      sched_len;  // scheduled upper layer payload for this LCID
  int      buffer_len; // outstanding bytes for this LCID
};

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

/// NR specific config for DL HARQ with configurable number of processes
struct dl_harq_cfg_nr_t {
  uint8_t nof_procs; // Number of HARQ processes used in the DL
  dl_harq_cfg_nr_t() { reset(); }
  void reset() { nof_procs = SRSRAN_DEFAULT_HARQ_PROC_DL_NR; }
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

// 38.321 5.1.1 Not complete yet
struct rach_cfg_nr_t {
  uint32_t prach_ConfigurationIndex;
  int      PreambleReceivedTargetPower;
  uint32_t preambleTransMax;
  uint32_t powerRampingStep;
  uint32_t ra_responseWindow;
  uint32_t ra_ContentionResolutionTimer;

  rach_cfg_nr_t() { reset(); }
  void reset()
  {
    prach_ConfigurationIndex    = 0;
    PreambleReceivedTargetPower = 0;
    powerRampingStep            = 0;
    preambleTransMax            = 0;
    ra_responseWindow           = 0;
  }
};

// 38.321 Section 5.4.4 (only one config supported right now)
struct sr_cfg_item_nr_t {
  uint8_t sched_request_id;
  uint8_t prohibit_timer;
  uint8_t trans_max;
};

#define SRSRAN_MAX_MAX_NR_OF_SR_CFG_PER_CELL_GROUP (8)
struct sr_cfg_nr_t {
  bool             enabled;
  uint8_t          num_items;
  sr_cfg_item_nr_t item[SRSRAN_MAX_MAX_NR_OF_SR_CFG_PER_CELL_GROUP];
};

struct tag_cfg_nr_t {
  uint8_t  tag_id;
  uint32_t time_align_timer;
};

struct phr_cfg_nr_t {
  int  periodic_timer;
  int  prohibit_timer;
  int  tx_pwr_factor_change;
  bool extended;
  phr_cfg_nr_t() { reset(); }
  void reset()
  {
    periodic_timer       = -1;
    prohibit_timer       = -1;
    tx_pwr_factor_change = -1;
    extended             = false;
  }
};

struct bsr_cfg_nr_t {
  // mandatory BSR config
  int periodic_timer;
  int retx_timer;

  // SR specific configs for logical channel
  bool sr_delay_timer_enabled;
  int  sr_delay_timer;
  bool sr_mask; // Indicates whether SR masking is configured for this logical channel
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

struct mac_cfg_nr_t {
  // Default constructor with default values as in 36.331 9.2.2
  mac_cfg_nr_t() { set_defaults(); }

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
  phr_cfg_nr_t  phr_cfg;
  sr_cfg_t      sr_cfg;
  rach_cfg_nr_t rach_cfg;
  ul_harq_cfg_t harq_cfg;
  int           time_alignment_timer = -1;
};

} // namespace srsran

#endif // SRSRAN_MAC_INTERFACE_TYPES_H
