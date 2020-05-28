/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
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

#ifndef SRSENB_COMMON_ENB_H
#define SRSENB_COMMON_ENB_H

/*******************************************************************************
                              INCLUDES
*******************************************************************************/

#include <stdint.h>

namespace srsenb {

#define ENB_METRICS_MAX_USERS 64
#define SRSENB_RRC_MAX_N_PLMN_IDENTITIES 6

#define SRSENB_N_SRB 3
#define SRSENB_N_DRB 8
#define SRSENB_N_RADIO_BEARERS 11

enum rb_id_t {
  RB_ID_SRB0 = 0,
  RB_ID_SRB1,
  RB_ID_SRB2,
  RB_ID_DRB1,
  RB_ID_DRB2,
  RB_ID_DRB3,
  RB_ID_DRB4,
  RB_ID_DRB5,
  RB_ID_DRB6,
  RB_ID_DRB7,
  RB_ID_DRB8,
  RB_ID_N_ITEMS,
};
inline const char* to_string(rb_id_t rb_id)
{
  const static char* names[] = {"SRB0", "SRB1", "SRB2", "DRB1", "DRB2", "DRB3", "DRB4", "DRB5", "DRB6", "DRB7", "DRB8"};
  return (rb_id < RB_ID_N_ITEMS) ? names[rb_id] : "invalid bearer id";
}

// Cat 3 UE - Max number of DL-SCH transport block bits received within a TTI
// 3GPP 36.306 Table 4.1.1
#define SRSENB_MAX_BUFFER_SIZE_BITS 102048
#define SRSENB_MAX_BUFFER_SIZE_BYTES 12756
#define SRSENB_BUFFER_HEADER_OFFSET 1024

} // namespace srsenb

#endif // SRSENB_COMMON_ENB_H
