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

#ifndef SRSUE_SCELL_RECV_H
#define SRSUE_SCELL_RECV_H

#include "srsue/hdr/phy/phy_common.h"
#include <set>
#include <srslte/srslte.h>

namespace srsue {
namespace scell {

// Class to receive secondary cell
class scell_recv
{
public:
  explicit scell_recv(srslog::basic_logger& logger) : logger(logger) {}

  void               init(uint32_t max_sf_window);
  void               deinit();
  void               reset();
  std::set<uint32_t> find_cells(const cf_t* input_buffer, const srslte_cell_t serving_cell, const uint32_t nof_sf);

private:
  // 36.133 9.1.2.1 for band 7
  constexpr static float ABSOLUTE_RSRP_THRESHOLD_DBM = -125;

  cf_t*                 sf_buffer[SRSLTE_MAX_PORTS];
  srslog::basic_logger& logger;
  srslte_sync_t         sync_find;

  uint32_t current_fft_sz;
};

} // namespace scell
} // namespace srsue

#endif // SRSUE_SCELL_RECV_H
