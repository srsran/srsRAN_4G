/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef ULHARQ_H
#define ULHARQ_H

#include "common/mac_interface.h"
#include "common/log.h"
#include "mac/mux.h"
#include "mac/ul_sps.h"
#include "common/mac_pcap.h"
#include "common/timers.h"

/* Uplink HARQ entity as defined in 5.4.2 of 36.321 */


namespace srsue {
  
class ul_harq_entity
{
public:

  const static uint32_t NOF_HARQ_PROC = 8; 
  static uint32_t pidof(uint32_t tti);
  
  ul_harq_entity() {  
    pcap        = NULL; 
    timers_db   = NULL; 
    mux_unit    = NULL; 
    log_h       = NULL; 
    mac_cfg     = NULL; 
    rntis       = NULL; 
    average_retx = 0; 
    nof_pkts     = 0; 
  }
  bool init(srslte::log *log_h, 
            mac_interface_rrc::ue_rnti_t *rntis, 
            mac_interface_rrc::mac_cfg_t *mac_cfg, 
            srslte::timers* timers_, 
            mux *mux_unit);
  void reset();
  void reset_ndi();

  void start_pcap(srslte::mac_pcap* pcap);

  
  /***************** PHY->MAC interface for UL processes **************************/
  void new_grant_ul(mac_interface_phy::mac_grant_t grant, mac_interface_phy::tb_action_ul_t *action);
  void new_grant_ul_ack(mac_interface_phy::mac_grant_t grant, bool ack, mac_interface_phy::tb_action_ul_t *action);
  void harq_recv(uint32_t tti, bool ack, mac_interface_phy::tb_action_ul_t *action);

  int get_current_tbs(uint32_t tti);
  
  float get_average_retx(); 
    
private:  

  class ul_harq_process {
  public:
    ul_harq_process();
    bool init(uint32_t pid, ul_harq_entity *parent);
    void reset();
    void reset_ndi();
    
    void run_tti(uint32_t tti, mac_interface_phy::mac_grant_t *grant, mac_interface_phy::tb_action_ul_t* action);

    uint32_t get_rv();
    bool has_grant();

    void set_harq_feedback(bool ack);
    bool get_ndi();
    bool is_sps();
    uint32_t last_tx_tti();
    uint32_t get_nof_retx();
    int get_current_tbs();
   
  private: 
    mac_interface_phy::mac_grant_t cur_grant;
    
    uint32_t                    pid;
    uint32_t                    current_tx_nb;
    uint32_t                    current_irv; 
    bool                        harq_feedback; 
    bool                        ndi; 
    srslte::log                 *log_h;
    ul_harq_entity              *harq_entity; 
    bool                        is_grant_configured; 
    srslte_softbuffer_tx_t      softbuffer; 
    bool                        is_msg3;
    bool                        is_initiated;    
    uint32_t                    tti_last_tx;
    
    
    const static int payload_buffer_len = 128*1024; 
    uint8_t *payload_buffer;
    uint8_t *pdu_ptr; 
    
    void generate_retx(uint32_t tti_tx, mac_interface_phy::tb_action_ul_t *action); 
    void generate_retx(uint32_t tti_tx, mac_interface_phy::mac_grant_t *grant, 
                                        mac_interface_phy::tb_action_ul_t *action); 
    void generate_new_tx(uint32_t tti_tx, bool is_msg3, mac_interface_phy::mac_grant_t *grant, 
                         mac_interface_phy::tb_action_ul_t *action);
    void generate_tx(uint32_t tti_tx, mac_interface_phy::tb_action_ul_t *action);
  };

  
  void run_tti(uint32_t tti, mac_interface_phy::mac_grant_t *grant, mac_interface_phy::tb_action_ul_t* action);
  void set_ack(uint32_t tti, bool ack);
  
  ul_sps           ul_sps_assig;

  srslte::timers  *timers_db;
  mux             *mux_unit;
  ul_harq_process proc[NOF_HARQ_PROC];
  srslte::log     *log_h;
  srslte::mac_pcap *pcap; 

  mac_interface_rrc::ue_rnti_t *rntis;
  mac_interface_rrc::mac_cfg_t *mac_cfg; 
  
  float            average_retx;   
  uint64_t         nof_pkts; 
};

} // namespace srsue

#endif // ULHARQ_H
