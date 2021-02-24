/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSLTE_STATE_H
#define SRSLTE_STATE_H

#include "srslte/adt/circular_array.h"
#include "srslte/common/common.h"
#include "srslte/interfaces/ue_nr_interfaces.h"
#include "srslte/srslte.h"
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
    srslte_sch_cfg_nr_t sch_cfg;
  };
  srslte::circular_array<pending_ul_grant_t, TTIMOD_SZ> pending_ul_grant = {};
  mutable std::mutex                                    pending_ul_grant_mutex;

  struct pending_dl_grant_t {
    bool                           enable;
    uint32_t                       pid;
    srslte_sch_cfg_nr_t            sch_cfg;
    srslte_pdsch_ack_resource_nr_t ack_resource;
  };
  srslte::circular_array<pending_dl_grant_t, TTIMOD_SZ> pending_dl_grant = {};
  mutable std::mutex                                    pending_dl_grant_mutex;

  srslte::circular_array<srslte_pdsch_ack_nr_t, TTIMOD_SZ> pending_ack = {};
  mutable std::mutex                                       pending_ack_mutex;

public:
  mac_interface_phy_nr* stack   = nullptr;
  srslte_carrier_nr_t   carrier = {};
  srslte::phy_cfg_nr_t  cfg;
  phy_args_nr_t         args;

  uint16_t ra_rnti       = 0;
  uint32_t rar_grant_tti = 0;

  state()
  {
    carrier.id              = 500;
    carrier.nof_prb         = 100;
    carrier.max_mimo_layers = 1;
  }

  /**
   * @brief Stores a received UL DCI into the pending UL grant list
   * @param tti_rx The TTI in which the grant was received
   * @param dci_ul The UL DCI message to store
   */
  void set_ul_pending_grant(uint32_t tti_rx, const srslte_dci_ul_nr_t& dci_ul)
  {
    // Convert UL DCI to grant
    srslte_sch_cfg_nr_t pusch_cfg = {};
    if (srslte_ra_ul_dci_to_grant_nr(&carrier, &cfg.pusch, &dci_ul, &pusch_cfg, &pusch_cfg.grant)) {
      std::array<char, 512> str;
      srslte_dci_ul_nr_to_str(&dci_ul, str.data(), str.size());
      ERROR("Computing UL grant %s", str.data());
      return;
    }

    // Calculate Transmit TTI
    uint32_t tti_tx = TTI_ADD(tti_rx, pusch_cfg.grant.k);

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
  bool get_ul_pending_grant(uint32_t tti_tx, srslte_sch_cfg_nr_t& pusch_cfg, uint32_t& pid)
  {
    // Scope mutex to protect read/write the list
    std::lock_guard<std::mutex> lock(pending_ul_grant_mutex);

    // Select entry
    pending_ul_grant_t& pending_grant = pending_ul_grant[tti_tx];

    // If the entry is not active, just return
    if (!pending_grant.enable) {
      return false;
    }

    // Load shared channel configuration
    pusch_cfg = pending_grant.sch_cfg;

    // Reset entry
    pending_grant.enable = false;

    return true;
  }

  /**
   * @brief Stores a received DL DCI into the pending DL grant list
   * @param tti_rx The TTI in which the grant was received
   * @param dci_dl The DL DCI message to store
   */
  void set_dl_pending_grant(uint32_t tti_rx, const srslte_dci_dl_nr_t& dci_dl)
  {
    // Convert DL DCI to grant
    srslte_sch_cfg_nr_t pdsch_cfg = {};
    if (srslte_ra_dl_dci_to_grant_nr(&carrier, &cfg.pdsch, &dci_dl, &pdsch_cfg, &pdsch_cfg.grant)) {
      ERROR("Computing UL grant");
      return;
    }

    // Calculate DL DCI to PDSCH ACK resource
    srslte_pdsch_ack_resource_nr_t ack_resource = {};
    if (srslte_ue_dl_nr_pdsch_ack_resource(&cfg.harq_ack, &dci_dl, &ack_resource) < SRSLTE_SUCCESS) {
      ERROR("Computing UL ACK resource");
      return;
    }

    // Calculate Receive TTI
    tti_rx = TTI_ADD(tti_rx, pdsch_cfg.grant.k);

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
  bool get_dl_pending_grant(uint32_t                        tti_rx,
                            srslte_sch_cfg_nr_t&            pdsch_cfg,
                            srslte_pdsch_ack_resource_nr_t& ack_resource,
                            uint32_t&                       pid)
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
  void set_pending_ack(const uint32_t& tti_rx, const srslte_pdsch_ack_resource_nr_t& ack_resource, const bool& crc_ok)
  {
    // Calculate Receive TTI
    uint32_t tti_tx = TTI_ADD(tti_rx, ack_resource.k1);

    // Scope mutex to protect read/write the list
    std::lock_guard<std::mutex> lock(pending_ack_mutex);

    // Select UL transmission time resource
    srslte_pdsch_ack_nr_t& ack = pending_ack[tti_tx];
    ack.nof_cc                 = 1;

    // Select serving cell
    srslte_pdsch_ack_cc_nr_t& ack_cc = ack.cc[ack_resource.scell_idx];
    srslte_pdsch_ack_m_nr_t&  ack_m  = ack_cc.m[ack_cc.M];
    ack_cc.M++;

    // Set PDSCH transmission information
    ack_m.resource = ack_resource;
    ack_m.value[0] = crc_ok ? 1 : 0;
    ack_m.present  = true;
  }

  bool get_pending_ack(const uint32_t& tti_tx, srslte_pdsch_ack_nr_t& pdsch_ack)
  {
    // Scope mutex to protect read/write the list
    std::lock_guard<std::mutex> lock(pending_ack_mutex);

    // Select UL transmission time resource
    srslte_pdsch_ack_nr_t& ack = pending_ack[tti_tx];

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
};
} // namespace nr
} // namespace srsue

#endif // SRSLTE_STATE_H
