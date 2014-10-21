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
 
 
#ifndef RRC_COMMON_
#define RRC_COMMON_

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  uint32_t mcc; // 3 digits in Decimal value
  uint32_t mnc; // 2 digits
} plmn_identity_t;

typedef struct {
  plmn_identity_t plmn;
  uint32_t tracking_area_code; // 8 bits 
  uint32_t cell_id; // 24 bits  
  bool cellBarred; 
  bool intraFreqReselection;
} cell_access_info_t;

#endif
