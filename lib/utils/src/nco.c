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



#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <complex.h>

#include "utils/nco.h"

void nco_init(nco_t *nco, int size) {
	int i;

	nco->size=size;
	nco->cost=malloc(size*sizeof(float));
	nco->sint=malloc(size*sizeof(float));
	assert(nco->cost && nco->sint);

	for (i=0;i<size;i++) {
		nco->cost[i] = cosf(2*M_PI*i/size);
		nco->sint[i] = sinf(2*M_PI*i/size);
	}
}

void nco_destroy(nco_t *nco) {
	if (nco->cost) {
		free(nco->cost);
	}
	if (nco->sint) {
		free(nco->sint);
	}
	nco->size=0;
	bzero(nco, sizeof(nco_t));
}

unsigned int nco_idx(float phase, int size) {
	while(phase>=2*M_PI) {
		phase-=2*M_PI;
	}
	unsigned int idx = (unsigned int) (phase*size/(2*M_PI));
	return idx;
}

inline float nco_sin(nco_t *nco, float phase) {
	return nco->sint[nco_idx(phase,nco->size)];
}
inline float nco_cos(nco_t *nco, float phase) {
	return nco->cost[nco_idx(phase,nco->size)];
}
inline void nco_sincos(nco_t *nco, float phase, float *sin, float *cos) {
	unsigned int idx = nco_idx(phase,nco->size);
	*sin = nco->sint[idx];
	*cos = nco->cost[idx];
}

inline _Complex float nco_cexp(nco_t *nco, float arg) {
	float s,c;
	nco_sincos(nco,arg,&s,&c);
	return c+I*s;
}

void nco_sin_f(nco_t *nco, float *x, float freq, int len) {
	int i;
	unsigned int idx;

	idx=0;
	for (i=0;i<len;i++) {
		idx=((unsigned int) (freq*i*nco->size/len))%nco->size;
		x[i] = nco->sint[idx];
	}
}


void nco_cos_f(nco_t *nco, float *x, float freq, int len) {
	int i;
	unsigned int idx;

	idx=0;
	for (i=0;i<len;i++) {
		idx=((unsigned int) (freq*i*nco->size/len))%nco->size;
		x[i] = nco->cost[idx];
	}
}


void nco_cexp_f(nco_t *nco, _Complex float *x, float freq, int len) {
	int i;
	unsigned int idx;

	idx=0;
	for (i=0;i<len;i++) {
		idx=((unsigned int) (freq*i*nco->size/len))%nco->size;
		x[i] = nco->cost[idx] + I*nco->sint[idx];
	}
}

void nco_cexp_f_direct(_Complex float *x, float freq, int len) {
	int i;
	for (i=0;i<len;i++) {
		x[i] *= cexpf(_Complex_I * 2 * M_PI * freq * i);
	}
}

