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

#define SI_PERIODS  10
#define SI_X_PERIOD 10

typedef enum {BCCH_DLSCH_SIB1, BCCH_DLSCH_SIB2, BCCH_DLSCH_SIB3, BCCH_DLSCH_SIB6, BCCH_DLSCH_UNKNOWN} bcch_dlsch_sib_type_t; 

typedef struct {
  bcch_dlsch_sib_type_t type[SI_X_PERIOD]; 
  uint32_t period; 
} bcch_si_scheduling_info_t;

typedef struct {
  uint32_t window_length_ms; 
  uint32_t nof_periods; 
  bcch_si_scheduling_info_t si_period_list[SI_PERIODS]; 
} bcch_si_scheduling_t; 


LIBLTE_API int bcch_bch_pack(lte_cell_t *cell, 
                                 uint32_t sfn, 
                                 uint8_t *buffer, 
                                 uint32_t buffer_size_bytes);

LIBLTE_API int bcch_bch_unpack(uint8_t *buffer, 
                                   uint32_t msg_nof_bits, 
                                   lte_cell_t *cell, 
                                   uint32_t *sfn);

LIBLTE_API int bcch_dlsch_pack(void *bcch_dlsch_msg, 
                               uint8_t *buffer, 
                               uint32_t buffer_size_bytes); 

LIBLTE_API void* bcch_dlsch_unpack(uint8_t *buffer, 
                                   uint32_t msg_nof_bits);

LIBLTE_API bcch_dlsch_sib_type_t bcch_dlsch_get_type(void *bcch_dlsch_msg);

LIBLTE_API void bcch_dlsch_fprint(void *bcch_dlsch_msg, 
                                  FILE *stream);

LIBLTE_API void bcch_dlsch_free(void *bcch_dlsch_msg); 

#endif // BCCH_
