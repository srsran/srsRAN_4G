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

#include "srsapps/common/log.h"
#include "srsapps/ue/phy/phy.h"
#include "srsapps/ue/mac/mac_params.h"
#include "srsapps/ue/mac/dl_harq.h"
#include "srsapps/ue/mac/ul_harq.h"
#include "srsapps/common/timers.h"
#include "srsapps/ue/mac/proc_ra.h"
#include "srsapps/ue/mac/proc_sr.h"
#include "srsapps/ue/mac/proc_bsr.h"
#include "srsapps/ue/mac/proc_phr.h"
#include "srsapps/ue/mac/mux.h"
#include "srsapps/ue/mac/demux.h"
#include "srsapps/ue/mac/mac_pcap.h"
#include "srsapps/common/mac_interface.h"
#include "srsapps/common/tti_sync_cv.h"
#include "srsapps/common/threads.h"


#ifndef UEMAC_H
#define UEMAC_H

namespace srslte {
namespace ue {
  
class mac : public mac_interface_phy, mac_interface_rlc, thread, timer_callback, mac_interface_params
{
public:
  mac();
  bool init(phy_interface *phy, rlc_interface_mac *rlc, log *log_h);
  void stop();

  /******** Interface from PHY (PHY -> MAC) ****************/ 
  /* see mac_interface.h for comments */
  void new_grant_ul(mac_grant_t grant, tb_action_ul_t *action);
  void new_grant_ul_ack(mac_grant_t grant, bool ack, tb_action_ul_t *action);
  void harq_recv(uint32_t tti, bool ack, tb_action_ul_t *action);
  void new_grant_dl(mac_grant_t grant, tb_action_dl_t *action);
  void tb_decoded(bool ack, srslte_rnti_type_t rnti_type, uint32_t harq_pid);
  void bch_decoded_ok(uint8_t *payload, uint32_t len);  
  void tti_clock(uint32_t tti);

  
  /******** Interface from RLC (RLC -> MAC) ****************/ 
  void setup_lcid(uint32_t lcid, uint32_t lcg, uint32_t priority, int PBR_x_tti, uint32_t BSD);
  void reconfiguration(); 
  void reset(); 

  /******** MAC parameters  ****************/ 
  void    set_param(mac_interface_params::mac_param_t param, int64_t value); 
  int64_t get_param(mac_interface_params::mac_param_t param);
  
  void timer_expired(uint32_t timer_id); 
  void start_pcap(mac_pcap* pcap);
  
  uint32_t get_current_tti();
      
  
  enum {
    HARQ_RTT, 
    TIME_ALIGNMENT,
    CONTENTION_TIMER,
    BSR_TIMER_PERIODIC,
    BSR_TIMER_RETX,
    NOF_MAC_TIMERS
  } mac_timers_t; 
  
private:  
  void run_thread(); 
  void search_si_rnti();
  

  static const int MAC_THREAD_PRIO = 5; 

  // Interaction with PHY 
  tti_sync_cv        ttisync; 
  phy_interface     *phy_h; 
  rlc_interface_mac *rlc_h; 
  log               *log_h; 
  
  mac_params    params_db; 
  
  uint32_t      tti; 
  bool          started; 
  bool          is_synchronized; 
  uint16_t      last_temporal_crnti;
  uint16_t      phy_rnti;
  
  /* Multiplexing/Demultiplexing Units */
  mux           mux_unit; 
  demux         demux_unit; 
  
  /* DL/UL HARQ */  
  dl_harq_entity dl_harq; 
  ul_harq_entity ul_harq; 
  
  /* MAC Uplink-related Procedures */
  ra_proc       ra_procedure;
  sr_proc       sr_procedure; 
  bsr_proc      bsr_procedure; 
  phr_proc      phr_procedure; 
  
  /* Functions for MAC Timers */
  timers        timers_db; 
  void          setup_timers();
  void          timeAlignmentTimerExpire();

  // pointer to MAC PCAP object
  mac_pcap* pcap;
  bool si_search_in_progress;
  int si_window_length;
  int si_window_start;
  bool signals_pregenerated;
  
};

} 
}
#endif
