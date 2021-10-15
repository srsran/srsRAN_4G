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

#ifndef SRSUE_SYNC_NR_SA_H
#define SRSUE_SYNC_NR_SA_H

#include "cell_search.h"
#include "slot_sync.h"
#include "srsran/common/threads.h"
#include "srsran/interfaces/radio_interfaces.h"
#include "srsran/interfaces/ue_nr_interfaces.h"
#include "srsran/radio/rf_buffer.h"
#include "srsran/radio/rf_timestamp.h"
#include "srsran/srslog/logger.h"
#include "srsran/srsran.h"
#include "worker_pool.h"
#include <condition_variable>
#include <mutex>
#include <srsran/common/tti_sempahore.h>

namespace srsue {
namespace nr {

/**
 * @brief NR Standalone synchronization class
 */
class sync_sa : public srsran::thread, public srsran::phy_common_interface
{
public:
  struct args_t {
    double                      srate_hz        = 61.44e6;
    srsran_subcarrier_spacing_t ssb_min_scs     = srsran_subcarrier_spacing_15kHz;
    uint32_t                    nof_rx_channels = 1;
    bool                        disable_cfo     = false;
    float                       pbch_dmrs_thr   = 0.0f; ///< PBCH DMRS correlation detection threshold (0 means auto)
    float                       cfo_alpha       = 0.0f; ///< CFO averaging alpha (0 means auto)
    int                         thread_priority = -1;

    cell_search::args_t get_cell_search() const
    {
      cell_search::args_t ret = {};
      ret.max_srate_hz        = srate_hz;
      return ret;
    }

    slot_sync::args_t get_slot_sync() const
    {
      slot_sync::args_t ret = {};
      ret.max_srate_hz      = srate_hz;
      ret.nof_rx_channels   = nof_rx_channels;
      ret.ssb_min_scs       = ssb_min_scs;

      return ret;
    }
  };

  typedef enum {
    STATE_IDLE = 0, ///< No process is in progress
    STATE_CELL_SEARCH,
    STATE_CELL_SELECT
  } state_t;

  sync_sa(stack_interface_phy_sa_nr& stack_, srsran::radio_interface_phy& radio_, worker_pool& workers_);
  ~sync_sa();

  bool init(const args_t& args_);

  // The following methods control the SYNC state machine
  bool start_cell_search(const cell_search::cfg_t& cfg);
  bool start_cell_select();
  bool go_idle();

  void stop();

  state_t get_state() const;

  void worker_end(const worker_context_t& w_ctx, const bool& tx_enable, srsran::rf_buffer_t& buffer) override;

private:
  stack_interface_phy_sa_nr&   stack;  ///< Stand-Alone RRC interface
  srsran::radio_interface_phy& radio;  ///< Radio object
  srslog::basic_logger&        logger; ///< General PHY logger
  worker_pool&                 workers;

  state_t                      state      = STATE_IDLE;
  state_t                      next_state = STATE_IDLE;
  mutable std::mutex           state_mutex;
  std::condition_variable      state_cvar;
  std::atomic<bool>            running = {false};
  uint32_t                     sf_sz   = 0; ///< Subframe size (1-ms)
  srsran::tti_semaphore<void*> tti_semaphore;
  srsran_slot_cfg_t            slot_cfg = {};

  cell_search searcher;
  slot_sync   slot_synchronizer;

  // FSM States
  void run_state_idle();
  void run_state_cell_search();
  void run_state_cell_select();

  // FSM transitions
  void enter_state_idle();

  void run_thread() override;
};

} // namespace nr
} // namespace srsue

#endif // SRSUE_SYNC_NR_SA_H
