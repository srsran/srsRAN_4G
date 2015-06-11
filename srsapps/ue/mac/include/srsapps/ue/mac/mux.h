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


#include <pthread.h>

#include "srsapps/common/log.h"
#include "srsapps/ue/mac/mac_io.h"
#include "srsapps/ue/mac/mac_params.h"
#include "srsapps/ue/mac/pdu.h"
#include "srsapps/ue/mac/proc_bsr.h"

#ifndef MUX_H
#define MUX_H

/* Logical Channel Multiplexing and Prioritization + Msg3 Buffer */   

namespace srslte {
namespace ue {

class mux
{
public:
  mux();
  void     reset();
  void     init(log *log_h, mac_io *mac_io_h, bsr_proc *bsr_procedure);

  bool     is_pending_ccch_sdu();
  bool     is_pending_any_sdu();
  bool     is_pending_sdu(uint32_t lcid); 
  
  uint8_t* pdu_pop(uint32_t pdu_sz);
  bool     pdu_move_to_msg3(uint32_t pdu_sz);
  void     pdu_release();

  uint8_t* msg3_pop(uint32_t pdu_sz);
  void     msg3_flush();
  void     msg3_transmitted(); 
  bool     msg3_is_transmitted();
  
  void     append_crnti_ce_next_tx(uint16_t crnti); 
  
  void     set_priority(uint32_t lcid, uint32_t priority, int PBR_x_tti, uint32_t BSD);
      
private:  
  bool          assemble_pdu(uint32_t pdu_sz); 
  bool          allocate_sdu(uint32_t lcid, sch_pdu *pdu);
  bool          allocate_sdu(uint32_t lcid, sch_pdu *pdu, bool *is_first);
  bool          allocate_sdu(uint32_t lcid, sch_pdu *pdu, uint32_t *sdu_sz, bool *is_first);
  
  int64_t       Bj[mac_io::NOF_UL_LCH];
  int           PBR[mac_io::NOF_UL_LCH]; // -1 sets to infinity
  uint32_t      BSD[mac_io::NOF_UL_LCH];
  uint32_t      priority[mac_io::NOF_UL_LCH];
  uint32_t      priority_sorted[mac_io::NOF_UL_LCH];
  uint32_t      lchid_sorted[mac_io::NOF_UL_LCH];
  uint32_t      nof_tx_pkts[mac_io::NOF_UL_LCH]; 
  
  // Mutex for priority setting from outside MAC 
  pthread_mutex_t mutex; 

  log        *log_h;
  mac_io     *mac_io_h; 
  bsr_proc   *bsr_procedure;
  uint16_t    pending_crnti_ce;
  
  /* Msg3 Buffer */
  static const uint32_t MSG3_BUFF_SZ = 128; 
  qbuff                 msg3_buff; 
  
  /* PDU Buffer */
  static const uint32_t PDU_BUFF_SZ  = 16*1024; 
  qbuff                 pdu_buff; 
  sch_pdu               pdu_msg; 
  bool msg3_has_been_transmitted;
  
};
}
}

#endif

