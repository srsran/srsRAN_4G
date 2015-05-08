
#include <stdint.h>

#include "srsapps/ue/mac/proc.h"

#ifndef PROCBSR_H
#define PROCBSR_H

/* Buffer status report procedure */

namespace srslte {
namespace ue {

class bsr_proc : public proc
{
public:
  void step(uint32_t tti) {
    if (is_running()) {
      fprintf(stderr, "BSR procedure not implemented\n");          
    }
  }
  void reset() {
    
  }

};
}
}

#endif