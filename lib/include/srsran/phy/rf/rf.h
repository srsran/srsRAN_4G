/**
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSRAN_RF_H
#define SRSRAN_RF_H

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/time.h>

#include "srsran/config.h"

#ifdef __cplusplus
extern "C" {
#endif

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
} srsran_rf_t;

typedef struct {
  double min_tx_gain;
  double max_tx_gain;
  double min_rx_gain;
  double max_rx_gain;
} srsran_rf_info_t;

typedef struct {
  enum {
    SRSRAN_RF_ERROR_LATE,
    SRSRAN_RF_ERROR_UNDERFLOW,
    SRSRAN_RF_ERROR_OVERFLOW,
    SRSRAN_RF_ERROR_RX,
    SRSRAN_RF_ERROR_OTHER
  } type;
  int         opt;
  const char* msg;
} srsran_rf_error_t;

typedef void (*srsran_rf_error_handler_t)(void* arg, srsran_rf_error_t error);

/* RF frontend API */
typedef struct {
  const char* name;
  const char* (*srsran_rf_devname)(void* h);
  int (*srsran_rf_start_rx_stream)(void* h, bool now);
  int (*srsran_rf_stop_rx_stream)(void* h);
  void (*srsran_rf_flush_buffer)(void* h);
  bool (*srsran_rf_has_rssi)(void* h);
  float (*srsran_rf_get_rssi)(void* h);
  void (*srsran_rf_suppress_stdout)(void* h);
  void (*srsran_rf_register_error_handler)(void* h, srsran_rf_error_handler_t error_handler, void* arg);
  int (*srsran_rf_open)(char* args, void** h);
  int (*srsran_rf_open_multi)(char* args, void** h, uint32_t nof_channels);
  int (*srsran_rf_close)(void* h);
  double (*srsran_rf_set_rx_srate)(void* h, double freq);
  int (*srsran_rf_set_rx_gain)(void* h, double gain);
  int (*srsran_rf_set_rx_gain_ch)(void* h, uint32_t ch, double gain);
  int (*srsran_rf_set_tx_gain)(void* h, double gain);
  int (*srsran_rf_set_tx_gain_ch)(void* h, uint32_t ch, double gain);
  double (*srsran_rf_get_rx_gain)(void* h);
  double (*srsran_rf_get_tx_gain)(void* h);
  srsran_rf_info_t* (*srsran_rf_get_info)(void* h);
  double (*srsran_rf_set_rx_freq)(void* h, uint32_t ch, double freq);
  double (*srsran_rf_set_tx_srate)(void* h, double freq);
  double (*srsran_rf_set_tx_freq)(void* h, uint32_t ch, double freq);
  void (*srsran_rf_get_time)(void* h, time_t* secs, double* frac_secs);
  void (*srsran_rf_sync_pps)(void* h);
  int (*srsran_rf_recv_with_time)(void*    h,
                                  void*    data,
                                  uint32_t nsamples,
                                  bool     blocking,
                                  time_t*  secs,
                                  double*  frac_secs);
  int (*srsran_rf_recv_with_time_multi)(void*    h,
                                        void**   data,
                                        uint32_t nsamples,
                                        bool     blocking,
                                        time_t*  secs,
                                        double*  frac_secs);
  int (*srsran_rf_send_timed)(void*  h,
                              void*  data,
                              int    nsamples,
                              time_t secs,
                              double frac_secs,
                              bool   has_time_spec,
                              bool   blocking,
                              bool   is_start_of_burst,
                              bool   is_end_of_burst);
  int (*srsran_rf_send_timed_multi)(void*  h,
                                    void** data,
                                    int    nsamples,
                                    time_t secs,
                                    double frac_secs,
                                    bool   has_time_spec,
                                    bool   blocking,
                                    bool   is_start_of_burst,
                                    bool   is_end_of_burst);
} rf_dev_t;

typedef struct {
  const char* plugin_name;
  void*       dl_handle;
  rf_dev_t*   rf_api;
} srsran_rf_plugin_t;

SRSRAN_API int srsran_rf_load_plugins();

SRSRAN_API int srsran_rf_open(srsran_rf_t* h, char* args);

SRSRAN_API int srsran_rf_open_multi(srsran_rf_t* h, char* args, uint32_t nof_channels);

SRSRAN_API int srsran_rf_open_devname(srsran_rf_t* h, const char* devname, char* args, uint32_t nof_channels);

/**
 * @brief Opens a file-based RF abstraction
 * @param[out] rf Device handle
 * @param[in] rx_files List of pre-opened FILE* for each RX channel; NULL to disable
 * @param[in] tx_files List of pre-opened FILE* for each TX channel; NULL to disable
 * @param[in] nof_channels Number of channels per direction
 * @param[in] base_srate Sample rate of RX and TX files
 * @return SRSRAN_SUCCESS on success, otherwise error code
 */
SRSRAN_API int
srsran_rf_open_file(srsran_rf_t* rf, FILE** rx_files, FILE** tx_files, uint32_t nof_channels, uint32_t base_srate);

SRSRAN_API const char* srsran_rf_name(srsran_rf_t* h);

SRSRAN_API int srsran_rf_start_gain_thread(srsran_rf_t* rf, bool tx_gain_same_rx);

SRSRAN_API int srsran_rf_close(srsran_rf_t* h);

SRSRAN_API int srsran_rf_start_rx_stream(srsran_rf_t* h, bool now);

SRSRAN_API int srsran_rf_stop_rx_stream(srsran_rf_t* h);

SRSRAN_API void srsran_rf_flush_buffer(srsran_rf_t* h);

SRSRAN_API bool srsran_rf_has_rssi(srsran_rf_t* h);

SRSRAN_API float srsran_rf_get_rssi(srsran_rf_t* h);

SRSRAN_API double srsran_rf_set_rx_srate(srsran_rf_t* h, double freq);

SRSRAN_API int srsran_rf_set_rx_gain(srsran_rf_t* h, double gain);

SRSRAN_API int srsran_rf_set_rx_gain_ch(srsran_rf_t* h, uint32_t ch, double gain);

SRSRAN_API void srsran_rf_set_tx_rx_gain_offset(srsran_rf_t* h, double offset);

SRSRAN_API int srsran_rf_set_rx_gain_th(srsran_rf_t* h, double gain);

SRSRAN_API double srsran_rf_get_rx_gain(srsran_rf_t* h);

SRSRAN_API double srsran_rf_get_tx_gain(srsran_rf_t* h);

SRSRAN_API srsran_rf_info_t* srsran_rf_get_info(srsran_rf_t* h);

SRSRAN_API void srsran_rf_suppress_stdout(srsran_rf_t* h);

SRSRAN_API void srsran_rf_register_error_handler(srsran_rf_t* h, srsran_rf_error_handler_t error_handler, void* arg);

SRSRAN_API double srsran_rf_set_rx_freq(srsran_rf_t* h, uint32_t ch, double freq);

SRSRAN_API int srsran_rf_recv(srsran_rf_t* h, void* data, uint32_t nsamples, bool blocking);

SRSRAN_API int
srsran_rf_recv_with_time(srsran_rf_t* h, void* data, uint32_t nsamples, bool blocking, time_t* secs, double* frac_secs);

SRSRAN_API int srsran_rf_recv_with_time_multi(srsran_rf_t* h,
                                              void**       data,
                                              uint32_t     nsamples,
                                              bool         blocking,
                                              time_t*      secs,
                                              double*      frac_secs);

SRSRAN_API double srsran_rf_set_tx_srate(srsran_rf_t* h, double freq);

SRSRAN_API int srsran_rf_set_tx_gain(srsran_rf_t* h, double gain);

SRSRAN_API int srsran_rf_set_tx_gain_ch(srsran_rf_t* h, uint32_t ch, double gain);

SRSRAN_API double srsran_rf_set_tx_freq(srsran_rf_t* h, uint32_t ch, double freq);

SRSRAN_API void srsran_rf_get_time(srsran_rf_t* h, time_t* secs, double* frac_secs);

SRSRAN_API int srsran_rf_sync(srsran_rf_t* rf);

SRSRAN_API int srsran_rf_send(srsran_rf_t* h, void* data, uint32_t nsamples, bool blocking);

SRSRAN_API int
srsran_rf_send2(srsran_rf_t* h, void* data, uint32_t nsamples, bool blocking, bool start_of_burst, bool end_of_burst);

SRSRAN_API int srsran_rf_send(srsran_rf_t* h, void* data, uint32_t nsamples, bool blocking);

SRSRAN_API int srsran_rf_send_timed(srsran_rf_t* h, void* data, int nsamples, time_t secs, double frac_secs);

SRSRAN_API int srsran_rf_send_timed2(srsran_rf_t* h,
                                     void*        data,
                                     int          nsamples,
                                     time_t       secs,
                                     double       frac_secs,
                                     bool         is_start_of_burst,
                                     bool         is_end_of_burst);

SRSRAN_API int srsran_rf_send_timed_multi(srsran_rf_t* rf,
                                          void**       data,
                                          int          nsamples,
                                          time_t       secs,
                                          double       frac_secs,
                                          bool         blocking,
                                          bool         is_start_of_burst,
                                          bool         is_end_of_burst);

SRSRAN_API int srsran_rf_send_multi(srsran_rf_t* rf,
                                    void**       data,
                                    int          nsamples,
                                    bool         blocking,
                                    bool         is_start_of_burst,
                                    bool         is_end_of_burst);

#ifdef __cplusplus
}
#endif

#endif // SRSRAN_RF_H
