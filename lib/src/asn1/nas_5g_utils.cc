// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

#include "srsran/asn1/nas_5g_utils.h"

#include "srsran/asn1/asn1_utils.h"
#include "srsran/common/buffer_pool.h"
#include "srsran/common/common.h"
#include "srsran/config.h"

#include <array>
#include <stdint.h>
#include <vector>

namespace srsran {
namespace nas_5g {

SRSASN_CODE unpack_mcc_mnc(uint8_t* mcc_bytes, uint8_t* mnc_bytes, asn1::cbit_ref& bref)
{
  // MCC digit 2 | MCC digit 1 | octet 5
  // MNC digit 3 | MCC digit 3 | octet 6
  // MNC digit 2 | MNC digit 1 | octet 7
  HANDLE_CODE(bref.unpack(mcc_bytes[1], 4));
  HANDLE_CODE(bref.unpack(mcc_bytes[0], 4));
  HANDLE_CODE(bref.unpack(mnc_bytes[2], 4));
  HANDLE_CODE(bref.unpack(mcc_bytes[2], 4));
  HANDLE_CODE(bref.unpack(mnc_bytes[1], 4));
  HANDLE_CODE(bref.unpack(mnc_bytes[0], 4));
  return SRSASN_SUCCESS;
}

SRSASN_CODE pack_mcc_mnc(uint8_t* mcc_bytes, uint8_t* mnc_bytes, asn1::bit_ref& bref)
{
  // MCC digit 2 | MCC digit 1 | octet 5
  // MNC digit 3 | MCC digit 3 | octet 6
  // MNC digit 2 | MNC digit 1 | octet 7
  HANDLE_CODE(bref.pack(mcc_bytes[1], 4));
  HANDLE_CODE(bref.pack(mcc_bytes[0], 4));
  HANDLE_CODE(bref.pack(mnc_bytes[2], 4));
  HANDLE_CODE(bref.pack(mcc_bytes[2], 4));
  HANDLE_CODE(bref.pack(mnc_bytes[1], 4));
  HANDLE_CODE(bref.pack(mnc_bytes[0], 4));
  return SRSASN_SUCCESS;
}

} // namespace nas_5g
} // namespace srsran