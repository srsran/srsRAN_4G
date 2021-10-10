/**
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
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
#include "srsran/adt/circular_array.h"
#include "srsran/common/gen_mch_tables.h"
#include "srsran/common/tti_sempahore.h"
#include "srsran/interfaces/phy_common_interface.h"
#include "srsran/interfaces/phy_interface_types.h"
#include "srsran/interfaces/radio_interfaces.h"
#include "srsran/interfaces/rrc_interface_types.h"
#include "srsran/interfaces/ue_phy_interfaces.h"
#include "srsran/radio/radio.h"
#include "srsran/srslog/srslog.h"
#include "srsran/srsran.h"
#include "srsue/hdr/phy/scell/scell_state.h"
#include "ta_control.h"
#include <condition_variable>
#include <mutex>
#include <string.h>
#include <vector>

namespace srsue {

class stack_interface_phy_lte;

class rsrp_insync_itf
{
public:
  virtual void in_sync() = 0;

  virtual void out_of_sync() = 0;

  virtual void set_cfo(float cfo) = 0;
};

/* Subclass that manages variables common to all workers */
class phy_common : public srsran::phy_common_interface
{
public:
  /* Common variables used by all phy workers */
  phy_args_t*              args  = nullptr;
  stack_interface_phy_lte* stack = nullptr;

  srsran::phy_cfg_mbsfn_t mbsfn_config = {};

  std::atomic<bool> cell_is_selecting = {false};

  // Secondary serving cell states
  scell::state cell_state;

  // Save last TBS for uplink (mcs >= 28)
  srsran_ra_tb_t last_ul_tb[SRSRAN_MAX_HARQ_PROC][SRSRAN_MAX_CARRIERS] = {};

  // Save last TBS for DL (Format1C)
  int last_dl_tbs[SRSRAN_MAX_HARQ_PROC][SRSRAN_MAX_CARRIERS][SRSRAN_MAX_CODEWORDS] = {};

  srsran::tti_semaphore<void*> semaphore;

  // Time Aligment Controller, internal thread safe
  ta_control ta;

  // Last reported RI
  std::atomic<uint32_t> last_ri = {0};

  phy_common(srslog::basic_logger& logger);

  ~phy_common();

  void init(phy_args_t*                  args,
            srsran::radio_interface_phy* _radio,
            stack_interface_phy_lte*     _stack,
            rsrp_insync_itf*             rsrp_insync);

  uint32_t ul_pidof(uint32_t tti, srsran_tdd_config_t* tdd_config);

  // Set configurations for lib objects
  void set_ue_dl_cfg(srsran_ue_dl_cfg_t* ue_dl_cfg);
  void set_ue_ul_cfg(srsran_ue_ul_cfg_t* ue_ul_cfg);
  void set_pdsch_cfg(srsran_pdsch_cfg_t* pdsch_cfg);

  void set_rar_grant(uint8_t grant_payload[SRSRAN_RAR_GRANT_LEN], uint16_t rnti, srsran_tdd_config_t tdd_config);

  void set_dl_pending_grant(uint32_t tti, uint32_t cc_idx, uint32_t grant_cc_idx, const srsran_dci_dl_t* dl_dci);
  bool get_dl_pending_grant(uint32_t tti, uint32_t cc_idx, uint32_t* grant_cc_idx, srsran_dci_dl_t* dl_dci);

  void set_ul_pending_ack(srsran_ul_sf_cfg_t*  sf,
                          uint32_t             cc_idx,
                          srsran_phich_grant_t phich_grant,
                          srsran_dci_ul_t*     dci_ul);
  bool get_ul_pending_ack(srsran_dl_sf_cfg_t*   sf,
                          uint32_t              cc_idx,
                          srsran_phich_grant_t* phich_grant,
                          srsran_dci_ul_t*      dci_ul);
  bool is_any_ul_pending_ack();

  bool get_ul_received_ack(srsran_ul_sf_cfg_t* sf, uint32_t cc_idx, bool* ack_value, srsran_dci_ul_t* dci_ul);
  void set_ul_received_ack(srsran_dl_sf_cfg_t* sf,
                           uint32_t            cc_idx,
                           bool                ack_value,
                           uint32_t            I_phich,
                           srsran_dci_ul_t*    dci_ul);

  void set_ul_pending_grant(srsran_dl_sf_cfg_t* sf, uint32_t cc_idx, srsran_dci_ul_t* dci);
  bool get_ul_pending_grant(srsran_ul_sf_cfg_t* sf, uint32_t cc_idx, uint32_t* pid, srsran_dci_ul_t* dci);

  /**
   * If there is a UL Grant it returns the lowest index component carrier that has a grant, otherwise it returns 0.
   *
   * @param tti_tx TTI in which the transmission is happening
   * @return The number of carrier if a grant is available, otherwise 0
   */
  uint32_t get_ul_uci_cc(uint32_t tti_tx) const;

  void set_rar_grant_tti(uint32_t tti);

  void set_dl_pending_ack(srsran_dl_sf_cfg_t*         sf,
                          uint32_t                    cc_idx,
                          uint8_t                     value[SRSRAN_MAX_CODEWORDS],
                          srsran_pdsch_ack_resource_t resource);
  bool get_dl_pending_ack(srsran_ul_sf_cfg_t* sf, uint32_t cc_idx, srsran_pdsch_ack_cc_t* ack);

  void worker_end(const worker_context_t& w_ctx, const bool& tx_enable, srsran::rf_buffer_t& buffer) override;

  void set_cell(const srsran_cell_t& c);

  class sr_signal
  {
  public:
    void reset()
    {
      std::lock_guard<std::mutex> lock(mutex);
      enabled     = false;
      last_tx_tti = -1;
    }
    bool is_triggered()
    {
      std::lock_guard<std::mutex> lock(mutex);
      return enabled;
    }
    void trigger()
    {
      std::lock_guard<std::mutex> lock(mutex);
      enabled     = true;
      last_tx_tti = -1;
    }
    int get_last_tx_tti()
    {
      std::lock_guard<std::mutex> lock(mutex);
      return last_tx_tti;
    }
    bool set_last_tx_tti(int last_tx_tti_)
    {
      std::lock_guard<std::mutex> lock(mutex);
      if (enabled) {
        enabled     = false;
        last_tx_tti = last_tx_tti_;
        return true;
      }
      return false;
    }

  private:
    std::mutex mutex;
    bool       enabled     = false;
    int        last_tx_tti = -1;
  };
  sr_signal sr;

  srsran::radio_interface_phy* get_radio();

  void set_dl_metrics(uint32_t cc_idx, const dl_metrics_t& m);
  void get_dl_metrics(dl_metrics_t::array_t& m);

  void set_ch_metrics(uint32_t cc_idx, const ch_metrics_t& m);
  void get_ch_metrics(ch_metrics_t::array_t& m);

  void set_ul_metrics(uint32_t cc_idx, const ul_metrics_t& m);
  void get_ul_metrics(ul_metrics_t::array_t& m);

  void set_sync_metrics(const uint32_t& cc_idx, const sync_metrics_t& m);
  void get_sync_metrics(sync_metrics_t::array_t& m);

  void reset();
  void reset_radio();

  void build_mch_table();
  void build_mcch_table();
  void set_mcch();
  bool is_mbsfn_sf(srsran_mbsfn_cfg_t* cfg, uint32_t tti);
  void set_mch_period_stop(uint32_t stop);

  /**
   * Deduces the UL EARFCN from a DL EARFCN. If the UL-EARFCN was defined in the UE PHY arguments it will use the
   * corresponding UL-EARFCN to the DL-EARFCN. Otherwise, it will use default.
   *
   * @param dl_earfcn
   * @return the deduced UL EARFCN
   */
  uint32_t get_ul_earfcn(uint32_t dl_earfcn);

  /**
   * @brief Resets measurements from a given CC
   * @param cc_idx CC index
   */
  void reset_measurements(uint32_t cc_idx);

  void update_measurements(uint32_t                     cc_idx,
                           const srsran_chest_dl_res_t& chest_res,
                           srsran_dl_sf_cfg_t           sf_cfg_dl,
                           float                        tx_crs_power,
                           std::vector<phy_meas_t>&     serving_cells,
                           cf_t*                        rssi_power_buffer = nullptr);

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

  void neighbour_cells_reset(uint32_t cc_idx)
  {
    std::unique_lock<std::mutex> lock(meas_mutex);
    avg_rsrp_neigh[cc_idx] = NAN;
  }

  void set_neighbour_cells(uint32_t cc_idx, const std::vector<phy_meas_t>& meas)
  {
    // Add RSRP in the linear domain and average
    float total_rsrp = 0;
    for (auto& m : meas) {
      total_rsrp += srsran_convert_dB_to_power(m.rsrp);
    }
    if (std::isnormal(total_rsrp)) {
      std::unique_lock<std::mutex> lock(meas_mutex);
      if (std::isnormal(avg_rsrp_neigh[cc_idx])) {
        avg_rsrp_neigh[cc_idx] = SRSRAN_VEC_EMA(total_rsrp, avg_rsrp_neigh[cc_idx], 0.9);
      } else {
        avg_rsrp_neigh[cc_idx] = total_rsrp;
      }
    }
  }

private:
  std::mutex meas_mutex;

  float                                  cur_pathloss    = 0.0f;
  float                                  cur_pusch_power = 0.0f;
  float                                  rx_gain_offset  = 0.0f;
  std::array<float, SRSRAN_MAX_CARRIERS> pathloss        = {};
  std::array<float, SRSRAN_MAX_CARRIERS> avg_rsrp        = {};
  std::array<float, SRSRAN_MAX_CARRIERS> avg_rsrp_dbm    = {};
  std::array<float, SRSRAN_MAX_CARRIERS> avg_rsrq_db     = {};
  std::array<float, SRSRAN_MAX_CARRIERS> avg_rssi_dbm    = {};
  std::array<float, SRSRAN_MAX_CARRIERS> avg_cfo_hz      = {};
  std::array<float, SRSRAN_MAX_CARRIERS> avg_sinr_db     = {};
  std::array<float, SRSRAN_MAX_CARRIERS> avg_snr_db      = {};
  std::array<float, SRSRAN_MAX_CARRIERS> avg_noise       = {};
  std::array<float, SRSRAN_MAX_CARRIERS> avg_rsrp_neigh  = {};

  static constexpr uint32_t pcell_report_period = 20;
  uint32_t                  rssi_read_cnt       = 0;

  rsrp_insync_itf* insync_itf = nullptr;

  bool                    have_mtch_stop = false;
  std::mutex              mtch_mutex;
  std::condition_variable mtch_cvar;

  std::atomic<bool> is_pending_tx_end{false};

  srsran::radio_interface_phy* radio_h = nullptr;
  srslog::basic_logger&        logger;
  srsran::channel_ptr          ul_channel = nullptr;

  int rar_grant_tti = -1;

  typedef struct {
    bool                 enable;
    srsran_phich_grant_t phich_grant;
    srsran_dci_ul_t      dci_ul;
  } pending_ul_ack_t;
  srsran::circular_array<pending_ul_ack_t, TTIMOD_SZ> pending_ul_ack[SRSRAN_MAX_CARRIERS][2] = {};
  std::mutex                                          pending_ul_ack_mutex;

  typedef struct {
    bool            hi_value;
    bool            hi_present;
    srsran_dci_ul_t dci_ul;
  } received_ul_ack_t;
  srsran::circular_array<received_ul_ack_t, TTIMOD_SZ> received_ul_ack[SRSRAN_MAX_CARRIERS] = {};
  std::mutex                                           received_ul_ack_mutex;

  typedef struct {
    bool            enable;
    uint32_t        pid;
    srsran_dci_ul_t dci;
  } pending_ul_grant_t;
  srsran::circular_array<pending_ul_grant_t, TTIMOD_SZ> pending_ul_grant[SRSRAN_MAX_CARRIERS] = {};
  mutable std::mutex                                    pending_ul_grant_mutex;

  typedef struct {
    bool                        enable;
    uint8_t                     value[SRSRAN_MAX_CODEWORDS]; // 0/1 or 2 for DTX
    srsran_pdsch_ack_resource_t resource;
  } received_ack_t;
  srsran::circular_array<received_ack_t, TTIMOD_SZ> pending_dl_ack[SRSRAN_MAX_CARRIERS] = {};
  srsran::circular_array<uint32_t, TTIMOD_SZ>       pending_dl_dai[SRSRAN_MAX_CARRIERS] = {};
  std::mutex                                        pending_dl_ack_mutex;
  std::mutex                                        pending_dl_grant_mutex;

  // Cross-carried grants scheduled from PCell
  typedef struct {
    bool            enable;
    uint32_t        grant_cc_idx;
    srsran_dci_dl_t dl_dci;
  } pending_dl_grant_t;
  pending_dl_grant_t pending_dl_grant[FDD_HARQ_DELAY_UL_MS][SRSRAN_MAX_CARRIERS] = {};

  srsran_cell_t cell = {};

  std::mutex metrics_mutex;

  ch_metrics_t::array_t   ch_metrics   = {};
  dl_metrics_t::array_t   dl_metrics   = {};
  ul_metrics_t::array_t   ul_metrics   = {};
  sync_metrics_t::array_t sync_metrics = {};

  // MBSFN
  bool     sib13_configured = false;
  bool     mcch_configured  = false;
  uint32_t mch_period_stop  = 0;
  uint8_t  mch_table[40]    = {};
  uint8_t  mcch_table[10]   = {};

  bool is_mch_subframe(srsran_mbsfn_cfg_t* cfg, uint32_t phy_tti);

  bool is_mcch_subframe(srsran_mbsfn_cfg_t* cfg, uint32_t phy_tti);

  // NR carriers buffering synchronization, LTE workers are in charge of transmitting
  bool                tx_enabled = false;
  srsran::rf_buffer_t tx_buffer  = {};
};
} // namespace srsue

#endif // SRSUE_PDCH_COMMON_H
