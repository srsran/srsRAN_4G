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
#include "srsapps/common/phy_interface.h"
#include "srsapps/common/log.h"
#include "srsapps/ue/phy/phch_recv.h"
#include "srsapps/ue/phy/prach.h"
#include "srsapps/ue/phy/phy_params.h"
#include "srsapps/ue/phy/phch_worker.h"
#include "srsapps/ue/phy/phch_common.h"
#include "srsapps/radio/radio.h"
#include "srsapps/common/task_dispatcher.h"
#include "srsapps/common/trace.h"
#include "srsapps/common/mac_interface.h"

#ifndef UEPHY_H
#define UEPHY_H

namespace srslte {
namespace ue {
    
typedef _Complex float cf_t; 

class phy : public phy_interface, phy_interface_params
{
public:
  phy();
  bool init(radio *radio_handler, mac_interface_phy *mac, log *log_h);
  bool init_agc(radio *radio_handler, mac_interface_phy *mac, log *log_h);
  void stop();
  
  void set_crnti(uint16_t rnti);
  
  // Get status 
  bool status_is_sync();
  
  static uint32_t tti_to_SFN(uint32_t tti);
  static uint32_t tti_to_subf(uint32_t tti);

  void enable_pregen_signals(bool enable); 
  
  void start_trace();
  void write_trace(std::string filename); 
  
  /********** MAC INTERFACE ********************/
  /* Instructs the PHY to configure using the parameters written by set_param() */
  void    configure_prach_params();
  void    configure_ul_params();

  /* Functions to synchronize with a cell */
  void    sync_start(); 
  void    sync_stop();
  
  /* Transmits PRACH in the next opportunity */
  void    prach_send(uint32_t preamble_idx, int allowed_subframe = -1, float target_power_dbm = 0.0);  
  int     prach_tx_tti();
  
  /* Indicates the transmission of a SR signal in the next opportunity */
  void    sr_send();  
  int     sr_last_tx_tti();

  // Time advance commands
  void    set_timeadv_rar(uint32_t ta_cmd);
  void    set_timeadv(uint32_t ta_cmd);
  
  /* Sets RAR grant payload */
  void    set_rar_grant(uint32_t tti, uint8_t grant_payload[SRSLTE_RAR_GRANT_LEN]); 

  /* Instruct the PHY to decode PDCCH with the CRC scrambled with given RNTI */  
  void    pdcch_ul_search(srslte_rnti_type_t rnti_type, uint16_t rnti, int tti_start = -1, int tti_end = -1);
  void    pdcch_dl_search(srslte_rnti_type_t rnti_type, uint16_t rnti, int tti_start = -1, int tti_end = -1);
  void    pdcch_ul_search_reset();
  void    pdcch_dl_search_reset();

  /* Get/Set PHY parameters */  
  void    set_param(phy_param_t param, int64_t value); 
  int64_t get_param(phy_param_t param);

  void    reset();
  
  uint32_t get_current_tti();
  void     get_current_cell(srslte_cell_t *cell);
  
private:
    
  const static int NOF_WORKERS         = 1; 
  const static int SF_RECV_THREAD_PRIO = 1;
  const static int WORKERS_THREAD_PRIO = 0; 
  
  radio         *radio_handler;
  log           *log_h; 

  thread_pool              workers_pool; 
  std::vector<phch_worker> workers;
  phch_common              workers_common; 
  phch_recv                sf_recv; 
  prach                    prach_buffer; 
  
  phy_params   params_db; 
  
  /* Current time advance */
  uint32_t     n_ta;
  
  bool         init_(radio *radio_handler, mac_interface_phy *mac, log *log_h, bool do_agc);

  trace<uint32_t> tr_start_time;
  trace<uint32_t> tr_end_time;
  bool tr_enabled; 
  void tr_log_start();
  void tr_log_end();    
};

} 
}
#endif
