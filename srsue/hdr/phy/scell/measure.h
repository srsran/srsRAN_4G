/*
 * Copyright 2013-2019 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSUE_MEASURE_RECV_H
#define SRSUE_MEASURE_RECV_H

#include <srslte/srslte.h>

#include "srsue/hdr/phy/phy_common.h"

namespace srsue {
namespace scell {
// Class to perform cell measurements
class measure
{

  // TODO: This class could early stop once the variance between the last N measurements is below 3GPP requirements

public:
  typedef enum { IDLE, MEASURE_OK, ERROR } ret_code;

  ~measure();
  void     init(cf_t*        buffer[SRSLTE_MAX_PORTS],
                srslte::log* log_h,
                uint32_t     nof_rx_antennas,
                phy_common*  worker_com,
                uint32_t     nof_subframes = RSRP_MEASURE_NOF_FRAMES);
  void     reset();
  void     set_cell(srslte_cell_t cell);
  ret_code run_subframe(uint32_t sf_idx);
  ret_code run_multiple_subframes(cf_t* buffer, uint32_t offset, uint32_t sf_idx, uint32_t nof_sf);
  float    rssi();
  float    rsrp();
  float    rsrq();
  float    snr();
  float    cfo();
  uint32_t frame_st_idx();
  void     set_rx_gain_offset(float rx_gain_offset);

private:
  srslte::log*       log_h;
  srslte_ue_dl_t     ue_dl;
  srslte_ue_dl_cfg_t ue_dl_cfg;
  cf_t*              buffer[SRSLTE_MAX_PORTS];
  uint32_t           cnt;
  uint32_t           nof_subframes;
  uint32_t           current_prb;
  float              rx_gain_offset;
  float              mean_rsrp, mean_rsrq, mean_snr, mean_rssi, mean_cfo;
  uint32_t           final_offset;
  const static int   RSRP_MEASURE_NOF_FRAMES = 5;
};

} // namespace scell
} // namespace srsue

#endif // SRSUE_MEASURE_RECV_H
