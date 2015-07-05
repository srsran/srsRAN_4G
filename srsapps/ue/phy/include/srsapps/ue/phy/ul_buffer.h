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
#include "srsapps/common/queue.h"
#include "srsapps/ue/phy/ul_sched_grant.h"
#include "srsapps/ue/phy/dl_sched_grant.h"
#include "srsapps/ue/phy/phy_params.h"
#include "srsapps/radio/radio.h"

#ifndef UEULBUFFER_H
#define UEULBUFFER_H


namespace srslte {
namespace ue {
  
  /* Uplink scheduling assignment. The MAC instructs the PHY to prepare an UL packet (PUSCH or PUCCH) 
   * for transmission. The MAC must call generate_data() to set the packet ready for transmission
   */
  class ul_buffer : public queue::element {

  public: 
    bool     init_cell(srslte_cell_t cell, phy_params *params_db, log *log_h, radio *radio_h);
    void     free_cell();
    void     set_crnti(uint16_t rnti);
    void     set_current_tx_nb(uint32_t current_tx_nb);
    bool     generate_ack(bool ack, dl_sched_grant *last_dl_grant); 
    bool     generate_ack(bool ack[2]); 
    bool     generate_sr(); 
    bool     generate_cqi_report(); 
    bool     uci_ready();
    bool     srs_is_ready_to_send();
    bool     generate_data();   
    bool     generate_data(ul_sched_grant *pusch_grant, uint8_t *payload);   
    bool     generate_data(ul_sched_grant *pusch_grant, srslte_softbuffer_tx_t *softbuffer, uint8_t *payload);   
    void     set_tx_params(float cfo, float time_adv_sec, srslte_timestamp_t tx_time);
    void     send_end_of_burst();    
    void     pregen_signals();
    static const uint32_t tx_advance_sf = 1; // Number of subframes to advance transmission
    static const bool normalize_amp = true; 
  private: 
    log               *log_h; 
    phy_params        *params_db; 
    radio             *radio_h; 
    float              cfo;
    srslte_timestamp_t tx_time; 
    srslte_cell_t      cell; 
    srslte_ue_ul_t     ue_ul; 
    bool               cell_initiated; 
    cf_t*              signal_buffer;
    uint32_t           current_tx_nb; 
    uint32_t           last_n_cce;
    srslte_uci_data_t  uci_data; 
    bool               uci_pending; 
  };

}
}

#endif