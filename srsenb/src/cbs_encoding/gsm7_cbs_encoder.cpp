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

#include "srsenb/hdr/cbs_encoding/gsm7_cbs_encoder.h"
#include <map>

using namespace srsenb;

/// GSM-7 default alphabet map, including the default extension table.
static const std::map<char, gsm7_cbs_encoder::gsm7_char> gsm7_default_alphabet_map = {
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

void gsm7_cbs_encoder::encode(std::vector<uint8_t>& out, const std::string& data)
{
  unsigned bit_count = 0;
  unsigned bit_queue = 0;

  for (unsigned i_char = 0, i_char_end = data.length(); i_char != i_char_end; ++i_char) {
    // Convert from ASCII to GSM-7 using the general language table.
    std::map<char, gsm7_char>::const_iterator gsm_char = gsm7_default_alphabet_map.find(data[i_char]);

    if (gsm_char == gsm7_default_alphabet_map.end()) {
      fprintf(stderr,
              "Character '%c' in message not found in GSM-7 default alphabet. It will be replaced by '@'.",
              data[i_char]);
      gsm_char = gsm7_default_alphabet_map.find('@');
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
}

unsigned gsm7_cbs_encoder::get_encoded_message_nof_chars(const std::string& data)
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
