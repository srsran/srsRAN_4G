// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

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