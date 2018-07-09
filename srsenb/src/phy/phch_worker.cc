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

#include "srsenb/hdr/phy/phch_worker.h"

#define Error(fmt, ...)   if (SRSLTE_DEBUG_ENABLED) log_h->error(fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) if (SRSLTE_DEBUG_ENABLED) log_h->warning(fmt, ##__VA_ARGS__)
#define Info(fmt, ...)    if (SRSLTE_DEBUG_ENABLED) log_h->info(fmt, ##__VA_ARGS__)
#define Debug(fmt, ...)   if (SRSLTE_DEBUG_ENABLED) log_h->debug(fmt, ##__VA_ARGS__)

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

  bzero(&enb_dl, sizeof(enb_dl));
  bzero(&enb_ul, sizeof(enb_ul));
  bzero(&tx_time, sizeof(tx_time));

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
  for(int p = 0; p < SRSLTE_MAX_PORTS; p++) {
    signal_buffer_rx[p] = (cf_t *) srslte_vec_malloc(2 * SRSLTE_SF_LEN_PRB(phy->cell.nof_prb) * sizeof(cf_t));
    if (!signal_buffer_rx[p]) {
      fprintf(stderr, "Error allocating memory\n");
      return;
    }
    bzero(signal_buffer_rx[p], 2 * SRSLTE_SF_LEN_PRB(phy->cell.nof_prb) * sizeof(cf_t));
    signal_buffer_tx[p] = (cf_t *) srslte_vec_malloc(2 * SRSLTE_SF_LEN_PRB(phy->cell.nof_prb) * sizeof(cf_t));
    if (!signal_buffer_tx[p]) {
      fprintf(stderr, "Error allocating memory\n");
      return;
    }
    bzero(signal_buffer_tx[p], 2 * SRSLTE_SF_LEN_PRB(phy->cell.nof_prb) * sizeof(cf_t));
  }
  if (srslte_enb_dl_init(&enb_dl, signal_buffer_tx, phy->cell.nof_prb)) {
    fprintf(stderr, "Error initiating ENB DL\n");
    return;
  }
  if (srslte_enb_dl_set_cell(&enb_dl, phy->cell)) {
    fprintf(stderr, "Error initiating ENB DL\n");
    return;
  }
  if (srslte_enb_ul_init(&enb_ul, signal_buffer_rx[0], phy->cell.nof_prb)) {
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
  
  
  if (srslte_softbuffer_tx_init(&temp_mbsfn_softbuffer, phy->cell.nof_prb)) {
    fprintf(stderr, "Error initiating soft buffer\n");
    exit(-1);
  }
  
  srslte_softbuffer_tx_reset(&temp_mbsfn_softbuffer);
  srslte_pucch_set_threshold(&enb_ul.pucch, 0.5);
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

  int cnt = 0;
  while(is_worker_running && cnt<100) {
    usleep(1000);
    cnt++;
  }

  if (!is_worker_running) {
    srslte_softbuffer_tx_free(&temp_mbsfn_softbuffer);
    srslte_enb_dl_free(&enb_dl);
    srslte_enb_ul_free(&enb_ul);
    for (int p  = 0; p < SRSLTE_MAX_PORTS; p++) {
      if (signal_buffer_rx[p]) {
        free(signal_buffer_rx[p]);
      }
      if (signal_buffer_tx[p]) {
        free(signal_buffer_tx[p]);
      }
    }
  } else {
    printf("Warning could not stop properly PHY\n");
  }
  pthread_mutex_unlock(&mutex);
  pthread_mutex_destroy(&mutex);
}
void phch_worker::reset() 
{
  initiated  = false; 
  ue_db.clear();
}

cf_t* phch_worker::get_buffer_rx(uint32_t antenna_idx)
{
  return signal_buffer_rx[antenna_idx];
}

void phch_worker::set_time(uint32_t tti_, uint32_t tx_mutex_cnt_, srslte_timestamp_t tx_time_)
{
  tti_rx       = tti_; 
  tti_tx_dl    = TTI_TX(tti_rx);
  tti_tx_ul    = TTI_RX_ACK(tti_rx);

  sf_rx        = tti_rx%10;
  sf_tx        = tti_tx_dl%10;

  t_tx_dl      = TTIMOD(tti_tx_dl);
  t_rx         = TTIMOD(tti_rx);
  t_tx_ul      = TTIMOD(tti_tx_ul);

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

void phch_worker::set_conf_dedicated_ack(uint16_t rnti, bool ack){
  pthread_mutex_lock(&mutex);
  if (ue_db.count(rnti)) {
    ue_db[rnti].dedicated_ack = ack;
  } else {
    Error("Setting dedicated ack: rnti=0x%x does not exist\n", rnti);
  }
  pthread_mutex_unlock(&mutex);
}

void phch_worker::set_config_dedicated(uint16_t rnti,
                                       srslte_refsignal_srs_cfg_t *srs_cfg, 
                                       LIBLTE_RRC_PHYSICAL_CONFIG_DEDICATED_STRUCT* dedicated)
{
  bool pucch_cqi_ack = dedicated->cqi_report_cnfg.report_periodic.simult_ack_nack_and_cqi;
  bool pucch_ri = dedicated->cqi_report_cnfg.report_periodic.ri_cnfg_idx_present;

  pthread_mutex_lock(&mutex);
  if (ue_db.count(rnti)) {
    /* PUSCH UCI and scheduling configuration */
    srslte_uci_cfg_t uci_cfg;
    ZERO_OBJECT(uci_cfg);

    if (dedicated->pusch_cnfg_ded_present && dedicated->sched_request_cnfg_present) {
      uci_cfg.I_offset_ack = dedicated->pusch_cnfg_ded.beta_offset_ack_idx;
      uci_cfg.I_offset_cqi = dedicated->pusch_cnfg_ded.beta_offset_cqi_idx;
      uci_cfg.I_offset_ri = dedicated->pusch_cnfg_ded.beta_offset_ri_idx;

      srslte_pucch_sched_t pucch_sched;
      ZERO_OBJECT(pucch_sched);

      pucch_sched.N_pucch_1 = phy->pucch_cfg.n1_pucch_an;
      pucch_sched.n_pucch_2 = dedicated->cqi_report_cnfg.report_periodic.pucch_resource_idx;
      pucch_sched.n_pucch_sr = dedicated->sched_request_cnfg.sr_pucch_resource_idx;
      srslte_enb_ul_cfg_ue(&enb_ul, rnti, &uci_cfg, &pucch_sched, srs_cfg);

      ue_db[rnti].I_sr = dedicated->sched_request_cnfg.sr_cnfg_idx;
      ue_db[rnti].I_sr_en = true;
    }

    /* CQI Reporting */
    if (dedicated->cqi_report_cnfg.report_periodic_setup_present) {
      ue_db[rnti].pmi_idx = dedicated->cqi_report_cnfg.report_periodic.pmi_cnfg_idx;
      ue_db[rnti].cqi_en = true;
      ue_db[rnti].pucch_cqi_ack = pucch_cqi_ack;
    } else {
      ue_db[rnti].pmi_idx = 0;
      ue_db[rnti].cqi_en = false;
    }

    /* RI reporting */
    if (pucch_ri) {
      ue_db[rnti].ri_idx = dedicated->cqi_report_cnfg.report_periodic.ri_cnfg_idx;
      ue_db[rnti].ri_en = true;
    } else {
      ue_db[rnti].ri_idx = 0;
      ue_db[rnti].ri_en = false;
    }

    if (dedicated->antenna_info_present) {
      /* If default antenna info then follow 3GPP 36.331 clause 9.2.4 Default physical channel configuration */
      if (dedicated->antenna_info_default_value) {
        if (enb_dl.cell.nof_ports == 1) {
          ue_db[rnti].dedicated.antenna_info_explicit_value.tx_mode = LIBLTE_RRC_TRANSMISSION_MODE_1;
        } else {
          ue_db[rnti].dedicated.antenna_info_explicit_value.tx_mode = LIBLTE_RRC_TRANSMISSION_MODE_2;
        }
        ue_db[rnti].dedicated.antenna_info_explicit_value.codebook_subset_restriction_present = false;
        ue_db[rnti].dedicated.antenna_info_explicit_value.ue_tx_antenna_selection_setup_present = false;
        ue_db[rnti].ri_idx = 0;
        ue_db[rnti].ri_en = false;
      } else {
        /* Physical channel reconfiguration according to 3GPP 36.331 clause 5.3.10.6 */
        memcpy(&ue_db[rnti].dedicated.antenna_info_explicit_value,
               &dedicated->antenna_info_explicit_value,
               sizeof(LIBLTE_RRC_ANTENNA_INFO_DEDICATED_STRUCT));
        if (dedicated->antenna_info_explicit_value.tx_mode != LIBLTE_RRC_TRANSMISSION_MODE_3 &&
            dedicated->antenna_info_explicit_value.tx_mode != LIBLTE_RRC_TRANSMISSION_MODE_4 &&
            ue_db[rnti].ri_en) {
          ue_db[rnti].ri_idx = 0;
          ue_db[rnti].ri_en = false;
        }
      }
    }

    /* Set PDSCH power allocation */
    if (dedicated->pdsch_cnfg_ded_present) {
      ue_db[rnti].dedicated.pdsch_cnfg_ded_present = true;
      ue_db[rnti].dedicated.pdsch_cnfg_ded = dedicated->pdsch_cnfg_ded;
    }
  } else {
    Error("Setting config dedicated: rnti=0x%x does not exist\n", rnti);
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
    for (uint32_t i=0;i<TTIMOD_SZ;i++) {
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
  if (!running) {
    return;
  }

  subframe_cfg_t sf_cfg;
  phy->get_sf_config(&sf_cfg, tti_tx_dl);// TODO difference between  tti_tx_dl and t_tx_dl

  pthread_mutex_lock(&mutex);
  is_worker_running = true;

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
  srslte_enb_ul_fft(&enb_ul);

  // Decode pending UL grants for the tti they were scheduled
  decode_pusch(ul_grants[t_rx].sched_grants, ul_grants[t_rx].nof_grants);

  // Decode remaining PUCCH ACKs not associated with PUSCH transmission and SR signals
  decode_pucch();

  // Get DL scheduling for the TX TTI from MAC

  if(sf_cfg.sf_type == SUBFRAME_TYPE_REGULAR) {
    if (mac->get_dl_sched(tti_tx_dl, &dl_grants[t_tx_dl]) < 0) {
      Error("Getting DL scheduling from MAC\n");
      goto unlock;
    }
  } else {
    dl_grants[t_tx_dl].cfi = sf_cfg.non_mbsfn_region_length;
    srslte_enb_dl_set_non_mbsfn_region(&enb_dl, sf_cfg.non_mbsfn_region_length);
    if(mac->get_mch_sched(sf_cfg.is_mcch, &dl_grants[t_tx_dl])){
      Error("Getting MCH packets from MAC\n");
      goto unlock;
    }
  }

  if (dl_grants[t_tx_dl].cfi < 1 || dl_grants[t_tx_dl].cfi > 3) {
    Error("Invalid CFI=%d\n", dl_grants[t_tx_dl].cfi);
    goto unlock;
  }

  // Get UL scheduling for the TX TTI from MAC
  if (mac->get_ul_sched(tti_tx_ul, &ul_grants[t_tx_ul]) < 0) {
    Error("Getting UL scheduling from MAC\n");
    goto unlock;
  }

  // Put base signals (references, PBCH, PCFICH and PSS/SSS) into the resource grid
  srslte_enb_dl_clear_sf(&enb_dl);
  srslte_enb_dl_set_cfi(&enb_dl, dl_grants[t_tx_dl].cfi);
  
  if(sf_cfg.sf_type == SUBFRAME_TYPE_REGULAR) {
    srslte_enb_dl_put_base(&enb_dl, tti_tx_dl);
  } else if (sf_cfg.mbsfn_encode){
    srslte_enb_dl_put_mbsfn_base(&enb_dl, tti_tx_dl);
  }
  
  if(sf_cfg.sf_type == SUBFRAME_TYPE_REGULAR) {
    // Put UL/DL grants to resource grid. PDSCH data will be encoded as well.
    encode_pdcch_dl(dl_grants[t_tx_dl].sched_grants, dl_grants[t_tx_dl].nof_grants);
    encode_pdsch(dl_grants[t_tx_dl].sched_grants, dl_grants[t_tx_dl].nof_grants);
  }else {
    srslte_ra_dl_grant_t phy_grant;
    phy_grant.mcs[0].idx = sf_cfg.mbsfn_mcs;
    encode_pmch(&dl_grants[t_tx_dl].sched_grants[0], &phy_grant);
  }
  
  encode_pdcch_ul(ul_grants[t_tx_ul].sched_grants, ul_grants[t_tx_ul].nof_grants);
  // Put pending PHICH HARQ ACK/NACK indications into subframe
  encode_phich(ul_grants[t_tx_ul].phich, ul_grants[t_tx_ul].nof_phich);

  // Prepare for receive ACK for DL grants in t_tx_dl+4
  phy->ue_db_clear(TTIMOD(TTI_TX(t_tx_dl)));
  
  for (uint32_t i=0;i<dl_grants[t_tx_dl].nof_grants;i++) {
    // SI-RNTI and RAR-RNTI do not have ACK
    uint16_t rnti = dl_grants[t_tx_dl].sched_grants[i].rnti;
    if (rnti >= SRSLTE_CRNTI_START && rnti <= SRSLTE_CRNTI_END) {
      /* For each TB */
      for (uint32_t tb_idx = 0; tb_idx < SRSLTE_MAX_TB; tb_idx++) {
        /* If TB enabled, set pending ACK */
        if (dl_grants[t_tx_dl].sched_grants[i].grant.tb_en[tb_idx]) {
          phy->ue_db_set_ack_pending(TTIMOD(TTI_TX(t_tx_dl)),
                                     rnti,
                                     tb_idx,
                                     dl_grants[t_tx_dl].sched_grants[i].location.ncce);
        }
      }
    }
  }

  // Generate signal and transmit
  if(sf_cfg.sf_type == SUBFRAME_TYPE_REGULAR) {
    srslte_enb_dl_gen_signal(&enb_dl);
  } else {
    srslte_enb_dl_gen_signal_mbsfn(&enb_dl);
  }

  pthread_mutex_unlock(&mutex);

  Debug("Sending to radio\n");
  phy->worker_end(tx_mutex_cnt, signal_buffer_tx, SRSLTE_SF_LEN_PRB(phy->cell.nof_prb), tx_time);

  is_worker_running = false;

#ifdef DEBUG_WRITE_FILE
  fwrite(signal_buffer_tx, SRSLTE_SF_LEN_PRB(phy->cell.nof_prb)*sizeof(cf_t), 1, f);
#endif

#ifdef DEBUG_WRITE_FILE
  if (tti_tx_dl == 10) {
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
  if (is_worker_running) {
    is_worker_running = false;
    pthread_mutex_unlock(&mutex);
  }

}


int phch_worker::decode_pusch(srslte_enb_ul_pusch_t *grants, uint32_t nof_pusch)
{
  srslte_uci_data_t uci_data;
  ZERO_OBJECT(uci_data);

  uint32_t wideband_cqi_value = 0, wideband_pmi = 0;
  bool wideband_pmi_present = false;

  uint32_t n_rb_ho = 0;
  for (uint32_t i=0;i<nof_pusch;i++) {
    uint16_t rnti = grants[i].rnti;
    if (rnti) {

    #ifdef LOG_EXECTIME
      char timestr[64];
      struct timeval t[3];
      gettimeofday(&t[1], NULL);
    #endif

      bool acks_pending[SRSLTE_MAX_TB] = {false};

      // Get pending ACKs with an associated PUSCH transmission
      for (uint32_t tb = 0; tb < SRSLTE_MAX_TB; tb++) {
        acks_pending[tb] = phy->ue_db_is_ack_pending(t_rx, rnti, tb);
        if (acks_pending[tb]) {
          uci_data.uci_ack_len++;
        }
      }

      // Configure PUSCH CQI channel
      srslte_cqi_value_t cqi_value;
      ZERO_OBJECT(cqi_value);

      bool cqi_enabled = false;

      if (ue_db[rnti].cqi_en && ue_db[rnti].ri_en && srslte_ri_send(ue_db[rnti].pmi_idx, ue_db[rnti].ri_idx, tti_rx) ) {
        uci_data.uci_ri_len = 1; /* Asumes only 1 bit for RI */
        uci_data.ri_periodic_report = true;
      } else if (ue_db[rnti].cqi_en && srslte_cqi_send(ue_db[rnti].pmi_idx, tti_rx)) {
        cqi_value.type = SRSLTE_CQI_TYPE_WIDEBAND;
        cqi_enabled = true;
        if (ue_db[rnti].dedicated.antenna_info_explicit_value.tx_mode == LIBLTE_RRC_TRANSMISSION_MODE_4) {
          cqi_value.wideband.pmi_present = true;
          cqi_value.wideband.rank_is_not_one = phy->ue_db_get_ri(rnti) > 0;
        }
      } else if (grants[i].grant.cqi_request) {
        cqi_value.type = SRSLTE_CQI_TYPE_SUBBAND_HL;
        if (ue_db[rnti].dedicated.antenna_info_present && (
            ue_db[rnti].dedicated.antenna_info_explicit_value.tx_mode == LIBLTE_RRC_TRANSMISSION_MODE_3 ||
            ue_db[rnti].dedicated.antenna_info_explicit_value.tx_mode == LIBLTE_RRC_TRANSMISSION_MODE_4
        )) {
          cqi_value.subband_hl.ri_present = true;
        }
        cqi_value.subband_hl.N = (phy->cell.nof_prb > 7) ? srslte_cqi_hl_get_no_subbands(phy->cell.nof_prb) : 0;
        cqi_value.subband_hl.four_antenna_ports = (phy->cell.nof_ports == 4);
        cqi_value.subband_hl.pmi_present = (ue_db[rnti].dedicated.cqi_report_cnfg.report_mode_aperiodic == LIBLTE_RRC_CQI_REPORT_MODE_APERIODIC_RM31);
        cqi_value.subband_hl.rank_is_not_one = phy->ue_db_get_ri(rnti) > 0;
        cqi_enabled = true;
      }

      // mark this tti as having an ul grant to avoid pucch
      ue_db[rnti].has_grant_tti = tti_rx;

      srslte_ra_ul_grant_t phy_grant;
      int res = -1;
      if (!srslte_ra_ul_dci_to_grant(&grants[i].grant, enb_ul.cell.nof_prb, n_rb_ho, &phy_grant)) {

        // Handle Format0 adaptive retx
        // Use last TBS for this TB in case of mcs>28
        if (phy_grant.mcs.idx > 28) {
          phy_grant.mcs.tbs = phy->ue_db_get_last_ul_tbs(rnti, tti_rx);
          Info("RETX: mcs=%d, old_tbs=%d pid=%d\n", phy_grant.mcs.idx, phy_grant.mcs.tbs, TTI_TX(tti_rx)%(2*HARQ_DELAY_MS));
        }
        phy->ue_db_set_last_ul_tbs(rnti, tti_rx, phy_grant.mcs.tbs);

        if (phy_grant.mcs.mod == SRSLTE_MOD_LAST) {
          phy_grant.mcs.mod = phy->ue_db_get_last_ul_mod(rnti, tti_rx);
          phy_grant.Qm      = srslte_mod_bits_x_symbol(phy_grant.mcs.mod);
        }
        phy->ue_db_set_last_ul_mod(rnti, tti_rx, phy_grant.mcs.mod);


        if (phy_grant.mcs.mod == SRSLTE_MOD_64QAM) {
          phy_grant.mcs.mod = SRSLTE_MOD_16QAM;
        }
        phy_grant.Qm = SRSLTE_MIN(phy_grant.Qm, 4);
        res = srslte_enb_ul_get_pusch(&enb_ul, &phy_grant, grants[i].softbuffer,
                                                rnti, grants[i].rv_idx,
                                                grants[i].current_tx_nb,
                                                grants[i].data,
                                                (cqi_enabled) ? &cqi_value : NULL,
                                                &uci_data,
                                                sf_rx);
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

      char cqi_str[SRSLTE_CQI_STR_MAX_CHAR];
      if (cqi_enabled) {
        if (ue_db[rnti].cqi_en) {
          wideband_cqi_value = cqi_value.wideband.wideband_cqi;
          if (cqi_value.wideband.pmi_present) {
            wideband_pmi_present = true;
            wideband_pmi = cqi_value.wideband.pmi;
          }
        } else if (grants[i].grant.cqi_request) {
          wideband_cqi_value = cqi_value.subband_hl.wideband_cqi_cw0;
          if (cqi_value.subband_hl.pmi_present) {
            wideband_pmi_present = true;
            wideband_pmi = cqi_value.subband_hl.pmi;
            if (cqi_value.subband_hl.rank_is_not_one) {
              Info("PUSCH: Aperiodic ri~1, CQI=%02d/%02d, pmi=%d for %d subbands\n",
                   cqi_value.subband_hl.wideband_cqi_cw0, cqi_value.subband_hl.wideband_cqi_cw1,
                   cqi_value.subband_hl.pmi, cqi_value.subband_hl.N);
            } else {
              Info("PUSCH: Aperiodic ri=1, CQI=%02d, pmi=%d for %d subbands\n",
                   cqi_value.subband_hl.wideband_cqi_cw0, cqi_value.subband_hl.pmi, cqi_value.subband_hl.N);
            }
          } else {
            Info("PUSCH: Aperiodic ri%s, CQI=%02d for %d subbands\n",
                 cqi_value.subband_hl.rank_is_not_one?"~1":"=1",
                 cqi_value.subband_hl.wideband_cqi_cw0, cqi_value.subband_hl.N);
          }
        }
        srslte_cqi_value_tostring(&cqi_value, cqi_str, SRSLTE_CQI_STR_MAX_CHAR);
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
      log_h->info_hex(grants[i].data, phy_grant.mcs.tbs / 8,
                      "PUSCH: rnti=0x%x, prb=(%d,%d), tbs=%d, mcs=%d, rv=%d, snr=%.1f dB, n_iter=%d, crc=%s%s%s%s%s%s%s\n",
                      rnti, phy_grant.n_prb[0], phy_grant.n_prb[0]+phy_grant.L_prb,
                      phy_grant.mcs.tbs / 8, phy_grant.mcs.idx, grants[i].grant.rv_idx,
                      snr_db,
                      srslte_pusch_last_noi(&enb_ul.pusch),
                      crc_res ? "OK" : "KO",
                      (acks_pending[0] || acks_pending[1]) ? ", ack=" : "",
                      (acks_pending[0]) ? (uci_data.uci_ack ? "1" : "0") : "",
                      (acks_pending[1]) ? (uci_data.uci_ack_2 ? "1" : "0") : "",
                      uci_data.uci_cqi_len > 0 ? cqi_str : "",
                      uci_data.uci_ri_len > 0 ? ((uci_data.uci_ri == 0) ? ", ri=0" : ", ri=1") : "",
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
      uint32_t ack_idx = 0;
      for (uint32_t tb = 0; tb < SRSLTE_MAX_TB; tb++) {
        if (acks_pending[tb]) {
          bool ack = ((ack_idx++ == 0) ? uci_data.uci_ack : uci_data.uci_ack_2);
          bool valid = (crc_res || snr_db > PUSCH_RL_SNR_DB_TH);
          phy->mac->ack_info(tti_rx, rnti, tb, ack && valid);
        }
      }

      // Notify MAC of UL SNR, DL CQI and DL RI
      if (snr_db >= PUSCH_RL_SNR_DB_TH) {
        phy->mac->snr_info(tti_rx, rnti, snr_db);
      }
      if (uci_data.uci_cqi_len>0 && crc_res) {
        phy->mac->cqi_info(tti_rx, rnti, wideband_cqi_value);
      }
      if (uci_data.uci_ri_len > 0 && crc_res) {
        phy->mac->ri_info(tti_rx, rnti, uci_data.uci_ri);
        phy->ue_db_set_ri(rnti, uci_data.uci_ri);
      }
      if (wideband_pmi_present && crc_res) {
        phy->mac->pmi_info(tti_rx, rnti, wideband_pmi);
      }

      // Save metrics stats
      ue_db[rnti].metrics_ul(phy_grant.mcs.idx, 0, snr_db, srslte_pusch_last_noi(&enb_ul.pusch));
    }
  }
  return SRSLTE_SUCCESS;
}


int phch_worker::decode_pucch()
{
  srslte_uci_data_t uci_data;

  for(std::map<uint16_t, ue>::iterator iter=ue_db.begin(); iter!=ue_db.end(); ++iter) {
    uint16_t rnti = (uint16_t) iter->first;

    if (rnti >= SRSLTE_CRNTI_START && rnti <= SRSLTE_CRNTI_END && ue_db[rnti].has_grant_tti != (int) tti_rx) {
      // Check if user needs to receive PUCCH
      bool needs_pucch = false, needs_ack[SRSLTE_MAX_TB] = {false}, needs_sr = false, needs_cqi = false;
      uint32_t last_n_pdcch = 0;
      bzero(&uci_data, sizeof(srslte_uci_data_t));

      if (ue_db[rnti].I_sr_en) {
        if (srslte_ue_ul_sr_send_tti(ue_db[rnti].I_sr, tti_rx)) {
          needs_pucch = true;
          needs_sr = true;
          uci_data.scheduling_request = true;
        }
      }

      for (uint32_t tb = 0; tb < SRSLTE_MAX_TB; tb++) {
        needs_ack[tb] = phy->ue_db_is_ack_pending(t_rx, rnti, tb, &last_n_pdcch);
        if (needs_ack[tb]) {
          needs_pucch = true;
          uci_data.uci_ack_len++;
        }
      }
      srslte_cqi_value_t cqi_value;
      ZERO_OBJECT(cqi_value);

      LIBLTE_RRC_PHYSICAL_CONFIG_DEDICATED_STRUCT *dedicated = &ue_db[rnti].dedicated;
      LIBLTE_RRC_TRANSMISSION_MODE_ENUM tx_mode = dedicated->antenna_info_explicit_value.tx_mode;

      if (ue_db[rnti].cqi_en && (ue_db[rnti].pucch_cqi_ack || !needs_ack[0] || !needs_ack[1])) {
        if (ue_db[rnti].ri_en && srslte_ri_send(ue_db[rnti].pmi_idx, ue_db[rnti].ri_idx, tti_rx)) {
          needs_pucch = true;
          uci_data.uci_ri_len = 1;
          uci_data.ri_periodic_report = true;
        } else if (srslte_cqi_send(ue_db[rnti].pmi_idx, tti_rx)) {
          needs_pucch = true;
          needs_cqi = true;
          cqi_value.type = SRSLTE_CQI_TYPE_WIDEBAND;
          if (tx_mode == LIBLTE_RRC_TRANSMISSION_MODE_4) {
            cqi_value.wideband.pmi_present = true;
            cqi_value.wideband.rank_is_not_one = phy->ue_db_get_ri(rnti) > 0;
          }
          uci_data.uci_cqi_len = (uint32_t) srslte_cqi_size(&cqi_value);
        }
      }

      if (needs_pucch) {
        if (srslte_enb_ul_get_pucch(&enb_ul, rnti, last_n_pdcch, sf_rx, &uci_data)) {
          fprintf(stderr, "Error getting PUCCH\n");
          return SRSLTE_ERROR;
        }
        /* If only one ACK is required, it can be for TB0 or TB1 */
        uint32_t ack_idx = 0;
        for (uint32_t tb = 0; tb < SRSLTE_MAX_TB; tb++) {
          if (needs_ack[tb]) {
            bool ack = ((ack_idx++ == 0) ? uci_data.uci_ack : uci_data.uci_ack_2);
            bool valid = srslte_pucch_get_last_corr(&enb_ul.pucch) >= PUCCH_RL_CORR_TH;
            phy->mac->ack_info(tti_rx, rnti, tb, ack && valid);
          }
        }
        if (uci_data.scheduling_request) {
          phy->mac->sr_detected(tti_rx, rnti);
        }
        
        char cqi_ri_str[64] = {0};
        if (srslte_pucch_get_last_corr(&enb_ul.pucch) > PUCCH_RL_CORR_TH) {
          if (uci_data.ri_periodic_report) {
            phy->mac->ri_info(tti_rx, rnti, uci_data.uci_ri);
            phy->ue_db_set_ri(rnti, uci_data.uci_ri);
            sprintf(cqi_ri_str, ", ri=%d", uci_data.uci_ri);
          } else if (uci_data.uci_cqi_len && needs_cqi) {
            srslte_cqi_value_unpack(uci_data.uci_cqi, &cqi_value);
            phy->mac->cqi_info(tti_rx, rnti, cqi_value.wideband.wideband_cqi);
            sprintf(cqi_ri_str, ", cqi=%d", cqi_value.wideband.wideband_cqi);

            if (cqi_value.type == SRSLTE_CQI_TYPE_WIDEBAND && cqi_value.wideband.pmi_present) {
              phy->mac->pmi_info(tti_rx, rnti, cqi_value.wideband.pmi);
              sprintf(cqi_ri_str, "%s, pmi=%d", cqi_ri_str, cqi_value.wideband.pmi);
            }
          }
        }
        log_h->info("PUCCH: rnti=0x%x, corr=%.2f, n_pucch=%d, n_prb=%d%s%s%s%s\n",
                    rnti,
                    srslte_pucch_get_last_corr(&enb_ul.pucch),
                    enb_ul.pucch.last_n_pucch, enb_ul.pucch.last_n_prb,
                    (uci_data.uci_ack_len)?(uci_data.uci_ack?", ack=1":", ack=0"):"",
                    (uci_data.uci_ack_len > 1)?(uci_data.uci_ack_2?"1":"0"):"",
                    needs_sr?(uci_data.scheduling_request?", sr=yes":", sr=no"):"",
                    (needs_cqi || uci_data.ri_periodic_report)?cqi_ri_str:"");


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


int phch_worker::encode_phich(srslte_enb_dl_phich_t *acks, uint32_t nof_acks)
{
  for (uint32_t i=0;i<nof_acks;i++) {
    uint16_t rnti = acks[i].rnti;
    if (rnti) {
      srslte_enb_dl_put_phich(&enb_dl, acks[i].ack,
                              ue_db[rnti].phich_info.n_prb_lowest,
                              ue_db[rnti].phich_info.n_dmrs,
                              sf_tx);

      Info("PHICH: rnti=0x%x, hi=%d, I_lowest=%d, n_dmrs=%d, tti_tx_dl=%d\n",
          rnti, acks[i].ack,
          ue_db[rnti].phich_info.n_prb_lowest,
          ue_db[rnti].phich_info.n_dmrs, tti_tx_dl);
    }
  }
  return SRSLTE_SUCCESS;
}


int phch_worker::encode_pdcch_ul(srslte_enb_ul_pusch_t *grants, uint32_t nof_grants)
{
  for (uint32_t i=0;i<nof_grants;i++) {
    uint16_t rnti = grants[i].rnti;
    if (grants[i].needs_pdcch && rnti) {
      if (srslte_enb_dl_put_pdcch_ul(&enb_dl, &grants[i].grant, grants[i].location, rnti, sf_tx)) {
        fprintf(stderr, "Error putting PUSCH %d\n",i);
        return SRSLTE_ERROR;
      }

      Info("PDCCH: UL DCI Format0  rnti=0x%x, cce_index=%d, L=%d, tpc=%d, tti_tx_dl=%d\n",
           rnti, grants[i].location.ncce, (1<<grants[i].location.L), grants[i].grant.tpc_pusch, tti_tx_dl);
    }
  }
  return SRSLTE_SUCCESS;
}

int phch_worker::encode_pdcch_dl(srslte_enb_dl_pdsch_t *grants, uint32_t nof_grants)
{
  for (uint32_t i=0;i<nof_grants;i++) {
    srslte_enb_dl_pdsch_t *grant = &grants[i];
    uint16_t rnti = grant->rnti;
    if (rnti) {
      if (srslte_enb_dl_put_pdcch_dl(&enb_dl, &grants[i].grant, grant->dci_format, grants[i].location, rnti, sf_tx)) {
        fprintf(stderr, "Error putting PDCCH %d\n",i);
        return SRSLTE_ERROR;
      }

      if (LOG_THIS(rnti)) {
        Info("PDCCH: DL DCI %s rnti=0x%x, cce_index=%d, L=%d, tti_tx_dl=%d\n", srslte_dci_format_string(grant->dci_format),
          rnti, grants[i].location.ncce, (1<<grants[i].location.L), tti_tx_dl);
      }
    }
  }
  return 0;
}


int phch_worker::encode_pmch(srslte_enb_dl_pdsch_t *grant, srslte_ra_dl_grant_t *phy_grant)
{
  
  phy_grant->tb_en[0] = true;
  phy_grant->tb_en[1] = false;
  phy_grant->nof_prb = enb_dl.cell.nof_prb;
  phy_grant->sf_type = SRSLTE_SF_MBSFN;
  srslte_dl_fill_ra_mcs(&phy_grant->mcs[0], enb_dl.cell.nof_prb);
  phy_grant->Qm[0] = srslte_mod_bits_x_symbol(phy_grant->mcs[0].mod);
  for(int i = 0; i < 2; i++){
    for(uint32_t j = 0; j < phy_grant->nof_prb; j++){
      phy_grant->prb_idx[i][j] = true;
    }
  }
  srslte_enb_dl_put_pmch(&enb_dl, phy_grant, &temp_mbsfn_softbuffer, sf_tx, &grant->data[0][0]);
  return SRSLTE_SUCCESS;
}

int phch_worker::encode_pdsch(srslte_enb_dl_pdsch_t *grants, uint32_t nof_grants) {

  /* Scales the Resources Elements affected by the power allocation (p_b) */
  srslte_enb_dl_prepare_power_allocation(&enb_dl);

  for (uint32_t i = 0; i < nof_grants; i++) {
    uint16_t rnti = grants[i].rnti;
    if (rnti) {

      bool rnti_is_user = true;
      if (rnti == SRSLTE_SIRNTI || rnti == SRSLTE_PRNTI || rnti == SRSLTE_MRNTI) {
        rnti_is_user = false;
      }
      /* Mimo type (tx scheme) shall be single or tx diversity by default */
      srslte_mimo_type_t mimo_type = (enb_dl.cell.nof_ports == 1) ? SRSLTE_MIMO_TYPE_SINGLE_ANTENNA
                                                                  : SRSLTE_MIMO_TYPE_TX_DIVERSITY;
      srslte_ra_dl_grant_t phy_grant;
      srslte_ra_dl_dci_to_grant(&grants[i].grant, enb_dl.cell.nof_prb, rnti, &phy_grant);

      char grant_str[64];
      switch (grants[i].grant.alloc_type) {
        case SRSLTE_RA_ALLOC_TYPE0:
          sprintf(grant_str, "mask=0x%x", grants[i].grant.type0_alloc.rbg_bitmask);
          break;
        case SRSLTE_RA_ALLOC_TYPE1:
          sprintf(grant_str, "mask=0x%x", grants[i].grant.type1_alloc.vrb_bitmask);
          break;
        default:
          sprintf(grant_str, "rb_start=%d", grants[i].grant.type2_alloc.RB_start);
          break;
      }


      srslte_dci_format_t dci_format = grants[i].dci_format;
      switch (dci_format) {
        case SRSLTE_DCI_FORMAT1:
        case SRSLTE_DCI_FORMAT1A:
          /* Do nothing, it keeps default */
          break;
        case SRSLTE_DCI_FORMAT2A:
          if (SRSLTE_RA_DL_GRANT_NOF_TB(&phy_grant) == 1) {
            mimo_type = SRSLTE_MIMO_TYPE_TX_DIVERSITY;
          } else if (SRSLTE_RA_DL_GRANT_NOF_TB(&phy_grant) == 2) {
            mimo_type = SRSLTE_MIMO_TYPE_CDD;
          }
          break;
        case SRSLTE_DCI_FORMAT2:
          if (SRSLTE_RA_DL_GRANT_NOF_TB(&phy_grant) == 1) {
            if (phy_grant.pinfo == 0) {
              mimo_type = SRSLTE_MIMO_TYPE_TX_DIVERSITY;
            } else {
              mimo_type = SRSLTE_MIMO_TYPE_SPATIAL_MULTIPLEX;
            }
          } else if (SRSLTE_RA_DL_GRANT_NOF_TB(&phy_grant) == 2) {
            mimo_type = SRSLTE_MIMO_TYPE_SPATIAL_MULTIPLEX;
          }
          break;
        case SRSLTE_DCI_FORMAT0:
        case SRSLTE_DCI_FORMAT1C:
        case SRSLTE_DCI_FORMAT1B:
        case SRSLTE_DCI_FORMAT1D:
        case SRSLTE_DCI_FORMAT2B:
        default:
          Error("Not implemented/Undefined DCI format (%d)\n", dci_format);
      }

      if (LOG_THIS(rnti)) {
        uint8_t x = 0;
        uint8_t *ptr = grants[i].data[0];
        uint32_t len = phy_grant.mcs[0].tbs / (uint32_t) 8;
        if (!ptr) {
          ptr = &x;
          len = 1;
        }
        char pinfo_str[16] = {0};
        if (dci_format == SRSLTE_DCI_FORMAT2) {
          snprintf(pinfo_str, 15, ", pinfo=%x", phy_grant.pinfo);
        }
        char tbstr[SRSLTE_MAX_TB][128];
        for (int tb = 0; tb < SRSLTE_MAX_TB; tb++) {
          if (phy_grant.tb_en[tb]) {
            snprintf(tbstr[tb], 128, ", TB%d: tbs=%d, mcs=%d, rv=%d",
                     tb,
                     phy_grant.mcs[tb].tbs / 8,
                     phy_grant.mcs[tb].idx,
                     (tb == 0) ? grants[i].grant.rv_idx : grants[i].grant.rv_idx_1);
          } else {
            tbstr[tb][0] = '\0';
          }
        }
        log_h->info_hex(ptr, len,
                        "PDSCH: rnti=0x%x, l_crb=%2d, %s, harq=%d, tti_tx_dl=%d, tx_scheme=%s%s%s%s\n",
                        rnti, phy_grant.nof_prb, grant_str, grants[i].grant.harq_process, tti_tx_dl,
                        srslte_mimotype2str(mimo_type), pinfo_str, tbstr[0], tbstr[1]);
      }

      int rv[SRSLTE_MAX_CODEWORDS] = {grants[i].grant.rv_idx, grants[i].grant.rv_idx_1};

      /* Set power allocation */
      float rho_a = ((enb_dl.cell.nof_ports == 1) ? 1.0f : sqrtf(2.0f)), rho_b = 1.0f;
      uint32_t pdsch_cnfg_ded = ue_db[rnti].dedicated.pdsch_cnfg_ded;
      if (pdsch_cnfg_ded < (uint32_t) LIBLTE_RRC_PDSCH_CONFIG_P_A_N_ITEMS) {
        float rho_a_db = liblte_rrc_pdsch_config_p_a_num[pdsch_cnfg_ded];
        rho_a *= powf(10.0f, rho_a_db / 20.0f);
      }
      if (phy->pdsch_p_b < 4) {
        uint32_t idx0 = (phy->cell.nof_ports == 1) ? 0 : 1;
        float cell_specific_ratio = pdsch_cfg_cell_specific_ratio_table[idx0][phy->pdsch_p_b];
        rho_b = sqrtf(cell_specific_ratio);
      }
      srslte_enb_dl_set_power_allocation(&enb_dl, rho_a, rho_b);
      if (srslte_enb_dl_put_pdsch(&enb_dl, &phy_grant, grants[i].softbuffers, rnti, rv, sf_tx, grants[i].data, mimo_type)) {
        fprintf(stderr, "Error putting PDSCH %d\n", i);
        return SRSLTE_ERROR;
      }

      // Save metrics stats
      ue_db[rnti].metrics_dl(phy_grant.mcs[0].idx);
    }
  }

  srslte_enb_dl_apply_power_allocation(&enb_dl);

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

int phch_worker::read_ce_arg(float *ce_arg) {
  uint32_t i=0;
  int sz = srslte_symbol_sz(phy->cell.nof_prb);
  bzero(ce_arg, sizeof(float)*sz);
  int g = (sz - 12*phy->cell.nof_prb)/2;
  for (i = 0; i < 12*phy->cell.nof_prb; i++) {
    ce_arg[g+i] = cargf(enb_ul.ce[i]) * 180.0f / (float) M_PI;
    if (isinf(ce_arg[g+i])) {
      ce_arg[g+i] = -80;
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

int phch_worker::read_pucch_d(cf_t* pdsch_d)
{
  int nof_re = SRSLTE_PUCCH_MAX_BITS/2;//enb_ul.pusch_cfg.nbits.nof_re
  memcpy(pdsch_d, enb_ul.pucch.z_tmp, nof_re*sizeof(cf_t));
  return nof_re;
}


}


/***********************************************************
 * 
 * PLOT TO VISUALIZE THE CHANNEL RESPONSEE 
 * 
 ***********************************************************/


#ifdef ENABLE_GUI
plot_real_t    pce, pce_arg;
plot_scatter_t pconst;
plot_scatter_t pconst2;
#define SCATTER_PUSCH_BUFFER_LEN   (20*6*SRSLTE_SF_LEN_RE(SRSLTE_MAX_PRB, SRSLTE_CP_NORM))
#define SCATTER_PUSCH_PLOT_LEN    4000
float tmp_plot[SCATTER_PUSCH_BUFFER_LEN];
float tmp_plot_arg[SCATTER_PUSCH_BUFFER_LEN];
cf_t  tmp_plot2[SRSLTE_SF_LEN_RE(SRSLTE_MAX_PRB, SRSLTE_CP_NORM)];
cf_t  tmp_pucch_plot[SRSLTE_PUCCH_MAX_BITS/2];

void *plot_thread_run(void *arg) {
  srsenb::phch_worker *worker = (srsenb::phch_worker*) arg; 
  
  sdrgui_init_title("srsENB");  
  plot_real_init(&pce);
  plot_real_setTitle(&pce, (char*) "Channel Response - Magnitude");
  plot_real_setLabels(&pce, (char*) "Index", (char*) "dB");
  plot_real_setYAxisScale(&pce, -40, 40);

  plot_real_init(&pce_arg);
  plot_real_setTitle(&pce_arg, (char*) "Channel Response - Argument");
  plot_real_setLabels(&pce_arg, (char*) "Angle", (char*) "deg");
  plot_real_setYAxisScale(&pce_arg, -180, 180);
  
  plot_scatter_init(&pconst);
  plot_scatter_setTitle(&pconst, (char*) "PUSCH - Equalized Symbols");
  plot_scatter_setXAxisScale(&pconst, -4, 4);
  plot_scatter_setYAxisScale(&pconst, -4, 4);

  plot_scatter_init(&pconst2);
  plot_scatter_setTitle(&pconst2, (char*) "PUCCH - Equalized Symbols");
  plot_scatter_setXAxisScale(&pconst2, -4, 4);
  plot_scatter_setYAxisScale(&pconst2, -4, 4);
  
  plot_real_addToWindowGrid(&pce, (char*)"srsenb", 0, 0);
  plot_real_addToWindowGrid(&pce_arg, (char*)"srsenb", 1, 0);
  plot_scatter_addToWindowGrid(&pconst, (char*)"srsenb", 0, 1);
  plot_scatter_addToWindowGrid(&pconst2, (char*)"srsenb", 1, 1);

  int n, n_arg, n_pucch;
  int readed_pusch_re=0; 
  while(1) {
    sem_wait(&plot_sem);    
    
    n = worker->read_pusch_d(tmp_plot2);
    n_pucch = worker->read_pucch_d(tmp_pucch_plot);
    plot_scatter_setNewData(&pconst, tmp_plot2, n);
    plot_scatter_setNewData(&pconst2, tmp_pucch_plot, n_pucch);

    n = worker->read_ce_abs(tmp_plot);
    plot_real_setNewData(&pce, tmp_plot, n);

    n_arg = worker->read_ce_arg(tmp_plot_arg);
    plot_real_setNewData(&pce_arg, tmp_plot_arg, n_arg);
    
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




