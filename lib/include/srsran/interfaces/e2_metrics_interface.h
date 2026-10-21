// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

#include "srsgnb/hdr/stack/ric/e2sm.h"
#include "srsran/common/common.h"
#include "srsran/common/interfaces_common.h"
#include "srsran/interfaces/enb_metrics_interface.h"
#include "srsran/srsran.h"

#ifndef SRSRAN_E2_INTERFACES_H
#define SRSRAN_E2_INTERFACES_H

namespace srsenb {

class e2_interface_metrics
{
public:
  virtual bool pull_metrics(enb_metrics_t* m) = 0;

  virtual bool register_e2sm(e2sm* sm)   = 0;
  virtual bool unregister_e2sm(e2sm* sm) = 0;
};

} // namespace srsenb

#endif // SRSRAN_ENB_INTERFACES_H
