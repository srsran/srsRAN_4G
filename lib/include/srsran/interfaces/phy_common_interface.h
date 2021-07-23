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

#ifndef SRSRAN_PHY_COMMON_INTERFACE_H
#define SRSRAN_PHY_COMMON_INTERFACE_H

#include "../radio/rf_buffer.h"
#include "../radio/rf_timestamp.h"

namespace srsran {

/**
 * @brief Descibes a physical layer common interface
 */
class phy_common_interface
{
private:
  std::mutex              tx_mutex;        ///< Protect Tx attributes
  std::condition_variable tx_cvar;         ///< Tx condition variable
  bool                    tx_hold = false; ///< Hold threads until the signal is transmitted

protected:
  /**
   * @brief Waits for the last worker to call `last_worker()` to prevent that the current SF worker is released and
   * overwrites the transmit signal prior transmission
   */
  void wait_last_worker()
  {
    std::unique_lock<std::mutex> lock(tx_mutex);
    tx_hold = true;
    while (tx_hold) {
      tx_cvar.wait(lock);
    }
  }

  /**
   * @brief Notifies the last SF worker transmitted the baseband and all the workers waiting are released
   */
  void last_worker()
  {
    std::unique_lock<std::mutex> lock(tx_mutex);
    tx_hold = false;
    tx_cvar.notify_all();
  }

public:
  /**
   * @brief Describes a worker context
   */
  struct worker_context_t {
    uint32_t               sf_idx     = 0;       ///< Subframe index
    void*                  worker_ptr = nullptr; ///< Worker pointer for wait/release semaphore
    bool                   last       = false;   ///< Indicates this worker is the last one in the sub-frame processing
    srsran::rf_timestamp_t tx_time    = {};      ///< Transmit time, used only by last worker

    void copy(const worker_context_t& other)
    {
      sf_idx     = other.sf_idx;
      worker_ptr = other.worker_ptr;
      last       = other.last;
      tx_time.copy(other.tx_time);
    }

    worker_context_t() = default;
    worker_context_t(const worker_context_t& other) { copy(other); }
  };

  /**
   * @brief Common PHY interface for workers to indicate they ended
   * @param w_ctx Worker context
   * @param tx_enable Indicates whether the buffer has baseband samples to transmit
   * @param buffer Baseband buffer
   */
  virtual void worker_end(const worker_context_t& w_ctx, const bool& tx_enable, srsran::rf_buffer_t& buffer) = 0;
};

} // namespace srsran

#endif // SRSRAN_PHY_COMMON_INTERFACE_H
