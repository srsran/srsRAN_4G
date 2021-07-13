/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
  struct args_t {
    uint32_t cell_index         = 0;
    uint32_t nof_max_prb        = SRSRAN_MAX_PRB_NR;
    uint32_t nof_tx_ports       = 1;
    uint32_t nof_rx_ports       = 1;
    uint32_t pusch_max_nof_iter = 10;
  };

  slot_worker(srsran::phy_common_interface& common_, stack_interface_phy_nr& stack_, srslog::basic_logger& logger);
  ~slot_worker();

  bool init(const args_t& args);

  bool set_common_cfg(const srsran_carrier_nr_t& carrier, const srsran_pdcch_cfg_nr_t& pdcch_cfg_);

  /* Functions used by main PHY thread */
  cf_t*    get_buffer_rx(uint32_t antenna_idx);
  cf_t*    get_buffer_tx(uint32_t antenna_idx);
  uint32_t get_buffer_len();
  void     set_time(const uint32_t& tti, const srsran::rf_timestamp_t& timestamp);

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

  uint32_t               sf_len      = 0;
  uint32_t               cell_index  = 0;
  srsran_slot_cfg_t      dl_slot_cfg = {};
  srsran_slot_cfg_t      ul_slot_cfg = {};
  srsran_pdcch_cfg_nr_t  pdcch_cfg   = {};
  srsran::rf_timestamp_t tx_time     = {};
  srsran_gnb_dl_t        gnb_dl      = {};
  srsran_gnb_ul_t        gnb_ul      = {};
  std::vector<cf_t*>     tx_buffer; ///< Baseband transmit buffers
  std::vector<cf_t*>     rx_buffer; ///< Baseband receive buffers
};

} // namespace nr
} // namespace srsenb

#endif // SRSENB_NR_PHCH_WORKER_H
