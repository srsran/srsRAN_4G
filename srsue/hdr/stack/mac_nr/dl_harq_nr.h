/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#ifndef SRSRAN_DL_HARQ_NR_H
#define SRSRAN_DL_HARQ_NR_H

#include "srsran/interfaces/mac_interface_types.h"
#include "srsran/interfaces/ue_nr_interfaces.h"
#include "srsran/srslog/logger.h"
#include "srsue/hdr/stack/mac_nr/mac_nr_interfaces.h"
#include <mutex>

namespace srsue {

/**
 * @brief Downlink HARQ entity as defined in 5.3.2 of 38.321
 *
 * The class supports a configurable number of HARQ processes (up to 16).
 *
 * The class is configured (init and reset) by the MAC class from the
 * Stack thread context. Main functionality, however, is carried
 * out from a PHY worker context.
 *
 * Concurrent access from threads is protected through rwlocks.
 *
 */
class dl_harq_entity_nr
{
  using mac_nr_grant_dl_t = mac_interface_phy_nr::mac_nr_grant_dl_t;

public:
  dl_harq_entity_nr(uint8_t cc_idx_, mac_interface_harq_nr* mac_, demux_interface_harq_nr* demux_unit_);
  ~dl_harq_entity_nr();

  int32_t set_config(const srsran::dl_harq_cfg_nr_t& cfg_);
  void    reset();

  /// PHY->MAC interface for DL processes
  void new_grant_dl(const mac_nr_grant_dl_t& grant, mac_interface_phy_nr::tb_action_dl_t* action);
  void tb_decoded(const mac_nr_grant_dl_t& grant, mac_interface_phy_nr::tb_action_dl_result_t result);

  float get_average_retx();

  // DL HARQ metrics combined for all processes
  struct dl_harq_metrics_t {
    uint32_t rx_ok;
    uint32_t rx_ko;
    uint32_t rx_brate;
  };
  dl_harq_metrics_t get_metrics();

private:
  class dl_harq_process_nr
  {
  public:
    dl_harq_process_nr(dl_harq_entity_nr* parent);
    ~dl_harq_process_nr();
    bool    init(int pid);
    void    reset(void);
    uint8_t get_ndi();

    void
    new_grant_dl(const mac_nr_grant_dl_t& grant, const bool& ndi_toggled, mac_interface_phy_nr::tb_action_dl_t* action);
    void tb_decoded(const mac_nr_grant_dl_t& grant, mac_interface_phy_nr::tb_action_dl_result_t result);

  private:
    dl_harq_entity_nr*    harq_entity = nullptr;
    srslog::basic_logger& logger;

    bool is_first_tb = true;

    bool     is_bcch = false;
    uint32_t pid     = 0; // HARQ Proccess ID
    bool     acked   = false;
    uint32_t n_retx  = 0;

    mac_nr_grant_dl_t                       current_grant = {};
    std::unique_ptr<srsran_softbuffer_rx_t> softbuffer_rx;
  };

  // Private members of dl_harq_entity_nr
  mac_interface_harq_nr*                                                      mac = nullptr;
  srsran::dl_harq_cfg_nr_t                                                    cfg = {};
  std::array<std::unique_ptr<dl_harq_process_nr>, SRSRAN_MAX_HARQ_PROC_DL_NR> harq_procs;
  dl_harq_process_nr                                                          bcch_proc;
  demux_interface_harq_nr*                                                    demux_unit = nullptr;
  srslog::basic_logger&                                                       logger;
  uint16_t                                                                    last_temporal_crnti = SRSRAN_INVALID_RNTI;
  dl_harq_metrics_t                                                           metrics             = {};
  std::mutex                                                                  metrics_mutex;
  uint8_t                                                                     cc_idx = 0;
  pthread_rwlock_t                                                            rwlock;
};

typedef std::unique_ptr<dl_harq_entity_nr>                     dl_harq_entity_nr_ptr;
typedef std::array<dl_harq_entity_nr_ptr, SRSRAN_MAX_CARRIERS> dl_harq_entity_nr_vector;

} // namespace srsue

#endif // SRSRAN_DL_HARQ_NR_H
