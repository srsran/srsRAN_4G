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
 * MERCHANTABILITY or FITNESS FOR A PARTICRXAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */


#include "srslte/upper/rlc.h"
#include "srslte/upper/rlc_tm.h"
#include "srslte/upper/rlc_um.h"
#include "srslte/upper/rlc_am.h"

namespace srslte {

rlc::rlc()
{
  pool = byte_buffer_pool::get_instance();
  rlc_log = NULL;
  pdcp = NULL;
  rrc = NULL;
  mac_timers = NULL;
  ue = NULL;
  default_lcid = 0;
  bzero(metrics_time, sizeof(metrics_time));
  bzero(ul_tput_bytes, sizeof(ul_tput_bytes));
  bzero(dl_tput_bytes, sizeof(dl_tput_bytes));
}

void rlc::init(srsue::pdcp_interface_rlc *pdcp_,
               srsue::rrc_interface_rlc  *rrc_,
               srsue::ue_interface       *ue_,
               log                       *rlc_log_, 
               mac_interface_timers      *mac_timers_,
               uint32_t                  lcid_)
{
  pdcp    = pdcp_;
  rrc     = rrc_;
  ue      = ue_;
  rlc_log = rlc_log_;
  mac_timers = mac_timers_;
  default_lcid = lcid_;

  gettimeofday(&metrics_time[1], NULL);
  reset_metrics(); 

  rlc_array[0].init(RLC_MODE_TM, rlc_log, default_lcid, pdcp, rrc, mac_timers); // SRB0
}

void rlc::reset_metrics() 
{
  bzero(dl_tput_bytes, sizeof(long)*SRSLTE_N_RADIO_BEARERS);
  bzero(ul_tput_bytes, sizeof(long)*SRSLTE_N_RADIO_BEARERS);
}

void rlc::stop()
{
  for(uint32_t i=0; i<SRSLTE_N_RADIO_BEARERS; i++) {
    if(rlc_array[i].active())
      rlc_array[i].stop();
  }
}

void rlc::get_metrics(rlc_metrics_t &m)
{
  
  gettimeofday(&metrics_time[2], NULL);
  get_time_interval(metrics_time);
  double secs = (double)metrics_time[0].tv_sec + metrics_time[0].tv_usec*1e-6;
  
  m.dl_tput_mbps = 0; 
  m.ul_tput_mbps = 0; 
  for (int i=0;i<SRSLTE_N_RADIO_BEARERS;i++) {
    m.dl_tput_mbps += (dl_tput_bytes[i]*8/(double)1e6)/secs;
    m.ul_tput_mbps += (ul_tput_bytes[i]*8/(double)1e6)/secs;    
    if(rlc_array[i].active()) {
      rlc_log->info("LCID=%d, RX throughput: %4.6f Mbps. TX throughput: %4.6f Mbps.\n",
                    i,
                    (dl_tput_bytes[i]*8/(double)1e6)/secs,
                    (ul_tput_bytes[i]*8/(double)1e6)/secs);
    }
  }

  memcpy(&metrics_time[1], &metrics_time[2], sizeof(struct timeval));
  reset_metrics();
}

void rlc::reestablish() {
  for(uint32_t i=0; i<SRSLTE_N_RADIO_BEARERS; i++) {
    if(rlc_array[i].active()) {
      rlc_array[i].reestablish();
    }
  }
}

void rlc::reset()
{
  for(uint32_t i=0; i<SRSLTE_N_RADIO_BEARERS; i++) {
    if(rlc_array[i].active())
      rlc_array[i].reset();
  }

  rlc_array[0].init(RLC_MODE_TM, rlc_log, default_lcid, pdcp, rrc, mac_timers); // SRB0
}

void rlc::empty_queue()
{
  for(uint32_t i=0; i<SRSLTE_N_RADIO_BEARERS; i++) {
    if(rlc_array[i].active())
      rlc_array[i].empty_queue();
  }
}

/*******************************************************************************
  PDCP interface
*******************************************************************************/
void rlc::write_sdu(uint32_t lcid, byte_buffer_t *sdu)
{
  if(valid_lcid(lcid)) {
    rlc_array[lcid].write_sdu(sdu);
  }
}

bool rlc::rb_is_um(uint32_t lcid) {
  return rlc_array[lcid].get_mode()==RLC_MODE_UM;
}

/*******************************************************************************
  MAC interface
*******************************************************************************/
uint32_t rlc::get_buffer_state(uint32_t lcid)
{
  if(valid_lcid(lcid)) {
    return rlc_array[lcid].get_buffer_state();
  } else {
    return 0;
  }
}

uint32_t rlc::get_total_buffer_state(uint32_t lcid)
{
  if(valid_lcid(lcid)) {
    return rlc_array[lcid].get_total_buffer_state();
  } else {
    return 0;
  }
}

int rlc::read_pdu(uint32_t lcid, uint8_t *payload, uint32_t nof_bytes)
{
  if(valid_lcid(lcid)) {
    ul_tput_bytes[lcid] += nof_bytes;
    return rlc_array[lcid].read_pdu(payload, nof_bytes);
  }
  return 0;
}

void rlc::write_pdu(uint32_t lcid, uint8_t *payload, uint32_t nof_bytes)
{
  if(valid_lcid(lcid)) {
    dl_tput_bytes[lcid] += nof_bytes;
    rlc_array[lcid].write_pdu(payload, nof_bytes);
  }
}

void rlc::write_pdu_bcch_bch(uint8_t *payload, uint32_t nof_bytes)
{
  rlc_log->info_hex(payload, nof_bytes, "BCCH BCH message received.");
  dl_tput_bytes[0] += nof_bytes;
  byte_buffer_t *buf = pool_allocate;
  if (buf) {
    memcpy(buf->msg, payload, nof_bytes);
    buf->N_bytes = nof_bytes;
    buf->set_timestamp();
    pdcp->write_pdu_bcch_bch(buf);
  } else {
    rlc_log->error("Fatal error: Out of buffers from the pool in write_pdu_bcch_bch()\n");
  }
}

void rlc::write_pdu_bcch_dlsch(uint8_t *payload, uint32_t nof_bytes)
{
  rlc_log->info_hex(payload, nof_bytes, "BCCH TXSCH message received.");
  dl_tput_bytes[0] += nof_bytes;
  byte_buffer_t *buf = pool_allocate;
  if (buf) {
    memcpy(buf->msg, payload, nof_bytes);
    buf->N_bytes = nof_bytes;
    buf->set_timestamp();
    pdcp->write_pdu_bcch_dlsch(buf);
  } else {
    rlc_log->error("Fatal error: Out of buffers from the pool in write_pdu_bcch_dlsch()\n");
  }
}

void rlc::write_pdu_pcch(uint8_t *payload, uint32_t nof_bytes)
{
  rlc_log->info_hex(payload, nof_bytes, "PCCH message received.");
  dl_tput_bytes[0] += nof_bytes;
  byte_buffer_t *buf = pool_allocate;
  if (buf) {
    memcpy(buf->msg, payload, nof_bytes);
    buf->N_bytes = nof_bytes;
    buf->set_timestamp();
    pdcp->write_pdu_pcch(buf);
  } else {
    rlc_log->error("Fatal error: Out of buffers from the pool in write_pdu_pcch()\n");
  }
}

/*******************************************************************************
  RRC interface
*******************************************************************************/
void rlc::add_bearer(uint32_t lcid)
{
  // No config provided - use defaults for SRB1 and SRB2
  if(lcid < 3) {
    if (!rlc_array[lcid].active()) {
      LIBLTE_RRC_RLC_CONFIG_STRUCT cnfg;
      cnfg.rlc_mode                     = LIBLTE_RRC_RLC_MODE_AM;
      cnfg.ul_am_rlc.t_poll_retx        = LIBLTE_RRC_T_POLL_RETRANSMIT_MS45;
      cnfg.ul_am_rlc.poll_pdu           = LIBLTE_RRC_POLL_PDU_INFINITY;
      cnfg.ul_am_rlc.poll_byte          = LIBLTE_RRC_POLL_BYTE_INFINITY;
      cnfg.ul_am_rlc.max_retx_thresh    = LIBLTE_RRC_MAX_RETX_THRESHOLD_T4;
      cnfg.dl_am_rlc.t_reordering       = LIBLTE_RRC_T_REORDERING_MS35;
      cnfg.dl_am_rlc.t_status_prohibit  = LIBLTE_RRC_T_STATUS_PROHIBIT_MS0;
      add_bearer(lcid, srslte_rlc_config_t(&cnfg));
    } else {
      rlc_log->warning("Bearer %s already configured. Reconfiguration not supported\n", rrc->get_rb_name(lcid).c_str());
    }
  }else{
    rlc_log->error("Radio bearer %s does not support default RLC configuration.\n", rrc->get_rb_name(lcid).c_str());
  }
}

void rlc::add_bearer(uint32_t lcid, srslte_rlc_config_t cnfg)
{
  if(lcid >= SRSLTE_N_RADIO_BEARERS) {
    rlc_log->error("Radio bearer id must be in [0:%d] - %d\n", SRSLTE_N_RADIO_BEARERS, lcid);
    return;
  }

  if (!rlc_array[lcid].active()) {
    rlc_log->warning("Adding radio bearer %s with mode %s\n",
                  rrc->get_rb_name(lcid).c_str(), liblte_rrc_rlc_mode_text[cnfg.rlc_mode]);
    switch(cnfg.rlc_mode)
    {
    case LIBLTE_RRC_RLC_MODE_AM:
      rlc_array[lcid].init(RLC_MODE_AM, rlc_log, lcid, pdcp, rrc, mac_timers);
      break;
    case LIBLTE_RRC_RLC_MODE_UM_BI:
      rlc_array[lcid].init(RLC_MODE_UM, rlc_log, lcid, pdcp, rrc, mac_timers);
      break;
    case LIBLTE_RRC_RLC_MODE_UM_UNI_DL:
      rlc_array[lcid].init(RLC_MODE_UM, rlc_log, lcid, pdcp, rrc, mac_timers);
      break;
    case LIBLTE_RRC_RLC_MODE_UM_UNI_UL:
      rlc_array[lcid].init(RLC_MODE_UM, rlc_log, lcid, pdcp, rrc, mac_timers);
      break;
    default:
      rlc_log->error("Cannot add RLC entity - invalid mode\n");
      return;
    }
  } else {
    rlc_log->warning("Bearer %s already created.\n", rrc->get_rb_name(lcid).c_str());
  }
  rlc_array[lcid].configure(cnfg);    

}

/*******************************************************************************
  Helpers
*******************************************************************************/
bool rlc::valid_lcid(uint32_t lcid)
{
  if(lcid >= SRSLTE_N_RADIO_BEARERS) {
    rlc_log->warning("Invalid LCID=%d\n", lcid);
    return false;
  } else if(!rlc_array[lcid].active()) {
    return false;
  }
  return true;
}


} // namespace srsue
