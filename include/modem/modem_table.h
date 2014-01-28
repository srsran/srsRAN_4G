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


#ifndef MODEM_TABLE_
#define MODEM_TABLE_

#include <stdbool.h>
#include <complex.h>
#include <stdint.h>

typedef _Complex float cf;
typedef struct {
	int idx[2][6][32];
}soft_table_t;

typedef struct {
	cf* symbol_table; 			// bit-to-symbol mapping
	soft_table_t soft_table; 	// symbol-to-bit mapping (used in soft demodulating)
	int nsymbols;				// number of modulation symbols
	int nbits_x_symbol;			// number of bits per symbol
}modem_table_t;


// Modulation standards
enum modem_std {
	LTE_BPSK, LTE_QPSK, LTE_QAM16, LTE_QAM64
};

void modem_table_init(modem_table_t* q);
void modem_table_free(modem_table_t* q);
void modem_table_reset(modem_table_t* q);
int modem_table_set(modem_table_t* q, cf* table, soft_table_t *soft_table, int nsymbols, int nbits_x_symbol);
int modem_table_std(modem_table_t* q, enum modem_std table, bool compute_soft_demod);

#endif
