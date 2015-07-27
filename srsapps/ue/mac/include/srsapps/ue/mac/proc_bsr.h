/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
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



#include <stdint.h>

#include "srsapps/common/log.h"
#include "srsapps/ue/mac/proc.h"
#include "srsapps/common/mac_interface.h"
#include "srsapps/ue/mac/mac_params.h"
#include "srsapps/common/timers.h" 

#ifndef PROCBSR_H
#define PROCBSR_H

/* Buffer status report procedure */

namespace srslte {
namespace ue {

class bsr_proc : public proc, timer_callback
{
public:
  bsr_proc();
  void init(rlc_interface_mac *rlc, log *log_h, mac_params *params_db, timers *timers_db);
  void step(uint32_t tti);  
  void reset();
  void setup_lcg(uint32_t lcid, uint32_t new_lcg);
  void set_priority(uint32_t lcid, uint32_t priority); 
  void timer_expired(uint32_t timer_id);
  
  typedef enum {
    LONG_BSR, 
    SHORT_BSR, 
    TRUNC_BSR    
  } bsr_format_t;
  
  typedef struct {
    bsr_format_t format; 
    uint32_t buff_size[4];
  } bsr_t; 

  uint32_t need_to_send_bsr_on_ul_grant(uint32_t grant_size); 
  bool generate_bsr_on_ul_grant(uint32_t nof_padding_bytes, bsr_t *bsr);
  bool need_to_send_sr(); 
  bool need_to_reset_sr(); 
  
private:
  
  bool       reset_sr; 
  mac_params *params_db; 
  timers     *timers_db; 
  log        *log_h; 
  rlc_interface_mac *rlc;
  bool       initiated;
  const static int MAX_LCID = 20; 
  int        lcg[MAX_LCID];
  uint32_t   last_pending_data[MAX_LCID];
  int        priorities[MAX_LCID]; 
  uint32_t   find_max_priority_lcid(); 
  typedef enum {NONE, REGULAR, PADDING, PERIODIC} triggered_bsr_type_t;
  triggered_bsr_type_t triggered_bsr_type; 
  bool timer_periodic;
  bool timer_retx;
  
  bool sr_is_sent;
  uint32_t last_print;
  void update_pending_data(); 
  bool check_highest_channel(); 
  bool check_single_channel(); 
  bool generate_bsr(bsr_t *bsr, uint32_t nof_padding_bytes); 
  char* bsr_type_tostring(triggered_bsr_type_t type); 
  char* bsr_format_tostring(bsr_format_t format);
};
}
}

#endif