

#include "srsapps/common/log.h"
#include "srsapps/ue/phy/phy.h"
#include "srsapps/ue/mac/mac_params.h"
#include "srsapps/common/timers.h"

#ifndef ULSPS_H
#define ULSPS_H

/* Uplink Semi-Persistent schedulign (Section 5.10.2) */

namespace srslte {
namespace ue {
  
typedef _Complex float cf_t; 

class ul_sps
{
public:

  void           clear() {}
  void           reset(uint32_t tti, sched_grant *grant) {}
  ul_sched_grant *get_pending_grant(uint32_t tti) { return NULL; }
private:  
  
};

} 
}
#endif