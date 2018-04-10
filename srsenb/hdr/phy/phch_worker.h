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

#ifndef SRSENB_PHCH_WORKER_H
#define SRSENB_PHCH_WORKER_H

#include <string.h>

#include "srslte/srslte.h"
#include "phch_common.h"

#define LOG_EXECTIME

namespace srsenb {

class phch_worker : public srslte::thread_pool::worker
{
public:
  
  phch_worker();
  void  init(phch_common *phy, srslte::log *log_h);
  void  stop();
  void  reset();
  
  cf_t *get_buffer_rx(uint32_t antenna_idx);
  void set_time(uint32_t tti, uint32_t tx_mutex_cnt, srslte_timestamp_t tx_time);
  
  int  add_rnti(uint16_t rnti);
  void rem_rnti(uint16_t rnti);
  uint32_t get_nof_rnti(); 
  
  /* These are used by the GUI plotting tools */
  int read_ce_abs(float *ce_abs);
  int read_ce_arg(float *ce_abs);
  int read_pusch_d(cf_t *pusch_d);
  int read_pucch_d(cf_t *pusch_d);
  void start_plot();

  void set_conf_dedicated_ack(uint16_t rnti,
                         bool rrc_completed);
  
  void set_config_dedicated(uint16_t rnti, 
                            srslte_refsignal_srs_cfg_t *srs_cfg, 
                            LIBLTE_RRC_PHYSICAL_CONFIG_DEDICATED_STRUCT* dedicated);
  
  uint32_t get_metrics(phy_metrics_t metrics[ENB_METRICS_MAX_USERS]);
  
private: 
  
  const static float PUSCH_RL_SNR_DB_TH = 1.0; 
  const static float PUCCH_RL_CORR_TH = 0.1; 
  
  void work_imp();
  
  int encode_pdsch(srslte_enb_dl_pdsch_t *grants, uint32_t nof_grants);
  int decode_pusch(srslte_enb_ul_pusch_t *grants, uint32_t nof_pusch);
  int encode_phich(srslte_enb_dl_phich_t *acks, uint32_t nof_acks);
  int encode_pdcch_dl(srslte_enb_dl_pdsch_t *grants, uint32_t nof_grants);
  int encode_pdcch_ul(srslte_enb_ul_pusch_t *grants, uint32_t nof_grants);
  int decode_pucch();
  
  
  /* Common objects */  
  srslte::log    *log_h; 
  phch_common    *phy;
  bool           initiated;
  bool           running;

  cf_t          *signal_buffer_rx[SRSLTE_MAX_PORTS];
  cf_t          *signal_buffer_tx[SRSLTE_MAX_PORTS];
  uint32_t       tti_rx, tti_tx_dl, tti_tx_ul;
  uint32_t       sf_rx, sf_tx, tx_mutex_cnt;
  uint32_t       t_rx, t_tx_dl, t_tx_ul;
  srslte_enb_dl_t enb_dl;
  srslte_enb_ul_t enb_ul;
  
  srslte_timestamp_t tx_time;

  // Class to store user information 
  class ue {
  public:
    ue() : I_sr(0), I_sr_en(false), cqi_en(false), pucch_cqi_ack(false), pmi_idx(0), has_grant_tti(0),
           dedicated_ack(false), ri_idx(0), ri_en(false), rnti(0) {
      bzero(&dedicated, sizeof(LIBLTE_RRC_PHYSICAL_CONFIG_DEDICATED_STRUCT));
      bzero(&phich_info, sizeof(srslte_enb_ul_phich_info_t));
      bzero(&metrics, sizeof(phy_metrics_t));
    }
    uint32_t I_sr; 
    uint32_t pmi_idx;
    uint32_t ri_idx;
    bool I_sr_en; 
    bool cqi_en;
    bool ri_en;
    bool pucch_cqi_ack; 
    int has_grant_tti; 
    LIBLTE_RRC_PHYSICAL_CONFIG_DEDICATED_STRUCT dedicated;
    bool dedicated_ack;
    uint32_t rnti; 
    srslte_enb_ul_phich_info_t phich_info;
    void metrics_read(phy_metrics_t *metrics);
    void metrics_dl(uint32_t mcs);
    void metrics_ul(uint32_t mcs, float rssi, float sinr, uint32_t turbo_iters);

    int last_dl_tbs[2*HARQ_DELAY_MS][SRSLTE_MAX_CODEWORDS];

  private:
    phy_metrics_t metrics; 
  }; 
  std::map<uint16_t,ue> ue_db;   
  
  // mutex to protect worker_imp() from configuration interface 
  pthread_mutex_t mutex; 
};

} // namespace srsenb

#endif // SRSENB_PHCH_WORKER_H

