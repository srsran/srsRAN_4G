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




#include "srsapps/ue/phy/phy.h"
#include "srsapps/common/log.h"
#include "srsapps/ue/mac/mac_params.h"
#include "srsapps/ue/mac/mux.h"
#include "srsapps/ue/mac/mac_pcap.h"
#include "srsapps/common/timers.h"

#ifndef ULHARQ_H
#define ULHARQ_H

/* Uplink HARQ entity as defined in 5.4.2 of 36.321 */

namespace srslte {
namespace ue {
  
typedef _Complex float cf_t; 

class ul_harq_entity
{
public:

  const static uint32_t NOF_HARQ_PROC = 8; 
  static uint32_t pidof(uint32_t tti);
  
  ul_harq_entity() {  pcap      = NULL; }
  bool init(srslte_cell_t cell, mac_params *params_db, log *log_h, timers* timers_, mux *mux_unit);
  
  void reset();
  void reset_ndi();
  bool is_sps(uint32_t pid); 
  void run_tti(uint32_t tti, ul_sched_grant *grant, phy *phy_);
  void run_tti(uint32_t tti, phy *phy_);
  void start_pcap(mac_pcap* pcap);
  
private:  
  
  class ul_harq_process {
  public:
    ul_harq_process();
    bool init(srslte_cell_t cell, ul_harq_entity *parent);
    void reset();
    void reset_ndi();

    void generate_retx(uint32_t tti_tx, ul_buffer *ul); 
    void generate_retx(uint32_t tti_tx, ul_sched_grant *ul_grant, ul_buffer *ul); 
    void generate_new_tx(uint32_t tti_tx, uint8_t *payload, bool is_msg3, ul_sched_grant* grant, ul_buffer *ul);

    uint32_t get_rv();
    bool has_grant();
    ul_sched_grant *get_grant();
    void set_harq_feedback(bool ack);
    bool get_ndi();
    uint32_t last_tx_tti();
    uint32_t pid;
   
  private: 
    uint32_t                    current_tx_nb;
    uint32_t                    current_irv; 
    bool                        harq_feedback; 
    bool                        ndi; 
    log                         *log_h; 
    ul_harq_entity              *harq_entity; 
    ul_sched_grant              cur_grant; 
    bool                        is_grant_configured; 
    srslte_softbuffer_tx_t      softbuffer; 
    bool                        is_msg3;
    bool is_initiated;    
    uint32_t tti_last_tx;
    
    void generate_tx(uint32_t tti_tx, uint8_t *pdu_payload, ul_buffer* ul);
  };
    
  timers          *timers_db; 
  mux             *mux_unit;
  ul_harq_process proc[NOF_HARQ_PROC];
  log             *log_h; 
  mac_params      *params_db; 
  mac_pcap        *pcap; 
};

} 
}
#endif