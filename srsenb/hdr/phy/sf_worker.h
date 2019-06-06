/*
 * Copyright 2013-2019 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
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

#ifndef SRSENB_PHCH_WORKER_H
#define SRSENB_PHCH_WORKER_H

#include <string.h>

#include "phy_common.h"
#include "srslte/srslte.h"

#define LOG_EXECTIME

namespace srsenb {

class sf_worker : public srslte::thread_pool::worker
{
public:
  sf_worker();
  void init(phy_common* phy, srslte::log* log_h);
  void stop();
  void reset();

  cf_t* get_buffer_rx(uint32_t antenna_idx);
  void  set_time(uint32_t tti, uint32_t tx_worker_cnt, srslte_timestamp_t tx_time);

  int      add_rnti(uint16_t rnti, bool is_temporal);
  void     rem_rnti(uint16_t rnti);
  uint32_t get_nof_rnti();

  /* These are used by the GUI plotting tools */
  int  read_ce_abs(float* ce_abs);
  int  read_ce_arg(float* ce_abs);
  int  read_pusch_d(cf_t* pusch_d);
  int  read_pucch_d(cf_t* pusch_d);
  void start_plot();

  void set_config_dedicated(uint16_t rnti, asn1::rrc::phys_cfg_ded_s* dedicated);

  uint32_t get_metrics(phy_metrics_t metrics[ENB_METRICS_MAX_USERS]);

private:
  constexpr static float PUSCH_RL_SNR_DB_TH = 1.0;
  constexpr static float PUCCH_RL_CORR_TH   = 0.15;

  void work_imp();

  int encode_pdsch(stack_interface_phy_lte::dl_sched_grant_t* grants, uint32_t nof_grants);
  int encode_pmch(stack_interface_phy_lte::dl_sched_grant_t* grant, srslte_mbsfn_cfg_t* mbsfn_cfg);
  int decode_pusch(stack_interface_phy_lte::ul_sched_grant_t* grants, uint32_t nof_pusch);
  int encode_phich(stack_interface_phy_lte::ul_sched_ack_t* acks, uint32_t nof_acks);
  int encode_pdcch_dl(stack_interface_phy_lte::dl_sched_grant_t* grants, uint32_t nof_grants);
  int encode_pdcch_ul(stack_interface_phy_lte::ul_sched_grant_t* grants, uint32_t nof_grants);
  int decode_pucch();

  void send_uci_data(uint16_t rnti, srslte_uci_cfg_t* uci_cfg, srslte_uci_value_t* uci_value);
  bool fill_uci_cfg(uint16_t rnti, bool aperiodic_cqi_request, srslte_uci_cfg_t* uci_cfg);

  /* Common objects */
  srslte::log* log_h;
  phy_common*  phy;
  bool         initiated;
  bool         running;

  cf_t*              signal_buffer_rx[SRSLTE_MAX_PORTS];
  cf_t*              signal_buffer_tx[SRSLTE_MAX_PORTS];
  uint32_t           tti_rx, tti_tx_dl, tti_tx_ul;
  uint32_t           t_rx, t_tx_dl, t_tx_ul;
  uint32_t           tx_worker_cnt;
  srslte_timestamp_t tx_time;

  srslte_enb_dl_t enb_dl;
  srslte_enb_ul_t enb_ul;

  srslte_dl_sf_cfg_t dl_sf;
  srslte_ul_sf_cfg_t ul_sf;

  srslte_softbuffer_tx_t temp_mbsfn_softbuffer;

  // Class to store user information
  class ue
  {
  public:
    ue(uint16_t rnti, phy_common* phy) : is_grant_available(false), rnti(0)
    {
      ZERO_OBJECT(phich_grant);
      ZERO_OBJECT(metrics);
      bzero(&metrics, sizeof(phy_metrics_t));

      // Copy common configuartion
      ul_cfg = phy->ul_cfg_com;
      dl_cfg = phy->dl_cfg_com;

      // Fill RNTI
      this->rnti        = rnti;
      dl_cfg.pdsch.rnti = rnti;
      ul_cfg.pusch.rnti = rnti;
      ul_cfg.pucch.rnti = rnti;
    }

    bool                 is_grant_available;
    srslte_phich_grant_t phich_grant;

    srslte_dl_cfg_t dl_cfg;
    srslte_ul_cfg_t ul_cfg;

    void metrics_read(phy_metrics_t* metrics);
    void metrics_dl(uint32_t mcs);
    void metrics_ul(uint32_t mcs, float rssi, float sinr, uint32_t turbo_iters);

  private:
    uint32_t      rnti;
    phy_metrics_t metrics;
  };

  // Each worker keeps a local copy of the user database. Uses more memory but more efficient to manage concurrency
  std::map<uint16_t, ue*> ue_db;

  // mutex to protect worker_imp() from configuration interface
  pthread_mutex_t mutex;
  bool            is_worker_running;
};

} // namespace srsenb

#endif // SRSENB_PHCH_WORKER_H
