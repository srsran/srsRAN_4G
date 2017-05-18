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

/******************************************************************************
 *  File:         mac_interface.h
 *  Description:  LTE MAC layer interface
 *  Reference:
 *****************************************************************************/

#ifndef MAC_INTERFACE_H
#define MAC_INTERFACE_H

#include <stdint.h>
#include <string>
#include "srslte/srslte.h"

#include "common/interfaces_common.h"
#include "common/timers.h"

#include "liblte_rrc.h"

namespace srsue {
  
/* Interface PHY -> MAC */
class mac_interface_phy
{
public:
    
  typedef struct {
    uint32_t    pid;    
    uint32_t    tti;
    uint32_t    last_tti;
    bool        ndi; 
    bool        last_ndi; 
    uint32_t    n_bytes;
    int         rv; 
    uint16_t    rnti; 
    bool        is_from_rar;
    bool        is_sps_release;
    bool        has_cqi_request;
    srslte_rnti_type_t rnti_type; 
    srslte_phy_grant_t phy_grant; 
  } mac_grant_t; 
  
  typedef struct {
    bool                    decode_enabled;
    int                     rv;
    uint16_t                rnti; 
    bool                    generate_ack; 
    bool                    default_ack; 
    // If non-null, called after tb_decoded_ok to determine if ack needs to be sent
    bool                  (*generate_ack_callback)(void*); 
    void                   *generate_ack_callback_arg;
    uint8_t                *payload_ptr; 
    srslte_softbuffer_rx_t *softbuffer;
    srslte_phy_grant_t      phy_grant;
  } tb_action_dl_t;

  typedef struct {
    bool                    tx_enabled;
    bool                    expect_ack;
    uint32_t                rv;
    uint16_t                rnti; 
    uint32_t                current_tx_nb;
    srslte_softbuffer_tx_t *softbuffer;
    srslte_phy_grant_t      phy_grant;
    uint8_t                *payload_ptr; 
  } tb_action_ul_t;
  
  /* Indicate reception of UL grant. 
   * payload_ptr points to memory where MAC PDU must be written by MAC layer */
  virtual void new_grant_ul(mac_grant_t grant, tb_action_ul_t *action) = 0;

  /* Indicate reception of UL grant + HARQ information throught PHICH in the same TTI. */
  virtual void new_grant_ul_ack(mac_grant_t grant, bool ack, tb_action_ul_t *action) = 0;

  /* Indicate reception of HARQ information only through PHICH.   */
  virtual void harq_recv(uint32_t tti, bool ack, tb_action_ul_t *action) = 0;
  
  /* Indicate reception of DL grant. */ 
  virtual void new_grant_dl(mac_grant_t grant, tb_action_dl_t *action) = 0;
  
  /* Indicate successfull decoding of PDSCH TB. */
  virtual void tb_decoded(bool ack, srslte_rnti_type_t rnti_type, uint32_t harq_pid) = 0;
  
  /* Indicate successfull decoding of BCH TB through PBCH */
  virtual void bch_decoded_ok(uint8_t *payload, uint32_t len) = 0;  
  
  /* Indicate successfull decoding of PCH TB through PDSCH */
  virtual void pch_decoded_ok(uint32_t len) = 0;  
  
  /* Function called every start of a subframe (TTI). Warning, this function is called 
   * from a high priority thread and should terminate asap 
   */
  virtual void tti_clock(uint32_t tti) = 0;
  
};


/* Interface RRC -> MAC */
class mac_interface_rrc
{
public:
  
  typedef struct {
    LIBLTE_RRC_MAC_MAIN_CONFIG_STRUCT           main; 
    LIBLTE_RRC_RACH_CONFIG_COMMON_STRUCT        rach;     
    LIBLTE_RRC_SCHEDULING_REQUEST_CONFIG_STRUCT sr; 
    uint32_t prach_config_index; 
  } mac_cfg_t; 

  
  // Class to handle UE specific RNTIs between RRC and MAC
  typedef struct {
    uint16_t crnti; 
    uint16_t temp_rnti; 
    uint16_t tpc_rnti; 
    uint16_t sps_rnti; 
    uint64_t contention_id; 
  } ue_rnti_t;  
  
  /* Instructs the MAC to start receiving BCCH */
  virtual void    bcch_start_rx() = 0; 
  virtual void    bcch_stop_rx() = 0; 
  virtual void    bcch_start_rx(int si_window_start, int si_window_length) = 0;

  /* Instructs the MAC to start receiving PCCH */
  virtual void    pcch_start_rx() = 0; 
  virtual void    pcch_stop_rx() = 0; 
  
  /* RRC configures a logical channel */
  virtual void    setup_lcid(uint32_t lcid, uint32_t lcg, uint32_t priority, int PBR_x_tti, uint32_t BSD) = 0;

  virtual uint32_t get_current_tti() = 0;
  
  virtual void set_config(mac_cfg_t *mac_cfg) = 0;
  virtual void set_config_main(LIBLTE_RRC_MAC_MAIN_CONFIG_STRUCT *main_cfg) = 0;
  virtual void set_config_rach(LIBLTE_RRC_RACH_CONFIG_COMMON_STRUCT *rach_cfg, uint32_t prach_config_index) = 0;
  virtual void set_config_sr(LIBLTE_RRC_SCHEDULING_REQUEST_CONFIG_STRUCT *sr_cfg) = 0;
  virtual void get_config(mac_cfg_t *mac_cfg) = 0;
  
  virtual void get_rntis(ue_rnti_t *rntis) = 0;
  virtual void set_contention_id(uint64_t uecri) = 0;

  
  virtual void reconfiguration() = 0;
  virtual void reset() = 0;
};

}


#endif
  
