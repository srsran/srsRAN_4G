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




#include "srsapps/common/log.h"
#include "srsapps/ue/phy/phy.h"
#include "srsapps/ue/mac/mac_params.h"
#include "srsapps/common/timers.h"
#include "srsapps/ue/mac/demux.h"

#ifndef DLHARQ_H
#define DLHARQ_H

/* Downlink HARQ entity as defined in 5.3.2 of 36.321 */

namespace srslte {
namespace ue {
  
typedef _Complex float cf_t; 

class dl_harq_entity
{
public:

  const static uint32_t NOF_HARQ_PROC = 8; 
  const static uint32_t HARQ_BCCH_PID = NOF_HARQ_PROC; 
  
  dl_harq_entity();
  bool init(srslte_cell_t cell, uint32_t max_payload_len, srslte::log *log_h_, timers *timers_, demux *demux_unit);
  bool is_sps(uint32_t pid); 
  void set_harq_info(uint32_t pid, dl_sched_grant *grant);
  void receive_data(uint32_t tti, uint32_t pid, dl_buffer *dl_buffer, phy *phy_h);
  void reset();
  bool is_ack_pending_resolution();
  void send_pending_ack_contention_resolution();
private:  
  
  
  class dl_harq_process {
  public:
    dl_harq_process();
    bool init(srslte_cell_t cell, uint32_t max_payload_len, dl_harq_entity *parent);
    void set_harq_info(dl_sched_grant *grant); 
    void receive_data(uint32_t tti, dl_buffer *dl_buffer, phy *phy_h); 
    void reset();
    // Called after the contention resolution is terminated to send pending ACKs, if any
    void send_pending_ack_contention_resolution();
    uint32_t pid;    
  private: 
    
    bool            is_initiated; 
    dl_harq_entity *harq_entity; 
    uint8_t        *payload; 
    uint32_t       max_payload_len; 
    dl_sched_grant cur_grant;
    dl_sched_grant pending_ack_grant;
    ul_buffer     *pending_ul_buffer;
    bool           pending_ack; 
    srslte::log    *log_h; 
    
    srslte_softbuffer_rx_t softbuffer; 
    bool          ack;
  };
  
  dl_harq_process proc[NOF_HARQ_PROC+1];
  timers          *timers_db; 
  demux           *demux_unit; 
  srslte::log     *log_h; 
  int              pending_ack_pid; 
};

} 
}
#endif