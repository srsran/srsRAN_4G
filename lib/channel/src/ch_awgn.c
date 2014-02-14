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

#include <complex.h>
#include <stdlib.h>
#include <strings.h>

#include "gauss.h"
#include "channel/ch_awgn.h"

void ch_awgn(const cf* x, cf* y, float variance, int buff_sz) {
	_Complex float tmp;
	int i;

	for (i=0;i<buff_sz;i++) {
		__real__ tmp = rand_gauss();
		__imag__ tmp = rand_gauss();
		tmp *= variance;
		y[i] = tmp + x[i];
	}
}

/* High-level API */
int ch_awgn_initialize(ch_awgn_hl* hl) {

	return 0;
}

int ch_awgn_work(ch_awgn_hl* hl) {
	ch_awgn(hl->input,hl->output,hl->ctrl_in.variance,hl->in_len);
	hl->out_len = hl->in_len;
	return 0;
}

int ch_awgn_stop(ch_awgn_hl* hl) {
	return 0;
}
