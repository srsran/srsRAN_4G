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

#ifndef SRSUE_NR_PHCH_WORKER_H
#define SRSUE_NR_PHCH_WORKER_H

#include "cc_worker.h"
#include "srsran/common/thread_pool.h"
#include "srsran/interfaces/phy_common_interface.h"

namespace srsue {
namespace nr {

/**
 * The sf_worker class handles the PHY processing, UL and DL procedures associated with 1 subframe.
 * It contains multiple cc_worker objects, one for each component carrier which may be executed in
 * one or multiple threads.
 *
 * A sf_worker object is executed by a thread within the thread_pool.
 */

class sf_worker final : public srsran::thread_pool::worker
{
public:
  sf_worker(srsran::phy_common_interface& common_, state& phy_state_, srslog::basic_logger& logger);
  ~sf_worker() = default;

  bool update_cfg(uint32_t cc_idx);

  /* Functions used by main PHY thread */
  cf_t*    get_buffer(uint32_t cc_idx, uint32_t antenna_idx);
  uint32_t get_buffer_len();
  void     set_context(const srsran::phy_common_interface::worker_context_t& w_ctx);
  int      read_pdsch_d(cf_t* pdsch_d);
  void     start_plot();

  void set_prach(cf_t* prach_ptr, float prach_power);

private:
  /* Inherited from thread_pool::worker. Function called every subframe to run the DL/UL processing */
  void work_imp() override;

  std::vector<std::unique_ptr<cc_worker> > cc_workers;

  srsran::phy_common_interface&                  common;
  state&                                         phy_state;
  srslog::basic_logger&                          logger;
  srsran::rf_timestamp_t                         tx_time     = {};
  uint32_t                                       tti_rx      = 0;
  cf_t*                                          prach_ptr   = nullptr;
  float                                          prach_power = 0;
  srsran::phy_common_interface::worker_context_t context     = {};
};

} // namespace nr
} // namespace srsue

#endif // SRSUE_NR_PHCH_WORKER_H
