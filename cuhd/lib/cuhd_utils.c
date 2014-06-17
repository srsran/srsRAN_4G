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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>


#include "liblte/cuhd/cuhd.h"
#include "liblte/phy/utils/vector.h"
#include "liblte/phy/utils/debug.h"

int cuhd_rssi_scan(void *uhd, float *freqs, float *rssi, int nof_bands, double fs, int nsamp) {
  int i, j;
  int ret = -1;
  _Complex float *buffer;
  double f;

  buffer = calloc(nsamp, sizeof(_Complex float));
  if (!buffer) {
    goto free_and_exit;
  }

  cuhd_set_rx_gain(uhd, 0.0);
  cuhd_set_rx_srate(uhd, fs);

  for (i=0;i<nof_bands;i++) {
    cuhd_stop_rx_stream(uhd);

    f = (double) freqs[i];
    cuhd_set_rx_freq(uhd, f);
    cuhd_rx_wait_lo_locked(uhd);

    cuhd_start_rx_stream(uhd);

    /* discard first samples */
    for (j=0;j<2;j++) {
      if (cuhd_recv(uhd, buffer, nsamp, 1) != nsamp) {
        goto free_and_exit;
      }
    }
    rssi[i] = vec_avg_power_cf(buffer, nsamp);
    printf("[%3d]: Freq %4.1f Mhz - RSSI: %3.2f dBm\r", i, f/1000000, 10*log10f(rssi[i]) + 30); fflush(stdout);
    if (VERBOSE_ISINFO()) {
      printf("\n");
    }
  }
  cuhd_stop_rx_stream(uhd);

  ret = 0;
free_and_exit:
  free(buffer);
  return ret;
}
