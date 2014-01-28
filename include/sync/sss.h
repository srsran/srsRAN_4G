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

#ifndef SSS_
#define SSS_

#include <stdint.h>
#include <stdbool.h>

#include "utils/dft.h"

typedef _Complex float cf_t; /* this is only a shortcut */

/** gives the beginning of the SSS symbol (to be passed to sss_synch_m0m1).
 * subframe_sz is the length of the subframe, e.g. 1920 for the 1.9 MHz
 * symbol_sz is the OFDM symbol size (including CP), e.g. 137 for the 1.9 MHz
 */
#define SSS_SYMBOL_ST(subframe_sz, symbol_sz) (subframe_sz/2-2*symbol_sz)
#define SSS_POS_SYMBOL	33

#define SSS_DFT_LEN 128
#define N_SSS 		31

struct sss_tables {
	int z1[N_SSS][N_SSS];
	int c[2][N_SSS];
	int s[N_SSS][N_SSS];
	int N_id_2;
};

/* Allocate 32 complex to make it multiple of 32-byte AVX instructions alignment requirement.
 * Should use vect_malloc() to make it platform agnostic.
 */
struct fc_tables {
	cf_t z1[N_SSS+1][N_SSS+1];
	cf_t c[2][N_SSS+1];
	cf_t s[N_SSS+1][N_SSS+1];
};


/* Low-level API */
typedef struct {

	dft_plan_t dftp_input;

	float corr_peak_threshold;
	int symbol_sz;
	int subframe_sz;

	int N_id_1_table[30][30];
	struct fc_tables fc_tables;

}sss_synch_t;


/* Basic functionality */
int sss_synch_init(sss_synch_t *q);
void sss_synch_free(sss_synch_t *q);
void sss_generate(float *signal, int cell_id);

int sss_synch_set_N_id_2(sss_synch_t *q, int N_id_2);

void sss_synch_m0m1(sss_synch_t *q, cf_t *input, int *m0, float *m0_value,
		int *m1, float *m1_value);
int sss_synch_subframe(int m0, int m1);
int sss_synch_N_id_1(sss_synch_t *q, int m0, int m1);

int sss_synch_frame(sss_synch_t *q, cf_t *input, int *subframe_idx, int *N_id_1);
void sss_synch_set_threshold(sss_synch_t *q, float threshold);
void sss_synch_set_symbol_sz(sss_synch_t *q, int symbol_sz);
void sss_synch_set_subframe_sz(sss_synch_t *q, int subframe_sz);


/* High-level API */

typedef struct {
	sss_synch_t obj;
	struct sss_synch_init {
		int N_id_2;
	} init;
	cf_t *input;
	int in_len;
	struct sss_synch_ctrl_in {
		int symbol_sz;
		int subframe_sz;
		int correlation_threshold;
	} ctrl_in;
	struct sss_synch_ctrl_out {
		int subframe_idx;
		int N_id_1;
	} ctrl_out;
}sss_synch_hl;

#define DEFAULT_FRAME_SIZE		2048

int sss_synch_initialize(sss_synch_hl* h);
int sss_synch_work(sss_synch_hl* hl);
int sss_synch_stop(sss_synch_hl* hl);

#endif

