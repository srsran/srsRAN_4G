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