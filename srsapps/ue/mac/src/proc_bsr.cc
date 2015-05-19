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

#include "srsapps/ue/mac/proc_bsr.h"
#include "srsapps/ue/mac/mac_params.h"

namespace srslte {
  namespace ue {
    
bsr_proc::bsr_proc()
{
  initiated = false; 
}

void bsr_proc::init(log* log_h_, mac_params* params_db_, mux *mux_unit_)
{
  log_h     = log_h; 
  params_db = params_db_;
  mux_unit  = mux_unit_;
  initiated = true;
}

void bsr_proc::reset()
{

}

void bsr_proc::start()
{

}
void bsr_proc::step(uint32_t tti)
{
  if (!initiated) {
    return;
  }  
}
    
  }
}
