/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 *
 */

#ifndef SRSRAN_E2SM_KPM_METRICS_H
#define SRSRAN_E2SM_KPM_METRICS_H

#include "srsran/srsran.h"

#define NO_LABEL 0x0001
#define MIN_LABEL 0x0002
#define MAX_LABEL 0x0004
#define AVG_LABEL 0x0008

enum e2_metric_data_type_t { INTEGER, REAL };

struct E2SM_KPM_metric_t {
  std::string           name;
  e2_metric_data_type_t data_type;
  std::string           units;
  bool                  min_val_present;
  double                min_val;
  bool                  max_val_present;
  double                max_val;
  uint32_t              supported_labels;
  bool                  supported;
};

// Measurements defined in 3GPP TS 28.552
std::vector<E2SM_KPM_metric_t> get_e2sm_kpm_28_552_metrics()
{
  // TODO: add all metrics from 3GPP TS 28.552
  std::vector<E2SM_KPM_metric_t> metrics;
  // supported metrics
  metrics.push_back({"RRU.PrbTotDl", REAL, "%", true, 0, true, 100, NO_LABEL | AVG_LABEL, true});
  metrics.push_back({"RRU.PrbTotUl", REAL, "%", true, 0, true, 100, NO_LABEL | AVG_LABEL, true});
  // not supported metrics
  metrics.push_back({"RRU.RachPreambleDedMean", REAL, "%", false, 0, false, 100, NO_LABEL, false});
  return metrics;
}

// Measurements defined in 3GPP TS 32.425
std::vector<E2SM_KPM_metric_t> get_e2sm_kpm_34_425_metrics()
{
  // TODO: add all metrics from 3GPP TS 32.425
  std::vector<E2SM_KPM_metric_t> metrics;
  return metrics;
}

// E2SM_KPM O-RAN specific Measurements
std::vector<E2SM_KPM_metric_t> e2sm_kpm_oran_metrics()
{
  // TODO: add all E2SM_KPM O-RAN specific Measurements
  std::vector<E2SM_KPM_metric_t> metrics;
  return metrics;
}

// Custom Measurements
std::vector<E2SM_KPM_metric_t> e2sm_kpm_custom_metrics()
{
  std::vector<E2SM_KPM_metric_t> metrics;
  // supported metrics
  metrics.push_back({"test", REAL, "", true, 0, true, 100, NO_LABEL, true});
  // not supported metrics
  metrics.push_back({"test123", REAL, "", true, 0, true, 100, NO_LABEL, false});
  return metrics;
}

#endif // SRSRAN_E2SM_KPM_METRICS_H
