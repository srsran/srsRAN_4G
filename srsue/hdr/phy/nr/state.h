/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#ifndef SRSRAN_STATE_H
#define SRSRAN_STATE_H

#include "../phy_metrics.h"
#include "srsran/adt/circular_array.h"
#include "srsran/common/common.h"
#include "srsran/interfaces/ue_nr_interfaces.h"
#include "srsran/srsran.h"
#include <array>
#include <mutex>
#include <vector>

namespace srsue {
namespace nr {

class state
{
private:
  struct pending_ul_grant_t {
    bool                enable;
    uint32_t            pid;
    srsran_sch_cfg_nr_t sch_cfg;
  };
  srsran::circular_array<pending_ul_grant_t, TTIMOD_SZ> pending_ul_grant = {};
  mutable std::mutex                                    pending_ul_grant_mutex;

  struct pending_dl_grant_t {
    bool                       enable;
    uint32_t                   pid;
    srsran_sch_cfg_nr_t        sch_cfg;
    srsran_harq_ack_resource_t ack_resource;
  };
  srsran::circular_array<pending_dl_grant_t, TTIMOD_SZ> pending_dl_grant = {};
  mutable std::mutex                                    pending_dl_grant_mutex;

  srsran::circular_array<srsran_pdsch_ack_nr_t, TTIMOD_SZ> pending_ack = {};
  mutable std::mutex                                       pending_ack_mutex;

  /// Metrics section
  info_metrics_t     info_metrics = {};
  sync_metrics_t     sync_metrics = {};
  ch_metrics_t       ch_metrics   = {};
  dl_metrics_t       dl_metrics   = {};
  ul_metrics_t       ul_metrics   = {};
  mutable std::mutex metrics_mutex;

  /// CSI-RS measurements
  std::mutex                                                                  csi_measurements_mutex;
  std::array<srsran_csi_channel_measurements_t, SRSRAN_CSI_MAX_NOF_RESOURCES> csi_measurements = {};

  /// TRS measurements
  mutable std::mutex            trs_measurements_mutex;
  srsran_csi_trs_measurements_t trs_measurements = {};

  /// Other measurements
  std::atomic<float> ul_ext_cfo_hz = {0.0f};

  /**
   * @brief Resets all metrics (unprotected)
   */
  void reset_metrics_()
  {
    sync_metrics.reset();
    ch_metrics.reset();
    dl_metrics.reset();
    ul_metrics.reset();
  }

public:
  mac_interface_phy_nr* stack = nullptr;

  /// Physical layer user configuration
  phy_args_nr_t args = {};

  /// Semaphore for aligning UL work
  srsran::tti_semaphore<void*> dl_ul_semaphore;

  state()
  {
    // Hard-coded values, this should be set when the measurements take place
    csi_measurements[0].K_csi_rs  = 1;
    csi_measurements[0].nof_ports = 1;
    csi_measurements[1].K_csi_rs  = 4;
    csi_measurements[1].nof_ports = 1;
  }

  /**
   * @brief Stores a received UL DCI into the pending UL grant list
   * @param cfg Physical layer configuration object
   * @param slot_rx The TTI in which the grant was received
   * @param dci_ul The UL DCI message to store
   */
  void set_ul_pending_grant(const srsran::phy_cfg_nr_t& cfg,
                            const srsran_slot_cfg_t&    slot_rx,
                            const srsran_dci_ul_nr_t&   dci_ul)
  {
    // Convert UL DCI to grant
    srsran_sch_cfg_nr_t pusch_cfg = {};
    if (not cfg.get_pusch_cfg(slot_rx, dci_ul, pusch_cfg)) {
      std::array<char, 512> str;
      srsran_dci_ul_nr_to_str(NULL, &dci_ul, str.data(), str.size());
      ERROR("Computing UL grant %s", str.data());
      return;
    }

    // Calculate Transmit TTI
    uint32_t tti_tx = TTI_ADD(slot_rx.idx, pusch_cfg.grant.k);

    // Scope mutex to protect read/write the list
    std::lock_guard<std::mutex> lock(pending_ul_grant_mutex);

    // Save entry
    pending_ul_grant_t& pending_grant = pending_ul_grant[tti_tx];
    pending_grant.sch_cfg             = pusch_cfg;
    pending_grant.pid                 = dci_ul.pid;
    pending_grant.enable              = true;
  }

  /**
   * @brief Checks the UL pending grant list if there is any grant to transmit for the given transmit TTI
   * @param tti_tx Current transmit TTI
   * @param sch_cfg Provides the Shared Channel configuration for the PUSCH transmission
   * @param pid Provides the HARQ process identifier
   * @return true if there is a pending grant for the given TX tti, false otherwise
   */
  bool get_ul_pending_grant(uint32_t tti_tx, srsran_sch_cfg_nr_t& pusch_cfg, uint32_t& pid)
  {
    // Scope mutex to protect read/write the list
    std::lock_guard<std::mutex> lock(pending_ul_grant_mutex);

    // Select entry
    pending_ul_grant_t& pending_grant = pending_ul_grant[tti_tx];

    // If the entry is not active, just return
    if (!pending_grant.enable) {
      return false;
    }

    // Load shared channel configuration and PID
    pusch_cfg = pending_grant.sch_cfg;
    pid       = pending_grant.pid;

    // Reset entry
    pending_grant.enable = false;

    return true;
  }

  /**
   * @brief Stores a received DL DCI into the pending DL grant list
   * @param cfg Physical layer configuration object
   * @param tti_rx The TTI in which the grant was received
   * @param dci_dl The DL DCI message to store
   */
  void
  set_dl_pending_grant(const srsran::phy_cfg_nr_t& cfg, const srsran_slot_cfg_t& slot, const srsran_dci_dl_nr_t& dci_dl)
  {
    // Convert DL DCI to grant
    srsran_sch_cfg_nr_t pdsch_cfg = {};

    if (not cfg.get_pdsch_cfg(slot, dci_dl, pdsch_cfg)) {
      std::array<char, 512> str;
      srsran_dci_dl_nr_to_str(NULL, &dci_dl, str.data(), str.size());
      ERROR("Computing DL grant %s", str.data());
      return;
    }

    // Calculate DL DCI to PDSCH ACK resource
    srsran_harq_ack_resource_t ack_resource = {};
    if (not cfg.get_pdsch_ack_resource(dci_dl, ack_resource)) {
      ERROR("Computing UL ACK resource");
      return;
    }

    // Calculate Receive TTI
    uint32_t tti_rx = TTI_ADD(slot.idx, pdsch_cfg.grant.k);

    // Scope mutex to protect read/write the list
    std::lock_guard<std::mutex> lock(pending_dl_grant_mutex);

    // Save entry
    pending_dl_grant_t& pending_grant = pending_dl_grant[tti_rx];
    pending_grant.sch_cfg             = pdsch_cfg;
    pending_grant.ack_resource        = ack_resource;
    pending_grant.pid                 = dci_dl.pid;
    pending_grant.enable              = true;
  }

  /**
   * @brief Checks the DL pending grant list if there is any grant to receive for the given receive TTI
   * @param tti_rx Current receive TTI
   * @param sch_cfg Provides the Shared Channel configuration for the PDSCH transmission
   * @param ack_resource Provides the UL ACK resource
   * @param pid Provides the HARQ process identifier
   * @return true if there is a pending grant for the given TX tti, false otherwise
   */
  bool get_dl_pending_grant(uint32_t                    tti_rx,
                            srsran_sch_cfg_nr_t&        pdsch_cfg,
                            srsran_harq_ack_resource_t& ack_resource,
                            uint32_t&                   pid)
  {
    // Scope mutex to protect read/write the list
    std::lock_guard<std::mutex> lock(pending_dl_grant_mutex);

    // Select entry
    pending_dl_grant_t& pending_grant = pending_dl_grant[tti_rx];

    // If the entry is not active, just return
    if (!pending_grant.enable) {
      return false;
    }

    // Load shared channel configuration and resource
    pdsch_cfg    = pending_grant.sch_cfg;
    ack_resource = pending_grant.ack_resource;
    pid          = pending_grant.pid;

    // Reset entry
    pending_grant.enable = false;

    return true;
  }

  /**
   * @brief Stores a pending PDSCH ACK into the pending ACK list
   * @param tti_rx The TTI in which the PDSCH transmission was received
   * @param dci_dl The DL DCI message to store
   */
  void set_pending_ack(const uint32_t& tti_rx, const srsran_harq_ack_resource_t& ack_resource, const bool& crc_ok)
  {
    // Calculate Receive TTI
    uint32_t tti_tx = TTI_ADD(tti_rx, ack_resource.k1);

    // Prepare ACK information
    srsran_harq_ack_m_t ack_m = {};
    ack_m.resource            = ack_resource;
    ack_m.value[0]            = crc_ok ? 1 : 0;
    ack_m.present             = true;

    // Scope mutex to protect read/write the list
    std::lock_guard<std::mutex> lock(pending_ack_mutex);

    // Select UL transmission time resource
    srsran_pdsch_ack_nr_t& ack = pending_ack[tti_tx];
    ack.nof_cc                 = 1;

    // Insert PDSCH transmission information
    if (srsran_harq_ack_insert_m(&ack, &ack_m) < SRSRAN_SUCCESS) {
      ERROR("Error inserting ACK m value for Tx slot %d", tti_tx);
    }
  }

  bool get_pending_ack(const uint32_t& tti_tx, srsran_pdsch_ack_nr_t& pdsch_ack)
  {
    // Scope mutex to protect read/write the list
    std::lock_guard<std::mutex> lock(pending_ack_mutex);

    // Select UL transmission time resource
    srsran_pdsch_ack_nr_t& ack = pending_ack[tti_tx];

    // No pending grant was set
    if (ack.nof_cc == 0) {
      return false;
    }

    // Copy data
    pdsch_ack = ack;

    // Reset list entry
    ack = {};

    return true;
  }

  void reset()
  {
    clear_pending_grants();
    reset_metrics();
    reset_measurements();
  }

  bool has_valid_sr_resource(const srsran::phy_cfg_nr_t& cfg, uint32_t sr_id)
  {
    for (const srsran_pucch_nr_sr_resource_t& r : cfg.pucch.sr_resources) {
      if (r.configured && r.sr_id == sr_id) {
        return true;
      }
    }
    return false;
  }

  void clear_pending_grants()
  {
    // Clear all PDSCH assignments and PUSCH grants
    // Scope mutex to protect read/write each list
    {
      std::lock_guard<std::mutex> lock(pending_ul_grant_mutex);
      pending_ul_grant = {};
    }
    {
      std::lock_guard<std::mutex> lock(pending_dl_grant_mutex);
      pending_dl_grant = {};
    }
    {
      std::lock_guard<std::mutex> lock(pending_ack_mutex);
      pending_ack = {};
    }
  }

  void get_pending_sr(const srsran::phy_cfg_nr_t& cfg, const uint32_t& tti, srsran_uci_data_nr_t& uci_data)
  {
    // Calculate all SR opportunities in the given TTI
    uint32_t sr_resource_id[SRSRAN_PUCCH_MAX_NOF_SR_RESOURCES] = {};
    int      n = srsran_ue_ul_nr_sr_send_slot(cfg.pucch.sr_resources, tti, sr_resource_id);
    if (n < SRSRAN_SUCCESS) {
      ERROR("Calculating SR opportunities");
      return;
    }

    // Initialise counters
    uint32_t sr_count_all      = (uint32_t)n; // Number of opportunities in this TTI
    uint32_t sr_count_positive = 0;

    // Iterate all opportunities and check if there is a pending SR
    for (uint32_t i = 0; i < sr_count_all; i++) {
      // Extract SR identifier
      uint32_t sr_id = cfg.pucch.sr_resources[sr_resource_id[i]].sr_id;

      // Check if the SR resource ID is pending
      if (args.fixed_sr.count(sr_id) > 0 ||
          stack->sr_opportunity(tti, sr_id, false, pending_ul_grant[TTI_TX(tti)].enable)) {
        // Count it as present
        sr_count_positive++;
      }
    }

    // Configure SR fields in UCI data
    uci_data.cfg.pucch.sr_resource_id = sr_resource_id[0];
    uci_data.cfg.o_sr                 = srsran_ra_ul_nr_nof_sr_bits(sr_count_all);
    uci_data.cfg.sr_positive_present  = sr_count_positive > 0;
    uci_data.value.sr                 = sr_count_positive;
  }

  void
  get_periodic_csi(const srsran::phy_cfg_nr_t& cfg, const srsran_slot_cfg_t& slot_cfg, srsran_uci_data_nr_t& uci_data)
  {
    // Generate report configurations
    int n = srsran_csi_reports_generate(&cfg.csi, &slot_cfg, uci_data.cfg.csi);
    if (n > SRSRAN_SUCCESS) {
      uci_data.cfg.nof_csi = n;
    }

    // Quantify reports from measurements
    n = srsran_csi_reports_quantify(uci_data.cfg.csi, csi_measurements.data(), uci_data.value.csi);
    if (n > SRSRAN_SUCCESS) {
      uci_data.cfg.nof_csi = n;
    }

    // Set fix wideband CQI if it is not zero nor greater than 15
    if (args.fix_wideband_cqi != 0 && args.fix_wideband_cqi < 15) {
      for (uint32_t i = 0; i < uci_data.cfg.nof_csi; i++) {
        if (uci_data.cfg.csi[i].cfg.quantity == SRSRAN_CSI_REPORT_QUANTITY_CRI_RI_PMI_CQI &&
            uci_data.cfg.csi[i].cfg.freq_cfg == SRSRAN_CSI_REPORT_FREQ_WIDEBAND) {
          uci_data.value.csi[i].wideband_cri_ri_pmi_cqi.cqi = args.fix_wideband_cqi;
        }
      }
    }

    uci_data.cfg.pucch.rnti = stack->get_ul_sched_rnti_nr(slot_cfg.idx).id;
  }

  /**
   * @brief Sets time and frequency synchronization metrics
   * @param m Metrics object
   */
  void set_info_metrics(const info_metrics_t& m)
  {
    std::lock_guard<std::mutex> lock(metrics_mutex);
    info_metrics = m;
  }

  /**
   * @brief Sets time and frequency synchronization metrics
   * @param m Metrics object
   */
  void set_sync_metrics(const sync_metrics_t& m)
  {
    std::lock_guard<std::mutex> lock(metrics_mutex);
    sync_metrics.set(m);
  }

  /**
   * @brief Sets DL channel metrics from received CSI-RS resources
   * @param m Metrics object
   */
  void set_channel_metrics(const ch_metrics_t& m)
  {
    std::lock_guard<std::mutex> lock(metrics_mutex);
    ch_metrics.set(m);
  }

  /**
   * @brief Sets DL metrics of a given PDSCH transmission
   * @param m Metrics object
   */
  void set_dl_metrics(const dl_metrics_t& m)
  {
    std::lock_guard<std::mutex> lock(metrics_mutex);
    dl_metrics.set(m);
  }

  /**
   * @brief Sets UL metrics of a given PUSCH transmission
   * @param m Metrics object
   */
  void set_ul_metrics(const ul_metrics_t& m)
  {
    std::lock_guard<std::mutex> lock(metrics_mutex);
    ul_metrics.set(m);
  }

  /**
   * @brief Resets all metrics (protected)
   */
  void reset_metrics()
  {
    std::lock_guard<std::mutex> lock(metrics_mutex);
    reset_metrics_();
  }

  /**
   * @brief Appends the NR PHY metrics to the general metric hub
   * @param m PHY Metrics object
   */
  void get_metrics(phy_metrics_t& m)
  {
    std::lock_guard<std::mutex> lock(metrics_mutex);

    uint32_t cc = m.nof_active_cc;
    m.info[cc]  = info_metrics;
    m.sync[cc]  = sync_metrics;
    m.ch[cc]    = ch_metrics;
    m.dl[cc]    = dl_metrics;
    m.ul[cc]    = ul_metrics;
    m.nof_active_cc++;

    // Reset all metrics
    reset_metrics_();
  }

  /**
   * @brief Resets all PHY measurements (protected)
   */
  void reset_measurements()
  {
    std::lock_guard<std::mutex> lock(csi_measurements_mutex);
    csi_measurements = {};
  }

  /**
   * @brief Processes a new NZP-CSI-RS channel measurement
   * @param cfg Physical layer configuration object
   * @param new_measure New measurement
   * @param resource_set_id NZP-CSI-RS resource set identifier used for the channel measurement
   */
  void new_nzp_csi_rs_channel_measurement(const srsran::phy_cfg_nr_t&              cfg,
                                          const srsran_csi_channel_measurements_t& new_measure,
                                          uint32_t                                 resource_set_id)
  {
    std::lock_guard<std::mutex> lock(csi_measurements_mutex);

    if (srsran_csi_new_nzp_csi_rs_measurement(
            cfg.csi.csi_resources, csi_measurements.data(), &new_measure, resource_set_id) < SRSRAN_SUCCESS) {
      ERROR("Error processing new NZP-CSI-RS");
      return;
    }
  }

  /**
   * @brief Processes a new Tracking Reference Signal (TRS) measurement
   * @param new_measure New measurement
   * @param cfg Physical layer configuration object
   * @param resource_set_id NZP-CSI-RS resource set identifier used for the channel measurement if it is configured from
   * a NZP-CSI-RS
   * @param K_csi_rs Number of NZP-CSI-RS resources used for the measurement, set to 0 if another type of signal is
   * measured (i.e. SSB)
   */
  void new_csi_trs_measurement(const srsran_csi_trs_measurements_t& new_meas,
                               const srsran::phy_cfg_nr_t&          cfg,
                               uint32_t                             resource_set_id = 0,
                               uint32_t                             K_csi_rs        = 0)
  {
    // Compute channel metrics and push it
    ch_metrics_t new_ch_metrics = {};
    new_ch_metrics.sinr         = new_meas.snr_dB;
    new_ch_metrics.rsrp         = new_meas.rsrp_dB;
    new_ch_metrics.rsrq         = 0.0f; // Not supported
    new_ch_metrics.rssi         = 0.0f; // Not supported
    new_ch_metrics.sync_err     = new_meas.delay_us;
    set_channel_metrics(new_ch_metrics);

    // Compute synch metrics and report it to the PHY state
    sync_metrics_t new_sync_metrics = {};
    new_sync_metrics.cfo            = new_meas.cfo_hz + ul_ext_cfo_hz;
    set_sync_metrics(new_sync_metrics);

    // Convert to CSI channel measurement and report new NZP-CSI-RS measurement to the PHY state
    srsran_csi_channel_measurements_t measurements = {};
    measurements.cri                               = 0;
    measurements.wideband_rsrp_dBm                 = new_meas.rsrp_dB;
    measurements.wideband_epre_dBm                 = new_meas.epre_dB;
    measurements.wideband_snr_db                   = new_meas.snr_dB;
    measurements.nof_ports                         = 1; // Other values are not supported
    measurements.K_csi_rs                          = K_csi_rs;
    new_nzp_csi_rs_channel_measurement(cfg, measurements, resource_set_id);

    // Update tracking information
    trs_measurements_mutex.lock();
    trs_measurements.rsrp_dB = SRSRAN_VEC_SAFE_EMA(new_meas.rsrp_dB, trs_measurements.rsrp_dB, args.trs_epre_ema_alpha);
    trs_measurements.epre_dB = SRSRAN_VEC_SAFE_EMA(new_meas.epre_dB, trs_measurements.epre_dB, args.trs_rsrp_ema_alpha);
    trs_measurements.snr_dB  = SRSRAN_VEC_SAFE_EMA(new_meas.snr_dB, trs_measurements.snr_dB, args.trs_sinr_ema_alpha);
    // Consider CFO measurement invalid if the SNR is negative. In this case, set CFO to 0.
    if (new_meas.snr_dB > 0.0f) {
      trs_measurements.cfo_hz = SRSRAN_VEC_SAFE_EMA(new_meas.cfo_hz, trs_measurements.cfo_hz, args.trs_cfo_ema_alpha);
    }
    trs_measurements.nof_re++;
    trs_measurements_mutex.unlock();
  }

  float get_dl_cfo()
  {
    std::lock_guard<std::mutex> lock(trs_measurements_mutex);
    return trs_measurements.cfo_hz;
  }

  float get_ul_cfo() const
  {
    std::lock_guard<std::mutex> lock(trs_measurements_mutex);
    return trs_measurements.cfo_hz + ul_ext_cfo_hz;
  }

  void set_ul_ext_cfo(float ext_cfo_hz) { ul_ext_cfo_hz = ext_cfo_hz; }
};

} // namespace nr
} // namespace srsue

#endif // SRSRAN_STATE_H
