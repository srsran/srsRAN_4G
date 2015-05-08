
#include <stdint.h>

#include "srsapps/ue/mac/proc.h"

#ifndef PROCPHR_H
#define PROCPHR_H

/* Power headroom report procedure */

namespace srslte {
namespace ue {

class phr_proc : public proc
{
public:
  void step(uint32_t tti) {
    if (is_running()) {
      fprintf(stderr, "PHR procedure not implemented\n");          
    }
  }
  void reset() {
    
  }
};
}
}

#endif