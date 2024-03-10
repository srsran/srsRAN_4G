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

#include "srsenb/hdr/cbs_encoding/cbs_encoder.h"
#include "srsran/adt/bounded_bitset.h"
#include "srsran/phy/utils/debug.h"
#include <codecvt>
#include <locale>
#include <map>

using namespace srsenb;

/// Description of a GSM-7 character (See TS23.038 Section 6.2.1).
struct gsm7_char {
  /// Character code.
  uint8_t code;
  /// Indicates if the character is in the extension table.
  bool is_ext_table;
};

/// GSM-7 default alphabet map, including the default extension table.
static const std::map<char, gsm7_char> gsm7_default_alphabet_map = {
    {'@', {0x00, false}},  {'$', {0x02, false}}, {'\n', {0x0A, false}}, {'_', {0x11, false}}, {' ', {0x20, false}},
    {'!', {0x21, false}},  {'"', {0x22, false}}, {'#', {0x23, false}},  {'%', {0x25, false}}, {'&', {0x26, false}},
    {'\'', {0x27, false}}, {'(', {0x28, false}}, {')', {0x29, false}},  {'*', {0x2A, false}}, {'+', {0x2B, false}},
    {',', {0x2C, false}},  {'-', {0x2D, false}}, {'.', {0x2E, false}},  {'/', {0x2F, false}}, {'0', {0x30, false}},
    {'1', {0x31, false}},  {'2', {0x32, false}}, {'3', {0x33, false}},  {'4', {0x34, false}}, {'5', {0x35, false}},
    {'6', {0x36, false}},  {'7', {0x37, false}}, {'8', {0x38, false}},  {'9', {0x39, false}}, {':', {0x3A, false}},
    {';', {0x3B, false}},  {'<', {0x3C, false}}, {'=', {0x3D, false}},  {'>', {0x3E, false}}, {'?', {0x3F, false}},
    {'A', {0x41, false}},  {'B', {0x42, false}}, {'C', {0x43, false}},  {'D', {0x44, false}}, {'E', {0x45, false}},
    {'F', {0x46, false}},  {'G', {0x47, false}}, {'H', {0x48, false}},  {'I', {0x49, false}}, {'J', {0x4A, false}},
    {'K', {0x4B, false}},  {'L', {0x4C, false}}, {'M', {0x4D, false}},  {'N', {0x4E, false}}, {'O', {0x4F, false}},
    {'P', {0x50, false}},  {'Q', {0x51, false}}, {'R', {0x52, false}},  {'S', {0x53, false}}, {'T', {0x54, false}},
    {'U', {0x55, false}},  {'V', {0x56, false}}, {'W', {0x57, false}},  {'X', {0x58, false}}, {'Y', {0x59, false}},
    {'Z', {0x5A, false}},  {'?', {0x60, false}}, {'a', {0x61, false}},  {'b', {0x62, false}}, {'c', {0x63, false}},
    {'d', {0x64, false}},  {'e', {0x65, false}}, {'f', {0x66, false}},  {'g', {0x67, false}}, {'h', {0x68, false}},
    {'i', {0x69, false}},  {'j', {0x6A, false}}, {'k', {0x6B, false}},  {'l', {0x6C, false}}, {'m', {0x6D, false}},
    {'n', {0x6E, false}},  {'o', {0x6F, false}}, {'p', {0x70, false}},  {'q', {0x71, false}}, {'r', {0x72, false}},
    {'s', {0x73, false}},  {'t', {0x74, false}}, {'u', {0x75, false}},  {'v', {0x76, false}}, {'w', {0x77, false}},
    {'x', {0x78, false}},  {'y', {0x79, false}}, {'z', {0x7A, false}},  {'^', {0x14, true}},  {'{', {0x28, true}},
    {'}', {0x29, true}},   {'\\', {0x2F, true}}, {'[', {0x3C, true}},   {'~', {0x3D, true}},  {']', {0x3E, true}},
    {'|', {0x40, true}}};

bool cbs_encoder::encode_gsm7(std::vector<uint8_t>& out, const std::string& data)
{
  unsigned bit_count = 0;
  unsigned bit_queue = 0;

  for (unsigned i_char = 0, i_char_end = data.length(); i_char != i_char_end; ++i_char) {
    // Convert from ASCII to GSM-7 using the general language table.
    std::map<char, gsm7_char>::const_iterator gsm_char = gsm7_default_alphabet_map.find(data[i_char]);

    if (gsm_char == gsm7_default_alphabet_map.end()) {
      fprintf(stderr,
              "Character '%c' in message not found in GSM-7 default alphabet. Message will not be encoded.\n",
              data[i_char]);
      return false;
    }

    // If the character is in the extension table, append the escape character.
    if (gsm_char->second.is_ext_table) {
      bit_queue |= (esc_code & 0x7FU) << bit_count;
      bit_count += 7;
    }

    // Add the GSM-7 character to the queue.
    bit_queue |= (gsm_char->second.code & 0x7FU) << bit_count;
    bit_count += 7;

    // Write full octets to the output.
    while (bit_count >= 8) {
      out.push_back(static_cast<uint8_t>(bit_queue));
      bit_count -= 8;
      bit_queue >>= 8U;
    }
  }

  // If there are 7 spare bits at the last octet, the receiver may confuse these for the @ character, therefore a
  // carriage return (CR) is appended. See TS23.038 Section 6.1.2.
  if (bit_count == 1) {
    bit_queue |= (cr_code & 0x7FU) << 1U;
  }

  // Add the trailing bits to the output if there are any.
  if (bit_count > 0) {
    out.push_back(static_cast<uint8_t>(bit_queue));
  }

  return true;
}

unsigned cbs_encoder::get_nof_gsm7_chars(const std::string& data)
{
  unsigned nof_chars = 0;
  for (unsigned i_char = 0, i_char_end = data.length(); i_char != i_char_end; ++i_char) {
    // Each ASCII character is converted into a GSM-7 character.
    ++nof_chars;

    gsm7_char gsm_char = gsm7_default_alphabet_map.find(data[i_char])->second;

    // If the character is in the extension table, an additional escape character is appended.
    if (gsm_char.is_ext_table) {
      ++nof_chars;
    }
  }

  return nof_chars;
}

bool cbs_encoder::encode_ucs2(std::vector<uint8_t>& out, const std::string& data)
{
  // Convert from UTF-8 (8bit string variable length encoding) to UCS-2 (16 bit string fixed length encoding).
  std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> ucs2_converter;
  std::u16string                                              ucs2_data;

  // Attempt conversion to UCS-2. If any character requires more than two bytes, handle the exception.
  try {
    ucs2_data = ucs2_converter.from_bytes(data);
  } catch (std::range_error& e) {
    fprintf(stderr, "One or more characters are not UCS-2 supported code points. Message will not be encoded.\n");
    return false;
  }

  // Number of encoded UCS-2 bytes.
  size_t nof_out_bytes = ucs2_data.size() * sizeof(char16_t);

  // Reserve memory in output vector.
  out.reserve(nof_out_bytes);

  // Add the UCS-2 characters into the vector, in big-endian order (MSB first).
  for (auto& ucs2_char : ucs2_data) {
    out.push_back(reinterpret_cast<char*>(&ucs2_char)[1]);
    out.push_back(reinterpret_cast<char*>(&ucs2_char)[0]);
  }

  return true;
}

unsigned cbs_encoder::fill_cb_data_gsm7(std::vector<uint8_t>& cb_data, const std::string& warning_message)
{
  // Number of warning message characters that fit in an 82 byte CB-Data page, as per TS23.038 Section 6.1.2.2.
  static constexpr unsigned MAX_NOF_GSM7_CHARS_PAGE = 93;

  // Number of characters of the warning message to be transmitted.
  unsigned warning_message_len = warning_message.size();

  // Number of GSM-7 characters required to encode the warning message.
  unsigned encoded_message_len = cbs_encoder::get_nof_gsm7_chars(warning_message);

  // Determine the number of information pages.
  unsigned nof_pages = srsran::ceil_div(encoded_message_len, MAX_NOF_GSM7_CHARS_PAGE);

  if (nof_pages == 0) {
    ERROR("No warning message is provided");
    return SRSRAN_ERROR;
  }

  if (nof_pages > MAX_NOF_CB_DATA_PAGES) {
    ERROR("GSM-7 message length (i.e. %d chars) is too long. Maximum length is %d chars without extension characters.",
          static_cast<unsigned>(warning_message.size()),
          MAX_NOF_CB_DATA_PAGES * MAX_NOF_GSM7_CHARS_PAGE);
    return SRSRAN_ERROR;
  }

  // Resize the CB data message. See TS23.041 Section 9.4.2.2.5.
  cb_data.resize(1 + nof_pages * (INFO_PAGE_NBYTES + INFO_LEN_NBYTES));

  // Write the number of information pages.
  cb_data[0] = static_cast<uint8_t>(nof_pages);

  // Byte offset for the warning message.
  unsigned i_message_offset = 0;

  for (unsigned i_page = 0; i_page != nof_pages; ++i_page) {
    // Byte offset for the current page inside the CB-DATA IE.
    unsigned i_page_offset = 1 + i_page * (INFO_PAGE_NBYTES + INFO_LEN_NBYTES);

    // Number of characters of the warning message to be encoded into the current information page.
    unsigned i_message_len =
        std::min(MAX_NOF_GSM7_CHARS_PAGE, static_cast<unsigned>(warning_message.size()) - i_message_offset);

    // Segment of the message that will go in the current information page.
    std::string i_message_segment = warning_message.substr(i_message_offset, i_message_len);

    // Number of GSM-7 characters belonging to the current segment.
    unsigned i_encoded_message_len = cbs_encoder::get_nof_gsm7_chars(i_message_segment);

    // If the encoded message length is too large to fit in the current information page, reduce the length of the
    // message segment to be encoded.
    if (i_encoded_message_len > MAX_NOF_GSM7_CHARS_PAGE) {
      i_message_len -= (i_encoded_message_len - MAX_NOF_GSM7_CHARS_PAGE);
      i_message_segment     = warning_message.substr(i_message_offset, i_message_len);
      i_encoded_message_len = cbs_encoder::get_nof_gsm7_chars(i_message_segment);
    }

    // Encode the characters of the warning message that go into the current page.
    std::vector<uint8_t> encoded_message;
    if (!cbs_encoder::encode_gsm7(encoded_message, i_message_segment)) {
      ERROR("Warning message could not be encoded using GSM-7 default alphabet. Set the data coding scheme to a scheme "
            "using UCS-2.");
      return SRSRAN_ERROR;
    }

    srsran_assert(encoded_message.size() <= INFO_PAGE_NBYTES,
                  "The encoded warning message size (i.e., %d bytes) exceeds the size of the CB-Data information page "
                  "(i.e. %d bytes).",
                  encoded_message.size(),
                  INFO_PAGE_NBYTES);

    // Copy the encoded message bytes into the information page.
    memcpy(&cb_data[i_page_offset], encoded_message.data(), encoded_message.size());

    // Add padding to unused information page bytes.
    if (i_encoded_message_len < INFO_PAGE_NBYTES) {
      std::fill(&cb_data[i_page_offset + encoded_message.size()], &cb_data[i_page_offset + INFO_PAGE_NBYTES], 0x00);
    }

    // Add message information page length at the end.
    cb_data[i_page_offset + INFO_PAGE_NBYTES] = static_cast<uint8_t>(encoded_message.size());

    i_message_offset += i_message_len;
  }

  return SRSRAN_SUCCESS;
}

unsigned cbs_encoder::fill_cb_data_ucs2(std::vector<uint8_t>& cb_data, const std::string& warning_message)
{
  // Number of UCS-2 warning message characters that fit in an 82-byte CB-Data page.
  static constexpr unsigned MAX_NOF_UCS2_CHARS_PAGE = INFO_PAGE_NBYTES / 2;

  // Encode the characters of the warning message in one go.
  std::vector<uint8_t> encoded_message;
  if (!cbs_encoder::encode_ucs2(encoded_message, warning_message)) {
    ERROR("Warning message could not be encoded using UCS-2.");
    return SRSRAN_ERROR;
  }

  // Determine the number of information pages.
  unsigned nof_pages = srsran::ceil_div(encoded_message.size(), INFO_PAGE_NBYTES);

  if (nof_pages == 0) {
    ERROR("No warning message is provided.");
    return SRSRAN_ERROR;
  }

  if (nof_pages > MAX_NOF_CB_DATA_PAGES) {
    ERROR("UCS-2 message length (i.e. %d chars) is too long. Maximum length is %d chars.",
          static_cast<unsigned>(warning_message.size()),
          MAX_NOF_CB_DATA_PAGES * MAX_NOF_UCS2_CHARS_PAGE);
    return SRSRAN_ERROR;
  }

  // Resize the CB data message. See TS23.041 Section 9.4.2.2.5.
  cb_data.resize(1 + nof_pages * (INFO_PAGE_NBYTES + INFO_LEN_NBYTES));

  // Write the number of information pages.
  cb_data[0] = static_cast<uint8_t>(nof_pages);

  // Byte offset for the encoded warning message.
  unsigned i_message_offset = 0;

  for (unsigned i_page = 0; i_page != nof_pages; ++i_page) {
    // Byte offset for the current page inside the CB-DATA IE.
    unsigned i_page_offset = 1 + i_page * (INFO_PAGE_NBYTES + INFO_LEN_NBYTES);

    // Number of bytes of the current message segment.
    unsigned i_message_nof_bytes =
        std::min(static_cast<size_t>(INFO_PAGE_NBYTES), encoded_message.size() - static_cast<size_t>(i_message_offset));

    // Copy the encoded message bytes into the information page.
    memcpy(&cb_data[i_page_offset], &encoded_message[i_message_offset], i_message_nof_bytes);

    // Add padding to unused information page bytes.
    if (i_message_nof_bytes < INFO_PAGE_NBYTES) {
      std::fill(&cb_data[i_page_offset + i_message_nof_bytes], &cb_data[i_page_offset + INFO_PAGE_NBYTES], 0x00);
    }

    // Add message information page length at the end.
    cb_data[i_page_offset + INFO_PAGE_NBYTES] = static_cast<uint8_t>(i_message_nof_bytes);

    i_message_offset += i_message_nof_bytes;
  }

  return SRSRAN_SUCCESS;
}
