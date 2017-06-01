/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2016 Software Radio Systems Limited
 *
 */

#ifndef ENB_RRC_METRICS_H
#define ENB_RRC_METRICS_H

#include "upper/common_enb.h"

namespace srsenb {

typedef enum{
  RRC_STATE_IDLE = 0,
  RRC_STATE_WAIT_FOR_CON_SETUP_COMPLETE,
  RRC_STATE_WAIT_FOR_SECURITY_MODE_COMPLETE,
  RRC_STATE_WAIT_FOR_UE_CAP_INFO,
  RRC_STATE_WAIT_FOR_CON_RECONF_COMPLETE,
  RRC_STATE_REGISTERED,
  RRC_STATE_RELEASE_REQUEST,
  RRC_STATE_N_ITEMS,
}rrc_state_t;

struct rrc_ue_metrics_t
{
  rrc_state_t state;
};

struct rrc_metrics_t
{
  uint16_t n_ues;
  rrc_ue_metrics_t ues[ENB_METRICS_MAX_USERS];
};

} // namespace srsenb

#endif // ENB_S1AP_METRICS_H
