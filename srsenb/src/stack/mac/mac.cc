/*
 * Copyright 2013-2019 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
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

#define Error(fmt, ...)   log_h->error(fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) log_h->warning(fmt, ##__VA_ARGS__)
#define Info(fmt, ...)    log_h->info(fmt, ##__VA_ARGS__)
#define Debug(fmt, ...)   log_h->debug(fmt, ##__VA_ARGS__)

#include <string.h>
#include <strings.h>
#include <pthread.h>
#include <unistd.h>
#include <srslte/interfaces/sched_interface.h>

#include "srsenb/hdr/stack/mac/mac.h"
#include "srslte/common/log.h"

//#define WRITE_SIB_PCAP
using namespace asn1::rrc;

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
  pthread_rwlock_init(&rwlock, NULL);
}

mac::~mac()
{
  stop();
  pthread_rwlock_destroy(&rwlock);
}

bool mac::init(const mac_args_t&        args_,
               srslte_cell_t*           cell_,
               phy_interface_stack_lte* phy,
               rlc_interface_mac*       rlc,
               rrc_interface_mac*       rrc,
               srslte::log*             log_h_)
{
  started = false;

  if (cell_ && phy && rlc && log_h_) {
    phy_h = phy;
    rlc_h = rlc; 
    rrc_h = rrc; 
    log_h = log_h_;

    args = args_;
    cell = *cell_;

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
  pthread_rwlock_wrlock(&rwlock);
  if (started) {
    for (uint32_t i = 0; i < ue_db.size(); i++) {
      delete ue_db[i];
    }
    for (int i = 0; i < NOF_BCCH_DLSCH_MSG; i++) {
      srslte_softbuffer_tx_free(&bcch_softbuffer_tx[i]);
    }
    srslte_softbuffer_tx_free(&pcch_softbuffer_tx);
    srslte_softbuffer_tx_free(&rar_softbuffer_tx);
    started = false;
    timers_thread.stop();
    pdu_process_thread.stop();
  }
  pthread_rwlock_unlock(&rwlock);
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
  pthread_rwlock_rdlock(&rwlock);
  int ret = -1;
  if (ue_db.count(rnti)) {
    if(rnti != SRSLTE_MRNTI){
      ret = scheduler.dl_rlc_buffer_state(rnti, lc_id, tx_queue, retx_queue);
    } else {
      for(uint32_t i = 0; i < mch.num_mtch_sched; i++){
        if(lc_id == mch.mtch_sched[i].lcid){
          mch.mtch_sched[i].lcid_buffer_size = tx_queue;
        }
      }
      ret = 0;
    }
  } else {
    Error("User rnti=0x%x not found\n", rnti);
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

int mac::bearer_ue_cfg(uint16_t rnti, uint32_t lc_id, sched_interface::ue_bearer_cfg_t* cfg)
{
  int ret = -1;
  pthread_rwlock_rdlock(&rwlock);
  if (ue_db.count(rnti)) {
    // configure BSR group in UE
    ue_db[rnti]->set_lcg(lc_id, (uint32_t) cfg->group);
    ret = scheduler.bearer_ue_cfg(rnti, lc_id, cfg);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

int mac::bearer_ue_rem(uint16_t rnti, uint32_t lc_id)
{
  pthread_rwlock_rdlock(&rwlock);
  int ret = -1;
  if (ue_db.count(rnti)) {
    ret = scheduler.bearer_ue_rem(rnti, lc_id);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

void mac::phy_config_enabled(uint16_t rnti, bool enabled)
{
  scheduler.phy_config_enabled(rnti, enabled);
}

// Update UE configuration
int mac::ue_cfg(uint16_t rnti, sched_interface::ue_cfg_t* cfg)
{
  int ret = -1;
  pthread_rwlock_rdlock(&rwlock);
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
    } else {
      ret = 0;
    }
  } else {
    Error("User rnti=0x%x not found\n", rnti);
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

// Removes UE from DB
int mac::ue_rem(uint16_t rnti)
{
  int ret = -1;
  pthread_rwlock_rdlock(&rwlock);
  if (ue_db.count(rnti)) {
    scheduler.ue_rem(rnti);
    phy_h->rem_rnti(rnti);
    ret = 0;
  } else {
    Error("User rnti=0x%x not found\n", rnti);
  }
  pthread_rwlock_unlock(&rwlock);
  if (ret) {
    return ret;
  }
  pthread_rwlock_wrlock(&rwlock);
  if (ue_db.count(rnti)) {
    delete ue_db[rnti];
    ue_db.erase(rnti);
    Info("User rnti=0x%x removed from MAC/PHY\n", rnti);
  } else {
    Error("User rnti=0x%x already removed\n", rnti);
  }
  pthread_rwlock_unlock(&rwlock);
  return 0;
}

int mac::cell_cfg(sched_interface::cell_cfg_t* cell_cfg)
{
  this->cell_config = *cell_cfg;
  return scheduler.cell_cfg(cell_cfg);
}

void mac::get_metrics(mac_metrics_t metrics[ENB_METRICS_MAX_USERS])
{
  pthread_rwlock_rdlock(&rwlock);
  int cnt=0;
  for(std::map<uint16_t, ue*>::iterator iter=ue_db.begin(); iter!=ue_db.end(); ++iter) {
    ue *u = iter->second;
    if(iter->first != SRSLTE_MRNTI) {
      u->metrics_read(&metrics[cnt]);
      cnt++;
    }
  }
  pthread_rwlock_unlock(&rwlock);
}


/********************************************************
 *
 * PHY interface
 *
 *******************************************************/

void mac::rl_failure(uint16_t rnti)
{
  pthread_rwlock_rdlock(&rwlock);
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
  pthread_rwlock_unlock(&rwlock);
}

void mac::rl_ok(uint16_t rnti)
{
  pthread_rwlock_rdlock(&rwlock);
  if (ue_db.count(rnti)) {
    ue_db[rnti]->rl_failure_reset();
  } else {
    Error("User rnti=0x%x not found\n", rnti);
  }
  pthread_rwlock_unlock(&rwlock);
}

int mac::ack_info(uint32_t tti, uint16_t rnti, uint32_t tb_idx, bool ack)
{
  pthread_rwlock_rdlock(&rwlock);
  log_h->step(tti);
  uint32_t nof_bytes = scheduler.dl_ack_info(tti, rnti, tb_idx, ack);
  ue_db[rnti]->metrics_tx(ack, nof_bytes);

  if (ack) {
    if (nof_bytes > 64) { // do not count RLC status messages only
      rrc_h->set_activity_user(rnti);
      log_h->debug("DL activity rnti=0x%x, n_bytes=%d\n", rnti, nof_bytes);
    }
  }
  pthread_rwlock_unlock(&rwlock);
  return 0;
}

int mac::crc_info(uint32_t tti, uint16_t rnti, uint32_t nof_bytes, bool crc)
{
  log_h->step(tti);
  int ret = -1;
  pthread_rwlock_rdlock(&rwlock);
  if (ue_db.count(rnti)) {
    ue_db[rnti]->set_tti(tti);

    ue_db[rnti]->metrics_rx(crc, nof_bytes);

    // push the pdu through the queue if received correctly
    if (crc) {
      Info("Pushing PDU rnti=%d, tti=%d, nof_bytes=%d\n", rnti, tti, nof_bytes);
      ue_db[rnti]->push_pdu(tti, nof_bytes);
      pdu_process_thread.notify();
    } else {
      ue_db[rnti]->deallocate_pdu(tti);
    }

    ret = scheduler.ul_crc_info(tti, rnti, crc);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

int mac::set_dl_ant_info(uint16_t rnti, phys_cfg_ded_s::ant_info_c_* dl_ant_info)
{
  log_h->step(tti);

  int ret = -1;
  pthread_rwlock_rdlock(&rwlock);
  if (ue_db.count(rnti)) {
    scheduler.dl_ant_info(rnti, dl_ant_info);
    ret = 0;
  } else {
    Error("User rnti=0x%x not found\n", rnti);
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

int mac::ri_info(uint32_t tti, uint16_t rnti, uint32_t ri_value)
{
  log_h->step(tti);
  int ret = -1;
  pthread_rwlock_rdlock(&rwlock);
  if (ue_db.count(rnti)) {
    scheduler.dl_ri_info(tti, rnti, ri_value);
    ue_db[rnti]->metrics_dl_ri(ri_value);
    ret = 0;
  } else {
    Error("User rnti=0x%x not found\n", rnti);
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

int mac::pmi_info(uint32_t tti, uint16_t rnti, uint32_t pmi_value)
{
  log_h->step(tti);
  pthread_rwlock_rdlock(&rwlock);
  int ret = -1;
  if (ue_db.count(rnti)) {
    scheduler.dl_pmi_info(tti, rnti, pmi_value);
    ue_db[rnti]->metrics_dl_pmi(pmi_value);
    ret = 0;
  } else {
    Error("User rnti=0x%x not found\n", rnti);
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

int mac::cqi_info(uint32_t tti, uint16_t rnti, uint32_t cqi_value)
{
  log_h->step(tti);
  int ret = -1;

  pthread_rwlock_rdlock(&rwlock);
  if (ue_db.count(rnti)) {
    scheduler.dl_cqi_info(tti, rnti, cqi_value);
    ue_db[rnti]->metrics_dl_cqi(cqi_value);
    ret = 0;
  } else {
    Error("User rnti=0x%x not found\n", rnti);
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

int mac::snr_info(uint32_t tti, uint16_t rnti, float snr)
{
  log_h->step(tti);
  int ret = -1;
  pthread_rwlock_rdlock(&rwlock);
  if (ue_db.count(rnti)) {
    uint32_t cqi = srslte_cqi_from_snr(snr);
    scheduler.ul_cqi_info(tti, rnti, cqi, 0);
    ret = 0;
  } else {
    Error("User rnti=0x%x not found\n", rnti);
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

int mac::sr_detected(uint32_t tti, uint16_t rnti)
{
  log_h->step(tti);
  int ret = -1;
  pthread_rwlock_rdlock(&rwlock);
  if (ue_db.count(rnti)) {
    scheduler.ul_sr_info(tti, rnti);
    ret = 0;
  } else {
    Error("User rnti=0x%x not found\n", rnti);
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
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

  pthread_rwlock_rdlock(&rwlock);

  // Create new UE
  ue_db[last_rnti] = new ue(last_rnti, cell.nof_prb, &scheduler, rrc_h, rlc_h, log_h);

  // Set PCAP if available
  if (pcap) {
    ue_db[last_rnti]->start_pcap(pcap);
  }

  pthread_rwlock_unlock(&rwlock);

  // Save RA info
  pending_rars[ra_id].preamble_idx = preamble_idx;
  pending_rars[ra_id].ta_cmd       = time_adv;
  pending_rars[ra_id].temp_crnti   = last_rnti;

  // Add new user to the scheduler so that it can RX/TX SRB0
  sched_interface::ue_cfg_t uecfg;
  bzero(&uecfg, sizeof(sched_interface::ue_cfg_t));
  uecfg.ue_bearers[0].direction = srsenb::sched_interface::ue_bearer_cfg_t::BOTH;
  if (scheduler.ue_cfg(last_rnti, &uecfg)) {
    // Release pending RAR
    bzero(&pending_rars[ra_id], sizeof(pending_rar_t));
    Error("Registering new user rnti=0x%x to SCHED\n", last_rnti);
    return -1;
  }

  // Register new user in RRC
  rrc_h->add_user(last_rnti);

  // Add temporal rnti to the PHY
  if (phy_h->add_rnti(last_rnti, true)) {
    Error("Registering temporal-rnti=0x%x to PHY\n", last_rnti);
  }

  // Trigger scheduler RACH
  scheduler.dl_rach_info(tti, ra_id, last_rnti, 7);

  log_h->info("RACH:  tti=%d, preamble=%d, offset=%d, temp_crnti=0x%x\n",
                 tti, preamble_idx, time_adv, last_rnti);
  log_h->console("RACH:  tti=%d, preamble=%d, offset=%d, temp_crnti=0x%x\n",
                 tti, preamble_idx, time_adv, last_rnti);

  // Increase RNTI counter
  last_rnti++;
  if (last_rnti >= 60000) {
    last_rnti = 70;
  }
  return 0;
}

int mac::get_dl_sched(uint32_t tti, dl_sched_t *dl_sched_res)
{
  if (!started) {
    return 0;
  }

  log_h->step(tti);

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

  pthread_rwlock_rdlock(&rwlock);

  // Copy data grants
  for (uint32_t i=0;i<sched_result.nof_data_elems;i++) {

    // Get UE
    uint16_t rnti = sched_result.data[i].dci.rnti;

    if (ue_db.count(rnti)) {
      // Copy dci info
      dl_sched_res->pdsch[n].dci = sched_result.data[i].dci;

      for (uint32_t tb = 0; tb < SRSLTE_MAX_TB; tb++) {
        dl_sched_res->pdsch[n].softbuffer_tx[tb] = ue_db[rnti]->get_tx_softbuffer(sched_result.data[i].dci.pid, tb);

        if (sched_result.data[i].nof_pdu_elems[tb] > 0) {
          /* Get PDU if it's a new transmission */
          dl_sched_res->pdsch[n].data[tb] = ue_db[rnti]->generate_pdu(sched_result.data[i].dci.pid,
                                                                      tb,
                                                                      sched_result.data[i].pdu[tb],
                                                                      sched_result.data[i].nof_pdu_elems[tb],
                                                                      sched_result.data[i].tbs[tb]);

          if (!dl_sched_res->pdsch[n].data[tb]) {
            Error("Error! PDU was not generated (rnti=0x%04x, tb=%d)\n", rnti, tb);
          }

          if (pcap) {
            pcap->write_dl_crnti(dl_sched_res->pdsch[n].data[tb], sched_result.data[i].tbs[tb], rnti, true, tti);
          }

        } else {
          /* TB not enabled OR no data to send: set pointers to NULL  */
          dl_sched_res->pdsch[n].data[tb] = NULL;
        }
      }
      n++;
    } else {
      Warning("Invalid DL scheduling result. User 0x%x does not exist\n", rnti);
    }
  }

  // No more uses of shared ue_db beyond here
  pthread_rwlock_unlock(&rwlock);

  // Copy RAR grants
  for (uint32_t i=0;i<sched_result.nof_rar_elems;i++) {
    // Copy dci info
    dl_sched_res->pdsch[n].dci = sched_result.rar[i].dci;

    // Set softbuffer (there are no retx in RAR but a softbuffer is required)
    dl_sched_res->pdsch[n].softbuffer_tx[0] = &rar_softbuffer_tx;

    // Assemble PDU
    dl_sched_res->pdsch[n].data[0] =
        assemble_rar(sched_result.rar[i].msg3_grant, sched_result.rar[i].nof_grants, i, sched_result.rar[i].tbs);

    if (pcap) {
      pcap->write_dl_ranti(
          dl_sched_res->pdsch[n].data[0], sched_result.rar[i].tbs, dl_sched_res->pdsch[n].dci.rnti, true, tti);
    }

    n++;
  }

  // Copy SI and Paging grants
  for (uint32_t i = 0; i < sched_result.nof_bc_elems; i++) {
    // Copy dci info
    dl_sched_res->pdsch[n].dci = sched_result.bc[i].dci;

    // Set softbuffer
    if (sched_result.bc[i].type == sched_interface::dl_sched_bc_t::BCCH) {
      dl_sched_res->pdsch[n].softbuffer_tx[0] = &bcch_softbuffer_tx[sched_result.bc[i].index];
      dl_sched_res->pdsch[n].data[0]          = assemble_si(sched_result.bc[i].index);
#ifdef WRITE_SIB_PCAP
      if (pcap) {
        pcap->write_dl_sirnti(dl_sched_res->pdsch[n].data[0], sched_result.bc[i].tbs, true, tti);
      }
#endif
    } else {
      dl_sched_res->pdsch[n].softbuffer_tx[0] = &pcch_softbuffer_tx;
      dl_sched_res->pdsch[n].data[0]          = pcch_payload_buffer;
      rlc_h->read_pdu_pcch(pcch_payload_buffer, pcch_payload_buffer_len);

      if (pcap) {
        pcap->write_dl_pch(dl_sched_res->pdsch[n].data[0], sched_result.bc[i].tbs, true, tti);
      }
    }

    n++;
  }

  dl_sched_res->nof_grants = n;

  // Number of CCH symbols
  dl_sched_res->cfi = sched_result.cfi;

  return SRSLTE_SUCCESS;
}

void mac::build_mch_sched(uint32_t tbs)
{
  int sfs_per_sched_period = mcch.msg.c1().mbsfn_area_cfg_r9().pmch_info_list_r9[0].pmch_cfg_r9.sf_alloc_end_r9;
  int bytes_per_sf = tbs/8 - 6;// leave 6 bytes for header

  int total_space_avail_bytes =  sfs_per_sched_period*bytes_per_sf;

  int total_bytes_to_tx = 0;


  // calculate total bytes to be scheduled
  for (uint32_t i = 0; i < mch.num_mtch_sched; i++) {
    total_bytes_to_tx += mch.mtch_sched[i].lcid_buffer_size;
    mch.mtch_sched[i].stop = 0;
  }

  int last_mtch_stop = 0;

  if (total_bytes_to_tx > 0 && total_bytes_to_tx >= total_space_avail_bytes) {
    for(uint32_t i = 0; i < mch.num_mtch_sched;i++){
       double ratio =  mch.mtch_sched[i].lcid_buffer_size/total_bytes_to_tx;
       float assigned_sfs = floor(sfs_per_sched_period*ratio);
       mch.mtch_sched[i].stop = last_mtch_stop + (uint32_t)assigned_sfs;
       last_mtch_stop = mch.mtch_sched[i].stop;
    }
  } else {
    for(uint32_t i = 0; i < mch.num_mtch_sched;i++){
      float assigned_sfs =  ceil(((float)mch.mtch_sched[i].lcid_buffer_size)/((float)bytes_per_sf));
      mch.mtch_sched[i].stop = last_mtch_stop + (uint32_t)assigned_sfs;
      last_mtch_stop = mch.mtch_sched[i].stop;
    }
  }
}

int mac::get_mch_sched(uint32_t tti, bool is_mcch, dl_sched_t* dl_sched_res)
{
  log_h->step(tti);
  srslte_ra_tb_t mcs      = {};
  srslte_ra_tb_t mcs_data = {};
  mcs.mcs_idx             = this->sib13.mbsfn_area_info_list_r9[0].mcch_cfg_r9.sig_mcs_r9.to_number();
  mcs_data.mcs_idx = this->mcch.msg.c1().mbsfn_area_cfg_r9().pmch_info_list_r9[0].pmch_cfg_r9.data_mcs_r9;
  srslte_dl_fill_ra_mcs(&mcs, 0, this->cell_config.cell.nof_prb, false);
  srslte_dl_fill_ra_mcs(&mcs_data, 0, this->cell_config.cell.nof_prb, false);
  if (is_mcch) {
    build_mch_sched(mcs_data.tbs);
    mch.mcch_payload = mcch_payload_buffer;
    mch.current_sf_allocation_num = 1;
    Info("MCH Sched Info: LCID: %d, Stop: %d, tti is %d \n",
         mch.mtch_sched[0].lcid, mch.mtch_sched[mch.num_mtch_sched - 1].stop,
         tti);
    phy_h->set_mch_period_stop(mch.mtch_sched[mch.num_mtch_sched - 1].stop);
    for(uint32_t i = 0; i < mch.num_mtch_sched; i++) {
      mch.pdu[i].lcid = srslte::sch_subh::MCH_SCHED_INFO;
     // mch.mtch_sched[i].lcid = 1+i;
    }

    mch.pdu[mch.num_mtch_sched].lcid = 0;
    mch.pdu[mch.num_mtch_sched].nbytes = current_mcch_length;
    dl_sched_res->pdsch[0].dci.rnti    = SRSLTE_MRNTI;

    // we use TTI % HARQ to make sure we use different buffers for consecutive TTIs to avoid races between PHY workers
    dl_sched_res->pdsch[0].data[0] =
        ue_db[SRSLTE_MRNTI]->generate_mch_pdu(tti % SRSLTE_FDD_NOF_HARQ, mch, mch.num_mtch_sched + 1, mcs.tbs / 8);

  } else {
    uint32_t current_lcid = 1;
    uint32_t mtch_index = 0;
    uint32_t mtch_stop = mch.mtch_sched[mch.num_mtch_sched -1].stop;

    for(uint32_t i = 0;i < mch.num_mtch_sched;i++) {
      if(mch.current_sf_allocation_num <= mch.mtch_sched[i].stop){
        current_lcid = mch.mtch_sched[i].lcid;
        mtch_index = i;
        break;
      }
    }
    if(mch.current_sf_allocation_num <= mtch_stop) {
      int requested_bytes = (mcs_data.tbs/8 > (int)mch.mtch_sched[mtch_index].lcid_buffer_size)?(mch.mtch_sched[mtch_index].lcid_buffer_size):((mcs_data.tbs/8) - 2);
      int  bytes_received = ue_db[SRSLTE_MRNTI]->read_pdu(current_lcid, mtch_payload_buffer, requested_bytes);
      mch.pdu[0].lcid = current_lcid;
      mch.pdu[0].nbytes = bytes_received;
      mch.mtch_sched[0].mtch_payload  = mtch_payload_buffer;
      dl_sched_res->pdsch[0].dci.rnti = SRSLTE_MRNTI;
      if (bytes_received) {
        dl_sched_res->pdsch[0].data[0] =
            ue_db[SRSLTE_MRNTI]->generate_mch_pdu(tti % SRSLTE_FDD_NOF_HARQ, mch, 1, mcs_data.tbs / 8);
      }
    } else {
      dl_sched_res->pdsch[0].dci.rnti = 0;
      dl_sched_res->pdsch[0].data[0]  = NULL;
    }
    mch.current_sf_allocation_num++;
  }

 return SRSLTE_SUCCESS;
}

uint8_t* mac::assemble_rar(sched_interface::dl_sched_rar_grant_t* grants, uint32_t nof_grants, int rar_idx, uint32_t pdu_len)
{
  uint8_t grant_buffer[64] = {};
  if (pdu_len < rar_payload_len) {
    srslte::rar_pdu *pdu = &rar_pdu_msg[rar_idx];
    rar_payload[rar_idx].clear();
    pdu->init_tx(&rar_payload[rar_idx], pdu_len);
    for (uint32_t i = 0; i < nof_grants; i++) {
      srslte_dci_rar_pack(&grants[i].grant, grant_buffer);
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
    pdu->write_packet(rar_payload[rar_idx].msg);
    return rar_payload[rar_idx].msg;
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

  pthread_rwlock_rdlock(&rwlock);

  // Copy DCI grants
  ul_sched_res->nof_grants = 0;
  int n = 0;
  for (uint32_t i=0;i<sched_result.nof_dci_elems;i++) {

    if (sched_result.pusch[i].tbs > 0) {
      // Get UE
      uint16_t rnti = sched_result.pusch[i].dci.rnti;

      if (ue_db.count(rnti)) {
        // Copy grant info
        ul_sched_res->pusch[n].current_tx_nb = sched_result.pusch[i].current_tx_nb;
        ul_sched_res->pusch[n].needs_pdcch   = sched_result.pusch[i].needs_pdcch;
        ul_sched_res->pusch[n].dci           = sched_result.pusch[i].dci;
        ul_sched_res->pusch[n].softbuffer_rx = ue_db[rnti]->get_rx_softbuffer(tti);

        if (sched_result.pusch[n].current_tx_nb == 0) {
          srslte_softbuffer_rx_reset_tbs(ul_sched_res->pusch[n].softbuffer_rx, sched_result.pusch[i].tbs * 8);
        }
        ul_sched_res->pusch[n].data = ue_db[rnti]->request_buffer(tti, sched_result.pusch[i].tbs);
        ul_sched_res->nof_grants++;
        n++;
      } else {
        Warning("Invalid DL scheduling result. User 0x%x does not exist\n", rnti);
      }

    } else {
      Warning("Grant %d for rnti=0x%x has zero TBS\n", i, sched_result.pusch[i].dci.rnti);
    }
  }

  // No more uses of ue_db beyond here
  pthread_rwlock_unlock(&rwlock);

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
 * DEMUX unit
 *
 *******************************************************/
mac::pdu_process::pdu_process(pdu_process_handler* h) : running(false), thread("MAC_PDU_PROCESS")
{
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
  pthread_rwlock_rdlock(&rwlock);
  bool ret = false;
  for(std::map<uint16_t, ue*>::iterator iter=ue_db.begin(); iter!=ue_db.end(); ++iter) {
    ue *u         = iter->second;
    uint16_t rnti = iter->first;
    ret = ret | u->process_pdus();
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

void mac::write_mcch(sib_type2_s* sib2, sib_type13_r9_s* sib13, mcch_msg_s* mcch)
{
  this->mcch         = *mcch;
  mch.num_mtch_sched = this->mcch.msg.c1().mbsfn_area_cfg_r9().pmch_info_list_r9[0].mbms_session_info_list_r9.size();
  for (uint32_t i = 0; i < mch.num_mtch_sched; ++i) {
    mch.mtch_sched[i].lcid =
        this->mcch.msg.c1().mbsfn_area_cfg_r9().pmch_info_list_r9[0].mbms_session_info_list_r9[i].lc_ch_id_r9;
  }
  this->sib2  = *sib2;
  this->sib13 = *sib13;

  const int     rlc_header_len = 1;
  asn1::bit_ref bref(&mcch_payload_buffer[rlc_header_len], sizeof(mcch_payload_buffer) - rlc_header_len);
  mcch->pack(bref);
  current_mcch_length = bref.distance_bytes(&mcch_payload_buffer[1]);
  current_mcch_length =  current_mcch_length + rlc_header_len;
  ue_db[SRSLTE_MRNTI] = new ue(SRSLTE_MRNTI, cell.nof_prb, &scheduler, rrc_h, rlc_h, log_h);

  rrc_h->add_user(SRSLTE_MRNTI);
}

}



