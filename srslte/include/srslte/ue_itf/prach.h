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

#include "srslte/srslte.h"
#include "srslte/common/radio.h"
#include "srslte/ue_itf/queue.h"
#include "srslte/ue_itf/phy_params.h"

#ifndef UEPRACH_H
#define UEPRACH_H

namespace srslte {
namespace ue {

  class SRSLTE_API prach {
  public: 
    prach() {
      params_db = NULL; 
      initiated = false; 
      signal_buffer = NULL; 
    }
    bool           init_cell(srslte_cell_t cell, phy_params *params_db);
    void           free_cell();
    bool           prepare_to_send(uint32_t preamble_idx);
    bool           prepare_to_send(uint32_t preamble_idx, int allowed_subframe);
    bool           prepare_to_send(uint32_t preamble_idx, int allowed_subframe, int target_power_dbm);
    bool           is_ready_to_send(uint32_t current_tti);
    int            get_transmitted_tti(); 
    bool           send(srslte::radio* radio_handler, float cfo, srslte_timestamp_t rx_time);
  private: 
    static const uint32_t tx_advance_sf = 1; // Number of subframes to advance transmission
    phy_params    *params_db     = NULL; 
    int            preamble_idx;  
    int            allowed_subframe; 
    bool           initiated     = false;   
    uint32_t       len; 
    cf_t          *buffer[64]; 
    srslte_prach_t prach_obj; 
    uint32_t       transmitted_tti;
    srslte_cell_t  cell;
    cf_t          *signal_buffer;
    srslte_cfo_t   cfo_h; 
  };

}
}
#endif
