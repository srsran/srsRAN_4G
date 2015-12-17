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



#ifdef __cplusplus
extern "C" {
#endif
#include <stdbool.h>
#include <stdint.h>
  
#include "srslte/config.h"

typedef void (*cuhd_msg_handler_t)(const char*);

SRSLTE_API int cuhd_open(char *args, 
                         void **handler);

SRSLTE_API int cuhd_open_th(char *args, 
                            void **handler, 
                            bool tx_gain_same_rx);

SRSLTE_API int cuhd_close(void *h);

SRSLTE_API int cuhd_start_rx_stream(void *h);

SRSLTE_API int cuhd_start_rx_stream_nsamples(void *h, 
                                             uint32_t nsamples);

SRSLTE_API int cuhd_stop_rx_stream(void *h);

SRSLTE_API void cuhd_flush_buffer(void *h);

SRSLTE_API bool cuhd_has_rssi(void *h);

SRSLTE_API float cuhd_get_rssi(void *h); 

SRSLTE_API bool cuhd_rx_wait_lo_locked(void *h);

SRSLTE_API void cuhd_set_master_clock_rate(void *h, 
                                           double rate); 

SRSLTE_API bool cuhd_is_master_clock_dynamic(void *h); 

SRSLTE_API double cuhd_set_rx_srate(void *h, 
                                    double freq);

SRSLTE_API double cuhd_set_rx_gain(void *h, 
                                   double gain);

SRSLTE_API void cuhd_set_tx_rx_gain_offset(void *h, 
                                           double offset); 

SRSLTE_API double cuhd_set_rx_gain_th(void *h, 
                                      double gain);

SRSLTE_API double cuhd_set_tx_gain_th(void *h, 
                                      double gain);

SRSLTE_API float cuhd_get_rx_gain_offset(void *h); 

SRSLTE_API double cuhd_get_rx_gain(void *h);

SRSLTE_API double cuhd_get_tx_gain(void *h);

SRSLTE_API void cuhd_suppress_stdout();

SRSLTE_API void cuhd_register_msg_handler(cuhd_msg_handler_t h);

SRSLTE_API double cuhd_set_rx_freq(void *h, 
                                   double freq);

SRSLTE_API double cuhd_set_rx_freq_offset(void *h, 
                                          double freq,  
                                          double off); 

SRSLTE_API double cuhd_set_rx_freq_offset(void *h, 
                                          double freq, 
                                          double off);

SRSLTE_API int cuhd_recv(void *h, 
                         void *data, 
                         uint32_t nsamples, 
                         bool blocking);

SRSLTE_API int cuhd_recv_with_time(void *h,
                                   void *data,
                                   uint32_t nsamples,
                                   bool blocking,
                                   time_t *secs,
                                   double *frac_secs);

SRSLTE_API double cuhd_set_tx_srate(void *h, 
                                    double freq);

SRSLTE_API double cuhd_set_tx_gain(void *h, 
                                   double gain);

SRSLTE_API double cuhd_set_tx_freq(void *h,
                                   double freq);

SRSLTE_API double cuhd_set_tx_freq_offset(void *h,
                                          double freq, 
                                          double offset);

SRSLTE_API void cuhd_get_time(void *h, 
                              time_t *secs, 
                              double *frac_secs); 

SRSLTE_API int cuhd_send(void *h, 
                         void *data, 
                         uint32_t nsamples, 
                         bool blocking);

SRSLTE_API int cuhd_send2(void *h, 
                          void *data, 
                          uint32_t nsamples, 
                          bool blocking, 
                          bool start_of_burst, 
                          bool end_of_burst); 

SRSLTE_API int cuhd_send(void *h, 
                         void *data, 
                         uint32_t nsamples, 
                         bool blocking);


SRSLTE_API int cuhd_send_timed(void *h,
                               void *data,
                               int nsamples,
                               time_t secs,
                               double frac_secs);

SRSLTE_API int cuhd_send_timed2(void *h,
                                void *data,
                                int nsamples,
                                time_t secs,
                                double frac_secs,                      
                                bool is_start_of_burst,
                                bool is_end_of_burst);

#ifdef __cplusplus
}
#endif
