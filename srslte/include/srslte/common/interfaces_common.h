
#include "common/timers.h"

#ifndef INTERFACE_COMMON_H
#define INTERFACE_COMMON_H

namespace srslte {

class mac_interface_timers
{
public: 
  /* Timer services with ms resolution. 
   * timer_id must be lower than MAC_NOF_UPPER_TIMERS
   */
  virtual timers::timer* get(uint32_t timer_id) = 0;
  virtual uint32_t               get_unique_id() = 0;
};

class read_pdu_interface
{
public:
  virtual int read_pdu(uint32_t lcid, uint8_t *payload, uint32_t requested_bytes) = 0; 
};

}

#endif
