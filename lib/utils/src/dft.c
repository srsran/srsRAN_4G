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

#include <math.h>
#include <complex.h>
#include <fftw3.h>
#include <string.h>

#include "utils/dft.h"

#define div(a,b) ((a-1)/b+1)


int dft_plan_multi(const int *dft_points, dft_mode_t *modes, dft_dir_t *dirs,
		int nof_plans, dft_plan_t *plans) {
	int i;
	for (i=0;i<nof_plans;i++) {
		if (dft_plan(dft_points[i],modes[i],dirs[i], &plans[i])) {
			return -1;
		}
	}
	return 0;
}

int dft_plan_multi_c2c(const int *dft_points, dft_dir_t dir, int nof_plans, dft_plan_t *plans) {
	int i;
	for (i=0;i<nof_plans;i++) {
		if (dft_plan(dft_points[i],COMPLEX_2_COMPLEX,dir,&plans[i])) {
			return -1;
		}
	}
	return 0;
}

int dft_plan_multi_c2r(const int *dft_points, dft_dir_t dir, int nof_plans, dft_plan_t *plans) {
	int i;
	for (i=0;i<nof_plans;i++) {
		if (dft_plan(dft_points[i],COMPLEX_2_REAL,dir,&plans[i])) {
			return -1;
		}
	}
	return 0;
}
int dft_plan_multi_r2r(const int *dft_points, dft_dir_t dir, int nof_plans, dft_plan_t *plans) {
	int i;
	for (i=0;i<nof_plans;i++) {
		if (dft_plan(dft_points[i],REAL_2_REAL,dir,&plans[i])) {
			return -1;
		}
	}
	return 0;
}


int dft_plan(const int dft_points, dft_mode_t mode, dft_dir_t dir, dft_plan_t *plan) {

	switch(mode) {
	case COMPLEX_2_COMPLEX:
		if (dft_plan_c2c(dft_points,dir,plan)) {
			return -1;
		}
		break;
	case REAL_2_REAL:
		if (dft_plan_r2r(dft_points,dir,plan)) {
			return -1;
		}
		break;
	case COMPLEX_2_REAL:
		if (dft_plan_c2r(dft_points,dir,plan)) {
			return -1;
		}
		break;
	}
	return 0;
}

static void allocate(dft_plan_t *plan, int size_in, int size_out, int len) {
	plan->in = fftwf_malloc(size_in*len);
	plan->out = fftwf_malloc(size_out*len);
}

int dft_plan_c2c(const int dft_points, dft_dir_t dir, dft_plan_t *plan) {
	int sign;
	sign = (dir == FORWARD) ? FFTW_FORWARD : FFTW_BACKWARD;
	allocate(plan,sizeof(fftwf_complex),sizeof(fftwf_complex), dft_points);

	plan->p = fftwf_plan_dft_1d(dft_points, plan->in, plan->out, sign, 0U);
	if (!plan->p) {
		return -1;
	}

	plan->size = dft_points;
	plan->mode = COMPLEX_2_COMPLEX;

	return 0;
}

int dft_plan_r2r(const int dft_points, dft_dir_t dir, dft_plan_t *plan) {
	int sign;
	sign = (dir == FORWARD) ? FFTW_R2HC : FFTW_HC2R;

	allocate(plan,sizeof(float),sizeof(float), dft_points);

	plan->p = fftwf_plan_r2r_1d(dft_points, plan->in, plan->out, sign, 0U);
	if (!plan->p) {
		return -1;
	}

	plan->size = dft_points;
	plan->mode = REAL_2_REAL;

	return 0;
}

int dft_plan_c2r(const int dft_points, dft_dir_t dir, dft_plan_t *plan) {
	if (dft_plan_c2c(dft_points, dir, plan)) {
		return -1;
	}
	plan->mode = COMPLEX_2_REAL;
	return 0;
}

static void copy(char *dst, char *src, int size_d, int len, int mirror, int dc_offset) {
	int offset=dc_offset?1:0;
	int hlen;
	if (mirror == DFT_MIRROR_PRE) {
		hlen = div(len,2);
		memset(dst,0,size_d*offset);
		memcpy(&dst[offset*size_d], &src[size_d*hlen], size_d*(hlen-offset));
		memcpy(&dst[hlen*size_d], src, size_d*(len - hlen));
	} else if (mirror == DFT_MIRROR_POS) {
		hlen = div(len,2);
		memcpy(dst, &src[size_d*hlen], size_d*hlen);
		memcpy(&dst[hlen*size_d], &src[size_d*offset], size_d*(len - hlen));
	} else {
		memcpy(dst,src,size_d*len);
	}
}

void dft_run(dft_plan_t *plan, void *in, void *out) {
	switch(plan->mode) {
	case COMPLEX_2_COMPLEX:
		dft_run_c2c(plan,in,out);
		break;
	case REAL_2_REAL:
		dft_run_r2r(plan,in,out);
		break;
	case COMPLEX_2_REAL:
		dft_run_c2r(plan,in,out);
		break;
	}
}

void dft_run_c2c(dft_plan_t *plan, dft_c_t *in, dft_c_t *out) {
	float norm;
	int i;
	fftwf_complex *f_out = plan->out;

	copy((char*) plan->in,(char*) in,sizeof(dft_c_t),plan->size,plan->options & DFT_MIRROR_PRE,
			plan->options & DFT_DC_OFFSET);

	fftwf_execute(plan->p);

	if (plan->options & DFT_NORMALIZE) {
		norm = sqrtf(plan->size);
		for (i=0;i<plan->size;i++) {
			f_out[i] /= norm;
		}
	}
	if (plan->options & DFT_OUT_DB) {
		for (i=0;i<plan->size;i++) {
			f_out[i] = 10*log10(f_out[i]);
		}
	}
	copy((char*) out,(char*) plan->out,sizeof(dft_c_t),plan->size,plan->options & DFT_MIRROR_POS,
			plan->options & DFT_DC_OFFSET);
}

void dft_run_r2r(dft_plan_t *plan, dft_r_t *in, dft_r_t *out) {
	float norm;
	int i;
	int len = plan->size;
	float *f_out = plan->out;

	copy((char*) plan->in,(char*) in,sizeof(dft_r_t),plan->size,plan->options & DFT_MIRROR_PRE,
			plan->options & DFT_DC_OFFSET);

	fftwf_execute(plan->p);

	if (plan->options & DFT_NORMALIZE) {
		norm = plan->size;
		for (i=0;i<len;i++) {
			f_out[i] /= norm;
		}
	}
	if (plan->options & DFT_PSD) {
		for (i=0;i<(len+1)/2-1;i++) {
			out[i] = sqrtf(f_out[i]*f_out[i]+f_out[len-i-1]*f_out[len-i-1]);
		}
	}
	if (plan->options & DFT_OUT_DB) {
		for (i=0;i<len;i++) {
			out[i] = 10*log10(out[i]);
		}
	}
}

void dft_run_c2r(dft_plan_t *plan, dft_c_t *in, dft_r_t *out) {
	int i;
	float norm;
	float *f_out = plan->out;

	copy((char*) plan->in,(char*) in,sizeof(dft_r_t),plan->size,plan->options & DFT_MIRROR_PRE,
			plan->options & DFT_DC_OFFSET);

	fftwf_execute(plan->p);

	if (plan->options & DFT_NORMALIZE) {
		norm = plan->size;
		for (i=0;i<plan->size;i++) {
			f_out[i] /= norm;
		}
	}
	if (plan->options & DFT_PSD) {
		for (i=0;i<plan->size;i++) {
			out[i] = (__real__ f_out[i])*(__real__ f_out[i])+
					 (__imag__ f_out[i])*(__imag__ f_out[i]);
			if (!(plan->options & DFT_OUT_DB)) {
				out[i] = sqrtf(out[i]);
			}
		}
	}
	if (plan->options & DFT_OUT_DB) {
		for (i=0;i<plan->size;i++) {
			out[i] = 10*log10(out[i]);
		}
	}
}


void dft_plan_free(dft_plan_t *plan) {
	if (!plan) return;
	if (!plan->size) return;
	if (plan->in) fftwf_free(plan->in);
	if (plan->out) fftwf_free(plan->out);
	if (plan->p) fftwf_destroy_plan(plan->p);
	bzero(plan, sizeof(dft_plan_t));
}

void dft_plan_free_vector(dft_plan_t *plan, int nof_plans) {
	int i;
	for (i=0;i<nof_plans;i++) {
		dft_plan_free(&plan[i]);
	}
}



