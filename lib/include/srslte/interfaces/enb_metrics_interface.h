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

#ifndef SRSLTE_ENB_METRICS_INTERFACE_H
#define SRSLTE_ENB_METRICS_INTERFACE_H

#include <stdint.h>

#include "srsenb/hdr/phy/phy_metrics.h"
#include "srsenb/hdr/stack/mac/mac_metrics.h"
#include "srsenb/hdr/stack/rrc/rrc_metrics.h"
#include "srsenb/hdr/stack/upper/common_enb.h"
#include "srsenb/hdr/stack/upper/s1ap_metrics.h"
#include "srslte/common/metrics_hub.h"
#include "srslte/radio/radio_metrics.h"
#include "srslte/upper/rlc_metrics.h"
#include "srsue/hdr/stack/upper/gw_metrics.h"

namespace srsenb {

//:TODO: move this struct to the layer it belongs to.
struct bearer_container_t {
  uint32_t id;
  uint32_t qci;
};
using bearer_list_t = std::vector<bearer_container_t>;

struct stack_metrics_t {
  std::vector<mac_metrics_t> mac;
  // For XICO: Each position in the vector represents a ue. Should have the same number of ues and ue order as the mac vector
  // above.
  std::vector<bearer_list_t> bearers;
  rrc_metrics_t              rrc;
  s1ap_metrics_t             s1ap;
};

struct enb_metrics_t {
  srslte::rf_metrics_t       rf;
  std::vector<phy_metrics_t> phy;
  stack_metrics_t            stack;
  bool                       running;
};

// ENB interface
class enb_metrics_interface : public srslte::metrics_interface<enb_metrics_t>
{
public:
  virtual bool get_metrics(enb_metrics_t* m) = 0;
};

} // namespace srsenb

#endif // SRSLTE_ENB_METRICS_INTERFACE_H
