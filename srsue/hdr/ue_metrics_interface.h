/*
 * Copyright 2013-2020 Software Radio Systems Limited
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

#ifndef SRSUE_UE_METRICS_INTERFACE_H
#define SRSUE_UE_METRICS_INTERFACE_H

#include <stdint.h>

#include "phy/phy_metrics.h"
#include "srslte/common/metrics_hub.h"
#include "srslte/radio/radio_metrics.h"
#include "srslte/upper/rlc_metrics.h"
#include "stack/mac/mac_metrics.h"
#include "stack/rrc/rrc_metrics.h"
#include "stack/upper/gw_metrics.h"
#include "stack/upper/nas_metrics.h"

namespace srsue {

typedef struct {
  mac_metrics_t         mac[SRSLTE_MAX_CARRIERS];
  srslte::rlc_metrics_t rlc;
  nas_metrics_t         nas;
  rrc_metrics_t         rrc;
} stack_metrics_t;

typedef struct {
  srslte::rf_metrics_t rf;
  phy_metrics_t        phy;
  gw_metrics_t         gw;
  stack_metrics_t      stack;
} ue_metrics_t;

// UE interface
class ue_metrics_interface : public srslte::metrics_interface<ue_metrics_t>
{
public:
  virtual bool get_metrics(ue_metrics_t* m) = 0;
};

} // namespace srsue

#endif // SRSUE_UE_METRICS_INTERFACE_H
