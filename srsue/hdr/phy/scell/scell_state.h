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

#ifndef SRSRAN_SCELL_STATE_H
#define SRSRAN_SCELL_STATE_H

#include <cinttypes>
#include <mutex>
#include <srsran/common/common.h>

namespace srsue {
namespace scell {

/**
 * References
 *
 * According to 3GPP 36.321 R10 (MAC procedures) section 5.13 Activation/Deactivation of SCells
 *  An activated cell operation shall include (Summarised):
 *  - SRS transmissions on the SCell;
 *  - CQI/PMI/RI/PTI reporting for the SCell;
 *  - PDCCH monitoring on the SCell;
 *  - PDCCH monitoring for the SCell
 *
 * According to 3GPP 36.213 R10 (PHY procedures) section 4.3 Timing for Secondary Cell Activation / Deactivation
 *  When a UE receives an activation command for a secondary cell in subframe n, the corresponding actions shall be
 *  applied at subframe n+8. (Summarised)
 */
class state
{
private:
  static constexpr uint32_t activation_delay_tti  = FDD_HARQ_DELAY_DL_MS + FDD_HARQ_DELAY_UL_MS;
  static constexpr uint32_t activation_margin_tti = FDD_HARQ_DELAY_DL_MS;

  // SCell EARFCN, PCI, configured and enabled list
  struct cfg {
    uint32_t earfcn                            = 0;
    uint32_t pci                               = 0;
    enum { none = 0, inactive, active } status = none;
  };

  std::array<cfg, SRSRAN_MAX_CARRIERS> scell_cfg;

  enum { idle = 0, waiting, transition } activation_state = idle;
  uint32_t           activation_cmd                       = 0;
  uint32_t           activation_tti                       = 0;
  mutable std::mutex mutex;

  bool _get_cmd_activation(uint32_t cc_idx) const { return ((activation_cmd >> cc_idx) & 0x1) == 0x1; }

  bool _tti_greater_or_equal_than(uint32_t a, uint32_t b) const { return TTI_SUB(a, b) < 10240 / 2; }

public:
  /**
   * A SCell Activation/Deactivation command is received. Stores the new command and the TTI. Also, the internal state
   * goes to waiting.
   *
   * If a previous command was received and not applied, it will discard it.
   *
   * @param cmd SCell Activation/Deactivation command
   * @param tti TTI in which the command was received
   */
  void set_activation_deactivation(uint32_t cmd, uint32_t tti)
  {
    std::unique_lock<std::mutex> lock(mutex);

    // Store command
    activation_cmd = cmd;

    // Command is waiting
    activation_state = waiting;

    activation_tti = TTI_ADD(tti, activation_delay_tti);
  }

  /**
   * @brief Deactivates all the active SCells
   */
  void deactivate_all()
  {
    std::unique_lock<std::mutex> lock(mutex);

    for (cfg& e : scell_cfg) {
      if (e.status == cfg::active) {
        e.status = cfg::inactive;
      }
    }
  }

  void run_tti(uint32_t tti)
  {
    std::unique_lock<std::mutex> lock(mutex);

    switch (activation_state) {
      case idle:
        // waiting for receiving a command, do nothing
        break;
      case waiting:
        // Detect that TTI when the CMD needs to be applied, the activation cannot be done instantly because some
        // workers might be currently ongoing, so only update state
        if (_tti_greater_or_equal_than(tti, activation_tti)) {
          activation_state = transition;
        }
        break;
      case transition:
        // Detect when the TTI has increased enough to make sure there arent workers, set the configuration
        if (TTI_SUB(tti, activation_tti) >= activation_margin_tti) {
          // Reload cell states
          for (uint32_t i = 1; i < SRSRAN_MAX_CARRIERS; i++) {
            // Get Activation command value
            bool activate = _get_cmd_activation(i);

            // Apply activation only if the cell was configured
            if (scell_cfg[i].status != cfg::none) {
              scell_cfg[i].status = activate ? cfg::active : cfg::inactive;
            }
          }

          // Go back to initial state
          activation_state = idle;
        }
        break;
    }
  }

  void configure(uint32_t cc_idx, uint32_t earfcn, uint32_t pci)
  {
    std::unique_lock<std::mutex> lock(mutex);

    if (cc_idx == 0 or cc_idx >= SRSRAN_MAX_CARRIERS) {
      ERROR("CC IDX %d out-of-range", cc_idx);
      return;
    }

    scell_cfg[cc_idx].status = cfg::inactive;
    scell_cfg[cc_idx].earfcn = earfcn;
    scell_cfg[cc_idx].pci    = pci;
  }

  bool is_active(uint32_t cc_idx, uint32_t tti) const
  {
    if (cc_idx == 0) {
      return true;
    }

    if (cc_idx >= SRSRAN_MAX_CARRIERS) {
      return false;
    }

    std::unique_lock<std::mutex> lock(mutex);

    // Use stashed activation if the activation is transitioning and the current TTI requires new value
    if (activation_state == transition and scell_cfg[cc_idx].status != cfg::none and
        _tti_greater_or_equal_than(tti, activation_tti)) {
      return _get_cmd_activation(cc_idx);
    }

    return scell_cfg[cc_idx].status == cfg::active;
  }

  bool is_configured(uint32_t cc_idx) const
  {
    if (cc_idx == 0) {
      return true;
    }

    if (cc_idx >= SRSRAN_MAX_CARRIERS) {
      return false;
    }

    std::unique_lock<std::mutex> lock(mutex);

    return scell_cfg[cc_idx].status != cfg::none;
  }

  void reset(uint32_t cc_idx)
  {
    if (cc_idx == 0 or cc_idx >= SRSRAN_MAX_CARRIERS) {
      return;
    }

    std::unique_lock<std::mutex> lock(mutex);

    activation_state = idle;

    cfg& e   = scell_cfg[cc_idx];
    e.status = cfg::none;
    e.earfcn = 0;
    e.pci    = UINT32_MAX;
  }

  void reset()
  {
    std::unique_lock<std::mutex> lock(mutex);

    activation_state = idle;

    for (cfg& e : scell_cfg) {
      e.status = cfg::none;
      e.earfcn = 0;
      e.pci    = UINT32_MAX;
    }
  }

  uint32_t get_pci(uint32_t cc_idx)
  {
    std::unique_lock<std::mutex> lock(mutex);

    if (cc_idx == 0 or cc_idx >= SRSRAN_MAX_CARRIERS) {
      ERROR("CC IDX %d out-of-range", cc_idx);
      return 0;
    }

    return scell_cfg[cc_idx].pci;
  }

  uint32_t get_earfcn(uint32_t cc_idx)
  {
    std::unique_lock<std::mutex> lock(mutex);

    if (cc_idx == 0 or cc_idx >= SRSRAN_MAX_CARRIERS) {
      ERROR("CC IDX %d out-of-range", cc_idx);
      return 0;
    }

    return scell_cfg[cc_idx].earfcn;
  }
};
} // namespace scell
} // namespace srsue
#endif // SRSRAN_SCELL_STATE_H