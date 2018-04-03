/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2017 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of srsLTE.
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

#include <iostream>
#include <string.h>

#include "srslte/interfaces/enb_interfaces.h"
#include "srsenb/hdr/mac/ue.h"

#define Error(fmt, ...)   log_h->error(fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) log_h->warning(fmt, ##__VA_ARGS__)
#define Info(fmt, ...)    log_h->info(fmt, ##__VA_ARGS__)
#define Debug(fmt, ...)   log_h->debug(fmt, ##__VA_ARGS__)


namespace srsenb {
  
void ue::config(uint16_t rnti_, uint32_t nof_prb, sched_interface *sched_, rrc_interface_mac *rrc_, rlc_interface_mac *rlc_, srslte::log *log_h_)
{
  rnti  = rnti_; 
  rlc   = rlc_; 
  rrc   = rrc_; 
  log_h = log_h_; 
  sched = sched_; 
  pdus.init(this, log_h);
  
  for (int i=0;i<NOF_HARQ_PROCESSES;i++) {
    srslte_softbuffer_rx_init(&softbuffer_rx[i], nof_prb);
    srslte_softbuffer_tx_init(&softbuffer_tx[i], nof_prb);
  }
  // don't need to reset because just initiated the buffers
  bzero(&metrics, sizeof(mac_metrics_t));  
  nof_failures = 0; 
  
  for(int i=0;i<NOF_HARQ_PROCESSES;i++) {
    pending_buffers[i] = NULL; 
  }

  // Set LCID group for SRB0 and SRB1
  set_lcg(0, 0);
  set_lcg(1, 0);
}  



void ue::reset()
{
  bzero(&metrics, sizeof(mac_metrics_t));  

  nof_failures = 0; 
  for (int i=0;i<NOF_HARQ_PROCESSES;i++) {
    srslte_softbuffer_rx_reset(&softbuffer_rx[i]);
    srslte_softbuffer_tx_reset(&softbuffer_tx[i]);
  }
}

void ue::start_pcap(srslte::mac_pcap* pcap_)
{
  pcap = pcap_; 
}

uint32_t ue::rl_failure()
{
  nof_failures++;
  return nof_failures;
}
  
void ue::rl_failure_reset()
{
  nof_failures = 0;
}

void ue::set_lcg(uint32_t lcid, uint32_t lcg)
{
  // find and remove if already exists
  for (int i=0;i<4;i++) {
    lc_groups[lcg].erase(std::remove(lc_groups[lcg].begin(), lc_groups[lcg].end(), lcid), lc_groups[lcg].end());
  }
  lc_groups[lcg].push_back(lcid);
}

srslte_softbuffer_rx_t* ue::get_rx_softbuffer(uint32_t tti)
{
  return &softbuffer_rx[tti%NOF_HARQ_PROCESSES];
}

srslte_softbuffer_tx_t* ue::get_tx_softbuffer(uint32_t harq_process, uint32_t tb_idx)
{
  return &softbuffer_tx[(harq_process * SRSLTE_MAX_TB + tb_idx  )%NOF_HARQ_PROCESSES];
}

uint8_t* ue::request_buffer(uint32_t tti, uint32_t len)
{
  uint8_t *ret = NULL; 
  pthread_mutex_lock(&mutex);
  if (len > 0) {   
    if (!pending_buffers[tti%NOF_HARQ_PROCESSES]) {
      ret = pdus.request(len);   
      pending_buffers[tti%NOF_HARQ_PROCESSES] = ret; 
    } else {
      log_h->console("Error requesting buffer for pid %d, not pushed yet\n", tti%NOF_HARQ_PROCESSES);
      log_h->error("Requesting buffer for pid %d, not pushed yet\n", tti%NOF_HARQ_PROCESSES);
    }
  } else {
    log_h->warning("Requesting buffer for zero bytes\n");
  }
  pthread_mutex_unlock(&mutex);
  return ret; 
}

bool ue::process_pdus()
{
  return pdus.process_pdus();  
}

void ue::set_tti(uint32_t tti) {
  last_tti = tti; 
}

#include <assert.h>

void ue::process_pdu(uint8_t* pdu, uint32_t nof_bytes, uint32_t tstamp)
{
  // Unpack ULSCH MAC PDU 
  mac_msg_ul.init_rx(nof_bytes, true);
  mac_msg_ul.parse_packet(pdu);

  if (pcap) {
    pcap->write_ul_crnti(pdu, nof_bytes, rnti, true, last_tti);
  }

  uint32_t lcid_most_data = 0;
  int most_data = -99;
  
  while(mac_msg_ul.next()) {
    assert(mac_msg_ul.get());
    if (mac_msg_ul.get()->is_sdu()) {
      // Route logical channel 
      log_h->debug_hex(mac_msg_ul.get()->get_sdu_ptr(), mac_msg_ul.get()->get_payload_size(),
                       "PDU:   rnti=0x%x, lcid=%d, %d bytes\n",
                       rnti, mac_msg_ul.get()->get_sdu_lcid(), mac_msg_ul.get()->get_payload_size());


      /* In some cases, an uplink transmission with only CQI has all zeros and gets routed to RRC 
       * Compute the checksum if lcid=0 and avoid routing in that case 
       */
      bool route_pdu = true;
      if (mac_msg_ul.get()->get_sdu_lcid() == 0) {
        uint8_t *x = mac_msg_ul.get()->get_sdu_ptr();
        uint32_t sum = 0;
        for (int i = 0; i < mac_msg_ul.get()->get_payload_size(); i++) {
          sum += x[i];
        }
        if (sum == 0) {
          route_pdu = false;
          Warning("Received all zero PDU\n");
        }
      }

      if (route_pdu) {
        rlc->write_pdu(rnti,
                       mac_msg_ul.get()->get_sdu_lcid(),
                       mac_msg_ul.get()->get_sdu_ptr(),
                       mac_msg_ul.get()->get_payload_size());
      }

      // Indicate scheduler to update BSR counters 
      sched->ul_recv_len(rnti, mac_msg_ul.get()->get_sdu_lcid(), mac_msg_ul.get()->get_payload_size());

      if ((int) mac_msg_ul.get()->get_payload_size() > most_data) {
        most_data = (int) mac_msg_ul.get()->get_payload_size();
        lcid_most_data = mac_msg_ul.get()->get_sdu_lcid();
      }

      // Save contention resolution if lcid == 0
      if (mac_msg_ul.get()->get_sdu_lcid() == 0 && route_pdu) {
        int nbytes = srslte::sch_subh::MAC_CE_CONTRES_LEN;
        if (mac_msg_ul.get()->get_payload_size() >= nbytes) {
          uint8_t *ue_cri_ptr = (uint8_t *) &conres_id;
          uint8_t *pkt_ptr = mac_msg_ul.get()->get_sdu_ptr(); // Warning here: we want to include the
          for (int i = 0; i < nbytes; i++) {
            ue_cri_ptr[nbytes - i - 1] = pkt_ptr[i];
          }
        } else {
          Error("Received CCCH UL message of invalid size=%d bytes\n", mac_msg_ul.get()->get_payload_size());
        }
      }
    }
  }
  mac_msg_ul.reset();

  /* Process CE after all SDUs because we need to update BSR after */
  bool bsr_received = false;
  while(mac_msg_ul.next()) {
    assert(mac_msg_ul.get());
    if (!mac_msg_ul.get()->is_sdu()) {
      // Process MAC Control Element
      bsr_received |= process_ce(mac_msg_ul.get());
    }
  }

  // If BSR is not received means that new data has arrived and there is no space for BSR transmission
  if (!bsr_received && lcid_most_data > 2) {
    // Add BSR to the LCID for which most data was received
    sched->ul_bsr(rnti, lcid_most_data, 256, false); // false adds BSR instead of setting
    Debug("BSR not received. Giving extra grant\n");
  }

  Debug("MAC PDU processed\n");
  
}

void ue::deallocate_pdu(uint32_t tti)
{
  if (pending_buffers[tti%NOF_HARQ_PROCESSES]) {
    pdus.deallocate(pending_buffers[tti%NOF_HARQ_PROCESSES]);
    pending_buffers[tti%NOF_HARQ_PROCESSES] = NULL; 
  } else {
    log_h->console("Error deallocating buffer for pid=%d. Not requested\n", tti%NOF_HARQ_PROCESSES);
  }
}

void ue::push_pdu(uint32_t tti, uint32_t len)
{
  if (pending_buffers[tti%NOF_HARQ_PROCESSES]) {
    pdus.push(pending_buffers[tti%NOF_HARQ_PROCESSES], len);
    pending_buffers[tti%NOF_HARQ_PROCESSES] = NULL; 
  } else {
    log_h->console("Error pushing buffer for pid=%d. Not requested\n", tti%NOF_HARQ_PROCESSES);
  }
}

bool ue::process_ce(srslte::sch_subh *subh) {
  uint32_t buff_size[4] = {0, 0, 0, 0};
  float phr = 0;
  int32_t idx = 0;
  uint16_t old_rnti = 0;
  bool is_bsr = false;
  switch(subh->ce_type()) {
    case srslte::sch_subh::PHR_REPORT: 
      phr = subh->get_phr(); 
      Info("CE:    Received PHR from rnti=0x%x, value=%.0f\n", rnti, phr);
      sched->ul_phr(rnti, (int) phr);
      metrics_phr(phr);
      break;
    case srslte::sch_subh::CRNTI: 
      old_rnti = subh->get_c_rnti(); 
      Info("CE:    Received C-RNTI from temp_rnti=0x%x, rnti=0x%x\n", rnti, old_rnti);
      if (sched->ue_exists(old_rnti)) {
        rrc->upd_user(rnti, old_rnti);
        rnti = old_rnti;         
      } else {
        Error("Updating user C-RNTI: rnti=0x%x already released\n", old_rnti);
      }
      break;
    case srslte::sch_subh::TRUNC_BSR: 
    case srslte::sch_subh::SHORT_BSR:
      idx = subh->get_bsr(buff_size);
      if(idx == -1){
        Error("Invalid Index Passed to lc groups\n");
        break;
      }
      for (uint32_t i=0;i<lc_groups[idx].size();i++) {
        // Indicate BSR to scheduler
        sched->ul_bsr(rnti, lc_groups[idx][i], buff_size[idx]);
      }
      Info("CE:    Received %s BSR rnti=0x%x, lcg=%d, value=%d\n",
           subh->ce_type()==srslte::sch_subh::SHORT_BSR?"Short":"Trunc", rnti, idx, buff_size[idx]);
      is_bsr = true;
      break;
    case srslte::sch_subh::LONG_BSR:
      subh->get_bsr(buff_size);
      for (idx=0;idx<4;idx++) {
        for (uint32_t i=0;i<lc_groups[idx].size();i++) {
          sched->ul_bsr(rnti, lc_groups[idx][i], buff_size[idx]);
        }
      }
      is_bsr = true;
      Info("CE:    Received Long BSR rnti=0x%x, value=%d,%d,%d,%d\n", rnti,
           buff_size[0], buff_size[1], buff_size[2], buff_size[3]);
      break;
    case srslte::sch_subh::PADDING: 
      Debug("CE:    Received padding for rnti=0x%x\n", rnti);
      break;
    default:
      Error("CE:    Invalid lcid=0x%x\n", subh->ce_type());
      break;
  }
  return is_bsr;
}


int ue::read_pdu(uint32_t lcid, uint8_t *payload, uint32_t requested_bytes) 
{
  return rlc->read_pdu(rnti, lcid, payload, requested_bytes);  
}

void ue::allocate_sdu(srslte::sch_pdu *pdu, uint32_t lcid, uint32_t total_sdu_len) 
{
  int sdu_space = pdu->get_sdu_space();
  if (sdu_space > 0) {
    int sdu_len = SRSLTE_MIN(total_sdu_len, (uint32_t) sdu_space);
    int n=1;
    while(sdu_len > 3 && n > 0) {
      if (pdu->new_subh()) { // there is space for a new subheader
        log_h->debug("SDU:   set_sdu(), lcid=%d, sdu_len=%d, sdu_space=%d\n", lcid, sdu_len, sdu_space);
        n = pdu->get()->set_sdu(lcid, sdu_len, this); 
        if (n > 0) { // new SDU could be added      
          sdu_len -= n; 
          log_h->debug("SDU:   rnti=0x%x, lcid=%d, nbytes=%d, rem_len=%d\n", 
                      rnti, lcid, n, sdu_len);
        } else {
          Debug("Could not add SDU lcid=%d nbytes=%d, space=%d\n", lcid, sdu_len, sdu_space);
          pdu->del_subh();
        }
      } else {
        n=0; 
      }
    }    
  }
}

void ue::allocate_ce(srslte::sch_pdu *pdu, uint32_t lcid)
{
  switch((srslte::sch_subh::cetype) lcid) {
    case srslte::sch_subh::CON_RES_ID: 
      if (pdu->new_subh()) {
        if (pdu->get()->set_con_res_id(conres_id)) {
          Info("CE:    Added Contention Resolution ID=0x%lx\n", conres_id);
        } else {
          Error("CE:    Setting Contention Resolution ID CE\n");
        }
      } else {
        Error("CE:    Setting Contention Resolution ID CE. No space for a subheader\n");
      }
    break;
    default:
      Error("CE:    Allocating CE=0x%x. Not supported\n", lcid);
      break;
  }
}

uint8_t* ue::generate_pdu(uint32_t tb_idx, sched_interface::dl_sched_pdu_t pdu[sched_interface::MAX_RLC_PDU_LIST],
                      uint32_t nof_pdu_elems, uint32_t grant_size)
{
  uint8_t *ret = NULL; 
  pthread_mutex_lock(&mutex);
  if (rlc) {
    mac_msg_dl.init_tx(tx_payload_buffer[tb_idx], grant_size, false);
    for (uint32_t i=0;i<nof_pdu_elems;i++) {
      if (pdu[i].lcid <= srslte::sch_subh::PHR_REPORT) {
        allocate_sdu(&mac_msg_dl, pdu[i].lcid, pdu[i].nbytes);
      } else {
        allocate_ce(&mac_msg_dl,pdu[i].lcid);
      }
    }
    
    ret = mac_msg_dl.write_packet(log_h);   
    
  } else {
    std::cout << "Error ue not configured (must call config() first" << std::endl; 
  }
  
  pthread_mutex_unlock(&mutex);
  return ret; 
}



/******* METRICS interface ***************/
void ue::metrics_read(mac_metrics_t* metrics_)
{
  metrics.rnti = rnti; 
  metrics.ul_buffer = sched->get_ul_buffer(rnti);
  metrics.dl_buffer = sched->get_dl_buffer(rnti);
  
  memcpy(metrics_, &metrics, sizeof(mac_metrics_t));

  phr_counter = 0;
  dl_cqi_counter = 0;
  bzero(&metrics, sizeof(mac_metrics_t));  
}

void ue::metrics_phr(float phr) {
  metrics.phr = SRSLTE_VEC_CMA(phr, metrics.phr, phr_counter);
  phr_counter++;
}

void ue::metrics_dl_ri(uint32_t dl_ri) {
  if (metrics.dl_ri == 0.0f) {
    metrics.dl_ri = (float) dl_ri + 1.0f;
  } else {
    metrics.dl_ri = SRSLTE_VEC_EMA((float) dl_ri + 1.0f, metrics.dl_ri, 0.5f);
  }
  dl_ri_counter++;
}

void ue::metrics_dl_pmi(uint32_t dl_ri) {
  metrics.dl_pmi = SRSLTE_VEC_CMA((float) dl_ri, metrics.dl_pmi, dl_pmi_counter);
  dl_pmi_counter++;
}

void ue::metrics_dl_cqi(uint32_t dl_cqi) {
  metrics.dl_cqi = SRSLTE_VEC_CMA((float) dl_cqi, metrics.dl_cqi, dl_cqi_counter);
  dl_cqi_counter++;
}

void ue::metrics_rx(bool crc, uint32_t tbs)
{
  if (crc) {
    metrics.rx_brate += tbs*8; 
  } else {
    metrics.rx_errors++;
  }
  metrics.rx_pkts++;
}

void ue::metrics_tx(bool crc, uint32_t tbs)
{
  if (crc) {
    metrics.tx_brate += tbs*8; 
  } else {
    metrics.tx_errors++;
  }
  metrics.tx_pkts++;
}

  
}

