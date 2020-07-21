/*
 * Copyright 2013-2020 Software Radio Systems Limited
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

  state_t                             state    = STATE_IDLE;
  sync_callback*                      callback = nullptr;
  uint32_t                            channel  = 0;
  srslte_sync_t                       find_pss = {};
  int32_t                             sf_len   = 0;
  std::array<cf_t, 2 * SRSLTE_SF_LEN_MAX> temp;

  /**
   * Executes the PSS search state
   * @param tti
   * @param buffer
   */
  void run_state_search_pss(uint32_t tti, cf_t* buffer)
  {
    uint32_t peak_pos = 0;

    // Append new base-band
    srslte_vec_cf_copy(&temp[sf_len], buffer, sf_len);

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
        ERROR("No space error\n");
        break;
      case SRSLTE_SYNC_NOFOUND:
        // Ignore
        break;
      case SRSLTE_SYNC_ERROR:
        ERROR("Error finding PSS\n");
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
      ERROR("Initiating Synchronizer\n");
    }
  }

  ~sync() { srslte_sync_free(&find_pss); };

  /**
   * Sets the cell for the synchronizer
   */
  void set_cell(const srslte_cell_t& cell)
  {
    uint32_t symbol_sz = srslte_symbol_sz(cell.nof_prb);
    sf_len             = SRSLTE_SF_LEN_PRB(cell.nof_prb);

    // Resize Sync object
    if (srslte_sync_resize(&find_pss, 2 * sf_len, 2 * sf_len, symbol_sz) != SRSLTE_SUCCESS) {
      ERROR("Error setting cell sync find\n");
    }

    // Configure
    srslte_sync_set_frame_type(&find_pss, cell.frame_type);
    srslte_sync_set_N_id_2(&find_pss, cell.id % SRSLTE_NOF_NID_2);
    srslte_sync_set_N_id_1(&find_pss, cell.id / SRSLTE_NOF_NID_2);
    srslte_sync_set_cfo_ema_alpha(&find_pss, 0.1);
    srslte_sync_set_em_alpha(&find_pss, 1);
    srslte_sync_set_threshold(&find_pss, 3.0);

    // Reset Temporal buffer
    srslte_vec_cf_zero(temp.data(), 2 * sf_len);

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
  }

  /**
   * Get channel index
   * @return The channel number it has been configured
   */
  uint32_t get_channel() const { return channel; }

  /**
   * Indicates whether the secondary serving cell assigned to the instance is in-sync
   * @return true if it is in sync state
   */
  bool is_in_sync() const { return state == STATE_IN_SYNCH; }
};
} // namespace scell
} // namespace srsue
#endif // SRSLTE_SCELL_SYNC_H
