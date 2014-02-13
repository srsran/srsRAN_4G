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

#include "utils/bit.h"
#include "modem/demod_soft.h"
#include "soft_algs.h"


void demod_soft_init(demod_soft_t *q) {
	bzero((void*)q,sizeof(demod_soft_t));
}

void demod_soft_table_set(demod_soft_t *q, modem_table_t *table) {
	q->table = table;
}

void demod_soft_alg_set(demod_soft_t *q, enum alg alg_type) {
	q->alg_type = alg_type;
}

void demod_soft_sigma_set(demod_soft_t *q, float sigma) {
	q->sigma = sigma;
}

int demod_soft_demodulate(demod_soft_t *q, const cf* symbols, float* llr, int nsymbols) {
	switch(q->alg_type) {
	case EXACT:
		llr_exact(symbols, llr, nsymbols, q->table->nsymbols, q->table->nbits_x_symbol,
				q->table->symbol_table, q->table->soft_table.idx, q->sigma);
		break;
	case APPROX:
		llr_approx(symbols, llr, nsymbols, q->table->nsymbols, q->table->nbits_x_symbol,
				q->table->symbol_table, q->table->soft_table.idx, q->sigma);
		break;
	}
	return nsymbols*q->table->nbits_x_symbol;
}



/* High-Level API */
int demod_soft_initialize(demod_soft_hl* hl) {
	modem_table_init(&hl->table);
	if (modem_table_std(&hl->table,hl->init.std,true)) {
		return -1;
	}
	demod_soft_init(&hl->obj);
	hl->obj.table = &hl->table;

	return 0;
}

int demod_soft_work(demod_soft_hl* hl) {
	hl->obj.sigma = hl->ctrl_in.sigma;
	hl->obj.alg_type = hl->ctrl_in.alg_type;
	int ret = demod_soft_demodulate(&hl->obj,hl->input,hl->output,hl->in_len);
	hl->out_len = ret;
	return 0;
}

int demod_soft_stop(demod_soft_hl* hl) {
	modem_table_free(&hl->table);
	return 0;
}
