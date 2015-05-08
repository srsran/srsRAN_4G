

#include "srsapps/common/log.h"
#include "srsapps/ue/phy/phy.h"
#include "srsapps/ue/mac/mac_params.h"
#include "srsapps/common/timers.h"

#ifndef DLSPS_H
#define DLSPS_H

/* Downlink Semi-Persistent schedulign (Section 5.10.1) */

namespace srslte {
namespace ue {
  
typedef _Complex float cf_t; 

class dl_sps
{
public:

  void            clear(); 
  void            reset(uint32_t tti, dl_sched_grant *grant); 
  dl_sched_grant *get_pending_grant(uint32_t tti); 
private:  
  
};

} 
}
#endif