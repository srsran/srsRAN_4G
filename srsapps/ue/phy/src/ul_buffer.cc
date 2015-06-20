/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include <string.h>
#include <strings.h>
#include <pthread.h>
#include <math.h>

#include "srslte/srslte.h"

#include "srsapps/common/log.h"
#include "srsapps/ue/phy/sched_grant.h"
#include "srsapps/ue/phy/ul_buffer.h"
#include "srsapps/ue/phy/phy.h"
#include "srsapps/ue/phy/phy_params.h"

namespace srslte {
namespace ue {

bool ul_buffer::init_cell(srslte_cell_t cell_, phy_params *params_db_, log *log_h_, radio *radio_h_) {
  cell = cell_; 
  log_h = log_h_; 
  radio_h = radio_h_; 
  params_db = params_db_; 
  current_tx_nb = 0;
  tti_is_end_of_burst = false; 
  if (!srslte_ue_ul_init(&ue_ul, cell)) {  
    srslte_ue_ul_set_normalization(&ue_ul, false); 
    signal_buffer = (cf_t*) srslte_vec_malloc(sizeof(cf_t) * SRSLTE_SF_LEN_PRB(cell.nof_prb));
    cell_initiated = (signal_buffer)?true:false;
    srslte_ue_ul_set_cfo_enable(&ue_ul, true);
    bzero(&uci_data, sizeof(srslte_uci_data_t));
    uci_pending = false; 
    return cell_initiated; 
  } else {
    return false; 
  }
}

void ul_buffer::free_cell() {
  if (cell_initiated) {
    if (signal_buffer) {
      free(signal_buffer);
    }
    srslte_ue_ul_free(&ue_ul);    
  }
}

void ul_buffer::set_crnti(uint16_t rnti)
{
  srslte_ue_ul_set_rnti(&ue_ul, rnti);
}

bool ul_buffer::generate_ack(bool ack, dl_sched_grant *last_dl_grant)
{
  uci_data.uci_ack_len = 1; 
  uci_data.uci_ack = ack?1:0; 
  uci_pending = true; 
  last_n_cce = last_dl_grant->get_ncce();
}

bool ul_buffer::generate_ack(bool ack[2])
{
  uci_data.uci_ack_len = 2; 
  uci_data.uci_ack = ack[0]?1:0; 
  uci_data.uci_ack_2 = ack[1]?1:0; 
  uci_pending = true; 
}

void ul_buffer::set_current_tx_nb(uint32_t current_tx_nb_)
{
  current_tx_nb = current_tx_nb_;
}

bool ul_buffer::generate_cqi_report()
{
  uci_data.uci_cqi_len = 4; 
  uint8_t cqi[4] = {1, 1, 1, 1}; 
  uci_data.uci_cqi = cqi;          
  return true; 
}

bool ul_buffer::generate_sr() {
  uci_data.scheduling_request = true; 
  uci_pending = true; 
  return true; 
}

bool ul_buffer::uci_ready() {
  return uci_pending; 
}

bool ul_buffer::generate_data() {
  return generate_data(NULL, NULL);
}

bool ul_buffer::generate_data(ul_sched_grant *grant, 
                              uint8_t *payload) 
{
  generate_data(grant, &ue_ul.softbuffer, payload); 
}
//int nof_tx=0; 


bool ul_buffer::srs_is_ready_to_send() {
  if (params_db->get_param(phy_params::SRS_IS_CONFIGURED)) 
  {
    if (srslte_refsignal_srs_send_cs(params_db->get_param(phy_params::SRS_CS_SFCFG), tti%10) == 1 && 
        srslte_refsignal_srs_send_ue(params_db->get_param(phy_params::SRS_UE_CONFIGINDEX), tti) == 1)
    {
      return true; 
    }
  }
  return false; 
}

int srspkt = 0; 

bool ul_buffer::generate_data(ul_sched_grant *grant, srslte_softbuffer_tx_t *softbuffer, uint8_t *payload) 
{
  if (is_ready()) {
    
    bzero(signal_buffer, sizeof(cf_t)*SRSLTE_SF_LEN_PRB(cell.nof_prb));
    
    srslte_refsignal_dmrs_pusch_cfg_t dmrs_cfg; 
    bzero(&dmrs_cfg, sizeof(srslte_refsignal_dmrs_pusch_cfg_t));    
    dmrs_cfg.beta_pusch      = (float) params_db->get_param(phy_params::PUSCH_BETA)/10; 
    bool group_hopping_en    = params_db->get_param(phy_params::DMRS_GROUP_HOPPING_EN);
    bool sequence_hopping_en = params_db->get_param(phy_params::DMRS_SEQUENCE_HOPPING_EN);
    dmrs_cfg.cyclic_shift    = params_db->get_param(phy_params::PUSCH_RS_CYCLIC_SHIFT);
    dmrs_cfg.delta_ss        = params_db->get_param(phy_params::PUSCH_RS_GROUP_ASSIGNMENT);
    
    // Get cyclic shift for DMRS if PUSCH is not for RAR or (TODO) is not SPS
    if (grant) {
      if (!grant->is_from_rar()) {
        dmrs_cfg.en_dmrs_2 = true; 
        dmrs_cfg.cyclic_shift_for_dmrs = grant->get_n_dmrs();
      }
    }    

    srslte_pusch_hopping_cfg_t pusch_hopping; 
    if (grant) {
      bzero(&pusch_hopping, sizeof(srslte_pusch_hopping_cfg_t));
      pusch_hopping.n_sb           = params_db->get_param(phy_params::PUSCH_HOPPING_N_SB);
      pusch_hopping.hop_mode       = params_db->get_param(phy_params::PUSCH_HOPPING_INTRA_SF) ? 
                                      pusch_hopping.SRSLTE_PUSCH_HOP_MODE_INTRA_SF : 
                                      pusch_hopping.SRSLTE_PUSCH_HOP_MODE_INTER_SF; 
      pusch_hopping.hopping_offset = params_db->get_param(phy_params::PUSCH_HOPPING_OFFSET);
      pusch_hopping.current_tx_nb  = grant->get_current_tx_nb();       
    }
      
    srslte_pucch_cfg_t pucch_cfg; 
    bzero(&pucch_cfg, sizeof(srslte_pucch_cfg_t));
    pucch_cfg.beta_pucch        = (float) params_db->get_param(phy_params::PUCCH_BETA)/10; 
    pucch_cfg.delta_pucch_shift = params_db->get_param(phy_params::PUCCH_DELTA_SHIFT);
    pucch_cfg.N_cs              = params_db->get_param(phy_params::PUCCH_CYCLIC_SHIFT);
    pucch_cfg.n_rb_2            = params_db->get_param(phy_params::PUCCH_N_RB_2);
    pucch_cfg.srs_configured = params_db->get_param(phy_params::SRS_IS_CONFIGURED)?true:false;
    pucch_cfg.srs_cs_subf_cfg   = (uint32_t) params_db->get_param(phy_params::SRS_CS_SFCFG);
    pucch_cfg.srs_simul_ack     = params_db->get_param(phy_params::SRS_CS_ACKNACKSIMUL)?true:false;
    
    srslte_pucch_sched_t pucch_sched; 
    bzero(&pucch_sched, sizeof(srslte_pucch_sched_t));
    pucch_sched.n_cce = last_n_cce; 
    pucch_sched.n_pucch_1[0] = params_db->get_param(phy_params::PUCCH_N_PUCCH_1_0);
    pucch_sched.n_pucch_1[1] = params_db->get_param(phy_params::PUCCH_N_PUCCH_1_1);
    pucch_sched.n_pucch_1[2] = params_db->get_param(phy_params::PUCCH_N_PUCCH_1_2);
    pucch_sched.n_pucch_1[3] = params_db->get_param(phy_params::PUCCH_N_PUCCH_1_3);
    pucch_sched.N_pucch_1    = params_db->get_param(phy_params::PUCCH_N_PUCCH_1);
    pucch_sched.n_pucch_2    = params_db->get_param(phy_params::PUCCH_N_PUCCH_2);
    pucch_sched.n_pucch_sr   = params_db->get_param(phy_params::PUCCH_N_PUCCH_SR);

    srslte_refsignal_srs_cfg_t srs_cfg;           
    bzero(&srs_cfg, sizeof(srslte_refsignal_srs_cfg_t));
    srs_cfg.configured   = params_db->get_param(phy_params::SRS_IS_CONFIGURED)?true:false;
    srs_cfg.subframe_config = (uint32_t) params_db->get_param(phy_params::SRS_CS_SFCFG);
    srs_cfg.bw_cfg          = (uint32_t) params_db->get_param(phy_params::SRS_CS_BWCFG);
    srs_cfg.I_srs           = (uint32_t) params_db->get_param(phy_params::SRS_UE_CONFIGINDEX);
    srs_cfg.B               = (uint32_t) params_db->get_param(phy_params::SRS_UE_BW);
    srs_cfg.b_hop           = (uint32_t) params_db->get_param(phy_params::SRS_UE_HOP);
    srs_cfg.n_rrc           = (uint32_t) params_db->get_param(phy_params::SRS_UE_NRRC);
    srs_cfg.k_tc            = (uint32_t) params_db->get_param(phy_params::SRS_UE_TXCOMB);
    srs_cfg.n_srs           = (uint32_t) params_db->get_param(phy_params::SRS_UE_CYCLICSHIFT);
    srs_cfg.beta_srs        = ((float) params_db->get_param(phy_params::SRS_BETA))/10;
    
    srslte_ue_ul_set_cfg(&ue_ul, &dmrs_cfg, &pucch_cfg, &srs_cfg, &pucch_sched, 
                         group_hopping_en, sequence_hopping_en);

    uci_data.I_offset_ack    = params_db->get_param(phy_params::UCI_I_OFFSET_ACK);
    uci_data.I_offset_cqi    = params_db->get_param(phy_params::UCI_I_OFFSET_CQI);
    uci_data.I_offset_ri     = params_db->get_param(phy_params::UCI_I_OFFSET_RI);

    srslte_ue_ul_set_cfo(&ue_ul, cfo);
    
    int n = 0; 
    // Transmit on PUSCH if UL grant available, otherwise in PUCCH 
    if (grant) {
      
      if (params_db->get_param(phy_params::CQI_PERIODIC_CONFIGURED)) {
        if (srslte_cqi_send(params_db->get_param(phy_params::CQI_PERIODIC_PMI_IDX), tti)) {
          generate_cqi_report();
        }
      }
      
      srslte_pusch_hopping_cfg_t pusch_hopping_cfg; 
      bzero(&pusch_hopping_cfg, sizeof(srslte_pusch_hopping_cfg_t));
      
      pusch_hopping_cfg.n_sb           = params_db->get_param(phy_params::PUSCH_HOPPING_N_SB);
      pusch_hopping_cfg.hop_mode       = params_db->get_param(phy_params::PUSCH_HOPPING_INTRA_SF) ? 
                                      srslte_pusch_hopping_cfg_t::SRSLTE_PUSCH_HOP_MODE_INTRA_SF : 
                                      srslte_pusch_hopping_cfg_t::SRSLTE_PUSCH_HOP_MODE_INTER_SF; 
      pusch_hopping_cfg.hopping_offset = params_db->get_param(phy_params::PUSCH_HOPPING_OFFSET);
      pusch_hopping_cfg.current_tx_nb  = grant->get_current_tx_nb();       

      grant->to_pusch_cfg(&pusch_hopping_cfg, &srs_cfg, tti, &ue_ul);

      n = srslte_ue_ul_pusch_encode_rnti_softbuffer(&ue_ul, 
                                                    payload, uci_data, 
                                                    softbuffer,
                                                    grant->get_rnti(), 
                                                    signal_buffer);    
      
      if (ue_ul.pusch.shortened) {
        Warning("PUSCH shortened on tti=%d\n", tti);
      }
      

      Info("PUSCH: TTI=%d, CFO= %.1f KHz TBS=%d, mod=%s, rb_start=%d n_prb=%d, ack=%s, sr=%s, rnti=%d, shortened=%s\n", 
           tti, cfo*15e3, grant->get_tbs(), srslte_mod_string(ue_ul.pusch_cfg.grant.mcs.mod), ue_ul.pusch_cfg.grant.n_prb[0], 
           ue_ul.pusch_cfg.grant.L_prb,  
           uci_data.uci_ack_len>0?(uci_data.uci_ack?"1":"0"):"no",uci_data.scheduling_request?"yes":"no", 
           grant->get_rnti(), ue_ul.pusch.shortened?"yes":"no");
    

    } else if (uci_data.scheduling_request || uci_data.uci_cqi_len > 0 || uci_data.uci_ack_len) {
      n = srslte_ue_ul_pucch_encode(&ue_ul, uci_data, tti%10, signal_buffer);

      Info("PUCCH: TTI=%d, CFO= %.1f KHz n_cce=%d, ack=%s, sr=%s, shortened=%s\n", tti, cfo*15e3, last_n_cce, 
        uci_data.uci_ack_len>0?(uci_data.uci_ack?"1":"0"):"no",uci_data.scheduling_request?"yes":"no", 
        ue_ul.pucch.shortened?"yes":"no");        
    } else {
      n = srslte_ue_ul_srs_encode(&ue_ul, tti, signal_buffer);
      
      Info("SRS: TTI=%d, CFO= %.1f KHz \n", tti, cfo*15e3);
    }
    
    // Reset UCI data
    bzero(&uci_data, sizeof(srslte_uci_data_t));
    uci_pending = false; 
      
    // Compute peak
    float max = 0; 
    if (normalize_amp) {
      float *t = (float*) signal_buffer;
      for (int i=0;i<2*SRSLTE_SF_LEN_PRB(cell.nof_prb);i++) {
        if (fabsf(t[i]) > max) {
          max = fabsf(t[i]);
        }
      }
      
      // Normalize before TX 
      srslte_vec_sc_prod_cfc(signal_buffer, 0.7/max, signal_buffer, SRSLTE_SF_LEN_PRB(cell.nof_prb));
    }
    
    radio_h->tx(signal_buffer, SRSLTE_SF_LEN_PRB(cell.nof_prb), tx_time);         

    release();
    
    if (n < 0) {
      fprintf(stderr, "Error in UL buffer: Error encoding %s\n", signal_buffer?"PUSCH":"PUCCH");
      return false; 
    } else {
      return true; 
    }
  } else {
    fprintf(stderr, "Error in UL buffer: buffer not released\n");
    return false; 
  }
}

int nof_tx = 0; 

void ul_buffer::set_tx_params(float cfo_, float time_adv_sec, srslte_timestamp_t tx_time_)
{
  tti_is_end_of_burst = false; 
  cfo = cfo_; 
  srslte_timestamp_copy(&tx_time, &tx_time_);
  srslte_timestamp_add(&tx_time, 0, 4e-3 - time_adv_sec);
}

void ul_buffer::set_end_of_burst()
{
  Info("TTI %d Is end of burst\n", tti);
  tti_is_end_of_burst = true; 
}

bool ul_buffer::is_end_of_burst()
{
  return tti_is_end_of_burst; 
}
  
} // namespace ue
} // namespace srslte

