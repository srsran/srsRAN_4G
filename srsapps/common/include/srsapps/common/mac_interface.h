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
#include <string>
#include "srslte/srslte.h"

/******************************************************************************
 *  File:         mac_interface.h
 *
 *  Description:  LTE MAC layer interface
 *
 *  Reference:
 *****************************************************************************/

#ifndef MAC_INTERFACE_H
#define MAC_INTERFACE_H

namespace srslte {
namespace ue {
  
/* Interface PHY -> MAC */
class mac_interface_phy
{
public:
    
  typedef struct {
    uint32_t    pid;    
    uint32_t    tti;
    bool        ndi; 
    uint32_t    n_bytes;
    uint32_t    rv; 
    uint16_t    rnti; 
    bool        is_from_rar;
    bool        is_sps_release; 
    srslte_rnti_type_t rnti_type; 
    srslte_phy_grant_t phy_grant; 
  } mac_grant_t; 
  
  typedef struct {
    bool                    decode_enabled;
    uint32_t                rv;
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
  } tb_action_ul_t;
  
  /* Indicate reception of UL grant. 
   * payload_ptr points to memory where MAC PDU must be written by MAC layer */
  virtual void new_grant_ul(mac_grant_t grant, uint8_t *payload_ptr, tb_action_ul_t *action) = 0;

  /* Indicate reception of UL grant + HARQ information throught PHICH in the same TTI. */
  virtual void new_grant_ul_ack(mac_grant_t grant, uint8_t *payload_ptr, bool ack, tb_action_ul_t *action) = 0;

  /* Indicate reception of HARQ information only through PHICH.   */
  virtual void harq_recv(uint32_t tti, bool ack, tb_action_ul_t *action) = 0;
  
  /* Indicate reception of DL grant. */ 
  virtual void new_grant_dl(mac_grant_t grant, tb_action_dl_t *action) = 0;
  
  /* Indicate successfull decoding of PDSCH TB. */
  virtual void tb_decoded_ok(uint32_t harq_pid) = 0;
  
  /* Indicate successfull decoding of BCH TB through PBCH */
  virtual void bch_decoded_ok(uint8_t *payload, uint32_t len) = 0;  
  
  /* Function called every start of a subframe (TTI). Warning, this function is called 
   * from a high priority thread and should terminate asap 
   */
  virtual void tti_clock(uint32_t tti) = 0;
  
};

/* Interface RLC -> MAC */
class mac_interface_rlc
{
public:
 
  /* RLC configures a logical channel */
  virtual void    setup_lcid(uint32_t lcid, uint32_t lcg, uint32_t priority, int PBR_x_tti, uint32_t BSD) = 0;  
  
  virtual uint32_t get_current_tti() = 0;
  
  virtual void    reconfiguration() = 0; 
  virtual void    reset() = 0; 
};


/* Interface MAC -> RLC */
class rlc_interface_mac
{
public:
  /* MAC calls RLC to get buffer state for a logical channel. This function should return quickly */
  virtual uint32_t get_buffer_state(uint32_t lcid) = 0;
  
  /* MAC calls RLC to get RLC segment of nof_bytes length. Segmentation happens in this function. RLC PDU is stored in 
   * payload. */
  virtual uint32_t read_pdu(uint32_t lcid, uint8_t *payload, uint32_t nof_bytes) = 0;

  /* MAC calls RLC to push an RLC PDU. This function is called from an independent MAC thread. PDU gets placed into the 
   * PDCP buffer and higher layer thread gets notified 
   * when the last segment is received 
  */
  virtual void     write_pdu(uint32_t lcid, uint8_t *payload, uint32_t nof_bytes) = 0;
  virtual void     write_pdu_bcch_bch(uint8_t *payload, uint32_t nof_bytes) = 0;
  virtual void     write_pdu_bcch_dlsch(uint8_t *payload, uint32_t nof_bytes) = 0;

  
};

class mac_interface_params 
{
public: 
      typedef enum {
      
      SPS_DL_SCHED_INTERVAL,
      SPS_DL_NOF_PROC,
      
      RNTI_TEMP,
      RNTI_C,
      
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
  
  /* Sets/gets a parameter */
  virtual void    set_param(mac_param_t param, int64_t value) = 0; 
  virtual int64_t get_param(mac_param_t param) = 0;  
};

}
}

#endif
  
