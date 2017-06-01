#ifndef PRACH_WORKER_H
#define PRACH_WORKER_H

#include "srslte/interfaces/enb_interfaces.h"
#include "srslte/common/log.h"
#include "srslte/common/threads.h"

namespace srsenb {
  
class prach_worker : thread
{
public:
  prach_worker() : initiated(false),max_prach_offset_us(0) {}
  
  int  init(srslte_cell_t *cell, srslte_prach_cfg_t *prach_cfg, mac_interface_phy *mac, srslte::log *log_h, int priority);
  int  new_tti(uint32_t tti, cf_t *buffer);
  void set_max_prach_offset_us(float delay_us);
  void stop();
  
private:
  void run_thread();
  int run_tti(uint32_t tti); 
  
  uint32_t prach_nof_det; 
  uint32_t prach_indices[165]; 
  float    prach_offsets[165]; 
  float    prach_p2avg[165];
 
  srslte_cell_t cell; 
  srslte_prach_cfg_t prach_cfg;
  srslte_prach_t  prach;

  pthread_mutex_t mutex;
  pthread_cond_t  cvar;

  cf_t *signal_buffer_rx;
  
  srslte::log* log_h;
  mac_interface_phy *mac;
  float max_prach_offset_us;
  bool initiated;
  uint32_t pending_tti;
  int processed_tti;
  bool running;
  uint32_t nof_sf;
  uint32_t sf_cnt;
};
}
#endif // PRACH_WORKER_H
