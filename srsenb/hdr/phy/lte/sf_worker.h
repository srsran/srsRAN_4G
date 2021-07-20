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

#ifndef SRSENB_PHCH_WORKER_H
#define SRSENB_PHCH_WORKER_H

#include <mutex>
#include <string.h>

#include "../phy_common.h"
#include "cc_worker.h"
#include "srsran/srslog/srslog.h"
#include "srsran/srsran.h"

namespace srsenb {
namespace lte {

class sf_worker : public srsran::thread_pool::worker
{
public:
  sf_worker(srslog::basic_logger& logger) : logger(logger) {}
  ~sf_worker();
  void init(phy_common* phy);

  cf_t* get_buffer_rx(uint32_t cc_idx, uint32_t antenna_idx);
  void  set_context(const srsran::phy_common_interface::worker_context_t& w_ctx);

  int      add_rnti(uint16_t rnti, uint32_t cc_idx);
  void     rem_rnti(uint16_t rnti);
  uint32_t get_nof_rnti();

  /* These are used by the GUI plotting tools */
  uint32_t get_nof_carriers();
  int      get_carrier_pci(uint32_t cc_idx);
  int      read_ce_abs(uint32_t cc_idx, float* ce_abs);
  int      read_ce_arg(uint32_t cc_idx, float* ce_abs);
  int      read_pusch_d(uint32_t cc_idx, cf_t* pusch_d);
  int      read_pucch_d(uint32_t cc_idx, cf_t* pusch_d);
  void     start_plot();

  uint32_t get_metrics(std::vector<phy_metrics_t>& metrics);

private:
  void work_imp() final;

  /* Common objects */
  srslog::basic_logger& logger;
  phy_common*           phy       = nullptr;
  bool                  initiated = false;
  bool                  running   = false;
  std::mutex            work_mutex;

  uint32_t                                       tti_rx = 0, tti_tx_dl = 0, tti_tx_ul = 0;
  std::vector<std::unique_ptr<cc_worker> >       cc_workers;
  srsran::phy_common_interface::worker_context_t context = {};

  srsran_softbuffer_tx_t temp_mbsfn_softbuffer = {};
};

} // namespace lte
} // namespace srsenb

#endif // SRSENB_PHCH_WORKER_H
