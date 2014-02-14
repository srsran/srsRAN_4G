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

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <stdbool.h>

#include "common.h"
#include "phch/pbch.h"
#include "lte/base.h"
#include "utils/bit.h"
#include "utils/vector.h"
#include "utils/debug.h"

bool pbch_exists(int nframe, int nslot) {
	return (!(nframe % 4) && nslot == 1);
}

int pbch_cp(cf_t *input, cf_t *output, int nof_prb, lte_cp_t cp, int cell_id, bool put) {
	int i;
	cf_t *ptr;
	if (put) {
		ptr = input;
		output += nof_prb * RE_X_RB / 2 - 36;
	} else {
		ptr = output;
		input += nof_prb * RE_X_RB / 2 - 36;
	}

	/* symbol 0 & 1 */
	for (i=0;i<2;i++) {
		phch_cp_prb_ref(&input, &output, cell_id%3, 4, 6, put);
	}
	/* symbols 2 & 3 */
	if (CP_ISNORM(cp)) {
		for (i=0;i<2;i++) {
			phch_cp_prb(&input, &output, 6);
		}
	} else {
		phch_cp_prb(&input, &output, 6);
		phch_cp_prb_ref(&input, &output, cell_id%3, 4, 6, put);
	}
	if (put) {
		return input - ptr;
	} else {
		return output - ptr;
	}
}

/**
 * Puts PBCH in slot number 1
 *
 * Returns the number of symbols written to slot1_data
 *
 * 36.211 10.3 section 6.6.4
 */
int pbch_put(cf_t *pbch, cf_t *slot1_data, int nof_prb, lte_cp_t cp, int cell_id) {
	return pbch_cp(pbch, slot1_data, nof_prb, cp, cell_id, true);
}

/**
 * Extracts PBCH from slot number 1
 *
 * Returns the number of symbols written to pbch
 *
 * 36.211 10.3 section 6.6.4
 */
int pbch_get(cf_t *slot1_data, cf_t *pbch, int nof_prb, lte_cp_t cp, int cell_id) {
	return pbch_cp(slot1_data, pbch, nof_prb, cp, cell_id, false);
}


/* Checks CRC and blindly obtains the number of ports, which is saved in nof_ports.
 *
 * The bits buffer size must be at least 40 bytes.
 *
 * Returns 0 if the data is correct, -1 otherwise
 */
int pbch_crc_check(char *bits, int *nof_ports) {
	int i, j;
	const char crc_mask[3][16] = {
			{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
			{0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1}
	};
	const int ports[3] = {1, 2, 4};
	char data[40];

	memcpy(data, bits, 24 * sizeof(char));

	for (i=0;i<3;i++) {
		for (j=0;j<16;j++) {
			data[24+j] = (bits[24+j] + crc_mask[i][j]) % 2;
		}
		if (!crc(0, data, 40, 16, 0x11021, 0)) {
			*nof_ports = ports[i];
			return 0;
		}
	}
	*nof_ports = 0;
	return -1;
}


/** Initializes the PBCH channel receiver */
int pbch_init(pbch_t *q, int cell_id, lte_cp_t cp) {
	int ret = -1;
	bzero(q, sizeof(pbch_t));
	q->cell_id = cell_id;
	q->cp = cp;
	if (modem_table_std(&q->mod, LTE_QPSK, true)) {
		goto clean;
	}
	demod_soft_init(&q->demod);
	demod_soft_table_set(&q->demod, &q->mod);
	demod_soft_alg_set(&q->demod, APPROX);
	if (sequence_pbch(&q->seq_pbch, q->cp, q->cell_id)) {
		goto clean;
	}

	int poly[3] = {0x6D, 0x4F, 0x57};
	if (viterbi_init(&q->decoder, viterbi_37, poly, 40, true)) {
		goto clean;
	}
	int nof_symbols = (CP_ISNORM(q->cp)) ? PBCH_RE_CPNORM: PBCH_RE_CPEXT;

	q->pbch_symbols = malloc(sizeof(cf_t) * nof_symbols);
	if (!q->pbch_symbols) {
		goto clean;
	}
	q->pbch_llr = malloc(sizeof(float) * nof_symbols * 4 * 2);
	if (!q->pbch_llr) {
		goto clean;
	}
	q->temp = malloc(sizeof(float) * nof_symbols * 4 * 2);
	if (!q->temp) {
		goto clean;
	}
	q->pbch_rm = malloc(sizeof(float) * 120);
	if (!q->pbch_rm) {
		goto clean;
	}
	q->data = malloc(sizeof(char) * 40);
	if (!q->data) {
		goto clean;
	}

	ret = 0;
clean:
	if (ret == -1) {
		pbch_free(q);
	}
	return ret;
}

void pbch_free(pbch_t *q) {
	if (q->pbch_symbols) {
		free(q->pbch_symbols);
	}
	if (q->pbch_llr) {
		free(q->pbch_llr);
	}
	if (q->pbch_rm) {
		free(q->pbch_rm);
	}
	if (q->data) {
		free(q->data);
	}
	sequence_free(&q->seq_pbch);
	modem_table_free(&q->mod);
	viterbi_free(&q->decoder);
}

/** Unpacks MIB from PBCH message.
 * msg buffer must be 24 byte length at least
 */
void pbch_mib_unpack(char *msg, pbch_mib_t *mib) {
	int bw, phich_res;
	char *buffer;

	bw = 4*msg[0] + 2*msg[1] + msg[2];
	switch(bw) {
	case 0:
		mib->nof_prb = 6;
		break;
	case 1:
		mib->nof_prb = 15;
		break;
	default:
		mib->nof_prb = (bw-1)*25;
		break;
	}
	if (msg[3]) {
		mib->phich_length = EXTENDED;
	} else {
		mib->phich_length = NORMAL;
	}
	phich_res = 2*msg[4] + msg[5];
	switch(phich_res) {
	case 0:
		mib->phich_resources = R_1_6;
		break;
	case 1:
		mib->phich_resources = R_1_2;
		break;
	case 2:
		mib->phich_resources = R_1;
		break;
	case 3:
		mib->phich_resources = R_2;
		break;
	}
	buffer = &msg[6];
	mib->sfn = bit_unpack(&buffer, 8);
}

void pbch_mib_fprint(FILE *stream, pbch_mib_t *mib) {
	printf(" - Nof ports:       %d\n", mib->nof_ports);
	printf(" - PRB:             %d\n", mib->nof_prb);
	printf(" - PHICH Length:    %s\n", mib->phich_length==EXTENDED?"Extended":"Normal");
	printf(" - PHICH Resources: ");
	switch(mib->phich_resources) {
	case R_1_6:
		printf("1/6");
		break;
	case R_1_2:
		printf("1/2");
		break;
	case R_1:
		printf("1");
		break;
	case R_2:
		printf("2");
		break;
	}
	printf("\n");
	printf(" - SFN:             %d\n", mib->sfn);
}

void pbch_decode_reset(pbch_t *q) {
	q->frame_idx = 0;
}

int pbch_decode_frame(pbch_t *q, pbch_mib_t *mib, int src, int dst, int n, int nof_bits) {
	int j;

	memcpy(&q->temp[dst*nof_bits], &q->pbch_llr[src*nof_bits], n*nof_bits*sizeof(float));

	/* descramble */
	scrambling_float_offset(&q->seq_pbch, &q->temp[dst*nof_bits], dst*nof_bits, n*nof_bits);

	for (j=0;j<dst*nof_bits;j++) {
		q->temp[j] = RX_NULL;
	}
	for (j=(dst+n)*nof_bits;j<4*nof_bits;j++) {
		q->temp[j] = RX_NULL;
	}

	/* unrate matching */
	rm_conv_rx(q->temp, q->pbch_rm, 4 * nof_bits, 120);

	/* decode */
	viterbi_decode(&q->decoder, q->pbch_rm, q->data);

	/* check crc and get nof ports */
	if (pbch_crc_check(q->data, &mib->nof_ports)) {

		return 0;
	} else {

		printf("BCH Decoded Correctly.\n");

		/* unpack MIB */
		pbch_mib_unpack(q->data, mib);

		mib->sfn += dst-src;

		pbch_mib_fprint(stdout, mib);

		return 1;
	}
}

/* Decodes the PBCH channel
 *
 * The PBCH spans in 40 ms. This function is called every 10 ms. It tries to decode the MIB
 * given the symbols of the slot #1 of each radio frame. Successive calls will use more frames
 * to help the decoding process.
 *
 * Returns 1 if successfully decoded MIB, 0 if not and -1 on error
 */
int pbch_decode(pbch_t *q, cf_t *slot1_symbols, cf_t **ce, int nof_ports,
		int nof_prb, float ebno, pbch_mib_t *mib) {
	int src, dst, res, nb, nant;

	int nof_symbols = (CP_ISNORM(q->cp)) ? PBCH_RE_CPNORM: PBCH_RE_CPEXT;
	int nof_bits = 2 * nof_symbols;

	/* extract symbols */
	if (nof_symbols != pbch_get(slot1_symbols, q->pbch_symbols, nof_prb,
			q->cp, q->cell_id)) {
		fprintf(stderr, "There was an error getting the PBCH symbols\n");
		return -1;
	}

	/* Try decoding for 1 to nof_ports antennas */
	for (nant=0;nant<nof_ports;nant++) {

		/*@TODO: pre-decoder & matched filter */
		int i;
		for (i=0;i<nof_symbols;i++) {
			q->pbch_symbols[i] /= ce[0][i];
		}

		/*@TODO: layer demapping */

		/* demodulate symbols */
		demod_soft_sigma_set(&q->demod, ebno);
		demod_soft_demodulate(&q->demod, q->pbch_symbols,
				&q->pbch_llr[nof_bits * q->frame_idx], nof_symbols);

		q->frame_idx++;

		INFO("PBCH: %d frames in buffer\n", q->frame_idx);

		/* We don't know where the 40 ms begin, so we try all combinations. E.g. if we received
		 * 4 frames, try 1,2,3,4 individually, 12, 23, 34 in pairs, 123, 234 and finally 1234.
		 * We know they are ordered.
		 */
		res = 0;
		for (nb=0;nb<q->frame_idx && !res;nb++) {
			for (dst=0;(dst<4-nb) && !res;dst++) {
				for (src=0;src<q->frame_idx && !res;src++) {
					DEBUG("Trying %d blocks at offset %d as subframe mod4 number %d\n", nb+1, src, dst);
					res = pbch_decode_frame(q, mib, src, dst, nb+1, nof_bits);
				}
			}
		}

		if (res) {
			q->frame_idx = 0;
			return 1;
		}
	}

	/* If not found, make room for the next packet of radio frame symbols */
	if (q->frame_idx == 4) {
		memcpy(&q->pbch_llr[nof_bits], q->pbch_llr, nof_bits * 3 * sizeof(float));
		q->frame_idx = 3;
	}
	return 0;
}
