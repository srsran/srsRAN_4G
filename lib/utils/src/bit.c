/*
 * Copyright (c) 2013, Ismael Gomez-Miguelez <gomezi@tcd.ie>
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

#include <stdint.h>
#include <stdio.h>

void bit_pack(uint32_t value, char **bits, int nof_bits)
{
    int i;

    for(i=0; i<nof_bits; i++) {
        (*bits)[i] = (value >> (nof_bits-i-1)) & 0x1;
    }
    *bits += nof_bits;
}

uint32_t bit_unpack(char **bits, int nof_bits)
{
    int i;
    unsigned int value=0;

    for(i=0; i<nof_bits; i++) {
    	value |= (*bits)[i] << (nof_bits-i-1);
    }
    *bits += nof_bits;
    return value;
}

void bit_fprint(FILE *stream, char *bits, int nof_bits) {
	int i;

	fprintf(stream,"[");
	for (i=0;i<nof_bits-1;i++) {
		fprintf(stream,"%d,",bits[i]);
	}
	fprintf(stream,"%d]\n",bits[i]);
}

unsigned int bit_diff(char *x, char *y, int nbits) {
	unsigned int errors=0;
	for (int i=0;i<nbits;i++) {
		if (x[i] != y[i]) {
			errors++;
		}
	}
	return errors;
}
