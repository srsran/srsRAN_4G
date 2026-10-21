// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

#include "srsran/config.h"
#include <inttypes.h>

/* Thresholds for Demodulation */
/* Assume perfect amplitude and phase alignment.
 *  Check threshold values for real case
 *  or implement dynamic threshold adjustent as a function of received symbol amplitudes */
#define QAM16_THRESHOLD (2 / sqrtf(10))
#define QAM64_THRESHOLD_1 (2 / sqrtf(42))
#define QAM64_THRESHOLD_2 (4 / sqrtf(42))
#define QAM64_THRESHOLD_3 (6 / sqrtf(42))
#define QAM256_THRESHOLD_1 (2 / sqrtf(170))
#define QAM256_THRESHOLD_2 (4 / sqrtf(170))
#define QAM256_THRESHOLD_3 (6 / sqrtf(170))
#define QAM256_THRESHOLD_4 (8 / sqrtf(170))
#define QAM256_THRESHOLD_5 (10 / sqrtf(170))
#define QAM256_THRESHOLD_6 (12 / sqrtf(170))
#define QAM256_THRESHOLD_7 (14 / sqrtf(170))

void hard_bpsk_demod(const cf_t* in, uint8_t* out, uint32_t N);

void hard_qpsk_demod(const cf_t* in, uint8_t* out, uint32_t N);

void hard_qam16_demod(const cf_t* in, uint8_t* out, uint32_t N);

void hard_qam64_demod(const cf_t* in, uint8_t* out, uint32_t N);

void hard_qam256_demod(const cf_t* in, uint8_t* out, uint32_t N);
