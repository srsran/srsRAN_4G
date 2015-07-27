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
#include "srsapps/common/mac_interface.h"
#include "srsapps/common/log.h"
#include "srsapps/common/qbuff.h"
#include "srsapps/common/timers.h"
#include "srsapps/ue/mac/mac_params.h"
#include "srsapps/ue/mac/pdu.h"

#ifndef DEMUX_H
#define DEMUX_H

/* Logical Channel Demultiplexing and MAC CE dissassemble */   

namespace srslte {
namespace ue {

class demux
{
public:
  demux();
  void init(phy_interface* phy_h_, rlc_interface_mac *rlc, log* log_h_, timers* timers_db_);

  void     process_pdus();
  uint8_t* request_buffer(uint32_t len);
  
  void     release_pdu(uint8_t *buff, uint32_t nof_bytes);
  void     release_pdu_bcch(uint8_t *buff, uint32_t nof_bytes);
  void     release_pdu_temp_crnti(uint8_t *buff, uint32_t nof_bytes);

  bool     is_temp_crnti_pending();
  bool     is_contention_resolution_id_pending(); 
  void     demultiplex_pending_pdu();
  void     discard_pending_pdu();

  uint64_t get_contention_resolution_id();
  
private:
  const static int NOF_PDU_Q   = 3; // prevents threads from being locked
  const static int MAX_PDU_LEN = 128*1024; 

  sch_pdu mac_msg;
  sch_pdu pending_mac_msg;
  
  void process_pdu(uint8_t *pdu, uint32_t nof_bytes);
  void process_sch_pdu(sch_pdu *pdu);
  bool process_ce(sch_subh *subheader);
  bool find_unused_queue(uint8_t *idx);  
  bool find_nonempty_queue(uint8_t *idx);
  void push_buffer(uint8_t *buff, uint32_t nof_bytes);

  uint64_t   contention_resolution_id; 
  bool       pending_temp_rnti;
  bool       has_pending_contention_resolution_id; 
   
  typedef struct {
    uint8_t idx; 
    uint8_t dummy[15]; // FIXME: This it to keep 128-bit alignment
  } buff_header_t;

  // Mutex for exclusive access
  pthread_mutex_t mutex; 
  pthread_cond_t  cvar; 
  qbuff      pdu_q[NOF_PDU_Q]; 
  bool       used_q[NOF_PDU_Q]; 
  
  phy_interface     *phy_h; 
  log               *log_h;  
  timers            *timers_db; 
  rlc_interface_mac *rlc;
};
}
}

#endif



