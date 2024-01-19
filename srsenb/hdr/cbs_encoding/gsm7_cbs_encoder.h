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

#ifndef SRSENB_GSM_TEXT_ENC_H
#define SRSENB_GSM_TEXT_ENC_H

#include <string>
#include <vector>

namespace srsenb {

/// \brief GSM-7 encoder for CBS messages.
///
/// This class implements ASCII to GSM-7 encoding using the default alphabet and the default alphabet extension tables
/// defined in TS23.038 Section 6.2.1. The encoded messages are packed following the CBS packing format defined in
/// TS23.038 Section 6.1.2.2.
class gsm7_cbs_encoder
{
public:
  /// \brief Encodes an ASCII message into GSM-7 using the default alphabet and CBS packing of 7 bit characters.
  ///
  /// \param[out] out The GSM-7 encoded and packed message.
  /// \param [in] data Input data string.
  /// \remark An error message is thrown for each ASCII character in \c data that is not present in the GSM-7 default
  ///         alphabet and its extension table. These characters will be replaced by '@'.
  static void encode(std::vector<uint8_t>& out, const std::string& data);

  /// \brief Gets the number of 7-bit GSM-7 characters required to encode a message.
  ///
  /// The number of characters includes the escape characters required for accessing the extension tables. See TS23.038
  /// Section 6.2.1.2.2.
  ///
  /// \param[in] data Input data string.
  /// \return The number of required 7-bit GSM-7 characters.
  static unsigned get_encoded_message_nof_chars(const std::string& data);

  /// Description of a GSM-7 character (See TS23.038 Section 6.2.1).
  struct gsm7_char {
    /// Character code.
    uint8_t code;
    /// Indicates if the character is in the extension table.
    bool is_ext_table;
  };

private:
  /// GSM-7 escape code.
  static constexpr uint8_t esc_code = 0x1B;
  /// GSM-7 carriage return (CR) code.
  static constexpr uint8_t cr_code = 0x0D;
};
} // namespace srsenb

#endif // SRSENB_GSM_TEXT_ENC_H