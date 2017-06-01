/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2016 Software Radio Systems Limited
 *
 */

#ifndef ENB_S1AP_METRICS_H
#define ENB_S1AP_METRICS_H


namespace srsenb {

typedef enum{
  S1AP_ATTACHING = 0, // Attempting to create S1 connection
  S1AP_READY,         // S1 connected
  S1AP_ERROR          // Failure
}S1AP_STATUS_ENUM;

struct s1ap_metrics_t
{
  S1AP_STATUS_ENUM status;
};

} // namespace srsenb

#endif // ENB_S1AP_METRICS_H
