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

#ifndef SRSUE_SFN_SYNC_H
#define SRSUE_SFN_SYNC_H

#include "srsran/radio/radio.h"
#include "srsran/srslog/srslog.h"
#include "srsran/srsran.h"

namespace srsue {

struct phy_args_t;

// Class to synchronize system frame number
class sfn_sync
{
public:
  typedef enum { IDLE, SFN_FOUND, SFX0_FOUND, SFN_NOFOUND, ERROR } ret_code;
  explicit sfn_sync(srslog::basic_logger& logger) : logger(logger) {}
  ~sfn_sync();
  void     init(srsran_ue_sync_t*    ue_sync,
                const phy_args_t*    phy_args_,
                srsran::rf_buffer_t& buffer,
                uint32_t             buffer_max_samples_,
                uint32_t             nof_subframes = SFN_SYNC_NOF_SUBFRAMES);
  void     reset();
  bool     set_cell(srsran_cell_t cell);
  ret_code run_subframe(srsran_cell_t*                               cell,
                        uint32_t*                                    tti_cnt,
                        std::array<uint8_t, SRSRAN_BCH_PAYLOAD_LEN>& bch_payload,
                        bool                                         sfidx_only = false);
  ret_code decode_mib(srsran_cell_t*                               cell,
                      uint32_t*                                    tti_cnt,
                      srsran::rf_buffer_t*                         ext_buffer,
                      std::array<uint8_t, SRSRAN_BCH_PAYLOAD_LEN>& bch_payload,
                      bool                                         sfidx_only = false);

private:
  const static int SFN_SYNC_NOF_SUBFRAMES = 100;

  const phy_args_t*     phy_args = nullptr;
  uint32_t              cnt      = 0;
  uint32_t              timeout  = 0;
  srslog::basic_logger& logger;
  srsran_ue_sync_t*     ue_sync            = nullptr;
  srsran::rf_buffer_t   mib_buffer         = {};
  uint32_t              buffer_max_samples = 0;
  srsran_ue_mib_t       ue_mib             = {};
};

}; // namespace srsue

#endif // SRSUE_SFN_SYNC_H
