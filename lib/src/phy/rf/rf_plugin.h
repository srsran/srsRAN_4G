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
 */

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
