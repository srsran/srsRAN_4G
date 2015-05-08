
#include "srsapps/ue/phy/phy.h"
#include "srsapps/common/log.h"
#include "srsapps/ue/mac/mac_io.h"
#include "srsapps/common/timers.h"
#include "srsapps/ue/mac/mac_params.h"
#include "srsapps/ue/mac/mac_pdu.h"

#ifndef DEMUX_H
#define DEMUX_H

/* Logical Channel Demultiplexing and MAC CE dissassemble */   

namespace srslte {
namespace ue {

class demux
{
public:
  demux();
  void init(phy* phy_h_, log* log_h_, mac_io* mac_io_h_, timers* timers_db_);

  void     push_pdu(uint32_t tti, uint8_t *mac_pdu, uint32_t nof_bits);
  void     push_pdu_bcch(uint32_t tti, uint8_t *mac_pdu, uint32_t nof_bits);
  void     push_pdu_temp_crnti(uint32_t tti, uint8_t *mac_pdu, uint32_t nof_bits);
  bool     is_temp_crnti_pending();
  void     demultiplex_pending_pdu(uint32_t tti);

  uint64_t get_contention_resolution_id();
  
private:
  mac_pdu mac_msg;
  mac_pdu pending_mac_msg;
  
  void process_pdu(mac_pdu *pdu);
  bool process_ce(mac_pdu::mac_subh *subheader);

  uint64_t   contention_resolution_id; 
  bool       pending_temp_rnti;

  phy        *phy_h; 
  log        *log_h;
  mac_io     *mac_io_h; 
  timers     *timers_db; 
  uint32_t    tti;
};
}
}

#endif



