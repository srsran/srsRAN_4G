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
