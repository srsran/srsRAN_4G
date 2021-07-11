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