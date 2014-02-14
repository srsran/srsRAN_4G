#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

#include "uhd.h"
#include "utils/vector.h"
#include "utils/debug.h"

int uhd_rssi_scan(void *uhd, float *freqs, float *rssi, int nof_bands, double fs, int nsamp) {
	int i, j;
	int ret = -1;
	_Complex float *buffer;
	double f;

	buffer = calloc(nsamp, sizeof(_Complex float));
	if (!buffer) {
		goto free_and_exit;
	}

	uhd_set_rx_gain(uhd, 0.0);
	uhd_set_rx_srate(uhd, fs);

	for (i=0;i<nof_bands;i++) {
		uhd_stop_rx_stream(uhd);

		f = (double) freqs[i];
		uhd_set_rx_freq(uhd, f);
		uhd_rx_wait_lo_locked(uhd);

		uhd_start_rx_stream(uhd);

		/* discard first samples */
		for (j=0;j<2;j++) {
			if (uhd_recv(uhd, buffer, nsamp, 1) != nsamp) {
				goto free_and_exit;
			}
		}
		rssi[i] = vec_avg_power_cf(buffer, nsamp);
		printf("[%3d]: Freq %4.1f Mhz - RSSI: %3.2f dBm\r", i, f/1000000, 10*log10f(rssi[i]) + 30); fflush(stdout);
		if (VERBOSE_ISINFO()) {
			printf("\n");
		}
	}
	uhd_stop_rx_stream(uhd);

	ret = 0;
free_and_exit:
	free(buffer);
	return ret;
}
