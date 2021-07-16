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

#ifndef SRSRAN_ENB_METRICS_INTERFACE_H
#define SRSRAN_ENB_METRICS_INTERFACE_H

#include <stdint.h>

#include "srsenb/hdr/common/common_enb.h"
#include "srsenb/hdr/phy/phy_metrics.h"
#include "srsenb/hdr/stack/mac/common/mac_metrics.h"
#include "srsenb/hdr/stack/rrc/rrc_metrics.h"
#include "srsenb/hdr/stack/s1ap/s1ap_metrics.h"
#include "srsran/common/metrics_hub.h"
#include "srsran/radio/radio_metrics.h"
#include "srsran/rlc/rlc_metrics.h"
#include "srsran/system/sys_metrics.h"
#include "srsran/upper/pdcp_metrics.h"
#include "srsue/hdr/stack/upper/gw_metrics.h"

namespace srsenb {

struct rlc_metrics_t {
  std::vector<srsran::rlc_metrics_t> ues;
};

struct pdcp_metrics_t {
  std::vector<srsran::pdcp_metrics_t> ues;
};

struct stack_metrics_t {
  mac_metrics_t  mac;
  rrc_metrics_t  rrc;
  rlc_metrics_t  rlc;
  pdcp_metrics_t pdcp;
  s1ap_metrics_t s1ap;
};

struct enb_metrics_t {
  srsran::rf_metrics_t       rf;
  std::vector<phy_metrics_t> phy;
  stack_metrics_t            stack;
  srsran::sys_metrics_t      sys;
  bool                       running;
};

// ENB interface
class enb_metrics_interface : public srsran::metrics_interface<enb_metrics_t>
{
public:
  virtual bool get_metrics(enb_metrics_t* m) = 0;
};

} // namespace srsenb

#endif // SRSRAN_ENB_METRICS_INTERFACE_H
