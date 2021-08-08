/**
 * Copyright 2013-2021 Software Radio Systems Limited
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
#ifndef SRSENB_NGAP_UE_UTILS_H
#define SRSENB_NGAP_UE_UTILS_H

#include "srsenb/hdr/common/common_enb.h"
#include "srsran/adt/optional.h"
#include "srsran/common/common.h"
#include "srsran/phy/common/phy_common.h"

namespace srsenb {

struct ngap_ue_ctxt_t {
  static const uint32_t invalid_gnb_id = std::numeric_limits<uint32_t>::max();

  uint16_t                   rnti           = SRSRAN_INVALID_RNTI;
  uint32_t                   ran_ue_ngap_id = invalid_gnb_id;
  srsran::optional<uint32_t> amf_ue_ngap_id;
  uint32_t                   gnb_cc_idx     = 0;
  struct timeval             init_timestamp = {};

  // AMF identifier
  uint16_t amf_set_id;
  uint8_t  amf_pointer;
  uint8_t  amf_region_id;
};

} // namespace srsenb
#endif