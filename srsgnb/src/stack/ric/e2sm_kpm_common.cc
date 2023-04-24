/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#include "srsgnb/hdr/stack/ric/e2sm_kpm_common.h"

std::string e2sm_kpm_label_2_str(e2sm_kpm_label_enum label)
{
  switch (label) {
    case NO_LABEL:
      return "NO_LABEL";
    case MIN_LABEL:
      return "MIN_LABEL";
    case MAX_LABEL:
      return "MAX_LABEL";
    case AVG_LABEL:
      return "AVG_LABEL";
    case SUM_LABEL:
      return "SUM_LABEL";
    default:
      return "UNKNOWN_LABEL";
  }
}