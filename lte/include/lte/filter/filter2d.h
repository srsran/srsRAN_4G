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



#ifndef FILTER2D_
#define FILTER2D_

/* 2-D real filter of complex input
 *
 */
typedef _Complex float cf_t;

typedef struct {
	int sztime; // Output signal size in the time domain
	int szfreq;	// Output signal size in the freq domain
	int ntime;	// 2-D Filter size in time domain
	int nfreq;	// 2-D Filter size in frequency domain
	float **taps;	// 2-D filter coefficients
	cf_t *output; // Output signal
} filter2d_t;

int filter2d_init (filter2d_t* q, float **taps, int ntime, int nfreq, int sztime, int szfreq);
int filter2d_init_default (filter2d_t* q, int ntime, int nfreq, int sztime, int szfreq);
void filter2d_free(filter2d_t *q);
void filter2d_reset(filter2d_t *q);
void filter2d_add(filter2d_t *q, cf_t h, int time_idx, int freq_idx);

#endif
