/*
 * Copyright (c) 2013, Ismael Gomez-Miguelez <gomezi@tcd.ie>, Vuk Marojevic <maroje@vt.edu>.
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


#define BPSK_LEVEL      1/sqrt(2)

#define QPSK_LEVEL      1/sqrt(2)

#define QAM16_LEVEL_1	1/sqrt(10)
#define QAM16_LEVEL_2	3/sqrt(10)

#define QAM64_LEVEL_1	1/sqrt(42)
#define QAM64_LEVEL_2	3/sqrt(42)
#define QAM64_LEVEL_3	5/sqrt(42)
#define QAM64_LEVEL_4	7/sqrt(42)

#define QAM64_LEVEL_x	2/sqrt(42)
/* this is not an QAM64 level, but, rather, an auxiliary value that can be used for computing the
 * symbol from the bit sequence */




void set_BPSKtable(cf* table, soft_table_t *soft_table, bool compute_soft_demod);
void set_QPSKtable(cf* table, soft_table_t *soft_table, bool compute_soft_demod);
void set_16QAMtable(cf* table, soft_table_t *soft_table, bool compute_soft_demod);
void set_64QAMtable(cf* table, soft_table_t *soft_table, bool compute_soft_demod);
