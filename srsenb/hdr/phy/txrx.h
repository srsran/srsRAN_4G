

#ifndef ENBTXRX_H
#define ENBTXRX_H

#include "srslte/common/log.h"
#include "srslte/common/threads.h"
#include "srslte/common/thread_pool.h"
#include "srslte/radio/radio.h"
#include "phy/phch_common.h"
#include "phy/prach_worker.h"

namespace srsenb {
    
typedef _Complex float cf_t; 

class txrx : public thread
{
public:
  txrx();
  bool init(srslte::radio *radio_handler, 
            srslte::thread_pool *_workers_pool, 
            phch_common *worker_com, 
            prach_worker *prach, 
            srslte::log *log_h, 
            uint32_t prio);
  void stop();
    
  const static int MUTEX_X_WORKER = 4; 
  
private:
    
  void run_thread(); 
  
  srslte::radio        *radio_h;
  srslte::log          *log_h;
  srslte::thread_pool  *workers_pool;
  prach_worker         *prach; 
  phch_common          *worker_com;
    
  uint32_t tx_mutex_cnt; 
  uint32_t nof_tx_mutex; 
  
  // Main system TTI counter   
  uint32_t tti; 
  
  bool running; 
};

} // namespace srsenb

#endif // UEPHY_H
