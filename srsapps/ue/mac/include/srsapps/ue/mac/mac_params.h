
#include "srslte/srslte.h"
#include "srsapps/common/params_db.h"

#ifndef MACPARAMS_H
#define MACPARAMS_H


namespace srslte {
namespace ue {

  class mac_params : public params_db
  {
  public: 

    mac_params() : params_db(NOF_PARAMS) {}
   ~mac_params() {}
    
    typedef enum {
      
      // These 4 parameters must be together!!
      RNTI_C = 0, 
      RNTI_SPS,
      RNTI_TEMP,
      RNTI_RA,
      
      SPS_DL_SCHED_INTERVAL,
      SPS_DL_NOF_PROC,
      
      BCCH_SI_WINDOW_ST,
      BCCH_SI_WINDOW_LEN,

      PCCH_RECEIVE,
      
      CONTENTION_ID, // Transmitted UE Contention ID
      
      TIMER_TIMEALIGN,
      
      // Random Access parameters. See 5.1.1
      RA_CONFIGINDEX,
      RA_PREAMBLEINDEX,
      RA_MASKINDEX,
      RA_NOFPREAMBLES,
      RA_NOFGROUPAPREAMBLES,
      RA_MESSAGEPOWEROFFSETB,
      RA_MESSAGESIZEA,
      RA_PCMAX,
      RA_DELTAPREAMBLEMSG3,
      RA_RESPONSEWINDOW,
      RA_POWERRAMPINGSTEP,
      RA_PREAMBLETRANSMAX,
      RA_INITRECEIVEDPOWER,
      RA_CONTENTIONTIMER,
      RA_MAXTXMSG3,
      
      PDSCH_RSPOWER,
      PDSCH_PB,
      
      
      NOF_PARAMS,    
    } mac_param_t;
    
  };
}
}

#endif
