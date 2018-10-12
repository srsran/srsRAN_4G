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

#ifndef SHMEM_RF_DEV
#define SHMEM_RF_DEV

#include <stdbool.h>
#include <stdint.h>
  
#include "srslte/config.h"
#include "srslte/phy/rf/rf.h"

SRSLTE_API   char*  rf_shmem_devname (void *h);

SRSLTE_API   bool   rf_shmem_rx_wait_lo_locked(void *h);

SRSLTE_API   int    rf_shmem_start_rx_stream(void *h, bool now);

SRSLTE_API   int    rf_shmem_stop_rx_stream(void *h);

SRSLTE_API   void   rf_shmem_flush_buffer(void *h);

SRSLTE_API   bool   rf_shmem_has_rssi(void *h);

SRSLTE_API   float  rf_shmem_get_rssi(void *h);

SRSLTE_API   void   rf_shmem_suppress_stdout(void *h);

SRSLTE_API   void   rf_shmem_register_error_handler(void *h, 
                                                   srslte_rf_error_handler_t error_handler);

SRSLTE_API   int    rf_shmem_open(char *args, void **h);

SRSLTE_API   int    rf_shmem_open_multi(char *args, void **h, uint32_t nof_channels);

SRSLTE_API   int    rf_shmem_close(void *h);

SRSLTE_API   void   rf_shmem_set_master_clock_rate(void *h, double rate);

SRSLTE_API   bool   rf_shmem_is_master_clock_dynamic(void *h);

SRSLTE_API   double rf_shmem_set_rx_srate(void *h, double freq);

SRSLTE_API   double rf_shmem_set_rx_gain(void *h, double gain);

SRSLTE_API   double rf_shmem_set_tx_gain(void *h, double gain);

SRSLTE_API   double rf_shmem_get_rx_gain(void *h);

SRSLTE_API   double rf_shmem_get_tx_gain(void *h);

SRSLTE_API   srslte_rf_info_t * rf_shmem_get_rf_info(void *h);

SRSLTE_API   double rf_shmem_set_rx_freq(void *h, double freq);  

SRSLTE_API   double rf_shmem_set_tx_srate(void *h, double freq);

SRSLTE_API   double rf_shmem_set_tx_freq(void *h, double freq);

SRSLTE_API   void   rf_shmem_get_time(void *h, time_t *secs, double *frac_secs);  

SRSLTE_API   int    rf_shmem_recv_with_time(void *h, 
                                           void *data, 
                                           uint32_t nsamples, 
                                           bool blocking,
                                           time_t *secs,
                                           double *frac_secs);

SRSLTE_API   int    rf_shmem_recv_with_time_multi(void *h, 
                                                 void **data,
                                                 uint32_t nsamples, 
                                                 bool blocking,
                                                 time_t *secs,
                                                 double *frac_secs);

SRSLTE_API   int    rf_shmem_send_timed(void *h, 
                                       void *data,
                                       int nsamples,
                                       time_t secs,
                                       double frac_secs,
                                       bool has_time_spec,
                                       bool blocking,
                                       bool is_start_of_burst,
                                       bool is_end_of_burst);

SRSLTE_API   int    rf_shmem_send_timed_multi(void *h,
                                             void *data[4],
                                             int nsamples,
                                             time_t secs,
                                             double frac_secs,
                                             bool has_time_spec,
                                             bool blocking,
                                             bool is_start_of_burst,
                                             bool is_end_of_burst);

#endif
