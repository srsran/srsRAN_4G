// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

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
