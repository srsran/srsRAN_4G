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


#ifndef SIB4_
#define SIB4_


#include <stdio.h>
#include <stdlib.h>

#include "liblte/rrc/common/rrc_common.h"
#include "liblte/rrc/messages/bcch.h"
#include "liblte/phy/utils/bit.h"
#include "rrc_asn.h"

LIBLTE_API int bcch_dlsch_sib4_get_neighbour_cells(void *bcch_dlsch_msg, 
                                                   uint32_t *neighbour_cell_ids, 
                                                   uint32_t max_elems);

#endif