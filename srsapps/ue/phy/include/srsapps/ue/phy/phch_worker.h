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
#include "srslte/srslte.h"
#include "srsapps/common/thread_pool.h"
#include "srsapps/common/phy_interface.h"
#include "srsapps/ue/phy/phch_common.h"


#ifndef UEPHYWORKER_H
#define UEPHYWORKER_H

namespace srslte {                                                                                                                                                                                                                                                                                                                                                                                                                                                     
namespace ue {

class phch_worker : public thread_pool::worker
{
public:
  
  phch_worker();
  void  set_common(phch_common *phy);
  bool  init_cell(srslte_cell_t cell);
  void  free_cell();
  
  /* Functions used by main PHY thread */
  cf_t *get_buffer();
  void  set_tti(uint32_t tti); 
  void  set_tx_time(srslte_timestamp_t tx_time);
  void  set_cfo(float cfo);
  
  void  set_ul_params();
  void  set_crnti(uint16_t rnti);
  void  enable_pregen_signals(bool enabled);
  
private: 
  /* Inherited from thread_pool::worker. Function called every subframe to run the DL/UL processing */
  void work_imp();

  
  /* Internal methods */
  bool extract_fft_and_pdcch_llr(); 
  
  /* ... for DL */
  bool decode_pdcch_ul(mac_interface_phy::mac_grant_t *grant);
  bool decode_pdcch_dl(mac_interface_phy::mac_grant_t *grant);
  bool decode_phich(bool *ack); 
  bool decode_pdsch(srslte_ra_dl_grant_t *grant, uint8_t *payload, srslte_softbuffer_rx_t* softbuffer, uint32_t rv);

  /* ... for UL */
  void encode_pusch(srslte_ra_ul_grant_t *grant, uint32_t rv_idx, uint32_t current_tx_nb, srslte_softbuffer_tx_t *softbuffer);
  void encode_pucch();
  void encode_srs();
  void reset_uci();
  void set_uci_sr();
  void set_uci_cqi();
  void set_uci_ack(bool ack);
  bool srs_is_ready_to_send();
  void normalize();
  
  /* Common objects */  
  phch_common    *phy;
  srslte_cell_t  cell; 
  bool           cell_initiated; 
  cf_t          *signal_buffer; 
  uint32_t       tti; 
  bool           pregen_enabled;
  uint32_t       last_dl_pdcch_ncce;
  
  /* Objects for DL */
  srslte_ue_dl_t ue_dl; 
  uint32_t       cfi; 
  uint16_t       dl_rnti;
  
  /* Objects for UL */
  srslte_ue_ul_t     ue_ul; 
  srslte_timestamp_t tx_time; 
  srslte_uci_data_t  uci_data; 
  uint16_t           ul_rnti;
  uint8_t           *ul_payload;

  // UL configuration parameters 
  srslte_refsignal_srs_cfg_t        srs_cfg;           
  srslte_pucch_cfg_t                pucch_cfg; 
  srslte_refsignal_dmrs_pusch_cfg_t dmrs_cfg; 
  srslte_pusch_hopping_cfg_t        pusch_hopping; 
  srslte_pucch_sched_t              pucch_sched; 
  srslte_uci_cfg_t                  uci_cfg; 
  srslte_cqi_cfg_t                  cqi_cfg; 
  uint32_t                          I_sr; 
  float                             cfo;
  bool                              rar_cqi_request;
  
  
};
}
}

#endif

