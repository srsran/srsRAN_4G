/* \section COPYRIGHT
 *
 * Copyright 2013-2017 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of srsLTE.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */
#ifndef SRSLTE_GTPC_H
#define SRSLTE_GTPC_H

#include <stdint.h>
#include "srslte/asn1/gtpc_msg.h"


namespace srslte{

/*GTP-C Version*/
const uint8_t GTPC_V2 = 2;

/****************************************************************************
 * GTP-C v2 Header
 * Ref: 3GPP TS 29.274 v10.14.0 Section 5
 *
 *        | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 |
 *
 * 1      |  Version  | P | T | S | S | S |
 * 2      |           Message Type        |
 * 3      |         Length (1st Octet)    |
 * 4      |         Length (2nd Octet)    |
 * m      |   If T=1, TEID (1st Octet)    |
 * m+1    |   If T=1, TEID (2nd Octet)    |
 * m+2    |   If T=1, TEID (3st Octet)    |
 * m+3    |   If T=1, TEID (4st Octet)    |
 * n      |           Sequence            |
 * n+1    |           Sequence            |
 * n+2    |           Sequence            |
 * n+3    |            Spare              |
 ***************************************************************************/
typedef struct gtpc_header
{
  uint8_t version;
  bool piggyback;
  bool teid_present;
  uint8_t type;
  uint64_t teid;
  uint64_t sequence;
}gtpc_header_t;

/****************************************************************************
 * GTP-C v2 Payload
 * Ref: 3GPP TS 29.274 v10.14.0 Section 5
 *
 * Union that hold the different structures for the possible message types.
 ***************************************************************************/
typedef union gtpc_msg_choice
{
  struct gtpc_create_session_request create_session_request;
  struct gtpc_create_session_response create_session_response;
  struct gtpc_modify_bearer_request modify_bearer_request;
  struct gtpc_modify_bearer_response modify_bearer_response;
  struct gtpc_release_access_bearers_request release_access_bearers_request;
  struct gtpc_release_access_bearers_response release_access_bearers_response;
  struct gtpc_delete_session_request delete_session_request;
  struct gtpc_delete_session_response delete_session_response;
}gtpc_msg_choice_t;

/****************************************************************************
 * GTP-C v2 Message
 * Ref: 3GPP TS 29.274 v10.14.0
 *
 * This is the main structure to represent a GTP-C message. It is composed
 * of one GTP-C header and one union of structures, which can hold
 * all the possible GTP-C messages
 ***************************************************************************/
typedef struct gtpc_pdu
{
  struct gtpc_header header;
  union gtpc_msg_choice choice;
}gtpc_pdu_t;
}//namespace
#endif // SRSLTE_GTPC_H
