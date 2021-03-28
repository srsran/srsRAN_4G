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
