/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#ifndef SRSENB_CBS_ENCODING_HELPERS
#define SRSENB_CBS_ENCODING_HELPERS

#include <string>

namespace srsenb {

/// Supported CBS message encoding options.
enum class cbs_encodings {
  /// GSM-7 7-bit character encoding using the default alphabet and its extension table.
  GSM7,
  /// UCS-2 16-bit character encoding supporting all unicode code points in the basic multilingual plane.
  UCS2,
  /// Invalid or unsupported encoding formats.
  INVALID
};

/// Selects either GSM-7 or UCS-2 encoding based on the SIB-11 and SIB-12 \c dataCodingScheme field value.
cbs_encodings select_cbs_encoding(uint8_t data_coding_scheme)
{
  uint8_t coding_group = data_coding_scheme >> 4U;
  uint8_t bits3_0      = data_coding_scheme & 0x0F;
  switch (coding_group) {
    case 0x0:
      // Languages using the GSM-7 default alphabet.
      return cbs_encodings::GSM7;
    case 0x4:
      // General data coding indication, uncompressed and no message class meaning on bits 1..0.
    case 0x5: {
      // General data coding indication, uncompressed and message class meaning on bits 1..0.
      uint8_t coding_bits = bits3_0 >> 2U;
      // Bits 3..2 indicate the message encoding.
      if (coding_bits == 0U) {
        return cbs_encodings::GSM7;
      }
      if (coding_bits == 2U) {
        return cbs_encodings::UCS2;
      }
      return cbs_encodings::INVALID;
    }
    case 0xF:
      // Data coding / message handling.
      if (((bits3_0 >> 2U) & 1U) == 0U) {
        return cbs_encodings::GSM7;
      }
      return cbs_encodings::INVALID;
    default:
      // All unsupported or reserved coding groups.
      return cbs_encodings::INVALID;
  }
}

} // namespace srsenb

#endif // SRSENB_CBS_ENCODING_HELPERS
