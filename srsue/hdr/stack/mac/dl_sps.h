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

#ifndef SRSUE_DL_SPS_H
#define SRSUE_DL_SPS_H

#include "srsran/common/timers.h"

/* Downlink Semi-Persistent schedulign (Section 5.10.1) */

namespace srsue {

class dl_sps
{
public:
  void clear() {}
  void reset() {}
  bool get_pending_grant(uint32_t tti, mac_interface_phy_lte::mac_grant_dl_t* grant) { return false; }

private:
};

} // namespace srsue

#endif // SRSUE_DL_SPS_H
