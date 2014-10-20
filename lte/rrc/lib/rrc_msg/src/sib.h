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

#include <stdio.h>
#include <stdlib.h>

#include "liblte/rrc/rrc_msg/bcch.h"
#include "liblte/phy/utils/bit.h"
#include "rrc_asn.h"


void sib1_create_default(SystemInformationBlockType1_t *sib1, 
                            MCC_MNC_Digit_t mcc_val[3], 
                            MCC_MNC_Digit_t mnc_val[2], 
                            uint8_t tac_val[2],
                            uint8_t cid_val[4], 
                            int freq_band);
