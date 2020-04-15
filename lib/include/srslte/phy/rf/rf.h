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

#ifndef SRSLTE_RF_H
#define SRSLTE_RF_H

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/time.h>

#include "srslte/config.h"

#define RF_PARAM_LEN (256)

typedef struct {
  void* handler;
  void* dev;

  // The following variables are for threaded RX gain control
  bool            thread_gain_run;
  pthread_t       thread_gain;
  pthread_cond_t  cond;
  pthread_mutex_t mutex;
  double          cur_rx_gain;
  double          new_rx_gain;
  bool            tx_gain_same_rx;
  float           tx_rx_gain_offset;
} srslte_rf_t;

typedef struct {
  double min_tx_gain;
  double max_tx_gain;
  double min_rx_gain;
  double max_rx_gain;
} srslte_rf_info_t;

typedef struct {
  enum {
    SRSLTE_RF_ERROR_LATE,
    SRSLTE_RF_ERROR_UNDERFLOW,
    SRSLTE_RF_ERROR_OVERFLOW,
    SRSLTE_RF_ERROR_RX,
    SRSLTE_RF_ERROR_OTHER
  } type;
  int         opt;
  const char* msg;
} srslte_rf_error_t;

typedef void (*srslte_rf_error_handler_t)(void* arg, srslte_rf_error_t error);

SRSLTE_API int srslte_rf_open(srslte_rf_t* h, char* args);

SRSLTE_API int srslte_rf_open_multi(srslte_rf_t* h, char* args, uint32_t nof_channels);

SRSLTE_API int srslte_rf_open_devname(srslte_rf_t* h, const char* devname, char* args, uint32_t nof_channels);

SRSLTE_API const char* srslte_rf_name(srslte_rf_t* h);

SRSLTE_API int srslte_rf_start_gain_thread(srslte_rf_t* rf, bool tx_gain_same_rx);

SRSLTE_API int srslte_rf_close(srslte_rf_t* h);

SRSLTE_API int srslte_rf_start_rx_stream(srslte_rf_t* h, bool now);

SRSLTE_API int srslte_rf_stop_rx_stream(srslte_rf_t* h);

SRSLTE_API void srslte_rf_flush_buffer(srslte_rf_t* h);

SRSLTE_API bool srslte_rf_has_rssi(srslte_rf_t* h);

SRSLTE_API float srslte_rf_get_rssi(srslte_rf_t* h);

SRSLTE_API double srslte_rf_set_rx_srate(srslte_rf_t* h, double freq);

SRSLTE_API double srslte_rf_set_rx_gain(srslte_rf_t* h, double gain);

SRSLTE_API void srslte_rf_set_tx_rx_gain_offset(srslte_rf_t* h, double offset);

SRSLTE_API double srslte_rf_set_rx_gain_th(srslte_rf_t* h, double gain);

SRSLTE_API double srslte_rf_get_rx_gain(srslte_rf_t* h);

SRSLTE_API double srslte_rf_get_tx_gain(srslte_rf_t* h);

SRSLTE_API srslte_rf_info_t* srslte_rf_get_info(srslte_rf_t* h);

SRSLTE_API void srslte_rf_suppress_stdout(srslte_rf_t* h);

SRSLTE_API void srslte_rf_register_error_handler(srslte_rf_t* h, srslte_rf_error_handler_t error_handler, void* arg);

SRSLTE_API double srslte_rf_set_rx_freq(srslte_rf_t* h, uint32_t ch, double freq);

SRSLTE_API int srslte_rf_recv(srslte_rf_t* h, void* data, uint32_t nsamples, bool blocking);

SRSLTE_API int
srslte_rf_recv_with_time(srslte_rf_t* h, void* data, uint32_t nsamples, bool blocking, time_t* secs, double* frac_secs);

SRSLTE_API int srslte_rf_recv_with_time_multi(srslte_rf_t* h,
                                              void**       data,
                                              uint32_t     nsamples,
                                              bool         blocking,
                                              time_t*      secs,
                                              double*      frac_secs);

SRSLTE_API double srslte_rf_set_tx_srate(srslte_rf_t* h, double freq);

SRSLTE_API double srslte_rf_set_tx_gain(srslte_rf_t* h, double gain);

SRSLTE_API double srslte_rf_set_tx_freq(srslte_rf_t* h, uint32_t ch, double freq);

SRSLTE_API void srslte_rf_get_time(srslte_rf_t* h, time_t* secs, double* frac_secs);

SRSLTE_API int srslte_rf_sync(srslte_rf_t* rf);

SRSLTE_API int srslte_rf_send(srslte_rf_t* h, void* data, uint32_t nsamples, bool blocking);

SRSLTE_API int
srslte_rf_send2(srslte_rf_t* h, void* data, uint32_t nsamples, bool blocking, bool start_of_burst, bool end_of_burst);

SRSLTE_API int srslte_rf_send(srslte_rf_t* h, void* data, uint32_t nsamples, bool blocking);

SRSLTE_API int srslte_rf_send_timed(srslte_rf_t* h, void* data, int nsamples, time_t secs, double frac_secs);

SRSLTE_API int srslte_rf_send_timed2(srslte_rf_t* h,
                                     void*        data,
                                     int          nsamples,
                                     time_t       secs,
                                     double       frac_secs,
                                     bool         is_start_of_burst,
                                     bool         is_end_of_burst);

SRSLTE_API int srslte_rf_send_timed_multi(srslte_rf_t* rf,
                                          void**       data,
                                          int          nsamples,
                                          time_t       secs,
                                          double       frac_secs,
                                          bool         blocking,
                                          bool         is_start_of_burst,
                                          bool         is_end_of_burst);

SRSLTE_API int srslte_rf_send_multi(srslte_rf_t* rf,
                                    void**       data,
                                    int          nsamples,
                                    bool         blocking,
                                    bool         is_start_of_burst,
                                    bool         is_end_of_burst);

#endif // SRSLTE_RF_H
