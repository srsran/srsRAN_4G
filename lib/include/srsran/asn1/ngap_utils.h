// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

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