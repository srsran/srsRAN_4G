#include <string.h>
#include "srsapps/common/qbuff.h"

#ifndef MACIO_H
#define MACIO_H

/* Manages all MAC buffers including: 
 *   - communication with higher layers through logical channels 
 *   - communication between logical channels buffers and Dissassembly/Assembly unit
 *   - communication between Dissassembly/Assembly unit and Msg3 buffer
 * 
 * For logical channels, IN means higher layers to MAC. OUT means MAC to higher layers. 
 * 
 * See queue.h for instructions on how to manage buffers 
 */

namespace srslte {
namespace ue {

  class mac_io
  {
  public: 
    
    // list of logical channels
    // Keep this order to match LoCH id with RB
    typedef enum {
      // Downlink (UE MAC -> UE RRC)      
      MAC_LCH_CCCH_DL = 0,   // SRB0
      MAC_LCH_DCCH0_DL,      // SRB1
      MAC_LCH_DCCH1_DL,      // SRB2
      MAC_LCH_DTCH0_DL,      // DRB0
      MAC_LCH_DTCH1_DL,      // DRB1
      MAC_LCH_DTCH2_DL,      // DRB2
      MAC_LCH_BCCH_DL,       // N/A
      MAC_LCH_PCCH_DL,       // N/A

      // Uplink   (UE RLC -> UE MAC)
      MAC_LCH_CCCH_UL,        // SRB0
      MAC_LCH_DCCH0_UL,       // SRB1
      MAC_LCH_DCCH1_UL,       // SRB2
      MAC_LCH_DTCH0_UL,       // DRB0
      MAC_LCH_DTCH1_UL,       // DRB1
      MAC_LCH_DTCH2_UL,       // DRB2
      
      MAC_NOF_QUEUES
    } mac_lch_t;

    const static int NOF_DL_LCH = MAC_LCH_PCCH_DL  - MAC_LCH_CCCH_DL;
    const static int NOF_UL_LCH = MAC_LCH_DTCH2_UL - MAC_LCH_CCCH_UL; 

    const static int DEFAULT_MSG_SZ  = 8*1024; // 8 Kbytes
    const static int DEFAULT_NOF_MESSAGES = 8;  
    

    qbuff* get(mac_lch_t ch) {      
      return get((uint32_t) ch);
    }

    qbuff* get(int32_t lchid) {      
      if (lchid < MAC_NOF_QUEUES) {
        return &queues[lchid]; 
      } else {
        return NULL; 
      }
    }
    
    // Move packets between queues with only 1 memcpy
    void move(mac_lch_t src, mac_lch_t dst) {
      get(src)->move_to(get(dst));
    }
    
    mac_io() {
      for (int i=0;i<MAC_NOF_QUEUES;i++) {
        queues[i].init(DEFAULT_NOF_MESSAGES, DEFAULT_MSG_SZ);
      }
    }
  
  private: 
    qbuff queues[MAC_NOF_QUEUES];
  };
}
}

#endif
