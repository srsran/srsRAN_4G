
#include <stdint.h>

#ifndef PROC_H
#define PROC_H

/* Interface for a MAC procedure */

namespace srslte {
namespace ue {
  
class proc
{
public:
  proc() {
    running = false; 
  }
  void run() {
    running = true; 
  }
  void stop() {
    running = false; 
  }
  bool is_running() {
    return running; 
  }
  virtual void step(uint32_t tti) = 0; 
private:
  bool running; 
};
}
}

#endif