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



#include <stdio.h>
#include <assert.h>
#include <complex.h>
#include <string.h>
#include <math.h>

#include "lte/common/base.h"
#include "lte/mimo/precoding.h"
#include "lte/utils/vector.h"

int precoding_single(cf_t *x, cf_t *y, int nof_symbols) {
	memcpy(y, x, nof_symbols * sizeof(cf_t));
	return nof_symbols;
}
int precoding_diversity(cf_t *x[MAX_LAYERS], cf_t *y[MAX_PORTS], int nof_ports, int nof_symbols) {
	int i;
	if (nof_ports == 2) {
		/* FIXME: Use VOLK here */
		for (i=0;i<nof_symbols;i++) {
			y[0][2*i] = x[0][i]/sqrtf(2);
			y[1][2*i] = -conjf(x[1][i])/sqrtf(2);
			y[0][2*i+1] = x[1][i]/sqrtf(2);
			y[1][2*i+1] = conjf(x[0][i])/sqrtf(2);
		}
		return i;
	} else if (nof_ports == 4) {
		int m_ap = (nof_symbols%4)?(nof_symbols*4-2):nof_symbols*4;
		for (i=0;i<m_ap;i++) {
			y[0][4*i] = x[0][i]/sqrtf(2);
			y[1][4*i] = 0;
			y[2][4*i] = -conjf(x[1][i])/sqrtf(2);
			y[3][4*i] = 0;

			y[0][4*i+1] = x[1][i]/sqrtf(2);
			y[1][4*i+1] = 0;
			y[2][4*i+1] = conjf(x[0][i])/sqrtf(2);
			y[3][4*i+1] = 0;

			y[0][4*i+2] = 0;
			y[1][4*i+2] = x[2][i]/sqrtf(2);
			y[2][4*i+2] = 0;
			y[3][4*i+2] = -conjf(x[3][i])/sqrtf(2);

			y[0][4*i+3] = 0;
			y[1][4*i+3] = x[3][i]/sqrtf(2);
			y[2][4*i+3] = 0;
			y[3][4*i+3] = conjf(x[2][i])/sqrtf(2);
		}
		return i;
	} else {
		fprintf(stderr, "Number of ports must be 2 or 4 for transmit diversity\n");
		return -1;
	}
}

/* 36.211 v10.3.0 Section 6.3.4 */
int precoding_type(cf_t *x[MAX_LAYERS], cf_t *y[MAX_PORTS], int nof_layers, int nof_ports, int nof_symbols,
		mimo_type_t type) {

	if (nof_ports > MAX_PORTS) {
		fprintf(stderr, "Maximum number of ports is %d (nof_ports=%d)\n", MAX_PORTS, nof_ports);
		return -1;
	}
	if (nof_layers > MAX_LAYERS) {
		fprintf(stderr, "Maximum number of layers is %d (nof_layers=%d)\n", MAX_LAYERS, nof_layers);
		return -1;
	}

	switch(type) {
	case SINGLE_ANTENNA:
		if (nof_ports == 1 && nof_layers == 1) {
			return precoding_single(x[0], y[0], nof_symbols);
		} else {
			fprintf(stderr, "Number of ports and layers must be 1 for transmission on single antenna ports\n");
			return -1;
		}
		break;
	case TX_DIVERSITY:
		if (nof_ports == nof_layers) {
			return precoding_diversity(x, y, nof_ports, nof_symbols);
		} else {
			fprintf(stderr, "Error number of layers must equal number of ports in transmit diversity\n");
			return -1;
		}
	case SPATIAL_MULTIPLEX:
		fprintf(stderr, "Spatial multiplexing not supported\n");
		return -1;
	}
	return 0;
}


/* ZF detector */
int predecoding_single_zf(cf_t *y, cf_t *ce, cf_t *x, int nof_symbols) {
	vec_div_ccc(y, ce, x, nof_symbols);
	return nof_symbols;
}

/* ZF detector */
int predecoding_diversity_zf(cf_t *y[MAX_PORTS], cf_t *ce[MAX_PORTS],
		cf_t *x[MAX_LAYERS], int nof_ports, int nof_symbols) {
	int i;
	cf_t h0, h1, r0, r1;
	float hh;
	if (nof_ports == 2) {
		/* TODO: Use VOLK here */
		for (i=0;i<nof_symbols/2;i++) {
			h0 = ce[0][2*i];
			h1 = ce[1][2*i];
			hh = crealf(h0)*crealf(h0)+cimagf(h0)*cimagf(h0)+
					crealf(h1)*crealf(h1)+cimagf(h1)*cimagf(h1);
			r0 = y[0][2*i];
			r1 = y[0][2*i+1];
			x[0][i] = (conjf(h0)*r0 + h1*conjf(r1))/hh * sqrt(2);
			x[1][i] = (-h1*conj(r0) + conj(h0)*r1)/hh * sqrt(2);
		}
		return i;
	} else if (nof_ports == 4) {
		/*
		int m_ap = (nof_symbols%4)?((nof_symbols-2)/4):nof_symbols/4;
		for (i=0;i<m_ap;i++) {
			h0 = ce[0][2*i];
			h1 = ce[1][2*i];
			h2 = ce[2][2*i];
			h3 = ce[3][2*i];
			hh = crealf(h0)*crealf(h0)+cimagf(h0)*cimagf(h0)
					+ crealf(h1)*crealf(h1)+cimagf(h1)*cimagf(h1)
					+ crealf(h2)*crealf(h2)+cimagf(h2)*cimagf(h2)
					+ crealf(h3)*crealf(h3)+cimagf(h3)*cimagf(h3);
			r0 = y[0][2*i];
			r1 = y[0][2*i+1];
			r2 = y[0][2*i+2];
			r3 = y[0][2*i+3];

			x[0][i] = (conjf(h0)*r0 + conjf(h1)*r1)/hh * sqrt(2);
			x[1][i] = (h1*conj(r0) + conj(h0)*r1)/hh * sqrt(2);
			x[2][i] = (conjf(h0)*r0 + h1*conjf(r1))/hh * sqrt(2);
			x[3][i] = (-h1*conj(r0) + conj(h0)*r1)/hh * sqrt(2);

		}
		*/
		fprintf(stderr, "Error not implemented\n");
		return -1;
	} else {
		fprintf(stderr, "Number of ports must be 2 or 4 for transmit diversity\n");
		return -1;
	}
}

/* 36.211 v10.3.0 Section 6.3.4 */
int predecoding_type(cf_t *y[MAX_PORTS], cf_t *ce[MAX_PORTS],
		cf_t *x[MAX_LAYERS], int nof_ports, int nof_layers, int nof_symbols, mimo_type_t type) {

	if (nof_ports > MAX_PORTS) {
		fprintf(stderr, "Maximum number of ports is %d (nof_ports=%d)\n", MAX_PORTS, nof_ports);
		return -1;
	}
	if (nof_layers > MAX_LAYERS) {
		fprintf(stderr, "Maximum number of layers is %d (nof_layers=%d)\n", MAX_LAYERS, nof_layers);
		return -1;
	}


	switch(type) {
	case SINGLE_ANTENNA:
		if (nof_ports == 1 && nof_layers == 1) {
			return predecoding_single_zf(y[0], ce[0], x[0], nof_symbols);
		} else{
			fprintf(stderr, "Number of ports and layers must be 1 for transmission on single antenna ports\n");
			return -1;
		}
		break;
	case TX_DIVERSITY:
		if (nof_ports == nof_layers) {
			return predecoding_diversity_zf(y, ce, x, nof_ports, nof_symbols);
		} else {
			fprintf(stderr, "Error number of layers must equal number of ports in transmit diversity\n");
			return -1;
		}
		break;
	case SPATIAL_MULTIPLEX:
		fprintf(stderr, "Spatial multiplexing not supported\n");
		return -1;
	}
	return 0;
}

