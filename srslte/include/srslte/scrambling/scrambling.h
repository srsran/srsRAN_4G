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

/******************************************************************************
 *  File:         scrambling.h
 *
 *  Description:  Generic scrambling functions used by UL and DL channels.
 *
 *  Reference:    3GPP TS 36.211 version 10.0.0 Release 10 Sec. 5.3.1, 6.3.1
 *****************************************************************************/

#ifndef SCRAMBLING_
#define SCRAMBLING_

#include "srslte/config.h"
#include "srslte/common/sequence.h"
#include "srslte/common/phy_common.h"

/* Scrambling has no state */
SRSLTE_API void srslte_scrambling_b(srslte_sequence_t *s, 
                                    uint8_t *data);

SRSLTE_API void srslte_scrambling_b_offset(srslte_sequence_t *s, 
                                           uint8_t *data, 
                                           int offset, 
                                           int len);

SRSLTE_API void srslte_scrambling_bytes(srslte_sequence_t *s, 
                                        uint8_t *data, 
                                        int len); 

SRSLTE_API void srslte_scrambling_f(srslte_sequence_t *s, 
                                    float *data);

SRSLTE_API void srslte_scrambling_f_offset(srslte_sequence_t *s, 
                                           float *data, 
                                           int offset, 
                                           int len);

SRSLTE_API void srslte_scrambling_s(srslte_sequence_t *s, 
                                    short *data);

SRSLTE_API void srslte_scrambling_s_offset(srslte_sequence_t *s, 
                                           short *data, 
                                           int offset, 
                                           int len);

SRSLTE_API void srslte_scrambling_c(srslte_sequence_t *s, 
                                    cf_t *data);

SRSLTE_API void srslte_scrambling_c_offset(srslte_sequence_t *s, 
                                           cf_t *data, 
                                           int offset, 
                                           int len);

#endif // SCRAMBLING_
