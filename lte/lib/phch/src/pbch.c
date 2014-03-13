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


#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>

#include "phch.h"
#include "lte/phch/pbch.h"
#include "lte/common/base.h"
#include "lte/utils/bit.h"
#include "lte/utils/vector.h"
#include "lte/utils/debug.h"
#include "lte/io/udpsink.h"

const char crc_mask[4][16] = {
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1}
};


bool pbch_exists(int nframe, int nslot) {
	return (!(nframe % 4) && nslot == 1);
}

int pbch_cp(cf_t *input, cf_t *output, int nof_prb, lte_cp_t cp, int cell_id, bool put) {
	int i;
	cf_t *ptr;
	assert(cell_id >= 0);
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

/** Initializes the PBCH channel receiver */
int pbch_init(pbch_t *q, int cell_id, lte_cp_t cp) {
	int ret = -1;
	if (cell_id < 0) {
		return -1;
	}
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
	q->encoder.K = 7;
	q->encoder.R = 3;
	q->encoder.framelength = 40;
	q->encoder.tail_biting = true;
	memcpy(q->encoder.poly, poly, 3 * sizeof(int));

	q->nof_symbols = (CP_ISNORM(q->cp)) ? PBCH_RE_CPNORM: PBCH_RE_CPEXT;

	q->pbch_d = malloc(sizeof(cf_t) * q->nof_symbols);
	if (!q->pbch_d) {
		goto clean;
	}
	int i;
	for (i=0;i<MAX_PORTS_CTRL;i++) {
		q->ce[i] = malloc(sizeof(cf_t) * q->nof_symbols);
		if (!q->ce[i]) {
			goto clean;
		}
		q->pbch_x[i] = malloc(sizeof(cf_t) * q->nof_symbols);
		if (!q->pbch_x[i]) {
			goto clean;
		}
		q->pbch_symbols[i] = malloc(sizeof(cf_t) * q->nof_symbols);
		if (!q->pbch_symbols[i]) {
			goto clean;
		}
	}
	q->pbch_llr = malloc(sizeof(float) * q->nof_symbols * 4 * 2);
	if (!q->pbch_llr) {
		goto clean;
	}
	q->temp = malloc(sizeof(float) * q->nof_symbols * 4 * 2);
	if (!q->temp) {
		goto clean;
	}
	q->pbch_rm_f = malloc(sizeof(float) * 120);
	if (!q->pbch_rm_f) {
		goto clean;
	}
	q->pbch_rm_b = malloc(sizeof(float) * q->nof_symbols * 4 * 2);
	if (!q->pbch_rm_b) {
		goto clean;
	}
	q->data = malloc(sizeof(char) * 40);
	if (!q->data) {
		goto clean;
	}
	q->data_enc = malloc(sizeof(char) * 120);
	if (!q->data_enc) {
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
	if (q->pbch_d) {
		free(q->pbch_d);
	}
	int i;
	for (i=0;i<MAX_PORTS_CTRL;i++) {
		if (q->ce[i]) {
			free(q->ce[i]);
		}
		if (q->pbch_x[i]) {
			free(q->pbch_x[i]);
		}
		if (q->pbch_symbols[i]) {
			free(q->pbch_symbols[i]);
		}
	}
	if (q->pbch_llr) {
		free(q->pbch_llr);
	}
	if (q->pbch_rm_f) {
		free(q->pbch_rm_f);
	}
	if (q->pbch_rm_b) {
		free(q->pbch_rm_b);
	}
	if (q->data_enc) {
		free(q->data_enc);
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

	bw = bit_unpack(&msg, 3);
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
	if (*msg) {
		mib->phich_length = EXTENDED;
	} else {
		mib->phich_length = NORMAL;
	}
	msg++;

	phich_res = bit_unpack(&msg, 2);
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
	mib->sfn = bit_unpack(&msg, 8) << 2;
}


/** Unpacks MIB from PBCH message.
 * msg buffer must be 24 byte length at least
 */
void pbch_mib_pack(pbch_mib_t *mib, char *msg) {
	int bw, phich_res=0;

	bzero(msg, 24);

	if (mib->nof_prb<=6) {
		bw = 0;
	} else if (mib->nof_prb <= 15) {
		bw = 1;
	} else {
		bw = 1 + mib->nof_prb/25;
	}
	bit_pack(bw, &msg, 3);

	*msg = mib->phich_length == EXTENDED;
	msg++;

	switch(mib->phich_resources) {
	case R_1_6:
		phich_res = 0;
		break;
	case R_1_2:
		phich_res = 1;
		break;
	case R_1:
		phich_res = 2;
		break;
	case R_2:
		phich_res = 3;
		break;
	}
	bit_pack(phich_res, &msg, 2);
	bit_pack(mib->sfn >> 2, &msg, 8);
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


void crc_set_mask(char *data, int nof_ports) {
	int i;
	for (i=0;i<16;i++) {
		data[24+i] = (data[24+i] + crc_mask[nof_ports-1][i]) % 2;
	}

}


/* Checks CRC after applying the mask for the given number of ports.
 *
 * The bits buffer size must be at least 40 bytes.
 *
 * Returns 0 if the data is correct, -1 otherwise
 */
int pbch_crc_check(char *bits, int nof_ports) {
	char data[40];
	memcpy(data, bits, 40 * sizeof(char));
	crc_set_mask(data, nof_ports);
	return crc(0, data, 40, 16, 0x11021, 0);
}

int pbch_decode_frame(pbch_t *q, pbch_mib_t *mib, int src, int dst, int n, int nof_bits, int nof_ports) {
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
	rm_conv_rx(q->temp, q->pbch_rm_f, 4 * nof_bits, 120);

	/* FIXME: If channel estimates are zero, received LLR are NaN. Check and return error */
	for (j=0;j<120;j++) {
		if (isnan(q->pbch_rm_f[j]) || isinf(q->pbch_rm_f[j])) {
			return 0;
		}
	}

	/* decode */
	viterbi_decode_f(&q->decoder, q->pbch_rm_f, q->data);

	int c=0;
	for (j=0;j<40;j++) {
		c+=q->data[j];
	}
	if (!c) {
		c=1;
	}

	if (!pbch_crc_check(q->data, nof_ports)) {
		/* unpack MIB */
		pbch_mib_unpack(q->data, mib);

		mib->nof_ports = nof_ports;
		mib->sfn += dst-src;

		return 1;
	} else {
		return 0;
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
int pbch_decode(pbch_t *q, cf_t *slot1_symbols, cf_t *ce[MAX_PORTS_CTRL], int nof_prb, float ebno, pbch_mib_t *mib) {
	int src, dst, res, nb, nant;

	/* Set pointers for layermapping & precoding */
	int i;
	int nof_bits = 2 * q->nof_symbols;
	cf_t *x[MAX_LAYERS];

	/* number of layers equals number of ports */
	for (i=0;i<MAX_PORTS_CTRL;i++) {
		x[i] = q->pbch_x[i];
	}
	memset(&x[MAX_PORTS_CTRL], 0, sizeof(cf_t*) * (MAX_LAYERS - MAX_PORTS_CTRL));

	/* extract symbols */
	if (q->nof_symbols != pbch_get(slot1_symbols, q->pbch_symbols[0], nof_prb,
			q->cp, q->cell_id)) {
		fprintf(stderr, "There was an error getting the PBCH symbols\n");
		return -1;
	}

	/* extract channel estimates */
	for (i=0;i<MAX_PORTS_CTRL;i++) {
		if (q->nof_symbols != pbch_get(ce[i], q->ce[i], nof_prb,
				q->cp, q->cell_id)) {
			fprintf(stderr, "There was an error getting the PBCH symbols\n");
			return -1;
		}
	}

	q->frame_idx++;
	res = 0;

	/* Try decoding for 1 to 4 antennas */
	/** currently only 2 TX antennas are supported */
	for (nant=1;nant<=2 && !res;nant++) {

		INFO("Trying %d TX antennas with %d frames\n", nant, q->frame_idx);

		/* in conctrol channels, only diversity is supported */
		if (nant == 1) {
			/* no need for layer demapping */
			predecoding_single_zf(q->pbch_symbols[0], q->ce[0], q->pbch_d, q->nof_symbols);
		} else {
			predecoding_diversity_zf(q->pbch_symbols, q->ce, x, nant, q->nof_symbols);
			layerdemap_diversity(x, q->pbch_d, nant, q->nof_symbols/nant);
		}

		/* demodulate symbols */
		demod_soft_sigma_set(&q->demod, ebno);
		demod_soft_demodulate(&q->demod, q->pbch_d,
				&q->pbch_llr[nof_bits * (q->frame_idx - 1)], q->nof_symbols);

		/* We don't know where the 40 ms begin, so we try all combinations. E.g. if we received
		 * 4 frames, try 1,2,3,4 individually, 12, 23, 34 in pairs, 123, 234 and finally 1234.
		 * We know they are ordered.
		 *
		 * FIXME: There are unnecessary checks because 2,3,4 have already been processed in the previous
		 * calls.
		 */
		for (nb=0;nb<q->frame_idx && !res;nb++) {
			for (dst=0;(dst<4-nb) && !res;dst++) {
				for (src=0;src<q->frame_idx-nb && !res;src++) {
					DEBUG("Trying %d blocks at offset %d as subframe mod4 number %d\n", nb+1, src, dst);
					res = pbch_decode_frame(q, mib, src, dst, nb+1, nof_bits, nant);
				}
			}
		}
	}

	/* If not found, make room for the next packet of radio frame symbols */
	if (q->frame_idx == 4) {
		memmove(q->pbch_llr, &q->pbch_llr[nof_bits], nof_bits * 3 * sizeof(float));
		q->frame_idx = 3;
	}
	return res;
}


/** Converts the MIB message to symbols mapped to SLOT #1 ready for transmission
 */
void pbch_encode(pbch_t *q, pbch_mib_t *mib, cf_t *slot1_symbols[MAX_PORTS_CTRL],
		int nof_prb, int nof_ports) {
	int i;
	int nof_bits = 2 * q->nof_symbols;

	assert(nof_ports < MAX_PORTS_CTRL);

	/* Set pointers for layermapping & precoding */
	cf_t *x[MAX_LAYERS];

	/* number of layers equals number of ports */
	for (i=0;i<nof_ports;i++) {
		x[i] = q->pbch_x[i];
	}
	memset(&x[nof_ports], 0, sizeof(cf_t*) * (MAX_LAYERS - nof_ports));

	if (q->frame_idx == 0) {
		/* pack MIB */
		pbch_mib_pack(mib, q->data);

		/* encode & modulate */
		crc(0, q->data, 24, 16, 0x11021, 1);
		crc_set_mask(q->data, nof_ports);

		convcoder_encode(&q->encoder, q->data, q->data_enc);

		rm_conv_tx(q->data_enc, q->pbch_rm_b, 120, 4 * nof_bits);

	}

	scrambling_bit_offset(&q->seq_pbch, &q->pbch_rm_b[q->frame_idx * nof_bits],
			q->frame_idx * nof_bits, nof_bits);
	mod_modulate(&q->mod, &q->pbch_rm_b[q->frame_idx * nof_bits], q->pbch_d, nof_bits);


	/* layer mapping & precoding */
	if (nof_ports > 1) {
		layermap_diversity(q->pbch_d, x, nof_ports, q->nof_symbols/nof_ports);
		precoding_diversity(x, q->pbch_symbols, nof_ports, q->nof_symbols/nof_ports);
	} else {
		memcpy(q->pbch_symbols[0], q->pbch_d, q->nof_symbols * sizeof(cf_t));
	}


	/* mapping to resource elements */
	for (i=0;i<nof_ports;i++) {
		pbch_put(q->pbch_symbols[i], slot1_symbols[i], nof_prb, q->cp, q->cell_id);
	}
	q->frame_idx++;
	if (q->frame_idx == 4) {
		q->frame_idx = 0;
	}
}



