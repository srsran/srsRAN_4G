/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
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
#ifndef SRSLTE_LTE_TABLES_H_
#define SRSLTE_LTE_TABLES_H_

#include <srslte/config.h>

#define BPSK_LEVEL M_SQRT1_2

#define QPSK_LEVEL M_SQRT1_2

#define QAM16_LEVEL_1 (1.0f / sqrtf(10.0f))
#define QAM16_LEVEL_2 (3.0f / sqrtf(10.0f))

#define QAM64_LEVEL_1 (1.0f / sqrtf(42.0f))
#define QAM64_LEVEL_2 (3.0f / sqrtf(42.0f))
#define QAM64_LEVEL_3 (5.0f / sqrtf(42.0f))
#define QAM64_LEVEL_4 (7.0f / sqrtf(42.0f))

/* HARD DEMODULATION Thresholds, necessary for obtaining the zone of received symbol for optimized LLR approx
 * implementation */
#define QAM16_THRESHOLD (2.0f / sqrtf(10.0f))
#define QAM64_THRESHOLD_1 (2.0f / sqrtf(42.0f))
#define QAM64_THRESHOLD_2 (4.0f / sqrtf(42.0f))
#define QAM64_THRESHOLD_3 (6.0f / sqrtf(42.0f))
//=========================================//

#define QAM64_LEVEL_x 2 / sqrtf(42)
/* this is not an QAM64 level, but, rather, an auxiliary value that can be used for computing the
 * symbol from the bit sequence */

void set_BPSKtable(cf_t* table);

void set_QPSKtable(cf_t* table);

void set_16QAMtable(cf_t* table);

void set_64QAMtable(cf_t* table);

void set_256QAMtable(cf_t* table);

#endif /* SRSLTE_LTE_TABLES_H_ */
