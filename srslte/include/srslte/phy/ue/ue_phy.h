/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The libLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the libLTE library.
 *
 * libLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * libLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "ue_mac_itf.h"
#include "srslte/phy/utils/queue.h"

#ifndef UEPHY_H
#define UEPHY_H

typedef _Complex float cf_t; 

class ue_phy
{
public:
 
  class pdcch_ul_search : public queue::element {
  public: 
    set_crnti(uint16_t rnti);
    set_crnti_ra_procedure(uint16_t rnti);
    set_sps_crnti(uint16_t rnti);
    set_temporal_crnti(uint16_t rnti);
    set_tpc_pusch(uint16_t rnti);
    set_tpc_pucch(uint16_t rnti);
  private: 
    uint16_t rnti; 
    bool     is_crnti              = false; 
    bool     is_crnti_ra_procedure = false; 
    bool     is_sps_crnti          = false; 
    bool     is_t_crnti            = false; 
    bool     is_tpc_pusch          = false;
    bool     is_tpc_pucch          = false; 
  };

  class pdcch_dl_search : public queue::element {
  public: 
    set_crnti(uint16_t rnti);
    set_crnti_ra_procedure(uint16_t rnti);
    set_sps_crnti(uint16_t rnti);
    set_temporal_crnti(uint16_t rnti);
    set_tpc_pusch(uint16_t rnti);
    set_tpc_pucch(uint16_t rnti);
  private: 
    uint16_t rnti; 
    bool     is_crnti              = false; 
    bool     is_crnti_ra_procedure = false; 
    bool     is_sps_crnti          = false; 
    bool     is_t_crnti            = false; 
    bool     is_tpc_pusch          = false;
    bool     is_tpc_pucch          = false; 
  };
  
  class ul_allocation {
    
  };
  
  class ul_assignment : public queue::element {
  public: 
    ul_assignment(uint32_t nof_prb, uint32_t max_ulsch_bits);
    void     set_allocation(ul_allocation allocation);
    uint8_t* get_ulsch_buffer(); 
    cf_t*    get_signal_buffer();
    void     set_uci_data();
    void     generate_signal();    
  private: 
    cf_t*         signal_buffer = NULL; 
    uint8_t*      ulsch_buffer; 
    ul_allocation allocation; 
  };
  
  class dl_allocation {
    
  };

  class dl_assignment : public queue::element {
  public:
    void     set_allocation(dl_allocation allocation);
  private: 
    dl_allocation allocation; 
  };

  class phich_assignment {
    
  };

  class phich_assignment : public queue::element {
  public:
    phich_assignment();
    void     set_allocation(phich_assignment assignment);
  private: 
    phich_assignment assignment;
  };

  class dl_tb : public queue::element {
  public:
    dl_tb(uint32_t max_dlsch_bits);
    uint16_t get_rnti();
    bool     is_crc_valid();    
    uint8_t* get_dlsch_payload();
  private:
    uint8_t* dlsch_buffer; 
    uint16_t rnti; 
    bool     crc_result;
  };

  class dl_grant : public queue::element {
  public:
    dl_allocation get_allocation();
  private: 
    dl_allocation allocation; 
  };
  
  class ul_grant : public queue::element {
  public:
    ul_allocation get_allocation();
  private: 
    ul_allocation allocation; 

  };

  class ul_ack : public queue::element {
  public: 
    bool get_ack();
  private: 
    bool ack; 
  };
  
  class rx_buffer : public queue::element {
  public:
    rx_buffer(uint32_t nof_prb);
    cf_t* get_signal_buffer();
  private: 
    uint32_t nof_prb; 
    cf_t*    signal_buffer; 
  };

  ue_phy(ue_mac_itf *mac);
  ~ue_phy();
  
  void measure(); // TBD  
  void dl_bch();  
  void start_rxtx();
  void stop_rxtx();
  void init_prach();
  void send_prach(/* prach_cfg_t in prach.h with power, seq idx, etc */);  
  void set_param(); 
  
  pdcch_ul_search*    get_pdcch_ul_search(uint32_t tti);
  pdcch_dl_search*    get_pdcch_dl_search(uint32_t tti);
  ul_assignment*      get_ul_assignment(uint32_t tti);
  dl_assignment*      get_dl_assignment(uint32_t tti);
  phich_assignment*   get_phich_assignment(uint32_t tti);
  dl_tb*              get_dl_tb(uint32_t tti);
  dl_grant*           get_dl_grant(uint32_t tti);
  ul_grant*           get_ul_grant(uint32_t tti);
  ul_ack*             get_ul_ack(uint32_t tti);
    
private:
  enum {
    IDLE, MEASURE, RX_BCH, RXTX
  } phy_state; 

  bool prach_initiated     = false;   
  bool prach_ready_to_send = false;   

  queue pdcch_ul_search; 
  queue pdcch_dl_search; 
  queue ul_assignment; 
  queue dl_assignment; 
  queue phich_assignment; 
  queue dl_tb; 
  queue dl_grant; 
  queue ul_grant; 
  queue ul_grant; 
  queue rx_buffer; 
  
};
  
#endif