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


#ifndef NCO_
#define NCO_

#include <complex.h>

typedef struct {
	int size;
	float *cost;
	float *sint;
}nco_t;

void nco_init(nco_t *nco, int size);
void nco_destroy(nco_t *nco);

float nco_sin(nco_t *nco, float phase);
float nco_cos(nco_t *nco, float phase);
void nco_sincos(nco_t *nco, float phase, float *sin, float *cos);
_Complex float nco_cexp(nco_t *nco, float arg);

void nco_sin_f(nco_t *nco, float *x, float freq, int len);
void nco_cos_f(nco_t *nco, float *x, float freq, int len);
void nco_cexp_f(nco_t *nco, _Complex float *x, float freq, int len);
void nco_cexp_f_direct(_Complex float *x, float freq, int len);

#endif
