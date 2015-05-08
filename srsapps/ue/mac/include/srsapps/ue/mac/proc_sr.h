
#include <stdint.h>

#include "srsapps/ue/mac/proc.h"

#ifndef PROCSR_H
#define PROCSR_H

/* Scheduling Request procedure */

namespace srslte {
namespace ue {

class sr_proc : public proc
{
public:
  void step(uint32_t tti) {
    if (is_running()) {
      fprintf(stderr, "SR procedure not implemented\n");          
    }
  }
  void reset() {
    
  }

};
}
}

#endif