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

#ifndef ENBPHYWORKER_H
#define ENBPHYWORKER_H

#include <string.h>

#include "srslte/srslte.h"
#include "phy/phch_common.h"

#define LOG_EXECTIME

namespace srsenb {

class phch_worker : public srslte::thread_pool::worker
{
public:
  
  phch_worker();
  void  init(phch_common *phy, srslte::log *log_h);
  void  stop();
  void  reset();
  
  cf_t *get_buffer_rx();
  void set_time(uint32_t tti, uint32_t tx_mutex_cnt, srslte_timestamp_t tx_time);
  
  int  add_rnti(uint16_t rnti);
  void rem_rnti(uint16_t rnti);
  uint32_t get_nof_rnti(); 
  
  /* These are used by the GUI plotting tools */
  int read_ce_abs(float *ce_abs);
  int read_pusch_d(cf_t *pusch_d);
  void start_plot();
  
  
  void set_config_dedicated(uint16_t rnti, 
                            srslte_uci_cfg_t *uci_cfg, 
                            srslte_pucch_sched_t *pucch_sched,
                            srslte_refsignal_srs_cfg_t *srs_cfg, 
                            uint32_t I_sr, bool pucch_cqi, uint32_t pmi_idx, bool pucch_cqi_ack);
  
  uint32_t get_metrics(phy_metrics_t metrics[ENB_METRICS_MAX_USERS]);
  
private: 
  
  const static float PUSCH_RL_SNR_DB_TH = 1.0; 
  const static float PUCCH_RL_CORR_TH = 0.1; 
  
  void work_imp();
  
  int encode_pdsch(srslte_enb_dl_pdsch_t *grants, uint32_t nof_grants, uint32_t sf_idx);
  int decode_pusch(srslte_enb_ul_pusch_t *grants, uint32_t nof_pusch, uint32_t tti_rx);
  int encode_phich(srslte_enb_dl_phich_t *acks, uint32_t nof_acks, uint32_t sf_idx);
  int encode_pdcch_dl(srslte_enb_dl_pdsch_t *grants, uint32_t nof_grants, uint32_t sf_idx);
  int encode_pdcch_ul(srslte_enb_ul_pusch_t *grants, uint32_t nof_grants, uint32_t sf_idx); 
  int decode_pucch(uint32_t tti_rx);
  
  
  /* Common objects */  
  srslte::log    *log_h; 
  phch_common    *phy;
  bool           initiated;
  bool           running;

  cf_t          *signal_buffer_rx; 
  cf_t          *signal_buffer_tx; 
  uint32_t       tti_rx, tti_tx, tti_sched_ul, sf_rx, sf_tx, sf_sched_ul, tx_mutex_cnt;

  srslte_enb_dl_t enb_dl;
  srslte_enb_ul_t enb_ul;
  
  srslte_timestamp_t tx_time; 

  // Class to store user information 
  class ue {
  public:
    ue() : I_sr(0), I_sr_en(false), cqi_en(false), pucch_cqi_ack(false), pmi_idx(0), has_grant_tti(0) {bzero(&metrics, sizeof(phy_metrics_t));}
    uint32_t I_sr; 
    uint32_t pmi_idx;
    bool I_sr_en; 
    bool cqi_en;
    bool pucch_cqi_ack; 
    int has_grant_tti; 
    uint32_t rnti; 
    srslte_enb_ul_phich_info_t phich_info;
    void metrics_read(phy_metrics_t *metrics);
    void metrics_dl(uint32_t mcs);
    void metrics_ul(uint32_t mcs, float rssi, float sinr, uint32_t turbo_iters);
  private:
    phy_metrics_t metrics; 
  }; 
  std::map<uint16_t,ue> ue_db;   
  
  // mutex to protect worker_imp() from configuration interface 
  pthread_mutex_t mutex; 
};

} // namespace srsenb

#endif // ENBPHYWORKER_H

