/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/******************************************************************************
 *  File:         scrambling.h
 *
 *  Description:  Generic scrambling functions used by UL and DL channels.
 *
 *  Reference:    3GPP TS 36.211 version 10.0.0 Release 10 Sec. 5.3.1, 6.3.1
 *****************************************************************************/

#ifndef SRSLTE_SCRAMBLING_H
#define SRSLTE_SCRAMBLING_H

#include "srslte/config.h"
#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/common/sequence.h"

/* Scrambling has no state */
SRSLTE_API void srslte_scrambling_b(srslte_sequence_t* s, uint8_t* data);

SRSLTE_API void srslte_scrambling_b_offset(srslte_sequence_t* s, uint8_t* data, int offset, int len);

SRSLTE_API void srslte_scrambling_bytes(srslte_sequence_t* s, uint8_t* data, int len);

SRSLTE_API void srslte_scrambling_f(srslte_sequence_t* s, float* data);

SRSLTE_API void srslte_scrambling_f_offset(srslte_sequence_t* s, float* data, int offset, int len);

SRSLTE_API void srslte_scrambling_s(srslte_sequence_t* s, short* data);

SRSLTE_API void srslte_scrambling_s_offset(srslte_sequence_t* s, short* data, int offset, int len);

SRSLTE_API void srslte_scrambling_sb_offset(srslte_sequence_t* s, int8_t* data, int offset, int len);

SRSLTE_API void srslte_scrambling_c(srslte_sequence_t* s, cf_t* data);

SRSLTE_API void srslte_scrambling_c_offset(srslte_sequence_t* s, cf_t* data, int offset, int len);

#endif // SRSLTE_SCRAMBLING_H
