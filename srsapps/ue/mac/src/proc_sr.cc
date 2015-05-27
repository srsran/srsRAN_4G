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

#include "srsapps/ue/mac/proc_sr.h"
#include "srsapps/ue/mac/mac_params.h"


namespace srslte {
namespace ue {

sr_proc::sr_proc() {
  initiated = false; 
}
  
void sr_proc::init(log* log_h_, mac_params* params_db_, phy* phy_h_)
{
  log_h     = log_h_;
  params_db = params_db_; 
  phy_h     = phy_h_;
  initiated = true; 
}
  
void sr_proc::reset()
{
  is_pending_sr = false;    
  phy_h->send_sr(false);
}

void sr_proc::step(uint32_t tti)
{
  if (initiated) {
    if (is_pending_sr) {    
      if (sr_counter < dsr_transmax) {
        sr_counter++;
        Info("SR stepping: sr_counter=%d\n", sr_counter);
        phy_h->send_sr(true);
      } else {
        reset();
      }
    }
  }
}

bool sr_proc::need_random_access() {
  if (initiated) {
    if (sr_counter == dsr_transmax && dsr_transmax > 0 && 
        params_db->get_param(mac_params::SR_PUCCH_CONFIGURED)) {
      
      Info("SR checking need RA: sr_counter=%d, dsr_transmax=%d, configured=%d\n", sr_counter, dsr_transmax, params_db->get_param(mac_params::SR_PUCCH_CONFIGURED));
      return true;
    } else {
      return false; 
    }
  }
}

void sr_proc::start()
{
  if (initiated) {
    if (params_db->get_param(mac_params::SR_PUCCH_CONFIGURED)) {
      if (!is_pending_sr) {
        sr_counter = 0;
        dsr_transmax = params_db->get_param(mac_params::SR_TRANS_MAX);
        Info("SR starting dsrTransMax=%d. Setting sr_counter=0\n", dsr_transmax);
      }    
    }
  }
}

}
}

