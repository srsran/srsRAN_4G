/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
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
  
#ifndef RF_H 
#define RF_H

#include <sys/time.h>
#include <stdbool.h>
#include <stdint.h>
#include "srslte/config.h"

typedef struct {
  void *handler;
  void *dev;
} rf_t;

typedef void (*rf_msg_handler_t)(const char*);

SRSLTE_API int rf_open(rf_t *h, 
                       char *args);

SRSLTE_API int rf_open_th(rf_t *h, 
                          char *args, 
                          bool tx_gain_same_rx);

SRSLTE_API int rf_open_devname(rf_t *h, 
                               char *devname, 
                               char *args, 
                               bool agc_thread, 
                               bool tx_gain_same_rx);

SRSLTE_API int rf_close(rf_t *h);

SRSLTE_API int rf_start_rx_stream(rf_t *h);

SRSLTE_API int rf_stop_rx_stream(rf_t *h);

SRSLTE_API void rf_flush_buffer(rf_t *h);

SRSLTE_API bool rf_has_rssi(rf_t *h);

SRSLTE_API float rf_get_rssi(rf_t *h); 

SRSLTE_API bool rf_rx_wait_lo_locked(rf_t *h);

SRSLTE_API void rf_set_master_clock_rate(rf_t *h, 
                                         double rate); 

SRSLTE_API bool rf_is_master_clock_dynamic(rf_t *h); 

SRSLTE_API double rf_set_rx_srate(rf_t *h, 
                                  double freq);

SRSLTE_API double rf_set_rx_gain(rf_t *h, 
                                 double gain);

SRSLTE_API void rf_set_tx_rx_gain_offset(rf_t *h, 
                                         double offset); 

SRSLTE_API double rf_set_rx_gain_th(rf_t *h, 
                                    double gain);

SRSLTE_API double rf_get_rx_gain(rf_t *h);

SRSLTE_API double rf_get_tx_gain(rf_t *h);

SRSLTE_API void rf_suppress_stdout(rf_t *h);

SRSLTE_API void rf_register_msg_handler(rf_t *h, 
                                        rf_msg_handler_t msg_handler);

SRSLTE_API double rf_set_rx_freq(rf_t *h, 
                                 double freq);

SRSLTE_API int rf_recv(rf_t *h, 
                       void *data, 
                       uint32_t nsamples, 
                       bool blocking);

SRSLTE_API int rf_recv_with_time(rf_t *h,
                                 void *data,
                                 uint32_t nsamples,
                                 bool blocking,
                                 time_t *secs,
                                 double *frac_secs);

SRSLTE_API double rf_set_tx_srate(rf_t *h, 
                                  double freq);

SRSLTE_API double rf_set_tx_gain(rf_t *h, 
                                 double gain);

SRSLTE_API double rf_set_tx_freq(rf_t *h,
                                 double freq);

SRSLTE_API void rf_get_time(rf_t *h, 
                            time_t *secs, 
                            double *frac_secs); 

SRSLTE_API int rf_send(rf_t *h, 
                       void *data, 
                       uint32_t nsamples, 
                       bool blocking);

SRSLTE_API int rf_send2(rf_t *h, 
                        void *data, 
                        uint32_t nsamples, 
                        bool blocking, 
                        bool start_of_burst, 
                        bool end_of_burst); 

SRSLTE_API int rf_send(rf_t *h, 
                       void *data, 
                       uint32_t nsamples, 
                       bool blocking);


SRSLTE_API int rf_send_timed(rf_t *h,
                             void *data,
                             int nsamples,
                             time_t secs,
                             double frac_secs);

SRSLTE_API int rf_send_timed2(rf_t *h,
                              void *data,
                              int nsamples,
                              time_t secs,
                              double frac_secs,                      
                              bool is_start_of_burst,
                              bool is_end_of_burst);

#endif

