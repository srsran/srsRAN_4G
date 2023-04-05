/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
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
} E2SM_KPM_metric_t;

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
} E2SM_KPM_meas_def_t;

#endif // SRSRAN_E2SM_KPM_COMMON_H
