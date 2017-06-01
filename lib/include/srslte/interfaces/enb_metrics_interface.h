
#ifndef ENB_METRICS_INTERFACE_H
#define ENB_METRICS_INTERFACE_H

#include <stdint.h>

#include "upper/common_enb.h"
#include "upper/s1ap_metrics.h"
#include "upper/rrc_metrics.h"
#include "srslte/upper/gw_metrics.h"
#include "srslte/upper/rlc_metrics.h"
#include "mac/mac_metrics.h"
#include "phy/phy_metrics.h"

namespace srsenb {
  
typedef struct {
  uint32_t rf_o;
  uint32_t rf_u;
  uint32_t rf_l;
  bool     rf_error;
}rf_metrics_t;

typedef struct {
  rf_metrics_t    rf;
  phy_metrics_t   phy[ENB_METRICS_MAX_USERS];
  mac_metrics_t   mac[ENB_METRICS_MAX_USERS];
  rrc_metrics_t   rrc; 
  s1ap_metrics_t  s1ap;
  bool            running;
}enb_metrics_t;

// ENB interface
class enb_metrics_interface
{
public:
  virtual bool get_metrics(enb_metrics_t &m) = 0;
};

} // namespace srsenb

#endif // ENB_METRICS_INTERFACE_H
