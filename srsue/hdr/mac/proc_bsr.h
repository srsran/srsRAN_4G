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

#ifndef SRSUE_PROC_BSR_H
#define SRSUE_PROC_BSR_H

#include <stdint.h>

#include "srslte/common/log.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "srslte/common/timers.h"

/* Buffer status report procedure */

namespace srsue {

class bsr_proc : public srslte::timer_callback, public bsr_interface_mux
{
public:
  bsr_proc();
  void init(rlc_interface_mac *rlc, srslte::log *log_h, mac_interface_rrc::mac_cfg_t *mac_cfg, srslte::timers *timers_db);
  void step(uint32_t tti);  
  void reset();
  void setup_lcg(uint32_t lcid, uint32_t new_lcg);
  void set_priority(uint32_t lcid, uint32_t priority); 
  void timer_expired(uint32_t timer_id);
  uint32_t get_buffer_state();
  bool need_to_send_bsr_on_ul_grant(uint32_t grant_size, bsr_t *bsr); 
  bool generate_padding_bsr(uint32_t nof_padding_bytes, bsr_t *bsr);
  bool need_to_send_sr(uint32_t tti); 
  bool need_to_reset_sr(); 
  void set_tx_tti(uint32_t tti); 
  
private:
  
  const static int QUEUE_STATUS_PERIOD_MS = 500; 
  
  bool              reset_sr;
  mac_interface_rrc::mac_cfg_t *mac_cfg;
  srslte::timers    *timers_db;
  srslte::log       *log_h;
  rlc_interface_mac *rlc;
  bool              initiated;
  const static int MAX_LCID = 6; 
  int        lcg[MAX_LCID];
  uint32_t   last_pending_data[MAX_LCID];
  int        priorities[MAX_LCID]; 
  uint32_t   find_max_priority_lcid(); 
  typedef enum {NONE, REGULAR, PADDING, PERIODIC} triggered_bsr_type_t;
  triggered_bsr_type_t triggered_bsr_type; 
  
  bool sr_is_sent;
  uint32_t last_print;
  uint32_t next_tx_tti;
  void update_pending_data(); 
  bool check_highest_channel(); 
  bool check_single_channel(); 
  bool generate_bsr(bsr_t *bsr, uint32_t nof_padding_bytes); 
  char* bsr_type_tostring(triggered_bsr_type_t type); 
  char* bsr_format_tostring(bsr_format_t format);

  uint32_t timer_periodic_id;
  uint32_t timer_retx_id;
};

} // namespace srsue

#endif // SRSUE_PROC_BSR_H
