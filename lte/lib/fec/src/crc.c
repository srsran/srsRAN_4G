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
#include <stdlib.h>
#include <string.h>

#include "lte/utils/pack.h"
#include "lte/fec/crc.h"

void gen_crc_table(crc_t *h) {

	int i, j, ord = (h->order - 8);
	unsigned long bit, crc;

	for (i = 0; i < 256; i++) {
		crc = ((unsigned long) i) << ord;
		for (j = 0; j < 8; j++) {
			bit = crc & h->crchighbit;
			crc <<= 1;
			if (bit)
				crc ^= h->polynom;
		}
		h->table[i] = crc & h->crcmask;
	}
}

unsigned long crctable(crc_t *h) {

	// Polynom order 8, 16, 24 or 32 only.
	int ord = h->order - 8;
	unsigned long crc = h->crcinit;
	unsigned char byte = h->byte;

	crc = (crc << 8) ^ h->table[((crc >> (ord)) & 0xff) ^ byte];
	h->crcinit = crc;
	return (crc  & h->crcmask);
}

unsigned long reversecrcbit(unsigned int crc, int nbits, crc_t *h) {

	unsigned long m, rmask = 0x1;

	for (m = 0; m < nbits; m++) {
		if ((rmask & crc) == 0x01)
			crc = (crc ^ h->polynom) >> 1;
		else
			crc = crc >> 1;
	}
	return (crc & h->crcmask);
}

int crc_set_init(crc_t *crc_par, unsigned long crc_init_value) {

	crc_par->crcinit = crc_init_value;
	if (crc_par->crcinit != (crc_par->crcinit & crc_par->crcmask)) {
		printf("ERROR, invalid crcinit in crc_set_init().\n");
		return -1;
	}
	return 0;
}

int crc_init(crc_t *h, unsigned int crc_poly, int crc_order) {

	// Set crc working default parameters 	
	h->polynom = crc_poly;
	h->order = crc_order;
	h->crcinit = 0x00000000;

	// Compute bit masks for whole CRC and CRC high bit
	h->crcmask = ((((unsigned long) 1 << (h->order - 1)) - 1) << 1)
			| 1;
	h->crchighbit = (unsigned long) 1 << (h->order - 1);

	// check parameters
	if (h->order % 8 != 0) {
		fprintf(stderr, "ERROR, invalid order=%d, it must be 8, 16, 24 or 32.\n",
				h->order);
		return -1;
	}

	if (crc_set_init(h, h->crcinit)) {
		fprintf(stderr, "Error setting CRC init word\n");
		return -1;
	}

	// generate lookup table
	gen_crc_table(h);

	return 0;
}

unsigned int crc_checksum(crc_t *h, char *data, int len) {
	int i, k, len8, res8, a = 0;
	unsigned int crc = 0;
	char *pter;

	crc_set_init(h, 0);

	// Pack bits into bytes
	len8 = (len >> 3);
	res8 = (len - (len8 << 3));
	if (res8 > 0) {
		a = 1;
	}

	// Calculate CRC
	for (i = 0; i < len8 + a; i++) {
		pter = (char *) (data + 8 * i);
		if (i == len8) {
			h->byte = 0x00;
			for (k = 0; k < res8; k++) {
				h->byte |= ((unsigned char) *(pter + k)) << (7 - k);
			}
		} else {
			h->byte = (unsigned char) (unpack_bits(&pter, 8) & 0xFF);
		}
		crc = crctable(h);
	}

	// Reverse CRC res8 positions
	if (a == 1) {
		crc = reversecrcbit(crc, 8 - res8, h);
	}

	//Return CRC value
	return crc;

}

/** Appends crc_order checksum bits to the buffer data.
 * The buffer data must be len + crc_order bytes
 */
void crc_attach(crc_t *h, char *data, int len) {
	unsigned int checksum = crc_checksum(h, data, len);

	// Add CRC
	char *ptr = &data[len];
	pack_bits(checksum, &ptr, h->order);
}

