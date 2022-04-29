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

#ifndef SRSENB_NR_SLOT_WORKER_H
#define SRSENB_NR_SLOT_WORKER_H

#include "srsran/common/thread_pool.h"
#include "srsran/interfaces/gnb_interfaces.h"
#include "srsran/interfaces/phy_common_interface.h"
#include "srsran/srslog/srslog.h"
#include "srsran/srsran.h"

namespace srsenb {
namespace nr {

/**
 * The slot_worker class handles the PHY processing, UL and DL procedures associated with 1 slot.
 *
 * A slot_worker object is executed by a thread within the thread_pool.
 */

class slot_worker final : public srsran::thread_pool::worker
{
public:
  /**
   * @brief Slot worker synchronization interface
   */
  class sync_interface
  {
  public:
    /**
     * @brief Wait for the worker to start DL scheduler
     * @param w Worker pointer
     */
    virtual void wait(slot_worker* w) = 0;

    /**
     * @brief Releases the current worker
     */
    virtual void release() = 0;
  };

  struct args_t {
    uint32_t                    cell_index       = 0;
    uint32_t                    nof_max_prb      = SRSRAN_MAX_PRB_NR;
    uint32_t                    nof_tx_ports     = 1;
    uint32_t                    nof_rx_ports     = 1;
    uint32_t                    rf_port          = 0;
    srsran_subcarrier_spacing_t scs              = srsran_subcarrier_spacing_15kHz;
    uint32_t                    pusch_max_its    = 10;
    float                       pusch_min_snr_dB = -10.0f;
    double                      srate_hz         = 0.0;
  };

  slot_worker(srsran::phy_common_interface& common_,
              stack_interface_phy_nr&       stack_,
              sync_interface&               sync_,
              srslog::basic_logger&         logger);
  ~slot_worker();

  bool init(const args_t& args);

  bool set_common_cfg(const srsran_carrier_nr_t&   carrier,
                      const srsran_pdcch_cfg_nr_t& pdcch_cfg_,
                      const srsran_ssb_cfg_t&      ssb_cfg_);

  /* Functions used by main PHY thread */
  cf_t*    get_buffer_rx(uint32_t antenna_idx);
  cf_t*    get_buffer_tx(uint32_t antenna_idx);
  uint32_t get_buffer_len();
  void     set_context(const srsran::phy_common_interface::worker_context_t& w_ctx);

private:
  /**
   * @brief Inherited from thread_pool::worker. Function called every slot to run the DL/UL processing
   */
  void work_imp() override;

  /**
   * @brief Retrieves the scheduling results for the UL processing and performs reception
   * @return True if no error occurs, false otherwise
   */
  bool work_ul();

  /**
   * @brief Retrieves the scheduling results for the DL processing and performs transmission
   * @return True if no error occurs, false otherwise
   */
  bool work_dl();

  srsran::phy_common_interface& common;
  stack_interface_phy_nr&       stack;
  srslog::basic_logger&         logger;
  sync_interface&               sync;

  uint32_t                                       sf_len      = 0;
  uint32_t                                       cell_index  = 0;
  uint32_t                                       rf_port     = 0;
  srsran_slot_cfg_t                              dl_slot_cfg = {};
  srsran_slot_cfg_t                              ul_slot_cfg = {};
  srsran::phy_common_interface::worker_context_t context     = {};
  srsran_pdcch_cfg_nr_t                          pdcch_cfg   = {};
  srsran_gnb_dl_t                                gnb_dl      = {};
  srsran_gnb_ul_t                                gnb_ul      = {};
  std::vector<cf_t*>                             tx_buffer; ///< Baseband transmit buffers
  std::vector<cf_t*>                             rx_buffer; ///< Baseband receive buffers
  std::mutex mutex; ///< Protect concurrent access from workers (and main process that inits the class)
};

} // namespace nr
} // namespace srsenb

#endif // SRSENB_NR_PHCH_WORKER_H
