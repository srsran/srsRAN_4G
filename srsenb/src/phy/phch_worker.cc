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

#include <assert.h>

#include "srslte/common/threads.h"
#include "srslte/common/log.h"

#include "phy/phch_worker.h"

#define Error(fmt, ...)   if (SRSLTE_DEBUG_ENABLED) log_h->error_line(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) if (SRSLTE_DEBUG_ENABLED) log_h->warning_line(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define Info(fmt, ...)    if (SRSLTE_DEBUG_ENABLED) log_h->info_line(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define Debug(fmt, ...)   if (SRSLTE_DEBUG_ENABLED) log_h->debug_line(__FILE__, __LINE__, fmt, ##__VA_ARGS__)

using namespace std; 

// Enable this to log SI
//#define LOG_THIS(a) 1

// Enable this one to skip SI-RNTI
#define LOG_THIS(rnti) (rnti != 0xFFFF)


/* Define GUI-related things */
#ifdef ENABLE_GUI
#include "srsgui/srsgui.h"
#include <semaphore.h>
#include <srslte/phy/phch/ra.h>
#include <srslte/srslte.h>
#include <srslte/phy/phch/pdsch.h>
#include <srslte/phy/common/sequence.h>

void init_plots(srsenb::phch_worker *worker);
pthread_t plot_thread; 
sem_t plot_sem; 
static int plot_worker_id = -1;
#else
#warning Compiling without srsGUI support
#endif
/*********************************************/



//#define DEBUG_WRITE_FILE

namespace srsenb {


phch_worker::phch_worker()
{
  phy = NULL;
  reset();  
}

#ifdef DEBUG_WRITE_FILE
FILE *f; 
#endif

void phch_worker::init(phch_common* phy_, srslte::log *log_h_)
{
  phy   = phy_; 
  log_h = log_h_; 
  
  pthread_mutex_init(&mutex, NULL); 
  
  // Init cell here
  signal_buffer_rx    = (cf_t*) srslte_vec_malloc(2*SRSLTE_SF_LEN_PRB(phy->cell.nof_prb)*sizeof(cf_t));
  if (!signal_buffer_rx) {
    fprintf(stderr, "Error allocating memory\n");
    return; 
  }
  signal_buffer_tx = (cf_t*) srslte_vec_malloc(2*SRSLTE_SF_LEN_PRB(phy->cell.nof_prb)*sizeof(cf_t));
  if (!signal_buffer_tx) {
    fprintf(stderr, "Error allocating memory\n");
    return; 
  }
  if (srslte_enb_dl_init(&enb_dl, phy->cell.nof_prb)) {
    fprintf(stderr, "Error initiating ENB DL\n");
    return;
  }
  if (srslte_enb_dl_set_cell(&enb_dl, phy->cell)) {
    fprintf(stderr, "Error initiating ENB DL\n");
    return;
  }
  if (srslte_enb_ul_init(&enb_ul, phy->cell.nof_prb)) {
    fprintf(stderr, "Error initiating ENB UL\n");
    return;
  }
  if (srslte_enb_ul_set_cell(&enb_ul,
                              phy->cell,
                              NULL,
                              &phy->pusch_cfg,
                              &phy->hopping_cfg,
                              &phy->pucch_cfg))
  {
    fprintf(stderr, "Error initiating ENB UL\n");
    return;
  }
  
  /* Setup SI-RNTI in PHY */
  add_rnti(SRSLTE_SIRNTI);

  /* Setup P-RNTI in PHY */
  add_rnti(SRSLTE_PRNTI);

  /* Setup RA-RNTI in PHY */
  for (int i=0;i<10;i++) {
    add_rnti(1+i);
  }

  srslte_pucch_set_threshold(&enb_ul.pucch, 0.8);
  srslte_sch_set_max_noi(&enb_ul.pusch.ul_sch, phy->params.pusch_max_its);
  srslte_enb_dl_set_amp(&enb_dl, phy->params.tx_amplitude);
  
  Info("Worker %d configured cell %d PRB\n", get_id(), phy->cell.nof_prb);
  
  initiated = true; 
  running   = true;

#ifdef DEBUG_WRITE_FILE
  f = fopen("test.dat", "w");
#endif
}

void phch_worker::stop()
{
  running = false;
  pthread_mutex_lock(&mutex);

  srslte_enb_dl_free(&enb_dl);
  srslte_enb_ul_free(&enb_ul);
  if (signal_buffer_rx) {
    free(signal_buffer_rx);
  }
  if (signal_buffer_tx) {
    free(signal_buffer_tx);
  }
  pthread_mutex_unlock(&mutex);
  pthread_mutex_destroy(&mutex);
}
void phch_worker::reset() 
{
  initiated  = false; 
  ue_db.clear();
}

cf_t* phch_worker::get_buffer_rx()
{
  return signal_buffer_rx;
}

void phch_worker::set_time(uint32_t tti_, uint32_t tx_mutex_cnt_, srslte_timestamp_t tx_time_)
{
  tti_rx       = tti_; 
  tti_tx       = (tti_ + 4)%10240; 
  tti_sched_ul = (tti_ + 8)%10240; 
  sf_rx        = tti_rx%10;
  sf_tx        = tti_tx%10;
  sf_sched_ul  = tti_sched_ul%10;
  tx_mutex_cnt = tx_mutex_cnt_;
  memcpy(&tx_time, &tx_time_, sizeof(srslte_timestamp_t));
}

int phch_worker::add_rnti(uint16_t rnti)
{

  if (srslte_enb_dl_add_rnti(&enb_dl, rnti)) {
    return -1; 
  }
  if (srslte_enb_ul_add_rnti(&enb_ul, rnti)) {
    return -1; 
  }

  // Create user 
  ue_db[rnti].rnti = rnti; 
    
  return SRSLTE_SUCCESS; 

}

uint32_t phch_worker::get_nof_rnti() {
  return ue_db.size();
}

void phch_worker::set_config_dedicated(uint16_t rnti, 
                                       srslte_uci_cfg_t *uci_cfg, 
                                       srslte_pucch_sched_t *pucch_sched,
                                       srslte_refsignal_srs_cfg_t *srs_cfg, 
                                       uint32_t I_sr, bool pucch_cqi, uint32_t pmi_idx, bool pucch_cqi_ack)
{
  pthread_mutex_lock(&mutex); 
  if (ue_db.count(rnti)) {
    pucch_sched->N_pucch_1 = phy->pucch_cfg.n1_pucch_an;
    srslte_enb_ul_cfg_ue(&enb_ul, rnti, uci_cfg, pucch_sched, srs_cfg);
        
    ue_db[rnti].I_sr    = I_sr; 
    ue_db[rnti].I_sr_en = true;

    if (pucch_cqi) {
      ue_db[rnti].pmi_idx = pmi_idx; 
      ue_db[rnti].cqi_en  = true;       
      ue_db[rnti].pucch_cqi_ack = pucch_cqi_ack; 
    } else {
      ue_db[rnti].pmi_idx = 0; 
      ue_db[rnti].cqi_en  = false;             
    }
    
  } else {
    Error("Setting config dedicated: rnti=0x%x does not exist\n");
  }
  pthread_mutex_unlock(&mutex); 
}

void phch_worker::rem_rnti(uint16_t rnti)
{
  pthread_mutex_lock(&mutex); 
  if (ue_db.count(rnti)) {
    ue_db.erase(rnti);
    
    srslte_enb_dl_rem_rnti(&enb_dl, rnti); 
    srslte_enb_ul_rem_rnti(&enb_ul, rnti);
    
    // remove any pending grant for each subframe 
    for (uint32_t i=0;i<10;i++) {
      for (uint32_t j=0;j<phy->ul_grants[i].nof_grants;j++) {
        if (phy->ul_grants[i].sched_grants[j].rnti == rnti) {
          phy->ul_grants[i].sched_grants[j].rnti = 0; 
        }
      }
      for (uint32_t j=0;j<phy->dl_grants[i].nof_grants;j++) {
        if (phy->dl_grants[i].sched_grants[j].rnti == rnti) {
          phy->dl_grants[i].sched_grants[j].rnti = 0; 
        }
      }
    }
  } else {
    Error("Removing user: rnti=0x%x does not exist\n", rnti);
  }
  pthread_mutex_unlock(&mutex); 
}

void phch_worker::work_imp()
{
  uint32_t sf_ack;

  if (!running) {
    return;
  }

  pthread_mutex_lock(&mutex);
  
  mac_interface_phy::ul_sched_t *ul_grants = phy->ul_grants;
  mac_interface_phy::dl_sched_t *dl_grants = phy->dl_grants; 
  mac_interface_phy *mac = phy->mac; 
  
  log_h->step(tti_rx);
  
  Debug("Worker %d running\n", get_id());
  
  for(std::map<uint16_t, ue>::iterator iter=ue_db.begin(); iter!=ue_db.end(); ++iter) {
    uint16_t rnti = (uint16_t) iter->first;
    ue_db[rnti].has_grant_tti = -1; 
  }

  // Process UL signal
  srslte_enb_ul_fft(&enb_ul, signal_buffer_rx);

  // Decode pending UL grants for the tti they were scheduled
  decode_pusch(ul_grants[sf_rx].sched_grants, ul_grants[sf_rx].nof_grants, sf_rx);
  
  // Decode remaining PUCCH ACKs not associated with PUSCH transmission and SR signals
  decode_pucch(tti_rx);
      
  // Get DL scheduling for the TX TTI from MAC
  if (mac->get_dl_sched(tti_tx, &dl_grants[sf_tx]) < 0) {
    Error("Getting DL scheduling from MAC\n");
    goto unlock;
  } 
  
  if (dl_grants[sf_tx].cfi < 1 || dl_grants[sf_tx].cfi > 3) {
    Error("Invalid CFI=%d\n", dl_grants[sf_tx].cfi);
    goto unlock;
  }
  
  // Get UL scheduling for the TX TTI from MAC
  if (mac->get_ul_sched(tti_sched_ul, &ul_grants[sf_sched_ul]) < 0) {
    Error("Getting UL scheduling from MAC\n");
    goto unlock;
  } 
  
  // Put base signals (references, PBCH, PCFICH and PSS/SSS) into the resource grid
  srslte_enb_dl_clear_sf(&enb_dl);
  srslte_enb_dl_set_cfi(&enb_dl, dl_grants[sf_tx].cfi);
  srslte_enb_dl_put_base(&enb_dl, tti_tx);

  // Put UL/DL grants to resource grid. PDSCH data will be encoded as well. 
  encode_pdcch_dl(dl_grants[sf_tx].sched_grants, dl_grants[sf_tx].nof_grants, sf_tx);  
  encode_pdcch_ul(ul_grants[sf_sched_ul].sched_grants, ul_grants[sf_sched_ul].nof_grants, sf_tx);
  encode_pdsch(dl_grants[sf_tx].sched_grants, dl_grants[sf_tx].nof_grants, sf_tx);  
  
  // Put pending PHICH HARQ ACK/NACK indications into subframe
  encode_phich(ul_grants[sf_sched_ul].phich, ul_grants[sf_sched_ul].nof_phich, sf_tx);
  
  // Prepare for receive ACK for DL grants in sf_tx+4
  sf_ack = (sf_tx+4)%10; 
  phy->ack_clear(sf_ack);
  for (uint32_t i=0;i<dl_grants[sf_tx].nof_grants;i++) {
    // SI-RNTI and RAR-RNTI do not have ACK
    if (dl_grants[sf_tx].sched_grants[i].rnti >= SRSLTE_CRNTI_START && dl_grants[sf_tx].sched_grants[i].rnti <= SRSLTE_CRNTI_END) {
      phy->ack_set_pending(sf_ack, dl_grants[sf_tx].sched_grants[i].rnti, dl_grants[sf_tx].sched_grants[i].location.ncce);      
    }
  }
  
  // Generate signal and transmit
  srslte_enb_dl_gen_signal(&enb_dl, signal_buffer_tx);  
  Debug("Sending to radio\n");
  phy->worker_end(tx_mutex_cnt, signal_buffer_tx, SRSLTE_SF_LEN_PRB(phy->cell.nof_prb), tx_time);

#ifdef DEBUG_WRITE_FILE
  fwrite(signal_buffer_tx, SRSLTE_SF_LEN_PRB(phy->cell.nof_prb)*sizeof(cf_t), 1, f);
#endif

#ifdef DEBUG_WRITE_FILE
  if (tti_tx == 10) {
    fclose(f);
    exit(-1);
  }
#endif    
  
  /* Tell the plotting thread to draw the plots */
#ifdef ENABLE_GUI
  if ((int) get_id() == plot_worker_id) {
    sem_post(&plot_sem);    
  }
#endif

unlock:
  pthread_mutex_unlock(&mutex); 

}


int phch_worker::decode_pusch(srslte_enb_ul_pusch_t *grants, uint32_t nof_pusch, uint32_t tti)
{
  srslte_uci_data_t uci_data; 
  bzero(&uci_data, sizeof(srslte_uci_data_t));
  
  uint32_t wideband_cqi_value = 0; 
  
  uint32_t n_rb_ho = 0; 
  for (uint32_t i=0;i<nof_pusch;i++) {
    uint16_t rnti = grants[i].rnti; 
    if (rnti) {

    #ifdef LOG_EXECTIME
      char timestr[64];
      struct timeval t[3];
      gettimeofday(&t[1], NULL);
    #endif

      // Get pending ACKs with an associated PUSCH transmission
      if (phy->ack_is_pending(sf_rx, rnti)) {
        uci_data.uci_ack_len = 1; 
      }
      // Configure PUSCH CQI channel 
      srslte_cqi_value_t cqi_value;
      bool cqi_enabled = false; 
      if (ue_db[rnti].cqi_en && srslte_cqi_send(ue_db[rnti].pmi_idx, tti_rx)) {
        cqi_value.type = SRSLTE_CQI_TYPE_WIDEBAND;
        cqi_enabled = true; 
      } else if (grants[i].grant.cqi_request) {
        cqi_value.type = SRSLTE_CQI_TYPE_SUBBAND_HL;
        cqi_value.subband_hl.N = (phy->cell.nof_prb > 7) ? srslte_cqi_hl_get_no_subbands(phy->cell.nof_prb) : 0;
        cqi_enabled = true; 
      }
      if (cqi_enabled) {
        uci_data.uci_cqi_len = srslte_cqi_size(&cqi_value);
      }
      
      // mark this tti as having an ul grant to avoid pucch 
      ue_db[rnti].has_grant_tti = tti_rx; 
      
      srslte_ra_ul_grant_t phy_grant; 
      int res = -1;
      if (!srslte_ra_ul_dci_to_grant(&grants[i].grant, enb_ul.cell.nof_prb, n_rb_ho, &phy_grant, tti%8)) {
        if (phy_grant.mcs.mod == SRSLTE_MOD_64QAM) {
          phy_grant.mcs.mod = SRSLTE_MOD_16QAM;
        }
        phy_grant.Qm = SRSLTE_MIN(phy_grant.Qm, 4);
        res = srslte_enb_ul_get_pusch(&enb_ul, &phy_grant, grants[i].softbuffer,
                                                rnti, grants[i].rv_idx, 
                                                grants[i].current_tx_nb, 
                                                grants[i].data, 
                                                &uci_data, 
                                                tti);     
      } else {
        Error("Computing PUSCH grant\n");
        return SRSLTE_ERROR; 
      }
      
    #ifdef LOG_EXECTIME
      gettimeofday(&t[2], NULL);
      get_time_interval(t);
      snprintf(timestr, 64, ", dec_time=%4d us", (int) t[0].tv_usec);
    #endif
      
      bool crc_res = (res == 0); 
                   
      // Save PHICH scheduling for this user. Each user can have just 1 PUSCH grant per TTI
      ue_db[rnti].phich_info.n_prb_lowest = enb_ul.pusch_cfg.grant.n_prb_tilde[0];                                           
      ue_db[rnti].phich_info.n_dmrs       = phy_grant.ncs_dmrs;                                           

      char cqi_str[64];
      if (cqi_enabled) {
        srslte_cqi_value_unpack(uci_data.uci_cqi, &cqi_value);
        if (ue_db[rnti].cqi_en) {
          wideband_cqi_value = cqi_value.wideband.wideband_cqi;
        } else if (grants[i].grant.cqi_request) {
          wideband_cqi_value = cqi_value.subband_hl.wideband_cqi;
        }
        snprintf(cqi_str, 64, ", cqi=%d", wideband_cqi_value);
      }
      
      float snr_db  = 10*log10(srslte_chest_ul_get_snr(&enb_ul.chest)); 

      /*
      if (!crc_res && enb_ul.pusch_cfg.grant.L_prb == 1 && enb_ul.pusch_cfg.grant.n_prb[0] == 0 && snr_db > 5) {
        srslte_vec_save_file("sf_symbols", enb_ul.sf_symbols, sizeof(cf_t)*SRSLTE_SF_LEN_RE(25, SRSLTE_CP_NORM));
        srslte_vec_save_file("ce", enb_ul.ce, sizeof(cf_t)*SRSLTE_SF_LEN_RE(25, SRSLTE_CP_NORM));
        srslte_vec_save_file("d", enb_ul.pusch.d, sizeof(cf_t)*enb_ul.pusch_cfg.nbits.nof_re);
        srslte_vec_save_file("ce2", enb_ul.pusch.ce, sizeof(cf_t)*enb_ul.pusch_cfg.nbits.nof_re);
        srslte_vec_save_file("z", enb_ul.pusch.z, sizeof(cf_t)*enb_ul.pusch_cfg.nbits.nof_re);
        printf("saved sf_idx=%d, mcs=%d, tbs=%d, rnti=%d, rv=%d, snr=%.1f\n", tti%10, 
               grants[i].grant.mcs_idx, enb_ul.pusch_cfg.cb_segm.tbs, rnti, grants[i].rv_idx, snr_db); 
        exit(-1);
      }
      */
      log_h->info_hex(grants[i].data, phy_grant.mcs.tbs/8,
          "PUSCH: rnti=0x%x, prb=(%d,%d), tbs=%d, mcs=%d, rv=%d, snr=%.1f dB, n_iter=%d, crc=%s%s%s%s\n",
          rnti, phy_grant.n_prb[0], phy_grant.n_prb[0]+phy_grant.L_prb,
          phy_grant.mcs.tbs/8, phy_grant.mcs.idx, grants[i].grant.rv_idx,
          snr_db, 
          srslte_pusch_last_noi(&enb_ul.pusch),
          crc_res?"OK":"KO",
          uci_data.uci_ack_len>0?(uci_data.uci_ack?", ack=1":", ack=0"):"",
          uci_data.uci_cqi_len>0?cqi_str:"",         
          timestr);    
      
      // Notify MAC of RL status 
      if (grants[i].grant.rv_idx == 0) {
        if (res && snr_db < PUSCH_RL_SNR_DB_TH) {
          Debug("PUSCH: Radio-Link failure snr=%.1f dB\n", snr_db);
          phy->mac->rl_failure(rnti);
        } else {
          phy->mac->rl_ok(rnti);
        }        
      }
      
      // Notify MAC new received data and HARQ Indication value
      phy->mac->crc_info(tti_rx, rnti, phy_grant.mcs.tbs/8, crc_res);    
      if (uci_data.uci_ack_len) {
        phy->mac->ack_info(tti_rx, rnti, uci_data.uci_ack && (crc_res || snr_db > PUSCH_RL_SNR_DB_TH));
      }
      
      // Notify MAC of UL SNR and DL CQI 
      if (snr_db >= PUSCH_RL_SNR_DB_TH) {
        phy->mac->snr_info(tti_rx, rnti, snr_db);
      }
      if (uci_data.uci_cqi_len>0 && crc_res) {
        phy->mac->cqi_info(tti_rx, rnti, wideband_cqi_value);
      }
      
      // Save metrics stats 
      ue_db[rnti].metrics_ul(phy_grant.mcs.idx, 0, snr_db, srslte_pusch_last_noi(&enb_ul.pusch));
    }    
  }
  return SRSLTE_SUCCESS; 
}


int phch_worker::decode_pucch(uint32_t tti_rx)
{
  uint32_t sf_rx = tti_rx%10;
  srslte_uci_data_t uci_data; 
  
  for(std::map<uint16_t, ue>::iterator iter=ue_db.begin(); iter!=ue_db.end(); ++iter) {
    uint16_t rnti = (uint16_t) iter->first;

    if (rnti >= SRSLTE_CRNTI_START && rnti <= SRSLTE_CRNTI_END && ue_db[rnti].has_grant_tti != (int) tti_rx) {
      // Check if user needs to receive PUCCH 
      bool needs_pucch = false, needs_ack=false, needs_sr=false, needs_cqi=false; 
      uint32_t last_n_pdcch = 0;
      bzero(&uci_data, sizeof(srslte_uci_data_t));
      
      if (ue_db[rnti].I_sr_en) {
        if (srslte_ue_ul_sr_send_tti(ue_db[rnti].I_sr, tti_rx)) {
          needs_pucch = true; 
          needs_sr = true; 
          uci_data.scheduling_request = true; 
        }
      }      
      if (phy->ack_is_pending(sf_rx, rnti, &last_n_pdcch)) {
        needs_pucch = true; 
        needs_ack = true; 
        uci_data.uci_ack_len = 1; 
      }
      srslte_cqi_value_t cqi_value;
      if (ue_db[rnti].cqi_en && (ue_db[rnti].pucch_cqi_ack || !needs_ack)) {
        if (srslte_cqi_send(ue_db[rnti].pmi_idx, tti_rx)) {
          needs_pucch = true; 
          needs_cqi = true; 
          cqi_value.type = SRSLTE_CQI_TYPE_WIDEBAND; 
          uci_data.uci_cqi_len = srslte_cqi_size(&cqi_value);
        }
      }
      
      if (needs_pucch) {
        if (srslte_enb_ul_get_pucch(&enb_ul, rnti, last_n_pdcch, sf_rx, &uci_data)) {
          fprintf(stderr, "Error getting PUCCH\n");
          return SRSLTE_ERROR; 
        }
        if (uci_data.uci_ack_len > 0) {
          phy->mac->ack_info(tti_rx, rnti, uci_data.uci_ack && (srslte_pucch_get_last_corr(&enb_ul.pucch) >= PUCCH_RL_CORR_TH));      
        }
        if (uci_data.scheduling_request) {
          phy->mac->sr_detected(tti_rx, rnti);                
        }
        
        char cqi_str[64];
        if (uci_data.uci_cqi_len) {
          srslte_cqi_value_unpack(uci_data.uci_cqi, &cqi_value);
          phy->mac->cqi_info(tti_rx, rnti, cqi_value.wideband.wideband_cqi);
          sprintf(cqi_str, ", cqi=%d", cqi_value.wideband.wideband_cqi);
        }
        log_h->info("PUCCH: rnti=0x%x, corr=%.2f, n_pucch=%d, n_prb=%d%s%s%s\n", 
                    rnti, 
                    srslte_pucch_get_last_corr(&enb_ul.pucch),
                    enb_ul.pucch.last_n_pucch, enb_ul.pucch.last_n_prb,
                    needs_ack?(uci_data.uci_ack?", ack=1":", ack=0"):"", 
                    needs_sr?(uci_data.scheduling_request?", sr=yes":", sr=no"):"", 
                    needs_cqi?cqi_str:"");                


        // Notify MAC of RL status 
        if (!needs_sr) {
          if (srslte_pucch_get_last_corr(&enb_ul.pucch) < PUCCH_RL_CORR_TH) {
            Debug("PUCCH: Radio-Link failure corr=%.1f\n", srslte_pucch_get_last_corr(&enb_ul.pucch));
            phy->mac->rl_failure(rnti);
          } else {
            phy->mac->rl_ok(rnti);
          }          
        }                
      }
    }
  }    
  return 0; 
}


int phch_worker::encode_phich(srslte_enb_dl_phich_t *acks, uint32_t nof_acks, uint32_t sf_idx)
{
  for (uint32_t i=0;i<nof_acks;i++) {
    uint16_t rnti = acks[i].rnti;
    if (rnti) {
      srslte_enb_dl_put_phich(&enb_dl, acks[i].ack, 
                              ue_db[rnti].phich_info.n_prb_lowest, 
                              ue_db[rnti].phich_info.n_dmrs, 
                              sf_idx);
      
      Info("PHICH: rnti=0x%x, hi=%d, I_lowest=%d, n_dmrs=%d, tti_tx=%d\n", 
          rnti, acks[i].ack, 
          ue_db[rnti].phich_info.n_prb_lowest, 
          ue_db[rnti].phich_info.n_dmrs, tti_tx);
    }
  }
  return SRSLTE_SUCCESS;
}


int phch_worker::encode_pdcch_ul(srslte_enb_ul_pusch_t *grants, uint32_t nof_grants, uint32_t sf_idx)
{
  for (uint32_t i=0;i<nof_grants;i++) {
    uint16_t rnti = grants[i].rnti;
    if (grants[i].needs_pdcch && rnti) {
      if (srslte_enb_dl_put_pdcch_ul(&enb_dl, &grants[i].grant, grants[i].location, rnti, sf_idx)) {
        fprintf(stderr, "Error putting PUSCH %d\n",i);
        return SRSLTE_ERROR; 
      }

      Info("PDCCH: UL DCI Format0  rnti=0x%x, cce_index=%d, L=%d, tti_tx=%d\n", 
          rnti, grants[i].location.ncce, (1<<grants[i].location.L), tti_tx);
    }
  }
  return SRSLTE_SUCCESS; 
}

int phch_worker::encode_pdcch_dl(srslte_enb_dl_pdsch_t *grants, uint32_t nof_grants, uint32_t sf_idx)
{
  for (uint32_t i=0;i<nof_grants;i++) {
    uint16_t rnti = grants[i].rnti;
    if (rnti) {
      srslte_dci_format_t format = SRSLTE_DCI_FORMAT1; 
      switch(grants[i].grant.alloc_type) {
        case SRSLTE_RA_ALLOC_TYPE0:
        case SRSLTE_RA_ALLOC_TYPE1:
          format = SRSLTE_DCI_FORMAT1; 
        break;
        case SRSLTE_RA_ALLOC_TYPE2:
          format = SRSLTE_DCI_FORMAT1A; 
        break;
      }
      if (srslte_enb_dl_put_pdcch_dl(&enb_dl, &grants[i].grant, format, grants[i].location, rnti, sf_idx)) {
        fprintf(stderr, "Error putting PDCCH %d\n",i);
        return SRSLTE_ERROR; 
      }      
      
      if (LOG_THIS(rnti)) {
        Info("PDCCH: DL DCI %s rnti=0x%x, cce_index=%d, L=%d, tti_tx=%d\n", srslte_dci_format_string(format), 
          rnti, grants[i].location.ncce, (1<<grants[i].location.L), tti_tx);
      }
    }
  }
  return 0; 
}

int phch_worker::encode_pdsch(srslte_enb_dl_pdsch_t *grants, uint32_t nof_grants, uint32_t sf_idx)
{
  for (uint32_t i=0;i<nof_grants;i++) {
    uint16_t rnti = grants[i].rnti;
    if (rnti) {

      bool rnti_is_user = true; 
      if (rnti == SRSLTE_SIRNTI || rnti == SRSLTE_PRNTI || rnti == SRSLTE_MRNTI) {
        rnti_is_user = false; 
      }
      
      srslte_ra_dl_grant_t phy_grant; 
      srslte_ra_dl_dci_to_grant(&grants[i].grant, enb_dl.cell.nof_prb, rnti, &phy_grant);
      
      char grant_str[64];
      switch(grants[i].grant.alloc_type) {
        case SRSLTE_RA_ALLOC_TYPE0:
          sprintf(grant_str, "mask=0x%x",grants[i].grant.type0_alloc.rbg_bitmask);
        break;
        case SRSLTE_RA_ALLOC_TYPE1:
          sprintf(grant_str, "mask=0x%x",grants[i].grant.type1_alloc.vrb_bitmask);
        break;
        default:
          sprintf(grant_str, "rb_start=%d",grants[i].grant.type2_alloc.RB_start);
        break;
      }
      
      if (LOG_THIS(rnti)) { 
        uint8_t x = 0;
        uint8_t *ptr = grants[i].data;
        uint32_t len = phy_grant.mcs[0].tbs / (uint32_t) 8;
        if (!ptr) {          
          ptr = &x;
          len = 1; 
        }        
        log_h->info_hex(ptr, len,
                             "PDSCH: rnti=0x%x, l_crb=%2d, %s, harq=%d, tbs=%d, mcs=%d, rv=%d, tti_tx=%d\n", 
                             rnti, phy_grant.nof_prb, grant_str, grants[i].grant.harq_process, 
                             phy_grant.mcs[0].tbs/8, phy_grant.mcs[0].idx, grants[i].grant.rv_idx, tti_tx);
      }

      srslte_softbuffer_tx_t *sb[SRSLTE_MAX_CODEWORDS] = {grants[i].softbuffer, NULL};
      uint8_t                 *d[SRSLTE_MAX_CODEWORDS] = {grants[i].data, NULL};
      int                     rv[SRSLTE_MAX_CODEWORDS] = {grants[i].grant.rv_idx, 0};


      if (srslte_enb_dl_put_pdsch(&enb_dl, &phy_grant, sb, rnti, rv, sf_idx, d, SRSLTE_MIMO_TYPE_SINGLE_ANTENNA, 0))
      {
        fprintf(stderr, "Error putting PDSCH %d\n",i);
        return SRSLTE_ERROR; 
      }

      // Save metrics stats 
      ue_db[rnti].metrics_dl(phy_grant.mcs[0].idx);
    }
  }
  return SRSLTE_SUCCESS; 
}



/************ METRICS interface ********************/
uint32_t phch_worker::get_metrics(phy_metrics_t metrics[ENB_METRICS_MAX_USERS])
{
  uint32_t cnt=0;
  for(std::map<uint16_t, ue>::iterator iter=ue_db.begin(); iter!=ue_db.end(); ++iter) {
    ue *u = (ue*) &iter->second;
    uint16_t rnti = iter->first; 
    if (rnti >= SRSLTE_CRNTI_START && rnti <= SRSLTE_CRNTI_END) {
      u->metrics_read(&metrics[cnt]);
      cnt++;
    }
  } 
  return cnt;
}

void phch_worker::ue::metrics_read(phy_metrics_t* metrics_)
{
  memcpy(metrics_, &metrics, sizeof(phy_metrics_t));
  bzero(&metrics, sizeof(phy_metrics_t));
}

void phch_worker::ue::metrics_dl(uint32_t mcs)
{
  metrics.dl.mcs = SRSLTE_VEC_CMA(mcs, metrics.dl.mcs, metrics.dl.n_samples);
  metrics.dl.n_samples++;
}

void phch_worker::ue::metrics_ul(uint32_t mcs, float rssi, float sinr, uint32_t turbo_iters)
{
  metrics.ul.mcs         = SRSLTE_VEC_CMA((float) mcs,         metrics.ul.mcs,         metrics.ul.n_samples);
  metrics.ul.sinr        = SRSLTE_VEC_CMA((float) sinr,        metrics.ul.sinr,        metrics.ul.n_samples);
  metrics.ul.rssi        = SRSLTE_VEC_CMA((float) sinr,        metrics.ul.rssi,        metrics.ul.n_samples);
  metrics.ul.turbo_iters = SRSLTE_VEC_CMA((float) turbo_iters, metrics.ul.turbo_iters, metrics.ul.n_samples);  
  metrics.ul.n_samples++;
}


  












void phch_worker::start_plot() {
#ifdef ENABLE_GUI
  if (plot_worker_id == -1) {
    plot_worker_id = get_id();
    log_h->console("Starting plot for worker_id=%d\n", plot_worker_id);
    init_plots(this);
  } else {
    log_h->console("Trying to start a plot but already started by worker_id=%d\n", plot_worker_id);
  }
#else 
    log_h->console("Trying to start a plot but plots are disabled (ENABLE_GUI constant in phch_worker.cc)\n");
#endif
}


int phch_worker::read_ce_abs(float *ce_abs) {
  uint32_t i=0;
  int sz = srslte_symbol_sz(phy->cell.nof_prb);
  bzero(ce_abs, sizeof(float)*sz);
  int g = (sz - 12*phy->cell.nof_prb)/2;
  for (i = 0; i < 12*phy->cell.nof_prb; i++) {
    ce_abs[g+i] = 20 * log10(cabs(enb_ul.ce[i]));
    if (isinf(ce_abs[g+i])) {
      ce_abs[g+i] = -80;
    }
  }
  return sz;
}

int phch_worker::read_pusch_d(cf_t* pdsch_d)
{
  int nof_re = 400;//enb_ul.pusch_cfg.nbits.nof_re
  memcpy(pdsch_d, enb_ul.pusch.d, nof_re*sizeof(cf_t));
  return nof_re; 
}


}


/***********************************************************
 * 
 * PLOT TO VISUALIZE THE CHANNEL RESPONSEE 
 * 
 ***********************************************************/


#ifdef ENABLE_GUI
plot_real_t    pce;
plot_scatter_t pconst;
#define SCATTER_PUSCH_BUFFER_LEN   (20*6*SRSLTE_SF_LEN_RE(SRSLTE_MAX_PRB, SRSLTE_CP_NORM))
#define SCATTER_PUSCH_PLOT_LEN    4000
float tmp_plot[SCATTER_PUSCH_BUFFER_LEN];
cf_t  tmp_plot2[SRSLTE_SF_LEN_RE(SRSLTE_MAX_PRB, SRSLTE_CP_NORM)];

void *plot_thread_run(void *arg) {
  srsenb::phch_worker *worker = (srsenb::phch_worker*) arg; 
  
  sdrgui_init_title("srsENB");  
  plot_real_init(&pce);
  plot_real_setTitle(&pce, (char*) "Channel Response - Magnitude");
  plot_real_setLabels(&pce, (char*) "Index", (char*) "dB");
  plot_real_setYAxisScale(&pce, -40, 40);
  
  plot_scatter_init(&pconst);
  plot_scatter_setTitle(&pconst, (char*) "PUSCH - Equalized Symbols");
  plot_scatter_setXAxisScale(&pconst, -4, 4);
  plot_scatter_setYAxisScale(&pconst, -4, 4);
  
  plot_real_addToWindowGrid(&pce, (char*)"srsenb", 0, 0);
  plot_scatter_addToWindowGrid(&pconst, (char*)"srsenb", 0, 1);

  int n; 
  int readed_pusch_re=0; 
  while(1) {
    sem_wait(&plot_sem);    
    
    n = worker->read_pusch_d(tmp_plot2);
    plot_scatter_setNewData(&pconst, tmp_plot2, n);
    n = worker->read_ce_abs(tmp_plot);
    plot_real_setNewData(&pce, tmp_plot, n);             
    
  }  
  return NULL;
}


void init_plots(srsenb::phch_worker *worker) {

  if (sem_init(&plot_sem, 0, 0)) {
    perror("sem_init");
    exit(-1);
  }
  
  pthread_attr_t attr;
  struct sched_param param;
  param.sched_priority = 0;  
  pthread_attr_init(&attr);
  pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
  pthread_attr_setschedpolicy(&attr, SCHED_OTHER);
  pthread_attr_setschedparam(&attr, &param);
  if (pthread_create(&plot_thread, &attr, plot_thread_run, worker)) {
    perror("pthread_create");
    exit(-1);
  }  
}
#endif




