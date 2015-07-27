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



#include "srslte/srslte.h"
#include "srsapps/radio/radio.h"
#include "srsapps/common/log.h"
#include "srsapps/common/phy_interface.h"
#include "srsapps/ue/phy/phy_params.h"

#ifndef UEPRACH_H
#define UEPRACH_H

namespace srslte {
namespace ue {

  class prach {
  public: 
    prach() {
      params_db = NULL; 
      initiated = false; 
      signal_buffer = NULL; 
    }
    void           init(phy_params *params_db, log *log_h);
    bool           init_cell(srslte_cell_t cell);
    void           free_cell();
    bool           prepare_to_send(uint32_t preamble_idx, int allowed_subframe = -1, float target_power_dbm = -1);
    bool           is_ready_to_send(uint32_t current_tti);
    int            tx_tti();
    
    bool           send(radio* radio_handler, float cfo, srslte_timestamp_t rx_time);
    
  private: 
    static const uint32_t tx_advance_sf = 4; // Number of subframes to advance transmission
    phy_params    *params_db; 
    log           *log_h; 
    int            preamble_idx;  
    int            allowed_subframe; 
    bool           initiated;   
    uint32_t       len; 
    cf_t          *buffer[64]; 
    srslte_prach_t prach_obj; 
    int            transmitted_tti;
    srslte_cell_t  cell;
    cf_t          *signal_buffer;
    srslte_cfo_t   cfo_h; 
    
  };

}
}
#endif
