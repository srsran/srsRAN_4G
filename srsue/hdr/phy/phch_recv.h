/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSUE_PHCH_RECV_H
#define SRSUE_PHCH_RECV_H

#include <map>

#include "srslte/srslte.h"
#include "srslte/common/log.h"
#include "srslte/common/threads.h"
#include "srslte/common/thread_pool.h"
#include "srslte/common/tti_sync_cv.h"
#include "srslte/radio/radio_multi.h"
#include "prach.h"
#include "phch_worker.h"
#include "phch_common.h"
#include "srslte/interfaces/ue_interfaces.h"

namespace srsue {
    
typedef _Complex float cf_t;


class phch_recv : public thread, public chest_feedback_itf
{
public:
  phch_recv();
  ~phch_recv();
  void init(srslte::radio_multi* radio_handler, mac_interface_phy *mac,rrc_interface_phy *rrc,
            prach *prach_buffer, srslte::thread_pool *_workers_pool,
            phch_common *_worker_com, srslte::log* _log_h, srslte::log *_log_phy_lib_h, uint32_t nof_rx_antennas, uint32_t prio, int sync_cpu_affinity = -1);
  void stop();
  void set_agc_enable(bool enable);

  void    set_earfcn(std::vector<uint32_t> earfcn);
  void    force_freq(float dl_freq, float ul_freq);
  
  void    reset_sync();
  void    cell_search_start();
  void    cell_search_next(bool reset = false);
  void    cell_select(uint32_t earfcn, srslte_cell_t cell);
  bool    cell_handover(srslte_cell_t cell);

  void    meas_reset();
  int     meas_start(uint32_t earfcn, int pci);
  int     meas_stop(uint32_t earfcn, int pci);

  uint32_t get_current_tti();
  
  bool    status_is_sync();

  // from chest_feedback_itf
  void    in_sync();
  void    out_of_sync();
  void    set_cfo(float cfo);

  void    set_time_adv_sec(float time_adv_sec);
  void    get_current_cell(srslte_cell_t *cell, uint32_t *earfcn = NULL);


  const static int MUTEX_X_WORKER = 4;

  double set_rx_gain(double gain);
  int radio_recv_fnc(cf_t *data[SRSLTE_MAX_PORTS], uint32_t nsamples, srslte_timestamp_t *rx_time);
  int scell_recv_fnc(cf_t *data[SRSLTE_MAX_PORTS], uint32_t nsamples, srslte_timestamp_t *rx_time);

private:

  std::vector<uint32_t> earfcn;

  void   reset();
  void   radio_error();
  void   set_ue_sync_opts(srslte_ue_sync_t *q, float cfo);
  void   run_thread();

  void   set_sampling_rate();
  bool   set_frequency();
  bool   set_cell();

  void   cell_search_inc();
  void   cell_reselect();

  float  get_cfo();

  uint32_t new_earfcn;
  srslte_cell_t new_cell;

  bool   running;

  // Class to run cell search
  class search {
  public:
    typedef enum {CELL_NOT_FOUND, CELL_FOUND, ERROR, TIMEOUT} ret_code;

    ~search();
    void     init(cf_t *buffer[SRSLTE_MAX_PORTS], srslte::log *log_h, uint32_t nof_rx_antennas, phch_recv *parent);
    void     reset();
    float    get_last_gain();
    float    get_last_cfo();
    void     set_N_id_2(int N_id_2);
    void     set_agc_enable(bool enable);
    ret_code run(srslte_cell_t *cell);

  private:
    phch_recv              *p;
    srslte::log            *log_h;
    cf_t                   *buffer[SRSLTE_MAX_PORTS];
    srslte_ue_cellsearch_t  cs;
    srslte_ue_mib_sync_t    ue_mib_sync;
    int                     force_N_id_2;
  };

  // Class to synchronize system frame number
  class sfn_sync {
  public:
    typedef enum {IDLE, SFN_FOUND, SFX0_FOUND, ERROR, TIMEOUT} ret_code;

    ~sfn_sync();
    void     init(srslte_ue_sync_t *ue_sync, cf_t *buffer[SRSLTE_MAX_PORTS], srslte::log *log_h, uint32_t timeout = SYNC_SFN_TIMEOUT);
    void     reset();
    bool     set_cell(srslte_cell_t cell);
    ret_code run_subframe(srslte_cell_t *cell, uint32_t *tti_cnt, bool sfidx_only = false);

  private:
    srslte::log      *log_h;
    srslte_ue_sync_t *ue_sync;
    cf_t             *buffer[SRSLTE_MAX_PORTS];
    srslte_ue_mib_t   ue_mib;
    uint32_t          cnt;
    uint32_t          timeout;
    const static uint32_t SYNC_SFN_TIMEOUT = 80;
  };

  // Class to perform cell measurements
  class measure {

    // TODO: This class could early stop once the variance between the last N measurements is below 3GPP requirements

  public:
    typedef enum {IDLE, MEASURE_OK, ERROR} ret_code;

    ~measure();
    void      init(cf_t *buffer[SRSLTE_MAX_PORTS], srslte::log *log_h,
                   uint32_t nof_rx_antennas, uint32_t nof_subframes = RSRP_MEASURE_NOF_FRAMES);
    void      reset();
    void      set_cell(srslte_cell_t cell);
    ret_code  run_subframe(uint32_t sf_idx);
    ret_code  run_subframe_sync(srslte_ue_sync_t *ue_sync, uint32_t sf_idx);
    ret_code  run_multiple_subframes(cf_t *buffer, int offset, uint32_t sf_idx, uint32_t nof_sf);
    float     rssi();
    float     rsrp();
    float     rsrq();
    float     snr();
    uint32_t  frame_st_idx();
    void      set_rx_gain_offset(float rx_gain_offset);
  private:
    srslte::log      *log_h;
    srslte_ue_dl_t    ue_dl;
    cf_t              *buffer[SRSLTE_MAX_PORTS];
    uint32_t cnt;
    uint32_t nof_subframes;
    uint32_t current_prb;
    float rx_gain_offset;
    float mean_rsrp, mean_rsrq, mean_snr, mean_rssi;
    uint32_t final_offset;
    const static int RSRP_MEASURE_NOF_FRAMES = 5;
  };

  // Class to receive secondary cell
  class scell_recv {
  public:
    const static int MAX_CELLS = 8;
    typedef struct {
      uint32_t pci;
      float    rsrp;
      float    rsrq;
      uint32_t offset;
    } cell_info_t;
    void init(srslte::log *log_h, bool sic_pss_enabled, uint32_t max_sf_window);
    void deinit();
    void reset();
    int find_cells(cf_t *input_buffer, float rx_gain_offset, srslte_cell_t current_cell, uint32_t nof_sf, cell_info_t found_cells[MAX_CELLS]);
  private:

    cf_t               *sf_buffer[SRSLTE_MAX_PORTS];
    srslte::log        *log_h;
    srslte_sync_t       sync_find;

    bool       sic_pss_enabled;
    uint32_t   current_fft_sz;
    measure    measure_p;
  };

  /* TODO: Intra-freq measurements can be improved by capturing 200 ms length signal and run cell search +
   * measurements offline using sync object and finding multiple cells for each N_id_2
   */

  // Class to perform intra-frequency measurements
  class intra_measure : public thread {
  public:
    ~intra_measure();
    void init(phch_common *common, rrc_interface_phy *rrc, srslte::log *log_h);
    void stop();
    void add_cell(int pci);
    void rem_cell(int pci);
    void set_primay_cell(uint32_t earfcn, srslte_cell_t cell);
    void clear_cells();
    int  get_offset(uint32_t pci);
    void write(uint32_t tti, cf_t *data, uint32_t nsamples);
  private:
    void run_thread();
    const static int INTRA_FREQ_MEAS_PRIO      = DEFAULT_PRIORITY + 5;

    scell_recv         scell;
    rrc_interface_phy  *rrc;
    srslte::log        *log_h;
    phch_common        *common;
    uint32_t           current_earfcn;
    uint32_t           current_sflen;
    srslte_cell_t      primary_cell;
    std::vector<int> active_pci;

    srslte::tti_sync_cv tti_sync;

    cf_t               *search_buffer;

    scell_recv::cell_info_t info[scell_recv::MAX_CELLS];

    bool                running;
    bool                receive_enabled;
    bool                receiving;
    uint32_t            measure_tti;
    uint32_t            receive_cnt;
    srslte_ringbuffer_t ring_buffer;
  };

  // 36.133 9.1.2.1 for band 7
  const static float ABSOLUTE_RSRP_THRESHOLD_DBM = -125;


  // Objects for internal use
  measure               measure_p;
  search                search_p;
  sfn_sync              sfn_p;
  intra_measure         intra_freq_meas;

  uint32_t              current_sflen;
  int                   next_offset;
  uint32_t              nof_rx_antennas;

  // Pointers to other classes
  mac_interface_phy    *mac;
  rrc_interface_phy    *rrc;
  srslte::log          *log_h;
  srslte::log          *log_phy_lib_h;
  srslte::thread_pool  *workers_pool;
  srslte::radio_multi  *radio_h;
  phch_common          *worker_com;
  prach                *prach_buffer;

  // Object for synchronization of the primary cell
  srslte_ue_sync_t      ue_sync;

  // Buffer for primary cell samples
  cf_t                 *sf_buffer[SRSLTE_MAX_PORTS];

  // Sync metrics
  sync_metrics_t        metrics;

  // in-sync / out-of-sync counters
  uint32_t out_of_sync_cnt;
  uint32_t in_sync_cnt;

  const static uint32_t NOF_OUT_OF_SYNC_SF = 200;
  const static uint32_t NOF_IN_SYNC_SF     = 100;

  // State for primary cell
  typedef enum {
    IDLE = 0,
    CELL_SEARCH,
    CELL_SELECT,
    CELL_RESELECT,
    CELL_MEASURE,
    CELL_CAMP,
  } phy_state_t;

  phy_state_t phy_state, prev_state;

  bool is_in_idle;

  // Sampling rate mode (find is 1.96 MHz, camp is the full cell BW)
  enum {
    SRATE_NONE=0, SRATE_FIND, SRATE_CAMP
  } srate_mode;
  float         current_srate;

  // This is the primary cell
  srslte_cell_t cell;
  bool          cell_is_set;
  bool          started;
  float         time_adv_sec;
  uint32_t      tti;
  bool          do_agc;
  
  uint32_t      nof_tx_mutex;
  uint32_t      tx_mutex_cnt;

  float         ul_dl_factor;
  uint32_t      current_earfcn;
  int           cur_earfcn_index;

  float         dl_freq;
  float         ul_freq;

};

} // namespace srsue

#endif // SRSUE_PHCH_RECV_H
