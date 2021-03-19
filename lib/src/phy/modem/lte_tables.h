/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */
#ifndef SRSRAN_LTE_TABLES_H_
#define SRSRAN_LTE_TABLES_H_

#include <srsran/config.h>

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

#endif /* SRSRAN_LTE_TABLES_H_ */
