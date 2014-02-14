/*
 * Copyright (c) 2013, Ismael Gomez-Miguelez <gomezi@tcd.ie>.
 * This file is part of OSLD-lib (http://https://github.com/ismagom/osld-lib)
 *
 * OSLD-lib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OSLD-lib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with OSLD-lib.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <complex.h>
#include <math.h>

#include "sync/pss.h"
#include "utils/dft.h"
#include "utils/vector.h"
#include "utils/convolution.h"

#define NOT_SYNC	0xF0F0F0F0

/* Initializes the object. subframe_size is the size, in samples, of the 1ms subframe
 *
 */
int pss_synch_init(pss_synch_t *q, int frame_size) {
	int ret = -1;
	bzero(q, sizeof(pss_synch_t));

	q->pss_signal_freq = vec_malloc((PSS_LEN_FREQ+frame_size) * sizeof(cf_t));
	if (!q->pss_signal_freq) {
		fprintf(stderr, "Error allocating memory\n");
		goto clean_and_exit;
	}
	q->conv_abs = vec_malloc((PSS_LEN_FREQ+frame_size) * sizeof(float));
	if (!q->conv_abs) {
		fprintf(stderr, "Error allocating memory\n");
		goto clean_and_exit;
	}
	q->tmp_input = vec_malloc((PSS_LEN_FREQ+frame_size) * sizeof(cf_t));
	if (!q->tmp_input) {
		fprintf(stderr, "Error allocating memory\n");
		goto clean_and_exit;
	}
	q->frame_buffer = vec_malloc(4*frame_size * sizeof(cf_t));
	if (!q->frame_buffer) {
		fprintf(stderr, "Error allocating memory\n");
		goto clean_and_exit;
	}
	q->conv_output = vec_malloc((PSS_LEN_FREQ+frame_size) * sizeof(cf_t));
	if (!q->conv_output) {
		fprintf(stderr, "Error allocating memory\n");
		goto clean_and_exit;
	}

#ifdef CONVOLUTION_FFT
	if (conv_fft_cc_init(&q->conv_fft, frame_size, PSS_LEN_FREQ)) {
		fprintf(stderr, "Error initiating convolution FFT\n");
		goto clean_and_exit;
	}
#endif

	q->correlation_threshold = DEFAULT_CORRELATION_TH;
	q->nosync_timeout_frames = DEFAULT_NOSYNC_TIMEOUT;
	q->cfo_auto = true;
	q->N_id_2 = -1;
	q->frame_size = frame_size;
	q->frame_start_idx = NOT_SYNC;
	q->fb_wp = 0;

	ret = 0;
clean_and_exit:
	if (ret == -1) {
		pss_synch_free(q);
	}
	return ret;
}

void pss_synch_free(pss_synch_t *q) {
	if (q->pss_signal_freq) {
		free(q->pss_signal_freq);
	}
	if (q->conv_abs) {
		free(q->conv_abs);
	}
	if (q->tmp_input) {
		free(q->tmp_input);
	}
	if (q->frame_buffer) {
		free(q->frame_buffer);
	}
	if (q->conv_output) {
		free(q->conv_output);
	}

#ifdef CONVOLUTION_FFT
	conv_fft_cc_free(&q->conv_fft);
#endif

	bzero(q, sizeof(pss_synch_t));
}

/**
 * This function calculates the Zadoff-Chu sequence.
 * @param signal Output array.
 * @param direction 0 for tx, 1 for rx
 */
int pss_generate(cf_t *signal, int direction, int N_id_2) {
	int i;
	float arg;
	const float root_value[] = {25.0,29.0,34.0};
	int root_idx;

	int sign = direction ? 1 : -1;

	if (N_id_2 < 0 || N_id_2 > 2) {
		fprintf(stderr, "Invalid N_id_2 %d\n", N_id_2);
		return -1;
	}

	root_idx = N_id_2;

	for (i = 0; i < PSS_LEN / 2; i++) {
		arg = (float) sign * M_PI * root_value[root_idx]
				* ((float) i * ((float) i + 1.0)) / 63.0;
		__real__ signal[i] = cosf(arg);
		__imag__ signal[i] = sinf(arg);
	}
	for (i = PSS_LEN / 2; i < PSS_LEN; i++) {
		arg = (float) sign * M_PI * root_value[root_idx]
				* (((float) i + 2.0) * ((float) i + 1.0)) / 63.0;
		__real__ signal[i] = cosf(arg);
		__imag__ signal[i] = sinf(arg);
	}
	return 0;
}



/** Sets the current N_id_2 value. Initializes the object for this PSS sequence
 * Returns -1 on error, 0 otherwise
 */
int pss_synch_set_N_id_2(pss_synch_t *q, int N_id_2) {
	q->N_id_2 = N_id_2;

	dft_plan_t plan;
	cf_t pss_signal_pad[PSS_LEN_FREQ];
	cf_t pss_signal_time[PSS_LEN];

	if (N_id_2 < 0 || N_id_2 > 2) {
		fprintf(stderr, "Invalid N_id_2 %d\n", N_id_2);
		return -1;
	}

	pss_generate(pss_signal_time, 0, N_id_2);

	memset(pss_signal_pad, 0, PSS_LEN_FREQ * sizeof(cf_t));
	memset(q->pss_signal_freq, 0, PSS_LEN_FREQ * sizeof(cf_t));
	memcpy(&pss_signal_pad[33], pss_signal_time, PSS_LEN * sizeof(cf_t));

	if (dft_plan(PSS_LEN_FREQ - 1, COMPLEX_2_COMPLEX, BACKWARD, &plan)) {
		return -1;
	}
	plan.options = DFT_MIRROR_PRE | DFT_DC_OFFSET;

	dft_run_c2c(&plan, pss_signal_pad, q->pss_signal_freq);

	vec_sc_prod_cfc(q->pss_signal_freq, (float) 1 / (PSS_LEN_FREQ - 1),
			pss_signal_pad, PSS_LEN_FREQ);

	vec_conj_cc(pss_signal_pad, q->pss_signal_freq, PSS_LEN_FREQ);

	q->N_id_2 = N_id_2;

	dft_plan_free(&plan);

	return 0;
}

/** Returns the index of the PSS correlation peak in a subframe.
 * The frame starts at corr_peak_pos-subframe_size/2.
 * The value of the correlation is stored in corr_peak_value.
 *
 * Input buffer must be subframe_size long.
 */
int pss_synch_find_pss(pss_synch_t *q, cf_t *input, float *corr_peak_value, float *corr_mean_value) {
	int corr_peak_pos;
	int conv_output_len;

	memset(&q->pss_signal_freq[PSS_LEN_FREQ], 0, q->frame_size * sizeof(cf_t));
	memcpy(q->tmp_input, input, q->frame_size * sizeof(cf_t));
	memset(&q->tmp_input[q->frame_size], 0, PSS_LEN_FREQ * sizeof(cf_t));

#ifdef CONVOLUTION_FFT
	conv_output_len = conv_fft_cc_run(&q->conv_fft, q->tmp_input, q->pss_signal_freq, q->conv_output);
#else
	conv_output_len = conv_cc(input, q->pss_signal_freq, q->conv_output, q->frame_size, PSS_LEN_FREQ);
#endif

	vec_abs_cf(q->conv_output, q->conv_abs, conv_output_len);
	corr_peak_pos = vec_max_fi(q->conv_abs, conv_output_len);
	if (corr_peak_value) {
		*corr_peak_value = q->conv_abs[corr_peak_pos];
	}
	if (corr_mean_value) {
		*corr_mean_value = vec_acc_ff(q->conv_abs, conv_output_len) / conv_output_len;
	}

	return (int) corr_peak_pos;
}

/* Returns the CFO estimation given a PSS received sequence
 *
 * Source: An Efﬁcient CFO Estimation Algorithm for the Downlink of 3GPP-LTE
 * 			Feng Wang and Yu Zhu
 */
float pss_synch_cfo_compute(pss_synch_t* q, cf_t *pss_recv) {
	cf_t y0, y1, yr;
	cf_t y[PSS_LEN_FREQ-1];

	vec_prod_ccc_unalign(q->pss_signal_freq, pss_recv, y, PSS_LEN_FREQ - 1);

	y0 = vec_acc_cc(y, (PSS_LEN_FREQ - 1)/2);
	y1 = vec_acc_cc(&y[(PSS_LEN_FREQ - 1)/2], (PSS_LEN_FREQ - 1)/2);
	yr = conjf(y0) * y1;

	return atan2f(__imag__ yr, __real__ yr) / M_PI;
}










/** This function is designed to be called periodically on a subframe basis.
 * The function finds the PSS correlation peak and computes (does not adjust) CFO automatically as defined by
 * pss_synch_set_cfo_mode().
 *
 * If the PSS sequence is not found, returns 0 writes nothing to the output buffer.
 * If the PSS sequence is found, aligns the beginning of the subframe to the output buffer and returns the number of samples
 * written to the output buffer.
 * If synchronized, subsequent calls to this function align the input buffer to the subframe beginning.
 */
int pss_synch_frame(pss_synch_t *q, cf_t *input, cf_t *output, int nsamples) {
	int max_idx, tmp_start_idx;
	int retval;
	float max_value;

	if (nsamples != q->frame_size) {
		fprintf(stderr, "Configured for frame size %d but got %d samples\n",
				q->frame_size, nsamples);
		return -1;
	}

	if (q->N_id_2 < 0) {
		fprintf(stderr,
				"N_id_2 must be configured before calling pss_synch()\n");
		return -1;
	}

	max_idx = pss_synch_find_pss(q, input, &max_value, NULL);
	if (max_value > q->correlation_threshold) {
		tmp_start_idx = max_idx - nsamples / 2;
		if (q->frame_start_idx != tmp_start_idx) {
			printf("Re-synchronizing: new index is %d, old was %d\n",
					tmp_start_idx, q->frame_start_idx);
		}
		q->frame_start_idx = tmp_start_idx;
	} else {
		if (q->nosync_timeout_frames > 0) {
			q->nof_nosync_frames++;
			if (q->nof_nosync_frames >= q->nosync_timeout_frames) {
				q->frame_start_idx = NOT_SYNC;
			}
		}
	}

	if (q->frame_start_idx == NOT_SYNC) {

		memcpy(q->frame_buffer, input, nsamples * sizeof(cf_t));
		retval = 0;

	} else if (q->frame_start_idx > 0) {

		if (q->fb_wp) {
			memcpy(&q->frame_buffer[(nsamples - q->frame_start_idx)], input,
					q->frame_start_idx * sizeof(cf_t));
			memcpy(output, q->frame_buffer, nsamples * sizeof(cf_t));
			retval = nsamples;
		} else {
			retval = 0;
		}
		memcpy(q->frame_buffer, &input[q->frame_start_idx],
				(nsamples - q->frame_start_idx) * sizeof(cf_t));
		q->fb_wp = 1;

	} else {

		memcpy(output, &q->frame_buffer[nsamples + q->frame_start_idx],
				(-q->frame_start_idx) * sizeof(cf_t));
		memcpy(&output[-q->frame_start_idx], input,
				(nsamples + q->frame_start_idx) * sizeof(cf_t));
		memcpy(&q->frame_buffer[nsamples + q->frame_start_idx],
				&input[nsamples + q->frame_start_idx],
				(-q->frame_start_idx) * sizeof(cf_t));
		retval = nsamples;
	}

	if (q->frame_start_idx != NOT_SYNC && q->cfo_auto && retval) {
		q->current_cfo = pss_synch_cfo_compute(q, &output[q->frame_size/2 - PSS_LEN_FREQ + 1]);
	}

	return retval;
}


void pss_synch_set_timeout(pss_synch_t *q, int nof_frames) {
	q->nosync_timeout_frames = nof_frames;
}

void pss_synch_set_threshold(pss_synch_t *q, float threshold) {
	q->correlation_threshold = threshold;
}

void pss_synch_set_cfo_mode(pss_synch_t *q, bool cfo_auto) {
	q->cfo_auto = cfo_auto;
}

float pss_synch_get_cfo(pss_synch_t *q) {
	return q->current_cfo;
}

int pss_synch_get_frame_start_idx(pss_synch_t *q) {
	return q->frame_start_idx;
}






/** High-level API */



int pss_synch_initialize(pss_synch_hl* h) {
	int fs = h->init.frame_size;
	if (!fs) {
		fs = DEFAULT_FRAME_SIZE;
	}
	if (pss_synch_init(&h->obj, fs)) {
		return -1;
	}
	if (h->init.unsync_nof_pkts) {
		pss_synch_set_timeout(&h->obj, h->init.unsync_nof_pkts);
	}

	pss_synch_set_N_id_2(&h->obj, h->init.N_id_2);
	if (h->init.do_cfo) {
		pss_synch_set_cfo_mode(&h->obj, true);
	} else {
		pss_synch_set_cfo_mode(&h->obj, false);
	}
	return 0;
}

int pss_synch_work(pss_synch_hl* hl) {

	if (hl->ctrl_in.correlation_threshold) {
		pss_synch_set_threshold(&hl->obj, hl->ctrl_in.correlation_threshold);
	}

	hl->out_len = pss_synch_frame(&hl->obj, hl->input, hl->output, hl->in_len);

	return 0;
}

int pss_synch_stop(pss_synch_hl* hl) {
	pss_synch_free(&hl->obj);
	return 0;
}

