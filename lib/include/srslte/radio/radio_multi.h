/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2017 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of srsLTE.
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

#include <string.h>

#include "srslte/srslte.h"
extern "C" {
#include "srslte/phy/rf/rf.h"
}
#include "srslte/common/trace.h"

#include "srslte/radio/radio.h"

#ifndef SRSLTE_RADIO_MULTI_H
#define SRSLTE_RADIO_MULTI_H


namespace srslte {

/* Interface to the RF frontend. 
  */
class radio_multi
{
private:
  /* Temporal buffer size for flushing the radios */
  static const size_t TEMP_BUFFER_SIZE = 307200;

  /* Maximum sample offset that can be compensated without isssuing PPS synchronism */
  static const size_t MAX_NOF_ALIGN_SAMPLES = TEMP_BUFFER_SIZE * 10;

  log_filter*                 log_h;
  bool                        initiated;
  double                      rx_srate;
  std::vector<srslte::radio*> radios;
  srslte_timestamp_t          ts_rx[SRSLTE_MAX_RADIOS];
  cf_t*                       temp_buffers[SRSLTE_MAX_RADIOS][SRSLTE_MAX_PORTS];
  bool                        align_radio_ts();
  bool                        synch_wait();
  void                        synch_issue();
  pthread_mutex_t             mutex;
  bool                        locked;
  uint32_t                    nof_ports;

public:
  radio_multi();
  bool init(log_filter* _log_h,
            char*       args[SRSLTE_MAX_RADIOS] = NULL,
            char*       devname                 = NULL,
            uint32_t    nof_radios              = 1,
            uint32_t    nof_rf_ports            = 1);
  void stop();
  void reset();

  bool start_agc(bool tx_gain_same_rx = false);

  void set_burst_preamble(double preamble_us);
  void set_tx_adv(int nsamples);
  void set_tx_adv_neg(bool tx_adv_is_neg);

  void set_manual_calibration(rf_cal_t* calibration);

  bool is_continuous_tx();
  void set_continuous_tx(bool enable);

  bool tx_single(cf_t* buffer, uint32_t nof_samples, srslte_timestamp_t tx_time);
  bool tx(cf_t* buffer[SRSLTE_MAX_RADIOS][SRSLTE_MAX_PORTS], uint32_t nof_samples, srslte_timestamp_t tx_time);
  void tx_end();
  bool rx_now(cf_t* buffer[SRSLTE_MAX_RADIOS][SRSLTE_MAX_PORTS], uint32_t nof_samples, srslte_timestamp_t* rxd_time);

  void   set_tx_gain(float gain, uint32_t radio_idx = UINT32_MAX);
  void   set_rx_gain(float gain, uint32_t radio_idx = UINT32_MAX);
  void   set_tx_rx_gain_offset(float offset);
  double set_rx_gain_th(float gain);

  float get_tx_gain(uint32_t radio_idx = 0);
  float get_rx_gain(uint32_t radio_idx = 0);

  void set_freq_offset(double freq);
  void set_tx_freq(double freq, uint32_t radio_idx = UINT32_MAX);
  void set_rx_freq(double freq, uint32_t radio_idx = UINT32_MAX);

  double            get_freq_offset();
  double            get_tx_freq(uint32_t radio_idx = 0);
  double            get_rx_freq(uint32_t radio_idx = 0);
  srslte_rf_info_t* get_info(uint32_t radio_idx = 0);

  void set_master_clock_rate(double rate);
  void set_tx_srate(double srate);
  void set_rx_srate(double srate);

  float  get_max_tx_power();
  float  set_tx_power(float power);
  float  get_rssi();
  bool   has_rssi();
  radio* get_radio_ptr(uint32_t idx);

  void start_trace();
  void write_trace(std::string filename);

  void set_tti(uint32_t tti);

  bool is_init();

  void register_error_handler(srslte_rf_error_handler_t h);
};
}

#endif // SRSLTE_RADIO_MULTI_H
