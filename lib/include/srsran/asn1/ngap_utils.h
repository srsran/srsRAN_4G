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

#ifndef SRSRAN_NGAP_UTILS_H
#define SRSRAN_NGAP_UTILS_H

#include "asn1_utils.h"
#include "ngap.h"
/************************
 * Forward declarations
 ***********************/

namespace asn1 {
namespace ngap {
struct rrcestablishment_cause_opts;
struct cause_radio_network_opts;
using rrcestablishment_cause_e = enumerated<rrcestablishment_cause_opts, true, 1>;
using cause_radio_network_e    = enumerated<cause_radio_network_opts, true, 2>;
} // namespace ngap
} // namespace asn1

#endif // SRSRAN_NGAP_UTILS_H