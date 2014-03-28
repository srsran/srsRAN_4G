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


#ifndef SYNC_
#define SYNC_

#include <stdbool.h>

#include "pss.h"
#include "sss.h"
#include "sfo.h"

/**
 *
 * This object performs time and frequency synchronization using the PSS and SSS signals.
 * The object is designed to work with signals sampled at 1.92 Mhz centered at the carrier frequency.
 * Thus, downsampling is required if the signal is sampled at higher frequencies.
 *
 * Correlation peak is detected comparing the maximum at the output of the correlator with a threshold.
 * The comparison accepts two modes: absolute value or peak-to-mean ratio, which are configured with the
 * functions sync_pss_det_absolute() and sync_pss_det_peakmean().
 */

enum sync_pss_det { ABSOLUTE, PEAK_MEAN};

typedef struct {
	pss_synch_t pss[3]; // One for each N_id_2
	sss_synch_t sss[3]; // One for each N_id_2
	enum sync_pss_det pss_mode;
	float threshold;
	float peak_to_avg;
	int force_N_id_2;
	int N_id_2;
	int N_id_1;
	int slot_id;
	float cfo;
	lte_cp_t cp;
	bool detect_cp;
	bool sss_en;
}sync_t;


int sync_init(sync_t *q, int frame_size);
void sync_free(sync_t *q);

/* Runs the synchronization algorithm. input signal must be sampled at 1.92 MHz and should be frame_size long at least */
int sync_run(sync_t *q, cf_t *input);

/* Sets the threshold for peak comparison */
void sync_set_threshold(sync_t *q, float threshold);
/* Set peak comparison to absolute value */
void sync_pss_det_absolute(sync_t *q);
/* Set peak comparison to relative to the mean */
void sync_pss_det_peak_to_avg(sync_t *q);

/* Forces the synchronizer to check one N_id_2 PSS sequence only (useful for tracking mode) */
void sync_force_N_id_2(sync_t *q, int force_N_id_2);
/* Forces the synchronizer to skip CP detection (useful for tracking mode) */
void sync_force_cp(sync_t *q, lte_cp_t cp);
/* Enables/Disables SSS detection (useful for tracking mode) */
void sync_sss_en(sync_t *q, bool enabled);


/* Gets the slot id (0 or 10) */
int sync_get_slot_id(sync_t *q);
/* Gets the last peak-to-average ratio */
float sync_get_peak_to_avg(sync_t *q);
/* Gets the N_id_2 from the last call to synch_run() */
int sync_get_N_id_2(sync_t *q);
/* Gets the N_id_1 from the last call to synch_run() */
int sync_get_N_id_1(sync_t *q);
/* Gets the Physical CellId from the last call to synch_run() */
int sync_get_cell_id(sync_t *q);
/* Gets the CFO estimation from the last call to synch_run() */
float sync_get_cfo(sync_t *q);
/* Gets the CP length estimation from the last call to synch_run() */
lte_cp_t sync_get_cp(sync_t *q);

#endif

