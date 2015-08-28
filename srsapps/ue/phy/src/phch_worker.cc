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

#include <string.h>
#include "srsapps/ue/phy/phch_worker.h"
#include "srsapps/common/mac_interface.h"
#include "srsapps/common/phy_interface.h"

namespace srslte {
  namespace ue {

#define log_h phy->log_h

phch_worker::phch_worker() : tr_exec(10240)
{
  phy = NULL; 
  signal_buffer = NULL; 
  
  cell_initiated  = false; 
  pregen_enabled  = false; 
  rar_cqi_request = false; 
  trace_enabled   = false; 
  cfi = 0;
  
  reset_ul_params();
  
}

void phch_worker::set_common(phch_common* phy_)
{
  phy = phy_;   
}
    
bool phch_worker::init_cell(srslte_cell_t cell_)
{
  memcpy(&cell, &cell_, sizeof(srslte_cell_t));
  
  // ue_sync in phy.cc requires a buffer for 2 subframes 
  signal_buffer = (cf_t*) srslte_vec_malloc(2 * sizeof(cf_t) * SRSLTE_SF_LEN_PRB(cell.nof_prb));
  if (!signal_buffer) {
    Error("Allocating memory\n");
    return false; 
  }
      
  if (srslte_ue_dl_init(&ue_dl, cell)) {    
    Error("Initiating UE DL\n");
    return false; 
  }
  
  if (srslte_ue_ul_init(&ue_ul, cell)) {  
    Error("Initiating UE UL\n");
    return false; 
  }

  srslte_ue_ul_set_normalization(&ue_ul, false); 
  srslte_ue_ul_set_cfo_enable(&ue_ul, true);
  
  cell_initiated = true; 
  
  return true; 
}

void phch_worker::free_cell()
{
  if (cell_initiated) {
    if (signal_buffer) {
      free(signal_buffer);
    }
    srslte_ue_dl_free(&ue_dl);
    srslte_ue_ul_free(&ue_ul);
  }
}

cf_t* phch_worker::get_buffer()
{
  return signal_buffer; 
}

void phch_worker::set_tti(uint32_t tti_)
{
  tti = tti_; 
}

void phch_worker::set_cfo(float cfo_)
{
  cfo = cfo_;
}

void phch_worker::set_crnti(uint16_t rnti)
{
  srslte_ue_dl_set_rnti(&ue_dl, rnti);
  srslte_ue_ul_set_rnti(&ue_ul, rnti);
}

void phch_worker::work_imp()
{
  if (!cell_initiated) {
    return; 
  }
  
  Debug("TTI %d running\n", tti);

  tr_log_start();
  
  reset_uci();

  bool ul_grant_available = false; 
  bool dl_ack = false; 
  
  mac_interface_phy::mac_grant_t    dl_mac_grant;
  mac_interface_phy::tb_action_dl_t dl_action; 
  bzero(&dl_action, sizeof(mac_interface_phy::tb_action_dl_t));

  mac_interface_phy::mac_grant_t    ul_mac_grant;
  mac_interface_phy::tb_action_ul_t ul_action; 
  bzero(&ul_action, sizeof(mac_interface_phy::tb_action_ul_t));

  /* Do FFT and extract PDCCH LLR, or quit if no actions are required in this subframe */
  if (extract_fft_and_pdcch_llr()) {
    
    
    /***** Downlink Processing *******/
    
    /* PDCCH DL + PDSCH */
    if(decode_pdcch_dl(&dl_mac_grant)) {
      /* Send grant to MAC and get action for this TB */
      phy->mac->new_grant_dl(dl_mac_grant, &dl_action);
      
      /* Decode PDSCH if instructed to do so */
      dl_ack = dl_action.default_ack; 
      if (dl_action.decode_enabled) {
        dl_ack = decode_pdsch(&dl_action.phy_grant.dl, dl_action.payload_ptr, 
                              dl_action.softbuffer, dl_action.rv, dl_action.rnti);      
      }
      if (dl_action.generate_ack_callback && dl_action.decode_enabled) {
        phy->mac->tb_decoded(dl_ack, dl_mac_grant.rnti_type, dl_mac_grant.pid);
        dl_ack = dl_action.generate_ack_callback(dl_action.generate_ack_callback_arg);
        Info("Calling generate ACK callback returned=%d\n", dl_ack);
      }
      if (dl_action.generate_ack) {
        set_uci_ack(dl_ack);
      }
    }

    // Decode PHICH 
    bool ul_ack; 
    bool ul_ack_available = decode_phich(&ul_ack); 

    
    
    /***** Uplink Processing + Transmission *******/
    
    /* Generate UCI */
    set_uci_sr();    
    set_uci_cqi();
    
    
    /* Check if we have UL grant. ul_phy_grant will be overwritten by new grant */
    ul_grant_available = decode_pdcch_ul(&ul_mac_grant);   
    
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
  }
  
  /* Transmit PUSCH, PUCCH or SRS */
  bool tx_signal = false; 
  if (ul_action.tx_enabled) {
    encode_pusch(&ul_action.phy_grant.ul, ul_action.payload_ptr, ul_action.current_tx_nb, 
                 ul_action.softbuffer, ul_action.rv, ul_action.rnti);          
    tx_signal = true; 
    if (ul_action.expect_ack) {
      phy->set_pending_ack(tti + 8, ue_ul.pusch_cfg.grant.n_prb_tilde[0], ul_action.phy_grant.ul.ncs_dmrs);
    }
  } else if (dl_action.generate_ack || uci_data.scheduling_request || uci_data.uci_cqi_len > 0) {
    encode_pucch();
    tx_signal = true; 
  } else if (srs_is_ready_to_send()) {
    encode_srs();
    tx_signal = true; 
  } 

  tr_log_end();
  
  phy->worker_end(tti, tx_signal, signal_buffer, SRSLTE_SF_LEN_PRB(cell.nof_prb), tx_time);
  
  if (dl_action.decode_enabled && !dl_action.generate_ack_callback) {
    phy->mac->tb_decoded(dl_ack, dl_mac_grant.rnti_type, dl_mac_grant.pid);
  }
}


bool phch_worker::extract_fft_and_pdcch_llr() {
  bool decode_pdcch = false; 
  if (phy->get_ul_rnti(tti) || phy->get_dl_rnti(tti) || phy->get_pending_rar(tti)) {
    decode_pdcch = true; 
  } 
  
  /* Without a grant, we might need to do fft processing if need to decode PHICH */
  if (phy->get_pending_ack(tti) || decode_pdcch) {
    if (srslte_ue_dl_decode_fft_estimate(&ue_dl, signal_buffer, tti%10, &cfi) < 0) {
      Error("Getting PDCCH FFT estimate\n");
      return false; 
    }        
  }
  
  if (decode_pdcch) { /* and not in DRX mode */ 
    if (srslte_pdcch_extract_llr(&ue_dl.pdcch, ue_dl.sf_symbols, ue_dl.ce, 0, tti%10, cfi)) {
      Error("Extracting PDCCH LLR\n");
      return false; 
    }
  }
  return (decode_pdcch || phy->get_pending_ack(tti));
}









/********************* Downlink processing functions ****************************/

bool phch_worker::decode_pdcch_dl(srslte::ue::mac_interface_phy::mac_grant_t* grant)
{
  dl_rnti = phy->get_dl_rnti(tti); 
  if (dl_rnti) {
    
    srslte_rnti_type_t type = phy->get_dl_rnti_type();

    srslte_dci_msg_t dci_msg; 
    srslte_ra_dl_dci_t dci_unpacked;

    if (srslte_ue_dl_find_dl_dci_type(&ue_dl, &dci_msg, cfi, tti%10, dl_rnti, type) != 1) {
      return false; 
    }
    
    if (srslte_dci_msg_to_dl_grant(&dci_msg, dl_rnti, cell.nof_prb, &dci_unpacked, &grant->phy_grant.dl)) {
      Error("Converting DCI message to DL grant\n");
      return false;   
    }

    /* Fill MAC grant structure */
    grant->ndi = dci_unpacked.ndi;
    grant->pid = dci_unpacked.harq_process;
    grant->n_bytes = grant->phy_grant.dl.mcs.tbs/8;
    grant->tti = tti; 
    grant->rv  = dci_unpacked.rv_idx;
    grant->rnti = dl_rnti; 
    grant->rnti_type = type; 
    
    last_dl_pdcch_ncce = srslte_ue_dl_get_ncce(&ue_dl);

    Info("PDCCH: DL DCI %s cce_index=%d, n_data_bits=%d\n", srslte_ra_dl_dci_string(&dci_unpacked), 
         ue_dl.last_n_cce, dci_msg.nof_bits);
    
    return true; 
  } else {
    return false; 
  }
}

bool phch_worker::decode_pdsch(srslte_ra_dl_grant_t *grant, uint8_t *payload, 
                               srslte_softbuffer_rx_t* softbuffer, uint32_t rv, uint16_t rnti)
{
  Debug("DL Buffer TTI %d: Decoding PDSCH\n", tti);

  /* Setup PDSCH configuration for this CFI, SFIDX and RVIDX */
  if (!srslte_ue_dl_cfg_grant(&ue_dl, grant, cfi, tti%10, rnti, rv)) {
    if (ue_dl.pdsch_cfg.grant.mcs.mod > 0 && ue_dl.pdsch_cfg.grant.mcs.tbs >= 0) {
      
      if (srslte_pdsch_decode_rnti(&ue_dl.pdsch, &ue_dl.pdsch_cfg, softbuffer, ue_dl.sf_symbols, 
                                    ue_dl.ce, 0, rnti, payload) == 0) 
      {
        Debug("TB decoded OK\n");
        return true; 
      } else {
        Debug("TB decoded KO\n");
        return false; 
      }
    } else {
      Warning("Received grant for TBS=0\n");
    }
  } else {
    Error("Error configuring DL grant\n"); 
  }
  return true; 
}

bool phch_worker::decode_phich(bool *ack)
{
  uint32_t I_lowest, n_dmrs; 
  if (phy->get_pending_ack(tti, &I_lowest, &n_dmrs)) {
    if (ack) {
      Debug("Decoding PHICH I_lowest=%d, n_dmrs=%d\n", I_lowest, n_dmrs);
      *ack = srslte_ue_dl_decode_phich(&ue_dl, tti%10, I_lowest, n_dmrs);     
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
  phy->reset_pending_ack(tti + 8); 

  srslte_dci_msg_t dci_msg; 
  srslte_ra_ul_dci_t dci_unpacked;
  srslte_dci_rar_grant_t rar_grant;
  srslte_rnti_type_t type = phy->get_ul_rnti_type();
  
  bool ret = false; 
  if (phy->get_pending_rar(tti, &rar_grant)) {
    Info("Pending RAR UL grant\n");
    if (srslte_dci_rar_to_ul_grant(&rar_grant, cell.nof_prb, pusch_hopping.hopping_offset, 
      &dci_unpacked, &grant->phy_grant.ul)) 
    {
      Error("Converting RAR message to UL grant\n");
      return false; 
    } 
    grant->rnti_type = SRSLTE_RNTI_TEMP;
    grant->is_from_rar = true; 
    Info("RAR grant found for TTI=%d\n", tti);
    rar_cqi_request = rar_grant.cqi_request;    
    ret = true;  
  } else {
    ul_rnti = phy->get_ul_rnti(tti);
    if (ul_rnti) {
      if (srslte_ue_dl_find_ul_dci(&ue_dl, &dci_msg, cfi, tti%10, ul_rnti) != 1) {
        return false; 
      }
      if (srslte_dci_msg_to_ul_grant(&dci_msg, cell.nof_prb, pusch_hopping.hopping_offset, 
        &dci_unpacked, &grant->phy_grant.ul)) 
      {
        Error("Converting DCI message to UL grant\n");
        return false;   
      }
      grant->rnti_type = type; 
      grant->is_from_rar = false; 
      ret = true; 
      Info("PDCCH: UL DCI Format0 cce_index=%d, n_data_bits=%d\n", ue_dl.last_n_cce, dci_msg.nof_bits);
    }
  }
  if (ret) {    
    grant->ndi = dci_unpacked.ndi;
    grant->pid = 0; // This is computed by MAC from TTI 
    grant->n_bytes = grant->phy_grant.ul.mcs.tbs/8;
    grant->tti = tti; 
    grant->rnti = ul_rnti; 
    
    if (SRSLTE_VERBOSE_ISINFO()) {
      srslte_ra_pusch_fprint(stdout, &dci_unpacked, cell.nof_prb);
    }
    
    return true;
  } else {
    return false; 
  }    
}

void phch_worker::reset_uci()
{
  bzero(&uci_data, sizeof(srslte_uci_data_t));
}

void phch_worker::set_uci_ack(bool ack)
{
  uci_data.uci_ack = ack?1:0;
  uci_data.uci_ack_len = 1; 
}

void phch_worker::set_uci_sr()
{
  uci_data.scheduling_request = false; 
  if (phy->sr_enabled) {
    // Get I_sr parameter
    if (srslte_ue_ul_sr_send_tti(I_sr, tti+4)) {
      Info("SR transmission at TTI=%d\n", tti+4);
      uci_data.scheduling_request = true; 
      phy->sr_last_tx_tti = tti+4; 
      phy->sr_enabled = false;
    }
  } 
}

void phch_worker::set_uci_cqi()
{
  if (cqi_cfg.configured || rar_cqi_request) {
    if (srslte_cqi_send(cqi_cfg.pmi_idx, tti+4)) {
      uci_data.uci_cqi_len = 4; 
      uint8_t cqi[4] = {1, 1, 1, 1}; 
      uci_data.uci_cqi = cqi;          
      rar_cqi_request = false; 
    }
  }
}

bool phch_worker::srs_is_ready_to_send() {
  if (srs_cfg.configured) {
    if (srslte_refsignal_srs_send_cs(srs_cfg.subframe_config, (tti+4)%10) == 1 && 
        srslte_refsignal_srs_send_ue(srs_cfg.I_srs, tti+4)              == 1)
    {
      return true; 
    }
  }
  return false; 
}

void phch_worker::set_tx_time(srslte_timestamp_t _tx_time)
{
  memcpy(&tx_time, &_tx_time, sizeof(srslte_timestamp_t));
}

void phch_worker::normalize() {
  srslte_vec_norm_cfc(signal_buffer, 0.8, signal_buffer, SRSLTE_SF_LEN_PRB(cell.nof_prb));  
}

void phch_worker::encode_pusch(srslte_ra_ul_grant_t *grant, uint8_t *payload, uint32_t current_tx_nb, 
                               srslte_softbuffer_tx_t* softbuffer, uint32_t rv, uint16_t rnti)
{
  
  if (srslte_ue_ul_cfg_grant(&ue_ul, grant, tti+4, rv, current_tx_nb)) {
    Error("Configuring UL grant\n");
  }
    
  if (srslte_ue_ul_pusch_encode_rnti_softbuffer(&ue_ul, 
                                                payload, uci_data, 
                                                softbuffer,
                                                rnti, 
                                                signal_buffer)) 
  {
    Error("Encoding PUSCH\n");
  }
    
  Info("PUSCH: TTI=%d, CFO= %.1f KHz TBS=%d, mod=%s, rb_start=%d n_prb=%d, ack=%s, sr=%s, rnti=%d, shortened=%s\n", 
        tti+4, cfo*15e3, grant->mcs.tbs, srslte_mod_string(grant->mcs.mod),
        grant->n_prb[0], grant->L_prb,  
        uci_data.uci_ack_len>0?(uci_data.uci_ack?"1":"0"):"no",uci_data.scheduling_request?"yes":"no", 
        rnti, ue_ul.pusch.shortened?"yes":"no");

  normalize();
  
  /*
  char filename[128];
  sprintf(filename, "pusch%d",tti+4);
  srslte_vec_save_file(filename, signal_buffer, SRSLTE_SF_LEN_PRB(cell.nof_prb)*sizeof(cf_t));
  */
}

void phch_worker::encode_pucch()
{

  if (uci_data.scheduling_request || uci_data.uci_ack_len > 0) 
  {
    if (srslte_ue_ul_pucch_encode(&ue_ul, uci_data, last_dl_pdcch_ncce, tti+4, signal_buffer)) {
      Error("Encoding PUCCH\n");
    }

    Info("PUCCH: TTI=%d, CFO= %.1f KHz n_cce=%d, ack=%s, sr=%s, shortened=%s\n", tti+4, cfo*15e3, last_dl_pdcch_ncce, 
      uci_data.uci_ack_len>0?(uci_data.uci_ack?"1":"0"):"no",uci_data.scheduling_request?"yes":"no", 
      ue_ul.pucch.shortened?"yes":"no");        
  }   
  
  if (uci_data.scheduling_request) {
    phy->sr_enabled = false; 
  }
  normalize();
}

void phch_worker::encode_srs()
{
  if (srslte_ue_ul_srs_encode(&ue_ul, tti+4, signal_buffer)) 
  {
    Error("Encoding SRS\n");
  }
    
  Info("SRS: TTI=%d, CFO= %.1f KHz \n", tti+4, cfo*15e3);
  
  normalize();
}

void phch_worker::enable_pregen_signals(bool enabled)
{
  pregen_enabled = enabled; 
}

void phch_worker::reset_ul_params() 
{
  bzero(&dmrs_cfg, sizeof(srslte_refsignal_dmrs_pusch_cfg_t));    
  bzero(&pusch_hopping, sizeof(srslte_pusch_hopping_cfg_t));
  bzero(&uci_cfg, sizeof(srslte_uci_cfg_t));
  bzero(&pucch_cfg, sizeof(srslte_pucch_cfg_t));
  bzero(&pucch_sched, sizeof(srslte_pucch_sched_t));
  bzero(&srs_cfg, sizeof(srslte_refsignal_srs_cfg_t));
  bzero(&cqi_cfg, sizeof(srslte_cqi_cfg_t));
  I_sr = 0; 
}

void phch_worker::set_ul_params()
{

  /* PUSCH DMRS signal configuration */
  bzero(&dmrs_cfg, sizeof(srslte_refsignal_dmrs_pusch_cfg_t));    
  dmrs_cfg.group_hopping_en    = (bool)     phy->params_db->get_param(phy_interface_params::DMRS_GROUP_HOPPING_EN);
  dmrs_cfg.sequence_hopping_en = (bool)     phy->params_db->get_param(phy_interface_params::DMRS_SEQUENCE_HOPPING_EN);
  dmrs_cfg.cyclic_shift        = (uint32_t) phy->params_db->get_param(phy_interface_params::PUSCH_RS_CYCLIC_SHIFT);
  dmrs_cfg.delta_ss            = (uint32_t) phy->params_db->get_param(phy_interface_params::PUSCH_RS_GROUP_ASSIGNMENT);
  
  /* PUSCH Hopping configuration */
  bzero(&pusch_hopping, sizeof(srslte_pusch_hopping_cfg_t));
  pusch_hopping.n_sb           = (uint32_t) phy->params_db->get_param(phy_interface_params::PUSCH_HOPPING_N_SB);
  pusch_hopping.hop_mode       = (uint32_t) phy->params_db->get_param(phy_interface_params::PUSCH_HOPPING_INTRA_SF) ? 
                                  pusch_hopping.SRSLTE_PUSCH_HOP_MODE_INTRA_SF : 
                                  pusch_hopping.SRSLTE_PUSCH_HOP_MODE_INTER_SF; 
  pusch_hopping.hopping_offset = (uint32_t) phy->params_db->get_param(phy_interface_params::PUSCH_HOPPING_OFFSET);

  /* PUSCH UCI configuration */
  bzero(&uci_cfg, sizeof(srslte_uci_cfg_t));
  uci_cfg.I_offset_ack         = (uint32_t) phy->params_db->get_param(phy_interface_params::UCI_I_OFFSET_ACK);
  uci_cfg.I_offset_cqi         = (uint32_t) phy->params_db->get_param(phy_interface_params::UCI_I_OFFSET_CQI);
  uci_cfg.I_offset_ri          = (uint32_t) phy->params_db->get_param(phy_interface_params::UCI_I_OFFSET_RI);
  
  /* PUCCH configuration */  
  bzero(&pucch_cfg, sizeof(srslte_pucch_cfg_t));
  pucch_cfg.delta_pucch_shift  = (uint32_t) phy->params_db->get_param(phy_interface_params::PUCCH_DELTA_SHIFT);
  pucch_cfg.N_cs               = (uint32_t) phy->params_db->get_param(phy_interface_params::PUCCH_CYCLIC_SHIFT);
  pucch_cfg.n_rb_2             = (uint32_t) phy->params_db->get_param(phy_interface_params::PUCCH_N_RB_2);
  pucch_cfg.srs_configured     = (bool)     phy->params_db->get_param(phy_interface_params::SRS_IS_CONFIGURED)?true:false;
  pucch_cfg.srs_cs_subf_cfg    = (uint32_t) phy->params_db->get_param(phy_interface_params::SRS_CS_SFCFG);
  pucch_cfg.srs_simul_ack      = (bool)     phy->params_db->get_param(phy_interface_params::SRS_CS_ACKNACKSIMUL)?true:false;
  
  /* PUCCH Scheduling configuration */
  bzero(&pucch_sched, sizeof(srslte_pucch_sched_t));
  pucch_sched.n_pucch_1[0]     = (uint32_t) phy->params_db->get_param(phy_interface_params::PUCCH_N_PUCCH_1_0);
  pucch_sched.n_pucch_1[1]     = (uint32_t) phy->params_db->get_param(phy_interface_params::PUCCH_N_PUCCH_1_1);
  pucch_sched.n_pucch_1[2]     = (uint32_t) phy->params_db->get_param(phy_interface_params::PUCCH_N_PUCCH_1_2);
  pucch_sched.n_pucch_1[3]     = (uint32_t) phy->params_db->get_param(phy_interface_params::PUCCH_N_PUCCH_1_3);
  pucch_sched.N_pucch_1        = (uint32_t) phy->params_db->get_param(phy_interface_params::PUCCH_N_PUCCH_1);
  pucch_sched.n_pucch_2        = (uint32_t) phy->params_db->get_param(phy_interface_params::PUCCH_N_PUCCH_2);
  pucch_sched.n_pucch_sr       = (uint32_t) phy->params_db->get_param(phy_interface_params::PUCCH_N_PUCCH_SR);

  /* SRS Configuration */
  bzero(&srs_cfg, sizeof(srslte_refsignal_srs_cfg_t));
  srs_cfg.configured           = (bool)     phy->params_db->get_param(phy_interface_params::SRS_IS_CONFIGURED)?true:false;
  srs_cfg.subframe_config      = (uint32_t) phy->params_db->get_param(phy_interface_params::SRS_CS_SFCFG);
  srs_cfg.bw_cfg               = (uint32_t) phy->params_db->get_param(phy_interface_params::SRS_CS_BWCFG);
  srs_cfg.I_srs                = (uint32_t) phy->params_db->get_param(phy_interface_params::SRS_UE_CONFIGINDEX);
  srs_cfg.B                    = (uint32_t) phy->params_db->get_param(phy_interface_params::SRS_UE_BW);
  srs_cfg.b_hop                = (uint32_t) phy->params_db->get_param(phy_interface_params::SRS_UE_HOP);
  srs_cfg.n_rrc                = (uint32_t) phy->params_db->get_param(phy_interface_params::SRS_UE_NRRC);
  srs_cfg.k_tc                 = (uint32_t) phy->params_db->get_param(phy_interface_params::SRS_UE_TXCOMB);
  srs_cfg.n_srs                = (uint32_t) phy->params_db->get_param(phy_interface_params::SRS_UE_CYCLICSHIFT);

  srslte_ue_ul_set_cfg(&ue_ul, &dmrs_cfg, &srs_cfg, &pucch_cfg, &pucch_sched, &uci_cfg, &pusch_hopping);

  /* CQI configuration */
  bzero(&cqi_cfg, sizeof(srslte_cqi_cfg_t));
  cqi_cfg.configured           = (bool)     phy->params_db->get_param(phy_interface_params::CQI_PERIODIC_CONFIGURED)?true:false;
  cqi_cfg.pmi_idx              = (uint32_t) phy->params_db->get_param(phy_interface_params::CQI_PERIODIC_PMI_IDX); 
  
  /* SR configuration */
  I_sr                         = (uint32_t) phy->params_db->get_param(phy_interface_params::SR_CONFIG_INDEX);
  
  if (pregen_enabled) { 
    printf("Pre-generating UL signals\n");
    srslte_ue_ul_pregen_signals(&ue_ul);
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
}