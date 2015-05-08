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



#include <stdint.h>

#include "srsapps/ue/phy/phy.h"
#include "srsapps/common/log.h"
#include "srsapps/ue/mac/mac_params.h"
#include "srsapps/common/qbuff.h"
#include "srsapps/ue/mac/proc.h"
#include "srsapps/common/timers.h"
#include "srsapps/ue/mac/mux.h"
#include "srsapps/ue/mac/demux.h"
#include "srsapps/ue/mac/mac_pdu.h"

#ifndef PROCRA_H
#define PROCRA_H

/* Random access procedure as specified in Section 5.1 of 36.321 */

namespace srslte {
namespace ue {

class ra_proc : public proc,timer_callback
{
  public:
    ra_proc() : rar_pdu(20) {};
    bool init(mac_params *params_db, phy *phy_h, log *log_h, timers *timers_db, 
              mux *mux_unit, demux *demux_unit);
    void reset();
    void start_pdcch_order();
    void start_rlc_order();
    void start_mac_order();
    void step(uint32_t tti);
    bool is_successful(); 
    bool is_contention_resolution(); 
    bool is_error(); 
    bool in_progress();
    void pdcch_to_crnti(bool is_ul_grant);
    void timer_expired(uint32_t timer_id);
private: 
    
    void process_timeadv_cmd(uint32_t tti, uint32_t ta_cmd); 
    void step_initialization();
    void step_resource_selection();
    void step_preamble_transmission();
    void step_response_reception();
    void step_response_error();
    void step_backoff_wait();
    void step_contention_resolution();
    void step_completition();

    //  Buffer to receive RAR PDU 
    static const uint32_t MAX_RAR_PDU_LEN = 2048;
    uint8_t     rar_pdu_buffer[MAX_RAR_PDU_LEN];
    mac_rar_pdu rar_pdu; 
    
    // Random Access parameters provided by higher layers defined in 5.1.1
    // They are read from params_db during initialization init()    
    uint32_t configIndex;
    uint32_t nof_preambles; 
    uint32_t nof_groupA_preambles;
    uint32_t nof_groupB_preambles;
    uint32_t messagePowerOffsetGroupB;
    uint32_t messageSizeGroupA;
    uint32_t Pcmax;
    uint32_t deltaPreambleMsg3;
    uint32_t responseWindowSize;
    uint32_t powerRampingStep;
    uint32_t preambleTransMax;
    uint32_t iniReceivedTargetPower;
    int      delta_preamble_db; 
    uint32_t maxharq_msg3tx;
    uint32_t contentionResolutionTimer; 
    uint32_t maskIndex; 
    int      preambleIndex;
    
    // Internal variables
    uint32_t tti; 
    uint32_t preambleTransmissionCounter; 
    uint32_t backoff_param_ms; 
    uint32_t sel_maskIndex; 
    uint32_t sel_preamble; 
    uint32_t backoff_interval_start;
    uint32_t backoff_inteval;
    int      received_target_power_dbm; 
    uint32_t ra_rnti; 
    uint8_t  payload[256]; // 56 bits is often enough
    
    enum {
      IDLE = 0,
      INITIALIZATION,       // Section 5.1.1
      RESOURCE_SELECTION,       // Section 5.1.2
      PREAMBLE_TRANSMISSION,    // Section 5.1.3
      RESPONSE_RECEPTION,       // Section 5.1.4
      RESPONSE_ERROR,
      BACKOFF_WAIT,
      CONTENTION_RESOLUTION,    // Section 5.1.5
      COMPLETION,               // Section 5.1.6
      RA_PROBLEM                // Section 5.1.5 last part
    } state; 
    
    typedef enum {RA_GROUP_A, RA_GROUP_B} ra_group_t;
    
    ra_group_t  last_msg3_group; 
    bool        msg3_transmitted; 
    bool        first_rar_received; 
    void        read_params();
    
    phy         *phy_h; 
    log         *log_h; 
    mac_params  *params_db;
    timers      *timers_db;
    mux         *mux_unit; 
    demux       *demux_unit; 
    
    uint64_t    received_contention_id;
    uint64_t    transmitted_contention_id;
    uint16_t    transmitted_crnti; 
    enum {
       PDCCH_CRNTI_NOT_RECEIVED = 0, 
       PDCCH_CRNTI_UL_GRANT, 
       PDCCH_CRNTI_DL_GRANT      
    } pdcch_to_crnti_received;

    enum {
      PDCCH_ORDER = 0, 
      RLC_ORDER, 
      MAC_ORDER
    } start_mode; 
};
}
}

#endif