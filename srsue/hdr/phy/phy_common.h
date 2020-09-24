/*
 * Copyright 2013-2020 Software Radio Systems Limited
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

#include "phy_metrics.h"
#include "srslte/adt/circular_array.h"
#include "srslte/common/gen_mch_tables.h"
#include "srslte/common/log.h"
#include "srslte/common/tti_sempahore.h"
#include "srslte/interfaces/radio_interfaces.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "srslte/radio/radio.h"
#include "srslte/srslte.h"
#include "ta_control.h"
#include <condition_variable>
#include <mutex>
#include <string.h>
#include <vector>

namespace srsue {

class rsrp_insync_itf
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
  phy_args_t*              args  = nullptr;
  stack_interface_phy_lte* stack = nullptr;

  srslte::phy_cfg_mbsfn_t mbsfn_config = {};

  // SCell EARFCN, PCI, configured and enabled list
  typedef struct {
    uint32_t earfcn     = 0;
    uint32_t pci        = 0;
    bool     configured = false;
    bool     enabled    = false;
  } scell_cfg_t;
  scell_cfg_t scell_cfg[SRSLTE_MAX_CARRIERS];

  // Save last TBS for uplink (mcs >= 28)
  srslte_ra_tb_t last_ul_tb[SRSLTE_MAX_HARQ_PROC][SRSLTE_MAX_CARRIERS] = {};

  // Save last TBS for DL (Format1C)
  int last_dl_tbs[SRSLTE_MAX_HARQ_PROC][SRSLTE_MAX_CARRIERS][SRSLTE_MAX_CODEWORDS] = {};

  srslte::tti_semaphore<void*> semaphore;

  // Time Aligment Controller, internal thread safe
  ta_control ta;

  phy_common();

  ~phy_common();

  void init(phy_args_t*                  args,
            srslte::log*                 _log,
            srslte::radio_interface_phy* _radio,
            stack_interface_phy_lte*     _stack,
            rsrp_insync_itf*             rsrp_insync);

  uint32_t ul_pidof(uint32_t tti, srslte_tdd_config_t* tdd_config);

  // Set configurations for lib objects
  void set_ue_dl_cfg(srslte_ue_dl_cfg_t* ue_dl_cfg);
  void set_ue_ul_cfg(srslte_ue_ul_cfg_t* ue_ul_cfg);
  void set_pdsch_cfg(srslte_pdsch_cfg_t* pdsch_cfg);

  void set_rar_grant(uint8_t grant_payload[SRSLTE_RAR_GRANT_LEN], uint16_t rnti, srslte_tdd_config_t tdd_config);

  void set_dl_pending_grant(uint32_t tti, uint32_t cc_idx, uint32_t grant_cc_idx, const srslte_dci_dl_t* dl_dci);
  bool get_dl_pending_grant(uint32_t tti, uint32_t cc_idx, uint32_t* grant_cc_idx, srslte_dci_dl_t* dl_dci);

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
  void set_ul_received_ack(srslte_dl_sf_cfg_t* sf,
                           uint32_t            cc_idx,
                           bool                ack_value,
                           uint32_t            I_phich,
                           srslte_dci_ul_t*    dci_ul);

  void set_ul_pending_grant(srslte_dl_sf_cfg_t* sf, uint32_t cc_idx, srslte_dci_ul_t* dci);
  bool get_ul_pending_grant(srslte_ul_sf_cfg_t* sf, uint32_t cc_idx, uint32_t* pid, srslte_dci_ul_t* dci);

  /**
   * If there is a UL Grant it returns the lowest index component carrier that has a grant, otherwise it returns 0.
   *
   * @param tti_tx TTI in which the transmission is happening
   * @return The number of carrier if a grant is available, otherwise 0
   */
  uint32_t get_ul_uci_cc(uint32_t tti_tx) const;

  void set_rar_grant_tti(uint32_t tti);

  void set_dl_pending_ack(srslte_dl_sf_cfg_t*         sf,
                          uint32_t                    cc_idx,
                          uint8_t                     value[SRSLTE_MAX_CODEWORDS],
                          srslte_pdsch_ack_resource_t resource);
  bool get_dl_pending_ack(srslte_ul_sf_cfg_t* sf, uint32_t cc_idx, srslte_pdsch_ack_cc_t* ack);

  void worker_end(void* h, bool tx_enable, srslte::rf_buffer_t& buffer, srslte::rf_timestamp_t& tx_time);

  void set_cell(const srslte_cell_t& c);
  void set_nof_workers(uint32_t nof_workers);

  bool sr_enabled     = false;
  int  sr_last_tx_tti = -1;

  srslte::radio_interface_phy* get_radio();

  void set_dl_metrics(uint32_t cc_idx, const dl_metrics_t& m);
  void get_dl_metrics(dl_metrics_t m[SRSLTE_MAX_CARRIERS]);

  void set_ch_metrics(uint32_t cc_idx, const ch_metrics_t& m);
  void get_ch_metrics(ch_metrics_t m[SRSLTE_MAX_CARRIERS]);

  void set_ul_metrics(uint32_t cc_idx, const ul_metrics_t& m);
  void get_ul_metrics(ul_metrics_t m[SRSLTE_MAX_CARRIERS]);

  void set_sync_metrics(const uint32_t& cc_idx, const sync_metrics_t& m);
  void get_sync_metrics(sync_metrics_t m[SRSLTE_MAX_CARRIERS]);

  void reset();
  void reset_radio();

  /* SCell Management */
  void enable_scell(uint32_t cc_idx, bool enable);

  void build_mch_table();
  void build_mcch_table();
  void set_mcch();
  bool is_mbsfn_sf(srslte_mbsfn_cfg_t* cfg, uint32_t tti);
  void set_mch_period_stop(uint32_t stop);

  /**
   * Deduces the UL EARFCN from a DL EARFCN. If the UL-EARFCN was defined in the UE PHY arguments it will use the
   * corresponding UL-EARFCN to the DL-EARFCN. Otherwise, it will use default.
   *
   * @param dl_earfcn
   * @return the deduced UL EARFCN
   */
  uint32_t get_ul_earfcn(uint32_t dl_earfcn);

  void update_measurements(uint32_t                                        cc_idx,
                           srslte_chest_dl_res_t                           chest_res,
                           srslte_dl_sf_cfg_t                              sf_cfg_dl,
                           float                                           tx_crs_power,
                           std::vector<rrc_interface_phy_lte::phy_meas_t>& serving_cells,
                           cf_t*                                           rssi_power_buffer = nullptr);

  void update_cfo_measurement(uint32_t cc_idx, float cfo_hz);

  float get_sinr_db(uint32_t cc_idx)
  {
    std::unique_lock<std::mutex> lock(meas_mutex);
    return avg_sinr_db[cc_idx];
  }

  float get_pusch_power()
  {
    std::unique_lock<std::mutex> lock(meas_mutex);
    return cur_pusch_power;
  }

  float get_pathloss()
  {
    std::unique_lock<std::mutex> lock(meas_mutex);
    return cur_pathloss;
  }

  float get_rx_gain_offset()
  {
    std::unique_lock<std::mutex> lock(meas_mutex);
    return rx_gain_offset;
  }

  void neighbour_cells_reset(uint32_t cc_idx) { avg_rsrp_neigh[cc_idx] = NAN; }

  void set_neighbour_cells(uint32_t cc_idx, const std::vector<rrc_interface_phy_lte::phy_meas_t>& meas)
  {
    // Add RSRP in the linear domain and average
    float total_rsrp = 0;
    for (auto& m : meas) {
      total_rsrp += srslte_convert_dB_to_power(m.rsrp);
    }
    if (std::isnormal(total_rsrp)) {
      if (std::isnormal(avg_rsrp_neigh[cc_idx])) {
        avg_rsrp_neigh[cc_idx] = SRSLTE_VEC_EMA(total_rsrp, avg_rsrp_neigh[cc_idx], 0.9);
      } else {
        avg_rsrp_neigh[cc_idx] = total_rsrp;
      }
    }
  }
  void reset_neighbour_cells()
  {
    for (uint32_t i = 0; i < SRSLTE_MAX_CARRIERS; i++) {
      avg_rsrp_neigh[i] = NAN;
    }
  }

private:
  std::mutex meas_mutex;

  float    pathloss[SRSLTE_MAX_CARRIERS]       = {};
  float    cur_pathloss                        = 0.0f;
  float    cur_pusch_power                     = 0.0f;
  float    avg_rsrp[SRSLTE_MAX_CARRIERS]       = {};
  float    avg_rsrp_dbm[SRSLTE_MAX_CARRIERS]   = {};
  float    avg_rsrq_db[SRSLTE_MAX_CARRIERS]    = {};
  float    avg_rssi_dbm[SRSLTE_MAX_CARRIERS]   = {};
  float    avg_cfo_hz[SRSLTE_MAX_CARRIERS]     = {};
  float    rx_gain_offset                      = 0.0f;
  float    avg_sinr_db[SRSLTE_MAX_CARRIERS]    = {};
  float    avg_snr_db[SRSLTE_MAX_CARRIERS]     = {};
  float    avg_noise[SRSLTE_MAX_CARRIERS]      = {};
  float    avg_rsrp_neigh[SRSLTE_MAX_CARRIERS] = {};

  uint32_t pcell_report_period                 = 0;
  uint32_t rssi_read_cnt                       = 0;

  rsrp_insync_itf* insync_itf = nullptr;

  bool                    have_mtch_stop = false;
  std::mutex              mtch_mutex;
  std::condition_variable mtch_cvar;

  uint32_t nof_workers = 0;

  bool is_pending_tx_end = false;

  srslte::radio_interface_phy* radio_h    = nullptr;
  srslte::log*                 log_h      = nullptr;
  srslte::channel_ptr          ul_channel = nullptr;

  int rar_grant_tti = -1;

  typedef struct {
    bool                 enable;
    srslte_phich_grant_t phich_grant;
    srslte_dci_ul_t      dci_ul;
  } pending_ul_ack_t;
  srslte::circular_array<pending_ul_ack_t, TTIMOD_SZ> pending_ul_ack[SRSLTE_MAX_CARRIERS][2] = {};
  std::mutex                                          pending_ul_ack_mutex;

  typedef struct {
    bool            hi_value;
    bool            hi_present;
    srslte_dci_ul_t dci_ul;
  } received_ul_ack_t;
  srslte::circular_array<received_ul_ack_t, TTIMOD_SZ> received_ul_ack[SRSLTE_MAX_CARRIERS] = {};
  std::mutex                                           received_ul_ack_mutex;

  typedef struct {
    bool            enable;
    uint32_t        pid;
    srslte_dci_ul_t dci;
  } pending_ul_grant_t;
  srslte::circular_array<pending_ul_grant_t, TTIMOD_SZ> pending_ul_grant[SRSLTE_MAX_CARRIERS] = {};
  mutable std::mutex                                    pending_ul_grant_mutex;

  typedef struct {
    bool                        enable;
    uint8_t                     value[SRSLTE_MAX_CODEWORDS]; // 0/1 or 2 for DTX
    srslte_pdsch_ack_resource_t resource;
  } received_ack_t;
  srslte::circular_array<received_ack_t, TTIMOD_SZ> pending_dl_ack[SRSLTE_MAX_CARRIERS] = {};
  srslte::circular_array<uint32_t, TTIMOD_SZ>       pending_dl_dai[SRSLTE_MAX_CARRIERS] = {};
  std::mutex                                        pending_dl_ack_mutex;
  std::mutex                                        pending_dl_grant_mutex;

  // Cross-carried grants scheduled from PCell
  typedef struct {
    bool            enable;
    uint32_t        grant_cc_idx;
    srslte_dci_dl_t dl_dci;
  } pending_dl_grant_t;
  pending_dl_grant_t pending_dl_grant[FDD_HARQ_DELAY_UL_MS][SRSLTE_MAX_CARRIERS] = {};

  srslte_cell_t cell = {};

  std::mutex metrics_mutex;

  ch_metrics_t   ch_metrics[SRSLTE_MAX_CARRIERS]         = {};
  uint32_t       ch_metrics_count[SRSLTE_MAX_CARRIERS]   = {};
  dl_metrics_t   dl_metrics[SRSLTE_MAX_CARRIERS]         = {};
  uint32_t       dl_metrics_count[SRSLTE_MAX_CARRIERS]   = {};
  ul_metrics_t   ul_metrics[SRSLTE_MAX_CARRIERS]         = {};
  uint32_t       ul_metrics_count[SRSLTE_MAX_CARRIERS]   = {};
  sync_metrics_t sync_metrics[SRSLTE_MAX_CARRIERS]       = {};
  uint32_t       sync_metrics_count[SRSLTE_MAX_CARRIERS] = {};

  // MBSFN
  bool     sib13_configured = false;
  bool     mcch_configured  = false;
  uint32_t mch_period_stop  = 0;
  uint8_t  mch_table[40]    = {};
  uint8_t  mcch_table[10]   = {};

  bool is_mch_subframe(srslte_mbsfn_cfg_t* cfg, uint32_t phy_tti);

  bool is_mcch_subframe(srslte_mbsfn_cfg_t* cfg, uint32_t phy_tti);
};
} // namespace srsue

#endif // SRSUE_PDCH_COMMON_H
