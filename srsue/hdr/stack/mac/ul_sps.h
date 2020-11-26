/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSUE_UL_SPS_H
#define SRSUE_UL_SPS_H

#include "srslte/common/log.h"
#include "srslte/common/timers.h"

/* Uplink Semi-Persistent schedulign (Section 5.10.2) */

namespace srsue {

typedef _Complex float cf_t;

class ul_sps
{
public:
  void clear() {}
  void reset(uint32_t tti) {}
  bool get_pending_grant(uint32_t tti, mac_interface_phy_lte::mac_grant_ul_t* grant) { return false; }

private:
};

} // namespace srsue

#endif // SRSUE_UL_SPS_H
