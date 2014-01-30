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

#include <stdio.h>
#include <stdlib.h>
#include "sync/sfo.h"

/* Estimate SFO based on the array of time estimates t0
 * of length len. The parameter period is the time between t0 samples
 */
float sfo_estimate(int *t0, int len, float period) {
	int i;
	float sfo=0.0;
	for (i=1;i<len;i++) {
		sfo += (t0[i]-t0[i-1])/period/len;
	}
	return sfo;
}

/* Same as sfo_estimate but period is non-uniform.
 * Vector t is the sampling time times period for each t0
 */
float sfo_estimate_period(int *t0, int *t, int len, float period) {
	int i;
	float sfo=0.0;
	for (i=1;i<len;i++) {
		if (abs(t0[i]-t0[i-1]) < 5000) {
			sfo += (t0[i]-t0[i-1])/(t[i] - t[i-1])/period;
		}
	}
	return sfo/(len-2);
}
