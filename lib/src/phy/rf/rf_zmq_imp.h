/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#ifndef SRSRAN_RF_ZMQ_IMP_H_
#define SRSRAN_RF_ZMQ_IMP_H_

#include <inttypes.h>
#include <stdbool.h>

#include "srsran/config.h"
#include "srsran/phy/rf/rf.h"

#define DEVNAME_ZMQ "ZeroMQ"

extern rf_dev_t srsran_rf_dev_zmq;

SRSRAN_API int rf_zmq_open(char* args, void** handler);

SRSRAN_API int rf_zmq_open_multi(char* args, void** handler, uint32_t nof_channels);

SRSRAN_API const char* rf_zmq_devname(void* h);

SRSRAN_API int rf_zmq_close(void* h);

SRSRAN_API int rf_zmq_start_rx_stream(void* h, bool now);

SRSRAN_API int rf_zmq_start_rx_stream_nsamples(void* h, uint32_t nsamples);

SRSRAN_API int rf_zmq_stop_rx_stream(void* h);

SRSRAN_API void rf_zmq_flush_buffer(void* h);

SRSRAN_API bool rf_zmq_has_rssi(void* h);

SRSRAN_API float rf_zmq_get_rssi(void* h);

SRSRAN_API double rf_zmq_set_rx_srate(void* h, double freq);

SRSRAN_API int rf_zmq_set_rx_gain(void* h, double gain);

SRSRAN_API int rf_zmq_set_rx_gain_ch(void* h, uint32_t ch, double gain);

SRSRAN_API double rf_zmq_get_rx_gain(void* h);

SRSRAN_API double rf_zmq_get_tx_gain(void* h);

SRSRAN_API srsran_rf_info_t* rf_zmq_get_info(void* h);

SRSRAN_API void rf_zmq_suppress_stdout(void* h);

SRSRAN_API void rf_zmq_register_error_handler(void* h, srsran_rf_error_handler_t error_handler, void* arg);

SRSRAN_API double rf_zmq_set_rx_freq(void* h, uint32_t ch, double freq);

SRSRAN_API int
rf_zmq_recv_with_time(void* h, void* data, uint32_t nsamples, bool blocking, time_t* secs, double* frac_secs);

SRSRAN_API int
rf_zmq_recv_with_time_multi(void* h, void** data, uint32_t nsamples, bool blocking, time_t* secs, double* frac_secs);

SRSRAN_API double rf_zmq_set_tx_srate(void* h, double freq);

SRSRAN_API int rf_zmq_set_tx_gain(void* h, double gain);

SRSRAN_API int rf_zmq_set_tx_gain_ch(void* h, uint32_t ch, double gain);

SRSRAN_API double rf_zmq_set_tx_freq(void* h, uint32_t ch, double freq);

SRSRAN_API void rf_zmq_get_time(void* h, time_t* secs, double* frac_secs);

SRSRAN_API int rf_zmq_send_timed(void*  h,
                                 void*  data,
                                 int    nsamples,
                                 time_t secs,
                                 double frac_secs,
                                 bool   has_time_spec,
                                 bool   blocking,
                                 bool   is_start_of_burst,
                                 bool   is_end_of_burst);

SRSRAN_API int rf_zmq_send_timed_multi(void*  h,
                                       void*  data[4],
                                       int    nsamples,
                                       time_t secs,
                                       double frac_secs,
                                       bool   has_time_spec,
                                       bool   blocking,
                                       bool   is_start_of_burst,
                                       bool   is_end_of_burst);

#endif /* SRSRAN_RF_ZMQ_IMP_H_ */
