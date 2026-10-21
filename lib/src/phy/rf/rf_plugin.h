// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

#ifndef SRSRAN_RF_PLUGIN_H
#define SRSRAN_RF_PLUGIN_H

#include "srsran/phy/rf/rf.h"

#ifdef __cplusplus
extern "C" {
#endif

SRSRAN_API int register_plugin(rf_dev_t** rf_api);

#ifdef __cplusplus
}
#endif

#endif /* SRSRAN_RF_PLUGIN_H */
