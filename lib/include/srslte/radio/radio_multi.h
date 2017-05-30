

#include <string.h>

#include "srslte/srslte.h"
extern "C" {
#include "srslte/phy/rf/rf.h"
}
#include "srslte/common/trace.h"

#include "srslte/radio/radio.h"

#ifndef RADIO_MULTI_H
#define RADIO_MULTI_H


namespace srslte {
  
/* Interface to the RF frontend. 
  */
  class radio_multi : public radio
  {
    public: 
      
      bool init_multi(uint32_t nof_rx_antennas, char *args = NULL, char *devname = NULL);
      bool rx_now(cf_t *buffer[SRSLTE_MAX_PORTS], uint32_t nof_samples, srslte_timestamp_t *rxd_time);
  }; 
}

#endif
