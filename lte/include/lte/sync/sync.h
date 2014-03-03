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

#include "pss.h"
#include "sss.h"
#include "sfo.h"

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
}sync_t;

int sync_run(sync_t *q, cf_t *input);
float sync_get_cfo(sync_t *q);
void sync_pss_det_absolute(sync_t *q);
void sync_pss_det_peakmean(sync_t *q);
void sync_force_N_id_2(sync_t *q, int force_N_id_2);
int sync_get_slot_id(sync_t *q);
float sync_get_peak_to_avg(sync_t *q);
int sync_get_N_id_2(sync_t *q);
int sync_get_N_id_1(sync_t *q);
int sync_get_cell_id(sync_t *q);
void sync_set_threshold(sync_t *q, float threshold);
int sync_init(sync_t *q, int frame_size);
void sync_free(sync_t *q);

#endif

