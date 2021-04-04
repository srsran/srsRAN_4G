/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#ifndef SRSUE_MAC_COMMON_H
#define SRSUE_MAC_COMMON_H

/**
 * @brief Common definitions/interfaces between LTE/NR MAC components
 *
 * @remark: So far only the trigger types are identical. The BSR report type and LCID mapping is implemented in RAT
 * specialications.
 */
namespace srsue {

// BSR trigger are common between LTE and NR
typedef enum { NONE, REGULAR, PADDING, PERIODIC } bsr_trigger_type_t;
char* bsr_trigger_type_tostring(bsr_trigger_type_t type);

} // namespace srsue

#endif // SRSUE_MAC_COMMON_H
