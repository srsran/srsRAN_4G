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


#define BPSK_LEVEL      1/sqrt(2)

#define QPSK_LEVEL      1/sqrt(2)

#define QAM16_LEVEL_1  1/sqrt(10)
#define QAM16_LEVEL_2  3/sqrt(10)

#define QAM64_LEVEL_1  1/sqrt(42)
#define QAM64_LEVEL_2  3/sqrt(42)
#define QAM64_LEVEL_3  5/sqrt(42)
#define QAM64_LEVEL_4  7/sqrt(42)

//////////////// NUEVO //////////////////////
/* HARD DEMODULATION Thresholds, necessary for obtaining the zone of received symbol for optimized LLR approx implementation */
#define QAM16_THRESHOLD		2/sqrt(10)
#define QAM64_THRESHOLD_1	2/sqrt(42)
#define QAM64_THRESHOLD_2	4/sqrt(42)
#define QAM64_THRESHOLD_3	6/sqrt(42)
//=========================================//

#define QAM64_LEVEL_x  2/sqrt(42)
/* this is not an QAM64 level, but, rather, an auxiliary value that can be used for computing the
 * symbol from the bit sequence */




void set_BPSKtable(cf_t* table, 
                   soft_table_t *soft_table, 
                   bool compute_soft_demod);

void set_QPSKtable(cf_t* table, 
                   soft_table_t *soft_table, 
                   bool compute_soft_demod);

void set_16QAMtable(cf_t* table, 
                    soft_table_t *soft_table, 
                    bool compute_soft_demod);

void set_64QAMtable(cf_t* table, 
                    soft_table_t *soft_table, 
                    bool compute_soft_demod);
