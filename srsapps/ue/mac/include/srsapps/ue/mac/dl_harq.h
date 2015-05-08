

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
  ~dl_harq_entity();
  bool init(srslte_cell_t cell, uint32_t max_payload_len, srslte::log *log_h_, timers *timers_, demux *demux_unit);
  bool is_sps(uint32_t pid); 
  void set_harq_info(uint32_t tti, uint32_t pid, dl_sched_grant *grant);
  void receive_data(uint32_t tti, uint32_t pid, dl_buffer *dl_buffer, phy *phy_h);
  void reset();
  bool is_ack_pending_resolution();
  void send_pending_ack_contention_resolution();
private:  
  
  
  class dl_harq_process {
  public:
    dl_harq_process();
    bool init(srslte_cell_t cell, uint32_t max_payload_len, dl_harq_entity *parent);
    void set_harq_info(uint32_t tti, dl_sched_grant *grant); 
    void receive_data(uint32_t tti, dl_buffer *dl_buffer, phy *phy_h); 
    void reset();
    // Called after the contention resolution is terminated to send pending ACKs, if any
    void send_pending_ack_contention_resolution();
    uint32_t pid;    
  private: 
    
    dl_harq_entity *harq_entity; 
    uint8_t        *payload; 
    uint32_t       max_payload_len; 
    dl_sched_grant cur_grant;
    dl_sched_grant pending_ack_grant;
    ul_buffer     *pending_ul_buffer;
    bool           is_first_tx; 
    bool           is_first_decoded; 
    bool           pending_ack; 
    srslte::log    *log_h; 
    
    srslte_softbuffer_rx_t softbuffer; 
  };
  
  
  dl_harq_process *proc;
  timers          *timers_db; 
  demux           *demux_unit; 
  srslte::log     *log_h; 
  int              pending_ack_pid; 
};

} 
}
#endif