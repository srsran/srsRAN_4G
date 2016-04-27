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

/**********************************************************************************************
 *  File:         rm_conv.h
 *
 *  Description:  Rate matching for convolutionally coded transport channels and control
 *                information.
 *
 *  Reference:    3GPP TS 36.212 version 10.0.0 Release 10 Sec. 5.1.4.2
 *********************************************************************************************/

#ifndef RM_CONV_
#define RM_CONV_

#include "srslte/config.h"

#ifndef SRSLTE_RX_NULL
#define SRSLTE_RX_NULL 10000
#endif

#ifndef SRSLTE_TX_NULL
#define SRSLTE_TX_NULL 100
#endif
SRSLTE_API int srslte_rm_conv_tx(uint8_t *input, 
                                 uint32_t in_len, 
                                 uint8_t *output, 
                                 uint32_t out_len);

SRSLTE_API int srslte_rm_conv_rx(float *input, 
                                 uint32_t in_len, 
                                 float *output, 
                                 uint32_t out_len);


/************* FIX THIS. MOVE ALL PROCESSING TO INT16 AND HAVE ONLY 1 IMPLEMENTATION ******/ 

SRSLTE_API int srslte_rm_conv_rx_s(int16_t *input, 
                                   uint32_t in_len, 
                                   int16_t *output, 
                                   uint32_t out_len);

#endif
