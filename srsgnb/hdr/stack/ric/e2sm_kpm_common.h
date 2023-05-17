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

#include "srsran/asn1/e2ap.h"
#include "srsran/asn1/e2sm.h"
#include "srsran/asn1/e2sm_kpm_v2.h"
#include "srsran/srsran.h"

#ifndef SRSRAN_E2SM_KPM_COMMON_H
#define SRSRAN_E2SM_KPM_COMMON_H

using namespace asn1::e2ap;
using namespace asn1::e2sm_kpm;

enum e2_metric_data_type_t { INTEGER, REAL };

typedef struct {
  std::string           name;
  bool                  supported;
  e2_metric_data_type_t data_type;
  std::string           units;
  bool                  min_val_present;
  double                min_val;
  bool                  max_val_present;
  double                max_val;
  uint32_t              supported_labels;
  uint32_t              supported_scopes;
} e2sm_kpm_metric_t;

// TODO: define all labels and scopes

/* Labels supported for a metric */
enum e2sm_kpm_label_enum {
  NO_LABEL      = 0x0001,
  MIN_LABEL     = 0x0002,
  MAX_LABEL     = 0x0004,
  AVG_LABEL     = 0x0008,
  SUM_LABEL     = 0x0010,
  UNKNOWN_LABEL = 0x8000
};

std::string e2sm_kpm_label_2_str(e2sm_kpm_label_enum label);

/* Scopes supported for a metric */
enum e2sm_kpm_metric_scope_enum {
  ENB_LEVEL     = 0x0001,
  CELL_LEVEL    = 0x0002,
  UE_LEVEL      = 0x0004,
  BEARER_LEVEL  = 0x0008,
  UNKNOWN_LEVEL = 0xffff
};

typedef struct {
  std::string                name;
  e2sm_kpm_label_enum        label;
  e2sm_kpm_metric_scope_enum scope;
  meas_record_item_c::types  data_type;
  uint32_t                   ue_id;   // TODO: do we need to use type ueid_c? or we translate to local RNTI?
  uint32_t                   cell_id; // TODO: do we need to use type cgi_c? or we translate to local cell_id?
} e2sm_kpm_meas_def_t;

#endif // SRSRAN_E2SM_KPM_COMMON_H
