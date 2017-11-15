/**
 *
 * \section COPYRIGHT
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
#include <stdint.h>

namespace srslte{

/****************************************************************************
 * GTP-C Header
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
  enum gtpc_version version;
  bool piggyback;
  bool tied_present;
  enum gtpc_msg_type msg_type;
  uint64_t teid;
  uint64_t sequence;
} gtpc_header_t;

};
