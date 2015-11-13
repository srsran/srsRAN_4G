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
 *  File:         mod.h
 *
 *  Description:  Modulation.
 *                Supports BPSK, QPSK, 16QAM and 64QAM.
 *
 *  Reference:    3GPP TS 36.211 version 10.0.0 Release 10 Sec. 7.1
 *****************************************************************************/

#ifndef MOD_
#define MOD_

#include <complex.h>
#include <stdint.h>

#include "srslte/config.h"
#include "modem_table.h"

SRSLTE_API int srslte_mod_modulate(srslte_modem_table_t* table,
                                   uint8_t *bits, 
                                   cf_t* symbols, 
                                   uint32_t nbits);

SRSLTE_API int srslte_mod_modulate_bytes(srslte_modem_table_t* q, 
                                         uint8_t *bits, 
                                         cf_t* symbols, 
                                         uint32_t nbits); 

#endif // MOD_
