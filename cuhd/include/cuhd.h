/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The libLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the libLTE library.
 *
 * libLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * libLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */



#ifdef __cplusplus
extern "C" {
#endif
#include <stdbool.h>

#include "cuhd/cuhd_utils.h"

int cuhd_open(char *args, void **handler);
int cuhd_close(void *h);

int cuhd_start_rx_stream(void *h);
int cuhd_start_rx_stream_nsamples(void *h, int nsamples);
int cuhd_stop_rx_stream(void *h);
bool cuhd_rx_wait_lo_locked(void *h);
double cuhd_set_rx_srate(void *h, double freq);
double cuhd_set_rx_gain(void *h, double gain);
double cuhd_set_rx_freq(void *h, double freq);
int cuhd_recv(void *h, void *data, int nsamples, int blocking);

double cuhd_set_tx_srate(void *h, double freq);
double cuhd_set_tx_gain(void *h, double gain);
double cuhd_set_tx_freq(void *h, double freq);
int cuhd_send(void *h, void *data, int nsamples, int blocking);


#ifdef __cplusplus
}
#endif
