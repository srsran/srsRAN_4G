/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSUE_MUX_H
#define SRSUE_MUX_H

#include <pthread.h>

#include <vector>

#include "srslte/common/log.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "srslte/common/pdu.h"
#include "proc_bsr.h"
#include "proc_phr.h"

/* Logical Channel Multiplexing and Prioritization + Msg3 Buffer */   


typedef struct {
  uint32_t      id;
  int           Bj;
  int           PBR; // -1 sets to infinity
  uint32_t      BSD;
  uint32_t      priority;    
  int           sched_len;
  int           buffer_len; 
} lchid_t; 

namespace srsue {
  
class mux
{
public:
  mux(uint8_t nof_harq_proc_);
  void     reset();
  void     init(rlc_interface_mac *rlc, srslte::log *log_h, bsr_interface_mux *bsr_procedure, phr_proc *phr_procedure_);

  bool     is_pending_any_sdu();
  bool     is_pending_sdu(uint32_t lcid); 
  
  uint8_t* pdu_get(uint8_t *payload, uint32_t pdu_sz, uint32_t tx_tti, uint32_t pid);
  uint8_t* msg3_get(uint8_t* payload, uint32_t pdu_sz);
  
  void     msg3_flush();
  bool     msg3_is_transmitted();

  void     msg3_prepare();
  bool     msg3_is_pending();
  
  void     append_crnti_ce_next_tx(uint16_t crnti); 
  
  void     set_priority(uint32_t lcid, uint32_t priority, int PBR_x_tti, uint32_t BSD);
  void     clear_lch(uint32_t lch_id); 
  void     pusch_retx(uint32_t tx_tti, uint32_t pid);
      
private:  
  int      find_lchid(uint32_t lch_id);
  bool     pdu_move_to_msg3(uint32_t pdu_sz);
  bool     allocate_sdu(uint32_t lcid, srslte::sch_pdu *pdu, int max_sdu_sz);
  bool     sched_sdu(lchid_t *ch, int *sdu_space, int max_sdu_sz);
  
  const static int MIN_RLC_SDU_LEN = 0; 
  const static int MAX_NOF_SUBHEADERS = 20; 

  std::vector<lchid_t> lch; 
  
  // Keep track of the PIDs that transmitted BSR reports 
  std::vector<bool> pid_has_bsr;
  
  // Mutex for exclusive access
  pthread_mutex_t mutex; 

  srslte::log       *log_h;
  rlc_interface_mac *rlc; 
  bsr_interface_mux *bsr_procedure;
  phr_proc          *phr_procedure;
  uint16_t           pending_crnti_ce;
  uint8_t            nof_harq_proc;
  
  /* Msg3 Buffer */
  static const uint32_t MSG3_BUFF_SZ = 1024;
  uint8_t               msg3_buff[MSG3_BUFF_SZ];
  uint8_t              *msg3_buff_start_pdu;

  /* PDU Buffer */
  srslte::sch_pdu    pdu_msg; 
  bool msg3_has_been_transmitted;
  bool msg3_pending;
};

} // namespace srsue

#endif // SRSUE_MUX_H

