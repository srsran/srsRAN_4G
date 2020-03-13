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

#ifndef SRSUE_ASYNCH_SCELL_RECV_H
#define SRSUE_ASYNCH_SCELL_RECV_H

#include <srslte/common/log.h>
#include <srslte/common/threads.h>
#include <srslte/radio/radio.h>
#include <srslte/srslte.h>

#include "srsue/hdr/phy/phy_common.h"

namespace srsue {
namespace scell {

#define SF_BUFFER_MAX_SAMPLES (5 * SRSLTE_SF_LEN_MAX)

class async_scell_recv : private srslte::thread
{
public:
  async_scell_recv();
  ~async_scell_recv();

  void init(srslte::radio_interface_phy* _radio_handler, phy_common* _worker_com, srslte::log* _log_h);
  void stop();

  // from chest_feedback_itf
  void in_sync();
  void out_of_sync();
  void set_cfo(float cfo);

  float get_tx_cfo();

  // From UE configuration
  void                 set_agc_enable(bool enable);
  bool                 set_scell_cell(uint32_t carrier_idx, srslte_cell_t* _cell, uint32_t dl_earfcn);
  const srslte_cell_t* get_cell() { return &cell; };

  // Other functions
  void set_rx_gain(float gain);
  int  radio_recv_fnc(cf_t* data[SRSLTE_MAX_PORTS], uint32_t nsamples, srslte_timestamp_t* rx_time);
  bool tti_align(uint32_t tti);
  void read_sf(cf_t** dst, srslte_timestamp_t* timestamp, int* next_offset);

private:
  class phch_scell_recv_buffer
  {
  private:
    uint32_t           tti                      = 0;
    srslte_timestamp_t timestamp                = {};
    int                next_offset              = 0;
    cf_t*              buffer[SRSLTE_MAX_PORTS] = {};

  public:
    phch_scell_recv_buffer() = default;

    ~phch_scell_recv_buffer()
    {
      for (cf_t*& b : buffer) {
        if (b) {
          free(b);
        }
      }
    }

    void init(uint32_t nof_ports)
    {
      for (uint32_t i = 0; i < nof_ports; i++) {
        // It needs to support cell search
        buffer[i] = srslte_vec_cf_malloc(SF_BUFFER_MAX_SAMPLES);
        if (!buffer[i]) {
          fprintf(stderr, "Error allocating buffer\n");
        }
      }
    }

    void set_sf(uint32_t _tti, srslte_timestamp_t* _timestamp, const int& _next_offset)
    {
      tti         = _tti;
      next_offset = _next_offset;
      srslte_timestamp_copy(&timestamp, _timestamp);
    }

    uint32_t get_tti() { return tti; }

    cf_t** get_buffer_ptr() { return buffer; }

    void get_timestamp(srslte_timestamp_t* _timestamp) { srslte_timestamp_copy(_timestamp, &timestamp); }
    void get_next_offset(int* _next_offset)
    {
      if (_next_offset)
        *_next_offset = next_offset;
    }
  };

  static const uint32_t ASYNC_NOF_BUFFERS = SRSLTE_NOF_SF_X_FRAME;
  void                  reset();
  void                  radio_error();
  void                  set_ue_sync_opts(srslte_ue_sync_t* q, float cfo);

  void state_decode_mib();
  void state_write_buffer();
  void state_synch_idle();

  enum receiver_state { DECODE_MIB, WRITE_BUFFER, SYNCH_IDLE } state;
  static const uint32_t max_tti_align_timeout_counter = 10;
  uint32_t              tti_align_timeout_counter;
  srslte_cell_t         cell;

  bool initiated;
  bool running;

  uint32_t tti;
  uint32_t radio_idx;

  // Pointers to other classes
  srslte::log*                 log_h;
  srslte::radio_interface_phy* radio_h;
  phy_common*                  worker_com;

  // pthread objects
  pthread_mutex_t mutex_uesync;
  pthread_mutex_t mutex_buffer;
  pthread_cond_t  cvar_buffer;

  // Object for synchronization of the primary cell
  srslte_ue_sync_t ue_sync;
  srslte_ue_mib_t  ue_mib;

  // Buffer for secondary cell samples
  phch_scell_recv_buffer buffers[ASYNC_NOF_BUFFERS];
  uint32_t               buffer_write_idx;
  uint32_t               buffer_read_idx;

  // in-sync / out-of-sync counters
  uint32_t out_of_sync_cnt;
  uint32_t in_sync_cnt;

  cf_t*    sf_buffer[SRSLTE_MAX_PORTS];
  uint32_t current_sflen;
  int      next_radio_offset;

  const static uint32_t NOF_OUT_OF_SYNC_SF = 200;
  const static uint32_t NOF_IN_SYNC_SF     = 100;

  // This is the secondary cell
  bool started;
  bool do_agc;

  float    ul_dl_factor;
  uint32_t current_earfcn[SRSLTE_MAX_PORTS];

  float dl_freq;
  float ul_freq;

protected:
  void run_thread() override;
};

typedef std::unique_ptr<async_scell_recv> async_recv_ptr;
typedef std::vector<async_recv_ptr>       async_recv_vector;

} // namespace scell
} // namespace srsue

#endif // SRSUE_ASYNCH_SCELL_RECV_H
