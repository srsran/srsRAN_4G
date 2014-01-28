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
#include <strings.h>

#include "modem/demod_hard.h"
#include "hard_demod_lte.h"


void demod_hard_init(demod_hard_t* q) {
	bzero((void*) q, sizeof(demod_hard_t));
}

void demod_hard_table(demod_hard_t* q, enum modem_std table) {
	q->table = table;
}

int demod_hard_demodulate(demod_hard_t* q, const cf* symbols, char *bits, int nsymbols) {

	int nbits=-1;
	switch(q->table) {
	case LTE_BPSK:
		hard_bpsk_demod(symbols,bits,nsymbols);
		nbits=nsymbols;
		break;
	case LTE_QPSK:
		hard_qpsk_demod(symbols,bits,nsymbols);
		nbits=nsymbols*2;
		break;
	case LTE_QAM16:
		hard_qam16_demod(symbols,bits,nsymbols);
		nbits=nsymbols*4;
		break;
	case LTE_QAM64:
		hard_qam64_demod(symbols,bits,nsymbols);
		nbits=nsymbols*6;
		break;
	}
	return nbits;
}


int demod_hard_initialize(demod_hard_hl* hl) {
	demod_hard_init(&hl->obj);
	demod_hard_table(&hl->obj,hl->init.std);

	return 0;
}

int demod_hard_work(demod_hard_hl* hl) {
	int ret = demod_hard_demodulate(&hl->obj,hl->input,hl->output,hl->in_len);
	if (hl->out_len) {
		*hl->out_len = ret;
	}
	return 0;
}



