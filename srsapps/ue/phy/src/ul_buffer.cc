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

bool ul_buffer::init_cell(srslte_cell_t cell_, phy_params *params_db_, log *log_h_) {
  cell = cell_; 
  log_h = log_h_; 
  params_db = params_db_; 
  current_tx_nb = 0;
  if (!srslte_ue_ul_init(&ue_ul, cell)) {  
    signal_buffer = (cf_t*) srslte_vec_malloc(sizeof(cf_t) * SRSLTE_SF_LEN_PRB(cell.nof_prb));
    cell_initiated = signal_buffer?true:false;
    srslte_ue_ul_set_cfo_enable(&ue_ul, false);
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
  return false; 
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

bool ul_buffer::generate_data(ul_sched_grant *grant, srslte_softbuffer_tx_t *softbuffer, uint8_t *payload) 
{
  if (is_ready()) {
    
    bzero(signal_buffer, sizeof(cf_t)*SRSLTE_SF_LEN_PRB(cell.nof_prb));
    
    srslte_refsignal_dmrs_pusch_cfg_t dmrs_cfg; 
    bzero(&dmrs_cfg, sizeof(srslte_refsignal_dmrs_pusch_cfg_t));    
    dmrs_cfg.beta_pusch          = (float) params_db->get_param(phy_params::PUSCH_BETA)/10; 
    dmrs_cfg.group_hopping_en    = params_db->get_param(phy_params::PUSCH_RS_GROUP_HOPPING_EN);
    dmrs_cfg.sequence_hopping_en = params_db->get_param(phy_params::PUSCH_RS_SEQUENCE_HOPPING_EN);
    dmrs_cfg.cyclic_shift        = params_db->get_param(phy_params::PUSCH_RS_CYCLIC_SHIFT);
    dmrs_cfg.delta_ss            = params_db->get_param(phy_params::PUSCH_RS_GROUP_ASSIGNMENT);
    
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
    pucch_cfg.group_hopping_en  = dmrs_cfg.group_hopping_en;
    pucch_cfg.N_cs              = params_db->get_param(phy_params::PUCCH_CYCLIC_SHIFT);
    pucch_cfg.n_rb_2            = params_db->get_param(phy_params::PUCCH_N_RB_2);

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
    
    srslte_ue_ul_set_cfg(&ue_ul, &dmrs_cfg, &pusch_hopping, &pucch_cfg, &pucch_sched);

    uci_data.I_offset_ack    = params_db->get_param(phy_params::UCI_I_OFFSET_ACK);
    uci_data.I_offset_cqi    = params_db->get_param(phy_params::UCI_I_OFFSET_CQI);
    uci_data.I_offset_ri     = params_db->get_param(phy_params::UCI_I_OFFSET_RI);
    
    int n = 0; 
    // Transmit on PUSCH if UL grant available, otherwise in PUCCH 
    if (grant) {

      INFO("Encoding PUSCH TBS=%d, rb_start=%d n_prb=%d, rv=%d, rnti=%d\n", 
           grant->get_tbs(), pusch_cfg.grant.L_prb, pusch_cfg.grant.n_prb[0], grant->get_rv(), grant->get_rnti());
    
      grant->to_pusch_cfg(tti%10, cell.cp, &pusch_cfg);
      n = srslte_ue_ul_pusch_encode_cfg(&ue_ul, &pusch_cfg, 
                                        payload, uci_data, 
                                        softbuffer,
                                        grant->get_rnti(), 
                                        signal_buffer);            
    } else {
      Info("Encoding PUCCH n_cce=%d, ack=%d\n", last_n_cce, uci_data.uci_ack);
    
      n = srslte_ue_ul_pucch_encode(&ue_ul, uci_data, tti&10, signal_buffer);
    }
    // Reset UCI data
    bzero(&uci_data, sizeof(srslte_uci_data_t));
    uci_pending = false; 
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

bool ul_buffer::send(srslte::radio* radio_handler, float time_adv_sec, float cfo, srslte_timestamp_t rx_time)
{
  // send packet through usrp 
  srslte_timestamp_t tx_time; 
  srslte_timestamp_copy(&tx_time, &rx_time);
  srslte_timestamp_add(&tx_time, 0, tx_advance_sf*1e-3 - time_adv_sec); 

  // Correct CFO before transmission
  srslte_cfo_correct(&ue_ul.cfo, signal_buffer, signal_buffer, cfo / srslte_symbol_sz(cell.nof_prb));            
  
  // Compute peak
  float max = 0; 
  float *t = (float*) signal_buffer;
  for (int i=0;i<2*SRSLTE_SF_LEN_PRB(cell.nof_prb);i++) {
    if (fabsf(t[i]) > max) {
      max = fabsf(t[i]);
    }
  }

  Info("TX CFO: %f, len=%d, rx_time= %.6f tx_time = %.6f TA: %.1f us PeakAmplitude=%.2f\n", 
        cfo*15000, SRSLTE_SF_LEN_PRB(cell.nof_prb),
        srslte_timestamp_real(&rx_time), 
        srslte_timestamp_real(&tx_time), time_adv_sec*1000000, max);
  
  if (max > 1.0) {
    srslte_vec_save_file((char*) "first_pusch", signal_buffer, sizeof(cf_t)*SRSLTE_SF_LEN_PRB(cell.nof_prb));
  }
  
  radio_handler->tx(signal_buffer, SRSLTE_SF_LEN_PRB(cell.nof_prb), tx_time);                
  
  //srslte_vec_save_file("pucch_tx", signal_buffer, sizeof(cf_t)*SRSLTE_SF_LEN_PRB(cell.nof_prb));
  
  ready();
}
  
} // namespace ue
} // namespace srslte

