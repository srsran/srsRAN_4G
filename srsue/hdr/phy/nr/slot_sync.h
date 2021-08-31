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

#ifndef SRSUE_SLOT_SYNC_H
#define SRSUE_SLOT_SYNC_H

#include "srsran/interfaces/radio_interfaces.h"
#include "srsran/interfaces/ue_nr_interfaces.h"
#include "srsran/radio/rf_buffer.h"
#include "srsran/radio/rf_timestamp.h"
#include "srsran/srsran.h"

namespace srsue {
namespace nr {
class slot_sync
{
public:
  struct args_t {
    double                      max_srate_hz    = 1.92e6;
    uint32_t                    nof_rx_channels = 1;
    srsran_subcarrier_spacing_t ssb_min_scs     = srsran_subcarrier_spacing_15kHz;
    bool                        disable_cfo     = false;
    float                       pbch_dmrs_thr   = 0.0f; ///< PBCH DMRS correlation detection threshold (0 means auto)
    float                       cfo_alpha       = 0.0f; ///< CFO averaging alpha (0 means auto)
    int                         thread_priority = -1;
  };

  slot_sync(stack_interface_phy_sa_nr& stack_, srsran::radio_interface_phy& radio_);
  ~slot_sync();

  bool init(const args_t& args);

  int  recv_callback(srsran::rf_buffer_t& rf_buffer, srsran_timestamp_t* timestamp);
  void run_stack_tti();

private:
  const static int MIN_TTI_JUMP         = 1;    ///< Time gap reported to stack after receiving subframe
  const static int MAX_TTI_JUMP         = 1000; ///< Maximum time gap tolerance in RF stream metadata
  enum { SEARCHING = 0, CAMPING } state = SEARCHING;
  srslog::basic_logger&        logger;
  stack_interface_phy_sa_nr&   stack;
  srsran::radio_interface_phy& radio;
  srsran_ue_sync_nr_t          ue_sync_nr = {};
  srsran::rf_timestamp_t       last_rx_time;
  srsran_timestamp_t           stack_tti_ts_new    = {};
  srsran_timestamp_t           stack_tti_ts        = {};
  bool                         forced_rx_time_init = true; // Rx time sync after first receive from radio
  uint32_t                     tti                 = 0;
};
} // namespace nr
} // namespace srsue

#endif // SRSUE_SLOT_SYNC_H
