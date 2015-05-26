/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */



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
      
      SR_PUCCH_CONFIGURED,
      SR_TRANS_MAX,
      
      BSR_TIMER_PERIODIC,
      BSR_TIMER_RETX,
      
      HARQ_MAXTX,
      HARQ_MAXMSG3TX,
      
      PDSCH_RSPOWER,
      PDSCH_PB,
      
      
      NOF_PARAMS,    
    } mac_param_t;
    
  };
}
}

#endif
