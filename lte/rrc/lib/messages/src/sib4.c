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

#include "liblte/rrc/common/rrc_common.h"
#include "liblte/rrc/messages/bcch.h"
#include "liblte/rrc/messages/sib4.h"
#include "liblte/phy/utils/bit.h"
#include "rrc_asn.h"
#include <BCCH-DL-SCH-MessageType.h>


int bcch_dlsch_sib4_get_neighbour_cells(void *bcch_dlsch_msg, uint32_t *neighbour_cell_ids, uint32_t max_elems)
{
  int i = 0; 
  BCCH_DL_SCH_Message_t *msg = (BCCH_DL_SCH_Message_t*) bcch_dlsch_msg; 
  SystemInformationBlockType4_t *sib4 = 
  &(msg->message.choice.c1.choice.systemInformation.criticalExtensions.choice.systemInformation_r8.sib_TypeAndInfo.list.array[0]->choice.sib4);
 
  if (sib4->intraFreqNeighCellList) {
    for (i=0;i<sib4->intraFreqNeighCellList->list.count && i<max_elems;i++) {
      IntraFreqNeighCellInfo_t *cellInfo = sib4->intraFreqNeighCellList->list.array[i]; 
      neighbour_cell_ids[i] = cellInfo->physCellId;
    }    
  }
  
  return i; 
}


