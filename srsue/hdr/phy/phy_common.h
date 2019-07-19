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

#ifndef SRSUE_PHCH_COMMON_H
#define SRSUE_PHCH_COMMON_H

#define TX_MODE_CONTINUOUS 1

#include "phy_metrics.h"
#include "srslte/common/gen_mch_tables.h"
#include "srslte/common/log.h"
#include "srslte/interfaces/common_interfaces.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "srslte/radio/radio.h"
#include "srslte/srslte.h"
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <vector>

namespace srsue {

class chest_feedback_itf
{
public:
  virtual void in_sync() = 0;

  virtual void out_of_sync() = 0;

  virtual void set_cfo(float cfo) = 0;
};

/* Subclass that manages variables common to all workers */
class phy_common
{
public:
  /* Common variables used by all phy workers */
  phy_args_t*        args;
  stack_interface_phy_lte* stack;

  phy_interface_rrc_lte::phy_cfg_mbsfn_t mbsfn_config;

  /* Power control variables */
  float pathloss[SRSLTE_MAX_CARRIERS];
  float cur_pathloss;
  float p0_preamble;
  float cur_radio_power;
  float cur_pusch_power;
  float avg_rsrp[SRSLTE_MAX_CARRIERS];
  float avg_rsrp_dbm[SRSLTE_MAX_CARRIERS];
  float avg_rsrq_db;
  float avg_rssi_dbm;
  float rx_gain_offset;
  float avg_snr_db_cqi[SRSLTE_MAX_CARRIERS];
  float avg_snr_db_sync;

  float avg_noise[SRSLTE_MAX_CARRIERS];
  bool  pcell_meas_enabled;

  uint32_t pcell_report_period;
  bool     pcell_first_measurement;

  // SCell EARFCN, PCI, configured and enabled list
  typedef struct {
    uint32_t earfcn     = 0;
    uint32_t pci        = 0;
    bool     configured = false;
    bool     enabled    = false;
  } scell_cfg_t;
  scell_cfg_t scell_cfg[SRSLTE_MAX_CARRIERS];

  bool multiple_csi_request_enabled;      /* True means cross scheduling enabled */
  bool cif_enabled;                       /* True means cross scheduling enabled */
  bool srs_request_enabled;

  // Save last TBS for uplink (mcs >= 28)
  srslte_ra_tb_t last_ul_tb[SRSLTE_MAX_HARQ_PROC][SRSLTE_MAX_CARRIERS];

  // Save last TBS for DL (Format1C)
  int last_dl_tbs[SRSLTE_MAX_HARQ_PROC][SRSLTE_MAX_CARRIERS][SRSLTE_MAX_CODEWORDS];

  phy_common(uint32_t max_workers);

  ~phy_common();

  void init(phy_args_t* args, srslte::log* _log, srslte::radio_interface_phy* _radio, stack_interface_phy_lte* _stack);

  uint32_t ul_pidof(uint32_t tti, srslte_tdd_config_t* tdd_config);

  // Set configurations for lib objects
  void set_ue_dl_cfg(srslte_ue_dl_cfg_t* ue_dl_cfg);
  void set_ue_ul_cfg(srslte_ue_ul_cfg_t* ue_ul_cfg);
  void set_pdsch_cfg(srslte_pdsch_cfg_t* pdsch_cfg);

  void set_rar_grant(uint8_t grant_payload[SRSLTE_RAR_GRANT_LEN], uint16_t rnti, srslte_tdd_config_t tdd_config);

  void set_ul_pending_ack(srslte_ul_sf_cfg_t*  sf,
                          uint32_t             cc_idx,
                          srslte_phich_grant_t phich_grant,
                          srslte_dci_ul_t*     dci_ul);
  bool get_ul_pending_ack(srslte_dl_sf_cfg_t*   sf,
                          uint32_t              cc_idx,
                          srslte_phich_grant_t* phich_grant,
                          srslte_dci_ul_t*      dci_ul);
  bool is_any_ul_pending_ack();

  bool get_ul_received_ack(srslte_ul_sf_cfg_t* sf, uint32_t cc_idx, bool* ack_value, srslte_dci_ul_t* dci_ul);
  void set_ul_received_ack(
      srslte_dl_sf_cfg_t* sf, uint32_t cc_idx, bool ack_value, uint32_t I_phich, srslte_dci_ul_t* dci_ul);

  void set_ul_pending_grant(srslte_dl_sf_cfg_t* sf, uint32_t cc_idx, srslte_dci_ul_t* dci);
  bool get_ul_pending_grant(srslte_ul_sf_cfg_t* sf, uint32_t cc_idx, uint32_t* pid, srslte_dci_ul_t* dci);

  void set_rar_grant_tti(uint32_t tti);

  void set_dl_pending_ack(srslte_dl_sf_cfg_t*         sf,
                          uint32_t                    cc_idx,
                          uint8_t                     value[SRSLTE_MAX_CODEWORDS],
                          srslte_pdsch_ack_resource_t resource);
  bool get_dl_pending_ack(srslte_ul_sf_cfg_t* sf, uint32_t cc_idx, srslte_pdsch_ack_cc_t* ack);

  void worker_end(uint32_t           tti,
                  bool               tx_enable,
                  cf_t*              buffer[SRSLTE_MAX_RADIOS][SRSLTE_MAX_PORTS],
                  uint32_t           nof_samples[SRSLTE_MAX_RADIOS],
                  srslte_timestamp_t tx_time[SRSLTE_MAX_RADIOS]);

  void set_nof_workers(uint32_t nof_workers);

  bool sr_enabled;
  int  sr_last_tx_tti;

  srslte::radio_interface_phy* get_radio();

  void     set_cell(const srslte_cell_t& c);
  uint32_t get_nof_prb();

  void set_dl_metrics(const dl_metrics_t m, uint32_t cc_idx);
  void get_dl_metrics(dl_metrics_t m[SRSLTE_MAX_CARRIERS]);
  void set_ul_metrics(const ul_metrics_t m, uint32_t cc_idx);
  void get_ul_metrics(ul_metrics_t m[SRSLTE_MAX_CARRIERS]);
  void set_sync_metrics(const uint32_t& cc_idx, const sync_metrics_t& m);
  void get_sync_metrics(sync_metrics_t m[SRSLTE_MAX_CARRIERS]);

  void reset();

  /* SCell Management */
  void enable_scell(uint32_t cc_idx, bool enable);

  void build_mch_table();
  void build_mcch_table();
  void set_mcch();
  bool is_mbsfn_sf(srslte_mbsfn_cfg_t* cfg, uint32_t tti);
  void set_mch_period_stop(uint32_t stop);

private:
  bool            have_mtch_stop;
  pthread_mutex_t mtch_mutex;
  pthread_cond_t  mtch_cvar;

  std::vector<sem_t> tx_sem;
  uint32_t           nof_workers;
  uint32_t           max_workers;

  bool           is_first_of_burst[SRSLTE_MAX_RADIOS];
  srslte::radio_interface_phy* radio_h;
  float          cfo;
  srslte::log*   log_h;
  srslte::channel_ptr  ul_channel = nullptr;

  int rar_grant_tti;

  typedef struct {
    bool                 enable;
    srslte_phich_grant_t phich_grant;
    srslte_dci_ul_t      dci_ul;
  } pending_ul_ack_t;
  pending_ul_ack_t pending_ul_ack[TTIMOD_SZ][SRSLTE_MAX_CARRIERS][2];
  pthread_mutex_t  pending_ul_ack_mutex;

  typedef struct {
    bool            hi_value;
    bool            hi_present;
    srslte_dci_ul_t dci_ul;
  } received_ul_ack_t;
  received_ul_ack_t received_ul_ack[TTIMOD_SZ][SRSLTE_MAX_CARRIERS];
  pthread_mutex_t   received_ul_ack_mutex;

  typedef struct {
    bool            enable;
    uint32_t        pid;
    srslte_dci_ul_t dci;
  } pending_ul_grant_t;
  pending_ul_grant_t pending_ul_grant[TTIMOD_SZ][SRSLTE_MAX_CARRIERS];
  pthread_mutex_t    pending_ul_grant_mutex;

  typedef struct {
    bool                        enable;
    uint8_t                     value[SRSLTE_MAX_CODEWORDS]; // 0/1 or 2 for DTX
    srslte_pdsch_ack_resource_t resource;
  } received_ack_t;
  received_ack_t  pending_dl_ack[TTIMOD_SZ][SRSLTE_MAX_CARRIERS];
  uint32_t        pending_dl_dai[TTIMOD_SZ][SRSLTE_MAX_CARRIERS];
  pthread_mutex_t pending_dl_ack_mutex;

  bool is_first_tx;

  srslte_cell_t cell;

  dl_metrics_t   dl_metrics[SRSLTE_MAX_CARRIERS];
  uint32_t       dl_metrics_count;
  bool           dl_metrics_read;
  ul_metrics_t   ul_metrics[SRSLTE_MAX_CARRIERS];
  uint32_t       ul_metrics_count;
  bool           ul_metrics_read;
  sync_metrics_t sync_metrics[SRSLTE_MAX_CARRIERS];
  uint32_t       sync_metrics_count;
  bool           sync_metrics_read;

  // MBSFN
  bool     sib13_configured;
  bool     mcch_configured;
  uint32_t mch_period_stop;
  uint8_t  mch_table[40];
  uint8_t  mcch_table[10];

  bool is_mch_subframe(srslte_mbsfn_cfg_t* cfg, uint32_t phy_tti);

  bool is_mcch_subframe(srslte_mbsfn_cfg_t* cfg, uint32_t phy_tti);
};
} // namespace srsue

#endif // SRSUE_PDCH_COMMON_H
