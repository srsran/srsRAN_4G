#include <pthread.h>

#include "srsapps/common/log.h"
#include "srsapps/ue/mac/mac_io.h"
#include "srsapps/ue/mac/mac_params.h"
#include "srsapps/ue/mac/mac_pdu.h"

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
  void     init(log *log_h, mac_io *mac_io_h);

  uint8_t* pdu_pop(uint32_t tti_, uint32_t pdu_sz);
  bool     pdu_move_to_msg3(uint32_t tti, uint32_t pdu_sz);
  void     pdu_release();

  uint8_t* msg3_pop(uint32_t tti_, uint32_t pdu_sz);
  void     msg3_flush();
  void     msg3_release();
  bool     msg3_isempty();
  
  void append_crnti_ce_next_tx(uint16_t crnti); 
  
  void set_priority(uint32_t lch_id, uint32_t priority, int PBR_x_tti, uint32_t BSD);
    
private:  
  bool          assemble_pdu(uint32_t pdu_sz); 
  bool          allocate_sdu(uint32_t lcid, mac_pdu *pdu, uint32_t *sdu_sz);
  bool          allocate_sdu(uint32_t lcid, mac_pdu *pdu);
  
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
  uint32_t    tti;
  uint16_t    pending_crnti_ce;
  
  /* Msg3 Buffer */
  static const uint32_t MSG3_BUFF_SZ = 128; 
  qbuff                 msg3_buff; 
  
  /* PDU Buffer */
  static const uint32_t PDU_BUFF_SZ  = 16*1024; 
  qbuff                 pdu_buff; 
  mac_pdu               pdu_msg; 

};
}
}

#endif

