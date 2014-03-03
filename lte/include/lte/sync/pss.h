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


#ifndef PSS_
#define PSS_

#include <stdint.h>
#include <stdbool.h>

#include "lte/common/base.h"
#include "lte/utils/convolution.h"

typedef _Complex float cf_t; /* this is only a shortcut */

#define CONVOLUTION_FFT
#define DEFAULT_CORRELATION_TH 10000
#define DEFAULT_NOSYNC_TIMEOUT	5

#define PSS_LEN_FREQ 	129		// FFT-based convolution removes 1 leaving it in 128
#define PSS_LEN			62
#define PSS_RE			6*12



/** The pss_synch_t object provides functions for fast computation of the crosscorrelation
 * between the PSS and received signal and CFO estimation. Also, the function pss_synch_periodic() is designed
 * to be called periodically every subframe, taking care of the correct data alignment with respect
 * to the PSS sequence.
 */


/* Low-level API */
typedef struct {

#ifdef CONVOLUTION_FFT
	conv_fft_cc_t conv_fft;
#endif

	int frame_size;
	int N_id_2;
	float current_cfo;
	bool cfo_auto;					// default true
	int nof_nosync_frames;
	int nosync_timeout_frames;		// default 5
	float correlation_threshold; 	// default 10000
	int frame_start_idx;
	int fb_wp;

	cf_t *pss_signal_freq;
	cf_t *tmp_input;
	float *conv_abs;
	cf_t *frame_buffer;
	cf_t *conv_output;
	cf_t *tmp_nco;
}pss_synch_t;

typedef enum { PSS_TX, PSS_RX } pss_direction_t;

/* Basic functionality */

int pss_synch_init(pss_synch_t *q, int frame_size);
void pss_synch_free(pss_synch_t *q);
int pss_generate(cf_t *signal, int N_id_2);
void pss_put_slot(cf_t *pss_signal, cf_t *slot, int nof_prb, lte_cp_t cp);

int pss_synch_set_N_id_2(pss_synch_t *q, int N_id_2);
int pss_synch_find_pss(pss_synch_t *q, cf_t *input, float *corr_peak_value, float *corr_mean_value);
float pss_synch_cfo_compute(pss_synch_t* q, cf_t *pss_recv);






/* Automatic frame management functions (for periodic calling) */
int pss_synch_periodic(pss_synch_t *q, cf_t *input, cf_t *output, int nsamples);
void pss_synch_set_timeout(pss_synch_t *q, int nof_frames);
void pss_synch_set_threshold(pss_synch_t *q, float threshold);
void pss_synch_set_cfo_mode(pss_synch_t *q, bool cfo_auto);
float pss_synch_get_cfo(pss_synch_t *q);
int pss_synch_get_frame_start_idx(pss_synch_t *q);








/* High-level API */

typedef struct {
	pss_synch_t obj;
	struct pss_synch_init {
		int frame_size;				// if 0, 2048
		int unsync_nof_pkts;
		int N_id_2;
		int do_cfo;
	} init;
	cf_t *input;
	int in_len;
	struct pss_synch_ctrl_in {
		int correlation_threshold;
		float manual_cfo;
	} ctrl_in;
	cf_t *output;
	int out_len;
}pss_synch_hl;

#define DEFAULT_FRAME_SIZE		2048

int pss_synch_initialize(pss_synch_hl* h);
int pss_synch_work(pss_synch_hl* hl);
int pss_synch_stop(pss_synch_hl* hl);


#endif
