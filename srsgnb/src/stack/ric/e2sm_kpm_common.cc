/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
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