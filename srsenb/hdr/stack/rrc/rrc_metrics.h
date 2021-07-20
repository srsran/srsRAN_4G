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

#ifndef SRSENB_RRC_METRICS_H
#define SRSENB_RRC_METRICS_H

#include "srsenb/hdr/common/common_enb.h"
#include <vector>

namespace srsenb {

typedef enum {
  RRC_STATE_IDLE = 0,
  RRC_STATE_WAIT_FOR_CON_SETUP_COMPLETE,
  RRC_STATE_WAIT_FOR_CON_REEST_COMPLETE,
  RRC_STATE_WAIT_FOR_SECURITY_MODE_COMPLETE,
  RRC_STATE_WAIT_FOR_UE_CAP_INFO,
  RRC_STATE_WAIT_FOR_UE_CAP_INFO_ENDC, /* only entered for UEs with NSA support */
  RRC_STATE_WAIT_FOR_CON_RECONF_COMPLETE,
  RRC_STATE_REESTABLISHMENT_COMPLETE,
  RRC_STATE_REGISTERED,
  RRC_STATE_RELEASE_REQUEST,
  RRC_STATE_N_ITEMS,
} rrc_state_t;

struct rrc_ue_metrics_t {
  rrc_state_t                                 state;
  std::vector<std::pair<uint32_t, uint32_t> > drb_qci_map;
};

struct rrc_metrics_t {
  std::vector<rrc_ue_metrics_t> ues;
};

} // namespace srsenb

#endif // SRSENB_RRC_METRICS_H
