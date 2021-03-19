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

#ifndef SRSRAN_NPDSCH_UE_HELPER_H
#define SRSRAN_NPDSCH_UE_HELPER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "srsran/phy/phch/ra_nbiot.h"
#include <stdint.h>

int get_sib2_params(const uint8_t* sib1_payload, const uint32_t len, srsran_nbiot_si_params_t* sib2_params);
int bcch_bch_to_pretty_string(const uint8_t* bcch_bch_payload,
                              const uint32_t input_len,
                              char*          output,
                              const uint32_t max_output_len);
int bcch_dl_sch_to_pretty_string(const uint8_t* bcch_dl_sch_payload,
                                 const uint32_t input_len,
                                 char*          output,
                                 const uint32_t max_output_len);

#ifdef __cplusplus
}
#endif

#endif // SRSRAN_NPDSCH_UE_HELPER_H
