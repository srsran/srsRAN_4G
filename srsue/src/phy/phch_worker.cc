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

#include <unistd.h>
#include <string.h>
#include "srsue/hdr/phy/phch_worker.h"
#include "srslte/srslte.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "srslte/asn1/liblte_rrc.h"

#define Error(fmt, ...)   if (SRSLTE_DEBUG_ENABLED) log_h->error(fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) if (SRSLTE_DEBUG_ENABLED) log_h->warning(fmt, ##__VA_ARGS__)
#define Info(fmt, ...)    if (SRSLTE_DEBUG_ENABLED) log_h->info(fmt, ##__VA_ARGS__)
#define Debug(fmt, ...)   if (SRSLTE_DEBUG_ENABLED) log_h->debug(fmt, ##__VA_ARGS__)


/* This is to visualize the channel response */
#ifdef ENABLE_GUI
#include "srsgui/srsgui.h"
#include <semaphore.h>

void init_plots(srsue::phch_worker *worker);
pthread_t plot_thread; 
sem_t plot_sem; 
static int plot_worker_id = -1;
#else
#warning Compiling without srsGUI support
#endif
/*********************************************/


namespace srsue {


phch_worker::phch_worker() : tr_exec(10240)
{
  phy = NULL;
  chest_loop = NULL;

  bzero(signal_buffer, sizeof(cf_t*)*SRSLTE_MAX_PORTS);

  mem_initiated   = false;
  cell_initiated  = false; 
  pregen_enabled  = false; 
  trace_enabled   = false;
  reset();
}


phch_worker::~phch_worker()
{
  if (mem_initiated) {
    for (uint32_t i=0;i<phy->args->nof_rx_ant;i++) {
      if (signal_buffer[i]) {
        free(signal_buffer[i]);
      }
    }
    srslte_ue_dl_free(&ue_dl);
    srslte_ue_ul_free(&ue_ul);
    mem_initiated = false;
  }
}

void phch_worker::reset()
{
  bzero(&dl_metrics, sizeof(dl_metrics_t));
  bzero(&ul_metrics, sizeof(ul_metrics_t));
  bzero(&dmrs_cfg, sizeof(srslte_refsignal_dmrs_pusch_cfg_t));    
  bzero(&pusch_hopping, sizeof(srslte_pusch_hopping_cfg_t));
  bzero(&uci_cfg, sizeof(srslte_uci_cfg_t));
  bzero(&pucch_cfg, sizeof(srslte_pucch_cfg_t));
  bzero(&pucch_sched, sizeof(srslte_pucch_sched_t));
  bzero(&srs_cfg, sizeof(srslte_refsignal_srs_cfg_t));
  bzero(&period_cqi, sizeof(srslte_cqi_periodic_cfg_t));
  sr_configured   = false;
  rnti_is_set     = false; 
  rar_cqi_request = false;
  I_sr = 0;
  cfi  = 0;
  rssi_read_cnt = 0;
}

void phch_worker::set_common(phch_common* phy_)
{
  phy = phy_;   
}

bool phch_worker::init(uint32_t max_prb, srslte::log *log_h, srslte::log *log_phy_lib_h , chest_feedback_itf *chest_loop)
{
  this->log_h = log_h;
  this->log_phy_lib_h =  log_phy_lib_h;
  this->chest_loop = chest_loop;

  // ue_sync in phy.cc requires a buffer for 3 subframes
  for (uint32_t i=0;i<phy->args->nof_rx_ant;i++) {
    signal_buffer[i] = (cf_t*) srslte_vec_malloc(3 * sizeof(cf_t) * SRSLTE_SF_LEN_PRB(max_prb));
    if (!signal_buffer[i]) {
      Error("Allocating memory\n");
      return false;
    }
  }

  if (srslte_ue_dl_init(&ue_dl, signal_buffer, max_prb, phy->args->nof_rx_ant)) {
    Error("Initiating UE DL\n");
    return false;
  }

  if (srslte_ue_ul_init(&ue_ul, signal_buffer[0], max_prb)) {
    Error("Initiating UE UL\n");
    return false;
  }

  srslte_chest_dl_average_subframe(&ue_dl.chest, phy->args->average_subframe_enabled);
  srslte_chest_dl_cfo_estimate_enable(&ue_dl.chest, phy->args->cfo_ref_mask!=0, phy->args->cfo_ref_mask);
  srslte_ue_ul_set_normalization(&ue_ul, true);
  srslte_ue_ul_set_cfo_enable(&ue_ul, true);
  srslte_pdsch_enable_csi(&ue_dl.pdsch, phy->args->pdsch_csi_enabled);

  mem_initiated = true;

  pthread_mutex_init(&mutex, NULL);
  return true;
}

bool phch_worker::set_cell(srslte_cell_t cell_)
{
  bool ret = false;
  pthread_mutex_lock(&mutex);
  if (cell.id != cell_.id || !cell_initiated) {
    memcpy(&cell, &cell_, sizeof(srslte_cell_t));

    if (srslte_ue_dl_set_cell(&ue_dl, cell)) {
      Error("Initiating UE DL\n");
      goto unlock;
    }

    if (srslte_ue_ul_set_cell(&ue_ul, cell)) {
      Error("Initiating UE UL\n");
      goto unlock;
    }
    srslte_ue_ul_set_normalization(&ue_ul, true);
    srslte_ue_ul_set_cfo_enable(&ue_ul, true);

    cell_initiated = true;
  }
  ret = true;
unlock:
  pthread_mutex_unlock(&mutex);
  return ret;
}

cf_t* phch_worker::get_buffer(uint32_t antenna_idx)
{
  return signal_buffer[antenna_idx]; 
}

void phch_worker::set_tti(uint32_t tti_, uint32_t tx_tti_)
{
  tti    = tti_; 
  tx_tti = tx_tti_;
  log_h->step(tti);
  log_phy_lib_h->step(tti);
}

void phch_worker::set_cfo(float cfo_)
{
  cfo = cfo_;
}

void phch_worker::set_crnti(uint16_t rnti)
{
  srslte_ue_dl_set_rnti(&ue_dl, rnti);
  srslte_ue_ul_set_rnti(&ue_ul, rnti);
  rnti_is_set = true;
}

float phch_worker::get_ref_cfo()
{
  return srslte_chest_dl_get_cfo(&ue_dl.chest);
}

float phch_worker::get_snr()
{
  return 10*log10(srslte_chest_dl_get_snr(&ue_dl.chest));
}

float phch_worker::get_rsrp()
{
  return 10*log10(srslte_chest_dl_get_rsrp(&ue_dl.chest));
}

float phch_worker::get_noise()
{
  return 10*log10(srslte_chest_dl_get_noise_estimate(&ue_dl.chest));
}


float phch_worker::get_cfo()
{
  return cfo;
}

void phch_worker::work_imp()
{
  if (!cell_initiated) {
    return; 
  }

  pthread_mutex_lock(&mutex);

  Debug("TTI %d running\n", tti);

#ifdef LOG_EXECTIME
  gettimeofday(&logtime_start[1], NULL);
#endif

  tr_log_start();
  
  reset_uci();

  bool dl_grant_available = false; 
  bool ul_grant_available = false;
  bool dl_ack[SRSLTE_MAX_CODEWORDS] = {false};

  mac_interface_phy::mac_grant_t    dl_mac_grant = {};
  mac_interface_phy::tb_action_dl_t dl_action = {};

  mac_interface_phy::mac_grant_t    ul_mac_grant = {};
  mac_interface_phy::tb_action_ul_t ul_action = {};


  /** Calculate RSSI on the input signal before generating the output */

  // Average RSSI over all symbols (make sure SF length is non-zero)
  float rssi_dbm = SRSLTE_SF_LEN_PRB(cell.nof_prb) > 0 ? (10*log10(srslte_vec_avg_power_cf(signal_buffer[0], SRSLTE_SF_LEN_PRB(cell.nof_prb))) + 30) : 0;
  if (isnormal(rssi_dbm)) {
    phy->avg_rssi_dbm = SRSLTE_VEC_EMA(rssi_dbm, phy->avg_rssi_dbm, phy->args->snr_ema_coeff);
  }

  /* Do FFT and extract PDCCH LLR, or quit if no actions are required in this subframe */
  bool chest_ok = extract_fft_and_pdcch_llr();

  // Call feedback loop for chest
  if (chest_loop && ((1<<(tti%10)) & phy->args->cfo_ref_mask)) {
    chest_loop->set_cfo(srslte_chest_dl_get_cfo(&ue_dl.chest));
  }

  if (chest_ok) {

    /***** Downlink Processing *******/

    /* PDCCH DL + PDSCH */
    dl_grant_available = decode_pdcch_dl(&dl_mac_grant); 
    if(dl_grant_available) {
      /* Send grant to MAC and get action for this TB */
      phy->mac->new_grant_dl(dl_mac_grant, &dl_action);

      /* Set DL ACKs to default */
      for (uint32_t tb = 0; tb < SRSLTE_MAX_CODEWORDS; tb++) {
        dl_ack[tb] = dl_action.default_ack[tb];
      }

      /* Decode PDSCH if instructed to do so */
      if (dl_action.decode_enabled[0] || dl_action.decode_enabled[1]) {
        decode_pdsch(&dl_action.phy_grant.dl, dl_action.payload_ptr,
                      dl_action.softbuffers, dl_action.rv, dl_action.rnti,
                      dl_mac_grant.pid, dl_ack);
      }
      if (dl_action.generate_ack_callback) {
        for (uint32_t tb = 0; tb < SRSLTE_MAX_TB; tb++) {
          if (dl_action.decode_enabled[tb]) {
            phy->mac->tb_decoded(dl_ack[tb], tb, dl_mac_grant.rnti_type, dl_mac_grant.pid);
            dl_ack[tb] = dl_action.generate_ack_callback(dl_action.generate_ack_callback_arg);
            Debug("Calling generate ACK callback for TB %d returned=%d\n", tb, dl_ack[tb]);
          }
        }
      }
      Debug("dl_ack={%d, %d}, generate_ack=%d\n", dl_ack[0], dl_ack[1], dl_action.generate_ack);
      if (dl_action.generate_ack) {
        set_uci_ack(dl_ack, dl_mac_grant.tb_en);
      }
    }
  }

  // Process RAR before UL to enable zero-delay Msg3
  bool rar_delivered = false;
  if (HARQ_DELAY_MS == MSG3_DELAY_MS && dl_mac_grant.rnti_type == SRSLTE_RNTI_RAR) {
    rar_delivered = true;
    phy->mac->tb_decoded(dl_ack[0], 0, dl_mac_grant.rnti_type, dl_mac_grant.pid);
  }
  
  // Decode PHICH 
  bool ul_ack = false;
  bool ul_ack_available = decode_phich(&ul_ack);


  /***** Uplink Processing + Transmission *******/
  
  /* Generate SR if required*/
  set_uci_sr();

  /* Check if we have UL grant. ul_phy_grant will be overwritten by new grant */
  ul_grant_available = decode_pdcch_ul(&ul_mac_grant);

  /* Generate CQI reports if required, note that in case both aperiodic
      and periodic ones present, only aperiodic is sent (36.213 section 7.2) */
  if (ul_grant_available && ul_mac_grant.has_cqi_request) {
    set_uci_aperiodic_cqi();
  } else {
    set_uci_periodic_cqi();
  }

  /* TTI offset for UL */
  ul_action.tti_offset = HARQ_DELAY_MS;

  /* Send UL grant or HARQ information (from PHICH) to MAC */
  if (ul_grant_available         && ul_ack_available)  {
    phy->mac->new_grant_ul_ack(ul_mac_grant, ul_ack, &ul_action);      
  } else if (ul_grant_available  && !ul_ack_available) {
    phy->mac->new_grant_ul(ul_mac_grant, &ul_action);
  } else if (!ul_grant_available && ul_ack_available)  {    
    phy->mac->harq_recv(tti, ul_ack, &ul_action);        
  }

  /* Set UL CFO before transmission */  
  srslte_ue_ul_set_cfo(&ue_ul, cfo);

  /* Transmit PUSCH, PUCCH or SRS */
  bool signal_ready = false; 
  if (ul_action.tx_enabled) {
    encode_pusch(&ul_action.phy_grant.ul, ul_action.payload_ptr[0], ul_action.current_tx_nb,
                 &ul_action.softbuffers[0], ul_action.rv[0], ul_action.rnti, ul_mac_grant.is_from_rar);
    signal_ready = true; 
    if (ul_action.expect_ack) {
      phy->set_pending_ack(TTI_RX_ACK(tti), ue_ul.pusch_cfg.grant.n_prb_tilde[0], ul_action.phy_grant.ul.ncs_dmrs);
    }

  } else if (dl_action.generate_ack || uci_data.scheduling_request || uci_data.uci_cqi_len > 0 || uci_data.uci_ri_len > 0) {
    encode_pucch();
    signal_ready = true; 
  } else if (srs_is_ready_to_send()) {
    encode_srs();
    signal_ready = true; 
  } 

  tr_log_end();

  if (next_offset > 0) {
    phy->worker_end(tx_tti, signal_ready, signal_buffer[0], SRSLTE_SF_LEN_PRB(cell.nof_prb)+next_offset, tx_time);
  } else {
    phy->worker_end(tx_tti, signal_ready, &signal_buffer[0][-next_offset], SRSLTE_SF_LEN_PRB(cell.nof_prb)+next_offset, tx_time);
  }

  if (!dl_action.generate_ack_callback) {
    if (dl_mac_grant.rnti_type == SRSLTE_RNTI_PCH && dl_action.decode_enabled[0]) {
      if (dl_ack[0]) {
        phy->mac->pch_decoded_ok(dl_mac_grant.n_bytes[0]);
      }
    } else if (!rar_delivered) {
      for (uint32_t tb = 0; tb < SRSLTE_MAX_TB; tb++) {
        if (dl_action.decode_enabled[tb]) {
          phy->mac->tb_decoded(dl_ack[tb], tb, dl_mac_grant.rnti_type, dl_mac_grant.pid);
        }
      }
    }
  }

  update_measurements();

  if (chest_ok) {
    if (phy->avg_rsrp_dbm > -130.0 && phy->avg_snr_db > -10.0) {
      log_h->debug("SNR=%.1f dB, RSRP=%.1f dBm sync=in-sync from channel estimator\n",
                   phy->avg_snr_db, phy->avg_rsrp_dbm);
      chest_loop->in_sync();
    } else {
      log_h->warning("SNR=%.1f dB RSRP=%.1f dBm, sync=out-of-sync from channel estimator\n",
                     phy->avg_snr_db, phy->avg_rsrp_dbm);
      chest_loop->out_of_sync();
    }
  }

  pthread_mutex_unlock(&mutex);

  /* Tell the plotting thread to draw the plots */
#ifdef ENABLE_GUI
  if ((int) get_id() == plot_worker_id) {
    sem_post(&plot_sem);    
  }
#endif
}

void phch_worker::compute_ri(uint8_t *ri, uint8_t *pmi, float *sinr) {
  if (phy->config->dedicated.antenna_info_explicit_value.tx_mode == LIBLTE_RRC_TRANSMISSION_MODE_3) {
    if (ue_dl.nof_rx_antennas > 1) {
      /* If 2 ort more receiving antennas, select RI */
      float cn = 0.0f;
      srslte_ue_dl_ri_select(&ue_dl, ri, &cn);
      if (ri) {
        Debug("TM3 RI select %d layers, κ=%fdB\n", (*ri) + 1, cn);
      }
    } else {
      /* If only one receiving antenna, force RI for 1 layer */
      if (ri) {
        *ri = 0;
      }
    }
    uci_data.uci_ri_len = 1;
  } else if (phy->config->dedicated.antenna_info_explicit_value.tx_mode == LIBLTE_RRC_TRANSMISSION_MODE_4) {
    srslte_ue_dl_ri_pmi_select(&ue_dl, ri, pmi, sinr);
    Debug("TM4 ri=%d; pmi=%d; SINR=%.1fdB\n", ue_dl.ri, ue_dl.pmi[ue_dl.ri], 10*log10f(ue_dl.sinr[ue_dl.ri][ue_dl.pmi[ue_dl.ri]]));
  }
}

bool phch_worker::extract_fft_and_pdcch_llr() {
  bool decode_pdcch = true;

  /* Without a grant, we might need to do fft processing if need to decode PHICH */
  if (phy->get_pending_ack(tti) || decode_pdcch) {
    
    // Setup estimator filter 
    float w_coeff = phy->args->estimator_fil_w; 
    if (w_coeff > 0.0) {
      srslte_chest_dl_set_smooth_filter3_coeff(&ue_dl.chest, w_coeff); 
    } else if (w_coeff == 0.0) {
      srslte_chest_dl_set_smooth_filter(&ue_dl.chest, NULL, 0); 
    }
    
    if (!phy->args->snr_estim_alg.compare("refs")) {
      srslte_chest_dl_set_noise_alg(&ue_dl.chest, SRSLTE_NOISE_ALG_REFS);
    } else if (!phy->args->snr_estim_alg.compare("empty")) {
      srslte_chest_dl_set_noise_alg(&ue_dl.chest, SRSLTE_NOISE_ALG_EMPTY);
    } else {
      srslte_chest_dl_set_noise_alg(&ue_dl.chest, SRSLTE_NOISE_ALG_PSS);      
    }
  
    if (srslte_ue_dl_decode_fft_estimate(&ue_dl, tti%10, &cfi) < 0) {
      Error("Getting PDCCH FFT estimate\n");
      return false; 
    }        
    chest_done = true; 
  } else {
    chest_done = false; 
  }
  if (chest_done && decode_pdcch) { /* and not in DRX mode */
    
    float noise_estimate = phy->avg_noise;
    
    if (!phy->args->equalizer_mode.compare("zf")) {
      noise_estimate = 0; 
    }

    if (srslte_pdcch_extract_llr_multi(&ue_dl.pdcch, ue_dl.sf_symbols_m, ue_dl.ce_m, noise_estimate, tti%10, cfi)) {
      Error("Extracting PDCCH LLR\n");
      return false; 
    }
  }
  return (decode_pdcch || phy->get_pending_ack(tti));
}
  








/********************* Downlink processing functions ****************************/

bool phch_worker::decode_pdcch_dl(srsue::mac_interface_phy::mac_grant_t* grant)
{
  char timestr[64];
  timestr[0]='\0';

  dl_rnti = phy->get_dl_rnti(tti); 
  if (dl_rnti) {
    
    srslte_rnti_type_t type = phy->get_dl_rnti_type();

    srslte_dci_msg_t dci_msg; 
    srslte_ra_dl_dci_t dci_unpacked;

    if (type == SRSLTE_RNTI_RAR) {
      Info("Looking for RNTI=0x%x\n", dl_rnti);
    }

    if (srslte_ue_dl_find_dl_dci_type(&ue_dl, phy->config->dedicated.antenna_info_explicit_value.tx_mode, cfi, tti%10,
                                      dl_rnti, type, &dci_msg) != 1) {
      if (type == SRSLTE_RNTI_RAR) {
        Info("RAR not found, SNR=%.1f dB, tti=%d, cfi=%d, tx_mode=%d, cell_id=%d\n",
             10*log10(srslte_chest_dl_get_snr(&ue_dl.chest)), tti, cfi,
             phy->config->dedicated.antenna_info_explicit_value.tx_mode, cell.id);
      }
      return false;
    }
    
    if (srslte_dci_msg_to_dl_grant(&dci_msg, dl_rnti, cell.nof_prb, cell.nof_ports, &dci_unpacked, &grant->phy_grant.dl)) {
      Error("Converting DCI message to DL grant\n");
      return false;   
    }

    grant->pid = ASYNC_DL_SCHED?dci_unpacked.harq_process:(UL_PIDOF(TTI_TX(tti)));

    // Set last TBS for this TB (pid) in case of mcs>28 (7.1.7.2 of 36.213)
    for (int i=0;i<SRSLTE_MAX_CODEWORDS;i++) {
      if (grant->phy_grant.dl.mcs[i].idx > 28) {
        grant->phy_grant.dl.mcs[i].tbs = phy->last_dl_tbs[grant->pid][i];
      }
      if(grant->phy_grant.dl.mcs[i].tbs < 0) {
        Info("Invalid TBS size for PDSCH grant\n");
        grant->phy_grant.dl.mcs[i].tbs = 0;
      }
      // save it
      phy->last_dl_tbs[grant->pid][i] = grant->phy_grant.dl.mcs[i].tbs;
    }

    /* Fill MAC grant structure */
    grant->ndi[0] = dci_unpacked.ndi;
    grant->ndi[1] = dci_unpacked.ndi_1;
    grant->n_bytes[0] = grant->phy_grant.dl.mcs[0].tbs / (uint32_t) 8;
    grant->n_bytes[1] = grant->phy_grant.dl.mcs[1].tbs / (uint32_t) 8;
    grant->tti = tti;
    grant->rv[0] = dci_unpacked.rv_idx;
    grant->rv[1] = dci_unpacked.rv_idx_1;
    grant->rnti = dl_rnti;
    grant->rnti_type = type;
    grant->last_tti = 0;
    grant->tb_en[0] = dci_unpacked.tb_en[0];
    grant->tb_en[1] = dci_unpacked.tb_en[1];
    grant->tb_cw_swap = dci_unpacked.tb_cw_swap; // FIXME: tb_cw_swap not supported

    last_dl_pdcch_ncce = srslte_ue_dl_get_ncce(&ue_dl);

    char hexstr[512];
    hexstr[0]='\0';
    if (log_h->get_level() >= srslte::LOG_LEVEL_INFO) {
      srslte_vec_sprint_hex(hexstr, sizeof(hexstr), dci_msg.data, dci_msg.nof_bits);
    }
    Info("PDCCH: DL DCI %s cce_index=%2d, L=%d, n_data_bits=%d, hex=%s\n", srslte_dci_format_string(dci_msg.format), 
         last_dl_pdcch_ncce, (1<<ue_dl.last_location.L), dci_msg.nof_bits, hexstr);
    
    return true; 
  } else {
    return false; 
  }
}

int phch_worker::decode_pdsch(srslte_ra_dl_grant_t *grant, uint8_t *payload[SRSLTE_MAX_CODEWORDS],
                                     srslte_softbuffer_rx_t *softbuffers[SRSLTE_MAX_CODEWORDS],
                                     int rv[SRSLTE_MAX_CODEWORDS],
                                     uint16_t rnti, uint32_t harq_pid, bool acks[SRSLTE_MAX_CODEWORDS]) {
  char timestr[64];
  char commonstr[128];
  char tbstr[2][128];
  bool valid_config = true;
  timestr[0]='\0';
  srslte_mimo_type_t mimo_type = SRSLTE_MIMO_TYPE_SINGLE_ANTENNA;
  int ret = SRSLTE_SUCCESS;

  for (uint32_t tb = 0; tb < SRSLTE_MAX_CODEWORDS; tb++) {
    if (grant->tb_en[tb] && (rv[tb] < 0 || rv[tb] > 3)) {
      valid_config = false;
      Error("Wrong RV (%d) for TB index %d\n", rv[tb], tb);
    }
  }

  uint32_t nof_tb = SRSLTE_RA_DL_GRANT_NOF_TB(grant);
  switch(phy->config->dedicated.antenna_info_explicit_value.tx_mode) {
    /* Implemented Tx Modes */
    case LIBLTE_RRC_TRANSMISSION_MODE_1:
      mimo_type = SRSLTE_MIMO_TYPE_SINGLE_ANTENNA;
      if (nof_tb != 1) {
        Error("Wrong number of transport blocks (%d) for single antenna.", nof_tb);
        valid_config = false;
      }
      break;
    case LIBLTE_RRC_TRANSMISSION_MODE_2:
      if (cell.nof_ports > 1) {
        mimo_type = SRSLTE_MIMO_TYPE_TX_DIVERSITY;
      } else {
        mimo_type = SRSLTE_MIMO_TYPE_SINGLE_ANTENNA;
      }
      if (nof_tb != 1) {
        Error("Wrong number of transport blocks (%d) for transmit diversity.", nof_tb);
        valid_config = false; 
      }
      break;
    case LIBLTE_RRC_TRANSMISSION_MODE_3:
      if (nof_tb == 1) {
        mimo_type = SRSLTE_MIMO_TYPE_TX_DIVERSITY;
      } else if (ue_dl.nof_rx_antennas > 1 && nof_tb == 2) {
        mimo_type = SRSLTE_MIMO_TYPE_CDD;
      } else {
        Error("Wrong combination of antennas (%d) or transport blocks (%d) for TM3\n", ue_dl.nof_rx_antennas,
              nof_tb);
        valid_config = false;
      }
      break;
    case LIBLTE_RRC_TRANSMISSION_MODE_4:
      if (nof_tb == 1) {
        mimo_type = (grant->pinfo == 0) ? SRSLTE_MIMO_TYPE_TX_DIVERSITY : SRSLTE_MIMO_TYPE_SPATIAL_MULTIPLEX;
      } else if (ue_dl.nof_rx_antennas > 1 && nof_tb == 2) {
        mimo_type = SRSLTE_MIMO_TYPE_SPATIAL_MULTIPLEX;
      } else {
        Error("Wrong combination of antennas (%d) or transport blocks (%d) for TM3\n", ue_dl.nof_rx_antennas,
              nof_tb);
        valid_config = false;
      }
    break;

    /* Not implemented cases */
    case LIBLTE_RRC_TRANSMISSION_MODE_5:
    case LIBLTE_RRC_TRANSMISSION_MODE_6:
    case LIBLTE_RRC_TRANSMISSION_MODE_7:
    case LIBLTE_RRC_TRANSMISSION_MODE_8:
      Error("Not implemented Tx mode (%d)\n", phy->config->dedicated.antenna_info_explicit_value.tx_mode);
      break;

    /* Error cases */
    case LIBLTE_RRC_TRANSMISSION_MODE_N_ITEMS:
    default:
      Error("Wrong Tx mode (%d)\n", phy->config->dedicated.antenna_info_explicit_value.tx_mode);
      valid_config = false;
  }

  /* Set power allocation according to 3GPP 36.213 clause 5.2 Downlink power allocation */
  float rho_a = 1.0f, rho_b = 1.0f;
  if (phy->config->dedicated.pdsch_cnfg_ded < LIBLTE_RRC_PDSCH_CONFIG_P_A_N_ITEMS) {
    float rho_a_db = liblte_rrc_pdsch_config_p_a_num[(int) phy->config->dedicated.pdsch_cnfg_ded];
    rho_a = powf(10.0f, rho_a_db / 20.0f) * ((cell.nof_ports == 1) ? 1.0f : sqrtf(2.0f));
  }
  if (phy->config->common.pdsch_cnfg.p_b < 4) {
    uint32_t idx0 = (cell.nof_ports == 1) ? 0 : 1;
    float cell_specific_ratio = pdsch_cfg_cell_specific_ratio_table[idx0][phy->config->common.pdsch_cnfg.p_b];
    rho_b = sqrtf(cell_specific_ratio);
  }
  srslte_ue_dl_set_power_alloc(&ue_dl, rho_a, rho_b);

  Debug("DL Buffer TTI %d: Decoding PDSCH\n", tti);

  /* Setup PDSCH configuration for this CFI, SFIDX and RVIDX */
  if (valid_config) {
    if (!srslte_ue_dl_cfg_grant(&ue_dl, grant, cfi, tti%10, rv, mimo_type)) {
      if ((ue_dl.pdsch_cfg.grant.mcs[0].mod > 0 && ue_dl.pdsch_cfg.grant.mcs[0].tbs >= 0) ||
          (ue_dl.pdsch_cfg.grant.mcs[1].mod > 0 && ue_dl.pdsch_cfg.grant.mcs[1].tbs >= 0)) {
        
        float noise_estimate = srslte_chest_dl_get_noise_estimate(&ue_dl.chest);
        
        if (!phy->args->equalizer_mode.compare("zf")) {
          noise_estimate = 0; 
        }
        
        /* Set decoder iterations */
        if (phy->args->pdsch_max_its > 0) {
          srslte_pdsch_set_max_noi(&ue_dl.pdsch, phy->args->pdsch_max_its);
        }


  #ifdef LOG_EXECTIME
        struct timeval t[3];
        gettimeofday(&t[1], NULL);
  #endif
        ret = srslte_pdsch_decode(&ue_dl.pdsch, &ue_dl.pdsch_cfg, softbuffers, ue_dl.sf_symbols_m,
                                  ue_dl.ce_m, noise_estimate, rnti, payload, acks);
        if (ret) {
          Error("ERROR: Decoding PDSCH\n");
        }
  #ifdef LOG_EXECTIME
        gettimeofday(&t[2], NULL);
        get_time_interval(t);
        snprintf(timestr, 64, ", dec_time=%4d us", (int) t[0].tv_usec);
  #endif

        char pinfo_str[16] = {0};
        if (phy->config->dedicated.antenna_info_explicit_value.tx_mode == LIBLTE_RRC_TRANSMISSION_MODE_4) {
          snprintf(pinfo_str, 15, ", pinfo=%x", grant->pinfo);
        }

        snprintf(commonstr, 128, "PDSCH: l_crb=%2d, harq=%d, snr=%.1f dB, tx_scheme=%s%s", grant->nof_prb, harq_pid,
                 10 * log10(srslte_chest_dl_get_snr(&ue_dl.chest)), srslte_mimotype2str(mimo_type), pinfo_str);

        for (int i=0;i<SRSLTE_MAX_CODEWORDS;i++) {
          if (grant->tb_en[i]) {
            snprintf(tbstr[i], 128, ", CW%d: tbs=%d, mcs=%d, rv=%d, crc=%s, it=%d",
                     i, grant->mcs[i].tbs/8, grant->mcs[i].idx, rv[i], acks[i] ? "OK" : "KO",
                     srslte_pdsch_last_noi_cw(&ue_dl.pdsch, i));
          }
        }

        Info("%s%s%s%s\n", commonstr, grant->tb_en[0]?tbstr[0]:"", grant->tb_en[1]?tbstr[1]:"", timestr);

        // Store metrics
        dl_metrics.mcs    = grant->mcs[0].idx;
      } else {
        Warning("Received grant for TBS=0\n");
      }
    } else {
      Error("Error configuring DL grant\n");
      ret = SRSLTE_ERROR;
    }
  } else {
    Error("Error invalid DL config\n");
    ret = SRSLTE_ERROR;
  }
  return ret;
}

bool phch_worker::decode_phich(bool *ack)
{
  uint32_t I_lowest, n_dmrs; 
  if (phy->get_pending_ack(tti, &I_lowest, &n_dmrs)) {
    if (ack) {
      *ack = srslte_ue_dl_decode_phich(&ue_dl, tti%10, I_lowest, n_dmrs);     
      Info("PHICH: hi=%d, I_lowest=%d, n_dmrs=%d\n", *ack, I_lowest, n_dmrs);
    }
    phy->reset_pending_ack(tti);
    return true; 
  } else {
    return false; 
  }
}




/********************* Uplink processing functions ****************************/

bool phch_worker::decode_pdcch_ul(mac_interface_phy::mac_grant_t* grant)
{
  char timestr[64];
  timestr[0]='\0';

  phy->reset_pending_ack(TTI_RX_ACK(tti));

  srslte_dci_msg_t dci_msg; 
  srslte_ra_ul_dci_t dci_unpacked;
  srslte_dci_rar_grant_t rar_grant;
  srslte_rnti_type_t type = phy->get_ul_rnti_type();
  
  bool ret = false; 
  if (phy->get_pending_rar(tti, &rar_grant)) {

    if (srslte_dci_rar_to_ul_grant(&rar_grant, cell.nof_prb, pusch_hopping.hopping_offset, 
      &dci_unpacked, &grant->phy_grant.ul)) 
    {
      Error("Converting RAR message to UL grant\n");
      return false; 
    }
    grant->rnti_type = SRSLTE_RNTI_TEMP;
    grant->is_from_rar = true; 
    grant->has_cqi_request = false; // In contention-based Random Access CQI request bit is reserved
    Debug("RAR grant found for TTI=%d\n", tti);
    ret = true;  
  } else {
    ul_rnti = phy->get_ul_rnti(tti);
    if (ul_rnti) {
      if (srslte_ue_dl_find_ul_dci(&ue_dl, cfi, tti%10, ul_rnti, &dci_msg) != 1) {
        return false;
      }
      
      if (srslte_dci_msg_to_ul_grant(&dci_msg, cell.nof_prb, pusch_hopping.hopping_offset, 
        &dci_unpacked, &grant->phy_grant.ul, tti)) 
      {
        Error("Converting DCI message to UL grant\n");
        return false;   
      }
      grant->rnti_type = type; 
      grant->is_from_rar = false;
      grant->has_cqi_request = dci_unpacked.cqi_request;
      ret = true; 
      
      char hexstr[512];
      hexstr[0]='\0';
      if (log_h->get_level() >= srslte::LOG_LEVEL_INFO) {
        srslte_vec_sprint_hex(hexstr, sizeof(hexstr), dci_msg.data, dci_msg.nof_bits);
      }
      // Change to last_location_ul
      Info("PDCCH: UL DCI Format0  cce_index=%d, L=%d, n_data_bits=%d, hex=%s\n", 
           ue_dl.last_location_ul.ncce, (1<<ue_dl.last_location_ul.L), dci_msg.nof_bits, hexstr);
      
      if (grant->phy_grant.ul.mcs.tbs==0) {
        Info("Received PUSCH grant with empty data\n");
      }
    }
  }


  // Handle Format0 adaptive retx
  if (ret) {
    // Use last TBS for this TB in case of mcs>28
    if (grant->phy_grant.ul.mcs.idx > 28 && grant->phy_grant.ul.mcs.mod == SRSLTE_MOD_LAST) {
      // Make sure we received a grant in the previous TTI for this PID
      grant->phy_grant.ul.mcs.tbs = phy->last_ul_tbs[UL_PIDOF(TTI_TX(tti))];
      grant->phy_grant.ul.mcs.mod = phy->last_ul_mod[UL_PIDOF(TTI_TX(tti))];
      grant->phy_grant.ul.Qm      = srslte_mod_bits_x_symbol(grant->phy_grant.ul.mcs.mod);
    }
  }
  if (ret) {
    phy->last_ul_tbs[UL_PIDOF(TTI_TX(tti))] = grant->phy_grant.ul.mcs.tbs;
    phy->last_ul_mod[UL_PIDOF(TTI_TX(tti))] = grant->phy_grant.ul.mcs.mod;
    phy->last_ul_tti[UL_PIDOF(TTI_TX(tti))] = TTI_RX_ACK(tti);
    /* Limit UL modulation if not supported by the UE or disabled by higher layers */
    if (!phy->config->enable_64qam) {
      if (grant->phy_grant.ul.mcs.mod >= SRSLTE_MOD_64QAM) {
        grant->phy_grant.ul.mcs.mod = SRSLTE_MOD_16QAM;
        grant->phy_grant.ul.Qm      = 4;
      }
    }
  }

  /* Make sure the grant is valid */
  if (ret && !srslte_dft_precoding_valid_prb(grant->phy_grant.ul.L_prb) && grant->phy_grant.ul.L_prb <= cell.nof_prb) {
    Warning("Received invalid UL grant. L=%d\n", grant->phy_grant.ul.L_prb);
    ret = false; 
  }
  
  if (ret) {    
    grant->ndi[0] = dci_unpacked.ndi;
    grant->pid = 0; // This is computed by MAC from TTI 
    grant->n_bytes[0] = grant->phy_grant.ul.mcs.tbs / (uint32_t) 8;
    grant->tti = tti; 
    grant->rnti = ul_rnti; 
    grant->rv[0] = dci_unpacked.rv_idx;
    if (SRSLTE_VERBOSE_ISINFO()) {
      srslte_ra_pusch_fprint(stdout, &dci_unpacked, cell.nof_prb);
    }
  }

  return ret;
}

void phch_worker::reset_uci()
{
  bzero(&uci_data, sizeof(srslte_uci_data_t));
}

void phch_worker::set_uci_ack(bool ack[SRSLTE_MAX_CODEWORDS], bool tb_en[SRSLTE_MAX_CODEWORDS])
{
  /* Map ACK according to 3GPP 36.212 clause 5.2.3.1 */
  uint32_t nof_ack = 0;
  for (uint32_t tb = 0; tb < SRSLTE_MAX_CODEWORDS; tb++) {
    if (tb_en[tb]) {
      ((nof_ack == 0)?uci_data.uci_ack:uci_data.uci_ack_2) = (uint8_t)(ack[tb]?1:0);
      nof_ack++;
    }
  }
  uci_data.uci_ack_len = nof_ack;
}

void phch_worker::set_uci_sr()
{
  uci_data.scheduling_request = false; 
  if (phy->sr_enabled && sr_configured) {
    uint32_t sr_tx_tti = TTI_TX(tti);
    // Get I_sr parameter   
    if (srslte_ue_ul_sr_send_tti(I_sr, sr_tx_tti)) {
      Info("PUCCH: SR transmission at TTI=%d, I_sr=%d\n", sr_tx_tti, I_sr);
      uci_data.scheduling_request = true; 
      phy->sr_last_tx_tti = sr_tx_tti; 
      phy->sr_enabled = false;
    }
  } 
}

void phch_worker::set_uci_periodic_cqi()
{
  int cqi_fixed     = phy->args->cqi_fixed;
  int cqi_max       = phy->args->cqi_max;

  float sinr = ue_dl.sinr[phy->last_ri & SRSLTE_MAX_LAYERS][phy->last_pmi % SRSLTE_MAX_CODEBOOKS];

  if (period_cqi.configured && rnti_is_set) {
    if (period_cqi.ri_idx_present && srslte_ri_send(period_cqi.pmi_idx, period_cqi.ri_idx, TTI_TX(tti))) {
      /* Compute RI, PMI and SINR */
      compute_ri(&phy->last_ri, &phy->last_pmi, &sinr);
      uci_data.uci_ri = phy->last_ri;
      uci_data.uci_ri_len = 1;
      uci_data.ri_periodic_report = true;
      Debug("PUCCH: Periodic ri=%d, SINR=%.1f\n", phy->last_ri, sinr);
    } else if (srslte_cqi_send(period_cqi.pmi_idx, TTI_TX(tti))) {
      compute_ri(NULL, NULL, NULL);
      phy->last_pmi = (uint8_t) ue_dl.pmi[phy->last_ri % SRSLTE_MAX_LAYERS];

      srslte_cqi_value_t cqi_report = {0};
      if (period_cqi.format_is_subband) {
        // TODO: Implement subband periodic reports
        cqi_report.type = SRSLTE_CQI_TYPE_SUBBAND;
        cqi_report.subband.subband_cqi = srslte_cqi_from_snr(phy->avg_snr_db);
        cqi_report.subband.subband_label = 0;
        log_h->console("Warning: Subband CQI periodic reports not implemented\n");
        Debug("PUCCH: Periodic CQI=%d, SNR=%.1f dB\n", cqi_report.subband.subband_cqi, phy->avg_snr_db);
      } else {
        cqi_report.type = SRSLTE_CQI_TYPE_WIDEBAND;
        if (cqi_fixed >= 0) {
          cqi_report.wideband.wideband_cqi = cqi_fixed;
        } else {
          cqi_report.wideband.wideband_cqi = srslte_cqi_from_snr(phy->avg_snr_db);      
        }
        if (cqi_max >= 0 && cqi_report.wideband.wideband_cqi > cqi_max) {
          cqi_report.wideband.wideband_cqi = cqi_max; 
        }
        if (phy->config->dedicated.antenna_info_explicit_value.tx_mode == LIBLTE_RRC_TRANSMISSION_MODE_4) {
          cqi_report.wideband.pmi_present = true;
          cqi_report.wideband.pmi = phy->last_pmi;
          cqi_report.wideband.rank_is_not_one = (phy->last_ri != 0);
        }
        Debug("PUCCH: Periodic CQI=%d, SNR=%.1f dB\n", cqi_report.wideband.wideband_cqi, phy->avg_snr_db);
      }
      uci_data.uci_cqi_len = (uint32_t) srslte_cqi_value_pack(&cqi_report, uci_data.uci_cqi);
      rar_cqi_request = false;       
    }
  }
}

void phch_worker::set_uci_aperiodic_cqi()
{
  float sinr_db = ue_dl.sinr[phy->last_ri % SRSLTE_MAX_LAYERS][phy->last_pmi%SRSLTE_MAX_CODEBOOKS];

  if (phy->config->dedicated.cqi_report_cnfg.report_mode_aperiodic_present) {
    /* Compute RI, PMI and SINR */
    compute_ri(&phy->last_ri, &phy->last_pmi, &sinr_db);

    switch(phy->config->dedicated.cqi_report_cnfg.report_mode_aperiodic) {
      case LIBLTE_RRC_CQI_REPORT_MODE_APERIODIC_RM30:
        /* only Higher Layer-configured subband feedback support right now, according to TS36.213 section 7.2.1
          - A UE shall report a wideband CQI value which is calculated assuming transmission on set S subbands
          - The UE shall also report one subband CQI value for each set S subband. The subband CQI
            value is calculated assuming transmission only in the subband
          - Both the wideband and subband CQI represent channel quality for the first codeword,
            even when RI>1
          - For transmission mode 3 the reported CQI values are calculated conditioned on the
            reported RI. For other transmission modes they are reported conditioned on rank 1.
        */
        if (rnti_is_set) {
          srslte_cqi_value_t cqi_report = {0};
          cqi_report.type = SRSLTE_CQI_TYPE_SUBBAND_HL;
          cqi_report.subband_hl.wideband_cqi_cw0 = srslte_cqi_from_snr(phy->avg_snr_db);

          // TODO: implement subband CQI properly
          cqi_report.subband_hl.subband_diff_cqi_cw0 = 0; // Always report zero offset on all subbands
          cqi_report.subband_hl.N = (cell.nof_prb > 7) ? (uint32_t) srslte_cqi_hl_get_no_subbands(cell.nof_prb) : 0;

          int cqi_len = srslte_cqi_value_pack(&cqi_report, uci_data.uci_cqi);
          if (cqi_len < 0) {
            Error("Error packing CQI value (Aperiodic reporting mode RM30).");
            return;
          }
          uci_data.uci_cqi_len = (uint32_t) cqi_len;

          char cqi_str[SRSLTE_CQI_STR_MAX_CHAR] = {0};
          srslte_cqi_to_str(uci_data.uci_cqi, uci_data.uci_cqi_len, cqi_str, SRSLTE_CQI_STR_MAX_CHAR);

          /* Set RI = 1 */
          if (phy->config->dedicated.antenna_info_explicit_value.tx_mode == LIBLTE_RRC_TRANSMISSION_MODE_3 ||
              phy->config->dedicated.antenna_info_explicit_value.tx_mode == LIBLTE_RRC_TRANSMISSION_MODE_4) {
            uci_data.uci_ri = phy->last_ri;
            uci_data.uci_ri_len = 1;
          } else {
            uci_data.uci_ri_len = 0;
          }

          Info("PUSCH: Aperiodic RM30 CQI=%s, %sSNR=%.1f dB, for %d subbands\n",
               cqi_str, (uci_data.uci_ri_len)?((uci_data.uci_ri == 0)?"ri=0, ":"ri=1, "):"", phy->avg_snr_db, cqi_report.subband_hl.N);
        }
        break;
      case LIBLTE_RRC_CQI_REPORT_MODE_APERIODIC_RM31:
        /* only Higher Layer-configured subband feedback support right now, according to TS36.213 section 7.2.1
          - A single precoding matrix is selected from the codebook subset assuming transmission on set S subbands
          - A UE shall report one subband CQI value per codeword for each set S subband which are calculated assuming
            the use of the single precoding matrix in all subbands and assuming transmission in the corresponding
            subband.
          - A UE shall report a wideband CQI value per codeword which is calculated assuming the use of the single
            precoding matrix in all subbands and transmission on set S subbands
          - The UE shall report the single selected precoding matrix indicator.
          - For transmission mode 4 the reported PMI and CQI values are calculated conditioned on the reported RI. For
            other transmission modes they are reported conditioned on rank 1.
        */
        if (rnti_is_set) {
          /* Fill CQI Report */
          srslte_cqi_value_t cqi_report = {0};
          cqi_report.type = SRSLTE_CQI_TYPE_SUBBAND_HL;

          cqi_report.subband_hl.wideband_cqi_cw0 = srslte_cqi_from_snr(sinr_db);
          cqi_report.subband_hl.subband_diff_cqi_cw0 = 0; // Always report zero offset on all subbands

          if (phy->last_ri > 0) {
            cqi_report.subband_hl.rank_is_not_one = true;
            cqi_report.subband_hl.wideband_cqi_cw1 = srslte_cqi_from_snr(sinr_db);
            cqi_report.subband_hl.subband_diff_cqi_cw1 = 0; // Always report zero offset on all subbands
          }

          cqi_report.subband_hl.pmi = phy->last_pmi;
          cqi_report.subband_hl.pmi_present = true;
          cqi_report.subband_hl.four_antenna_ports = (cell.nof_ports == 4);

          // TODO: implement subband CQI properly
          cqi_report.subband_hl.N = (uint32_t) ((cell.nof_prb > 7) ? srslte_cqi_hl_get_no_subbands(cell.nof_prb) : 0);

          int cqi_len = srslte_cqi_value_pack(&cqi_report, uci_data.uci_cqi);
          if (cqi_len < 0) {
            Error("Error packing CQI value (Aperiodic reporting mode RM31).");
            return;
          }
          uci_data.uci_cqi_len = (uint32_t) cqi_len;
          uci_data.uci_ri_len = 1;
          uci_data.uci_ri = phy->last_ri;

          char cqi_str[SRSLTE_CQI_STR_MAX_CHAR] = {0};
          srslte_cqi_to_str(uci_data.uci_cqi, uci_data.uci_cqi_len, cqi_str, SRSLTE_CQI_STR_MAX_CHAR);

          if (cqi_report.subband_hl.rank_is_not_one) {
            Info("PUSCH: Aperiodic RM31 ri~1, CQI=%02d/%02d, SINR=%2.1f/%2.1fdB, pmi=%d for %d subbands\n",
                 cqi_report.subband_hl.wideband_cqi_cw0, cqi_report.subband_hl.wideband_cqi_cw1,
                 sinr_db, sinr_db, phy->last_pmi, cqi_report.subband_hl.N);
          } else {
            Info("PUSCH: Aperiodic RM31 ri=1, CQI=%02d, SINR=%2.1f, pmi=%d for %d subbands\n",
                 cqi_report.subband_hl.wideband_cqi_cw0,
                 sinr_db, phy->last_pmi, cqi_report.subband_hl.N);
          }
        }
        break;
      default:
        Warning("Received CQI request but mode %s is not supported\n", 
                liblte_rrc_cqi_report_mode_aperiodic_text[phy->config->dedicated.cqi_report_cnfg.report_mode_aperiodic]);
        break;
    }
  } else {
    Warning("Received CQI request but aperiodic mode is not configured\n");    
  }
}

bool phch_worker::srs_is_ready_to_send() {
  if (srs_cfg.configured) {
    if (srslte_refsignal_srs_send_cs(srs_cfg.subframe_config, TTI_TX(tti)%10) == 1 &&
        srslte_refsignal_srs_send_ue(srs_cfg.I_srs, TTI_TX(tti))              == 1)
    {
      return true; 
    }
  }
  return false; 
}

void phch_worker::set_tx_time(srslte_timestamp_t _tx_time, uint32_t next_offset)
{
  this->next_offset = next_offset;
  memcpy(&tx_time, &_tx_time, sizeof(srslte_timestamp_t));
}

void phch_worker::encode_pusch(srslte_ra_ul_grant_t *grant, uint8_t *payload, uint32_t current_tx_nb, 
                               srslte_softbuffer_tx_t* softbuffer, uint32_t rv, uint16_t rnti, bool is_from_rar)
{
  char timestr[64];
  timestr[0]='\0';
  
  if (srslte_ue_ul_cfg_grant(&ue_ul, grant, TTI_TX(tti), rv, current_tx_nb)) {
    Error("Configuring UL grant\n");
  }

  if (srslte_ue_ul_pusch_encode_rnti_softbuffer(&ue_ul, 
                                                payload, uci_data, 
                                                softbuffer,
                                                rnti, 
                                                signal_buffer[0])) 
  {
    Error("Encoding PUSCH\n");
  }
    
  float p0_preamble = 0; 
  if (is_from_rar) {
    p0_preamble = phy->p0_preamble;
  }
  float tx_power = srslte_ue_ul_pusch_power(&ue_ul, phy->pathloss, p0_preamble);
  float gain = set_power(tx_power);

  // Save PUSCH power for PHR calculation  
  phy->cur_pusch_power = tx_power; 
  
#ifdef LOG_EXECTIME
  gettimeofday(&logtime_start[2], NULL);
  get_time_interval(logtime_start);
  snprintf(timestr, 64, ", tot_time=%4d us", (int) logtime_start[0].tv_usec);
#endif

  char cqi_str[SRSLTE_CQI_STR_MAX_CHAR] = "";
  srslte_cqi_to_str(uci_data.uci_cqi, uci_data.uci_cqi_len, cqi_str, SRSLTE_CQI_STR_MAX_CHAR);

  uint8_t dummy[2] = {0,0};
  log_h->info_hex(payload, grant->mcs.tbs/8,
              "PUSCH: tti_tx=%d, alloc=(%d,%d), tbs=%d, mcs=%d, rv=%d%s%s%s, cfo=%.1f KHz%s%s%s\n",
              (tti + HARQ_DELAY_MS) % 10240,
              grant->n_prb[0], grant->n_prb[0] + grant->L_prb,
              grant->mcs.tbs / 8, grant->mcs.idx, rv,
              uci_data.uci_ack_len > 0 ? (uci_data.uci_ack ? ", ack=1" : ", ack=0") : "",
              uci_data.uci_ack_len > 1 ? (uci_data.uci_ack_2 ? "1" : "0") : "",
              uci_data.uci_ri_len > 0 ? (uci_data.uci_ri ? ", ri=1" : ", ri=0") : "",
              cfo * 15, timestr,
              uci_data.uci_cqi_len > 0 ? ", cqi=" : "",
              uci_data.uci_cqi_len > 0 ? cqi_str : "");

  // Store metrics
  ul_metrics.mcs   = grant->mcs.idx;
  ul_metrics.power = tx_power;
  phy->set_ul_metrics(ul_metrics);
}

void phch_worker::encode_pucch()
{
  char timestr[64];
  timestr[0]='\0';

  if (uci_data.scheduling_request || uci_data.uci_ack_len > 0 || uci_data.uci_cqi_len > 0 || uci_data.uci_ri_len > 0)
  {
    
    // Drop CQI if there is collision with ACK 
    if (!period_cqi.simul_cqi_ack && uci_data.uci_ack_len > 0 && uci_data.uci_cqi_len > 0) {
      uci_data.uci_cqi_len = 0; 
    }

#ifdef LOG_EXECTIME
    struct timeval t[3];
    gettimeofday(&t[1], NULL);
#endif

    if (srslte_ue_ul_pucch_encode(&ue_ul, uci_data, last_dl_pdcch_ncce, TTI_TX(tti), signal_buffer[0])) {
      Error("Encoding PUCCH\n");
    }

#ifdef LOG_EXECTIME
  gettimeofday(&logtime_start[2], NULL);
  memcpy(&t[2], &logtime_start[2], sizeof(struct timeval));
  get_time_interval(logtime_start);
  get_time_interval(t);
  snprintf(timestr, 64, ", tot_time=%d us", (int) logtime_start[0].tv_usec);
#endif

  float tx_power = srslte_ue_ul_pucch_power(&ue_ul, phy->pathloss, ue_ul.last_pucch_format, uci_data.uci_cqi_len, uci_data.uci_ack_len);
  float gain = set_power(tx_power);  

    char str_cqi[SRSLTE_CQI_STR_MAX_CHAR] = "";
    srslte_cqi_to_str(uci_data.uci_cqi, uci_data.uci_cqi_len, str_cqi, SRSLTE_CQI_STR_MAX_CHAR);

    Info("PUCCH: tti_tx=%d, n_pucch=%d, n_prb=%d, ack=%s%s%s%s%s, sr=%s, cfo=%.1f KHz%s\n",
         (tti + 4) % 10240,
         ue_ul.pucch.last_n_pucch, ue_ul.pucch.last_n_prb,
         uci_data.uci_ack_len > 0 ? (uci_data.uci_ack ? "1" : "0") : "no",
         uci_data.uci_ack_len > 1 ? (uci_data.uci_ack_2 ? "1" : "0") : "",
         uci_data.uci_ri_len > 0 ? (uci_data.uci_ri ? ", ri=1" : ", ri=0") : "",
         uci_data.uci_cqi_len > 0 ? ", cqi=" : "",
         uci_data.uci_cqi_len > 0 ? str_cqi : "",
         uci_data.scheduling_request ? "yes" : "no",
         cfo * 15, timestr);
  }
  
  if (uci_data.scheduling_request) {
    phy->sr_enabled = false; 
  }
}

void phch_worker::encode_srs()
{
  char timestr[64];
  timestr[0]='\0';
  
  if (srslte_ue_ul_srs_encode(&ue_ul, TTI_TX(tti), signal_buffer[0]))
  {
    Error("Encoding SRS\n");
  }

#ifdef LOG_EXECTIME
  gettimeofday(&logtime_start[2], NULL);
  get_time_interval(logtime_start);
  snprintf(timestr, 64, ", tot_time=%4d us", (int) logtime_start[0].tv_usec);
#endif
  
  float tx_power = srslte_ue_ul_srs_power(&ue_ul, phy->pathloss);  
  float gain = set_power(tx_power);
  Info("SRS:   power=%.2f dBm, tti_tx=%d%s\n", tx_power, TTI_TX(tti), timestr);
}

void phch_worker::enable_pregen_signals(bool enabled)
{
  pregen_enabled = enabled; 
  if (enabled) {
    Info("Pre-generating UL signals worker=%d\n", get_id());
    srslte_ue_ul_pregen_signals(&ue_ul);
    Info("Done pre-generating signals worker=%d\n", get_id());
  }
}

void phch_worker::set_ul_params(bool pregen_disabled)
{

  phy_interface_rrc::phy_cfg_common_t         *common    = &phy->config->common;
  LIBLTE_RRC_PHYSICAL_CONFIG_DEDICATED_STRUCT *dedicated = &phy->config->dedicated;
  
  Info("Setting new params worker_id=%d, pregen_disabled=%d\n", get_id(), pregen_disabled);
  
  /* PUSCH DMRS signal configuration */
  bzero(&dmrs_cfg, sizeof(srslte_refsignal_dmrs_pusch_cfg_t));    
  dmrs_cfg.group_hopping_en    = common->pusch_cnfg.ul_rs.group_hopping_enabled;
  dmrs_cfg.sequence_hopping_en = common->pusch_cnfg.ul_rs.sequence_hopping_enabled;
  dmrs_cfg.cyclic_shift        = common->pusch_cnfg.ul_rs.cyclic_shift;
  dmrs_cfg.delta_ss            = common->pusch_cnfg.ul_rs.group_assignment_pusch;
  
  /* PUSCH Hopping configuration */
  bzero(&pusch_hopping, sizeof(srslte_pusch_hopping_cfg_t));
  pusch_hopping.n_sb           = common->pusch_cnfg.n_sb;
  pusch_hopping.hop_mode       = common->pusch_cnfg.hopping_mode == LIBLTE_RRC_HOPPING_MODE_INTRA_AND_INTER_SUBFRAME ? 
                                  pusch_hopping.SRSLTE_PUSCH_HOP_MODE_INTRA_SF : 
                                  pusch_hopping.SRSLTE_PUSCH_HOP_MODE_INTER_SF; 
  pusch_hopping.hopping_offset = common->pusch_cnfg.pusch_hopping_offset;

  /* PUSCH UCI configuration */
  bzero(&uci_cfg, sizeof(srslte_uci_cfg_t));
  uci_cfg.I_offset_ack         = dedicated->pusch_cnfg_ded.beta_offset_ack_idx;
  uci_cfg.I_offset_cqi         = dedicated->pusch_cnfg_ded.beta_offset_cqi_idx;
  uci_cfg.I_offset_ri          = dedicated->pusch_cnfg_ded.beta_offset_ri_idx;
  
  /* PUCCH configuration */  
  bzero(&pucch_cfg, sizeof(srslte_pucch_cfg_t));
  pucch_cfg.delta_pucch_shift  = liblte_rrc_delta_pucch_shift_num[common->pucch_cnfg.delta_pucch_shift%LIBLTE_RRC_DELTA_PUCCH_SHIFT_N_ITEMS];
  pucch_cfg.N_cs               = common->pucch_cnfg.n_cs_an;
  pucch_cfg.n_rb_2             = common->pucch_cnfg.n_rb_cqi;
  pucch_cfg.srs_configured     = dedicated->srs_ul_cnfg_ded.setup_present;
  if (pucch_cfg.srs_configured) {
    pucch_cfg.srs_cs_subf_cfg    = liblte_rrc_srs_subfr_config_num[common->srs_ul_cnfg.subfr_cnfg%LIBLTE_RRC_SRS_SUBFR_CONFIG_N_ITEMS];
    pucch_cfg.srs_simul_ack      = common->srs_ul_cnfg.ack_nack_simul_tx;
  }
  
  /* PUCCH Scheduling configuration */
  bzero(&pucch_sched, sizeof(srslte_pucch_sched_t));
  pucch_sched.n_pucch_1[0]     = 0; // TODO: n_pucch_1 for SPS
  pucch_sched.n_pucch_1[1]     = 0;
  pucch_sched.n_pucch_1[2]     = 0;
  pucch_sched.n_pucch_1[3]     = 0;
  pucch_sched.N_pucch_1        = common->pucch_cnfg.n1_pucch_an;
  pucch_sched.n_pucch_2        = dedicated->cqi_report_cnfg.report_periodic.pucch_resource_idx;
  pucch_sched.n_pucch_sr       = dedicated->sched_request_cnfg.sr_pucch_resource_idx;

  /* SRS Configuration */
  bzero(&srs_cfg, sizeof(srslte_refsignal_srs_cfg_t));
  srs_cfg.configured           = dedicated->srs_ul_cnfg_ded.setup_present;
  if (pucch_cfg.srs_configured) {
    srs_cfg.subframe_config      = liblte_rrc_srs_subfr_config_num[common->srs_ul_cnfg.subfr_cnfg%LIBLTE_RRC_SRS_SUBFR_CONFIG_N_ITEMS];
    srs_cfg.bw_cfg               = liblte_rrc_srs_bw_config_num[common->srs_ul_cnfg.bw_cnfg%LIBLTE_RRC_SRS_BW_CONFIG_N_ITEMS];
    srs_cfg.I_srs                = dedicated->srs_ul_cnfg_ded.srs_cnfg_idx;
    srs_cfg.B                    = dedicated->srs_ul_cnfg_ded.srs_bandwidth;
    srs_cfg.b_hop                = dedicated->srs_ul_cnfg_ded.srs_hopping_bandwidth;
    srs_cfg.n_rrc                = dedicated->srs_ul_cnfg_ded.freq_domain_pos;
    srs_cfg.k_tc                 = dedicated->srs_ul_cnfg_ded.tx_comb;
    srs_cfg.n_srs                = dedicated->srs_ul_cnfg_ded.cyclic_shift;
  }
  
  /* UL power control configuration */
  bzero(&power_ctrl, sizeof(srslte_ue_ul_powerctrl_t));
  power_ctrl.p0_nominal_pusch  = common->ul_pwr_ctrl.p0_nominal_pusch;
  power_ctrl.alpha             = liblte_rrc_ul_power_control_alpha_num[common->ul_pwr_ctrl.alpha%LIBLTE_RRC_UL_POWER_CONTROL_ALPHA_N_ITEMS];
  power_ctrl.p0_nominal_pucch  = common->ul_pwr_ctrl.p0_nominal_pucch;
  power_ctrl.delta_f_pucch[0]  = liblte_rrc_delta_f_pucch_format_1_num[common->ul_pwr_ctrl.delta_flist_pucch.format_1%LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_1_N_ITEMS];
  power_ctrl.delta_f_pucch[1]  = liblte_rrc_delta_f_pucch_format_1b_num[common->ul_pwr_ctrl.delta_flist_pucch.format_1b%LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_1B_N_ITEMS];
  power_ctrl.delta_f_pucch[2]  = liblte_rrc_delta_f_pucch_format_2_num[common->ul_pwr_ctrl.delta_flist_pucch.format_2%LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_2_N_ITEMS];
  power_ctrl.delta_f_pucch[3]  = liblte_rrc_delta_f_pucch_format_2a_num[common->ul_pwr_ctrl.delta_flist_pucch.format_2a%LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_2A_N_ITEMS];
  power_ctrl.delta_f_pucch[4]  = liblte_rrc_delta_f_pucch_format_2b_num[common->ul_pwr_ctrl.delta_flist_pucch.format_2b%LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_2B_N_ITEMS];
  
  power_ctrl.delta_preamble_msg3 = common->ul_pwr_ctrl.delta_preamble_msg3;
  
  power_ctrl.p0_ue_pusch       = dedicated->ul_pwr_ctrl_ded.p0_ue_pusch;
  power_ctrl.delta_mcs_based   = dedicated->ul_pwr_ctrl_ded.delta_mcs_en==LIBLTE_RRC_DELTA_MCS_ENABLED_EN0;
  power_ctrl.acc_enabled       = dedicated->ul_pwr_ctrl_ded.accumulation_en;
  power_ctrl.p0_ue_pucch       = dedicated->ul_pwr_ctrl_ded.p0_ue_pucch;
  power_ctrl.p_srs_offset      = dedicated->ul_pwr_ctrl_ded.p_srs_offset;
  
  srslte_ue_ul_set_cfg(&ue_ul, &dmrs_cfg, &srs_cfg, &pucch_cfg, &pucch_sched, &uci_cfg, &pusch_hopping, &power_ctrl);

  /* CQI configuration */
  bzero(&period_cqi, sizeof(srslte_cqi_periodic_cfg_t));
  period_cqi.configured        = dedicated->cqi_report_cnfg.report_periodic_setup_present;
  period_cqi.pmi_idx           = dedicated->cqi_report_cnfg.report_periodic.pmi_cnfg_idx;
  period_cqi.simul_cqi_ack     = dedicated->cqi_report_cnfg.report_periodic.simult_ack_nack_and_cqi;
  period_cqi.format_is_subband = dedicated->cqi_report_cnfg.report_periodic.format_ind_periodic ==
                                 LIBLTE_RRC_CQI_FORMAT_INDICATOR_PERIODIC_SUBBAND_CQI;
  period_cqi.subband_size      = dedicated->cqi_report_cnfg.report_periodic.format_ind_periodic_subband_k;

  if (dedicated->cqi_report_cnfg.report_periodic.ri_cnfg_idx_present) {
    period_cqi.ri_idx = dedicated->cqi_report_cnfg.report_periodic.ri_cnfg_idx;
    period_cqi.ri_idx_present = true;
  } else {
    period_cqi.ri_idx_present = false;
  }

  /* SR configuration */
  I_sr                         = dedicated->sched_request_cnfg.sr_cnfg_idx;
  sr_configured                = true;
  
  if (pregen_enabled && !pregen_disabled) { 
    Info("Pre-generating UL signals worker=%d\n", get_id());
    srslte_ue_ul_pregen_signals(&ue_ul);
    Info("Done pre-generating signals worker=%d\n", get_id());
  } 
}

float phch_worker::set_power(float tx_power) {
  float gain = 0; 
  /* Check if UL power control is enabled */
  if(phy->args->ul_pwr_ctrl_en) {    
    /* Adjust maximum power if it changes significantly */
    if (tx_power < phy->cur_radio_power - 5 || tx_power > phy->cur_radio_power + 5) {
      phy->cur_radio_power = tx_power; 
      float radio_tx_power = phy->cur_radio_power;
      gain = phy->get_radio()->set_tx_power(radio_tx_power);  
   }    
  }
  return gain;
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

int phch_worker::read_ce_abs(float *ce_abs, uint32_t tx_antenna, uint32_t rx_antenna) {
  uint32_t i=0;
  int sz = srslte_symbol_sz(cell.nof_prb);
  bzero(ce_abs, sizeof(float)*sz);
  int g = (sz - 12*cell.nof_prb)/2;
  for (i = 0; i < 12*cell.nof_prb; i++) {
    ce_abs[g+i] = 20 * log10f(cabsf(ue_dl.ce_m[tx_antenna][rx_antenna][i]));
    if (isinf(ce_abs[g+i])) {
      ce_abs[g+i] = -80;
    }
  }
  return sz;
}

int phch_worker::read_pdsch_d(cf_t* pdsch_d)
{

  memcpy(pdsch_d, ue_dl.pdsch.d[0], ue_dl.pdsch_cfg.nbits[0].nof_re*sizeof(cf_t));
  return ue_dl.pdsch_cfg.nbits[0].nof_re;
}



/**************************** Measurements **************************/

void phch_worker::update_measurements() 
{
  float snr_ema_coeff = phy->args->snr_ema_coeff;
  if (chest_done) {

    /* Only worker 0 reads the RSSI sensor every ~1-nof_cores s */
    if (get_id() == 0) {
      if (!rssi_read_cnt) {
        if (phy->get_radio()->has_rssi() && phy->args->rssi_sensor_enabled) {
          phy->last_radio_rssi = phy->get_radio()->get_rssi();
          phy->rx_gain_offset = phy->avg_rssi_dbm - phy->last_radio_rssi + 30;
        } else {
          phy->rx_gain_offset = phy->get_radio()->get_rx_gain() + phy->args->rx_gain_offset;
        }
      }
      rssi_read_cnt++;
      if (rssi_read_cnt == 1000) {
        rssi_read_cnt = 0;
      }
    }
    
    // Average RSRQ
    float rsrq_db = 10*log10(srslte_chest_dl_get_rsrq(&ue_dl.chest));
    if (isnormal(rsrq_db)) {
      if (!phy->avg_rsrq_db) {
        phy->avg_rsrq_db = SRSLTE_VEC_EMA(rsrq_db, phy->avg_rsrq_db, snr_ema_coeff);
      } else {
        phy->avg_rsrq_db = rsrq_db;
      }
    }

    // Average RSRP
    float rsrp_lin = srslte_chest_dl_get_rsrp(&ue_dl.chest);
    if (isnormal(rsrp_lin)) {
      if (!phy->avg_rsrp) {
        phy->avg_rsrp = SRSLTE_VEC_EMA(rsrp_lin, phy->avg_rsrp, snr_ema_coeff);
      } else {
        phy->avg_rsrp = rsrp_lin;
      }
    }
    
    /* Correct absolute power measurements by RX gain offset */
    float rsrp_dbm = 10*log10(rsrp_lin) + 30 - phy->rx_gain_offset;

    // Serving cell measurements are averaged over DEFAULT_MEAS_PERIOD_MS then sent to RRC
    if (isnormal(rsrp_dbm)) {
      if (!phy->avg_rsrp_dbm) {
        phy->avg_rsrp_dbm = rsrp_dbm;
      } else {
        phy->avg_rsrp_dbm = SRSLTE_VEC_EMA(rsrp_dbm, phy->avg_rsrp_dbm, snr_ema_coeff);
      }
      if ((tti%phy->pcell_report_period) == 0 && phy->pcell_meas_enabled) {
        phy->rrc->new_phy_meas(phy->avg_rsrp_dbm, phy->avg_rsrq_db, tti);
      }
    }

    // Compute PL
    float tx_crs_power = phy->config->common.pdsch_cnfg.rs_power;
    phy->pathloss = tx_crs_power - phy->avg_rsrp_dbm;

    // Average noise 
    float cur_noise = srslte_chest_dl_get_noise_estimate(&ue_dl.chest);
    if (isnormal(cur_noise)) {
      if (!phy->avg_noise) {  
        phy->avg_noise = cur_noise;          
      } else {
        phy->avg_noise = SRSLTE_VEC_EMA(cur_noise, phy->avg_noise, snr_ema_coeff);
      }
    }
    
    // Compute SNR
    phy->avg_snr_db = 10*log10(phy->avg_rsrp/phy->avg_noise);
    
    // Store metrics
    dl_metrics.n      = phy->avg_noise;
    dl_metrics.rsrp   = phy->avg_rsrp_dbm;
    dl_metrics.rsrq   = phy->avg_rsrq_db;
    dl_metrics.rssi   = phy->avg_rssi_dbm;
    dl_metrics.pathloss = phy->pathloss;
    dl_metrics.sinr   = phy->avg_snr_db;
    dl_metrics.turbo_iters = srslte_pdsch_last_noi(&ue_dl.pdsch);
    phy->set_dl_metrics(dl_metrics);
    
  }
}


/********** Execution time trace function ************/

void phch_worker::start_trace() {
  trace_enabled = true; 
}

void phch_worker::write_trace(std::string filename) {
  tr_exec.writeToBinary(filename + ".exec");
}

void phch_worker::tr_log_start()
{
  if (trace_enabled) {
    gettimeofday(&tr_time[1], NULL);
  }
}

void phch_worker::tr_log_end()
{
  if (trace_enabled) {
    gettimeofday(&tr_time[2], NULL);
    get_time_interval(tr_time);
    tr_exec.push(tti, tr_time[0].tv_usec);
  }
}

}








/***********************************************************
 * 
 * PLOT TO VISUALIZE THE CHANNEL RESPONSEE 
 * 
 ***********************************************************/


#ifdef ENABLE_GUI
plot_real_t    pce[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS];
plot_scatter_t pconst;
#define SCATTER_PDSCH_BUFFER_LEN   (20*6*SRSLTE_SF_LEN_RE(SRSLTE_MAX_PRB, SRSLTE_CP_NORM))
#define SCATTER_PDSCH_PLOT_LEN    4000
float tmp_plot[SCATTER_PDSCH_BUFFER_LEN];
cf_t  tmp_plot2[SRSLTE_SF_LEN_RE(SRSLTE_MAX_PRB, SRSLTE_CP_NORM)];

#define CFO_PLOT_LEN 0 /* Set to non zero for enabling CFO plot */
#if CFO_PLOT_LEN > 0
static plot_real_t    pcfo;
static uint32_t icfo = 0;
static float cfo_buffer[CFO_PLOT_LEN];
#endif /* CFO_PLOT_LEN > 0 */

void *plot_thread_run(void *arg) {
  srsue::phch_worker *worker = (srsue::phch_worker*) arg; 

  sdrgui_init();
  for (uint32_t tx = 0; tx < worker->get_cell_nof_ports(); tx++) {
    for (uint32_t rx = 0; rx < worker->get_rx_nof_antennas(); rx++) {
      char str_buf[64];
      snprintf(str_buf, 64, "|H%d%d|", rx, tx);
      plot_real_init(&pce[tx][rx]);
      plot_real_setTitle(&pce[tx][rx], str_buf);
      plot_real_setLabels(&pce[tx][rx], (char *) "Index", (char *) "dB");
      plot_real_setYAxisScale(&pce[tx][rx], -40, 40);

      plot_real_addToWindowGrid(&pce[tx][rx], (char*)"srsue", tx, rx);
    }
  }

  plot_scatter_init(&pconst);
  plot_scatter_setTitle(&pconst, (char*) "PDSCH - Equalized Symbols");
  plot_scatter_setXAxisScale(&pconst, -4, 4);
  plot_scatter_setYAxisScale(&pconst, -4, 4);

  plot_scatter_addToWindowGrid(&pconst, (char*)"srsue", 0, worker->get_rx_nof_antennas());

#if CFO_PLOT_LEN > 0
  plot_real_init(&pcfo);
  plot_real_setTitle(&pcfo, (char*) "CFO (Hz)");
  plot_real_setLabels(&pcfo, (char *) "Time", (char *) "Hz");
  plot_real_setYAxisScale(&pcfo, -4000, 4000);

  plot_scatter_addToWindowGrid(&pcfo, (char*)"srsue", 1, worker->get_rx_nof_antennas());
#endif /* CFO_PLOT_LEN > 0 */

  int n; 
  int readed_pdsch_re=0; 
  while(1) {
    sem_wait(&plot_sem);    
    
    if (readed_pdsch_re < SCATTER_PDSCH_PLOT_LEN) {
      n = worker->read_pdsch_d(&tmp_plot2[readed_pdsch_re]);
      readed_pdsch_re += n;           
    } else {
      for (uint32_t tx = 0; tx < worker->get_cell_nof_ports(); tx++) {
        for (uint32_t rx = 0; rx < worker->get_rx_nof_antennas(); rx++) {
          n = worker->read_ce_abs(tmp_plot, tx, rx);
          if (n > 0) {
            plot_real_setNewData(&pce[tx][rx], tmp_plot, n);
          }
        }
      }
      if (readed_pdsch_re > 0) {
        plot_scatter_setNewData(&pconst, tmp_plot2, readed_pdsch_re);
      }
      readed_pdsch_re = 0; 
    }

#if CFO_PLOT_LEN > 0
    cfo_buffer[icfo] = worker->get_cfo() * 15000.0f;
    icfo = (icfo + 1)%CFO_PLOT_LEN;
    plot_real_setNewData(&pcfo, cfo_buffer, CFO_PLOT_LEN);
#endif /* CFO_PLOT_LEN > 0 */

  }
  return NULL;
}


void init_plots(srsue::phch_worker *worker) {

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

