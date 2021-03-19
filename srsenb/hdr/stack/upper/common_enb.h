/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSENB_COMMON_ENB_H
#define SRSENB_COMMON_ENB_H

/*******************************************************************************
                              INCLUDES
*******************************************************************************/

#include <stdint.h>

namespace srsenb {

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
