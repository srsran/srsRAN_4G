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

#include "utils/debug.h"
#include "lte/base.h"
#include "sync/sync.h"

int sync_init(sync_t *q, int frame_size) {
	int N_id_2;

	bzero(q, sizeof(sync_t));
	q->force_N_id_2 = -1;
	q->threshold = 1.5;
	q->pss_mode = PEAK_MEAN;

	for (N_id_2=0;N_id_2<3;N_id_2++) {
		if (pss_synch_init(&q->pss[N_id_2], frame_size)) {
			fprintf(stderr, "Error initializing PSS object\n");
			return -1;
		}
		if (pss_synch_set_N_id_2(&q->pss[N_id_2], N_id_2)) {
			fprintf(stderr, "Error initializing N_id_2\n");
			return -1;
		}
		if (sss_synch_init(&q->sss[N_id_2])) {
			fprintf(stderr, "Error initializing SSS object\n");
			return -1;
		}
		if (sss_synch_set_N_id_2(&q->sss[N_id_2], N_id_2)) {
			fprintf(stderr, "Error initializing N_id_2\n");
			return -1;
		}
		DEBUG("PSS and SSS initiated N_id_2=%d\n", N_id_2);
	}

	return 0;
}

void sync_free(sync_t *q) {
	int N_id_2;

	for (N_id_2=0;N_id_2<3;N_id_2++) {
		pss_synch_free(&q->pss[N_id_2]);
		sss_synch_free(&q->sss[N_id_2]);
	}
}

void sync_pss_det_absolute(sync_t *q) {
	q->pss_mode = ABSOLUTE;
}
void sync_pss_det_peakmean(sync_t *q) {
	q->pss_mode = PEAK_MEAN;
}

void sync_set_threshold(sync_t *q, float threshold) {
	q->threshold = threshold;
}

void sync_force_N_id_2(sync_t *q, int force_N_id_2) {
	q->force_N_id_2 = force_N_id_2;
}

int sync_get_cell_id(sync_t *q) {
	if (q->N_id_1 >=0 && q->N_id_2 >= 0) {
		return q->N_id_1*3 + q->N_id_2;
	} else {
		return -1;
	}
}

int sync_get_N_id_1(sync_t *q) {
	return q->N_id_1;
}

int sync_get_N_id_2(sync_t *q) {
	return q->N_id_2;
}

int sync_get_slot_id(sync_t *q) {
	return q->slot_id;
}

float sync_get_cfo(sync_t *q) {
	return q->cfo;
}

float sync_get_peak_to_avg(sync_t *q) {
	return q->peak_to_avg;
}

int sync_run(sync_t *q, cf_t *input, int read_offset) {
	int N_id_2, peak_pos[3], sss_idx;
	int m0, m1;
	float m0_value, m1_value;
	float peak_value[3];
	float mean_value[3];
	float max=-999;
	int i;
	int peak_detected;

	if (q->force_N_id_2 == -1) {
		for (N_id_2=0;N_id_2<3;N_id_2++) {
			peak_pos[N_id_2] = pss_synch_find_pss(&q->pss[N_id_2], &input[read_offset],
					&peak_value[N_id_2], &mean_value[N_id_2]);
		}
		for (i=0;i<3;i++) {
			if (peak_value[i] > max) {
				max = peak_value[i];
				N_id_2 = i;
			}
		}
	} else {
		N_id_2 = q->force_N_id_2;
		peak_pos[N_id_2] = pss_synch_find_pss(&q->pss[N_id_2], &input[read_offset],
				&peak_value[N_id_2], &mean_value[N_id_2]);
	}

	DEBUG("PSS possible peak N_id_2=%d, pos=%d value=%.2f threshold=%.2f\n",
			N_id_2, peak_pos[N_id_2], peak_value[N_id_2], q->threshold);

	q->peak_to_avg = peak_value[N_id_2] / mean_value[N_id_2];

	/* If peak detected */
	peak_detected = 0;
	if (peak_pos[N_id_2] > 128) {
		if (q->pss_mode == ABSOLUTE) {
			if (peak_value[N_id_2] > q->threshold) {
				peak_detected = 1;
			}
		} else {
			if (q->peak_to_avg  > q->threshold) {
				peak_detected = 1;
			}
		}
	}
	if (peak_detected) {


		q->cfo = pss_synch_cfo_compute(&q->pss[N_id_2], &input[read_offset + peak_pos[N_id_2]-128]);

		INFO("PSS peak detected N_id_2=%d, pos=%d peak=%.2f par=%.2f th=%.2f cfo=%.4f\n", N_id_2,
				peak_pos[N_id_2], peak_value[N_id_2], q->peak_to_avg, q->threshold, q->cfo);

		sss_idx = read_offset + peak_pos[N_id_2]-2*(128+CP(128,CPNORM_LEN));
		if (sss_idx>= 0) {
			sss_synch_m0m1(&q->sss[N_id_2], &input[sss_idx],
					&m0, &m0_value, &m1, &m1_value);

			q->N_id_2 = N_id_2;
			q->slot_id = 2 * sss_synch_subframe(m0, m1);
			q->N_id_1 = sss_synch_N_id_1(&q->sss[N_id_2], m0, m1);

			INFO("SSS detected N_id_1=%d, slot_idx=%d, m0=%d, m1=%d\n",
					q->N_id_1, q->slot_id, m0, m1);

			return peak_pos[N_id_2];
		} else {
			return -1;
		}
	} else {
		return -1;
	}
}
