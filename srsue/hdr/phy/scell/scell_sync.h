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

#ifndef SRSLTE_SCELL_SYNC_H
#define SRSLTE_SCELL_SYNC_H

namespace srsue {
namespace scell {

/**
 * Radio feedback interface
 */
class sync_callback
{
public:
  /**
   * Provides secondary serving cell synchronization feedback
   * @param ch channel index
   * @param offset Number of samples to offset
   */
  virtual void set_rx_channel_offset(uint32_t ch, int32_t offset) = 0;
};

class sync
{
private:
  /**
   * FSM
   *
   *  Init   +------+ Set cell  +------------+ PSS found +----------+
   *   or -->| IDLE |---------->| Search PSS |---------->| In-Synch |
   *  Stop   +------+           +------------+           +----------+
   *                                   ^ Set Cell             |
   *                                   |                      |
   *                                   +----------------------+
   */
  typedef enum { STATE_IDLE = 0, STATE_SEARCH_PSS, STATE_IN_SYNCH } state_t;

  state_t                                 state    = STATE_IDLE;
  sync_callback*                          callback = nullptr;
  uint32_t                                channel  = 0;
  srslte_sync_t                           find_pss = {};
  int32_t                                 sf_len   = 0;
  int32_t                                 cell_id  = -1;
  std::array<cf_t, 2 * SRSLTE_SF_LEN_MAX> temp     = {};
  std::mutex                              mutex; ///< Used for avoiding reconfiguring (set_cell) while it is searching

  /**
   * Executes the PSS search state
   * @param tti
   * @param buffer
   */
  void run_state_search_pss(uint32_t tti, cf_t* buffer)
  {
    uint32_t peak_pos = 0;

    // Append new base-band
    if (buffer == nullptr) {
      srslte_vec_cf_zero(&temp[sf_len], sf_len);
    } else {
      srslte_vec_cf_copy(&temp[sf_len], buffer, sf_len);
    }

    // Run PSS search
    switch (srslte_sync_find(&find_pss, temp.data(), 0, &peak_pos)) {
      case SRSLTE_SYNC_FOUND:
        if (callback != nullptr) {
          // Calculate Sample Offset from TTI difference
          int tti_mod    = (int)tti % (SRSLTE_NOF_SF_X_FRAME / 2);
          int tti_offset = (tti_mod < 3) ? tti_mod : (tti_mod - SRSLTE_NOF_SF_X_FRAME / 2);

          // Calculate sample offset from PSS correlation peak
          int offset = (int)(peak_pos - (3 * sf_len) / 2);

          // Provide offset through feedback interface
          callback->set_rx_channel_offset(channel, offset + tti_offset * sf_len);
        }
        state = STATE_IN_SYNCH;
        break;
      case SRSLTE_SYNC_FOUND_NOSPACE:
        ERROR("No space error");
        break;
      case SRSLTE_SYNC_NOFOUND:
        // Ignore
        break;
      case SRSLTE_SYNC_ERROR:
        ERROR("Error finding PSS");
        break;
    }

    // If the state has not changed, copy new data into the temp buffer
    if (state == STATE_SEARCH_PSS) {
      srslte_vec_cf_copy(&temp[0], buffer, sf_len);
    }
  }

public:
  /**
   * Constructor
   *
   * Init PSS search object with twice the max subframe length to support max cell bandwidth.
   *
   * @param _callback provides the class for giving feedback
   * @param _channel provides the channel index where the feedback needs to be applied
   */
  sync(sync_callback* _callback, uint32_t _channel) : callback(_callback), channel(_channel)
  {
    // Initialise Find PSS object
    if (srslte_sync_init(&find_pss, 2 * SRSLTE_SF_LEN_MAX, 2 * SRSLTE_SF_LEN_MAX, SRSLTE_SYMBOL_SZ_MAX) !=
        SRSLTE_SUCCESS) {
      ERROR("Initiating Synchronizer");
    }
  }

  ~sync() { srslte_sync_free(&find_pss); };

  void set_bw(const uint32_t nof_prb)
  {
    // Protect DSP objects and buffers; As it is called by asynchronous thread, it can wait to finish current processing
    std::unique_lock<std::mutex> lock(mutex);

    uint32_t symbol_sz  = srslte_symbol_sz(nof_prb);
    int32_t  new_sf_len = SRSLTE_SF_LEN_PRB(nof_prb);

    // Skip if no BW is changed
    if (new_sf_len == sf_len) {
      return;
    }

    // Resize
    if (srslte_sync_resize(&find_pss, 2 * new_sf_len, 2 * new_sf_len, symbol_sz) != SRSLTE_SUCCESS) {
      ERROR("Error setting cell sync find");
    }

    // Update values
    sf_len  = new_sf_len;
    cell_id = -1; // Force next set_cell to set the ID

    // Reset state to idle
    state = STATE_IDLE;
  }

  /**
   * Sets the cell for the synchronizer
   */
  void set_cell(const srslte_cell_t& cell)
  {
    // Protect DSP objects and buffers; As it is called by asynchronous thread, it can wait to finish current processing
    std::unique_lock<std::mutex> lock(mutex);

    uint32_t symbol_sz   = srslte_symbol_sz(cell.nof_prb);
    int32_t  new_sf_len  = SRSLTE_SF_LEN_PRB(cell.nof_prb);
    int32_t  new_cell_id = cell.id;

    // Resize Sync object
    if (new_sf_len != sf_len) {
      if (srslte_sync_resize(&find_pss, 2 * new_sf_len, 2 * new_sf_len, symbol_sz) != SRSLTE_SUCCESS) {
        ERROR("Error setting cell sync find");
      }

      // Force cell_id set
      cell_id = -1;
    }

    // Configure
    if (cell_id != new_cell_id) {
      srslte_sync_set_frame_type(&find_pss, cell.frame_type);
      srslte_sync_set_N_id_2(&find_pss, new_cell_id % SRSLTE_NOF_NID_2);
      srslte_sync_set_N_id_1(&find_pss, new_cell_id / SRSLTE_NOF_NID_2);
      srslte_sync_set_cfo_ema_alpha(&find_pss, 0.1);
      srslte_sync_set_em_alpha(&find_pss, 1);
      srslte_sync_set_threshold(&find_pss, 3.0);
    }

    // Reset Temporal buffer
    srslte_vec_cf_zero(temp.data(), 2 * sf_len);

    // Update new values
    sf_len  = new_sf_len;
    cell_id = new_cell_id;

    // Go to search PSS
    state = STATE_SEARCH_PSS;
  }

  /**
   * Resets the class, goes back into IDLE mode
   */
  void stop() { state = STATE_IDLE; }

  /**
   * Runs internal FSM, performing Synchronization operations on the provided buffer. It expects data per sub-frame
   * basis (1 ms).
   * @param tti Current primary serving cell time
   * @param buffer Base-band buffer of the given secondary serving cell
   */
  void run(uint32_t tti, cf_t* buffer)
  {
    // Try to get lock. The lock is unsuccessful if the DSP objects are getting configured. In this case, ignore
    // the sub-frame.
    if (not mutex.try_lock()) {
      return;
    }

    switch (state) {
      case STATE_IDLE:
        // Do nothing
        break;
      case STATE_SEARCH_PSS:
        run_state_search_pss(tti, buffer);
        break;
      case STATE_IN_SYNCH:
        // Do nothing
        break;
    }

    mutex.unlock();
  }

  /**
   * Indicates whether the secondary serving cell assigned to the instance is in-sync
   * @return true if it is in sync state
   */
  bool is_in_sync() const { return state == STATE_IN_SYNCH; }
};
} // namespace scell
} // namespace srsue
#endif // SRSLTE_SCELL_SYNC_H
