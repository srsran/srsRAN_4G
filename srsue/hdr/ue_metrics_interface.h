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

#ifndef SRSUE_UE_METRICS_INTERFACE_H
#define SRSUE_UE_METRICS_INTERFACE_H

#include <stdint.h>

#include "phy/phy_metrics.h"
#include "srsran/common/metrics_hub.h"
#include "srsran/radio/radio_metrics.h"
#include "srsran/rlc/rlc_metrics.h"
#include "srsran/system/sys_metrics.h"
#include "stack/mac/mac_metrics.h"
#include "stack/rrc/rrc_metrics.h"
#include "stack/upper/gw_metrics.h"
#include "stack/upper/nas_metrics.h"

namespace srsue {

typedef struct {
  uint32_t              ul_dropped_sdus;
  mac_metrics_t         mac[SRSRAN_MAX_CARRIERS];
  mac_metrics_t         mac_nr[SRSRAN_MAX_CARRIERS];
  srsran::rlc_metrics_t rlc;
  nas_metrics_t         nas;
  rrc_metrics_t         rrc;
  rrc_metrics_t         rrc_nr;
} stack_metrics_t;

typedef struct {
  srsran::rf_metrics_t  rf;
  phy_metrics_t         phy;
  phy_metrics_t         phy_nr;
  gw_metrics_t          gw;
  stack_metrics_t       stack;
  srsran::sys_metrics_t sys;
} ue_metrics_t;

// UE interface
class ue_metrics_interface : public srsran::metrics_interface<ue_metrics_t>
{
public:
  virtual bool get_metrics(ue_metrics_t* m) = 0;
};

} // namespace srsue

#endif // SRSUE_UE_METRICS_INTERFACE_H
