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


#ifndef SIB1_
#define SIB1_


#include <stdio.h>
#include <stdlib.h>

#include "liblte/rrc/common/rrc_common.h"
#include "liblte/rrc/messages/bcch.h"
#include "liblte/phy/utils/bit.h"
#include "rrc_asn.h"

LIBLTE_API uint32_t bcch_dlsch_sib1_get_freq_num(void *bcch_dlsch_msg); 

LIBLTE_API void bcch_dlsch_sib1_get_plmns(void *bcch_dlsch_msg, 
                                          plmn_identity_t *plmns, 
                                          uint32_t max_plmn_identities);

LIBLTE_API void bcch_dlsch_sib1_get_cell_access_info(void *bcch_dlsch_msg, 
                                                     cell_access_info_t *info); 

LIBLTE_API int bcch_dlsch_sib1_get_scheduling_info(void *bcch_dlsch_msg, 
                                                    uint32_t *si_window_length,
                                                    scheduling_info_t *info, 
                                                    uint32_t max_elems);

void bcch_dlsch_sib1(BCCH_DL_SCH_Message_t *sib1, 
                     MCC_MNC_Digit_t mcc_val[3], 
                     MCC_MNC_Digit_t mnc_val[2], 
                     uint8_t tac_val[2], 
                     uint8_t cid_val[4], 
                     int freq_band);

#endif