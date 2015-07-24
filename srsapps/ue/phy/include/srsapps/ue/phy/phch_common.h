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


#include <pthread.h>
#include <string.h>
#include "srslte/srslte.h"
#include "srsapps/common/mac_interface.h"
#include "srsapps/radio/radio.h"
#include "srsapps/common/log.h"
#include "srsapps/ue/phy/phy_params.h"


#ifndef UEPHYWORKERCOMMON_H
#define UEPHYWORKERCOMMON_H

namespace srslte {                                                                                                                                                                                                                                                                                                                                                                                                                                                     
namespace ue {

/* Subclass that manages variables common to all workers */
  class phch_common {
  public:
    /* Common variables used by all phy workers */
    phy_params        *params_db; 
    log               *log_h; 
    mac_interface_phy *mac;

    phch_common();
    void init(phy_params *_params, log *_log, radio *_radio, mac_interface_phy *_mac);
    
    /* For RNTI searches, -1 means now or forever */
    
    void               set_ul_rnti(srslte_rnti_type_t type, uint16_t rnti_value, int tti_start = -1, int tti_end = -1);
    uint16_t           get_ul_rnti(uint32_t tti);
    srslte_rnti_type_t get_ul_rnti_type();

    void               set_dl_rnti(srslte_rnti_type_t type, uint16_t rnti_value, int tti_start = -1, int tti_end = -1);
    uint16_t           get_dl_rnti(uint32_t tti);
    srslte_rnti_type_t get_dl_rnti_type();
    
    void set_rar_grant(uint32_t tti, uint8_t grant_payload[SRSLTE_RAR_GRANT_LEN]);
    bool get_pending_rar(uint32_t tti, srslte_dci_rar_grant_t *rar_grant = NULL);
    
    void reset_pending_ack(uint32_t tti);
    void set_pending_ack(uint32_t tti, uint32_t I_lowest, uint32_t n_dmrs);   
    bool get_pending_ack(uint32_t tti);    
    bool get_pending_ack(uint32_t tti, uint32_t *I_lowest, uint32_t *n_dmrs);
        
    void worker_end(uint32_t tti, bool tx_enable, cf_t *buffer, uint32_t nof_samples, srslte_timestamp_t tx_time);
    
    bool sr_enabled; 
    
  private: 
    pthread_mutex_t    tx_mutex; 
    pthread_cond_t     tx_cvar; 
    uint32_t           tx_tti_cnt; 
    
    bool               is_first_of_burst;
    radio             *radio_h;
    float              cfo;
    
    
    bool               ul_rnti_active(uint32_t tti);
    bool               dl_rnti_active(uint32_t tti);
    uint16_t           ul_rnti, dl_rnti;  
    srslte_rnti_type_t ul_rnti_type, dl_rnti_type; 
    int                ul_rnti_start, ul_rnti_end, dl_rnti_start, dl_rnti_end; 
    
    float              time_adv_sec; 
    
    srslte_dci_rar_grant_t rar_grant; 
    bool                   rar_grant_pending; 
    uint32_t               rar_grant_tti; 
    
    typedef struct {
      bool enabled; 
      uint32_t I_lowest; 
      uint32_t n_dmrs;
    } pending_ack_t;
    pending_ack_t pending_ack[10];
    
    bool is_first_tx;
    
  };
  
}
}

#endif