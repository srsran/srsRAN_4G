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

#ifndef SRSENB_CBS_ENCODER_H
#define SRSENB_CBS_ENCODER_H

#include <cstdint>
#include <string>
#include <vector>

namespace srsenb {

/// \brief GSM-7 encoder for CBS messages.
///
/// This class implements ASCII to GSM-7 encoding using the default alphabet and the default alphabet extension tables
/// defined in TS23.038 Section 6.2.1, as well as UTF-8 to UCS-2 encoding as defined in TS23.038 Section 6.2.3. The
/// GSM-7 encoded messages are packed following the CBS packing format defined in TS23.038 Section 6.1.2.2.
class cbs_encoder
{
public:
  /// \brief Fills a CB-Data information element with the contents of an ETWS/CMAS warning message using GSM-7 encoding.
  ///
  /// Only printable ASCII characters in the default GSM-7 alphabet and its extension table are supported by this
  /// method.
  ///
  /// \param[out] cb_data         CB-Data IE in packed form.
  /// \param[in]  warning_message ETWS/CMAS warning message string.
  /// \return \c SRSRAN_SUCCESS if the encoding is successful, \c SRSRAN_ERROR otherwise.
  /// \remark An error is thrown if any character in the warning message is not present in the GSM-7 default alphabet
  ///         or is not a printable ASCII character.
  static unsigned fill_cb_data_gsm7(std::vector<uint8_t>& cb_data, const std::string& warning_message);

  /// \brief Fills a CB-Data information element with the contents of an ETWS/CMAS warning message using UCS-2 encoding.
  ///
  /// All unicode code points in the basic multilingual plane are supported by this method.
  ///
  /// \param[out] cb_data         CB-Data IE in packed form.
  /// \param[in]  warning_message ETWS/CMAS warning message string.
  /// \return \c SRSRAN_SUCCESS if the encoding is successful, \c SRSRAN_ERROR otherwise.
  /// \remark An error is thrown if any character in the warning message is not present in the basic multilingual
  ///         unicode plane.
  static unsigned fill_cb_data_ucs2(std::vector<uint8_t>& cb_data, const std::string& warning_message);

private:
  /// \brief Encodes an ASCII message into GSM-7 using the default alphabet and CBS packing of 7 bit characters.
  ///
  /// \param[out] out  The GSM-7 encoded and packed message.
  /// \param[in]  data Input data string.
  /// \remark An error message is thrown if any character in \c data is not present in the GSM-7 default alphabet and
  /// its extension table or is not a printable ASCII character.
  /// \retun \c True if the message was successfully encoded, \c false otherwise.
  static bool encode_gsm7(std::vector<uint8_t>& out, const std::string& data);

  /// \brief Encodes an UTF-8 message into UCS-2 as specified in TS23.038 Section 6.2.3.
  ///
  /// Some code points supported by UTF-8 cannot be encoded in UCS-2. Since UCS-2 encodes each code point in a fixed
  /// 16-bit value, code points outside the basic multilingual unicode plane are not supported by UCS-2. If the provided
  /// data string contains any of such characters, an error message is displayed and the string is not encoded.
  ///
  /// \param[out] out  The UCS-2 encoded and packed message. Each 16-bit code point is packed in big-endian order.
  /// \param[in]  data Input data string in UTF-8 format.
  /// \retun \c True if the message was successfully encoded, \c false otherwise.
  static bool encode_ucs2(std::vector<uint8_t>& out, const std::string& data);

  /// \brief Gets the number of 7-bit GSM-7 characters required to encode a message.
  ///
  /// The number of characters includes the escape characters required for accessing the extension tables. See TS23.038
  /// Section 6.2.1.2.2.
  ///
  /// \param[in] data Input data string.
  /// \return The number of required 7-bit GSM-7 characters.
  static unsigned get_nof_gsm7_chars(const std::string& data);

  /// CB Data IE Information Page field number of bytes. See TS23.041 Section 9.4.2.2.5.
  static constexpr unsigned INFO_PAGE_NBYTES = 82;
  /// CB Data IE Information Page Length field number of bytes.
  static constexpr unsigned INFO_LEN_NBYTES = 1;
  /// Maximum number of CB Data IE information pages.
  static constexpr unsigned MAX_NOF_CB_DATA_PAGES = 15;
  /// GSM-7 escape code.
  static constexpr uint8_t esc_code = 0x1B;
  /// GSM-7 carriage return (CR) code.
  static constexpr uint8_t cr_code = 0x0D;
};
} // namespace srsenb

#endif // SRSENB_CBS_ENCODER_H