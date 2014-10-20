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



#ifndef BCCH_
#define BCCH_

#include "liblte/config.h"
#include "liblte/phy/common/phy_common.h"

#include <stdint.h>

LIBLTE_API int bcch_bch_mib_pack(lte_cell_t *cell, 
                                 uint32_t sfn, 
                                 uint8_t *buffer, 
                                 uint32_t buffer_size_bytes);

LIBLTE_API int bcch_bch_mib_unpack(uint8_t *buffer, 
                                   uint32_t msg_nof_bits, 
                                   lte_cell_t *cell, 
                                   uint32_t *sfn);

LIBLTE_API int bcch_dlsch_sib1_pack(uint8_t *buffer, 
                                    uint32_t buffer_size_bytes); 

LIBLTE_API int bcch_dlsch_sib1_unpack(uint8_t *buffer, 
                                      uint32_t msg_nof_bits);
#endif // AGC_
