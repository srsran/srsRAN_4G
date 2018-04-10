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

#define Error(fmt, ...)   log_h->error(fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) log_h->warning(fmt, ##__VA_ARGS__)
#define Info(fmt, ...)    log_h->info(fmt, ##__VA_ARGS__)
#define Debug(fmt, ...)   log_h->debug(fmt, ##__VA_ARGS__)

#include <string.h>
#include <strings.h>
#include <pthread.h>
#include <unistd.h>
#include <srslte/interfaces/sched_interface.h>

#include "srslte/common/log.h"
#include "srsenb/hdr/mac/mac.h"

//#define WRITE_SIB_PCAP

namespace srsenb {

mac::mac() : timers_db(128), timers_thread(&timers_db), tti(0), last_rnti(0),
             rar_pdu_msg(sched_interface::MAX_RAR_LIST), rar_payload(),
             pdu_process_thread(this)
{
  started = false;  
  pcap = NULL;
  phy_h = NULL;
  rlc_h = NULL;
  rrc_h = NULL;
  log_h = NULL;

  bzero(&locations, sizeof(locations));
  bzero(&cell, sizeof(cell));
  bzero(&args, sizeof(args));
  bzero(&pending_rars, sizeof(pending_rars));
  bzero(&bcch_dlsch_payload, sizeof(bcch_dlsch_payload));
  bzero(&pcch_payload_buffer, sizeof(pcch_payload_buffer));
  bzero(&bcch_softbuffer_tx, sizeof(bcch_softbuffer_tx));
  bzero(&pcch_softbuffer_tx, sizeof(pcch_softbuffer_tx));
  bzero(&rar_softbuffer_tx, sizeof(rar_softbuffer_tx));
}
  
bool mac::init(mac_args_t *args_, srslte_cell_t *cell_, phy_interface_mac *phy, rlc_interface_mac *rlc, rrc_interface_mac *rrc, srslte::log *log_h_)
{
  started = false; 

  if (cell_ && phy && rlc && log_h_ && args_) {
    phy_h = phy;
    rlc_h = rlc; 
    rrc_h = rrc; 
    log_h = log_h_; 
        
    memcpy(&args, args_, sizeof(mac_args_t));
    memcpy(&cell, cell_, sizeof(srslte_cell_t));
    
    scheduler.init(rrc, log_h);
    // Set default scheduler (RR)
    scheduler.set_metric(&sched_metric_dl_rr, &sched_metric_ul_rr);
    
    // Set default scheduler configuration 
    scheduler.set_sched_cfg(&args.sched);
    
    // Init softbuffer for SI messages
    for (int i=0;i<NOF_BCCH_DLSCH_MSG;i++) {
      srslte_softbuffer_tx_init(&bcch_softbuffer_tx[i], cell.nof_prb);
    }
    // Init softbuffer for PCCH
    srslte_softbuffer_tx_init(&pcch_softbuffer_tx, cell.nof_prb);

    // Init softbuffer for RAR 
    srslte_softbuffer_tx_init(&rar_softbuffer_tx, cell.nof_prb);

    reset();

    started = true; 
  }

  return started; 
}

void mac::stop()
{
  for (uint32_t i=0;i<ue_db.size();i++) {
    delete ue_db[i];
  }
  for (int i=0;i<NOF_BCCH_DLSCH_MSG;i++) {
    srslte_softbuffer_tx_free(&bcch_softbuffer_tx[i]);
  }  
  srslte_softbuffer_tx_free(&pcch_softbuffer_tx);
  srslte_softbuffer_tx_free(&rar_softbuffer_tx);
  started = false;   
  timers_thread.stop();
  pdu_process_thread.stop();
}

// Implement Section 5.9
void mac::reset()
{
  Info("Resetting MAC\n");
  
  timers_db.stop_all();

  tti = 0; 
  last_rnti = 70; 
  
  /* Setup scheduler */
  scheduler.reset();
  
}

void mac::start_pcap(srslte::mac_pcap* pcap_)
{
  pcap = pcap_; 
  // Set pcap in all UEs for UL messages 
  for(std::map<uint16_t, ue*>::iterator iter=ue_db.begin(); iter!=ue_db.end(); ++iter) {
    ue *u = iter->second;
    u->start_pcap(pcap);
  }  
}

/********************************************************
 *
 * RLC interface 
 *
 *******************************************************/
int mac::rlc_buffer_state(uint16_t rnti, uint32_t lc_id, uint32_t tx_queue, uint32_t retx_queue)
{
  if (ue_db.count(rnti)) {   
    return scheduler.dl_rlc_buffer_state(rnti, lc_id, tx_queue, retx_queue);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
    return -1;
  }
}

int mac::bearer_ue_cfg(uint16_t rnti, uint32_t lc_id, sched_interface::ue_bearer_cfg_t* cfg)
{
  if (ue_db.count(rnti)) {
    // configure BSR group in UE
    ue_db[rnti]->set_lcg(lc_id, (uint32_t) cfg->group);
    return scheduler.bearer_ue_cfg(rnti, lc_id, cfg);      
  } else {
    Error("User rnti=0x%x not found\n", rnti);
    return -1;
  }
}

int mac::bearer_ue_rem(uint16_t rnti, uint32_t lc_id)
{
  if (ue_db.count(rnti)) {   
    return scheduler.bearer_ue_rem(rnti, lc_id);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
    return -1;
  }
}

void mac::phy_config_enabled(uint16_t rnti, bool enabled)
{
  scheduler.phy_config_enabled(rnti, enabled);
}

// Update UE configuration 
int mac::ue_cfg(uint16_t rnti, sched_interface::ue_cfg_t* cfg)
{
  if (ue_db.count(rnti)) {         
    
    // Add RNTI to the PHY (pregerate signals) now instead of after PRACH
    if (!ue_db[rnti]->is_phy_added) {
      ue_db[rnti]->is_phy_added = true; 
      Info("Registering rnti=0x%x to PHY...\n", rnti);
      // Register new user in PHY
      if (phy_h->add_rnti(rnti)) {
        Error("Registering new ue rnti=0x%x to PHY\n", rnti);       
      }
      Info("Done registering rnti=0x%x to PHY...\n", rnti);
    }
    
    // Update Scheduler configuration 
    if (scheduler.ue_cfg(rnti, cfg)) {
      Error("Registering new UE rnti=0x%x to SCHED\n", rnti);
      return -1;
    }     
    return 0;
  } else {
    Error("User rnti=0x%x not found\n", rnti);
    return -1;
  }
}

// Removes UE from DB
int mac::ue_rem(uint16_t rnti)
{
  if (ue_db.count(rnti)) {         
    scheduler.ue_rem(rnti);
    phy_h->rem_rnti(rnti);
    delete ue_db[rnti]; 
    ue_db.erase(rnti);
    Info("User rnti=0x%x removed from MAC/PHY\n", rnti);
    return 0; 
  } else {
    Error("User rnti=0x%x not found\n", rnti);
    return -1;
  }
}

int mac::cell_cfg(sched_interface::cell_cfg_t* cell_cfg)
{
  return scheduler.cell_cfg(cell_cfg);  
}

void mac::get_metrics(mac_metrics_t metrics[ENB_METRICS_MAX_USERS])
{
  int cnt=0;
  for(std::map<uint16_t, ue*>::iterator iter=ue_db.begin(); iter!=ue_db.end(); ++iter) {
    ue *u = iter->second;
    u->metrics_read(&metrics[cnt]);
    cnt++;
  } 
}




/********************************************************
 *
 * PHY interface 
 *
 *******************************************************/

void mac::rl_failure(uint16_t rnti)
{
  if (ue_db.count(rnti)) {         
    uint32_t nof_fails = ue_db[rnti]->rl_failure();  
    if (nof_fails >= (uint32_t) args.link_failure_nof_err && args.link_failure_nof_err > 0) {    
      Info("Detected Uplink failure for rnti=0x%x\n", rnti);
      rrc_h->rl_failure(rnti);
      ue_db[rnti]->rl_failure_reset();
    }
  } else {
    Error("User rnti=0x%x not found\n", rnti);
  }
}

void mac::rl_ok(uint16_t rnti)
{
  if (ue_db.count(rnti)) {         
    ue_db[rnti]->rl_failure_reset();  
  } else {
    Error("User rnti=0x%x not found\n", rnti);
  }
}

int mac::ack_info(uint32_t tti, uint16_t rnti, uint32_t tb_idx, bool ack)
{
  log_h->step(tti);
  uint32_t nof_bytes = scheduler.dl_ack_info(tti, rnti, tb_idx, ack);
  ue_db[rnti]->metrics_tx(ack, nof_bytes);
  
  if (ack) {
    if (nof_bytes > 64) { // do not count RLC status messages only
      rrc_h->set_activity_user(rnti); 
      log_h->debug("DL activity rnti=0x%x, n_bytes=%d\n", rnti, nof_bytes);
    }
  }
  return 0; 
}

int mac::crc_info(uint32_t tti, uint16_t rnti, uint32_t nof_bytes, bool crc)
{
  log_h->step(tti);

  if (ue_db.count(rnti)) {         
    ue_db[rnti]->set_tti(tti);
    
    ue_db[rnti]->metrics_rx(crc, nof_bytes);
    
    // push the pdu through the queue if received correctly
    if (crc) {
       ue_db[rnti]->push_pdu(tti, nof_bytes); 
      pdu_process_thread.notify();      
      if (nof_bytes > 64) { // do not count RLC status messages only
        rrc_h->set_activity_user(rnti); 
        log_h->debug("UL activity rnti=0x%x, n_bytes=%d\n", rnti, nof_bytes);
      }
    } else {
      ue_db[rnti]->deallocate_pdu(tti);
    }
    
    return scheduler.ul_crc_info(tti, rnti, crc);  
  } else {
    Error("User rnti=0x%x not found\n", rnti);
    return -1;
  }
}

int mac::set_dl_ant_info(uint16_t rnti, LIBLTE_RRC_ANTENNA_INFO_DEDICATED_STRUCT *dl_ant_info) {
  log_h->step(tti);

  if (ue_db.count(rnti)) {
    scheduler.dl_ant_info(rnti, dl_ant_info);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
    return -1;
  }
  return 0;
}

int mac::ri_info(uint32_t tti, uint16_t rnti, uint32_t ri_value)
{
  log_h->step(tti);

  if (ue_db.count(rnti)) {         
    scheduler.dl_ri_info(tti, rnti, ri_value);
    ue_db[rnti]->metrics_dl_ri(ri_value);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
    return -1;
  }
  return 0; 
}

int mac::pmi_info(uint32_t tti, uint16_t rnti, uint32_t pmi_value)
{
  log_h->step(tti);

  if (ue_db.count(rnti)) {
    scheduler.dl_pmi_info(tti, rnti, pmi_value);
    ue_db[rnti]->metrics_dl_pmi(pmi_value);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
    return -1;
  }
  return 0;
}

int mac::cqi_info(uint32_t tti, uint16_t rnti, uint32_t cqi_value)
{
  log_h->step(tti);

  if (ue_db.count(rnti)) {
    scheduler.dl_cqi_info(tti, rnti, cqi_value);
    ue_db[rnti]->metrics_dl_cqi(cqi_value);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
    return -1;
  }
  return 0; 
}

int mac::snr_info(uint32_t tti, uint16_t rnti, float snr)
{
  log_h->step(tti);

  if (ue_db.count(rnti)) {         
    uint32_t cqi = srslte_cqi_from_snr(snr); 
    scheduler.ul_cqi_info(tti, rnti, cqi, 0);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
    return -1;
  }
  return 0; 
}

int mac::sr_detected(uint32_t tti, uint16_t rnti)
{
  log_h->step(tti);

  if (ue_db.count(rnti)) {         
    scheduler.ul_sr_info(tti, rnti);    
  } else {
    Error("User rnti=0x%x not found\n", rnti);
    return -1;
  }
  return 0; 
}

int mac::rach_detected(uint32_t tti, uint32_t preamble_idx, uint32_t time_adv)
{
  log_h->step(tti);

  // Find empty slot for pending rars
  uint32_t ra_id=0;
  while(pending_rars[ra_id].temp_crnti && ra_id<MAX_PENDING_RARS-1) {
    ra_id++;
  }
  if (ra_id == MAX_PENDING_RARS) {
    Error("Maximum number of pending RARs exceeded (%d)\n", MAX_PENDING_RARS);
    return -1; 
  }
  
  // Create new UE 
  ue_db[last_rnti] = new ue; 
  ue_db[last_rnti]->config(last_rnti, cell.nof_prb, &scheduler, rrc_h, rlc_h, log_h);
  
  // Set PCAP if available 
  if (pcap) {
    ue_db[last_rnti]->start_pcap(pcap);
  }
  // Save RA info
  pending_rars[ra_id].preamble_idx = preamble_idx; 
  pending_rars[ra_id].ta_cmd       = 2*time_adv;
  pending_rars[ra_id].temp_crnti   = last_rnti;   
  
  // Add new user to the scheduler so that it can RX/TX SRB0
  sched_interface::ue_cfg_t uecfg; 
  bzero(&uecfg, sizeof(sched_interface::ue_cfg_t));
  uecfg.ue_bearers[0].direction = srsenb::sched_interface::ue_bearer_cfg_t::BOTH; 
  if (scheduler.ue_cfg(last_rnti, &uecfg)) {
    Error("Registering new user rnti=0x%x to SCHED\n", last_rnti);
    return -1;
  }

  // Register new user in RRC 
  rrc_h->add_user(last_rnti);
  
  // Trigger scheduler RACH 
  scheduler.dl_rach_info(tti, ra_id, last_rnti, 7);    
    
  log_h->info("RACH:  tti=%d, preamble=%d, offset=%d, temp_crnti=0x%x\n", 
                 tti, preamble_idx, time_adv, last_rnti);  
  log_h->console("RACH:  tti=%d, preamble=%d, offset=%d, temp_crnti=0x%x\n", 
                 tti, preamble_idx, time_adv, last_rnti);  
  
  // Increae RNTI counter 
  last_rnti++;
  if (last_rnti >= 60000) {
    last_rnti = 70; 
  }
  return 0; 
}

int mac::get_dl_sched(uint32_t tti, dl_sched_t *dl_sched_res)
{
  log_h->step(tti);

  if (!started) {
    return 0;
  }
  
  if (!dl_sched_res) {
    return SRSLTE_ERROR_INVALID_INPUTS;  
  }
  
  // Run scheduler with current info 
  sched_interface::dl_sched_res_t sched_result; 
  bzero(&sched_result, sizeof(sched_interface::dl_sched_res_t));
  if (scheduler.dl_sched(tti, &sched_result) < 0) {
    Error("Running scheduler\n");
    return SRSLTE_ERROR;
  }
  
  int n = 0; 
  
  // Copy data grants 
  for (uint32_t i=0;i<sched_result.nof_data_elems;i++) {
    
    // Get UE 
    uint16_t rnti = sched_result.data[i].rnti;
    
    // Copy grant info 
    dl_sched_res->sched_grants[n].rnti = rnti; 
    dl_sched_res->sched_grants[n].dci_format = sched_result.data[i].dci_format;
    memcpy(&dl_sched_res->sched_grants[n].grant,    &sched_result.data[i].dci,          sizeof(srslte_ra_dl_dci_t));
    memcpy(&dl_sched_res->sched_grants[n].location, &sched_result.data[i].dci_location, sizeof(srslte_dci_location_t));    

    for (uint32_t tb = 0; tb < SRSLTE_MAX_TB; tb++) {
      dl_sched_res->sched_grants[n].softbuffers[tb] =
          ue_db[rnti]->get_tx_softbuffer(sched_result.data[i].dci.harq_process, tb);

      if (sched_result.data[i].nof_pdu_elems[tb] > 0) {
        /* Get PDU if it's a new transmission */
        dl_sched_res->sched_grants[n].data[tb] = ue_db[rnti]->generate_pdu(tb,
                                                                           sched_result.data[i].pdu[tb],
                                                                           sched_result.data[i].nof_pdu_elems[tb],
                                                                           sched_result.data[i].tbs[tb]);

        if (!dl_sched_res->sched_grants[n].data[tb]) {
          Error("Error! PDU was not generated (rnti=0x%04x, tb=%d)\n", rnti, tb);
          sched_result.data[i].dci.tb_en[tb] = false;
        }

        if (pcap) {
          pcap->write_dl_crnti(dl_sched_res->sched_grants[n].data[tb], sched_result.data[i].tbs[tb], rnti, true, tti);
        }

      } else {
        /* TB not enabled OR no data to send: set pointers to NULL  */
        dl_sched_res->sched_grants[n].data[tb] = NULL;
      }
    }
    n++;
  }

  // Copy RAR grants 
  for (uint32_t i=0;i<sched_result.nof_rar_elems;i++) {
    // Copy grant info 
    dl_sched_res->sched_grants[n].rnti = sched_result.rar[i].rarnti;
    dl_sched_res->sched_grants[n].dci_format = SRSLTE_DCI_FORMAT1A; // Force Format 1A
    memcpy(&dl_sched_res->sched_grants[n].grant,    &sched_result.rar[i].dci,          sizeof(srslte_ra_dl_dci_t));
    memcpy(&dl_sched_res->sched_grants[n].location, &sched_result.rar[i].dci_location, sizeof(srslte_dci_location_t));    

    // Set softbuffer (there are no retx in RAR but a softbuffer is required)
    dl_sched_res->sched_grants[n].softbuffers[0] = &rar_softbuffer_tx;

    // Assemble PDU 
    dl_sched_res->sched_grants[n].data[0] = assemble_rar(sched_result.rar[i].grants, sched_result.rar[i].nof_grants, i, sched_result.rar[i].tbs);

    
    if (pcap) {
      pcap->write_dl_ranti(dl_sched_res->sched_grants[n].data[0], sched_result.rar[i].tbs, dl_sched_res->sched_grants[n].rnti, true, tti);
    }

    n++;
  }

  // Copy SI and Paging grants   
  for (uint32_t i=0;i<sched_result.nof_bc_elems;i++) {
    // Copy grant info 
    dl_sched_res->sched_grants[n].rnti = (sched_result.bc[i].type == sched_interface::dl_sched_bc_t::BCCH ) ? SRSLTE_SIRNTI : SRSLTE_PRNTI;
    dl_sched_res->sched_grants[n].dci_format = SRSLTE_DCI_FORMAT1A; // Force Format 1A
    memcpy(&dl_sched_res->sched_grants[n].grant,    &sched_result.bc[i].dci,          sizeof(srslte_ra_dl_dci_t));
    memcpy(&dl_sched_res->sched_grants[n].location, &sched_result.bc[i].dci_location, sizeof(srslte_dci_location_t));    
    
    // Set softbuffer    
    if (sched_result.bc[i].type == sched_interface::dl_sched_bc_t::BCCH) {
      dl_sched_res->sched_grants[n].softbuffers[0] = &bcch_softbuffer_tx[sched_result.bc[i].index];
      dl_sched_res->sched_grants[n].data[0] = assemble_si(sched_result.bc[i].index);
#ifdef WRITE_SIB_PCAP
      if (pcap) {
        pcap->write_dl_sirnti(dl_sched_res->sched_grants[n].data[0], sched_result.bc[i].tbs, true, tti);
      }
#endif
    } else {
      dl_sched_res->sched_grants[n].softbuffers[0] = &pcch_softbuffer_tx;
      dl_sched_res->sched_grants[n].data[0] = pcch_payload_buffer;
      rlc_h->read_pdu_pcch(pcch_payload_buffer, pcch_payload_buffer_len);
      
      if (pcap) {
        pcap->write_dl_pch(dl_sched_res->sched_grants[n].data[0], sched_result.bc[i].tbs, true, tti);
      }
    }
    
    n++;
  }
  
  dl_sched_res->nof_grants = n; 
  
  // Number of CCH symbols 
  dl_sched_res->cfi = sched_result.cfi; 
    
  return SRSLTE_SUCCESS;
}

uint8_t* mac::assemble_rar(sched_interface::dl_sched_rar_grant_t* grants, uint32_t nof_grants, int rar_idx, uint32_t pdu_len)
{
  uint8_t grant_buffer[64];
  if (pdu_len < rar_payload_len) {
    srslte::rar_pdu *pdu = &rar_pdu_msg[rar_idx];
    pdu->init_tx(rar_payload[rar_idx], pdu_len);
    for (uint32_t i=0;i<nof_grants;i++) {
      srslte_dci_rar_grant_pack(&grants[i].grant, grant_buffer);
      if (pdu->new_subh()) {
        /* Search pending RAR */
        int idx = grants[i].ra_id;
        pdu->get()->set_rapid(pending_rars[idx].preamble_idx); 
        pdu->get()->set_ta_cmd(pending_rars[idx].ta_cmd);
        pdu->get()->set_temp_crnti(pending_rars[idx].temp_crnti);          
        pdu->get()->set_sched_grant(grant_buffer);                 
        bzero(&pending_rars[idx], sizeof(pending_rar_t));
      }
    }
    pdu->write_packet(rar_payload[rar_idx]);
    return rar_payload[rar_idx];
  } else {
    Error("Assembling RAR: pdu_len > rar_payload_len (%d>%d)\n", pdu_len, rar_payload_len);
    return NULL; 
  }
}

uint8_t* mac::assemble_si(uint32_t index)
{  
  rlc_h->read_pdu_bcch_dlsch(index, bcch_dlsch_payload);
  return bcch_dlsch_payload;
}

int mac::get_ul_sched(uint32_t tti, ul_sched_t *ul_sched_res) 
{

  log_h->step(tti);
  
  if (!started) {
    return 0; 
  }
  
  if (!ul_sched_res) {
    return SRSLTE_ERROR_INVALID_INPUTS; 
  }
  
  // Run scheduler with current info 
  sched_interface::ul_sched_res_t sched_result; 
  bzero(&sched_result, sizeof(sched_interface::ul_sched_res_t));
  if (scheduler.ul_sched(tti, &sched_result)<0) {
    Error("Running scheduler\n");
    return SRSLTE_ERROR; 
  }

  // Copy DCI grants 
  ul_sched_res->nof_grants = 0; 
  int n = 0; 
  for (uint32_t i=0;i<sched_result.nof_dci_elems;i++) {
    
    if (sched_result.pusch[i].tbs > 0) {
      // Get UE 
      uint16_t rnti = sched_result.pusch[i].rnti;
            
      // Copy grant info 
      ul_sched_res->sched_grants[n].rnti             = rnti; 
      ul_sched_res->sched_grants[n].current_tx_nb    = sched_result.pusch[i].current_tx_nb; 
      ul_sched_res->sched_grants[n].needs_pdcch      = sched_result.pusch[i].needs_pdcch; 
      memcpy(&ul_sched_res->sched_grants[n].grant,    &sched_result.pusch[i].dci,          sizeof(srslte_ra_ul_dci_t));
      memcpy(&ul_sched_res->sched_grants[n].location, &sched_result.pusch[i].dci_location, sizeof(srslte_dci_location_t));    
      
      ul_sched_res->sched_grants[n].softbuffer = ue_db[rnti]->get_rx_softbuffer(tti);        
      
      if (sched_result.pusch[n].current_tx_nb == 0) {
        srslte_softbuffer_rx_reset_tbs(ul_sched_res->sched_grants[n].softbuffer, sched_result.pusch[i].tbs*8);      
      }
      ul_sched_res->sched_grants[n].data = ue_db[rnti]->request_buffer(tti, sched_result.pusch[i].tbs);
      ul_sched_res->nof_grants++;
      n++; 
      
    } else {
      Warning("Grant %d for rnti=0x%x has zero TBS\n", i, sched_result.pusch[i].rnti);
    }
  }

  // Copy PHICH actions
  for (uint32_t i=0;i<sched_result.nof_phich_elems;i++) {
    ul_sched_res->phich[i].ack  = sched_result.phich[i].phich == sched_interface::ul_sched_phich_t::ACK;
    ul_sched_res->phich[i].rnti = sched_result.phich[i].rnti; 
  }
  ul_sched_res->nof_phich = sched_result.nof_phich_elems;
  return SRSLTE_SUCCESS; 
}
  
void mac::tti_clock()
{
  timers_thread.tti_clock();
}




/********************************************************
 *
 * Interface for upper layer timers
 *
 *******************************************************/
uint32_t mac::timer_get_unique_id()
{
  return timers_db.get_unique_id();
}

void mac::timer_release_id(uint32_t timer_id)
{
  timers_db.release_id(timer_id);
}

/* Front-end to upper-layer timers */
srslte::timers::timer* mac::timer_get(uint32_t timer_id)
{
  return timers_db.get(timer_id);
}



/********************************************************
 *
 * Class to run timers with normal priority
 *
 *******************************************************/
void mac::timer_thread::run_thread()
{
  running=true; 
  ttisync.set_producer_cntr(0);
  ttisync.resync();
  while(running) {
    ttisync.wait();
    timers->step_all();
  }
}

void mac::timer_thread::stop()
{
  running=false;
  ttisync.increase();
  wait_thread_finish();
}

void mac::timer_thread::tti_clock()
{
  ttisync.increase();
}



/********************************************************
 *
 * Class that runs a thread to process DL MAC PDUs from
 * DEMU unit
 *
 *******************************************************/
mac::pdu_process::pdu_process(pdu_process_handler *h) : running(false) {
  handler = h; 
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&cvar, NULL);
  have_data = false; 
  start(MAC_PDU_THREAD_PRIO);  
}

void mac::pdu_process::stop()
{
  pthread_mutex_lock(&mutex);
  running = false; 
  pthread_cond_signal(&cvar);
  pthread_mutex_unlock(&mutex);
  
  wait_thread_finish();
}

void mac::pdu_process::notify()
{
  pthread_mutex_lock(&mutex);
  have_data = true; 
  pthread_cond_signal(&cvar);
  pthread_mutex_unlock(&mutex);
}

void mac::pdu_process::run_thread()
{
  running = true; 
  while(running) {
    have_data = handler->process_pdus();
    if (!have_data) {
      pthread_mutex_lock(&mutex);
      while(!have_data && running) {
        pthread_cond_wait(&cvar, &mutex);
      }
      pthread_mutex_unlock(&mutex);
    }
  }
}

bool mac::process_pdus()
{
  bool ret = false; 
  for(std::map<uint16_t, ue*>::iterator iter=ue_db.begin(); iter!=ue_db.end(); ++iter) {
    ue *u         = iter->second; 
    uint16_t rnti = iter->first; 
    ret = ret | u->process_pdus();
  }
  return ret; 
}





}



