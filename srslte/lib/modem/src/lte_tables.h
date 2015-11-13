/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
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

/* HARD DEMODULATION Thresholds, necessary for obtaining the zone of received symbol for optimized LLR approx implementation */
#define QAM16_THRESHOLD         2/sqrt(10)
#define QAM64_THRESHOLD_1       2/sqrt(42)
#define QAM64_THRESHOLD_2       4/sqrt(42)
#define QAM64_THRESHOLD_3       6/sqrt(42)
//=========================================//

#define QAM64_LEVEL_x  2/sqrt(42)
/* this is not an QAM64 level, but, rather, an auxiliary value that can be used for computing the
 * symbol from the bit sequence */




void set_BPSKtable(cf_t* table);

void set_QPSKtable(cf_t* table);

void set_16QAMtable(cf_t* table);

void set_64QAMtable(cf_t* table);
