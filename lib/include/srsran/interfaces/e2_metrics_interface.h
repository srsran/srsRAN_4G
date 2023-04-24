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
