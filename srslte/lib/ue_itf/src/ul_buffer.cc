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
#include "srslte/srslte.h"

#include "srslte/ue_itf/sched_grant.h"
#include "srslte/ue_itf/ul_buffer.h"
#include "srslte/ue_itf/phy.h"
#include "srslte/ue_itf/params.h"

namespace srslte {
namespace ue {

bool ul_buffer::init_cell(srslte_cell_t cell_, params *params_db) {
  cell = cell_; 
  params_db = params_db; 
  current_tx_nb = 0;
  if (!srslte_ue_ul_init(&ue_ul, cell)) {  
    signal_buffer = (cf_t*) srslte_vec_malloc(sizeof(cf_t) * SRSLTE_SF_LEN_PRB(cell.nof_prb));
    cell_initiated = signal_buffer?true:false;
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

bool ul_buffer::generate_pusch(sched_grant pusch_grant, 
                                  uint8_t *payload) 
{
  srslte_uci_data_t uci_data; 
  bzero(&uci_data, sizeof(srslte_uci_data_t));
  return generate_pusch(pusch_grant, payload, uci_data);
}

bool ul_buffer::generate_pusch(sched_grant pusch_grant, 
                                  uint8_t *payload, 
                                  srslte_uci_data_t uci_data) 
{
  if (is_ready()) {
    if (!pusch_grant.is_uplink()) {
      fprintf(stderr, "Error in UL buffer: Invalid scheduling grant. Grant is for Downlink\n");
      return false; 
    }
    
    srslte_refsignal_drms_pusch_cfg_t drms_cfg; 
    bzero(&drms_cfg, sizeof(srslte_refsignal_drms_pusch_cfg_t));
    
    drms_cfg.beta_pusch          = (float) params_db->get_param(params::PUSCH_BETA)/10; 
    drms_cfg.group_hopping_en    = params_db->get_param(params::PUSCH_RS_GROUP_HOPPING_EN);
    drms_cfg.sequence_hopping_en = params_db->get_param(params::PUSCH_RS_SEQUENCE_HOPPING_EN);
    drms_cfg.cyclic_shift        = params_db->get_param(params::PUSCH_RS_CYCLIC_SHIFT);
    drms_cfg.delta_ss            = params_db->get_param(params::PUSCH_RS_GROUP_ASSIGNMENT);
    
    srslte_pusch_hopping_cfg_t pusch_hopping; 
    pusch_hopping.n_sb           = params_db->get_param(params::PUSCH_HOPPING_N_SB);
    pusch_hopping.hop_mode       = params_db->get_param(params::PUSCH_HOPPING_INTRA_SF) ? 
                                    pusch_hopping.SRSLTE_PUSCH_HOP_MODE_INTRA_SF : 
                                    pusch_hopping.SRSLTE_PUSCH_HOP_MODE_INTER_SF; 
    pusch_hopping.hopping_offset = params_db->get_param(params::PUSCH_HOPPING_OFFSET);
    pusch_hopping.current_tx_nb  = current_tx_nb; 
    
    srslte_ue_ul_set_pusch_cfg(&ue_ul, &drms_cfg, &pusch_hopping);

    int n = srslte_ue_ul_pusch_uci_encode_rnti(&ue_ul, (srslte_ra_pusch_t*) pusch_grant.get_grant_ptr(), 
                                              payload, uci_data, 
                                              (tti+tx_advance_sf)%10, pusch_grant.get_rnti(), 
                                              signal_buffer);
    if (n < 0) {
      fprintf(stderr, "Error in UL buffer: Error encoding PUSCH\n");
      return false; 
    }
    release();
    return true; 
  } else {
    fprintf(stderr, "Error in UL buffer: buffer not released\n");
    return false; 
  }
}

bool ul_buffer::generate_pucch(srslte_uci_data_t uci_data)
{  
  fprintf(stderr, "Not implemented\n");
  return false; 
}

bool ul_buffer::send_packet(radio *radio_handler, float time_adv_sec, srslte_timestamp_t rx_time)
{
  // send packet through usrp 
  srslte_timestamp_t tx_time; 
  srslte_timestamp_copy(&tx_time, &rx_time);
  srslte_timestamp_add(&tx_time, 0, tx_advance_sf*1e-3 - time_adv_sec); 
  INFO("Send PUSCH rx_time= %.6f tx_time = %.6f TA: %.1f us\n", 
        srslte_timestamp_real(&rx_time), 
        srslte_timestamp_real(&tx_time), time_adv_sec*1000000);
  
  radio_handler->tx(signal_buffer, SRSLTE_SF_LEN_PRB(cell.nof_prb), tx_time);                

  ready();
}
  
} // namespace ue
} // namespace srslte

