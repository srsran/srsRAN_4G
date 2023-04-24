/**
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSRAN_E2SM_KPM_METRICS_H
#define SRSRAN_E2SM_KPM_METRICS_H

#include "e2sm_kpm_common.h"
#include "srsran/srsran.h"

// clang-format off
// Measurements defined in 3GPP TS 28.552
std::vector<e2sm_kpm_metric_t> get_e2sm_kpm_28_552_metrics()
{
  // TODO: add all metrics from 3GPP TS 28.552
  std::vector<e2sm_kpm_metric_t> metrics;
  // not supported metrics
  metrics.push_back({"RRU.PrbTotDl", false, REAL, "%", true, 0, true, 100, NO_LABEL | AVG_LABEL, CELL_LEVEL | UE_LEVEL });
  metrics.push_back({"RRU.PrbTotUl", false, REAL, "%", true, 0, true, 100, NO_LABEL | AVG_LABEL, CELL_LEVEL | UE_LEVEL });
  // not supported metrics
  metrics.push_back({"RRU.RachPreambleDedMean", false, REAL, "-", false, 0, false, 100, NO_LABEL, CELL_LEVEL | UE_LEVEL });
  return metrics;
}

// Measurements defined in 3GPP TS 32.425
std::vector<e2sm_kpm_metric_t> get_e2sm_kpm_34_425_metrics()
{
  // TODO: add all metrics from 3GPP TS 32.425
  std::vector<e2sm_kpm_metric_t> metrics;
  return metrics;
}

// E2SM_KPM O-RAN specific Measurements
std::vector<e2sm_kpm_metric_t> e2sm_kpm_oran_metrics()
{
  // TODO: add all E2SM_KPM O-RAN specific Measurements
  std::vector<e2sm_kpm_metric_t> metrics;
  return metrics;
}

// Custom Measurements
std::vector<e2sm_kpm_metric_t> e2sm_kpm_custom_metrics()
{
  std::vector<e2sm_kpm_metric_t> metrics;
  // supported metrics
  metrics.push_back({"test", true, INTEGER, "", true, 0, true, 100, NO_LABEL, ENB_LEVEL | CELL_LEVEL | UE_LEVEL });
  metrics.push_back({"random_int", true, INTEGER, "", true, 0, true, 100, NO_LABEL, CELL_LEVEL });
  metrics.push_back({"cpu0_load", true, REAL, "", true, 0, true, 100, NO_LABEL, ENB_LEVEL });
  metrics.push_back({"cpu_load", true, REAL, "", true, 0, true, 100, MIN_LABEL|MAX_LABEL|AVG_LABEL, ENB_LEVEL });
  // not supported metrics
  metrics.push_back({"test123", false,  REAL, "", true, 0, true, 100, NO_LABEL, CELL_LEVEL | UE_LEVEL });
  return metrics;
}

// clang-format on
#endif // SRSRAN_E2SM_KPM_METRICS_H
