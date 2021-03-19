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

/******************************************************************************
 *  File:         scrambling.h
 *
 *  Description:  Generic scrambling functions used by UL and DL channels.
 *
 *  Reference:    3GPP TS 36.211 version 10.0.0 Release 10 Sec. 5.3.1, 6.3.1
 *****************************************************************************/

#ifndef SRSRAN_SCRAMBLING_H
#define SRSRAN_SCRAMBLING_H

#include "srsran/config.h"
#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/common/sequence.h"

/* Scrambling has no state */
SRSRAN_API void srsran_scrambling_b(srsran_sequence_t* s, uint8_t* data);

SRSRAN_API void srsran_scrambling_b_offset(srsran_sequence_t* s, uint8_t* data, int offset, int len);

SRSRAN_API void srsran_scrambling_bytes(srsran_sequence_t* s, uint8_t* data, int len);

SRSRAN_API void srsran_scrambling_f(srsran_sequence_t* s, float* data);

SRSRAN_API void srsran_scrambling_f_offset(srsran_sequence_t* s, float* data, int offset, int len);

SRSRAN_API void srsran_scrambling_s(srsran_sequence_t* s, short* data);

SRSRAN_API void srsran_scrambling_s_offset(srsran_sequence_t* s, short* data, int offset, int len);

SRSRAN_API void srsran_scrambling_sb_offset(srsran_sequence_t* s, int8_t* data, int offset, int len);

SRSRAN_API void srsran_scrambling_c(srsran_sequence_t* s, cf_t* data);

SRSRAN_API void srsran_scrambling_c_offset(srsran_sequence_t* s, cf_t* data, int offset, int len);

#endif // SRSRAN_SCRAMBLING_H
