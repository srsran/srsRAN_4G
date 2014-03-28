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



#include <strings.h>

#include "lte/utils/debug.h"
#include "lte/common/base.h"
#include "lte/sync/sync.h"

int sync_init(sync_t *q, int frame_size) {
	int N_id_2;

	bzero(q, sizeof(sync_t));
	q->force_N_id_2 = -1;
	q->threshold = 1.5;
	q->pss_mode = PEAK_MEAN;
	q->detect_cp = true;

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
void sync_pss_det_peak_to_avg(sync_t *q) {
	q->pss_mode = PEAK_MEAN;
}

void sync_set_threshold(sync_t *q, float threshold) {
	q->threshold = threshold;
}

void sync_force_N_id_2(sync_t *q, int force_N_id_2) {
	q->force_N_id_2 = force_N_id_2;
}

void sync_force_cp(sync_t *q, lte_cp_t cp) {
	q->cp = cp;
	q->detect_cp = false;
}

void sync_sss_en(sync_t *q, bool enabled) {
	q->sss_en = enabled;
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

lte_cp_t sync_get_cp(sync_t *q) {
	return q->cp;
}

int sync_run(sync_t *q, cf_t *input) {
	int N_id_2, peak_pos[3], sss_idx_n, sss_idx_e;
	int m0, m1;
	float m0_value_e, m1_value_e,m0_value_n, m1_value_n;
	int slot_id_e, N_id_1_e, slot_id_n, N_id_1_n;
	float peak_value[3];
	float mean_value[3];
	float max=-999;
	int i;
	int peak_detected;

	if (q->force_N_id_2 == -1) {
		for (N_id_2=0;N_id_2<3;N_id_2++) {
			peak_pos[N_id_2] = pss_synch_find_pss(&q->pss[N_id_2], input,
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
		peak_pos[N_id_2] = pss_synch_find_pss(&q->pss[N_id_2], input,
				&peak_value[N_id_2], &mean_value[N_id_2]);
	}

	q->peak_to_avg = peak_value[N_id_2] / mean_value[N_id_2];

	DEBUG("PSS possible peak N_id_2=%d, pos=%d peak=%.2f par=%.2f threshold=%.2f\n",
			N_id_2, peak_pos[N_id_2], peak_value[N_id_2], q->peak_to_avg, q->threshold);

	/* If peak detected */
	peak_detected = 0;
	if (peak_pos[N_id_2] - 128 >= 0) {
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

		q->cfo = pss_synch_cfo_compute(&q->pss[N_id_2], &input[peak_pos[N_id_2]-128]);

		INFO("PSS peak detected N_id_2=%d, pos=%d peak=%.2f par=%.2f th=%.2f cfo=%.4f\n", N_id_2,
				peak_pos[N_id_2], peak_value[N_id_2], q->peak_to_avg, q->threshold, q->cfo);

		if (q->sss_en) {

			/* Make sure we have enough room to find SSS sequence */
			sss_idx_n = peak_pos[N_id_2]-2*(128+CP(128,CPNORM_LEN));
			sss_idx_e = peak_pos[N_id_2]-2*(128+CP(128,CPEXT_LEN));

			if (q->detect_cp) {
				if (sss_idx_n < 0 || sss_idx_e < 0) {
					INFO("Not enough room to decode SSS (%d, %d)\n", sss_idx_n, sss_idx_e);
					return -1;
				}
			} else {
				if (CP_ISNORM(q->cp)) {
					if (sss_idx_n < 0) {
						INFO("Not enough room to decode SSS (%d)\n", sss_idx_n);
						return -1;
					}
				} else {
					if (sss_idx_e < 0) {
						INFO("Not enough room to decode SSS (%d)\n", sss_idx_e);
						return -1;
					}
				}
			}

			/* try Normal CP length */
			if (q->detect_cp || CP_ISNORM(q->cp)) {
				sss_synch_m0m1(&q->sss[N_id_2], &input[sss_idx_n],
						&m0, &m0_value_n, &m1, &m1_value_n);

				slot_id_n = 2 * sss_synch_subframe(m0, m1);
				N_id_1_n = sss_synch_N_id_1(&q->sss[N_id_2], m0, m1);
			}

			if (q->detect_cp || CP_ISEXT(q->cp)) {
				/* Now try Extended CP length */
				sss_synch_m0m1(&q->sss[N_id_2], &input[sss_idx_e],
						&m0, &m0_value_e, &m1, &m1_value_e);

				slot_id_e = 2 * sss_synch_subframe(m0, m1);
				N_id_1_e = sss_synch_N_id_1(&q->sss[N_id_2], m0, m1);
			}

			/* Correlation with extended CP hypoteshis is greater than with normal? */
			if ((q->detect_cp && m0_value_e * m1_value_e > m0_value_n * m1_value_n)
					|| CP_ISEXT(q->cp)) {
				q->cp = CPEXT;
				q->slot_id = slot_id_e;
				q->N_id_1 = N_id_1_e;
			/* then is normal CP */
			} else {
				q->cp = CPNORM;
				q->slot_id = slot_id_n;
				q->N_id_1 = N_id_1_n;
			}
			q->N_id_2 = N_id_2;

			INFO("SSS detected N_id_1=%d, slot_idx=%d, %s CP\n",
					q->N_id_1, q->slot_id, CP_ISNORM(q->cp)?"Normal":"Extended");
		}

		return peak_pos[N_id_2];

	} else {
		return -1;
	}
}
