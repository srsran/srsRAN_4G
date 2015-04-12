/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include <string.h>
#include <strings.h>
#include <pthread.h>

#include "srslte/srslte.h"
#include "srslte/cuhd/cuhd.h"
#include "srslte/ue_itf/prach.h"
#include "srslte/ue_itf/phy.h"
#include "srslte/ue_itf/params.h"

namespace srslte {
namespace ue {
 
  
void prach::free_cell() 
{
  if (initiated) {
    for (uint32_t i=0;i<64;i++) {
      if (buffer[i]) {
        free(buffer[i]);
      }
    }
    srslte_prach_free(&prach);
  }
}

bool prach::init_cell(srslte_cell_t cell, params *params_db_)
{
  params_db = params_db_; 
  preamble_idx = -1; 
  if (srslte_prach_init(&prach, srslte_symbol_sz(cell.nof_prb), 
                        srslte_prach_get_preamble_format(params_db->get_param(params::PRACH_CONFIG_INDEX)), 
                        params_db->get_param(params::PRACH_ROOT_SEQ_IDX), 
                        params_db->get_param(params::PRACH_HIGH_SPEED_FLAG)?true:false, 
                        params_db->get_param(params::PRACH_ZC_CONFIG))) {
    return false; 
  }
  
  len = prach.N_seq + prach.N_cp;
  for (uint32_t i=0;i<64;i++) {
    buffer[i] = (cf_t*) srslte_vec_malloc(len*sizeof(cf_t));
    if(!buffer[i]) {
      return false; 
    }
    if(srslte_prach_gen(&prach, i, params_db->get_param(params::PRACH_FREQ_OFFSET), buffer[i])){
      return false;
    }
  }
  initiated = true; 
  return true;  
}

bool prach::prepare_to_send(uint32_t preamble_idx_)
{
  if (initiated && preamble_idx_ < 64) {
    preamble_idx = preamble_idx_;
    INFO("PRACH Buffer: Prepare to send preamble %d\n", preamble_idx);
    return true; 
  } else {
    return false; 
  }
}

bool prach::is_ready_to_send(uint32_t current_tti_) {
  if (initiated && preamble_idx >= 0 && preamble_idx < 64 && params_db != NULL) {
    // consider the number of subframes the transmission must be anticipated 
    uint32_t current_tti = current_tti_ + tx_advance_sf;
    
    // Get SFN and sf_idx from the PRACH configuration index
    uint32_t config_idx = (uint32_t) params_db->get_param(params::PRACH_CONFIG_INDEX); 
    srslte_prach_sfn_t prach_sfn = srslte_prach_get_sfn(config_idx);  

    if (prach_sfn == SRSLTE_PRACH_SFN_EVEN && ((current_tti/10)%2)==0 ||
        prach_sfn == SRSLTE_PRACH_SFN_ANY) 
    {
      srslte_prach_sf_config_t sf_config;
      srslte_prach_sf_config(config_idx, &sf_config);
      for (int i=0;i<sf_config.nof_sf;i++) {
        if ((current_tti%10) == sf_config.sf[i]) {
          INFO("PRACH Buffer: Ready to send at tti: %d\n", current_tti);
          return true; 
        }
      }
    }
  }
  INFO("PRACH Buffer: Not ready to send at tti: %d\n", current_tti_);
  return false;     
}

bool prach::send(void *radio_handler, srslte_timestamp_t rx_time)
{
  // advance transmission time
  srslte_timestamp_t tx_time; 
  srslte_timestamp_copy(&tx_time, &rx_time);
  srslte_timestamp_add(&tx_time, 0, 1e-3*tx_advance_sf); 

  // transmit
  cuhd_send_timed(radio_handler, buffer[preamble_idx], len, tx_time.full_secs, tx_time.frac_secs);
}
  
} // namespace ue
} // namespace srslte
