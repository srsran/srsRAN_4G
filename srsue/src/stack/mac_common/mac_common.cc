/**
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "srsue/hdr/stack/mac_common/mac_common.h"

namespace srsue {

char* bsr_trigger_type_tostring(bsr_trigger_type_t type)
{
  switch (type) {
    case bsr_trigger_type_t::NONE:
      return (char*)"none";
    case bsr_trigger_type_t::REGULAR:
      return (char*)"Regular";
    case bsr_trigger_type_t::PADDING:
      return (char*)"Padding";
    case bsr_trigger_type_t::PERIODIC:
      return (char*)"Periodic";
  }
  return (char*)"unknown";
}

} // namespace srsue
