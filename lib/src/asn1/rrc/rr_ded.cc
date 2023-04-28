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

#include "srsran/asn1/rrc/rr_ded.h"
#include <sstream>

using namespace asn1;
using namespace asn1::rrc;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// PollByte-r14 ::= ENUMERATED
const char* poll_byte_r14_opts::to_string() const
{
  static const char* options[] = {"kB1",     "kB2",     "kB5",     "kB8",     "kB10",    "kB15",    "kB3500",
                                  "kB4000",  "kB4500",  "kB5000",  "kB5500",  "kB6000",  "kB6500",  "kB7000",
                                  "kB7500",  "kB8000",  "kB9000",  "kB10000", "kB11000", "kB12000", "kB13000",
                                  "kB14000", "kB15000", "kB16000", "kB17000", "kB18000", "kB19000", "kB20000",
                                  "kB25000", "kB30000", "kB35000", "kB40000"};
  return convert_enum_idx(options, 32, value, "poll_byte_r14_e");
}
uint16_t poll_byte_r14_opts::to_number() const
{
  static const uint16_t options[] = {1,     2,     5,     8,     10,    15,    3500,  4000,  4500,  5000,  5500,
                                     6000,  6500,  7000,  7500,  8000,  9000,  10000, 11000, 12000, 13000, 14000,
                                     15000, 16000, 17000, 18000, 19000, 20000, 25000, 30000, 35000, 40000};
  return map_enum_number(options, 32, value, "poll_byte_r14_e");
}

// PollPDU-r15 ::= ENUMERATED
const char* poll_pdu_r15_opts::to_string() const
{
  static const char* options[] = {"p4",
                                  "p8",
                                  "p16",
                                  "p32",
                                  "p64",
                                  "p128",
                                  "p256",
                                  "p512",
                                  "p1024",
                                  "p2048-r15",
                                  "p4096-r15",
                                  "p6144-r15",
                                  "p8192-r15",
                                  "p12288-r15",
                                  "p16384-r15",
                                  "pInfinity"};
  return convert_enum_idx(options, 16, value, "poll_pdu_r15_e");
}
int16_t poll_pdu_r15_opts::to_number() const
{
  static const int16_t options[] = {4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 6144, 8192, 12288, 16384, -1};
  return map_enum_number(options, 16, value, "poll_pdu_r15_e");
}

// SN-FieldLength ::= ENUMERATED
const char* sn_field_len_opts::to_string() const
{
  static const char* options[] = {"size5", "size10"};
  return convert_enum_idx(options, 2, value, "sn_field_len_e");
}
uint8_t sn_field_len_opts::to_number() const
{
  static const uint8_t options[] = {5, 10};
  return map_enum_number(options, 2, value, "sn_field_len_e");
}

// SN-FieldLength-r15 ::= ENUMERATED
const char* sn_field_len_r15_opts::to_string() const
{
  static const char* options[] = {"size5", "size10", "size16-r15"};
  return convert_enum_idx(options, 3, value, "sn_field_len_r15_e");
}
uint8_t sn_field_len_r15_opts::to_number() const
{
  static const uint8_t options[] = {5, 10, 16};
  return map_enum_number(options, 3, value, "sn_field_len_r15_e");
}

// T-PollRetransmit ::= ENUMERATED
const char* t_poll_retx_opts::to_string() const
{
  static const char* options[] = {
      "ms5",          "ms10",         "ms15",         "ms20",   "ms25",   "ms30",   "ms35",   "ms40",
      "ms45",         "ms50",         "ms55",         "ms60",   "ms65",   "ms70",   "ms75",   "ms80",
      "ms85",         "ms90",         "ms95",         "ms100",  "ms105",  "ms110",  "ms115",  "ms120",
      "ms125",        "ms130",        "ms135",        "ms140",  "ms145",  "ms150",  "ms155",  "ms160",
      "ms165",        "ms170",        "ms175",        "ms180",  "ms185",  "ms190",  "ms195",  "ms200",
      "ms205",        "ms210",        "ms215",        "ms220",  "ms225",  "ms230",  "ms235",  "ms240",
      "ms245",        "ms250",        "ms300",        "ms350",  "ms400",  "ms450",  "ms500",  "ms800-v1310",
      "ms1000-v1310", "ms2000-v1310", "ms4000-v1310", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 64, value, "t_poll_retx_e");
}
uint16_t t_poll_retx_opts::to_number() const
{
  static const uint16_t options[] = {5,   10,  15,  20,  25,  30,  35,  40,  45,  50,  55,  60,   65,   70,  75,
                                     80,  85,  90,  95,  100, 105, 110, 115, 120, 125, 130, 135,  140,  145, 150,
                                     155, 160, 165, 170, 175, 180, 185, 190, 195, 200, 205, 210,  215,  220, 225,
                                     230, 235, 240, 245, 250, 300, 350, 400, 450, 500, 800, 1000, 2000, 4000};
  return map_enum_number(options, 59, value, "t_poll_retx_e");
}

// T-Reordering ::= ENUMERATED
const char* t_reordering_opts::to_string() const
{
  static const char* options[] = {"ms0",   "ms5",   "ms10",  "ms15",  "ms20",  "ms25",  "ms30",  "ms35",
                                  "ms40",  "ms45",  "ms50",  "ms55",  "ms60",  "ms65",  "ms70",  "ms75",
                                  "ms80",  "ms85",  "ms90",  "ms95",  "ms100", "ms110", "ms120", "ms130",
                                  "ms140", "ms150", "ms160", "ms170", "ms180", "ms190", "ms200", "ms1600-v1310"};
  return convert_enum_idx(options, 32, value, "t_reordering_e");
}
uint16_t t_reordering_opts::to_number() const
{
  static const uint16_t options[] = {0,  5,  10, 15, 20,  25,  30,  35,  40,  45,  50,  55,  60,  65,  70,  75,
                                     80, 85, 90, 95, 100, 110, 120, 130, 140, 150, 160, 170, 180, 190, 200, 1600};
  return map_enum_number(options, 32, value, "t_reordering_e");
}

// T-StatusProhibit ::= ENUMERATED
const char* t_status_prohibit_opts::to_string() const
{
  static const char* options[] = {
      "ms0",         "ms5",          "ms10",         "ms15",         "ms20",         "ms25",         "ms30",
      "ms35",        "ms40",         "ms45",         "ms50",         "ms55",         "ms60",         "ms65",
      "ms70",        "ms75",         "ms80",         "ms85",         "ms90",         "ms95",         "ms100",
      "ms105",       "ms110",        "ms115",        "ms120",        "ms125",        "ms130",        "ms135",
      "ms140",       "ms145",        "ms150",        "ms155",        "ms160",        "ms165",        "ms170",
      "ms175",       "ms180",        "ms185",        "ms190",        "ms195",        "ms200",        "ms205",
      "ms210",       "ms215",        "ms220",        "ms225",        "ms230",        "ms235",        "ms240",
      "ms245",       "ms250",        "ms300",        "ms350",        "ms400",        "ms450",        "ms500",
      "ms800-v1310", "ms1000-v1310", "ms1200-v1310", "ms1600-v1310", "ms2000-v1310", "ms2400-v1310", "spare2",
      "spare1"};
  return convert_enum_idx(options, 64, value, "t_status_prohibit_e");
}
uint16_t t_status_prohibit_opts::to_number() const
{
  static const uint16_t options[] = {0,   5,   10,  15,  20,  25,  30,  35,  40,  45,   50,   55,   60,   65,  70,  75,
                                     80,  85,  90,  95,  100, 105, 110, 115, 120, 125,  130,  135,  140,  145, 150, 155,
                                     160, 165, 170, 175, 180, 185, 190, 195, 200, 205,  210,  215,  220,  225, 230, 235,
                                     240, 245, 250, 300, 350, 400, 450, 500, 800, 1000, 1200, 1600, 2000, 2400};
  return map_enum_number(options, 62, value, "t_status_prohibit_e");
}

// DL-AM-RLC-r15 ::= SEQUENCE
SRSASN_CODE dl_am_rlc_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(t_reordering_r15.pack(bref));
  HANDLE_CODE(t_status_prohibit_r15.pack(bref));
  HANDLE_CODE(bref.pack(extended_rlc_li_field_r15, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_am_rlc_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(t_reordering_r15.unpack(bref));
  HANDLE_CODE(t_status_prohibit_r15.unpack(bref));
  HANDLE_CODE(bref.unpack(extended_rlc_li_field_r15, 1));

  return SRSASN_SUCCESS;
}
void dl_am_rlc_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("t-Reordering-r15", t_reordering_r15.to_string());
  j.write_str("t-StatusProhibit-r15", t_status_prohibit_r15.to_string());
  j.write_bool("extended-RLC-LI-Field-r15", extended_rlc_li_field_r15);
  j.end_obj();
}
bool dl_am_rlc_r15_s::operator==(const dl_am_rlc_r15_s& other) const
{
  return t_reordering_r15 == other.t_reordering_r15 and t_status_prohibit_r15 == other.t_status_prohibit_r15 and
         extended_rlc_li_field_r15 == other.extended_rlc_li_field_r15;
}

// DL-UM-RLC-r15 ::= SEQUENCE
SRSASN_CODE dl_um_rlc_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(sn_field_len_r15.pack(bref));
  HANDLE_CODE(t_reordering_r15.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_um_rlc_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(sn_field_len_r15.unpack(bref));
  HANDLE_CODE(t_reordering_r15.unpack(bref));

  return SRSASN_SUCCESS;
}
void dl_um_rlc_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("sn-FieldLength-r15", sn_field_len_r15.to_string());
  j.write_str("t-Reordering-r15", t_reordering_r15.to_string());
  j.end_obj();
}
bool dl_um_rlc_r15_s::operator==(const dl_um_rlc_r15_s& other) const
{
  return sn_field_len_r15 == other.sn_field_len_r15 and t_reordering_r15 == other.t_reordering_r15;
}

// PollByte ::= ENUMERATED
const char* poll_byte_opts::to_string() const
{
  static const char* options[] = {"kB25",
                                  "kB50",
                                  "kB75",
                                  "kB100",
                                  "kB125",
                                  "kB250",
                                  "kB375",
                                  "kB500",
                                  "kB750",
                                  "kB1000",
                                  "kB1250",
                                  "kB1500",
                                  "kB2000",
                                  "kB3000",
                                  "kBinfinity",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "poll_byte_e");
}
int16_t poll_byte_opts::to_number() const
{
  static const int16_t options[] = {25, 50, 75, 100, 125, 250, 375, 500, 750, 1000, 1250, 1500, 2000, 3000, -1};
  return map_enum_number(options, 15, value, "poll_byte_e");
}

// PollPDU ::= ENUMERATED
const char* poll_pdu_opts::to_string() const
{
  static const char* options[] = {"p4", "p8", "p16", "p32", "p64", "p128", "p256", "pInfinity"};
  return convert_enum_idx(options, 8, value, "poll_pdu_e");
}
int16_t poll_pdu_opts::to_number() const
{
  static const int16_t options[] = {4, 8, 16, 32, 64, 128, 256, -1};
  return map_enum_number(options, 8, value, "poll_pdu_e");
}

// UL-AM-RLC-r15 ::= SEQUENCE
SRSASN_CODE ul_am_rlc_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(t_poll_retx_r15.pack(bref));
  HANDLE_CODE(poll_pdu_r15.pack(bref));
  HANDLE_CODE(poll_byte_r15.pack(bref));
  HANDLE_CODE(max_retx_thres_r15.pack(bref));
  HANDLE_CODE(bref.pack(extended_rlc_li_field_r15, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_am_rlc_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(t_poll_retx_r15.unpack(bref));
  HANDLE_CODE(poll_pdu_r15.unpack(bref));
  HANDLE_CODE(poll_byte_r15.unpack(bref));
  HANDLE_CODE(max_retx_thres_r15.unpack(bref));
  HANDLE_CODE(bref.unpack(extended_rlc_li_field_r15, 1));

  return SRSASN_SUCCESS;
}
void ul_am_rlc_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("t-PollRetransmit-r15", t_poll_retx_r15.to_string());
  j.write_str("pollPDU-r15", poll_pdu_r15.to_string());
  j.write_str("pollByte-r15", poll_byte_r15.to_string());
  j.write_str("maxRetxThreshold-r15", max_retx_thres_r15.to_string());
  j.write_bool("extended-RLC-LI-Field-r15", extended_rlc_li_field_r15);
  j.end_obj();
}
bool ul_am_rlc_r15_s::operator==(const ul_am_rlc_r15_s& other) const
{
  return t_poll_retx_r15 == other.t_poll_retx_r15 and poll_pdu_r15 == other.poll_pdu_r15 and
         poll_byte_r15 == other.poll_byte_r15 and max_retx_thres_r15 == other.max_retx_thres_r15 and
         extended_rlc_li_field_r15 == other.extended_rlc_li_field_r15;
}

const char* ul_am_rlc_r15_s::max_retx_thres_r15_opts::to_string() const
{
  static const char* options[] = {"t1", "t2", "t3", "t4", "t6", "t8", "t16", "t32"};
  return convert_enum_idx(options, 8, value, "ul_am_rlc_r15_s::max_retx_thres_r15_e_");
}
uint8_t ul_am_rlc_r15_s::max_retx_thres_r15_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 6, 8, 16, 32};
  return map_enum_number(options, 8, value, "ul_am_rlc_r15_s::max_retx_thres_r15_e_");
}

// UL-UM-RLC ::= SEQUENCE
SRSASN_CODE ul_um_rlc_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(sn_field_len.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_um_rlc_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(sn_field_len.unpack(bref));

  return SRSASN_SUCCESS;
}
void ul_um_rlc_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("sn-FieldLength", sn_field_len.to_string());
  j.end_obj();
}
bool ul_um_rlc_s::operator==(const ul_um_rlc_s& other) const
{
  return sn_field_len == other.sn_field_len;
}

// DL-AM-RLC ::= SEQUENCE
SRSASN_CODE dl_am_rlc_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(t_reordering.pack(bref));
  HANDLE_CODE(t_status_prohibit.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_am_rlc_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(t_reordering.unpack(bref));
  HANDLE_CODE(t_status_prohibit.unpack(bref));

  return SRSASN_SUCCESS;
}
void dl_am_rlc_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("t-Reordering", t_reordering.to_string());
  j.write_str("t-StatusProhibit", t_status_prohibit.to_string());
  j.end_obj();
}
bool dl_am_rlc_s::operator==(const dl_am_rlc_s& other) const
{
  return t_reordering == other.t_reordering and t_status_prohibit == other.t_status_prohibit;
}

// DL-UM-RLC ::= SEQUENCE
SRSASN_CODE dl_um_rlc_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(sn_field_len.pack(bref));
  HANDLE_CODE(t_reordering.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_um_rlc_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(sn_field_len.unpack(bref));
  HANDLE_CODE(t_reordering.unpack(bref));

  return SRSASN_SUCCESS;
}
void dl_um_rlc_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("sn-FieldLength", sn_field_len.to_string());
  j.write_str("t-Reordering", t_reordering.to_string());
  j.end_obj();
}
bool dl_um_rlc_s::operator==(const dl_um_rlc_s& other) const
{
  return sn_field_len == other.sn_field_len and t_reordering == other.t_reordering;
}

// DiscardTimerExt-r17 ::= ENUMERATED
const char* discard_timer_ext_r17_opts::to_string() const
{
  static const char* options[] = {"ms2000", "spare"};
  return convert_enum_idx(options, 2, value, "discard_timer_ext_r17_e");
}
uint16_t discard_timer_ext_r17_opts::to_number() const
{
  static const uint16_t options[] = {2000};
  return map_enum_number(options, 1, value, "discard_timer_ext_r17_e");
}

// EthernetHeaderCompression-r16 ::= SEQUENCE
SRSASN_CODE ethernet_hdr_compress_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ehc_dl_r16_present, 1));
  HANDLE_CODE(bref.pack(ehc_ul_r16_present, 1));

  HANDLE_CODE(ehc_common_r16.ehc_cid_len_r16.pack(bref));
  if (ehc_dl_r16_present) {
    HANDLE_CODE(bref.pack(ehc_dl_r16.drb_continue_ehc_dl_r16_present, 1));
  }
  if (ehc_ul_r16_present) {
    HANDLE_CODE(bref.pack(ehc_ul_r16.drb_continue_ehc_ul_r16_present, 1));
    HANDLE_CODE(pack_integer(bref, ehc_ul_r16.max_cid_ehc_ul_r16, (uint16_t)1u, (uint16_t)32767u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ethernet_hdr_compress_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ehc_dl_r16_present, 1));
  HANDLE_CODE(bref.unpack(ehc_ul_r16_present, 1));

  HANDLE_CODE(ehc_common_r16.ehc_cid_len_r16.unpack(bref));
  if (ehc_dl_r16_present) {
    HANDLE_CODE(bref.unpack(ehc_dl_r16.drb_continue_ehc_dl_r16_present, 1));
  }
  if (ehc_ul_r16_present) {
    HANDLE_CODE(bref.unpack(ehc_ul_r16.drb_continue_ehc_ul_r16_present, 1));
    HANDLE_CODE(unpack_integer(ehc_ul_r16.max_cid_ehc_ul_r16, bref, (uint16_t)1u, (uint16_t)32767u));
  }

  return SRSASN_SUCCESS;
}
void ethernet_hdr_compress_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("ehc-Common-r16");
  j.start_obj();
  j.write_str("ehc-CID-Length-r16", ehc_common_r16.ehc_cid_len_r16.to_string());
  j.end_obj();
  if (ehc_dl_r16_present) {
    j.write_fieldname("ehc-Downlink-r16");
    j.start_obj();
    if (ehc_dl_r16.drb_continue_ehc_dl_r16_present) {
      j.write_str("drb-ContinueEHC-DL-r16", "true");
    }
    j.end_obj();
  }
  if (ehc_ul_r16_present) {
    j.write_fieldname("ehc-Uplink-r16");
    j.start_obj();
    j.write_int("maxCID-EHC-UL-r16", ehc_ul_r16.max_cid_ehc_ul_r16);
    if (ehc_ul_r16.drb_continue_ehc_ul_r16_present) {
      j.write_str("drb-ContinueEHC-UL-r16", "true");
    }
    j.end_obj();
  }
  j.end_obj();
}
bool ethernet_hdr_compress_r16_s::operator==(const ethernet_hdr_compress_r16_s& other) const
{
  return ext == other.ext and ehc_common_r16.ehc_cid_len_r16 == other.ehc_common_r16.ehc_cid_len_r16 and
         ehc_dl_r16.drb_continue_ehc_dl_r16_present == other.ehc_dl_r16.drb_continue_ehc_dl_r16_present and
         ehc_ul_r16.max_cid_ehc_ul_r16 == other.ehc_ul_r16.max_cid_ehc_ul_r16 and
         ehc_ul_r16.drb_continue_ehc_ul_r16_present == other.ehc_ul_r16.drb_continue_ehc_ul_r16_present;
}

const char* ethernet_hdr_compress_r16_s::ehc_common_r16_s_::ehc_cid_len_r16_opts::to_string() const
{
  static const char* options[] = {"bits7", "bits15"};
  return convert_enum_idx(options, 2, value, "ethernet_hdr_compress_r16_s::ehc_common_r16_s_::ehc_cid_len_r16_e_");
}
uint8_t ethernet_hdr_compress_r16_s::ehc_common_r16_s_::ehc_cid_len_r16_opts::to_number() const
{
  static const uint8_t options[] = {7, 15};
  return map_enum_number(options, 2, value, "ethernet_hdr_compress_r16_s::ehc_common_r16_s_::ehc_cid_len_r16_e_");
}

// LogicalChannelConfig ::= SEQUENCE
SRSASN_CODE lc_ch_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ul_specific_params_present, 1));

  if (ul_specific_params_present) {
    HANDLE_CODE(bref.pack(ul_specific_params.lc_ch_group_present, 1));
    HANDLE_CODE(pack_integer(bref, ul_specific_params.prio, (uint8_t)1u, (uint8_t)16u));
    HANDLE_CODE(ul_specific_params.prioritised_bit_rate.pack(bref));
    HANDLE_CODE(ul_specific_params.bucket_size_dur.pack(bref));
    if (ul_specific_params.lc_ch_group_present) {
      HANDLE_CODE(pack_integer(bref, ul_specific_params.lc_ch_group, (uint8_t)0u, (uint8_t)3u));
    }
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= lc_ch_sr_mask_r9_present;
    group_flags[1] |= lc_ch_sr_prohibit_r12_present;
    group_flags[2] |= laa_ul_allowed_r14_present;
    group_flags[2] |= bit_rate_query_prohibit_timer_r14_present;
    group_flags[3] |= allowed_tti_lens_r15.is_present();
    group_flags[3] |= lc_ch_sr_restrict_r15.is_present();
    group_flags[3] |= ch_access_prio_r15.is_present();
    group_flags[3] |= lch_cell_restrict_r15_present;
    group_flags[4] |= bit_rate_multiplier_r16_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(lc_ch_sr_mask_r9_present, 1));
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(lc_ch_sr_prohibit_r12_present, 1));
      if (lc_ch_sr_prohibit_r12_present) {
        HANDLE_CODE(bref.pack(lc_ch_sr_prohibit_r12, 1));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(laa_ul_allowed_r14_present, 1));
      HANDLE_CODE(bref.pack(bit_rate_query_prohibit_timer_r14_present, 1));
      if (laa_ul_allowed_r14_present) {
        HANDLE_CODE(bref.pack(laa_ul_allowed_r14, 1));
      }
      if (bit_rate_query_prohibit_timer_r14_present) {
        HANDLE_CODE(bit_rate_query_prohibit_timer_r14.pack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(allowed_tti_lens_r15.is_present(), 1));
      HANDLE_CODE(bref.pack(lc_ch_sr_restrict_r15.is_present(), 1));
      HANDLE_CODE(bref.pack(ch_access_prio_r15.is_present(), 1));
      HANDLE_CODE(bref.pack(lch_cell_restrict_r15_present, 1));
      if (allowed_tti_lens_r15.is_present()) {
        HANDLE_CODE(allowed_tti_lens_r15->pack(bref));
      }
      if (lc_ch_sr_restrict_r15.is_present()) {
        HANDLE_CODE(lc_ch_sr_restrict_r15->pack(bref));
      }
      if (ch_access_prio_r15.is_present()) {
        HANDLE_CODE(ch_access_prio_r15->pack(bref));
      }
      if (lch_cell_restrict_r15_present) {
        HANDLE_CODE(lch_cell_restrict_r15.pack(bref));
      }
    }
    if (group_flags[4]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(bit_rate_multiplier_r16_present, 1));
      if (bit_rate_multiplier_r16_present) {
        HANDLE_CODE(bit_rate_multiplier_r16.pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE lc_ch_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ul_specific_params_present, 1));

  if (ul_specific_params_present) {
    HANDLE_CODE(bref.unpack(ul_specific_params.lc_ch_group_present, 1));
    HANDLE_CODE(unpack_integer(ul_specific_params.prio, bref, (uint8_t)1u, (uint8_t)16u));
    HANDLE_CODE(ul_specific_params.prioritised_bit_rate.unpack(bref));
    HANDLE_CODE(ul_specific_params.bucket_size_dur.unpack(bref));
    if (ul_specific_params.lc_ch_group_present) {
      HANDLE_CODE(unpack_integer(ul_specific_params.lc_ch_group, bref, (uint8_t)0u, (uint8_t)3u));
    }
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(5);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(lc_ch_sr_mask_r9_present, 1));
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(lc_ch_sr_prohibit_r12_present, 1));
      if (lc_ch_sr_prohibit_r12_present) {
        HANDLE_CODE(bref.unpack(lc_ch_sr_prohibit_r12, 1));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(laa_ul_allowed_r14_present, 1));
      HANDLE_CODE(bref.unpack(bit_rate_query_prohibit_timer_r14_present, 1));
      if (laa_ul_allowed_r14_present) {
        HANDLE_CODE(bref.unpack(laa_ul_allowed_r14, 1));
      }
      if (bit_rate_query_prohibit_timer_r14_present) {
        HANDLE_CODE(bit_rate_query_prohibit_timer_r14.unpack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool allowed_tti_lens_r15_present;
      HANDLE_CODE(bref.unpack(allowed_tti_lens_r15_present, 1));
      allowed_tti_lens_r15.set_present(allowed_tti_lens_r15_present);
      bool lc_ch_sr_restrict_r15_present;
      HANDLE_CODE(bref.unpack(lc_ch_sr_restrict_r15_present, 1));
      lc_ch_sr_restrict_r15.set_present(lc_ch_sr_restrict_r15_present);
      bool ch_access_prio_r15_present;
      HANDLE_CODE(bref.unpack(ch_access_prio_r15_present, 1));
      ch_access_prio_r15.set_present(ch_access_prio_r15_present);
      HANDLE_CODE(bref.unpack(lch_cell_restrict_r15_present, 1));
      if (allowed_tti_lens_r15.is_present()) {
        HANDLE_CODE(allowed_tti_lens_r15->unpack(bref));
      }
      if (lc_ch_sr_restrict_r15.is_present()) {
        HANDLE_CODE(lc_ch_sr_restrict_r15->unpack(bref));
      }
      if (ch_access_prio_r15.is_present()) {
        HANDLE_CODE(ch_access_prio_r15->unpack(bref));
      }
      if (lch_cell_restrict_r15_present) {
        HANDLE_CODE(lch_cell_restrict_r15.unpack(bref));
      }
    }
    if (group_flags[4]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(bit_rate_multiplier_r16_present, 1));
      if (bit_rate_multiplier_r16_present) {
        HANDLE_CODE(bit_rate_multiplier_r16.unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void lc_ch_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ul_specific_params_present) {
    j.write_fieldname("ul-SpecificParameters");
    j.start_obj();
    j.write_int("priority", ul_specific_params.prio);
    j.write_str("prioritisedBitRate", ul_specific_params.prioritised_bit_rate.to_string());
    j.write_str("bucketSizeDuration", ul_specific_params.bucket_size_dur.to_string());
    if (ul_specific_params.lc_ch_group_present) {
      j.write_int("logicalChannelGroup", ul_specific_params.lc_ch_group);
    }
    j.end_obj();
  }
  if (ext) {
    if (lc_ch_sr_mask_r9_present) {
      j.write_str("logicalChannelSR-Mask-r9", "setup");
    }
    if (lc_ch_sr_prohibit_r12_present) {
      j.write_bool("logicalChannelSR-Prohibit-r12", lc_ch_sr_prohibit_r12);
    }
    if (laa_ul_allowed_r14_present) {
      j.write_bool("laa-UL-Allowed-r14", laa_ul_allowed_r14);
    }
    if (bit_rate_query_prohibit_timer_r14_present) {
      j.write_str("bitRateQueryProhibitTimer-r14", bit_rate_query_prohibit_timer_r14.to_string());
    }
    if (allowed_tti_lens_r15.is_present()) {
      j.write_fieldname("allowedTTI-Lengths-r15");
      allowed_tti_lens_r15->to_json(j);
    }
    if (lc_ch_sr_restrict_r15.is_present()) {
      j.write_fieldname("logicalChannelSR-Restriction-r15");
      lc_ch_sr_restrict_r15->to_json(j);
    }
    if (ch_access_prio_r15.is_present()) {
      j.write_fieldname("channelAccessPriority-r15");
      ch_access_prio_r15->to_json(j);
    }
    if (lch_cell_restrict_r15_present) {
      j.write_str("lch-CellRestriction-r15", lch_cell_restrict_r15.to_string());
    }
    if (bit_rate_multiplier_r16_present) {
      j.write_str("bitRateMultiplier-r16", bit_rate_multiplier_r16.to_string());
    }
  }
  j.end_obj();
}
bool lc_ch_cfg_s::operator==(const lc_ch_cfg_s& other) const
{
  return ext == other.ext and ul_specific_params.prio == other.ul_specific_params.prio and
         ul_specific_params.prioritised_bit_rate == other.ul_specific_params.prioritised_bit_rate and
         ul_specific_params.bucket_size_dur == other.ul_specific_params.bucket_size_dur and
         ul_specific_params.lc_ch_group_present == other.ul_specific_params.lc_ch_group_present and
         (not ul_specific_params.lc_ch_group_present or
          ul_specific_params.lc_ch_group == other.ul_specific_params.lc_ch_group) and
         (not ext or
          (lc_ch_sr_mask_r9_present == other.lc_ch_sr_mask_r9_present and
           lc_ch_sr_prohibit_r12_present == other.lc_ch_sr_prohibit_r12_present and
           (not lc_ch_sr_prohibit_r12_present or lc_ch_sr_prohibit_r12 == other.lc_ch_sr_prohibit_r12) and
           laa_ul_allowed_r14_present == other.laa_ul_allowed_r14_present and
           (not laa_ul_allowed_r14_present or laa_ul_allowed_r14 == other.laa_ul_allowed_r14) and
           bit_rate_query_prohibit_timer_r14_present == other.bit_rate_query_prohibit_timer_r14_present and
           (not bit_rate_query_prohibit_timer_r14_present or
            bit_rate_query_prohibit_timer_r14 == other.bit_rate_query_prohibit_timer_r14) and
           allowed_tti_lens_r15.is_present() == other.allowed_tti_lens_r15.is_present() and
           (not allowed_tti_lens_r15.is_present() or *allowed_tti_lens_r15 == *other.allowed_tti_lens_r15) and
           lc_ch_sr_restrict_r15.is_present() == other.lc_ch_sr_restrict_r15.is_present() and
           (not lc_ch_sr_restrict_r15.is_present() or *lc_ch_sr_restrict_r15 == *other.lc_ch_sr_restrict_r15) and
           ch_access_prio_r15.is_present() == other.ch_access_prio_r15.is_present() and
           (not ch_access_prio_r15.is_present() or *ch_access_prio_r15 == *other.ch_access_prio_r15) and
           lch_cell_restrict_r15_present == other.lch_cell_restrict_r15_present and
           (not lch_cell_restrict_r15_present or lch_cell_restrict_r15 == other.lch_cell_restrict_r15) and
           bit_rate_multiplier_r16_present == other.bit_rate_multiplier_r16_present and
           (not bit_rate_multiplier_r16_present or bit_rate_multiplier_r16 == other.bit_rate_multiplier_r16)));
}

const char* lc_ch_cfg_s::ul_specific_params_s_::prioritised_bit_rate_opts::to_string() const
{
  static const char* options[] = {"kBps0",
                                  "kBps8",
                                  "kBps16",
                                  "kBps32",
                                  "kBps64",
                                  "kBps128",
                                  "kBps256",
                                  "infinity",
                                  "kBps512-v1020",
                                  "kBps1024-v1020",
                                  "kBps2048-v1020",
                                  "spare5",
                                  "spare4",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "lc_ch_cfg_s::ul_specific_params_s_::prioritised_bit_rate_e_");
}
int16_t lc_ch_cfg_s::ul_specific_params_s_::prioritised_bit_rate_opts::to_number() const
{
  static const int16_t options[] = {0, 8, 16, 32, 64, 128, 256, -1, 512, 1024, 2048};
  return map_enum_number(options, 11, value, "lc_ch_cfg_s::ul_specific_params_s_::prioritised_bit_rate_e_");
}

const char* lc_ch_cfg_s::ul_specific_params_s_::bucket_size_dur_opts::to_string() const
{
  static const char* options[] = {"ms50", "ms100", "ms150", "ms300", "ms500", "ms1000", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "lc_ch_cfg_s::ul_specific_params_s_::bucket_size_dur_e_");
}
uint16_t lc_ch_cfg_s::ul_specific_params_s_::bucket_size_dur_opts::to_number() const
{
  static const uint16_t options[] = {50, 100, 150, 300, 500, 1000};
  return map_enum_number(options, 6, value, "lc_ch_cfg_s::ul_specific_params_s_::bucket_size_dur_e_");
}

const char* lc_ch_cfg_s::bit_rate_query_prohibit_timer_r14_opts::to_string() const
{
  static const char* options[] = {"s0", "s0dot4", "s0dot8", "s1dot6", "s3", "s6", "s12", "s30"};
  return convert_enum_idx(options, 8, value, "lc_ch_cfg_s::bit_rate_query_prohibit_timer_r14_e_");
}
float lc_ch_cfg_s::bit_rate_query_prohibit_timer_r14_opts::to_number() const
{
  static const float options[] = {0.0, 0.4, 0.8, 1.6, 3.0, 6.0, 12.0, 30.0};
  return map_enum_number(options, 8, value, "lc_ch_cfg_s::bit_rate_query_prohibit_timer_r14_e_");
}
const char* lc_ch_cfg_s::bit_rate_query_prohibit_timer_r14_opts::to_number_string() const
{
  static const char* options[] = {"0", "0.4", "0.8", "1.6", "3", "6", "12", "30"};
  return convert_enum_idx(options, 8, value, "lc_ch_cfg_s::bit_rate_query_prohibit_timer_r14_e_");
}

void lc_ch_cfg_s::allowed_tti_lens_r15_c_::set(types::options e)
{
  type_ = e;
}
void lc_ch_cfg_s::allowed_tti_lens_r15_c_::set_release()
{
  set(types::release);
}
lc_ch_cfg_s::allowed_tti_lens_r15_c_::setup_s_& lc_ch_cfg_s::allowed_tti_lens_r15_c_::set_setup()
{
  set(types::setup);
  return c;
}
void lc_ch_cfg_s::allowed_tti_lens_r15_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_bool("shortTTI-r15", c.short_tti_r15);
      j.write_bool("subframeTTI-r15", c.sf_tti_r15);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "lc_ch_cfg_s::allowed_tti_lens_r15_c_");
  }
  j.end_obj();
}
SRSASN_CODE lc_ch_cfg_s::allowed_tti_lens_r15_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.short_tti_r15, 1));
      HANDLE_CODE(bref.pack(c.sf_tti_r15, 1));
      break;
    default:
      log_invalid_choice_id(type_, "lc_ch_cfg_s::allowed_tti_lens_r15_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE lc_ch_cfg_s::allowed_tti_lens_r15_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.short_tti_r15, 1));
      HANDLE_CODE(bref.unpack(c.sf_tti_r15, 1));
      break;
    default:
      log_invalid_choice_id(type_, "lc_ch_cfg_s::allowed_tti_lens_r15_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool lc_ch_cfg_s::allowed_tti_lens_r15_c_::operator==(const allowed_tti_lens_r15_c_& other) const
{
  return type() == other.type() and c.short_tti_r15 == other.c.short_tti_r15 and c.sf_tti_r15 == other.c.sf_tti_r15;
}

void lc_ch_cfg_s::lc_ch_sr_restrict_r15_c_::set(types::options e)
{
  type_ = e;
}
void lc_ch_cfg_s::lc_ch_sr_restrict_r15_c_::set_release()
{
  set(types::release);
}
lc_ch_cfg_s::lc_ch_sr_restrict_r15_c_::setup_e_& lc_ch_cfg_s::lc_ch_sr_restrict_r15_c_::set_setup()
{
  set(types::setup);
  return c;
}
void lc_ch_cfg_s::lc_ch_sr_restrict_r15_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_str("setup", c.to_string());
      break;
    default:
      log_invalid_choice_id(type_, "lc_ch_cfg_s::lc_ch_sr_restrict_r15_c_");
  }
  j.end_obj();
}
SRSASN_CODE lc_ch_cfg_s::lc_ch_sr_restrict_r15_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "lc_ch_cfg_s::lc_ch_sr_restrict_r15_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE lc_ch_cfg_s::lc_ch_sr_restrict_r15_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "lc_ch_cfg_s::lc_ch_sr_restrict_r15_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool lc_ch_cfg_s::lc_ch_sr_restrict_r15_c_::operator==(const lc_ch_sr_restrict_r15_c_& other) const
{
  return type() == other.type() and c == other.c;
}

const char* lc_ch_cfg_s::lc_ch_sr_restrict_r15_c_::setup_opts::to_string() const
{
  static const char* options[] = {"spucch", "pucch"};
  return convert_enum_idx(options, 2, value, "lc_ch_cfg_s::lc_ch_sr_restrict_r15_c_::setup_e_");
}

void lc_ch_cfg_s::ch_access_prio_r15_c_::set(types::options e)
{
  type_ = e;
}
void lc_ch_cfg_s::ch_access_prio_r15_c_::set_release()
{
  set(types::release);
}
uint8_t& lc_ch_cfg_s::ch_access_prio_r15_c_::set_setup()
{
  set(types::setup);
  return c;
}
void lc_ch_cfg_s::ch_access_prio_r15_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_int("setup", c);
      break;
    default:
      log_invalid_choice_id(type_, "lc_ch_cfg_s::ch_access_prio_r15_c_");
  }
  j.end_obj();
}
SRSASN_CODE lc_ch_cfg_s::ch_access_prio_r15_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(pack_integer(bref, c, (uint8_t)1u, (uint8_t)4u));
      break;
    default:
      log_invalid_choice_id(type_, "lc_ch_cfg_s::ch_access_prio_r15_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE lc_ch_cfg_s::ch_access_prio_r15_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(unpack_integer(c, bref, (uint8_t)1u, (uint8_t)4u));
      break;
    default:
      log_invalid_choice_id(type_, "lc_ch_cfg_s::ch_access_prio_r15_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool lc_ch_cfg_s::ch_access_prio_r15_c_::operator==(const ch_access_prio_r15_c_& other) const
{
  return type() == other.type() and c == other.c;
}

const char* lc_ch_cfg_s::bit_rate_multiplier_r16_opts::to_string() const
{
  static const char* options[] = {"x40", "x70", "x100", "x200"};
  return convert_enum_idx(options, 4, value, "lc_ch_cfg_s::bit_rate_multiplier_r16_e_");
}
uint8_t lc_ch_cfg_s::bit_rate_multiplier_r16_opts::to_number() const
{
  static const uint8_t options[] = {40, 70, 100, 200};
  return map_enum_number(options, 4, value, "lc_ch_cfg_s::bit_rate_multiplier_r16_e_");
}

// P-a ::= ENUMERATED
const char* p_a_opts::to_string() const
{
  static const char* options[] = {"dB-6", "dB-4dot77", "dB-3", "dB-1dot77", "dB0", "dB1", "dB2", "dB3"};
  return convert_enum_idx(options, 8, value, "p_a_e");
}
float p_a_opts::to_number() const
{
  static const float options[] = {-6.0, -4.77, -3.0, -1.77, 0.0, 1.0, 2.0, 3.0};
  return map_enum_number(options, 8, value, "p_a_e");
}
const char* p_a_opts::to_number_string() const
{
  static const char* options[] = {"-6", "-4.77", "-3", "-1.77", "0", "1", "2", "3"};
  return convert_enum_idx(options, 8, value, "p_a_e");
}

// PollPDU-v1310 ::= ENUMERATED
const char* poll_pdu_v1310_opts::to_string() const
{
  static const char* options[] = {"p512", "p1024", "p2048", "p4096", "p6144", "p8192", "p12288", "p16384"};
  return convert_enum_idx(options, 8, value, "poll_pdu_v1310_e");
}
uint16_t poll_pdu_v1310_opts::to_number() const
{
  static const uint16_t options[] = {512, 1024, 2048, 4096, 6144, 8192, 12288, 16384};
  return map_enum_number(options, 8, value, "poll_pdu_v1310_e");
}

// RLC-Config-r15 ::= SEQUENCE
SRSASN_CODE rlc_cfg_r15_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(reestablish_rlc_r15_present, 1));
  HANDLE_CODE(bref.pack(rlc_out_of_order_delivery_r15_present, 1));

  HANDLE_CODE(mode_r15.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rlc_cfg_r15_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(reestablish_rlc_r15_present, 1));
  HANDLE_CODE(bref.unpack(rlc_out_of_order_delivery_r15_present, 1));

  HANDLE_CODE(mode_r15.unpack(bref));

  return SRSASN_SUCCESS;
}
void rlc_cfg_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("mode-r15");
  mode_r15.to_json(j);
  if (reestablish_rlc_r15_present) {
    j.write_str("reestablishRLC-r15", "true");
  }
  if (rlc_out_of_order_delivery_r15_present) {
    j.write_str("rlc-OutOfOrderDelivery-r15", "true");
  }
  j.end_obj();
}
bool rlc_cfg_r15_s::operator==(const rlc_cfg_r15_s& other) const
{
  return ext == other.ext and mode_r15 == other.mode_r15 and
         reestablish_rlc_r15_present == other.reestablish_rlc_r15_present and
         rlc_out_of_order_delivery_r15_present == other.rlc_out_of_order_delivery_r15_present;
}

void rlc_cfg_r15_s::mode_r15_c_::destroy_()
{
  switch (type_) {
    case types::am_r15:
      c.destroy<am_r15_s_>();
      break;
    case types::um_bi_dir_r15:
      c.destroy<um_bi_dir_r15_s_>();
      break;
    case types::um_uni_dir_ul_r15:
      c.destroy<um_uni_dir_ul_r15_s_>();
      break;
    case types::um_uni_dir_dl_r15:
      c.destroy<um_uni_dir_dl_r15_s_>();
      break;
    default:
      break;
  }
}
void rlc_cfg_r15_s::mode_r15_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::am_r15:
      c.init<am_r15_s_>();
      break;
    case types::um_bi_dir_r15:
      c.init<um_bi_dir_r15_s_>();
      break;
    case types::um_uni_dir_ul_r15:
      c.init<um_uni_dir_ul_r15_s_>();
      break;
    case types::um_uni_dir_dl_r15:
      c.init<um_uni_dir_dl_r15_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rlc_cfg_r15_s::mode_r15_c_");
  }
}
rlc_cfg_r15_s::mode_r15_c_::mode_r15_c_(const rlc_cfg_r15_s::mode_r15_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::am_r15:
      c.init(other.c.get<am_r15_s_>());
      break;
    case types::um_bi_dir_r15:
      c.init(other.c.get<um_bi_dir_r15_s_>());
      break;
    case types::um_uni_dir_ul_r15:
      c.init(other.c.get<um_uni_dir_ul_r15_s_>());
      break;
    case types::um_uni_dir_dl_r15:
      c.init(other.c.get<um_uni_dir_dl_r15_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rlc_cfg_r15_s::mode_r15_c_");
  }
}
rlc_cfg_r15_s::mode_r15_c_& rlc_cfg_r15_s::mode_r15_c_::operator=(const rlc_cfg_r15_s::mode_r15_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::am_r15:
      c.set(other.c.get<am_r15_s_>());
      break;
    case types::um_bi_dir_r15:
      c.set(other.c.get<um_bi_dir_r15_s_>());
      break;
    case types::um_uni_dir_ul_r15:
      c.set(other.c.get<um_uni_dir_ul_r15_s_>());
      break;
    case types::um_uni_dir_dl_r15:
      c.set(other.c.get<um_uni_dir_dl_r15_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rlc_cfg_r15_s::mode_r15_c_");
  }

  return *this;
}
rlc_cfg_r15_s::mode_r15_c_::am_r15_s_& rlc_cfg_r15_s::mode_r15_c_::set_am_r15()
{
  set(types::am_r15);
  return c.get<am_r15_s_>();
}
rlc_cfg_r15_s::mode_r15_c_::um_bi_dir_r15_s_& rlc_cfg_r15_s::mode_r15_c_::set_um_bi_dir_r15()
{
  set(types::um_bi_dir_r15);
  return c.get<um_bi_dir_r15_s_>();
}
rlc_cfg_r15_s::mode_r15_c_::um_uni_dir_ul_r15_s_& rlc_cfg_r15_s::mode_r15_c_::set_um_uni_dir_ul_r15()
{
  set(types::um_uni_dir_ul_r15);
  return c.get<um_uni_dir_ul_r15_s_>();
}
rlc_cfg_r15_s::mode_r15_c_::um_uni_dir_dl_r15_s_& rlc_cfg_r15_s::mode_r15_c_::set_um_uni_dir_dl_r15()
{
  set(types::um_uni_dir_dl_r15);
  return c.get<um_uni_dir_dl_r15_s_>();
}
void rlc_cfg_r15_s::mode_r15_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::am_r15:
      j.write_fieldname("am-r15");
      j.start_obj();
      j.write_fieldname("ul-AM-RLC-r15");
      c.get<am_r15_s_>().ul_am_rlc_r15.to_json(j);
      j.write_fieldname("dl-AM-RLC-r15");
      c.get<am_r15_s_>().dl_am_rlc_r15.to_json(j);
      j.end_obj();
      break;
    case types::um_bi_dir_r15:
      j.write_fieldname("um-Bi-Directional-r15");
      j.start_obj();
      j.write_fieldname("ul-UM-RLC-r15");
      c.get<um_bi_dir_r15_s_>().ul_um_rlc_r15.to_json(j);
      j.write_fieldname("dl-UM-RLC-r15");
      c.get<um_bi_dir_r15_s_>().dl_um_rlc_r15.to_json(j);
      j.end_obj();
      break;
    case types::um_uni_dir_ul_r15:
      j.write_fieldname("um-Uni-Directional-UL-r15");
      j.start_obj();
      j.write_fieldname("ul-UM-RLC-r15");
      c.get<um_uni_dir_ul_r15_s_>().ul_um_rlc_r15.to_json(j);
      j.end_obj();
      break;
    case types::um_uni_dir_dl_r15:
      j.write_fieldname("um-Uni-Directional-DL-r15");
      j.start_obj();
      j.write_fieldname("dl-UM-RLC-r15");
      c.get<um_uni_dir_dl_r15_s_>().dl_um_rlc_r15.to_json(j);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "rlc_cfg_r15_s::mode_r15_c_");
  }
  j.end_obj();
}
SRSASN_CODE rlc_cfg_r15_s::mode_r15_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::am_r15:
      HANDLE_CODE(c.get<am_r15_s_>().ul_am_rlc_r15.pack(bref));
      HANDLE_CODE(c.get<am_r15_s_>().dl_am_rlc_r15.pack(bref));
      break;
    case types::um_bi_dir_r15:
      HANDLE_CODE(c.get<um_bi_dir_r15_s_>().ul_um_rlc_r15.pack(bref));
      HANDLE_CODE(c.get<um_bi_dir_r15_s_>().dl_um_rlc_r15.pack(bref));
      break;
    case types::um_uni_dir_ul_r15:
      HANDLE_CODE(c.get<um_uni_dir_ul_r15_s_>().ul_um_rlc_r15.pack(bref));
      break;
    case types::um_uni_dir_dl_r15:
      HANDLE_CODE(c.get<um_uni_dir_dl_r15_s_>().dl_um_rlc_r15.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rlc_cfg_r15_s::mode_r15_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rlc_cfg_r15_s::mode_r15_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::am_r15:
      HANDLE_CODE(c.get<am_r15_s_>().ul_am_rlc_r15.unpack(bref));
      HANDLE_CODE(c.get<am_r15_s_>().dl_am_rlc_r15.unpack(bref));
      break;
    case types::um_bi_dir_r15:
      HANDLE_CODE(c.get<um_bi_dir_r15_s_>().ul_um_rlc_r15.unpack(bref));
      HANDLE_CODE(c.get<um_bi_dir_r15_s_>().dl_um_rlc_r15.unpack(bref));
      break;
    case types::um_uni_dir_ul_r15:
      HANDLE_CODE(c.get<um_uni_dir_ul_r15_s_>().ul_um_rlc_r15.unpack(bref));
      break;
    case types::um_uni_dir_dl_r15:
      HANDLE_CODE(c.get<um_uni_dir_dl_r15_s_>().dl_um_rlc_r15.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rlc_cfg_r15_s::mode_r15_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool rlc_cfg_r15_s::mode_r15_c_::operator==(const mode_r15_c_& other) const
{
  if (type_ != other.type_) {
    return false;
  }
  switch (type_) {
    case types::am_r15:
      return c.get<am_r15_s_>().ul_am_rlc_r15 == other.c.get<am_r15_s_>().ul_am_rlc_r15 and
             c.get<am_r15_s_>().dl_am_rlc_r15 == other.c.get<am_r15_s_>().dl_am_rlc_r15;
    case types::um_bi_dir_r15:
      return c.get<um_bi_dir_r15_s_>().ul_um_rlc_r15 == other.c.get<um_bi_dir_r15_s_>().ul_um_rlc_r15 and
             c.get<um_bi_dir_r15_s_>().dl_um_rlc_r15 == other.c.get<um_bi_dir_r15_s_>().dl_um_rlc_r15;
    case types::um_uni_dir_ul_r15:
      return c.get<um_uni_dir_ul_r15_s_>().ul_um_rlc_r15 == other.c.get<um_uni_dir_ul_r15_s_>().ul_um_rlc_r15;
    case types::um_uni_dir_dl_r15:
      return c.get<um_uni_dir_dl_r15_s_>().dl_um_rlc_r15 == other.c.get<um_uni_dir_dl_r15_s_>().dl_um_rlc_r15;
    default:
      return true;
  }
  return true;
}

// T-ReorderingExt-r17 ::= ENUMERATED
const char* t_reordering_ext_r17_opts::to_string() const
{
  static const char* options[] = {"ms2200", "ms3200"};
  return convert_enum_idx(options, 2, value, "t_reordering_ext_r17_e");
}
uint16_t t_reordering_ext_r17_opts::to_number() const
{
  static const uint16_t options[] = {2200, 3200};
  return map_enum_number(options, 2, value, "t_reordering_ext_r17_e");
}

// UL-AM-RLC ::= SEQUENCE
SRSASN_CODE ul_am_rlc_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(t_poll_retx.pack(bref));
  HANDLE_CODE(poll_pdu.pack(bref));
  HANDLE_CODE(poll_byte.pack(bref));
  HANDLE_CODE(max_retx_thres.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_am_rlc_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(t_poll_retx.unpack(bref));
  HANDLE_CODE(poll_pdu.unpack(bref));
  HANDLE_CODE(poll_byte.unpack(bref));
  HANDLE_CODE(max_retx_thres.unpack(bref));

  return SRSASN_SUCCESS;
}
void ul_am_rlc_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("t-PollRetransmit", t_poll_retx.to_string());
  j.write_str("pollPDU", poll_pdu.to_string());
  j.write_str("pollByte", poll_byte.to_string());
  j.write_str("maxRetxThreshold", max_retx_thres.to_string());
  j.end_obj();
}
bool ul_am_rlc_s::operator==(const ul_am_rlc_s& other) const
{
  return t_poll_retx == other.t_poll_retx and poll_pdu == other.poll_pdu and poll_byte == other.poll_byte and
         max_retx_thres == other.max_retx_thres;
}

const char* ul_am_rlc_s::max_retx_thres_opts::to_string() const
{
  static const char* options[] = {"t1", "t2", "t3", "t4", "t6", "t8", "t16", "t32"};
  return convert_enum_idx(options, 8, value, "ul_am_rlc_s::max_retx_thres_e_");
}
uint8_t ul_am_rlc_s::max_retx_thres_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 6, 8, 16, 32};
  return map_enum_number(options, 8, value, "ul_am_rlc_s::max_retx_thres_e_");
}

// CRS-AssistanceInfo-r11 ::= SEQUENCE
SRSASN_CODE crs_assist_info_r11_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, pci_r11, (uint16_t)0u, (uint16_t)503u));
  HANDLE_CODE(ant_ports_count_r11.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, mbsfn_sf_cfg_list_r11, 1, 8));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= mbsfn_sf_cfg_list_v1430.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(mbsfn_sf_cfg_list_v1430.is_present(), 1));
      if (mbsfn_sf_cfg_list_v1430.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *mbsfn_sf_cfg_list_v1430, 1, 8));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE crs_assist_info_r11_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(pci_r11, bref, (uint16_t)0u, (uint16_t)503u));
  HANDLE_CODE(ant_ports_count_r11.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(mbsfn_sf_cfg_list_r11, bref, 1, 8));

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool mbsfn_sf_cfg_list_v1430_present;
      HANDLE_CODE(bref.unpack(mbsfn_sf_cfg_list_v1430_present, 1));
      mbsfn_sf_cfg_list_v1430.set_present(mbsfn_sf_cfg_list_v1430_present);
      if (mbsfn_sf_cfg_list_v1430.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*mbsfn_sf_cfg_list_v1430, bref, 1, 8));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void crs_assist_info_r11_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("physCellId-r11", pci_r11);
  j.write_str("antennaPortsCount-r11", ant_ports_count_r11.to_string());
  j.start_array("mbsfn-SubframeConfigList-r11");
  for (const auto& e1 : mbsfn_sf_cfg_list_r11) {
    e1.to_json(j);
  }
  j.end_array();
  if (ext) {
    if (mbsfn_sf_cfg_list_v1430.is_present()) {
      j.start_array("mbsfn-SubframeConfigList-v1430");
      for (const auto& e1 : *mbsfn_sf_cfg_list_v1430) {
        e1.to_json(j);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

const char* crs_assist_info_r11_s::ant_ports_count_r11_opts::to_string() const
{
  static const char* options[] = {"an1", "an2", "an4", "spare1"};
  return convert_enum_idx(options, 4, value, "crs_assist_info_r11_s::ant_ports_count_r11_e_");
}
uint8_t crs_assist_info_r11_s::ant_ports_count_r11_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4};
  return map_enum_number(options, 3, value, "crs_assist_info_r11_s::ant_ports_count_r11_e_");
}

// CRS-AssistanceInfo-r13 ::= SEQUENCE
SRSASN_CODE crs_assist_info_r13_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(mbsfn_sf_cfg_list_r13_present, 1));

  HANDLE_CODE(pack_integer(bref, pci_r13, (uint16_t)0u, (uint16_t)503u));
  HANDLE_CODE(ant_ports_count_r13.pack(bref));
  if (mbsfn_sf_cfg_list_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, mbsfn_sf_cfg_list_r13, 1, 8));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= mbsfn_sf_cfg_list_v1430.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(mbsfn_sf_cfg_list_v1430.is_present(), 1));
      if (mbsfn_sf_cfg_list_v1430.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *mbsfn_sf_cfg_list_v1430, 1, 8));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE crs_assist_info_r13_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(mbsfn_sf_cfg_list_r13_present, 1));

  HANDLE_CODE(unpack_integer(pci_r13, bref, (uint16_t)0u, (uint16_t)503u));
  HANDLE_CODE(ant_ports_count_r13.unpack(bref));
  if (mbsfn_sf_cfg_list_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(mbsfn_sf_cfg_list_r13, bref, 1, 8));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool mbsfn_sf_cfg_list_v1430_present;
      HANDLE_CODE(bref.unpack(mbsfn_sf_cfg_list_v1430_present, 1));
      mbsfn_sf_cfg_list_v1430.set_present(mbsfn_sf_cfg_list_v1430_present);
      if (mbsfn_sf_cfg_list_v1430.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*mbsfn_sf_cfg_list_v1430, bref, 1, 8));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void crs_assist_info_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("physCellId-r13", pci_r13);
  j.write_str("antennaPortsCount-r13", ant_ports_count_r13.to_string());
  if (mbsfn_sf_cfg_list_r13_present) {
    j.start_array("mbsfn-SubframeConfigList-r13");
    for (const auto& e1 : mbsfn_sf_cfg_list_r13) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ext) {
    if (mbsfn_sf_cfg_list_v1430.is_present()) {
      j.start_array("mbsfn-SubframeConfigList-v1430");
      for (const auto& e1 : *mbsfn_sf_cfg_list_v1430) {
        e1.to_json(j);
      }
      j.end_array();
    }
  }
  j.end_obj();
}
bool crs_assist_info_r13_s::operator==(const crs_assist_info_r13_s& other) const
{
  return ext == other.ext and pci_r13 == other.pci_r13 and ant_ports_count_r13 == other.ant_ports_count_r13 and
         mbsfn_sf_cfg_list_r13_present == other.mbsfn_sf_cfg_list_r13_present and
         (not mbsfn_sf_cfg_list_r13_present or mbsfn_sf_cfg_list_r13 == other.mbsfn_sf_cfg_list_r13) and
         (not ext or
          (mbsfn_sf_cfg_list_v1430.is_present() == other.mbsfn_sf_cfg_list_v1430.is_present() and
           (not mbsfn_sf_cfg_list_v1430.is_present() or *mbsfn_sf_cfg_list_v1430 == *other.mbsfn_sf_cfg_list_v1430)));
}

const char* crs_assist_info_r13_s::ant_ports_count_r13_opts::to_string() const
{
  static const char* options[] = {"an1", "an2", "an4", "spare1"};
  return convert_enum_idx(options, 4, value, "crs_assist_info_r13_s::ant_ports_count_r13_e_");
}
uint8_t crs_assist_info_r13_s::ant_ports_count_r13_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4};
  return map_enum_number(options, 3, value, "crs_assist_info_r13_s::ant_ports_count_r13_e_");
}

// CRS-AssistanceInfo-r15 ::= SEQUENCE
SRSASN_CODE crs_assist_info_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(crs_intf_mitig_enabled_r15_present, 1));

  HANDLE_CODE(pack_integer(bref, pci_r15, (uint16_t)0u, (uint16_t)503u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE crs_assist_info_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(crs_intf_mitig_enabled_r15_present, 1));

  HANDLE_CODE(unpack_integer(pci_r15, bref, (uint16_t)0u, (uint16_t)503u));

  return SRSASN_SUCCESS;
}
void crs_assist_info_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("physCellId-r15", pci_r15);
  if (crs_intf_mitig_enabled_r15_present) {
    j.write_str("crs-IntfMitigEnabled-r15", "enabled");
  }
  j.end_obj();
}
bool crs_assist_info_r15_s::operator==(const crs_assist_info_r15_s& other) const
{
  return pci_r15 == other.pci_r15 and crs_intf_mitig_enabled_r15_present == other.crs_intf_mitig_enabled_r15_present;
}

// NeighCellsInfo-r12 ::= SEQUENCE
SRSASN_CODE neigh_cells_info_r12_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(mbsfn_sf_cfg_r12_present, 1));

  HANDLE_CODE(pack_integer(bref, pci_r12, (uint16_t)0u, (uint16_t)503u));
  HANDLE_CODE(pack_integer(bref, p_b_r12, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crs_ports_count_r12.pack(bref));
  if (mbsfn_sf_cfg_r12_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, mbsfn_sf_cfg_r12, 1, 8));
  }
  HANDLE_CODE(pack_dyn_seq_of(bref, p_a_list_r12, 1, 3));
  HANDLE_CODE(tx_mode_list_r12.pack(bref));
  HANDLE_CODE(pack_integer(bref, res_alloc_granularity_r12, (uint8_t)1u, (uint8_t)4u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE neigh_cells_info_r12_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(mbsfn_sf_cfg_r12_present, 1));

  HANDLE_CODE(unpack_integer(pci_r12, bref, (uint16_t)0u, (uint16_t)503u));
  HANDLE_CODE(unpack_integer(p_b_r12, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(crs_ports_count_r12.unpack(bref));
  if (mbsfn_sf_cfg_r12_present) {
    HANDLE_CODE(unpack_dyn_seq_of(mbsfn_sf_cfg_r12, bref, 1, 8));
  }
  HANDLE_CODE(unpack_dyn_seq_of(p_a_list_r12, bref, 1, 3));
  HANDLE_CODE(tx_mode_list_r12.unpack(bref));
  HANDLE_CODE(unpack_integer(res_alloc_granularity_r12, bref, (uint8_t)1u, (uint8_t)4u));

  return SRSASN_SUCCESS;
}
void neigh_cells_info_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("physCellId-r12", pci_r12);
  j.write_int("p-b-r12", p_b_r12);
  j.write_str("crs-PortsCount-r12", crs_ports_count_r12.to_string());
  if (mbsfn_sf_cfg_r12_present) {
    j.start_array("mbsfn-SubframeConfig-r12");
    for (const auto& e1 : mbsfn_sf_cfg_r12) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.start_array("p-aList-r12");
  for (const auto& e1 : p_a_list_r12) {
    j.write_str(e1.to_string());
  }
  j.end_array();
  j.write_str("transmissionModeList-r12", tx_mode_list_r12.to_string());
  j.write_int("resAllocGranularity-r12", res_alloc_granularity_r12);
  j.end_obj();
}
bool neigh_cells_info_r12_s::operator==(const neigh_cells_info_r12_s& other) const
{
  return ext == other.ext and pci_r12 == other.pci_r12 and p_b_r12 == other.p_b_r12 and
         crs_ports_count_r12 == other.crs_ports_count_r12 and
         mbsfn_sf_cfg_r12_present == other.mbsfn_sf_cfg_r12_present and
         (not mbsfn_sf_cfg_r12_present or mbsfn_sf_cfg_r12 == other.mbsfn_sf_cfg_r12) and
         p_a_list_r12 == other.p_a_list_r12 and tx_mode_list_r12 == other.tx_mode_list_r12 and
         res_alloc_granularity_r12 == other.res_alloc_granularity_r12;
}

const char* neigh_cells_info_r12_s::crs_ports_count_r12_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n4", "spare"};
  return convert_enum_idx(options, 4, value, "neigh_cells_info_r12_s::crs_ports_count_r12_e_");
}
uint8_t neigh_cells_info_r12_s::crs_ports_count_r12_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4};
  return map_enum_number(options, 3, value, "neigh_cells_info_r12_s::crs_ports_count_r12_e_");
}

// PDCP-Config ::= SEQUENCE
SRSASN_CODE pdcp_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(discard_timer_present, 1));
  HANDLE_CODE(bref.pack(rlc_am_present, 1));
  HANDLE_CODE(bref.pack(rlc_um_present, 1));

  if (discard_timer_present) {
    HANDLE_CODE(discard_timer.pack(bref));
  }
  if (rlc_am_present) {
    HANDLE_CODE(bref.pack(rlc_am.status_report_required, 1));
  }
  if (rlc_um_present) {
    HANDLE_CODE(rlc_um.pdcp_sn_size.pack(bref));
  }
  HANDLE_CODE(hdr_compress.pack(bref));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= rn_integrity_protection_r10_present;
    group_flags[1] |= pdcp_sn_size_v1130_present;
    group_flags[2] |= ul_data_split_drb_via_scg_r12_present;
    group_flags[2] |= t_reordering_r12_present;
    group_flags[3] |= ul_data_split_thres_r13.is_present();
    group_flags[3] |= pdcp_sn_size_v1310_present;
    group_flags[3] |= status_feedback_r13.is_present();
    group_flags[4] |= ul_lwa_cfg_r14.is_present();
    group_flags[4] |= ul_only_hdr_compress_r14.is_present();
    group_flags[5] |= ul_data_compress_r15.is_present();
    group_flags[5] |= pdcp_dupl_cfg_r15.is_present();
    group_flags[6] |= ethernet_hdr_compress_r16.is_present();
    group_flags[7] |= discard_timer_ext_r17.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(rn_integrity_protection_r10_present, 1));
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(pdcp_sn_size_v1130_present, 1));
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ul_data_split_drb_via_scg_r12_present, 1));
      HANDLE_CODE(bref.pack(t_reordering_r12_present, 1));
      if (ul_data_split_drb_via_scg_r12_present) {
        HANDLE_CODE(bref.pack(ul_data_split_drb_via_scg_r12, 1));
      }
      if (t_reordering_r12_present) {
        HANDLE_CODE(t_reordering_r12.pack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ul_data_split_thres_r13.is_present(), 1));
      HANDLE_CODE(bref.pack(pdcp_sn_size_v1310_present, 1));
      HANDLE_CODE(bref.pack(status_feedback_r13.is_present(), 1));
      if (ul_data_split_thres_r13.is_present()) {
        HANDLE_CODE(ul_data_split_thres_r13->pack(bref));
      }
      if (status_feedback_r13.is_present()) {
        HANDLE_CODE(status_feedback_r13->pack(bref));
      }
    }
    if (group_flags[4]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ul_lwa_cfg_r14.is_present(), 1));
      HANDLE_CODE(bref.pack(ul_only_hdr_compress_r14.is_present(), 1));
      if (ul_lwa_cfg_r14.is_present()) {
        HANDLE_CODE(ul_lwa_cfg_r14->pack(bref));
      }
      if (ul_only_hdr_compress_r14.is_present()) {
        HANDLE_CODE(ul_only_hdr_compress_r14->pack(bref));
      }
    }
    if (group_flags[5]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ul_data_compress_r15.is_present(), 1));
      HANDLE_CODE(bref.pack(pdcp_dupl_cfg_r15.is_present(), 1));
      if (ul_data_compress_r15.is_present()) {
        bref.pack(ul_data_compress_r15->ext, 1);
        HANDLE_CODE(bref.pack(ul_data_compress_r15->dictionary_r15_present, 1));
        HANDLE_CODE(ul_data_compress_r15->buffer_size_r15.pack(bref));
        if (ul_data_compress_r15->dictionary_r15_present) {
          HANDLE_CODE(ul_data_compress_r15->dictionary_r15.pack(bref));
        }
      }
      if (pdcp_dupl_cfg_r15.is_present()) {
        HANDLE_CODE(pdcp_dupl_cfg_r15->pack(bref));
      }
    }
    if (group_flags[6]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ethernet_hdr_compress_r16.is_present(), 1));
      if (ethernet_hdr_compress_r16.is_present()) {
        HANDLE_CODE(ethernet_hdr_compress_r16->pack(bref));
      }
    }
    if (group_flags[7]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(discard_timer_ext_r17.is_present(), 1));
      if (discard_timer_ext_r17.is_present()) {
        HANDLE_CODE(discard_timer_ext_r17->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pdcp_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(discard_timer_present, 1));
  HANDLE_CODE(bref.unpack(rlc_am_present, 1));
  HANDLE_CODE(bref.unpack(rlc_um_present, 1));

  if (discard_timer_present) {
    HANDLE_CODE(discard_timer.unpack(bref));
  }
  if (rlc_am_present) {
    HANDLE_CODE(bref.unpack(rlc_am.status_report_required, 1));
  }
  if (rlc_um_present) {
    HANDLE_CODE(rlc_um.pdcp_sn_size.unpack(bref));
  }
  HANDLE_CODE(hdr_compress.unpack(bref));

  if (ext) {
    ext_groups_unpacker_guard group_flags(8);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(rn_integrity_protection_r10_present, 1));
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(pdcp_sn_size_v1130_present, 1));
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(ul_data_split_drb_via_scg_r12_present, 1));
      HANDLE_CODE(bref.unpack(t_reordering_r12_present, 1));
      if (ul_data_split_drb_via_scg_r12_present) {
        HANDLE_CODE(bref.unpack(ul_data_split_drb_via_scg_r12, 1));
      }
      if (t_reordering_r12_present) {
        HANDLE_CODE(t_reordering_r12.unpack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool ul_data_split_thres_r13_present;
      HANDLE_CODE(bref.unpack(ul_data_split_thres_r13_present, 1));
      ul_data_split_thres_r13.set_present(ul_data_split_thres_r13_present);
      HANDLE_CODE(bref.unpack(pdcp_sn_size_v1310_present, 1));
      bool status_feedback_r13_present;
      HANDLE_CODE(bref.unpack(status_feedback_r13_present, 1));
      status_feedback_r13.set_present(status_feedback_r13_present);
      if (ul_data_split_thres_r13.is_present()) {
        HANDLE_CODE(ul_data_split_thres_r13->unpack(bref));
      }
      if (status_feedback_r13.is_present()) {
        HANDLE_CODE(status_feedback_r13->unpack(bref));
      }
    }
    if (group_flags[4]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool ul_lwa_cfg_r14_present;
      HANDLE_CODE(bref.unpack(ul_lwa_cfg_r14_present, 1));
      ul_lwa_cfg_r14.set_present(ul_lwa_cfg_r14_present);
      bool ul_only_hdr_compress_r14_present;
      HANDLE_CODE(bref.unpack(ul_only_hdr_compress_r14_present, 1));
      ul_only_hdr_compress_r14.set_present(ul_only_hdr_compress_r14_present);
      if (ul_lwa_cfg_r14.is_present()) {
        HANDLE_CODE(ul_lwa_cfg_r14->unpack(bref));
      }
      if (ul_only_hdr_compress_r14.is_present()) {
        HANDLE_CODE(ul_only_hdr_compress_r14->unpack(bref));
      }
    }
    if (group_flags[5]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool ul_data_compress_r15_present;
      HANDLE_CODE(bref.unpack(ul_data_compress_r15_present, 1));
      ul_data_compress_r15.set_present(ul_data_compress_r15_present);
      bool pdcp_dupl_cfg_r15_present;
      HANDLE_CODE(bref.unpack(pdcp_dupl_cfg_r15_present, 1));
      pdcp_dupl_cfg_r15.set_present(pdcp_dupl_cfg_r15_present);
      if (ul_data_compress_r15.is_present()) {
        bref.unpack(ul_data_compress_r15->ext, 1);
        HANDLE_CODE(bref.unpack(ul_data_compress_r15->dictionary_r15_present, 1));
        HANDLE_CODE(ul_data_compress_r15->buffer_size_r15.unpack(bref));
        if (ul_data_compress_r15->dictionary_r15_present) {
          HANDLE_CODE(ul_data_compress_r15->dictionary_r15.unpack(bref));
        }
      }
      if (pdcp_dupl_cfg_r15.is_present()) {
        HANDLE_CODE(pdcp_dupl_cfg_r15->unpack(bref));
      }
    }
    if (group_flags[6]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool ethernet_hdr_compress_r16_present;
      HANDLE_CODE(bref.unpack(ethernet_hdr_compress_r16_present, 1));
      ethernet_hdr_compress_r16.set_present(ethernet_hdr_compress_r16_present);
      if (ethernet_hdr_compress_r16.is_present()) {
        HANDLE_CODE(ethernet_hdr_compress_r16->unpack(bref));
      }
    }
    if (group_flags[7]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool discard_timer_ext_r17_present;
      HANDLE_CODE(bref.unpack(discard_timer_ext_r17_present, 1));
      discard_timer_ext_r17.set_present(discard_timer_ext_r17_present);
      if (discard_timer_ext_r17.is_present()) {
        HANDLE_CODE(discard_timer_ext_r17->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void pdcp_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (discard_timer_present) {
    j.write_str("discardTimer", discard_timer.to_string());
  }
  if (rlc_am_present) {
    j.write_fieldname("rlc-AM");
    j.start_obj();
    j.write_bool("statusReportRequired", rlc_am.status_report_required);
    j.end_obj();
  }
  if (rlc_um_present) {
    j.write_fieldname("rlc-UM");
    j.start_obj();
    j.write_str("pdcp-SN-Size", rlc_um.pdcp_sn_size.to_string());
    j.end_obj();
  }
  j.write_fieldname("headerCompression");
  hdr_compress.to_json(j);
  if (ext) {
    if (rn_integrity_protection_r10_present) {
      j.write_str("rn-IntegrityProtection-r10", "enabled");
    }
    if (pdcp_sn_size_v1130_present) {
      j.write_str("pdcp-SN-Size-v1130", "len15bits");
    }
    if (ul_data_split_drb_via_scg_r12_present) {
      j.write_bool("ul-DataSplitDRB-ViaSCG-r12", ul_data_split_drb_via_scg_r12);
    }
    if (t_reordering_r12_present) {
      j.write_str("t-Reordering-r12", t_reordering_r12.to_string());
    }
    if (ul_data_split_thres_r13.is_present()) {
      j.write_fieldname("ul-DataSplitThreshold-r13");
      ul_data_split_thres_r13->to_json(j);
    }
    if (pdcp_sn_size_v1310_present) {
      j.write_str("pdcp-SN-Size-v1310", "len18bits");
    }
    if (status_feedback_r13.is_present()) {
      j.write_fieldname("statusFeedback-r13");
      status_feedback_r13->to_json(j);
    }
    if (ul_lwa_cfg_r14.is_present()) {
      j.write_fieldname("ul-LWA-Config-r14");
      ul_lwa_cfg_r14->to_json(j);
    }
    if (ul_only_hdr_compress_r14.is_present()) {
      j.write_fieldname("uplinkOnlyHeaderCompression-r14");
      ul_only_hdr_compress_r14->to_json(j);
    }
    if (ul_data_compress_r15.is_present()) {
      j.write_fieldname("uplinkDataCompression-r15");
      j.start_obj();
      j.write_str("bufferSize-r15", ul_data_compress_r15->buffer_size_r15.to_string());
      if (ul_data_compress_r15->dictionary_r15_present) {
        j.write_str("dictionary-r15", ul_data_compress_r15->dictionary_r15.to_string());
      }
      j.end_obj();
    }
    if (pdcp_dupl_cfg_r15.is_present()) {
      j.write_fieldname("pdcp-DuplicationConfig-r15");
      pdcp_dupl_cfg_r15->to_json(j);
    }
    if (ethernet_hdr_compress_r16.is_present()) {
      j.write_fieldname("ethernetHeaderCompression-r16");
      ethernet_hdr_compress_r16->to_json(j);
    }
    if (discard_timer_ext_r17.is_present()) {
      j.write_fieldname("discardTimerExt-r17");
      discard_timer_ext_r17->to_json(j);
    }
  }
  j.end_obj();
}
bool pdcp_cfg_s::operator==(const pdcp_cfg_s& other) const
{
  return ext == other.ext and discard_timer_present == other.discard_timer_present and
         (not discard_timer_present or discard_timer == other.discard_timer) and
         rlc_am.status_report_required == other.rlc_am.status_report_required and
         rlc_um.pdcp_sn_size == other.rlc_um.pdcp_sn_size and hdr_compress == other.hdr_compress and
         (not ext or
          (rn_integrity_protection_r10_present == other.rn_integrity_protection_r10_present and
           pdcp_sn_size_v1130_present == other.pdcp_sn_size_v1130_present and
           ul_data_split_drb_via_scg_r12_present == other.ul_data_split_drb_via_scg_r12_present and
           (not ul_data_split_drb_via_scg_r12_present or
            ul_data_split_drb_via_scg_r12 == other.ul_data_split_drb_via_scg_r12) and
           t_reordering_r12_present == other.t_reordering_r12_present and
           (not t_reordering_r12_present or t_reordering_r12 == other.t_reordering_r12) and
           ul_data_split_thres_r13.is_present() == other.ul_data_split_thres_r13.is_present() and
           (not ul_data_split_thres_r13.is_present() or *ul_data_split_thres_r13 == *other.ul_data_split_thres_r13) and
           pdcp_sn_size_v1310_present == other.pdcp_sn_size_v1310_present and
           status_feedback_r13.is_present() == other.status_feedback_r13.is_present() and
           (not status_feedback_r13.is_present() or *status_feedback_r13 == *other.status_feedback_r13) and
           ul_lwa_cfg_r14.is_present() == other.ul_lwa_cfg_r14.is_present() and
           (not ul_lwa_cfg_r14.is_present() or *ul_lwa_cfg_r14 == *other.ul_lwa_cfg_r14) and
           ul_only_hdr_compress_r14.is_present() == other.ul_only_hdr_compress_r14.is_present() and
           (not ul_only_hdr_compress_r14.is_present() or
            *ul_only_hdr_compress_r14 == *other.ul_only_hdr_compress_r14) and
           ul_data_compress_r15->ext == other.ul_data_compress_r15->ext and
           ul_data_compress_r15->buffer_size_r15 == other.ul_data_compress_r15->buffer_size_r15 and
           ul_data_compress_r15->dictionary_r15_present == other.ul_data_compress_r15->dictionary_r15_present and
           (not ul_data_compress_r15->dictionary_r15_present or
            ul_data_compress_r15->dictionary_r15 == other.ul_data_compress_r15->dictionary_r15) and
           pdcp_dupl_cfg_r15.is_present() == other.pdcp_dupl_cfg_r15.is_present() and
           (not pdcp_dupl_cfg_r15.is_present() or *pdcp_dupl_cfg_r15 == *other.pdcp_dupl_cfg_r15) and
           ethernet_hdr_compress_r16.is_present() == other.ethernet_hdr_compress_r16.is_present() and
           (not ethernet_hdr_compress_r16.is_present() or
            *ethernet_hdr_compress_r16 == *other.ethernet_hdr_compress_r16) and
           discard_timer_ext_r17.is_present() == other.discard_timer_ext_r17.is_present() and
           (not discard_timer_ext_r17.is_present() or *discard_timer_ext_r17 == *other.discard_timer_ext_r17)));
}

const char* pdcp_cfg_s::discard_timer_opts::to_string() const
{
  static const char* options[] = {"ms50", "ms100", "ms150", "ms300", "ms500", "ms750", "ms1500", "infinity"};
  return convert_enum_idx(options, 8, value, "pdcp_cfg_s::discard_timer_e_");
}
int16_t pdcp_cfg_s::discard_timer_opts::to_number() const
{
  static const int16_t options[] = {50, 100, 150, 300, 500, 750, 1500, -1};
  return map_enum_number(options, 8, value, "pdcp_cfg_s::discard_timer_e_");
}

const char* pdcp_cfg_s::rlc_um_s_::pdcp_sn_size_opts::to_string() const
{
  static const char* options[] = {"len7bits", "len12bits"};
  return convert_enum_idx(options, 2, value, "pdcp_cfg_s::rlc_um_s_::pdcp_sn_size_e_");
}
uint8_t pdcp_cfg_s::rlc_um_s_::pdcp_sn_size_opts::to_number() const
{
  static const uint8_t options[] = {7, 12};
  return map_enum_number(options, 2, value, "pdcp_cfg_s::rlc_um_s_::pdcp_sn_size_e_");
}

void pdcp_cfg_s::hdr_compress_c_::set(types::options e)
{
  type_ = e;
}
void pdcp_cfg_s::hdr_compress_c_::set_not_used()
{
  set(types::not_used);
}
pdcp_cfg_s::hdr_compress_c_::rohc_s_& pdcp_cfg_s::hdr_compress_c_::set_rohc()
{
  set(types::rohc);
  return c;
}
void pdcp_cfg_s::hdr_compress_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::not_used:
      break;
    case types::rohc:
      j.write_fieldname("rohc");
      j.start_obj();
      if (c.max_cid_present) {
        j.write_int("maxCID", c.max_cid);
      }
      j.write_fieldname("profiles");
      j.start_obj();
      j.write_bool("profile0x0001", c.profiles.profile0x0001);
      j.write_bool("profile0x0002", c.profiles.profile0x0002);
      j.write_bool("profile0x0003", c.profiles.profile0x0003);
      j.write_bool("profile0x0004", c.profiles.profile0x0004);
      j.write_bool("profile0x0006", c.profiles.profile0x0006);
      j.write_bool("profile0x0101", c.profiles.profile0x0101);
      j.write_bool("profile0x0102", c.profiles.profile0x0102);
      j.write_bool("profile0x0103", c.profiles.profile0x0103);
      j.write_bool("profile0x0104", c.profiles.profile0x0104);
      j.end_obj();
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "pdcp_cfg_s::hdr_compress_c_");
  }
  j.end_obj();
}
SRSASN_CODE pdcp_cfg_s::hdr_compress_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::not_used:
      break;
    case types::rohc:
      bref.pack(c.ext, 1);
      HANDLE_CODE(bref.pack(c.max_cid_present, 1));
      if (c.max_cid_present) {
        HANDLE_CODE(pack_integer(bref, c.max_cid, (uint16_t)1u, (uint16_t)16383u));
      }
      HANDLE_CODE(bref.pack(c.profiles.profile0x0001, 1));
      HANDLE_CODE(bref.pack(c.profiles.profile0x0002, 1));
      HANDLE_CODE(bref.pack(c.profiles.profile0x0003, 1));
      HANDLE_CODE(bref.pack(c.profiles.profile0x0004, 1));
      HANDLE_CODE(bref.pack(c.profiles.profile0x0006, 1));
      HANDLE_CODE(bref.pack(c.profiles.profile0x0101, 1));
      HANDLE_CODE(bref.pack(c.profiles.profile0x0102, 1));
      HANDLE_CODE(bref.pack(c.profiles.profile0x0103, 1));
      HANDLE_CODE(bref.pack(c.profiles.profile0x0104, 1));
      break;
    default:
      log_invalid_choice_id(type_, "pdcp_cfg_s::hdr_compress_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pdcp_cfg_s::hdr_compress_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::not_used:
      break;
    case types::rohc:
      bref.unpack(c.ext, 1);
      HANDLE_CODE(bref.unpack(c.max_cid_present, 1));
      if (c.max_cid_present) {
        HANDLE_CODE(unpack_integer(c.max_cid, bref, (uint16_t)1u, (uint16_t)16383u));
      }
      HANDLE_CODE(bref.unpack(c.profiles.profile0x0001, 1));
      HANDLE_CODE(bref.unpack(c.profiles.profile0x0002, 1));
      HANDLE_CODE(bref.unpack(c.profiles.profile0x0003, 1));
      HANDLE_CODE(bref.unpack(c.profiles.profile0x0004, 1));
      HANDLE_CODE(bref.unpack(c.profiles.profile0x0006, 1));
      HANDLE_CODE(bref.unpack(c.profiles.profile0x0101, 1));
      HANDLE_CODE(bref.unpack(c.profiles.profile0x0102, 1));
      HANDLE_CODE(bref.unpack(c.profiles.profile0x0103, 1));
      HANDLE_CODE(bref.unpack(c.profiles.profile0x0104, 1));
      break;
    default:
      log_invalid_choice_id(type_, "pdcp_cfg_s::hdr_compress_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool pdcp_cfg_s::hdr_compress_c_::operator==(const hdr_compress_c_& other) const
{
  return type() == other.type() and c.ext == other.c.ext and c.max_cid_present == other.c.max_cid_present and
         (not c.max_cid_present or c.max_cid == other.c.max_cid) and
         c.profiles.profile0x0001 == other.c.profiles.profile0x0001 and
         c.profiles.profile0x0002 == other.c.profiles.profile0x0002 and
         c.profiles.profile0x0003 == other.c.profiles.profile0x0003 and
         c.profiles.profile0x0004 == other.c.profiles.profile0x0004 and
         c.profiles.profile0x0006 == other.c.profiles.profile0x0006 and
         c.profiles.profile0x0101 == other.c.profiles.profile0x0101 and
         c.profiles.profile0x0102 == other.c.profiles.profile0x0102 and
         c.profiles.profile0x0103 == other.c.profiles.profile0x0103 and
         c.profiles.profile0x0104 == other.c.profiles.profile0x0104;
}

const char* pdcp_cfg_s::t_reordering_r12_opts::to_string() const
{
  static const char* options[] = {"ms0",    "ms20",   "ms40",    "ms60",    "ms80",    "ms100",   "ms120",   "ms140",
                                  "ms160",  "ms180",  "ms200",   "ms220",   "ms240",   "ms260",   "ms280",   "ms300",
                                  "ms500",  "ms750",  "spare14", "spare13", "spare12", "spare11", "spare10", "spare9",
                                  "spare8", "spare7", "spare6",  "spare5",  "spare4",  "spare3",  "spare2",  "spare1"};
  return convert_enum_idx(options, 32, value, "pdcp_cfg_s::t_reordering_r12_e_");
}
uint16_t pdcp_cfg_s::t_reordering_r12_opts::to_number() const
{
  static const uint16_t options[] = {
      0, 20, 40, 60, 80, 100, 120, 140, 160, 180, 200, 220, 240, 260, 280, 300, 500, 750};
  return map_enum_number(options, 18, value, "pdcp_cfg_s::t_reordering_r12_e_");
}

void pdcp_cfg_s::ul_data_split_thres_r13_c_::set(types::options e)
{
  type_ = e;
}
void pdcp_cfg_s::ul_data_split_thres_r13_c_::set_release()
{
  set(types::release);
}
pdcp_cfg_s::ul_data_split_thres_r13_c_::setup_e_& pdcp_cfg_s::ul_data_split_thres_r13_c_::set_setup()
{
  set(types::setup);
  return c;
}
void pdcp_cfg_s::ul_data_split_thres_r13_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_str("setup", c.to_string());
      break;
    default:
      log_invalid_choice_id(type_, "pdcp_cfg_s::ul_data_split_thres_r13_c_");
  }
  j.end_obj();
}
SRSASN_CODE pdcp_cfg_s::ul_data_split_thres_r13_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "pdcp_cfg_s::ul_data_split_thres_r13_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pdcp_cfg_s::ul_data_split_thres_r13_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "pdcp_cfg_s::ul_data_split_thres_r13_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool pdcp_cfg_s::ul_data_split_thres_r13_c_::operator==(const ul_data_split_thres_r13_c_& other) const
{
  return type() == other.type() and c == other.c;
}

const char* pdcp_cfg_s::ul_data_split_thres_r13_c_::setup_opts::to_string() const
{
  static const char* options[] = {"b0",
                                  "b100",
                                  "b200",
                                  "b400",
                                  "b800",
                                  "b1600",
                                  "b3200",
                                  "b6400",
                                  "b12800",
                                  "b25600",
                                  "b51200",
                                  "b102400",
                                  "b204800",
                                  "b409600",
                                  "b819200",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "pdcp_cfg_s::ul_data_split_thres_r13_c_::setup_e_");
}
uint32_t pdcp_cfg_s::ul_data_split_thres_r13_c_::setup_opts::to_number() const
{
  static const uint32_t options[] = {
      0, 100, 200, 400, 800, 1600, 3200, 6400, 12800, 25600, 51200, 102400, 204800, 409600, 819200};
  return map_enum_number(options, 15, value, "pdcp_cfg_s::ul_data_split_thres_r13_c_::setup_e_");
}

void pdcp_cfg_s::status_feedback_r13_c_::set(types::options e)
{
  type_ = e;
}
void pdcp_cfg_s::status_feedback_r13_c_::set_release()
{
  set(types::release);
}
pdcp_cfg_s::status_feedback_r13_c_::setup_s_& pdcp_cfg_s::status_feedback_r13_c_::set_setup()
{
  set(types::setup);
  return c;
}
void pdcp_cfg_s::status_feedback_r13_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      if (c.status_pdu_type_for_polling_r13_present) {
        j.write_str("statusPDU-TypeForPolling-r13", c.status_pdu_type_for_polling_r13.to_string());
      }
      if (c.status_pdu_periodicity_type1_r13_present) {
        j.write_str("statusPDU-Periodicity-Type1-r13", c.status_pdu_periodicity_type1_r13.to_string());
      }
      if (c.status_pdu_periodicity_type2_r13_present) {
        j.write_str("statusPDU-Periodicity-Type2-r13", c.status_pdu_periodicity_type2_r13.to_string());
      }
      if (c.status_pdu_periodicity_offset_r13_present) {
        j.write_str("statusPDU-Periodicity-Offset-r13", c.status_pdu_periodicity_offset_r13.to_string());
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "pdcp_cfg_s::status_feedback_r13_c_");
  }
  j.end_obj();
}
SRSASN_CODE pdcp_cfg_s::status_feedback_r13_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.status_pdu_type_for_polling_r13_present, 1));
      HANDLE_CODE(bref.pack(c.status_pdu_periodicity_type1_r13_present, 1));
      HANDLE_CODE(bref.pack(c.status_pdu_periodicity_type2_r13_present, 1));
      HANDLE_CODE(bref.pack(c.status_pdu_periodicity_offset_r13_present, 1));
      if (c.status_pdu_type_for_polling_r13_present) {
        HANDLE_CODE(c.status_pdu_type_for_polling_r13.pack(bref));
      }
      if (c.status_pdu_periodicity_type1_r13_present) {
        HANDLE_CODE(c.status_pdu_periodicity_type1_r13.pack(bref));
      }
      if (c.status_pdu_periodicity_type2_r13_present) {
        HANDLE_CODE(c.status_pdu_periodicity_type2_r13.pack(bref));
      }
      if (c.status_pdu_periodicity_offset_r13_present) {
        HANDLE_CODE(c.status_pdu_periodicity_offset_r13.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "pdcp_cfg_s::status_feedback_r13_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pdcp_cfg_s::status_feedback_r13_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.status_pdu_type_for_polling_r13_present, 1));
      HANDLE_CODE(bref.unpack(c.status_pdu_periodicity_type1_r13_present, 1));
      HANDLE_CODE(bref.unpack(c.status_pdu_periodicity_type2_r13_present, 1));
      HANDLE_CODE(bref.unpack(c.status_pdu_periodicity_offset_r13_present, 1));
      if (c.status_pdu_type_for_polling_r13_present) {
        HANDLE_CODE(c.status_pdu_type_for_polling_r13.unpack(bref));
      }
      if (c.status_pdu_periodicity_type1_r13_present) {
        HANDLE_CODE(c.status_pdu_periodicity_type1_r13.unpack(bref));
      }
      if (c.status_pdu_periodicity_type2_r13_present) {
        HANDLE_CODE(c.status_pdu_periodicity_type2_r13.unpack(bref));
      }
      if (c.status_pdu_periodicity_offset_r13_present) {
        HANDLE_CODE(c.status_pdu_periodicity_offset_r13.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "pdcp_cfg_s::status_feedback_r13_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool pdcp_cfg_s::status_feedback_r13_c_::operator==(const status_feedback_r13_c_& other) const
{
  return type() == other.type() and
         c.status_pdu_type_for_polling_r13_present == other.c.status_pdu_type_for_polling_r13_present and
         (not c.status_pdu_type_for_polling_r13_present or
          c.status_pdu_type_for_polling_r13 == other.c.status_pdu_type_for_polling_r13) and
         c.status_pdu_periodicity_type1_r13_present == other.c.status_pdu_periodicity_type1_r13_present and
         (not c.status_pdu_periodicity_type1_r13_present or
          c.status_pdu_periodicity_type1_r13 == other.c.status_pdu_periodicity_type1_r13) and
         c.status_pdu_periodicity_type2_r13_present == other.c.status_pdu_periodicity_type2_r13_present and
         (not c.status_pdu_periodicity_type2_r13_present or
          c.status_pdu_periodicity_type2_r13 == other.c.status_pdu_periodicity_type2_r13) and
         c.status_pdu_periodicity_offset_r13_present == other.c.status_pdu_periodicity_offset_r13_present and
         (not c.status_pdu_periodicity_offset_r13_present or
          c.status_pdu_periodicity_offset_r13 == other.c.status_pdu_periodicity_offset_r13);
}

const char* pdcp_cfg_s::status_feedback_r13_c_::setup_s_::status_pdu_type_for_polling_r13_opts::to_string() const
{
  static const char* options[] = {"type1", "type2"};
  return convert_enum_idx(
      options, 2, value, "pdcp_cfg_s::status_feedback_r13_c_::setup_s_::status_pdu_type_for_polling_r13_e_");
}
uint8_t pdcp_cfg_s::status_feedback_r13_c_::setup_s_::status_pdu_type_for_polling_r13_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(
      options, 2, value, "pdcp_cfg_s::status_feedback_r13_c_::setup_s_::status_pdu_type_for_polling_r13_e_");
}

const char* pdcp_cfg_s::status_feedback_r13_c_::setup_s_::status_pdu_periodicity_type1_r13_opts::to_string() const
{
  static const char* options[] = {"ms5",   "ms10",   "ms20",   "ms30",   "ms40",    "ms50",    "ms60",
                                  "ms70",  "ms80",   "ms90",   "ms100",  "ms150",   "ms200",   "ms300",
                                  "ms500", "ms1000", "ms2000", "ms5000", "ms10000", "ms20000", "ms50000"};
  return convert_enum_idx(
      options, 21, value, "pdcp_cfg_s::status_feedback_r13_c_::setup_s_::status_pdu_periodicity_type1_r13_e_");
}
uint16_t pdcp_cfg_s::status_feedback_r13_c_::setup_s_::status_pdu_periodicity_type1_r13_opts::to_number() const
{
  static const uint16_t options[] = {5,   10,  20,  30,  40,   50,   60,   70,    80,    90,   100,
                                     150, 200, 300, 500, 1000, 2000, 5000, 10000, 20000, 50000};
  return map_enum_number(
      options, 21, value, "pdcp_cfg_s::status_feedback_r13_c_::setup_s_::status_pdu_periodicity_type1_r13_e_");
}

const char* pdcp_cfg_s::status_feedback_r13_c_::setup_s_::status_pdu_periodicity_type2_r13_opts::to_string() const
{
  static const char* options[] = {"ms5",   "ms10",   "ms20",   "ms30",   "ms40",    "ms50",    "ms60",
                                  "ms70",  "ms80",   "ms90",   "ms100",  "ms150",   "ms200",   "ms300",
                                  "ms500", "ms1000", "ms2000", "ms5000", "ms10000", "ms20000", "ms50000"};
  return convert_enum_idx(
      options, 21, value, "pdcp_cfg_s::status_feedback_r13_c_::setup_s_::status_pdu_periodicity_type2_r13_e_");
}
uint16_t pdcp_cfg_s::status_feedback_r13_c_::setup_s_::status_pdu_periodicity_type2_r13_opts::to_number() const
{
  static const uint16_t options[] = {5,   10,  20,  30,  40,   50,   60,   70,    80,    90,   100,
                                     150, 200, 300, 500, 1000, 2000, 5000, 10000, 20000, 50000};
  return map_enum_number(
      options, 21, value, "pdcp_cfg_s::status_feedback_r13_c_::setup_s_::status_pdu_periodicity_type2_r13_e_");
}

const char* pdcp_cfg_s::status_feedback_r13_c_::setup_s_::status_pdu_periodicity_offset_r13_opts::to_string() const
{
  static const char* options[] = {
      "ms1", "ms2", "ms5", "ms10", "ms25", "ms50", "ms100", "ms250", "ms500", "ms2500", "ms5000", "ms25000"};
  return convert_enum_idx(
      options, 12, value, "pdcp_cfg_s::status_feedback_r13_c_::setup_s_::status_pdu_periodicity_offset_r13_e_");
}
uint16_t pdcp_cfg_s::status_feedback_r13_c_::setup_s_::status_pdu_periodicity_offset_r13_opts::to_number() const
{
  static const uint16_t options[] = {1, 2, 5, 10, 25, 50, 100, 250, 500, 2500, 5000, 25000};
  return map_enum_number(
      options, 12, value, "pdcp_cfg_s::status_feedback_r13_c_::setup_s_::status_pdu_periodicity_offset_r13_e_");
}

void pdcp_cfg_s::ul_lwa_cfg_r14_c_::set(types::options e)
{
  type_ = e;
}
void pdcp_cfg_s::ul_lwa_cfg_r14_c_::set_release()
{
  set(types::release);
}
pdcp_cfg_s::ul_lwa_cfg_r14_c_::setup_s_& pdcp_cfg_s::ul_lwa_cfg_r14_c_::set_setup()
{
  set(types::setup);
  return c;
}
void pdcp_cfg_s::ul_lwa_cfg_r14_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_bool("ul-LWA-DRB-ViaWLAN-r14", c.ul_lwa_drb_via_wlan_r14);
      if (c.ul_lwa_data_split_thres_r14_present) {
        j.write_str("ul-LWA-DataSplitThreshold-r14", c.ul_lwa_data_split_thres_r14.to_string());
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "pdcp_cfg_s::ul_lwa_cfg_r14_c_");
  }
  j.end_obj();
}
SRSASN_CODE pdcp_cfg_s::ul_lwa_cfg_r14_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.ul_lwa_data_split_thres_r14_present, 1));
      HANDLE_CODE(bref.pack(c.ul_lwa_drb_via_wlan_r14, 1));
      if (c.ul_lwa_data_split_thres_r14_present) {
        HANDLE_CODE(c.ul_lwa_data_split_thres_r14.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "pdcp_cfg_s::ul_lwa_cfg_r14_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pdcp_cfg_s::ul_lwa_cfg_r14_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.ul_lwa_data_split_thres_r14_present, 1));
      HANDLE_CODE(bref.unpack(c.ul_lwa_drb_via_wlan_r14, 1));
      if (c.ul_lwa_data_split_thres_r14_present) {
        HANDLE_CODE(c.ul_lwa_data_split_thres_r14.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "pdcp_cfg_s::ul_lwa_cfg_r14_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool pdcp_cfg_s::ul_lwa_cfg_r14_c_::operator==(const ul_lwa_cfg_r14_c_& other) const
{
  return type() == other.type() and c.ul_lwa_drb_via_wlan_r14 == other.c.ul_lwa_drb_via_wlan_r14 and
         c.ul_lwa_data_split_thres_r14_present == other.c.ul_lwa_data_split_thres_r14_present and
         (not c.ul_lwa_data_split_thres_r14_present or
          c.ul_lwa_data_split_thres_r14 == other.c.ul_lwa_data_split_thres_r14);
}

const char* pdcp_cfg_s::ul_lwa_cfg_r14_c_::setup_s_::ul_lwa_data_split_thres_r14_opts::to_string() const
{
  static const char* options[] = {"b0",
                                  "b100",
                                  "b200",
                                  "b400",
                                  "b800",
                                  "b1600",
                                  "b3200",
                                  "b6400",
                                  "b12800",
                                  "b25600",
                                  "b51200",
                                  "b102400",
                                  "b204800",
                                  "b409600",
                                  "b819200"};
  return convert_enum_idx(
      options, 15, value, "pdcp_cfg_s::ul_lwa_cfg_r14_c_::setup_s_::ul_lwa_data_split_thres_r14_e_");
}
uint32_t pdcp_cfg_s::ul_lwa_cfg_r14_c_::setup_s_::ul_lwa_data_split_thres_r14_opts::to_number() const
{
  static const uint32_t options[] = {
      0, 100, 200, 400, 800, 1600, 3200, 6400, 12800, 25600, 51200, 102400, 204800, 409600, 819200};
  return map_enum_number(options, 15, value, "pdcp_cfg_s::ul_lwa_cfg_r14_c_::setup_s_::ul_lwa_data_split_thres_r14_e_");
}

void pdcp_cfg_s::ul_only_hdr_compress_r14_c_::set(types::options e)
{
  type_ = e;
}
void pdcp_cfg_s::ul_only_hdr_compress_r14_c_::set_not_used_r14()
{
  set(types::not_used_r14);
}
pdcp_cfg_s::ul_only_hdr_compress_r14_c_::rohc_r14_s_& pdcp_cfg_s::ul_only_hdr_compress_r14_c_::set_rohc_r14()
{
  set(types::rohc_r14);
  return c;
}
void pdcp_cfg_s::ul_only_hdr_compress_r14_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::not_used_r14:
      break;
    case types::rohc_r14:
      j.write_fieldname("rohc-r14");
      j.start_obj();
      if (c.max_cid_r14_present) {
        j.write_int("maxCID-r14", c.max_cid_r14);
      }
      j.write_fieldname("profiles-r14");
      j.start_obj();
      j.write_bool("profile0x0006-r14", c.profiles_r14.profile0x0006_r14);
      j.end_obj();
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "pdcp_cfg_s::ul_only_hdr_compress_r14_c_");
  }
  j.end_obj();
}
SRSASN_CODE pdcp_cfg_s::ul_only_hdr_compress_r14_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::not_used_r14:
      break;
    case types::rohc_r14:
      bref.pack(c.ext, 1);
      HANDLE_CODE(bref.pack(c.max_cid_r14_present, 1));
      if (c.max_cid_r14_present) {
        HANDLE_CODE(pack_integer(bref, c.max_cid_r14, (uint16_t)1u, (uint16_t)16383u));
      }
      HANDLE_CODE(bref.pack(c.profiles_r14.profile0x0006_r14, 1));
      break;
    default:
      log_invalid_choice_id(type_, "pdcp_cfg_s::ul_only_hdr_compress_r14_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pdcp_cfg_s::ul_only_hdr_compress_r14_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::not_used_r14:
      break;
    case types::rohc_r14:
      bref.unpack(c.ext, 1);
      HANDLE_CODE(bref.unpack(c.max_cid_r14_present, 1));
      if (c.max_cid_r14_present) {
        HANDLE_CODE(unpack_integer(c.max_cid_r14, bref, (uint16_t)1u, (uint16_t)16383u));
      }
      HANDLE_CODE(bref.unpack(c.profiles_r14.profile0x0006_r14, 1));
      break;
    default:
      log_invalid_choice_id(type_, "pdcp_cfg_s::ul_only_hdr_compress_r14_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool pdcp_cfg_s::ul_only_hdr_compress_r14_c_::operator==(const ul_only_hdr_compress_r14_c_& other) const
{
  return type() == other.type() and c.ext == other.c.ext and c.max_cid_r14_present == other.c.max_cid_r14_present and
         (not c.max_cid_r14_present or c.max_cid_r14 == other.c.max_cid_r14) and
         c.profiles_r14.profile0x0006_r14 == other.c.profiles_r14.profile0x0006_r14;
}

const char* pdcp_cfg_s::ul_data_compress_r15_s_::buffer_size_r15_opts::to_string() const
{
  static const char* options[] = {"kbyte2", "kbyte4", "kbyte8", "spare1"};
  return convert_enum_idx(options, 4, value, "pdcp_cfg_s::ul_data_compress_r15_s_::buffer_size_r15_e_");
}
uint8_t pdcp_cfg_s::ul_data_compress_r15_s_::buffer_size_r15_opts::to_number() const
{
  static const uint8_t options[] = {2, 4, 8};
  return map_enum_number(options, 3, value, "pdcp_cfg_s::ul_data_compress_r15_s_::buffer_size_r15_e_");
}

const char* pdcp_cfg_s::ul_data_compress_r15_s_::dictionary_r15_opts::to_string() const
{
  static const char* options[] = {"sip-SDP", "operator"};
  return convert_enum_idx(options, 2, value, "pdcp_cfg_s::ul_data_compress_r15_s_::dictionary_r15_e_");
}

void pdcp_cfg_s::pdcp_dupl_cfg_r15_c_::set(types::options e)
{
  type_ = e;
}
void pdcp_cfg_s::pdcp_dupl_cfg_r15_c_::set_release()
{
  set(types::release);
}
pdcp_cfg_s::pdcp_dupl_cfg_r15_c_::setup_s_& pdcp_cfg_s::pdcp_dupl_cfg_r15_c_::set_setup()
{
  set(types::setup);
  return c;
}
void pdcp_cfg_s::pdcp_dupl_cfg_r15_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_str("pdcp-Duplication-r15", c.pdcp_dupl_r15.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "pdcp_cfg_s::pdcp_dupl_cfg_r15_c_");
  }
  j.end_obj();
}
SRSASN_CODE pdcp_cfg_s::pdcp_dupl_cfg_r15_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.pdcp_dupl_r15.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "pdcp_cfg_s::pdcp_dupl_cfg_r15_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pdcp_cfg_s::pdcp_dupl_cfg_r15_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.pdcp_dupl_r15.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "pdcp_cfg_s::pdcp_dupl_cfg_r15_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool pdcp_cfg_s::pdcp_dupl_cfg_r15_c_::operator==(const pdcp_dupl_cfg_r15_c_& other) const
{
  return type() == other.type() and c.pdcp_dupl_r15 == other.c.pdcp_dupl_r15;
}

const char* pdcp_cfg_s::pdcp_dupl_cfg_r15_c_::setup_s_::pdcp_dupl_r15_opts::to_string() const
{
  static const char* options[] = {"configured", "activated"};
  return convert_enum_idx(options, 2, value, "pdcp_cfg_s::pdcp_dupl_cfg_r15_c_::setup_s_::pdcp_dupl_r15_e_");
}

// RLC-BearerConfig-r15 ::= CHOICE
void rlc_bearer_cfg_r15_c::set(types::options e)
{
  type_ = e;
}
void rlc_bearer_cfg_r15_c::set_release()
{
  set(types::release);
}
rlc_bearer_cfg_r15_c::setup_s_& rlc_bearer_cfg_r15_c::set_setup()
{
  set(types::setup);
  return c;
}
void rlc_bearer_cfg_r15_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      if (c.rlc_cfg_r15_present) {
        j.write_fieldname("rlc-Config-r15");
        c.rlc_cfg_r15.to_json(j);
      }
      j.write_fieldname("logicalChannelIdentityConfig-r15");
      c.lc_ch_id_cfg_r15.to_json(j);
      if (c.lc_ch_cfg_r15_present) {
        j.write_fieldname("logicalChannelConfig-r15");
        c.lc_ch_cfg_r15.to_json(j);
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "rlc_bearer_cfg_r15_c");
  }
  j.end_obj();
}
SRSASN_CODE rlc_bearer_cfg_r15_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.rlc_cfg_r15_present, 1));
      HANDLE_CODE(bref.pack(c.lc_ch_cfg_r15_present, 1));
      if (c.rlc_cfg_r15_present) {
        HANDLE_CODE(c.rlc_cfg_r15.pack(bref));
      }
      HANDLE_CODE(c.lc_ch_id_cfg_r15.pack(bref));
      if (c.lc_ch_cfg_r15_present) {
        HANDLE_CODE(c.lc_ch_cfg_r15.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "rlc_bearer_cfg_r15_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rlc_bearer_cfg_r15_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.rlc_cfg_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.lc_ch_cfg_r15_present, 1));
      if (c.rlc_cfg_r15_present) {
        HANDLE_CODE(c.rlc_cfg_r15.unpack(bref));
      }
      HANDLE_CODE(c.lc_ch_id_cfg_r15.unpack(bref));
      if (c.lc_ch_cfg_r15_present) {
        HANDLE_CODE(c.lc_ch_cfg_r15.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "rlc_bearer_cfg_r15_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool rlc_bearer_cfg_r15_c::operator==(const rlc_bearer_cfg_r15_c& other) const
{
  return type() == other.type() and c.rlc_cfg_r15_present == other.c.rlc_cfg_r15_present and
         (not c.rlc_cfg_r15_present or c.rlc_cfg_r15 == other.c.rlc_cfg_r15) and
         c.lc_ch_id_cfg_r15 == other.c.lc_ch_id_cfg_r15 and c.lc_ch_cfg_r15_present == other.c.lc_ch_cfg_r15_present and
         (not c.lc_ch_cfg_r15_present or c.lc_ch_cfg_r15 == other.c.lc_ch_cfg_r15);
}

void rlc_bearer_cfg_r15_c::setup_s_::lc_ch_id_cfg_r15_c_::destroy_() {}
void rlc_bearer_cfg_r15_c::setup_s_::lc_ch_id_cfg_r15_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
rlc_bearer_cfg_r15_c::setup_s_::lc_ch_id_cfg_r15_c_::lc_ch_id_cfg_r15_c_(
    const rlc_bearer_cfg_r15_c::setup_s_::lc_ch_id_cfg_r15_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::lc_ch_id_r15:
      c.init(other.c.get<uint8_t>());
      break;
    case types::lc_ch_id_ext_r15:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rlc_bearer_cfg_r15_c::setup_s_::lc_ch_id_cfg_r15_c_");
  }
}
rlc_bearer_cfg_r15_c::setup_s_::lc_ch_id_cfg_r15_c_& rlc_bearer_cfg_r15_c::setup_s_::lc_ch_id_cfg_r15_c_::operator=(
    const rlc_bearer_cfg_r15_c::setup_s_::lc_ch_id_cfg_r15_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::lc_ch_id_r15:
      c.set(other.c.get<uint8_t>());
      break;
    case types::lc_ch_id_ext_r15:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rlc_bearer_cfg_r15_c::setup_s_::lc_ch_id_cfg_r15_c_");
  }

  return *this;
}
uint8_t& rlc_bearer_cfg_r15_c::setup_s_::lc_ch_id_cfg_r15_c_::set_lc_ch_id_r15()
{
  set(types::lc_ch_id_r15);
  return c.get<uint8_t>();
}
uint8_t& rlc_bearer_cfg_r15_c::setup_s_::lc_ch_id_cfg_r15_c_::set_lc_ch_id_ext_r15()
{
  set(types::lc_ch_id_ext_r15);
  return c.get<uint8_t>();
}
void rlc_bearer_cfg_r15_c::setup_s_::lc_ch_id_cfg_r15_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::lc_ch_id_r15:
      j.write_int("logicalChannelIdentity-r15", c.get<uint8_t>());
      break;
    case types::lc_ch_id_ext_r15:
      j.write_int("logicalChannelIdentityExt-r15", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "rlc_bearer_cfg_r15_c::setup_s_::lc_ch_id_cfg_r15_c_");
  }
  j.end_obj();
}
SRSASN_CODE rlc_bearer_cfg_r15_c::setup_s_::lc_ch_id_cfg_r15_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::lc_ch_id_r15:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)1u, (uint8_t)10u));
      break;
    case types::lc_ch_id_ext_r15:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)32u, (uint8_t)38u));
      break;
    default:
      log_invalid_choice_id(type_, "rlc_bearer_cfg_r15_c::setup_s_::lc_ch_id_cfg_r15_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rlc_bearer_cfg_r15_c::setup_s_::lc_ch_id_cfg_r15_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::lc_ch_id_r15:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)1u, (uint8_t)10u));
      break;
    case types::lc_ch_id_ext_r15:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)32u, (uint8_t)38u));
      break;
    default:
      log_invalid_choice_id(type_, "rlc_bearer_cfg_r15_c::setup_s_::lc_ch_id_cfg_r15_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool rlc_bearer_cfg_r15_c::setup_s_::lc_ch_id_cfg_r15_c_::operator==(const lc_ch_id_cfg_r15_c_& other) const
{
  if (type_ != other.type_) {
    return false;
  }
  switch (type_) {
    case types::lc_ch_id_r15:
      return c.get<uint8_t>() == other.c.get<uint8_t>();
    case types::lc_ch_id_ext_r15:
      return c.get<uint8_t>() == other.c.get<uint8_t>();
    default:
      return true;
  }
  return true;
}

// RLC-Config ::= CHOICE
void rlc_cfg_c::destroy_()
{
  switch (type_) {
    case types::am:
      c.destroy<am_s_>();
      break;
    case types::um_bi_dir:
      c.destroy<um_bi_dir_s_>();
      break;
    case types::um_uni_dir_ul:
      c.destroy<um_uni_dir_ul_s_>();
      break;
    case types::um_uni_dir_dl:
      c.destroy<um_uni_dir_dl_s_>();
      break;
    default:
      break;
  }
}
void rlc_cfg_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::am:
      c.init<am_s_>();
      break;
    case types::um_bi_dir:
      c.init<um_bi_dir_s_>();
      break;
    case types::um_uni_dir_ul:
      c.init<um_uni_dir_ul_s_>();
      break;
    case types::um_uni_dir_dl:
      c.init<um_uni_dir_dl_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rlc_cfg_c");
  }
}
rlc_cfg_c::rlc_cfg_c(const rlc_cfg_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::am:
      c.init(other.c.get<am_s_>());
      break;
    case types::um_bi_dir:
      c.init(other.c.get<um_bi_dir_s_>());
      break;
    case types::um_uni_dir_ul:
      c.init(other.c.get<um_uni_dir_ul_s_>());
      break;
    case types::um_uni_dir_dl:
      c.init(other.c.get<um_uni_dir_dl_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rlc_cfg_c");
  }
}
rlc_cfg_c& rlc_cfg_c::operator=(const rlc_cfg_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::am:
      c.set(other.c.get<am_s_>());
      break;
    case types::um_bi_dir:
      c.set(other.c.get<um_bi_dir_s_>());
      break;
    case types::um_uni_dir_ul:
      c.set(other.c.get<um_uni_dir_ul_s_>());
      break;
    case types::um_uni_dir_dl:
      c.set(other.c.get<um_uni_dir_dl_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rlc_cfg_c");
  }

  return *this;
}
rlc_cfg_c::am_s_& rlc_cfg_c::set_am()
{
  set(types::am);
  return c.get<am_s_>();
}
rlc_cfg_c::um_bi_dir_s_& rlc_cfg_c::set_um_bi_dir()
{
  set(types::um_bi_dir);
  return c.get<um_bi_dir_s_>();
}
rlc_cfg_c::um_uni_dir_ul_s_& rlc_cfg_c::set_um_uni_dir_ul()
{
  set(types::um_uni_dir_ul);
  return c.get<um_uni_dir_ul_s_>();
}
rlc_cfg_c::um_uni_dir_dl_s_& rlc_cfg_c::set_um_uni_dir_dl()
{
  set(types::um_uni_dir_dl);
  return c.get<um_uni_dir_dl_s_>();
}
void rlc_cfg_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::am:
      j.write_fieldname("am");
      j.start_obj();
      j.write_fieldname("ul-AM-RLC");
      c.get<am_s_>().ul_am_rlc.to_json(j);
      j.write_fieldname("dl-AM-RLC");
      c.get<am_s_>().dl_am_rlc.to_json(j);
      j.end_obj();
      break;
    case types::um_bi_dir:
      j.write_fieldname("um-Bi-Directional");
      j.start_obj();
      j.write_fieldname("ul-UM-RLC");
      c.get<um_bi_dir_s_>().ul_um_rlc.to_json(j);
      j.write_fieldname("dl-UM-RLC");
      c.get<um_bi_dir_s_>().dl_um_rlc.to_json(j);
      j.end_obj();
      break;
    case types::um_uni_dir_ul:
      j.write_fieldname("um-Uni-Directional-UL");
      j.start_obj();
      j.write_fieldname("ul-UM-RLC");
      c.get<um_uni_dir_ul_s_>().ul_um_rlc.to_json(j);
      j.end_obj();
      break;
    case types::um_uni_dir_dl:
      j.write_fieldname("um-Uni-Directional-DL");
      j.start_obj();
      j.write_fieldname("dl-UM-RLC");
      c.get<um_uni_dir_dl_s_>().dl_um_rlc.to_json(j);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "rlc_cfg_c");
  }
  j.end_obj();
}
SRSASN_CODE rlc_cfg_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::am:
      HANDLE_CODE(c.get<am_s_>().ul_am_rlc.pack(bref));
      HANDLE_CODE(c.get<am_s_>().dl_am_rlc.pack(bref));
      break;
    case types::um_bi_dir:
      HANDLE_CODE(c.get<um_bi_dir_s_>().ul_um_rlc.pack(bref));
      HANDLE_CODE(c.get<um_bi_dir_s_>().dl_um_rlc.pack(bref));
      break;
    case types::um_uni_dir_ul:
      HANDLE_CODE(c.get<um_uni_dir_ul_s_>().ul_um_rlc.pack(bref));
      break;
    case types::um_uni_dir_dl:
      HANDLE_CODE(c.get<um_uni_dir_dl_s_>().dl_um_rlc.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rlc_cfg_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rlc_cfg_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::am:
      HANDLE_CODE(c.get<am_s_>().ul_am_rlc.unpack(bref));
      HANDLE_CODE(c.get<am_s_>().dl_am_rlc.unpack(bref));
      break;
    case types::um_bi_dir:
      HANDLE_CODE(c.get<um_bi_dir_s_>().ul_um_rlc.unpack(bref));
      HANDLE_CODE(c.get<um_bi_dir_s_>().dl_um_rlc.unpack(bref));
      break;
    case types::um_uni_dir_ul:
      HANDLE_CODE(c.get<um_uni_dir_ul_s_>().ul_um_rlc.unpack(bref));
      break;
    case types::um_uni_dir_dl:
      HANDLE_CODE(c.get<um_uni_dir_dl_s_>().dl_um_rlc.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rlc_cfg_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool rlc_cfg_c::operator==(const rlc_cfg_c& other) const
{
  if (type_ != other.type_) {
    return false;
  }
  switch (type_) {
    case types::am:
      return c.get<am_s_>().ul_am_rlc == other.c.get<am_s_>().ul_am_rlc and
             c.get<am_s_>().dl_am_rlc == other.c.get<am_s_>().dl_am_rlc;
    case types::um_bi_dir:
      return c.get<um_bi_dir_s_>().ul_um_rlc == other.c.get<um_bi_dir_s_>().ul_um_rlc and
             c.get<um_bi_dir_s_>().dl_um_rlc == other.c.get<um_bi_dir_s_>().dl_um_rlc;
    case types::um_uni_dir_ul:
      return c.get<um_uni_dir_ul_s_>().ul_um_rlc == other.c.get<um_uni_dir_ul_s_>().ul_um_rlc;
    case types::um_uni_dir_dl:
      return c.get<um_uni_dir_dl_s_>().dl_um_rlc == other.c.get<um_uni_dir_dl_s_>().dl_um_rlc;
    default:
      return true;
  }
  return true;
}

// RLC-Config-v1250 ::= SEQUENCE
SRSASN_CODE rlc_cfg_v1250_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ul_extended_rlc_li_field_r12, 1));
  HANDLE_CODE(bref.pack(dl_extended_rlc_li_field_r12, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rlc_cfg_v1250_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ul_extended_rlc_li_field_r12, 1));
  HANDLE_CODE(bref.unpack(dl_extended_rlc_li_field_r12, 1));

  return SRSASN_SUCCESS;
}
void rlc_cfg_v1250_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_bool("ul-extended-RLC-LI-Field-r12", ul_extended_rlc_li_field_r12);
  j.write_bool("dl-extended-RLC-LI-Field-r12", dl_extended_rlc_li_field_r12);
  j.end_obj();
}
bool rlc_cfg_v1250_s::operator==(const rlc_cfg_v1250_s& other) const
{
  return ul_extended_rlc_li_field_r12 == other.ul_extended_rlc_li_field_r12 and
         dl_extended_rlc_li_field_r12 == other.dl_extended_rlc_li_field_r12;
}

// RLC-Config-v1310 ::= SEQUENCE
SRSASN_CODE rlc_cfg_v1310_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(poll_pdu_v1310_present, 1));

  HANDLE_CODE(bref.pack(ul_extended_rlc_am_sn_r13, 1));
  HANDLE_CODE(bref.pack(dl_extended_rlc_am_sn_r13, 1));
  if (poll_pdu_v1310_present) {
    HANDLE_CODE(poll_pdu_v1310.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rlc_cfg_v1310_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(poll_pdu_v1310_present, 1));

  HANDLE_CODE(bref.unpack(ul_extended_rlc_am_sn_r13, 1));
  HANDLE_CODE(bref.unpack(dl_extended_rlc_am_sn_r13, 1));
  if (poll_pdu_v1310_present) {
    HANDLE_CODE(poll_pdu_v1310.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void rlc_cfg_v1310_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_bool("ul-extended-RLC-AM-SN-r13", ul_extended_rlc_am_sn_r13);
  j.write_bool("dl-extended-RLC-AM-SN-r13", dl_extended_rlc_am_sn_r13);
  if (poll_pdu_v1310_present) {
    j.write_str("pollPDU-v1310", poll_pdu_v1310.to_string());
  }
  j.end_obj();
}
bool rlc_cfg_v1310_s::operator==(const rlc_cfg_v1310_s& other) const
{
  return ul_extended_rlc_am_sn_r13 == other.ul_extended_rlc_am_sn_r13 and
         dl_extended_rlc_am_sn_r13 == other.dl_extended_rlc_am_sn_r13 and
         poll_pdu_v1310_present == other.poll_pdu_v1310_present and
         (not poll_pdu_v1310_present or poll_pdu_v1310 == other.poll_pdu_v1310);
}

// RLC-Config-v1430 ::= CHOICE
void rlc_cfg_v1430_c::set(types::options e)
{
  type_ = e;
}
void rlc_cfg_v1430_c::set_release()
{
  set(types::release);
}
rlc_cfg_v1430_c::setup_s_& rlc_cfg_v1430_c::set_setup()
{
  set(types::setup);
  return c;
}
void rlc_cfg_v1430_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_str("pollByte-r14", c.poll_byte_r14.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "rlc_cfg_v1430_c");
  }
  j.end_obj();
}
SRSASN_CODE rlc_cfg_v1430_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.poll_byte_r14.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rlc_cfg_v1430_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rlc_cfg_v1430_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.poll_byte_r14.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rlc_cfg_v1430_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool rlc_cfg_v1430_c::operator==(const rlc_cfg_v1430_c& other) const
{
  return type() == other.type() and c.poll_byte_r14 == other.c.poll_byte_r14;
}

// RLC-Config-v1510 ::= SEQUENCE
SRSASN_CODE rlc_cfg_v1510_s::pack(bit_ref& bref) const
{
  return SRSASN_SUCCESS;
}
SRSASN_CODE rlc_cfg_v1510_s::unpack(cbit_ref& bref)
{
  return SRSASN_SUCCESS;
}
void rlc_cfg_v1510_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("reestablishRLC-r15", "true");
  j.end_obj();
}
bool rlc_cfg_v1510_s::operator==(const rlc_cfg_v1510_s& other) const
{
  return true;
}

// RLC-Config-v1530 ::= CHOICE
void rlc_cfg_v1530_c::set(types::options e)
{
  type_ = e;
}
void rlc_cfg_v1530_c::set_release()
{
  set(types::release);
}
void rlc_cfg_v1530_c::set_setup()
{
  set(types::setup);
}
void rlc_cfg_v1530_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.end_obj();
}
SRSASN_CODE rlc_cfg_v1530_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  return SRSASN_SUCCESS;
}
SRSASN_CODE rlc_cfg_v1530_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  return SRSASN_SUCCESS;
}
bool rlc_cfg_v1530_c::operator==(const rlc_cfg_v1530_c& other) const
{
  return type() == other.type();
}

// RLC-Config-v1700 ::= SEQUENCE
SRSASN_CODE rlc_cfg_v1700_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(t_reordering_ext_r17.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rlc_cfg_v1700_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(t_reordering_ext_r17.unpack(bref));

  return SRSASN_SUCCESS;
}
void rlc_cfg_v1700_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("t-ReorderingExt-r17");
  t_reordering_ext_r17.to_json(j);
  j.end_obj();
}
bool rlc_cfg_v1700_s::operator==(const rlc_cfg_v1700_s& other) const
{
  return t_reordering_ext_r17 == other.t_reordering_ext_r17;
}

// SPS-ConfigSL-r14 ::= SEQUENCE
SRSASN_CODE sps_cfg_sl_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, sps_cfg_idx_r14, (uint8_t)1u, (uint8_t)8u));
  HANDLE_CODE(semi_persist_sched_interv_sl_r14.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sps_cfg_sl_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(sps_cfg_idx_r14, bref, (uint8_t)1u, (uint8_t)8u));
  HANDLE_CODE(semi_persist_sched_interv_sl_r14.unpack(bref));

  return SRSASN_SUCCESS;
}
void sps_cfg_sl_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("sps-ConfigIndex-r14", sps_cfg_idx_r14);
  j.write_str("semiPersistSchedIntervalSL-r14", semi_persist_sched_interv_sl_r14.to_string());
  j.end_obj();
}

const char* sps_cfg_sl_r14_s::semi_persist_sched_interv_sl_r14_opts::to_string() const
{
  static const char* options[] = {"sf20",
                                  "sf50",
                                  "sf100",
                                  "sf200",
                                  "sf300",
                                  "sf400",
                                  "sf500",
                                  "sf600",
                                  "sf700",
                                  "sf800",
                                  "sf900",
                                  "sf1000",
                                  "spare4",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "sps_cfg_sl_r14_s::semi_persist_sched_interv_sl_r14_e_");
}
uint16_t sps_cfg_sl_r14_s::semi_persist_sched_interv_sl_r14_opts::to_number() const
{
  static const uint16_t options[] = {20, 50, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};
  return map_enum_number(options, 12, value, "sps_cfg_sl_r14_s::semi_persist_sched_interv_sl_r14_e_");
}

// SPS-ConfigUL ::= CHOICE
void sps_cfg_ul_c::set(types::options e)
{
  type_ = e;
}
void sps_cfg_ul_c::set_release()
{
  set(types::release);
}
sps_cfg_ul_c::setup_s_& sps_cfg_ul_c::set_setup()
{
  set(types::setup);
  return c;
}
void sps_cfg_ul_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      c.to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "sps_cfg_ul_c");
  }
  j.end_obj();
}
SRSASN_CODE sps_cfg_ul_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sps_cfg_ul_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sps_cfg_ul_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sps_cfg_ul_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool sps_cfg_ul_c::operator==(const sps_cfg_ul_c& other) const
{
  return type() == other.type() and c == other.c;
}

SRSASN_CODE sps_cfg_ul_c::setup_s_::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(p0_persistent_present, 1));
  HANDLE_CODE(bref.pack(two_intervs_cfg_present, 1));

  HANDLE_CODE(semi_persist_sched_interv_ul.pack(bref));
  HANDLE_CODE(implicit_release_after.pack(bref));
  if (p0_persistent_present) {
    HANDLE_CODE(pack_integer(bref, p0_persistent.p0_nominal_pusch_persistent, (int8_t)-126, (int8_t)24));
    HANDLE_CODE(pack_integer(bref, p0_persistent.p0_ue_pusch_persistent, (int8_t)-8, (int8_t)7));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= p0_persistent_sf_set2_r12.is_present();
    group_flags[1] |= nof_conf_ul_sps_processes_r13_present;
    group_flags[2] |= fixed_rv_non_adaptive_r14_present;
    group_flags[2] |= sps_cfg_idx_r14_present;
    group_flags[2] |= semi_persist_sched_interv_ul_v1430_present;
    group_flags[3] |= cyclic_shift_sps_r15_present;
    group_flags[3] |= harq_proc_id_offset_r15_present;
    group_flags[3] |= rv_sps_ul_repeats_r15_present;
    group_flags[3] |= tpc_pdcch_cfg_pusch_sps_r15.is_present();
    group_flags[3] |= total_num_pusch_sps_ul_repeats_r15_present;
    group_flags[3] |= sps_cfg_idx_r15_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(p0_persistent_sf_set2_r12.is_present(), 1));
      if (p0_persistent_sf_set2_r12.is_present()) {
        HANDLE_CODE(p0_persistent_sf_set2_r12->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(nof_conf_ul_sps_processes_r13_present, 1));
      if (nof_conf_ul_sps_processes_r13_present) {
        HANDLE_CODE(pack_integer(bref, nof_conf_ul_sps_processes_r13, (uint8_t)1u, (uint8_t)8u));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(fixed_rv_non_adaptive_r14_present, 1));
      HANDLE_CODE(bref.pack(sps_cfg_idx_r14_present, 1));
      HANDLE_CODE(bref.pack(semi_persist_sched_interv_ul_v1430_present, 1));
      if (sps_cfg_idx_r14_present) {
        HANDLE_CODE(pack_integer(bref, sps_cfg_idx_r14, (uint8_t)1u, (uint8_t)8u));
      }
      if (semi_persist_sched_interv_ul_v1430_present) {
        HANDLE_CODE(semi_persist_sched_interv_ul_v1430.pack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(cyclic_shift_sps_r15_present, 1));
      HANDLE_CODE(bref.pack(harq_proc_id_offset_r15_present, 1));
      HANDLE_CODE(bref.pack(rv_sps_ul_repeats_r15_present, 1));
      HANDLE_CODE(bref.pack(tpc_pdcch_cfg_pusch_sps_r15.is_present(), 1));
      HANDLE_CODE(bref.pack(total_num_pusch_sps_ul_repeats_r15_present, 1));
      HANDLE_CODE(bref.pack(sps_cfg_idx_r15_present, 1));
      if (cyclic_shift_sps_r15_present) {
        HANDLE_CODE(cyclic_shift_sps_r15.pack(bref));
      }
      if (harq_proc_id_offset_r15_present) {
        HANDLE_CODE(pack_integer(bref, harq_proc_id_offset_r15, (uint8_t)0u, (uint8_t)7u));
      }
      if (rv_sps_ul_repeats_r15_present) {
        HANDLE_CODE(rv_sps_ul_repeats_r15.pack(bref));
      }
      if (tpc_pdcch_cfg_pusch_sps_r15.is_present()) {
        HANDLE_CODE(tpc_pdcch_cfg_pusch_sps_r15->pack(bref));
      }
      if (total_num_pusch_sps_ul_repeats_r15_present) {
        HANDLE_CODE(total_num_pusch_sps_ul_repeats_r15.pack(bref));
      }
      if (sps_cfg_idx_r15_present) {
        HANDLE_CODE(pack_integer(bref, sps_cfg_idx_r15, (uint8_t)1u, (uint8_t)6u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sps_cfg_ul_c::setup_s_::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(p0_persistent_present, 1));
  HANDLE_CODE(bref.unpack(two_intervs_cfg_present, 1));

  HANDLE_CODE(semi_persist_sched_interv_ul.unpack(bref));
  HANDLE_CODE(implicit_release_after.unpack(bref));
  if (p0_persistent_present) {
    HANDLE_CODE(unpack_integer(p0_persistent.p0_nominal_pusch_persistent, bref, (int8_t)-126, (int8_t)24));
    HANDLE_CODE(unpack_integer(p0_persistent.p0_ue_pusch_persistent, bref, (int8_t)-8, (int8_t)7));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(4);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool p0_persistent_sf_set2_r12_present;
      HANDLE_CODE(bref.unpack(p0_persistent_sf_set2_r12_present, 1));
      p0_persistent_sf_set2_r12.set_present(p0_persistent_sf_set2_r12_present);
      if (p0_persistent_sf_set2_r12.is_present()) {
        HANDLE_CODE(p0_persistent_sf_set2_r12->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(nof_conf_ul_sps_processes_r13_present, 1));
      if (nof_conf_ul_sps_processes_r13_present) {
        HANDLE_CODE(unpack_integer(nof_conf_ul_sps_processes_r13, bref, (uint8_t)1u, (uint8_t)8u));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(fixed_rv_non_adaptive_r14_present, 1));
      HANDLE_CODE(bref.unpack(sps_cfg_idx_r14_present, 1));
      HANDLE_CODE(bref.unpack(semi_persist_sched_interv_ul_v1430_present, 1));
      if (sps_cfg_idx_r14_present) {
        HANDLE_CODE(unpack_integer(sps_cfg_idx_r14, bref, (uint8_t)1u, (uint8_t)8u));
      }
      if (semi_persist_sched_interv_ul_v1430_present) {
        HANDLE_CODE(semi_persist_sched_interv_ul_v1430.unpack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(cyclic_shift_sps_r15_present, 1));
      HANDLE_CODE(bref.unpack(harq_proc_id_offset_r15_present, 1));
      HANDLE_CODE(bref.unpack(rv_sps_ul_repeats_r15_present, 1));
      bool tpc_pdcch_cfg_pusch_sps_r15_present;
      HANDLE_CODE(bref.unpack(tpc_pdcch_cfg_pusch_sps_r15_present, 1));
      tpc_pdcch_cfg_pusch_sps_r15.set_present(tpc_pdcch_cfg_pusch_sps_r15_present);
      HANDLE_CODE(bref.unpack(total_num_pusch_sps_ul_repeats_r15_present, 1));
      HANDLE_CODE(bref.unpack(sps_cfg_idx_r15_present, 1));
      if (cyclic_shift_sps_r15_present) {
        HANDLE_CODE(cyclic_shift_sps_r15.unpack(bref));
      }
      if (harq_proc_id_offset_r15_present) {
        HANDLE_CODE(unpack_integer(harq_proc_id_offset_r15, bref, (uint8_t)0u, (uint8_t)7u));
      }
      if (rv_sps_ul_repeats_r15_present) {
        HANDLE_CODE(rv_sps_ul_repeats_r15.unpack(bref));
      }
      if (tpc_pdcch_cfg_pusch_sps_r15.is_present()) {
        HANDLE_CODE(tpc_pdcch_cfg_pusch_sps_r15->unpack(bref));
      }
      if (total_num_pusch_sps_ul_repeats_r15_present) {
        HANDLE_CODE(total_num_pusch_sps_ul_repeats_r15.unpack(bref));
      }
      if (sps_cfg_idx_r15_present) {
        HANDLE_CODE(unpack_integer(sps_cfg_idx_r15, bref, (uint8_t)1u, (uint8_t)6u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sps_cfg_ul_c::setup_s_::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("semiPersistSchedIntervalUL", semi_persist_sched_interv_ul.to_string());
  j.write_str("implicitReleaseAfter", implicit_release_after.to_string());
  if (p0_persistent_present) {
    j.write_fieldname("p0-Persistent");
    j.start_obj();
    j.write_int("p0-NominalPUSCH-Persistent", p0_persistent.p0_nominal_pusch_persistent);
    j.write_int("p0-UE-PUSCH-Persistent", p0_persistent.p0_ue_pusch_persistent);
    j.end_obj();
  }
  if (two_intervs_cfg_present) {
    j.write_str("twoIntervalsConfig", "true");
  }
  if (ext) {
    if (p0_persistent_sf_set2_r12.is_present()) {
      j.write_fieldname("p0-PersistentSubframeSet2-r12");
      p0_persistent_sf_set2_r12->to_json(j);
    }
    if (nof_conf_ul_sps_processes_r13_present) {
      j.write_int("numberOfConfUlSPS-Processes-r13", nof_conf_ul_sps_processes_r13);
    }
    if (fixed_rv_non_adaptive_r14_present) {
      j.write_str("fixedRV-NonAdaptive-r14", "true");
    }
    if (sps_cfg_idx_r14_present) {
      j.write_int("sps-ConfigIndex-r14", sps_cfg_idx_r14);
    }
    if (semi_persist_sched_interv_ul_v1430_present) {
      j.write_str("semiPersistSchedIntervalUL-v1430", semi_persist_sched_interv_ul_v1430.to_string());
    }
    if (cyclic_shift_sps_r15_present) {
      j.write_str("cyclicShiftSPS-r15", cyclic_shift_sps_r15.to_string());
    }
    if (harq_proc_id_offset_r15_present) {
      j.write_int("harq-ProcID-Offset-r15", harq_proc_id_offset_r15);
    }
    if (rv_sps_ul_repeats_r15_present) {
      j.write_str("rv-SPS-UL-Repetitions-r15", rv_sps_ul_repeats_r15.to_string());
    }
    if (tpc_pdcch_cfg_pusch_sps_r15.is_present()) {
      j.write_fieldname("tpc-PDCCH-ConfigPUSCH-SPS-r15");
      tpc_pdcch_cfg_pusch_sps_r15->to_json(j);
    }
    if (total_num_pusch_sps_ul_repeats_r15_present) {
      j.write_str("totalNumberPUSCH-SPS-UL-Repetitions-r15", total_num_pusch_sps_ul_repeats_r15.to_string());
    }
    if (sps_cfg_idx_r15_present) {
      j.write_int("sps-ConfigIndex-r15", sps_cfg_idx_r15);
    }
  }
  j.end_obj();
}
bool sps_cfg_ul_c::setup_s_::operator==(const setup_s_& other) const
{
  return ext == other.ext and semi_persist_sched_interv_ul == other.semi_persist_sched_interv_ul and
         implicit_release_after == other.implicit_release_after and
         p0_persistent.p0_nominal_pusch_persistent == other.p0_persistent.p0_nominal_pusch_persistent and
         p0_persistent.p0_ue_pusch_persistent == other.p0_persistent.p0_ue_pusch_persistent and
         two_intervs_cfg_present == other.two_intervs_cfg_present and
         (not ext or
          (p0_persistent_sf_set2_r12.is_present() == other.p0_persistent_sf_set2_r12.is_present() and
           (not p0_persistent_sf_set2_r12.is_present() or
            *p0_persistent_sf_set2_r12 == *other.p0_persistent_sf_set2_r12) and
           nof_conf_ul_sps_processes_r13_present == other.nof_conf_ul_sps_processes_r13_present and
           (not nof_conf_ul_sps_processes_r13_present or
            nof_conf_ul_sps_processes_r13 == other.nof_conf_ul_sps_processes_r13) and
           fixed_rv_non_adaptive_r14_present == other.fixed_rv_non_adaptive_r14_present and
           sps_cfg_idx_r14_present == other.sps_cfg_idx_r14_present and
           (not sps_cfg_idx_r14_present or sps_cfg_idx_r14 == other.sps_cfg_idx_r14) and
           semi_persist_sched_interv_ul_v1430_present == other.semi_persist_sched_interv_ul_v1430_present and
           (not semi_persist_sched_interv_ul_v1430_present or
            semi_persist_sched_interv_ul_v1430 == other.semi_persist_sched_interv_ul_v1430) and
           cyclic_shift_sps_r15_present == other.cyclic_shift_sps_r15_present and
           (not cyclic_shift_sps_r15_present or cyclic_shift_sps_r15 == other.cyclic_shift_sps_r15) and
           harq_proc_id_offset_r15_present == other.harq_proc_id_offset_r15_present and
           (not harq_proc_id_offset_r15_present or harq_proc_id_offset_r15 == other.harq_proc_id_offset_r15) and
           rv_sps_ul_repeats_r15_present == other.rv_sps_ul_repeats_r15_present and
           (not rv_sps_ul_repeats_r15_present or rv_sps_ul_repeats_r15 == other.rv_sps_ul_repeats_r15) and
           tpc_pdcch_cfg_pusch_sps_r15.is_present() == other.tpc_pdcch_cfg_pusch_sps_r15.is_present() and
           (not tpc_pdcch_cfg_pusch_sps_r15.is_present() or
            *tpc_pdcch_cfg_pusch_sps_r15 == *other.tpc_pdcch_cfg_pusch_sps_r15) and
           total_num_pusch_sps_ul_repeats_r15_present == other.total_num_pusch_sps_ul_repeats_r15_present and
           (not total_num_pusch_sps_ul_repeats_r15_present or
            total_num_pusch_sps_ul_repeats_r15 == other.total_num_pusch_sps_ul_repeats_r15) and
           sps_cfg_idx_r15_present == other.sps_cfg_idx_r15_present and
           (not sps_cfg_idx_r15_present or sps_cfg_idx_r15 == other.sps_cfg_idx_r15)));
}

const char* sps_cfg_ul_c::setup_s_::semi_persist_sched_interv_ul_opts::to_string() const
{
  static const char* options[] = {"sf10",
                                  "sf20",
                                  "sf32",
                                  "sf40",
                                  "sf64",
                                  "sf80",
                                  "sf128",
                                  "sf160",
                                  "sf320",
                                  "sf640",
                                  "sf1-v1430",
                                  "sf2-v1430",
                                  "sf3-v1430",
                                  "sf4-v1430",
                                  "sf5-v1430",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "sps_cfg_ul_c::setup_s_::semi_persist_sched_interv_ul_e_");
}
uint16_t sps_cfg_ul_c::setup_s_::semi_persist_sched_interv_ul_opts::to_number() const
{
  static const uint16_t options[] = {10, 20, 32, 40, 64, 80, 128, 160, 320, 640, 1, 2, 3, 4, 5};
  return map_enum_number(options, 15, value, "sps_cfg_ul_c::setup_s_::semi_persist_sched_interv_ul_e_");
}

const char* sps_cfg_ul_c::setup_s_::implicit_release_after_opts::to_string() const
{
  static const char* options[] = {"e2", "e3", "e4", "e8"};
  return convert_enum_idx(options, 4, value, "sps_cfg_ul_c::setup_s_::implicit_release_after_e_");
}
uint8_t sps_cfg_ul_c::setup_s_::implicit_release_after_opts::to_number() const
{
  static const uint8_t options[] = {2, 3, 4, 8};
  return map_enum_number(options, 4, value, "sps_cfg_ul_c::setup_s_::implicit_release_after_e_");
}

void sps_cfg_ul_c::setup_s_::p0_persistent_sf_set2_r12_c_::set(types::options e)
{
  type_ = e;
}
void sps_cfg_ul_c::setup_s_::p0_persistent_sf_set2_r12_c_::set_release()
{
  set(types::release);
}
sps_cfg_ul_c::setup_s_::p0_persistent_sf_set2_r12_c_::setup_s_&
sps_cfg_ul_c::setup_s_::p0_persistent_sf_set2_r12_c_::set_setup()
{
  set(types::setup);
  return c;
}
void sps_cfg_ul_c::setup_s_::p0_persistent_sf_set2_r12_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_int("p0-NominalPUSCH-PersistentSubframeSet2-r12", c.p0_nominal_pusch_persistent_sf_set2_r12);
      j.write_int("p0-UE-PUSCH-PersistentSubframeSet2-r12", c.p0_ue_pusch_persistent_sf_set2_r12);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "sps_cfg_ul_c::setup_s_::p0_persistent_sf_set2_r12_c_");
  }
  j.end_obj();
}
SRSASN_CODE sps_cfg_ul_c::setup_s_::p0_persistent_sf_set2_r12_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(pack_integer(bref, c.p0_nominal_pusch_persistent_sf_set2_r12, (int8_t)-126, (int8_t)24));
      HANDLE_CODE(pack_integer(bref, c.p0_ue_pusch_persistent_sf_set2_r12, (int8_t)-8, (int8_t)7));
      break;
    default:
      log_invalid_choice_id(type_, "sps_cfg_ul_c::setup_s_::p0_persistent_sf_set2_r12_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sps_cfg_ul_c::setup_s_::p0_persistent_sf_set2_r12_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(unpack_integer(c.p0_nominal_pusch_persistent_sf_set2_r12, bref, (int8_t)-126, (int8_t)24));
      HANDLE_CODE(unpack_integer(c.p0_ue_pusch_persistent_sf_set2_r12, bref, (int8_t)-8, (int8_t)7));
      break;
    default:
      log_invalid_choice_id(type_, "sps_cfg_ul_c::setup_s_::p0_persistent_sf_set2_r12_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool sps_cfg_ul_c::setup_s_::p0_persistent_sf_set2_r12_c_::operator==(const p0_persistent_sf_set2_r12_c_& other) const
{
  return type() == other.type() and
         c.p0_nominal_pusch_persistent_sf_set2_r12 == other.c.p0_nominal_pusch_persistent_sf_set2_r12 and
         c.p0_ue_pusch_persistent_sf_set2_r12 == other.c.p0_ue_pusch_persistent_sf_set2_r12;
}

const char* sps_cfg_ul_c::setup_s_::semi_persist_sched_interv_ul_v1430_opts::to_string() const
{
  static const char* options[] = {"sf50",
                                  "sf100",
                                  "sf200",
                                  "sf300",
                                  "sf400",
                                  "sf500",
                                  "sf600",
                                  "sf700",
                                  "sf800",
                                  "sf900",
                                  "sf1000",
                                  "spare5",
                                  "spare4",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "sps_cfg_ul_c::setup_s_::semi_persist_sched_interv_ul_v1430_e_");
}
uint16_t sps_cfg_ul_c::setup_s_::semi_persist_sched_interv_ul_v1430_opts::to_number() const
{
  static const uint16_t options[] = {50, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};
  return map_enum_number(options, 11, value, "sps_cfg_ul_c::setup_s_::semi_persist_sched_interv_ul_v1430_e_");
}

const char* sps_cfg_ul_c::setup_s_::cyclic_shift_sps_r15_opts::to_string() const
{
  static const char* options[] = {"cs0", "cs1", "cs2", "cs3", "cs4", "cs5", "cs6", "cs7"};
  return convert_enum_idx(options, 8, value, "sps_cfg_ul_c::setup_s_::cyclic_shift_sps_r15_e_");
}
uint8_t sps_cfg_ul_c::setup_s_::cyclic_shift_sps_r15_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3, 4, 5, 6, 7};
  return map_enum_number(options, 8, value, "sps_cfg_ul_c::setup_s_::cyclic_shift_sps_r15_e_");
}

const char* sps_cfg_ul_c::setup_s_::rv_sps_ul_repeats_r15_opts::to_string() const
{
  static const char* options[] = {"ulrvseq1", "ulrvseq2", "ulrvseq3"};
  return convert_enum_idx(options, 3, value, "sps_cfg_ul_c::setup_s_::rv_sps_ul_repeats_r15_e_");
}
uint8_t sps_cfg_ul_c::setup_s_::rv_sps_ul_repeats_r15_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3};
  return map_enum_number(options, 3, value, "sps_cfg_ul_c::setup_s_::rv_sps_ul_repeats_r15_e_");
}

const char* sps_cfg_ul_c::setup_s_::total_num_pusch_sps_ul_repeats_r15_opts::to_string() const
{
  static const char* options[] = {"n2", "n3", "n4", "n6"};
  return convert_enum_idx(options, 4, value, "sps_cfg_ul_c::setup_s_::total_num_pusch_sps_ul_repeats_r15_e_");
}
uint8_t sps_cfg_ul_c::setup_s_::total_num_pusch_sps_ul_repeats_r15_opts::to_number() const
{
  static const uint8_t options[] = {2, 3, 4, 6};
  return map_enum_number(options, 4, value, "sps_cfg_ul_c::setup_s_::total_num_pusch_sps_ul_repeats_r15_e_");
}

// SPS-ConfigUL-STTI-r15 ::= CHOICE
void sps_cfg_ul_stti_r15_c::set(types::options e)
{
  type_ = e;
}
void sps_cfg_ul_stti_r15_c::set_release()
{
  set(types::release);
}
sps_cfg_ul_stti_r15_c::setup_s_& sps_cfg_ul_stti_r15_c::set_setup()
{
  set(types::setup);
  return c;
}
void sps_cfg_ul_stti_r15_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_str("semiPersistSchedIntervalUL-STTI-r15", c.semi_persist_sched_interv_ul_stti_r15.to_string());
      j.write_str("implicitReleaseAfter", c.implicit_release_after.to_string());
      if (c.p0_persistent_r15_present) {
        j.write_fieldname("p0-Persistent-r15");
        j.start_obj();
        j.write_int("p0-NominalSPUSCH-Persistent-r15", c.p0_persistent_r15.p0_nominal_spusch_persistent_r15);
        j.write_int("p0-UE-SPUSCH-Persistent-r15", c.p0_persistent_r15.p0_ue_spusch_persistent_r15);
        j.end_obj();
      }
      if (c.two_intervs_cfg_r15_present) {
        j.write_str("twoIntervalsConfig-r15", "true");
      }
      if (c.p0_persistent_sf_set2_r15_present) {
        j.write_fieldname("p0-PersistentSubframeSet2-r15");
        c.p0_persistent_sf_set2_r15.to_json(j);
      }
      if (c.nof_conf_ul_sps_processes_stti_r15_present) {
        j.write_int("numberOfConfUL-SPS-Processes-STTI-r15", c.nof_conf_ul_sps_processes_stti_r15);
      }
      j.write_int("sTTI-StartTimeUL-r15", c.stti_start_time_ul_r15);
      if (c.tpc_pdcch_cfg_pusch_sps_r15_present) {
        j.write_fieldname("tpc-PDCCH-ConfigPUSCH-SPS-r15");
        c.tpc_pdcch_cfg_pusch_sps_r15.to_json(j);
      }
      if (c.cyclic_shift_sps_s_tti_r15_present) {
        j.write_str("cyclicShiftSPS-sTTI-r15", c.cyclic_shift_sps_s_tti_r15.to_string());
      }
      if (c.ifdma_cfg_sps_r15_present) {
        j.write_bool("ifdma-Config-SPS-r15", c.ifdma_cfg_sps_r15);
      }
      if (c.harq_proc_id_offset_r15_present) {
        j.write_int("harq-ProcID-offset-r15", c.harq_proc_id_offset_r15);
      }
      if (c.rv_sps_stti_ul_repeats_r15_present) {
        j.write_str("rv-SPS-STTI-UL-Repetitions-r15", c.rv_sps_stti_ul_repeats_r15.to_string());
      }
      if (c.sps_cfg_idx_r15_present) {
        j.write_int("sps-ConfigIndex-r15", c.sps_cfg_idx_r15);
      }
      if (c.tbs_scaling_factor_subslot_sps_ul_repeats_r15_present) {
        j.write_str("tbs-scalingFactorSubslotSPS-UL-Repetitions-r15",
                    c.tbs_scaling_factor_subslot_sps_ul_repeats_r15.to_string());
      }
      if (c.total_num_pusch_sps_stti_ul_repeats_r15_present) {
        j.write_str("totalNumberPUSCH-SPS-STTI-UL-Repetitions-r15",
                    c.total_num_pusch_sps_stti_ul_repeats_r15.to_string());
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "sps_cfg_ul_stti_r15_c");
  }
  j.end_obj();
}
SRSASN_CODE sps_cfg_ul_stti_r15_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      bref.pack(c.ext, 1);
      HANDLE_CODE(bref.pack(c.p0_persistent_r15_present, 1));
      HANDLE_CODE(bref.pack(c.two_intervs_cfg_r15_present, 1));
      HANDLE_CODE(bref.pack(c.p0_persistent_sf_set2_r15_present, 1));
      HANDLE_CODE(bref.pack(c.nof_conf_ul_sps_processes_stti_r15_present, 1));
      HANDLE_CODE(bref.pack(c.tpc_pdcch_cfg_pusch_sps_r15_present, 1));
      HANDLE_CODE(bref.pack(c.cyclic_shift_sps_s_tti_r15_present, 1));
      HANDLE_CODE(bref.pack(c.ifdma_cfg_sps_r15_present, 1));
      HANDLE_CODE(bref.pack(c.harq_proc_id_offset_r15_present, 1));
      HANDLE_CODE(bref.pack(c.rv_sps_stti_ul_repeats_r15_present, 1));
      HANDLE_CODE(bref.pack(c.sps_cfg_idx_r15_present, 1));
      HANDLE_CODE(bref.pack(c.tbs_scaling_factor_subslot_sps_ul_repeats_r15_present, 1));
      HANDLE_CODE(bref.pack(c.total_num_pusch_sps_stti_ul_repeats_r15_present, 1));
      HANDLE_CODE(c.semi_persist_sched_interv_ul_stti_r15.pack(bref));
      HANDLE_CODE(c.implicit_release_after.pack(bref));
      if (c.p0_persistent_r15_present) {
        HANDLE_CODE(pack_integer(bref, c.p0_persistent_r15.p0_nominal_spusch_persistent_r15, (int8_t)-126, (int8_t)24));
        HANDLE_CODE(pack_integer(bref, c.p0_persistent_r15.p0_ue_spusch_persistent_r15, (int8_t)-8, (int8_t)7));
      }
      if (c.p0_persistent_sf_set2_r15_present) {
        HANDLE_CODE(c.p0_persistent_sf_set2_r15.pack(bref));
      }
      if (c.nof_conf_ul_sps_processes_stti_r15_present) {
        HANDLE_CODE(pack_integer(bref, c.nof_conf_ul_sps_processes_stti_r15, (uint8_t)1u, (uint8_t)12u));
      }
      HANDLE_CODE(pack_integer(bref, c.stti_start_time_ul_r15, (uint8_t)0u, (uint8_t)5u));
      if (c.tpc_pdcch_cfg_pusch_sps_r15_present) {
        HANDLE_CODE(c.tpc_pdcch_cfg_pusch_sps_r15.pack(bref));
      }
      if (c.cyclic_shift_sps_s_tti_r15_present) {
        HANDLE_CODE(c.cyclic_shift_sps_s_tti_r15.pack(bref));
      }
      if (c.ifdma_cfg_sps_r15_present) {
        HANDLE_CODE(bref.pack(c.ifdma_cfg_sps_r15, 1));
      }
      if (c.harq_proc_id_offset_r15_present) {
        HANDLE_CODE(pack_integer(bref, c.harq_proc_id_offset_r15, (uint8_t)0u, (uint8_t)15u));
      }
      if (c.rv_sps_stti_ul_repeats_r15_present) {
        HANDLE_CODE(c.rv_sps_stti_ul_repeats_r15.pack(bref));
      }
      if (c.sps_cfg_idx_r15_present) {
        HANDLE_CODE(pack_integer(bref, c.sps_cfg_idx_r15, (uint8_t)1u, (uint8_t)6u));
      }
      if (c.tbs_scaling_factor_subslot_sps_ul_repeats_r15_present) {
        HANDLE_CODE(c.tbs_scaling_factor_subslot_sps_ul_repeats_r15.pack(bref));
      }
      if (c.total_num_pusch_sps_stti_ul_repeats_r15_present) {
        HANDLE_CODE(c.total_num_pusch_sps_stti_ul_repeats_r15.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "sps_cfg_ul_stti_r15_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sps_cfg_ul_stti_r15_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      bref.unpack(c.ext, 1);
      HANDLE_CODE(bref.unpack(c.p0_persistent_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.two_intervs_cfg_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.p0_persistent_sf_set2_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.nof_conf_ul_sps_processes_stti_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.tpc_pdcch_cfg_pusch_sps_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.cyclic_shift_sps_s_tti_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.ifdma_cfg_sps_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.harq_proc_id_offset_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.rv_sps_stti_ul_repeats_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.sps_cfg_idx_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.tbs_scaling_factor_subslot_sps_ul_repeats_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.total_num_pusch_sps_stti_ul_repeats_r15_present, 1));
      HANDLE_CODE(c.semi_persist_sched_interv_ul_stti_r15.unpack(bref));
      HANDLE_CODE(c.implicit_release_after.unpack(bref));
      if (c.p0_persistent_r15_present) {
        HANDLE_CODE(
            unpack_integer(c.p0_persistent_r15.p0_nominal_spusch_persistent_r15, bref, (int8_t)-126, (int8_t)24));
        HANDLE_CODE(unpack_integer(c.p0_persistent_r15.p0_ue_spusch_persistent_r15, bref, (int8_t)-8, (int8_t)7));
      }
      if (c.p0_persistent_sf_set2_r15_present) {
        HANDLE_CODE(c.p0_persistent_sf_set2_r15.unpack(bref));
      }
      if (c.nof_conf_ul_sps_processes_stti_r15_present) {
        HANDLE_CODE(unpack_integer(c.nof_conf_ul_sps_processes_stti_r15, bref, (uint8_t)1u, (uint8_t)12u));
      }
      HANDLE_CODE(unpack_integer(c.stti_start_time_ul_r15, bref, (uint8_t)0u, (uint8_t)5u));
      if (c.tpc_pdcch_cfg_pusch_sps_r15_present) {
        HANDLE_CODE(c.tpc_pdcch_cfg_pusch_sps_r15.unpack(bref));
      }
      if (c.cyclic_shift_sps_s_tti_r15_present) {
        HANDLE_CODE(c.cyclic_shift_sps_s_tti_r15.unpack(bref));
      }
      if (c.ifdma_cfg_sps_r15_present) {
        HANDLE_CODE(bref.unpack(c.ifdma_cfg_sps_r15, 1));
      }
      if (c.harq_proc_id_offset_r15_present) {
        HANDLE_CODE(unpack_integer(c.harq_proc_id_offset_r15, bref, (uint8_t)0u, (uint8_t)15u));
      }
      if (c.rv_sps_stti_ul_repeats_r15_present) {
        HANDLE_CODE(c.rv_sps_stti_ul_repeats_r15.unpack(bref));
      }
      if (c.sps_cfg_idx_r15_present) {
        HANDLE_CODE(unpack_integer(c.sps_cfg_idx_r15, bref, (uint8_t)1u, (uint8_t)6u));
      }
      if (c.tbs_scaling_factor_subslot_sps_ul_repeats_r15_present) {
        HANDLE_CODE(c.tbs_scaling_factor_subslot_sps_ul_repeats_r15.unpack(bref));
      }
      if (c.total_num_pusch_sps_stti_ul_repeats_r15_present) {
        HANDLE_CODE(c.total_num_pusch_sps_stti_ul_repeats_r15.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "sps_cfg_ul_stti_r15_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool sps_cfg_ul_stti_r15_c::operator==(const sps_cfg_ul_stti_r15_c& other) const
{
  return type() == other.type() and c.ext == other.c.ext and
         c.semi_persist_sched_interv_ul_stti_r15 == other.c.semi_persist_sched_interv_ul_stti_r15 and
         c.implicit_release_after == other.c.implicit_release_after and
         c.p0_persistent_r15.p0_nominal_spusch_persistent_r15 ==
             other.c.p0_persistent_r15.p0_nominal_spusch_persistent_r15 and
         c.p0_persistent_r15.p0_ue_spusch_persistent_r15 == other.c.p0_persistent_r15.p0_ue_spusch_persistent_r15 and
         c.two_intervs_cfg_r15_present == other.c.two_intervs_cfg_r15_present and
         c.p0_persistent_sf_set2_r15_present == other.c.p0_persistent_sf_set2_r15_present and
         (not c.p0_persistent_sf_set2_r15_present or
          c.p0_persistent_sf_set2_r15 == other.c.p0_persistent_sf_set2_r15) and
         c.nof_conf_ul_sps_processes_stti_r15_present == other.c.nof_conf_ul_sps_processes_stti_r15_present and
         (not c.nof_conf_ul_sps_processes_stti_r15_present or
          c.nof_conf_ul_sps_processes_stti_r15 == other.c.nof_conf_ul_sps_processes_stti_r15) and
         c.stti_start_time_ul_r15 == other.c.stti_start_time_ul_r15 and
         c.tpc_pdcch_cfg_pusch_sps_r15_present == other.c.tpc_pdcch_cfg_pusch_sps_r15_present and
         (not c.tpc_pdcch_cfg_pusch_sps_r15_present or
          c.tpc_pdcch_cfg_pusch_sps_r15 == other.c.tpc_pdcch_cfg_pusch_sps_r15) and
         c.cyclic_shift_sps_s_tti_r15_present == other.c.cyclic_shift_sps_s_tti_r15_present and
         (not c.cyclic_shift_sps_s_tti_r15_present or
          c.cyclic_shift_sps_s_tti_r15 == other.c.cyclic_shift_sps_s_tti_r15) and
         c.ifdma_cfg_sps_r15_present == other.c.ifdma_cfg_sps_r15_present and
         (not c.ifdma_cfg_sps_r15_present or c.ifdma_cfg_sps_r15 == other.c.ifdma_cfg_sps_r15) and
         c.harq_proc_id_offset_r15_present == other.c.harq_proc_id_offset_r15_present and
         (not c.harq_proc_id_offset_r15_present or c.harq_proc_id_offset_r15 == other.c.harq_proc_id_offset_r15) and
         c.rv_sps_stti_ul_repeats_r15_present == other.c.rv_sps_stti_ul_repeats_r15_present and
         (not c.rv_sps_stti_ul_repeats_r15_present or
          c.rv_sps_stti_ul_repeats_r15 == other.c.rv_sps_stti_ul_repeats_r15) and
         c.sps_cfg_idx_r15_present == other.c.sps_cfg_idx_r15_present and
         (not c.sps_cfg_idx_r15_present or c.sps_cfg_idx_r15 == other.c.sps_cfg_idx_r15) and
         c.tbs_scaling_factor_subslot_sps_ul_repeats_r15_present ==
             other.c.tbs_scaling_factor_subslot_sps_ul_repeats_r15_present and
         (not c.tbs_scaling_factor_subslot_sps_ul_repeats_r15_present or
          c.tbs_scaling_factor_subslot_sps_ul_repeats_r15 == other.c.tbs_scaling_factor_subslot_sps_ul_repeats_r15) and
         c.total_num_pusch_sps_stti_ul_repeats_r15_present ==
             other.c.total_num_pusch_sps_stti_ul_repeats_r15_present and
         (not c.total_num_pusch_sps_stti_ul_repeats_r15_present or
          c.total_num_pusch_sps_stti_ul_repeats_r15 == other.c.total_num_pusch_sps_stti_ul_repeats_r15);
}

const char* sps_cfg_ul_stti_r15_c::setup_s_::semi_persist_sched_interv_ul_stti_r15_opts::to_string() const
{
  static const char* options[] = {"sTTI1",
                                  "sTTI2",
                                  "sTTI3",
                                  "sTTI4",
                                  "sTTI6",
                                  "sTTI8",
                                  "sTTI12",
                                  "sTTI16",
                                  "sTTI20",
                                  "sTTI40",
                                  "sTTI60",
                                  "sTTI80",
                                  "sTTI120",
                                  "sTTI240",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(
      options, 16, value, "sps_cfg_ul_stti_r15_c::setup_s_::semi_persist_sched_interv_ul_stti_r15_e_");
}
uint8_t sps_cfg_ul_stti_r15_c::setup_s_::semi_persist_sched_interv_ul_stti_r15_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 6, 8, 12, 16, 20, 40, 60, 80, 120, 240};
  return map_enum_number(
      options, 14, value, "sps_cfg_ul_stti_r15_c::setup_s_::semi_persist_sched_interv_ul_stti_r15_e_");
}

const char* sps_cfg_ul_stti_r15_c::setup_s_::implicit_release_after_opts::to_string() const
{
  static const char* options[] = {"e2", "e3", "e4", "e8"};
  return convert_enum_idx(options, 4, value, "sps_cfg_ul_stti_r15_c::setup_s_::implicit_release_after_e_");
}
uint8_t sps_cfg_ul_stti_r15_c::setup_s_::implicit_release_after_opts::to_number() const
{
  static const uint8_t options[] = {2, 3, 4, 8};
  return map_enum_number(options, 4, value, "sps_cfg_ul_stti_r15_c::setup_s_::implicit_release_after_e_");
}

void sps_cfg_ul_stti_r15_c::setup_s_::p0_persistent_sf_set2_r15_c_::set(types::options e)
{
  type_ = e;
}
void sps_cfg_ul_stti_r15_c::setup_s_::p0_persistent_sf_set2_r15_c_::set_release()
{
  set(types::release);
}
sps_cfg_ul_stti_r15_c::setup_s_::p0_persistent_sf_set2_r15_c_::setup_s_&
sps_cfg_ul_stti_r15_c::setup_s_::p0_persistent_sf_set2_r15_c_::set_setup()
{
  set(types::setup);
  return c;
}
void sps_cfg_ul_stti_r15_c::setup_s_::p0_persistent_sf_set2_r15_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_int("p0-NominalSPUSCH-PersistentSubframeSet2-r15", c.p0_nominal_spusch_persistent_sf_set2_r15);
      j.write_int("p0-UE-SPUSCH-PersistentSubframeSet2-r15", c.p0_ue_spusch_persistent_sf_set2_r15);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "sps_cfg_ul_stti_r15_c::setup_s_::p0_persistent_sf_set2_r15_c_");
  }
  j.end_obj();
}
SRSASN_CODE sps_cfg_ul_stti_r15_c::setup_s_::p0_persistent_sf_set2_r15_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(pack_integer(bref, c.p0_nominal_spusch_persistent_sf_set2_r15, (int8_t)-126, (int8_t)24));
      HANDLE_CODE(pack_integer(bref, c.p0_ue_spusch_persistent_sf_set2_r15, (int8_t)-8, (int8_t)7));
      break;
    default:
      log_invalid_choice_id(type_, "sps_cfg_ul_stti_r15_c::setup_s_::p0_persistent_sf_set2_r15_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sps_cfg_ul_stti_r15_c::setup_s_::p0_persistent_sf_set2_r15_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(unpack_integer(c.p0_nominal_spusch_persistent_sf_set2_r15, bref, (int8_t)-126, (int8_t)24));
      HANDLE_CODE(unpack_integer(c.p0_ue_spusch_persistent_sf_set2_r15, bref, (int8_t)-8, (int8_t)7));
      break;
    default:
      log_invalid_choice_id(type_, "sps_cfg_ul_stti_r15_c::setup_s_::p0_persistent_sf_set2_r15_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool sps_cfg_ul_stti_r15_c::setup_s_::p0_persistent_sf_set2_r15_c_::operator==(
    const p0_persistent_sf_set2_r15_c_& other) const
{
  return type() == other.type() and
         c.p0_nominal_spusch_persistent_sf_set2_r15 == other.c.p0_nominal_spusch_persistent_sf_set2_r15 and
         c.p0_ue_spusch_persistent_sf_set2_r15 == other.c.p0_ue_spusch_persistent_sf_set2_r15;
}

const char* sps_cfg_ul_stti_r15_c::setup_s_::cyclic_shift_sps_s_tti_r15_opts::to_string() const
{
  static const char* options[] = {"cs0", "cs1", "cs2", "cs3", "cs4", "cs5", "cs6", "cs7"};
  return convert_enum_idx(options, 8, value, "sps_cfg_ul_stti_r15_c::setup_s_::cyclic_shift_sps_s_tti_r15_e_");
}
uint8_t sps_cfg_ul_stti_r15_c::setup_s_::cyclic_shift_sps_s_tti_r15_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3, 4, 5, 6, 7};
  return map_enum_number(options, 8, value, "sps_cfg_ul_stti_r15_c::setup_s_::cyclic_shift_sps_s_tti_r15_e_");
}

const char* sps_cfg_ul_stti_r15_c::setup_s_::rv_sps_stti_ul_repeats_r15_opts::to_string() const
{
  static const char* options[] = {"ulrvseq1", "ulrvseq2", "ulrvseq3"};
  return convert_enum_idx(options, 3, value, "sps_cfg_ul_stti_r15_c::setup_s_::rv_sps_stti_ul_repeats_r15_e_");
}
uint8_t sps_cfg_ul_stti_r15_c::setup_s_::rv_sps_stti_ul_repeats_r15_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3};
  return map_enum_number(options, 3, value, "sps_cfg_ul_stti_r15_c::setup_s_::rv_sps_stti_ul_repeats_r15_e_");
}

const char* sps_cfg_ul_stti_r15_c::setup_s_::tbs_scaling_factor_subslot_sps_ul_repeats_r15_opts::to_string() const
{
  static const char* options[] = {"n6", "n12"};
  return convert_enum_idx(
      options, 2, value, "sps_cfg_ul_stti_r15_c::setup_s_::tbs_scaling_factor_subslot_sps_ul_repeats_r15_e_");
}
uint8_t sps_cfg_ul_stti_r15_c::setup_s_::tbs_scaling_factor_subslot_sps_ul_repeats_r15_opts::to_number() const
{
  static const uint8_t options[] = {6, 12};
  return map_enum_number(
      options, 2, value, "sps_cfg_ul_stti_r15_c::setup_s_::tbs_scaling_factor_subslot_sps_ul_repeats_r15_e_");
}

const char* sps_cfg_ul_stti_r15_c::setup_s_::total_num_pusch_sps_stti_ul_repeats_r15_opts::to_string() const
{
  static const char* options[] = {"n2", "n3", "n4", "n6"};
  return convert_enum_idx(
      options, 4, value, "sps_cfg_ul_stti_r15_c::setup_s_::total_num_pusch_sps_stti_ul_repeats_r15_e_");
}
uint8_t sps_cfg_ul_stti_r15_c::setup_s_::total_num_pusch_sps_stti_ul_repeats_r15_opts::to_number() const
{
  static const uint8_t options[] = {2, 3, 4, 6};
  return map_enum_number(
      options, 4, value, "sps_cfg_ul_stti_r15_c::setup_s_::total_num_pusch_sps_stti_ul_repeats_r15_e_");
}

// STAG-ToAddMod-r11 ::= SEQUENCE
SRSASN_CODE stag_to_add_mod_r11_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, stag_id_r11, (uint8_t)1u, (uint8_t)3u));
  HANDLE_CODE(time_align_timer_stag_r11.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE stag_to_add_mod_r11_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(stag_id_r11, bref, (uint8_t)1u, (uint8_t)3u));
  HANDLE_CODE(time_align_timer_stag_r11.unpack(bref));

  return SRSASN_SUCCESS;
}
void stag_to_add_mod_r11_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("stag-Id-r11", stag_id_r11);
  j.write_str("timeAlignmentTimerSTAG-r11", time_align_timer_stag_r11.to_string());
  j.end_obj();
}

// DRB-ToAddMod ::= SEQUENCE
SRSASN_CODE drb_to_add_mod_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(eps_bearer_id_present, 1));
  HANDLE_CODE(bref.pack(pdcp_cfg_present, 1));
  HANDLE_CODE(bref.pack(rlc_cfg_present, 1));
  HANDLE_CODE(bref.pack(lc_ch_id_present, 1));
  HANDLE_CODE(bref.pack(lc_ch_cfg_present, 1));

  if (eps_bearer_id_present) {
    HANDLE_CODE(pack_integer(bref, eps_bearer_id, (uint8_t)0u, (uint8_t)15u));
  }
  HANDLE_CODE(pack_integer(bref, drb_id, (uint8_t)1u, (uint8_t)32u));
  if (pdcp_cfg_present) {
    HANDLE_CODE(pdcp_cfg.pack(bref));
  }
  if (rlc_cfg_present) {
    HANDLE_CODE(rlc_cfg.pack(bref));
  }
  if (lc_ch_id_present) {
    HANDLE_CODE(pack_integer(bref, lc_ch_id, (uint8_t)3u, (uint8_t)10u));
  }
  if (lc_ch_cfg_present) {
    HANDLE_CODE(lc_ch_cfg.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= drb_type_change_r12_present;
    group_flags[0] |= rlc_cfg_v1250.is_present();
    group_flags[1] |= rlc_cfg_v1310.is_present();
    group_flags[1] |= drb_type_lwa_r13_present;
    group_flags[1] |= drb_type_lwip_r13_present;
    group_flags[2] |= rlc_cfg_v1430.is_present();
    group_flags[2] |= lwip_ul_aggregation_r14_present;
    group_flags[2] |= lwip_dl_aggregation_r14_present;
    group_flags[2] |= lwa_wlan_ac_r14_present;
    group_flags[3] |= rlc_cfg_v1510.is_present();
    group_flags[4] |= rlc_cfg_v1530.is_present();
    group_flags[4] |= rlc_bearer_cfg_secondary_r15.is_present();
    group_flags[4] |= lc_ch_id_r15_present;
    group_flags[5] |= daps_ho_r16_present;
    group_flags[6] |= rlc_cfg_v1700.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(drb_type_change_r12_present, 1));
      HANDLE_CODE(bref.pack(rlc_cfg_v1250.is_present(), 1));
      if (rlc_cfg_v1250.is_present()) {
        HANDLE_CODE(rlc_cfg_v1250->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(rlc_cfg_v1310.is_present(), 1));
      HANDLE_CODE(bref.pack(drb_type_lwa_r13_present, 1));
      HANDLE_CODE(bref.pack(drb_type_lwip_r13_present, 1));
      if (rlc_cfg_v1310.is_present()) {
        HANDLE_CODE(rlc_cfg_v1310->pack(bref));
      }
      if (drb_type_lwa_r13_present) {
        HANDLE_CODE(bref.pack(drb_type_lwa_r13, 1));
      }
      if (drb_type_lwip_r13_present) {
        HANDLE_CODE(drb_type_lwip_r13.pack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(rlc_cfg_v1430.is_present(), 1));
      HANDLE_CODE(bref.pack(lwip_ul_aggregation_r14_present, 1));
      HANDLE_CODE(bref.pack(lwip_dl_aggregation_r14_present, 1));
      HANDLE_CODE(bref.pack(lwa_wlan_ac_r14_present, 1));
      if (rlc_cfg_v1430.is_present()) {
        HANDLE_CODE(rlc_cfg_v1430->pack(bref));
      }
      if (lwip_ul_aggregation_r14_present) {
        HANDLE_CODE(bref.pack(lwip_ul_aggregation_r14, 1));
      }
      if (lwip_dl_aggregation_r14_present) {
        HANDLE_CODE(bref.pack(lwip_dl_aggregation_r14, 1));
      }
      if (lwa_wlan_ac_r14_present) {
        HANDLE_CODE(lwa_wlan_ac_r14.pack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(rlc_cfg_v1510.is_present(), 1));
      if (rlc_cfg_v1510.is_present()) {
        HANDLE_CODE(rlc_cfg_v1510->pack(bref));
      }
    }
    if (group_flags[4]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(rlc_cfg_v1530.is_present(), 1));
      HANDLE_CODE(bref.pack(rlc_bearer_cfg_secondary_r15.is_present(), 1));
      HANDLE_CODE(bref.pack(lc_ch_id_r15_present, 1));
      if (rlc_cfg_v1530.is_present()) {
        HANDLE_CODE(rlc_cfg_v1530->pack(bref));
      }
      if (rlc_bearer_cfg_secondary_r15.is_present()) {
        HANDLE_CODE(rlc_bearer_cfg_secondary_r15->pack(bref));
      }
      if (lc_ch_id_r15_present) {
        HANDLE_CODE(pack_integer(bref, lc_ch_id_r15, (uint8_t)32u, (uint8_t)38u));
      }
    }
    if (group_flags[5]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(daps_ho_r16_present, 1));
    }
    if (group_flags[6]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(rlc_cfg_v1700.is_present(), 1));
      if (rlc_cfg_v1700.is_present()) {
        HANDLE_CODE(rlc_cfg_v1700->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_to_add_mod_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(eps_bearer_id_present, 1));
  HANDLE_CODE(bref.unpack(pdcp_cfg_present, 1));
  HANDLE_CODE(bref.unpack(rlc_cfg_present, 1));
  HANDLE_CODE(bref.unpack(lc_ch_id_present, 1));
  HANDLE_CODE(bref.unpack(lc_ch_cfg_present, 1));

  if (eps_bearer_id_present) {
    HANDLE_CODE(unpack_integer(eps_bearer_id, bref, (uint8_t)0u, (uint8_t)15u));
  }
  HANDLE_CODE(unpack_integer(drb_id, bref, (uint8_t)1u, (uint8_t)32u));
  if (pdcp_cfg_present) {
    HANDLE_CODE(pdcp_cfg.unpack(bref));
  }
  if (rlc_cfg_present) {
    HANDLE_CODE(rlc_cfg.unpack(bref));
  }
  if (lc_ch_id_present) {
    HANDLE_CODE(unpack_integer(lc_ch_id, bref, (uint8_t)3u, (uint8_t)10u));
  }
  if (lc_ch_cfg_present) {
    HANDLE_CODE(lc_ch_cfg.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(7);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(drb_type_change_r12_present, 1));
      bool rlc_cfg_v1250_present;
      HANDLE_CODE(bref.unpack(rlc_cfg_v1250_present, 1));
      rlc_cfg_v1250.set_present(rlc_cfg_v1250_present);
      if (rlc_cfg_v1250.is_present()) {
        HANDLE_CODE(rlc_cfg_v1250->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool rlc_cfg_v1310_present;
      HANDLE_CODE(bref.unpack(rlc_cfg_v1310_present, 1));
      rlc_cfg_v1310.set_present(rlc_cfg_v1310_present);
      HANDLE_CODE(bref.unpack(drb_type_lwa_r13_present, 1));
      HANDLE_CODE(bref.unpack(drb_type_lwip_r13_present, 1));
      if (rlc_cfg_v1310.is_present()) {
        HANDLE_CODE(rlc_cfg_v1310->unpack(bref));
      }
      if (drb_type_lwa_r13_present) {
        HANDLE_CODE(bref.unpack(drb_type_lwa_r13, 1));
      }
      if (drb_type_lwip_r13_present) {
        HANDLE_CODE(drb_type_lwip_r13.unpack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool rlc_cfg_v1430_present;
      HANDLE_CODE(bref.unpack(rlc_cfg_v1430_present, 1));
      rlc_cfg_v1430.set_present(rlc_cfg_v1430_present);
      HANDLE_CODE(bref.unpack(lwip_ul_aggregation_r14_present, 1));
      HANDLE_CODE(bref.unpack(lwip_dl_aggregation_r14_present, 1));
      HANDLE_CODE(bref.unpack(lwa_wlan_ac_r14_present, 1));
      if (rlc_cfg_v1430.is_present()) {
        HANDLE_CODE(rlc_cfg_v1430->unpack(bref));
      }
      if (lwip_ul_aggregation_r14_present) {
        HANDLE_CODE(bref.unpack(lwip_ul_aggregation_r14, 1));
      }
      if (lwip_dl_aggregation_r14_present) {
        HANDLE_CODE(bref.unpack(lwip_dl_aggregation_r14, 1));
      }
      if (lwa_wlan_ac_r14_present) {
        HANDLE_CODE(lwa_wlan_ac_r14.unpack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool rlc_cfg_v1510_present;
      HANDLE_CODE(bref.unpack(rlc_cfg_v1510_present, 1));
      rlc_cfg_v1510.set_present(rlc_cfg_v1510_present);
      if (rlc_cfg_v1510.is_present()) {
        HANDLE_CODE(rlc_cfg_v1510->unpack(bref));
      }
    }
    if (group_flags[4]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool rlc_cfg_v1530_present;
      HANDLE_CODE(bref.unpack(rlc_cfg_v1530_present, 1));
      rlc_cfg_v1530.set_present(rlc_cfg_v1530_present);
      bool rlc_bearer_cfg_secondary_r15_present;
      HANDLE_CODE(bref.unpack(rlc_bearer_cfg_secondary_r15_present, 1));
      rlc_bearer_cfg_secondary_r15.set_present(rlc_bearer_cfg_secondary_r15_present);
      HANDLE_CODE(bref.unpack(lc_ch_id_r15_present, 1));
      if (rlc_cfg_v1530.is_present()) {
        HANDLE_CODE(rlc_cfg_v1530->unpack(bref));
      }
      if (rlc_bearer_cfg_secondary_r15.is_present()) {
        HANDLE_CODE(rlc_bearer_cfg_secondary_r15->unpack(bref));
      }
      if (lc_ch_id_r15_present) {
        HANDLE_CODE(unpack_integer(lc_ch_id_r15, bref, (uint8_t)32u, (uint8_t)38u));
      }
    }
    if (group_flags[5]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(daps_ho_r16_present, 1));
    }
    if (group_flags[6]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool rlc_cfg_v1700_present;
      HANDLE_CODE(bref.unpack(rlc_cfg_v1700_present, 1));
      rlc_cfg_v1700.set_present(rlc_cfg_v1700_present);
      if (rlc_cfg_v1700.is_present()) {
        HANDLE_CODE(rlc_cfg_v1700->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void drb_to_add_mod_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (eps_bearer_id_present) {
    j.write_int("eps-BearerIdentity", eps_bearer_id);
  }
  j.write_int("drb-Identity", drb_id);
  if (pdcp_cfg_present) {
    j.write_fieldname("pdcp-Config");
    pdcp_cfg.to_json(j);
  }
  if (rlc_cfg_present) {
    j.write_fieldname("rlc-Config");
    rlc_cfg.to_json(j);
  }
  if (lc_ch_id_present) {
    j.write_int("logicalChannelIdentity", lc_ch_id);
  }
  if (lc_ch_cfg_present) {
    j.write_fieldname("logicalChannelConfig");
    lc_ch_cfg.to_json(j);
  }
  if (ext) {
    if (drb_type_change_r12_present) {
      j.write_str("drb-TypeChange-r12", "toMCG");
    }
    if (rlc_cfg_v1250.is_present()) {
      j.write_fieldname("rlc-Config-v1250");
      rlc_cfg_v1250->to_json(j);
    }
    if (rlc_cfg_v1310.is_present()) {
      j.write_fieldname("rlc-Config-v1310");
      rlc_cfg_v1310->to_json(j);
    }
    if (drb_type_lwa_r13_present) {
      j.write_bool("drb-TypeLWA-r13", drb_type_lwa_r13);
    }
    if (drb_type_lwip_r13_present) {
      j.write_str("drb-TypeLWIP-r13", drb_type_lwip_r13.to_string());
    }
    if (rlc_cfg_v1430.is_present()) {
      j.write_fieldname("rlc-Config-v1430");
      rlc_cfg_v1430->to_json(j);
    }
    if (lwip_ul_aggregation_r14_present) {
      j.write_bool("lwip-UL-Aggregation-r14", lwip_ul_aggregation_r14);
    }
    if (lwip_dl_aggregation_r14_present) {
      j.write_bool("lwip-DL-Aggregation-r14", lwip_dl_aggregation_r14);
    }
    if (lwa_wlan_ac_r14_present) {
      j.write_str("lwa-WLAN-AC-r14", lwa_wlan_ac_r14.to_string());
    }
    if (rlc_cfg_v1510.is_present()) {
      j.write_fieldname("rlc-Config-v1510");
      rlc_cfg_v1510->to_json(j);
    }
    if (rlc_cfg_v1530.is_present()) {
      j.write_fieldname("rlc-Config-v1530");
      rlc_cfg_v1530->to_json(j);
    }
    if (rlc_bearer_cfg_secondary_r15.is_present()) {
      j.write_fieldname("rlc-BearerConfigSecondary-r15");
      rlc_bearer_cfg_secondary_r15->to_json(j);
    }
    if (lc_ch_id_r15_present) {
      j.write_int("logicalChannelIdentity-r15", lc_ch_id_r15);
    }
    if (daps_ho_r16_present) {
      j.write_str("daps-HO-r16", "true");
    }
    if (rlc_cfg_v1700.is_present()) {
      j.write_fieldname("rlc-Config-v1700");
      rlc_cfg_v1700->to_json(j);
    }
  }
  j.end_obj();
}
bool drb_to_add_mod_s::operator==(const drb_to_add_mod_s& other) const
{
  return ext == other.ext and eps_bearer_id_present == other.eps_bearer_id_present and
         (not eps_bearer_id_present or eps_bearer_id == other.eps_bearer_id) and drb_id == other.drb_id and
         pdcp_cfg_present == other.pdcp_cfg_present and (not pdcp_cfg_present or pdcp_cfg == other.pdcp_cfg) and
         rlc_cfg_present == other.rlc_cfg_present and (not rlc_cfg_present or rlc_cfg == other.rlc_cfg) and
         lc_ch_id_present == other.lc_ch_id_present and (not lc_ch_id_present or lc_ch_id == other.lc_ch_id) and
         lc_ch_cfg_present == other.lc_ch_cfg_present and (not lc_ch_cfg_present or lc_ch_cfg == other.lc_ch_cfg) and
         (not ext or
          (drb_type_change_r12_present == other.drb_type_change_r12_present and
           rlc_cfg_v1250.is_present() == other.rlc_cfg_v1250.is_present() and
           (not rlc_cfg_v1250.is_present() or *rlc_cfg_v1250 == *other.rlc_cfg_v1250) and
           rlc_cfg_v1310.is_present() == other.rlc_cfg_v1310.is_present() and
           (not rlc_cfg_v1310.is_present() or *rlc_cfg_v1310 == *other.rlc_cfg_v1310) and
           drb_type_lwa_r13_present == other.drb_type_lwa_r13_present and
           (not drb_type_lwa_r13_present or drb_type_lwa_r13 == other.drb_type_lwa_r13) and
           drb_type_lwip_r13_present == other.drb_type_lwip_r13_present and
           (not drb_type_lwip_r13_present or drb_type_lwip_r13 == other.drb_type_lwip_r13) and
           rlc_cfg_v1430.is_present() == other.rlc_cfg_v1430.is_present() and
           (not rlc_cfg_v1430.is_present() or *rlc_cfg_v1430 == *other.rlc_cfg_v1430) and
           lwip_ul_aggregation_r14_present == other.lwip_ul_aggregation_r14_present and
           (not lwip_ul_aggregation_r14_present or lwip_ul_aggregation_r14 == other.lwip_ul_aggregation_r14) and
           lwip_dl_aggregation_r14_present == other.lwip_dl_aggregation_r14_present and
           (not lwip_dl_aggregation_r14_present or lwip_dl_aggregation_r14 == other.lwip_dl_aggregation_r14) and
           lwa_wlan_ac_r14_present == other.lwa_wlan_ac_r14_present and
           (not lwa_wlan_ac_r14_present or lwa_wlan_ac_r14 == other.lwa_wlan_ac_r14) and
           rlc_cfg_v1510.is_present() == other.rlc_cfg_v1510.is_present() and
           (not rlc_cfg_v1510.is_present() or *rlc_cfg_v1510 == *other.rlc_cfg_v1510) and
           rlc_cfg_v1530.is_present() == other.rlc_cfg_v1530.is_present() and
           (not rlc_cfg_v1530.is_present() or *rlc_cfg_v1530 == *other.rlc_cfg_v1530) and
           rlc_bearer_cfg_secondary_r15.is_present() == other.rlc_bearer_cfg_secondary_r15.is_present() and
           (not rlc_bearer_cfg_secondary_r15.is_present() or
            *rlc_bearer_cfg_secondary_r15 == *other.rlc_bearer_cfg_secondary_r15) and
           lc_ch_id_r15_present == other.lc_ch_id_r15_present and
           (not lc_ch_id_r15_present or lc_ch_id_r15 == other.lc_ch_id_r15) and
           daps_ho_r16_present == other.daps_ho_r16_present and
           rlc_cfg_v1700.is_present() == other.rlc_cfg_v1700.is_present() and
           (not rlc_cfg_v1700.is_present() or *rlc_cfg_v1700 == *other.rlc_cfg_v1700)));
}

const char* drb_to_add_mod_s::drb_type_lwip_r13_opts::to_string() const
{
  static const char* options[] = {"lwip", "lwip-DL-only", "lwip-UL-only", "eutran"};
  return convert_enum_idx(options, 4, value, "drb_to_add_mod_s::drb_type_lwip_r13_e_");
}

const char* drb_to_add_mod_s::lwa_wlan_ac_r14_opts::to_string() const
{
  static const char* options[] = {"ac-bk", "ac-be", "ac-vi", "ac-vo"};
  return convert_enum_idx(options, 4, value, "drb_to_add_mod_s::lwa_wlan_ac_r14_e_");
}

// DRX-Config ::= CHOICE
void drx_cfg_c::set(types::options e)
{
  type_ = e;
}
void drx_cfg_c::set_release()
{
  set(types::release);
}
drx_cfg_c::setup_s_& drx_cfg_c::set_setup()
{
  set(types::setup);
  return c;
}
void drx_cfg_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_str("onDurationTimer", c.on_dur_timer.to_string());
      j.write_str("drx-InactivityTimer", c.drx_inactivity_timer.to_string());
      j.write_str("drx-RetransmissionTimer", c.drx_retx_timer.to_string());
      j.write_fieldname("longDRX-CycleStartOffset");
      c.long_drx_cycle_start_offset.to_json(j);
      if (c.short_drx_present) {
        j.write_fieldname("shortDRX");
        j.start_obj();
        j.write_str("shortDRX-Cycle", c.short_drx.short_drx_cycle.to_string());
        j.write_int("drxShortCycleTimer", c.short_drx.drx_short_cycle_timer);
        j.end_obj();
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "drx_cfg_c");
  }
  j.end_obj();
}
SRSASN_CODE drx_cfg_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.short_drx_present, 1));
      HANDLE_CODE(c.on_dur_timer.pack(bref));
      HANDLE_CODE(c.drx_inactivity_timer.pack(bref));
      HANDLE_CODE(c.drx_retx_timer.pack(bref));
      HANDLE_CODE(c.long_drx_cycle_start_offset.pack(bref));
      if (c.short_drx_present) {
        HANDLE_CODE(c.short_drx.short_drx_cycle.pack(bref));
        HANDLE_CODE(pack_integer(bref, c.short_drx.drx_short_cycle_timer, (uint8_t)1u, (uint8_t)16u));
      }
      break;
    default:
      log_invalid_choice_id(type_, "drx_cfg_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE drx_cfg_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.short_drx_present, 1));
      HANDLE_CODE(c.on_dur_timer.unpack(bref));
      HANDLE_CODE(c.drx_inactivity_timer.unpack(bref));
      HANDLE_CODE(c.drx_retx_timer.unpack(bref));
      HANDLE_CODE(c.long_drx_cycle_start_offset.unpack(bref));
      if (c.short_drx_present) {
        HANDLE_CODE(c.short_drx.short_drx_cycle.unpack(bref));
        HANDLE_CODE(unpack_integer(c.short_drx.drx_short_cycle_timer, bref, (uint8_t)1u, (uint8_t)16u));
      }
      break;
    default:
      log_invalid_choice_id(type_, "drx_cfg_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* drx_cfg_c::setup_s_::on_dur_timer_opts::to_string() const
{
  static const char* options[] = {"psf1",
                                  "psf2",
                                  "psf3",
                                  "psf4",
                                  "psf5",
                                  "psf6",
                                  "psf8",
                                  "psf10",
                                  "psf20",
                                  "psf30",
                                  "psf40",
                                  "psf50",
                                  "psf60",
                                  "psf80",
                                  "psf100",
                                  "psf200"};
  return convert_enum_idx(options, 16, value, "drx_cfg_c::setup_s_::on_dur_timer_e_");
}
uint8_t drx_cfg_c::setup_s_::on_dur_timer_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 5, 6, 8, 10, 20, 30, 40, 50, 60, 80, 100, 200};
  return map_enum_number(options, 16, value, "drx_cfg_c::setup_s_::on_dur_timer_e_");
}

const char* drx_cfg_c::setup_s_::drx_inactivity_timer_opts::to_string() const
{
  static const char* options[] = {"psf1",    "psf2",       "psf3",   "psf4",   "psf5",   "psf6",    "psf8",
                                  "psf10",   "psf20",      "psf30",  "psf40",  "psf50",  "psf60",   "psf80",
                                  "psf100",  "psf200",     "psf300", "psf500", "psf750", "psf1280", "psf1920",
                                  "psf2560", "psf0-v1020", "spare9", "spare8", "spare7", "spare6",  "spare5",
                                  "spare4",  "spare3",     "spare2", "spare1"};
  return convert_enum_idx(options, 32, value, "drx_cfg_c::setup_s_::drx_inactivity_timer_e_");
}
uint16_t drx_cfg_c::setup_s_::drx_inactivity_timer_opts::to_number() const
{
  static const uint16_t options[] = {1,  2,  3,   4,   5,   6,   8,   10,   20,   30,   40, 50,
                                     60, 80, 100, 200, 300, 500, 750, 1280, 1920, 2560, 0};
  return map_enum_number(options, 23, value, "drx_cfg_c::setup_s_::drx_inactivity_timer_e_");
}

const char* drx_cfg_c::setup_s_::drx_retx_timer_opts::to_string() const
{
  static const char* options[] = {"psf1", "psf2", "psf4", "psf6", "psf8", "psf16", "psf24", "psf33"};
  return convert_enum_idx(options, 8, value, "drx_cfg_c::setup_s_::drx_retx_timer_e_");
}
uint8_t drx_cfg_c::setup_s_::drx_retx_timer_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4, 6, 8, 16, 24, 33};
  return map_enum_number(options, 8, value, "drx_cfg_c::setup_s_::drx_retx_timer_e_");
}

void drx_cfg_c::setup_s_::long_drx_cycle_start_offset_c_::destroy_() {}
void drx_cfg_c::setup_s_::long_drx_cycle_start_offset_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
drx_cfg_c::setup_s_::long_drx_cycle_start_offset_c_::long_drx_cycle_start_offset_c_(
    const drx_cfg_c::setup_s_::long_drx_cycle_start_offset_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sf10:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf20:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf32:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf40:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf64:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf80:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf128:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf160:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf256:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sf320:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sf512:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sf640:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sf1024:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sf1280:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sf2048:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sf2560:
      c.init(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "drx_cfg_c::setup_s_::long_drx_cycle_start_offset_c_");
  }
}
drx_cfg_c::setup_s_::long_drx_cycle_start_offset_c_& drx_cfg_c::setup_s_::long_drx_cycle_start_offset_c_::operator=(
    const drx_cfg_c::setup_s_::long_drx_cycle_start_offset_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sf10:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf20:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf32:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf40:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf64:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf80:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf128:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf160:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf256:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sf320:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sf512:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sf640:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sf1024:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sf1280:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sf2048:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sf2560:
      c.set(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "drx_cfg_c::setup_s_::long_drx_cycle_start_offset_c_");
  }

  return *this;
}
uint8_t& drx_cfg_c::setup_s_::long_drx_cycle_start_offset_c_::set_sf10()
{
  set(types::sf10);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_c::setup_s_::long_drx_cycle_start_offset_c_::set_sf20()
{
  set(types::sf20);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_c::setup_s_::long_drx_cycle_start_offset_c_::set_sf32()
{
  set(types::sf32);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_c::setup_s_::long_drx_cycle_start_offset_c_::set_sf40()
{
  set(types::sf40);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_c::setup_s_::long_drx_cycle_start_offset_c_::set_sf64()
{
  set(types::sf64);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_c::setup_s_::long_drx_cycle_start_offset_c_::set_sf80()
{
  set(types::sf80);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_c::setup_s_::long_drx_cycle_start_offset_c_::set_sf128()
{
  set(types::sf128);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_c::setup_s_::long_drx_cycle_start_offset_c_::set_sf160()
{
  set(types::sf160);
  return c.get<uint8_t>();
}
uint16_t& drx_cfg_c::setup_s_::long_drx_cycle_start_offset_c_::set_sf256()
{
  set(types::sf256);
  return c.get<uint16_t>();
}
uint16_t& drx_cfg_c::setup_s_::long_drx_cycle_start_offset_c_::set_sf320()
{
  set(types::sf320);
  return c.get<uint16_t>();
}
uint16_t& drx_cfg_c::setup_s_::long_drx_cycle_start_offset_c_::set_sf512()
{
  set(types::sf512);
  return c.get<uint16_t>();
}
uint16_t& drx_cfg_c::setup_s_::long_drx_cycle_start_offset_c_::set_sf640()
{
  set(types::sf640);
  return c.get<uint16_t>();
}
uint16_t& drx_cfg_c::setup_s_::long_drx_cycle_start_offset_c_::set_sf1024()
{
  set(types::sf1024);
  return c.get<uint16_t>();
}
uint16_t& drx_cfg_c::setup_s_::long_drx_cycle_start_offset_c_::set_sf1280()
{
  set(types::sf1280);
  return c.get<uint16_t>();
}
uint16_t& drx_cfg_c::setup_s_::long_drx_cycle_start_offset_c_::set_sf2048()
{
  set(types::sf2048);
  return c.get<uint16_t>();
}
uint16_t& drx_cfg_c::setup_s_::long_drx_cycle_start_offset_c_::set_sf2560()
{
  set(types::sf2560);
  return c.get<uint16_t>();
}
void drx_cfg_c::setup_s_::long_drx_cycle_start_offset_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sf10:
      j.write_int("sf10", c.get<uint8_t>());
      break;
    case types::sf20:
      j.write_int("sf20", c.get<uint8_t>());
      break;
    case types::sf32:
      j.write_int("sf32", c.get<uint8_t>());
      break;
    case types::sf40:
      j.write_int("sf40", c.get<uint8_t>());
      break;
    case types::sf64:
      j.write_int("sf64", c.get<uint8_t>());
      break;
    case types::sf80:
      j.write_int("sf80", c.get<uint8_t>());
      break;
    case types::sf128:
      j.write_int("sf128", c.get<uint8_t>());
      break;
    case types::sf160:
      j.write_int("sf160", c.get<uint8_t>());
      break;
    case types::sf256:
      j.write_int("sf256", c.get<uint16_t>());
      break;
    case types::sf320:
      j.write_int("sf320", c.get<uint16_t>());
      break;
    case types::sf512:
      j.write_int("sf512", c.get<uint16_t>());
      break;
    case types::sf640:
      j.write_int("sf640", c.get<uint16_t>());
      break;
    case types::sf1024:
      j.write_int("sf1024", c.get<uint16_t>());
      break;
    case types::sf1280:
      j.write_int("sf1280", c.get<uint16_t>());
      break;
    case types::sf2048:
      j.write_int("sf2048", c.get<uint16_t>());
      break;
    case types::sf2560:
      j.write_int("sf2560", c.get<uint16_t>());
      break;
    default:
      log_invalid_choice_id(type_, "drx_cfg_c::setup_s_::long_drx_cycle_start_offset_c_");
  }
  j.end_obj();
}
SRSASN_CODE drx_cfg_c::setup_s_::long_drx_cycle_start_offset_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sf10:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)9u));
      break;
    case types::sf20:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)19u));
      break;
    case types::sf32:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)31u));
      break;
    case types::sf40:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)39u));
      break;
    case types::sf64:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)63u));
      break;
    case types::sf80:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)79u));
      break;
    case types::sf128:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)127u));
      break;
    case types::sf160:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)159u));
      break;
    case types::sf256:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u));
      break;
    case types::sf320:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)319u));
      break;
    case types::sf512:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)511u));
      break;
    case types::sf640:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)639u));
      break;
    case types::sf1024:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)1023u));
      break;
    case types::sf1280:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)1279u));
      break;
    case types::sf2048:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)2047u));
      break;
    case types::sf2560:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)2559u));
      break;
    default:
      log_invalid_choice_id(type_, "drx_cfg_c::setup_s_::long_drx_cycle_start_offset_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE drx_cfg_c::setup_s_::long_drx_cycle_start_offset_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sf10:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)9u));
      break;
    case types::sf20:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)19u));
      break;
    case types::sf32:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)31u));
      break;
    case types::sf40:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)39u));
      break;
    case types::sf64:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)63u));
      break;
    case types::sf80:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)79u));
      break;
    case types::sf128:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)127u));
      break;
    case types::sf160:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)159u));
      break;
    case types::sf256:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u));
      break;
    case types::sf320:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)319u));
      break;
    case types::sf512:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)511u));
      break;
    case types::sf640:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)639u));
      break;
    case types::sf1024:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)1023u));
      break;
    case types::sf1280:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)1279u));
      break;
    case types::sf2048:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)2047u));
      break;
    case types::sf2560:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)2559u));
      break;
    default:
      log_invalid_choice_id(type_, "drx_cfg_c::setup_s_::long_drx_cycle_start_offset_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* drx_cfg_c::setup_s_::short_drx_s_::short_drx_cycle_opts::to_string() const
{
  static const char* options[] = {"sf2",
                                  "sf5",
                                  "sf8",
                                  "sf10",
                                  "sf16",
                                  "sf20",
                                  "sf32",
                                  "sf40",
                                  "sf64",
                                  "sf80",
                                  "sf128",
                                  "sf160",
                                  "sf256",
                                  "sf320",
                                  "sf512",
                                  "sf640"};
  return convert_enum_idx(options, 16, value, "drx_cfg_c::setup_s_::short_drx_s_::short_drx_cycle_e_");
}
uint16_t drx_cfg_c::setup_s_::short_drx_s_::short_drx_cycle_opts::to_number() const
{
  static const uint16_t options[] = {2, 5, 8, 10, 16, 20, 32, 40, 64, 80, 128, 160, 256, 320, 512, 640};
  return map_enum_number(options, 16, value, "drx_cfg_c::setup_s_::short_drx_s_::short_drx_cycle_e_");
}

// DRX-Config-r13 ::= SEQUENCE
SRSASN_CODE drx_cfg_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(on_dur_timer_v1310_present, 1));
  HANDLE_CODE(bref.pack(drx_retx_timer_v1310_present, 1));
  HANDLE_CODE(bref.pack(drx_ul_retx_timer_r13_present, 1));

  if (on_dur_timer_v1310_present) {
    HANDLE_CODE(on_dur_timer_v1310.pack(bref));
  }
  if (drx_retx_timer_v1310_present) {
    HANDLE_CODE(drx_retx_timer_v1310.pack(bref));
  }
  if (drx_ul_retx_timer_r13_present) {
    HANDLE_CODE(drx_ul_retx_timer_r13.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE drx_cfg_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(on_dur_timer_v1310_present, 1));
  HANDLE_CODE(bref.unpack(drx_retx_timer_v1310_present, 1));
  HANDLE_CODE(bref.unpack(drx_ul_retx_timer_r13_present, 1));

  if (on_dur_timer_v1310_present) {
    HANDLE_CODE(on_dur_timer_v1310.unpack(bref));
  }
  if (drx_retx_timer_v1310_present) {
    HANDLE_CODE(drx_retx_timer_v1310.unpack(bref));
  }
  if (drx_ul_retx_timer_r13_present) {
    HANDLE_CODE(drx_ul_retx_timer_r13.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void drx_cfg_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (on_dur_timer_v1310_present) {
    j.write_str("onDurationTimer-v1310", on_dur_timer_v1310.to_string());
  }
  if (drx_retx_timer_v1310_present) {
    j.write_str("drx-RetransmissionTimer-v1310", drx_retx_timer_v1310.to_string());
  }
  if (drx_ul_retx_timer_r13_present) {
    j.write_str("drx-ULRetransmissionTimer-r13", drx_ul_retx_timer_r13.to_string());
  }
  j.end_obj();
}

const char* drx_cfg_r13_s::on_dur_timer_v1310_opts::to_string() const
{
  static const char* options[] = {"psf300", "psf400", "psf500", "psf600", "psf800", "psf1000", "psf1200", "psf1600"};
  return convert_enum_idx(options, 8, value, "drx_cfg_r13_s::on_dur_timer_v1310_e_");
}
uint16_t drx_cfg_r13_s::on_dur_timer_v1310_opts::to_number() const
{
  static const uint16_t options[] = {300, 400, 500, 600, 800, 1000, 1200, 1600};
  return map_enum_number(options, 8, value, "drx_cfg_r13_s::on_dur_timer_v1310_e_");
}

const char* drx_cfg_r13_s::drx_retx_timer_v1310_opts::to_string() const
{
  static const char* options[] = {"psf40", "psf64", "psf80", "psf96", "psf112", "psf128", "psf160", "psf320"};
  return convert_enum_idx(options, 8, value, "drx_cfg_r13_s::drx_retx_timer_v1310_e_");
}
uint16_t drx_cfg_r13_s::drx_retx_timer_v1310_opts::to_number() const
{
  static const uint16_t options[] = {40, 64, 80, 96, 112, 128, 160, 320};
  return map_enum_number(options, 8, value, "drx_cfg_r13_s::drx_retx_timer_v1310_e_");
}

const char* drx_cfg_r13_s::drx_ul_retx_timer_r13_opts::to_string() const
{
  static const char* options[] = {"psf0",
                                  "psf1",
                                  "psf2",
                                  "psf4",
                                  "psf6",
                                  "psf8",
                                  "psf16",
                                  "psf24",
                                  "psf33",
                                  "psf40",
                                  "psf64",
                                  "psf80",
                                  "psf96",
                                  "psf112",
                                  "psf128",
                                  "psf160",
                                  "psf320"};
  return convert_enum_idx(options, 17, value, "drx_cfg_r13_s::drx_ul_retx_timer_r13_e_");
}
uint16_t drx_cfg_r13_s::drx_ul_retx_timer_r13_opts::to_number() const
{
  static const uint16_t options[] = {0, 1, 2, 4, 6, 8, 16, 24, 33, 40, 64, 80, 96, 112, 128, 160, 320};
  return map_enum_number(options, 17, value, "drx_cfg_r13_s::drx_ul_retx_timer_r13_e_");
}

// DRX-Config-r15 ::= SEQUENCE
SRSASN_CODE drx_cfg_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(drx_retx_timer_short_tti_r15_present, 1));
  HANDLE_CODE(bref.pack(drx_ul_retx_timer_short_tti_r15_present, 1));

  if (drx_retx_timer_short_tti_r15_present) {
    HANDLE_CODE(drx_retx_timer_short_tti_r15.pack(bref));
  }
  if (drx_ul_retx_timer_short_tti_r15_present) {
    HANDLE_CODE(drx_ul_retx_timer_short_tti_r15.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE drx_cfg_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(drx_retx_timer_short_tti_r15_present, 1));
  HANDLE_CODE(bref.unpack(drx_ul_retx_timer_short_tti_r15_present, 1));

  if (drx_retx_timer_short_tti_r15_present) {
    HANDLE_CODE(drx_retx_timer_short_tti_r15.unpack(bref));
  }
  if (drx_ul_retx_timer_short_tti_r15_present) {
    HANDLE_CODE(drx_ul_retx_timer_short_tti_r15.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void drx_cfg_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (drx_retx_timer_short_tti_r15_present) {
    j.write_str("drx-RetransmissionTimerShortTTI-r15", drx_retx_timer_short_tti_r15.to_string());
  }
  if (drx_ul_retx_timer_short_tti_r15_present) {
    j.write_str("drx-UL-RetransmissionTimerShortTTI-r15", drx_ul_retx_timer_short_tti_r15.to_string());
  }
  j.end_obj();
}

const char* drx_cfg_r15_s::drx_retx_timer_short_tti_r15_opts::to_string() const
{
  static const char* options[] = {
      "tti10", "tti20", "tti40", "tti64", "tti80", "tti96", "tti112", "tti128", "tti160", "tti320"};
  return convert_enum_idx(options, 10, value, "drx_cfg_r15_s::drx_retx_timer_short_tti_r15_e_");
}
uint16_t drx_cfg_r15_s::drx_retx_timer_short_tti_r15_opts::to_number() const
{
  static const uint16_t options[] = {10, 20, 40, 64, 80, 96, 112, 128, 160, 320};
  return map_enum_number(options, 10, value, "drx_cfg_r15_s::drx_retx_timer_short_tti_r15_e_");
}

const char* drx_cfg_r15_s::drx_ul_retx_timer_short_tti_r15_opts::to_string() const
{
  static const char* options[] = {"tti0",
                                  "tti1",
                                  "tti2",
                                  "tti4",
                                  "tti6",
                                  "tti8",
                                  "tti16",
                                  "tti24",
                                  "tti33",
                                  "tti40",
                                  "tti64",
                                  "tti80",
                                  "tti96",
                                  "tti112",
                                  "tti128",
                                  "tti160",
                                  "tti320"};
  return convert_enum_idx(options, 17, value, "drx_cfg_r15_s::drx_ul_retx_timer_short_tti_r15_e_");
}
uint16_t drx_cfg_r15_s::drx_ul_retx_timer_short_tti_r15_opts::to_number() const
{
  static const uint16_t options[] = {0, 1, 2, 4, 6, 8, 16, 24, 33, 40, 64, 80, 96, 112, 128, 160, 320};
  return map_enum_number(options, 17, value, "drx_cfg_r15_s::drx_ul_retx_timer_short_tti_r15_e_");
}

// DRX-Config-v1130 ::= SEQUENCE
SRSASN_CODE drx_cfg_v1130_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(drx_retx_timer_v1130_present, 1));
  HANDLE_CODE(bref.pack(long_drx_cycle_start_offset_v1130_present, 1));
  HANDLE_CODE(bref.pack(short_drx_cycle_v1130_present, 1));

  if (long_drx_cycle_start_offset_v1130_present) {
    HANDLE_CODE(long_drx_cycle_start_offset_v1130.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE drx_cfg_v1130_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(drx_retx_timer_v1130_present, 1));
  HANDLE_CODE(bref.unpack(long_drx_cycle_start_offset_v1130_present, 1));
  HANDLE_CODE(bref.unpack(short_drx_cycle_v1130_present, 1));

  if (long_drx_cycle_start_offset_v1130_present) {
    HANDLE_CODE(long_drx_cycle_start_offset_v1130.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void drx_cfg_v1130_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (drx_retx_timer_v1130_present) {
    j.write_str("drx-RetransmissionTimer-v1130", "psf0-v1130");
  }
  if (long_drx_cycle_start_offset_v1130_present) {
    j.write_fieldname("longDRX-CycleStartOffset-v1130");
    long_drx_cycle_start_offset_v1130.to_json(j);
  }
  if (short_drx_cycle_v1130_present) {
    j.write_str("shortDRX-Cycle-v1130", "sf4-v1130");
  }
  j.end_obj();
}

void drx_cfg_v1130_s::long_drx_cycle_start_offset_v1130_c_::destroy_() {}
void drx_cfg_v1130_s::long_drx_cycle_start_offset_v1130_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
drx_cfg_v1130_s::long_drx_cycle_start_offset_v1130_c_::long_drx_cycle_start_offset_v1130_c_(
    const drx_cfg_v1130_s::long_drx_cycle_start_offset_v1130_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sf60_v1130:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf70_v1130:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "drx_cfg_v1130_s::long_drx_cycle_start_offset_v1130_c_");
  }
}
drx_cfg_v1130_s::long_drx_cycle_start_offset_v1130_c_& drx_cfg_v1130_s::long_drx_cycle_start_offset_v1130_c_::operator=(
    const drx_cfg_v1130_s::long_drx_cycle_start_offset_v1130_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sf60_v1130:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf70_v1130:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "drx_cfg_v1130_s::long_drx_cycle_start_offset_v1130_c_");
  }

  return *this;
}
uint8_t& drx_cfg_v1130_s::long_drx_cycle_start_offset_v1130_c_::set_sf60_v1130()
{
  set(types::sf60_v1130);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_v1130_s::long_drx_cycle_start_offset_v1130_c_::set_sf70_v1130()
{
  set(types::sf70_v1130);
  return c.get<uint8_t>();
}
void drx_cfg_v1130_s::long_drx_cycle_start_offset_v1130_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sf60_v1130:
      j.write_int("sf60-v1130", c.get<uint8_t>());
      break;
    case types::sf70_v1130:
      j.write_int("sf70-v1130", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "drx_cfg_v1130_s::long_drx_cycle_start_offset_v1130_c_");
  }
  j.end_obj();
}
SRSASN_CODE drx_cfg_v1130_s::long_drx_cycle_start_offset_v1130_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sf60_v1130:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)59u));
      break;
    case types::sf70_v1130:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)69u));
      break;
    default:
      log_invalid_choice_id(type_, "drx_cfg_v1130_s::long_drx_cycle_start_offset_v1130_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE drx_cfg_v1130_s::long_drx_cycle_start_offset_v1130_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sf60_v1130:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)59u));
      break;
    case types::sf70_v1130:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)69u));
      break;
    default:
      log_invalid_choice_id(type_, "drx_cfg_v1130_s::long_drx_cycle_start_offset_v1130_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// DRX-Config-v1310 ::= SEQUENCE
SRSASN_CODE drx_cfg_v1310_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(long_drx_cycle_start_offset_v1310_present, 1));

  if (long_drx_cycle_start_offset_v1310_present) {
    HANDLE_CODE(pack_integer(bref, long_drx_cycle_start_offset_v1310.sf60_v1310, (uint8_t)0u, (uint8_t)59u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE drx_cfg_v1310_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(long_drx_cycle_start_offset_v1310_present, 1));

  if (long_drx_cycle_start_offset_v1310_present) {
    HANDLE_CODE(unpack_integer(long_drx_cycle_start_offset_v1310.sf60_v1310, bref, (uint8_t)0u, (uint8_t)59u));
  }

  return SRSASN_SUCCESS;
}
void drx_cfg_v1310_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (long_drx_cycle_start_offset_v1310_present) {
    j.write_fieldname("longDRX-CycleStartOffset-v1310");
    j.start_obj();
    j.write_int("sf60-v1310", long_drx_cycle_start_offset_v1310.sf60_v1310);
    j.end_obj();
  }
  j.end_obj();
}

// DataInactivityTimer-r14 ::= ENUMERATED
const char* data_inactivity_timer_r14_opts::to_string() const
{
  static const char* options[] = {
      "s1", "s2", "s3", "s5", "s7", "s10", "s15", "s20", "s40", "s50", "s60", "s80", "s100", "s120", "s150", "s180"};
  return convert_enum_idx(options, 16, value, "data_inactivity_timer_r14_e");
}
uint8_t data_inactivity_timer_r14_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 5, 7, 10, 15, 20, 40, 50, 60, 80, 100, 120, 150, 180};
  return map_enum_number(options, 16, value, "data_inactivity_timer_r14_e");
}

// OffsetThresholdTA-r17 ::= ENUMERATED
const char* offset_thres_ta_r17_opts::to_string() const
{
  static const char* options[] = {"ms0dot5",
                                  "ms1",
                                  "ms2",
                                  "ms3",
                                  "ms4",
                                  "ms5",
                                  "ms6",
                                  "ms7",
                                  "ms8",
                                  "ms9",
                                  "ms10",
                                  "ms11",
                                  "ms12",
                                  "ms13",
                                  "ms14",
                                  "ms15"};
  return convert_enum_idx(options, 16, value, "offset_thres_ta_r17_e");
}
float offset_thres_ta_r17_opts::to_number() const
{
  static const float options[] = {0.5, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0};
  return map_enum_number(options, 16, value, "offset_thres_ta_r17_e");
}
const char* offset_thres_ta_r17_opts::to_number_string() const
{
  static const char* options[] = {
      "0.5", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15"};
  return convert_enum_idx(options, 16, value, "offset_thres_ta_r17_e");
}

// PeriodicBSR-Timer-r12 ::= ENUMERATED
const char* periodic_bsr_timer_r12_opts::to_string() const
{
  static const char* options[] = {"sf5",
                                  "sf10",
                                  "sf16",
                                  "sf20",
                                  "sf32",
                                  "sf40",
                                  "sf64",
                                  "sf80",
                                  "sf128",
                                  "sf160",
                                  "sf320",
                                  "sf640",
                                  "sf1280",
                                  "sf2560",
                                  "infinity",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "periodic_bsr_timer_r12_e");
}
int16_t periodic_bsr_timer_r12_opts::to_number() const
{
  static const int16_t options[] = {5, 10, 16, 20, 32, 40, 64, 80, 128, 160, 320, 640, 1280, 2560, -1};
  return map_enum_number(options, 15, value, "periodic_bsr_timer_r12_e");
}

// RetxBSR-Timer-r12 ::= ENUMERATED
const char* retx_bsr_timer_r12_opts::to_string() const
{
  static const char* options[] = {"sf320", "sf640", "sf1280", "sf2560", "sf5120", "sf10240", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "retx_bsr_timer_r12_e");
}
uint16_t retx_bsr_timer_r12_opts::to_number() const
{
  static const uint16_t options[] = {320, 640, 1280, 2560, 5120, 10240};
  return map_enum_number(options, 6, value, "retx_bsr_timer_r12_e");
}

// SPS-ConfigDL ::= CHOICE
void sps_cfg_dl_c::set(types::options e)
{
  type_ = e;
}
void sps_cfg_dl_c::set_release()
{
  set(types::release);
}
sps_cfg_dl_c::setup_s_& sps_cfg_dl_c::set_setup()
{
  set(types::setup);
  return c;
}
void sps_cfg_dl_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      c.to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "sps_cfg_dl_c");
  }
  j.end_obj();
}
SRSASN_CODE sps_cfg_dl_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sps_cfg_dl_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sps_cfg_dl_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sps_cfg_dl_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool sps_cfg_dl_c::operator==(const sps_cfg_dl_c& other) const
{
  return type() == other.type() and c == other.c;
}

SRSASN_CODE sps_cfg_dl_c::setup_s_::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(semi_persist_sched_interv_dl.pack(bref));
  HANDLE_CODE(pack_integer(bref, nof_conf_sps_processes, (uint8_t)1u, (uint8_t)8u));
  HANDLE_CODE(pack_dyn_seq_of(bref, n1_pucch_an_persistent_list, 1, 4, integer_packer<uint16_t>(0, 2047)));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= two_ant_port_activ_r10.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(two_ant_port_activ_r10.is_present(), 1));
      if (two_ant_port_activ_r10.is_present()) {
        HANDLE_CODE(two_ant_port_activ_r10->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sps_cfg_dl_c::setup_s_::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(semi_persist_sched_interv_dl.unpack(bref));
  HANDLE_CODE(unpack_integer(nof_conf_sps_processes, bref, (uint8_t)1u, (uint8_t)8u));
  HANDLE_CODE(unpack_dyn_seq_of(n1_pucch_an_persistent_list, bref, 1, 4, integer_packer<uint16_t>(0, 2047)));

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool two_ant_port_activ_r10_present;
      HANDLE_CODE(bref.unpack(two_ant_port_activ_r10_present, 1));
      two_ant_port_activ_r10.set_present(two_ant_port_activ_r10_present);
      if (two_ant_port_activ_r10.is_present()) {
        HANDLE_CODE(two_ant_port_activ_r10->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sps_cfg_dl_c::setup_s_::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("semiPersistSchedIntervalDL", semi_persist_sched_interv_dl.to_string());
  j.write_int("numberOfConfSPS-Processes", nof_conf_sps_processes);
  j.start_array("n1PUCCH-AN-PersistentList");
  for (const auto& e1 : n1_pucch_an_persistent_list) {
    j.write_int(e1);
  }
  j.end_array();
  if (ext) {
    if (two_ant_port_activ_r10.is_present()) {
      j.write_fieldname("twoAntennaPortActivated-r10");
      two_ant_port_activ_r10->to_json(j);
    }
  }
  j.end_obj();
}
bool sps_cfg_dl_c::setup_s_::operator==(const setup_s_& other) const
{
  return ext == other.ext and semi_persist_sched_interv_dl == other.semi_persist_sched_interv_dl and
         nof_conf_sps_processes == other.nof_conf_sps_processes and
         n1_pucch_an_persistent_list == other.n1_pucch_an_persistent_list and
         (not ext or
          (two_ant_port_activ_r10.is_present() == other.two_ant_port_activ_r10.is_present() and
           (not two_ant_port_activ_r10.is_present() or *two_ant_port_activ_r10 == *other.two_ant_port_activ_r10)));
}

const char* sps_cfg_dl_c::setup_s_::semi_persist_sched_interv_dl_opts::to_string() const
{
  static const char* options[] = {"sf10",
                                  "sf20",
                                  "sf32",
                                  "sf40",
                                  "sf64",
                                  "sf80",
                                  "sf128",
                                  "sf160",
                                  "sf320",
                                  "sf640",
                                  "spare6",
                                  "spare5",
                                  "spare4",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "sps_cfg_dl_c::setup_s_::semi_persist_sched_interv_dl_e_");
}
uint16_t sps_cfg_dl_c::setup_s_::semi_persist_sched_interv_dl_opts::to_number() const
{
  static const uint16_t options[] = {10, 20, 32, 40, 64, 80, 128, 160, 320, 640};
  return map_enum_number(options, 10, value, "sps_cfg_dl_c::setup_s_::semi_persist_sched_interv_dl_e_");
}

void sps_cfg_dl_c::setup_s_::two_ant_port_activ_r10_c_::set(types::options e)
{
  type_ = e;
}
void sps_cfg_dl_c::setup_s_::two_ant_port_activ_r10_c_::set_release()
{
  set(types::release);
}
sps_cfg_dl_c::setup_s_::two_ant_port_activ_r10_c_::setup_s_&
sps_cfg_dl_c::setup_s_::two_ant_port_activ_r10_c_::set_setup()
{
  set(types::setup);
  return c;
}
void sps_cfg_dl_c::setup_s_::two_ant_port_activ_r10_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.start_array("n1PUCCH-AN-PersistentListP1-r10");
      for (const auto& e1 : c.n1_pucch_an_persistent_list_p1_r10) {
        j.write_int(e1);
      }
      j.end_array();
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "sps_cfg_dl_c::setup_s_::two_ant_port_activ_r10_c_");
  }
  j.end_obj();
}
SRSASN_CODE sps_cfg_dl_c::setup_s_::two_ant_port_activ_r10_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.n1_pucch_an_persistent_list_p1_r10, 1, 4, integer_packer<uint16_t>(0, 2047)));
      break;
    default:
      log_invalid_choice_id(type_, "sps_cfg_dl_c::setup_s_::two_ant_port_activ_r10_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sps_cfg_dl_c::setup_s_::two_ant_port_activ_r10_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(
          unpack_dyn_seq_of(c.n1_pucch_an_persistent_list_p1_r10, bref, 1, 4, integer_packer<uint16_t>(0, 2047)));
      break;
    default:
      log_invalid_choice_id(type_, "sps_cfg_dl_c::setup_s_::two_ant_port_activ_r10_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool sps_cfg_dl_c::setup_s_::two_ant_port_activ_r10_c_::operator==(const two_ant_port_activ_r10_c_& other) const
{
  return type() == other.type() and c.n1_pucch_an_persistent_list_p1_r10 == other.c.n1_pucch_an_persistent_list_p1_r10;
}

// SPS-ConfigDL-STTI-r15 ::= CHOICE
void sps_cfg_dl_stti_r15_c::set(types::options e)
{
  type_ = e;
}
void sps_cfg_dl_stti_r15_c::set_release()
{
  set(types::release);
}
sps_cfg_dl_stti_r15_c::setup_s_& sps_cfg_dl_stti_r15_c::set_setup()
{
  set(types::setup);
  return c;
}
void sps_cfg_dl_stti_r15_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_str("semiPersistSchedIntervalDL-STTI-r15", c.semi_persist_sched_interv_dl_stti_r15.to_string());
      j.write_int("numberOfConfSPS-Processes-STTI-r15", c.nof_conf_sps_processes_stti_r15);
      if (c.two_ant_port_activ_r15_present) {
        j.write_fieldname("twoAntennaPortActivated-r15");
        c.two_ant_port_activ_r15.to_json(j);
      }
      j.write_int("sTTI-StartTimeDL-r15", c.stti_start_time_dl_r15);
      if (c.tpc_pdcch_cfg_pucch_sps_r15_present) {
        j.write_fieldname("tpc-PDCCH-ConfigPUCCH-SPS-r15");
        c.tpc_pdcch_cfg_pucch_sps_r15.to_json(j);
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "sps_cfg_dl_stti_r15_c");
  }
  j.end_obj();
}
SRSASN_CODE sps_cfg_dl_stti_r15_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      bref.pack(c.ext, 1);
      HANDLE_CODE(bref.pack(c.two_ant_port_activ_r15_present, 1));
      HANDLE_CODE(bref.pack(c.tpc_pdcch_cfg_pucch_sps_r15_present, 1));
      HANDLE_CODE(c.semi_persist_sched_interv_dl_stti_r15.pack(bref));
      HANDLE_CODE(pack_integer(bref, c.nof_conf_sps_processes_stti_r15, (uint8_t)1u, (uint8_t)12u));
      if (c.two_ant_port_activ_r15_present) {
        HANDLE_CODE(c.two_ant_port_activ_r15.pack(bref));
      }
      HANDLE_CODE(pack_integer(bref, c.stti_start_time_dl_r15, (uint8_t)0u, (uint8_t)5u));
      if (c.tpc_pdcch_cfg_pucch_sps_r15_present) {
        HANDLE_CODE(c.tpc_pdcch_cfg_pucch_sps_r15.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "sps_cfg_dl_stti_r15_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sps_cfg_dl_stti_r15_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      bref.unpack(c.ext, 1);
      HANDLE_CODE(bref.unpack(c.two_ant_port_activ_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.tpc_pdcch_cfg_pucch_sps_r15_present, 1));
      HANDLE_CODE(c.semi_persist_sched_interv_dl_stti_r15.unpack(bref));
      HANDLE_CODE(unpack_integer(c.nof_conf_sps_processes_stti_r15, bref, (uint8_t)1u, (uint8_t)12u));
      if (c.two_ant_port_activ_r15_present) {
        HANDLE_CODE(c.two_ant_port_activ_r15.unpack(bref));
      }
      HANDLE_CODE(unpack_integer(c.stti_start_time_dl_r15, bref, (uint8_t)0u, (uint8_t)5u));
      if (c.tpc_pdcch_cfg_pucch_sps_r15_present) {
        HANDLE_CODE(c.tpc_pdcch_cfg_pucch_sps_r15.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "sps_cfg_dl_stti_r15_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* sps_cfg_dl_stti_r15_c::setup_s_::semi_persist_sched_interv_dl_stti_r15_opts::to_string() const
{
  static const char* options[] = {"sTTI1",
                                  "sTTI2",
                                  "sTTI3",
                                  "sTTI4",
                                  "sTTI6",
                                  "sTTI8",
                                  "sTTI12",
                                  "sTTI16",
                                  "sTTI20",
                                  "sTTI40",
                                  "sTTI60",
                                  "sTTI80",
                                  "sTTI120",
                                  "sTTI240",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(
      options, 16, value, "sps_cfg_dl_stti_r15_c::setup_s_::semi_persist_sched_interv_dl_stti_r15_e_");
}
uint8_t sps_cfg_dl_stti_r15_c::setup_s_::semi_persist_sched_interv_dl_stti_r15_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 6, 8, 12, 16, 20, 40, 60, 80, 120, 240};
  return map_enum_number(
      options, 14, value, "sps_cfg_dl_stti_r15_c::setup_s_::semi_persist_sched_interv_dl_stti_r15_e_");
}

void sps_cfg_dl_stti_r15_c::setup_s_::two_ant_port_activ_r15_c_::set(types::options e)
{
  type_ = e;
}
void sps_cfg_dl_stti_r15_c::setup_s_::two_ant_port_activ_r15_c_::set_release()
{
  set(types::release);
}
sps_cfg_dl_stti_r15_c::setup_s_::two_ant_port_activ_r15_c_::setup_s_&
sps_cfg_dl_stti_r15_c::setup_s_::two_ant_port_activ_r15_c_::set_setup()
{
  set(types::setup);
  return c;
}
void sps_cfg_dl_stti_r15_c::setup_s_::two_ant_port_activ_r15_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.start_array("n1SPUCCH-AN-PersistentListP1-r15");
      for (const auto& e1 : c.n1_spucch_an_persistent_list_p1_r15) {
        j.write_int(e1);
      }
      j.end_array();
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "sps_cfg_dl_stti_r15_c::setup_s_::two_ant_port_activ_r15_c_");
  }
  j.end_obj();
}
SRSASN_CODE sps_cfg_dl_stti_r15_c::setup_s_::two_ant_port_activ_r15_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(
          pack_dyn_seq_of(bref, c.n1_spucch_an_persistent_list_p1_r15, 1, 4, integer_packer<uint16_t>(0, 2047)));
      break;
    default:
      log_invalid_choice_id(type_, "sps_cfg_dl_stti_r15_c::setup_s_::two_ant_port_activ_r15_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sps_cfg_dl_stti_r15_c::setup_s_::two_ant_port_activ_r15_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(
          unpack_dyn_seq_of(c.n1_spucch_an_persistent_list_p1_r15, bref, 1, 4, integer_packer<uint16_t>(0, 2047)));
      break;
    default:
      log_invalid_choice_id(type_, "sps_cfg_dl_stti_r15_c::setup_s_::two_ant_port_activ_r15_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// SR-ProhibitTimerOffset-r17 ::= ENUMERATED
const char* sr_prohibit_timer_offset_r17_opts::to_string() const
{
  static const char* options[] = {"ms90", "ms180", "ms270", "ms360", "ms450", "ms540", "ms1080", "spare"};
  return convert_enum_idx(options, 8, value, "sr_prohibit_timer_offset_r17_e");
}
uint16_t sr_prohibit_timer_offset_r17_opts::to_number() const
{
  static const uint16_t options[] = {90, 180, 270, 360, 450, 540, 1080};
  return map_enum_number(options, 7, value, "sr_prohibit_timer_offset_r17_e");
}

// SRB-ToAddMod ::= SEQUENCE
SRSASN_CODE srb_to_add_mod_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(rlc_cfg_present, 1));
  HANDLE_CODE(bref.pack(lc_ch_cfg_present, 1));

  HANDLE_CODE(pack_integer(bref, srb_id, (uint8_t)1u, (uint8_t)2u));
  if (rlc_cfg_present) {
    HANDLE_CODE(rlc_cfg.pack(bref));
  }
  if (lc_ch_cfg_present) {
    HANDLE_CODE(lc_ch_cfg.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= pdcp_ver_change_r15_present;
    group_flags[0] |= rlc_cfg_v1530.is_present();
    group_flags[0] |= rlc_bearer_cfg_secondary_r15.is_present();
    group_flags[0] |= srb_id_v1530_present;
    group_flags[1] |= rlc_cfg_v1560.is_present();
    group_flags[2] |= rlc_cfg_v1700.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(pdcp_ver_change_r15_present, 1));
      HANDLE_CODE(bref.pack(rlc_cfg_v1530.is_present(), 1));
      HANDLE_CODE(bref.pack(rlc_bearer_cfg_secondary_r15.is_present(), 1));
      HANDLE_CODE(bref.pack(srb_id_v1530_present, 1));
      if (rlc_cfg_v1530.is_present()) {
        HANDLE_CODE(rlc_cfg_v1530->pack(bref));
      }
      if (rlc_bearer_cfg_secondary_r15.is_present()) {
        HANDLE_CODE(rlc_bearer_cfg_secondary_r15->pack(bref));
      }
      if (srb_id_v1530_present) {
        HANDLE_CODE(pack_integer(bref, srb_id_v1530, (uint8_t)4u, (uint8_t)4u));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(rlc_cfg_v1560.is_present(), 1));
      if (rlc_cfg_v1560.is_present()) {
        HANDLE_CODE(rlc_cfg_v1560->pack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(rlc_cfg_v1700.is_present(), 1));
      if (rlc_cfg_v1700.is_present()) {
        HANDLE_CODE(rlc_cfg_v1700->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE srb_to_add_mod_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(rlc_cfg_present, 1));
  HANDLE_CODE(bref.unpack(lc_ch_cfg_present, 1));

  HANDLE_CODE(unpack_integer(srb_id, bref, (uint8_t)1u, (uint8_t)2u));
  if (rlc_cfg_present) {
    HANDLE_CODE(rlc_cfg.unpack(bref));
  }
  if (lc_ch_cfg_present) {
    HANDLE_CODE(lc_ch_cfg.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(3);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(pdcp_ver_change_r15_present, 1));
      bool rlc_cfg_v1530_present;
      HANDLE_CODE(bref.unpack(rlc_cfg_v1530_present, 1));
      rlc_cfg_v1530.set_present(rlc_cfg_v1530_present);
      bool rlc_bearer_cfg_secondary_r15_present;
      HANDLE_CODE(bref.unpack(rlc_bearer_cfg_secondary_r15_present, 1));
      rlc_bearer_cfg_secondary_r15.set_present(rlc_bearer_cfg_secondary_r15_present);
      HANDLE_CODE(bref.unpack(srb_id_v1530_present, 1));
      if (rlc_cfg_v1530.is_present()) {
        HANDLE_CODE(rlc_cfg_v1530->unpack(bref));
      }
      if (rlc_bearer_cfg_secondary_r15.is_present()) {
        HANDLE_CODE(rlc_bearer_cfg_secondary_r15->unpack(bref));
      }
      if (srb_id_v1530_present) {
        HANDLE_CODE(unpack_integer(srb_id_v1530, bref, (uint8_t)4u, (uint8_t)4u));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool rlc_cfg_v1560_present;
      HANDLE_CODE(bref.unpack(rlc_cfg_v1560_present, 1));
      rlc_cfg_v1560.set_present(rlc_cfg_v1560_present);
      if (rlc_cfg_v1560.is_present()) {
        HANDLE_CODE(rlc_cfg_v1560->unpack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool rlc_cfg_v1700_present;
      HANDLE_CODE(bref.unpack(rlc_cfg_v1700_present, 1));
      rlc_cfg_v1700.set_present(rlc_cfg_v1700_present);
      if (rlc_cfg_v1700.is_present()) {
        HANDLE_CODE(rlc_cfg_v1700->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void srb_to_add_mod_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("srb-Identity", srb_id);
  if (rlc_cfg_present) {
    j.write_fieldname("rlc-Config");
    rlc_cfg.to_json(j);
  }
  if (lc_ch_cfg_present) {
    j.write_fieldname("logicalChannelConfig");
    lc_ch_cfg.to_json(j);
  }
  if (ext) {
    if (pdcp_ver_change_r15_present) {
      j.write_str("pdcp-verChange-r15", "true");
    }
    if (rlc_cfg_v1530.is_present()) {
      j.write_fieldname("rlc-Config-v1530");
      rlc_cfg_v1530->to_json(j);
    }
    if (rlc_bearer_cfg_secondary_r15.is_present()) {
      j.write_fieldname("rlc-BearerConfigSecondary-r15");
      rlc_bearer_cfg_secondary_r15->to_json(j);
    }
    if (srb_id_v1530_present) {
      j.write_int("srb-Identity-v1530", srb_id_v1530);
    }
    if (rlc_cfg_v1560.is_present()) {
      j.write_fieldname("rlc-Config-v1560");
      rlc_cfg_v1560->to_json(j);
    }
    if (rlc_cfg_v1700.is_present()) {
      j.write_fieldname("rlc-Config-v1700");
      rlc_cfg_v1700->to_json(j);
    }
  }
  j.end_obj();
}
bool srb_to_add_mod_s::operator==(const srb_to_add_mod_s& other) const
{
  return ext == other.ext and srb_id == other.srb_id and rlc_cfg_present == other.rlc_cfg_present and
         (not rlc_cfg_present or rlc_cfg == other.rlc_cfg) and lc_ch_cfg_present == other.lc_ch_cfg_present and
         (not lc_ch_cfg_present or lc_ch_cfg == other.lc_ch_cfg) and
         (not ext or (pdcp_ver_change_r15_present == other.pdcp_ver_change_r15_present and
                      rlc_cfg_v1530.is_present() == other.rlc_cfg_v1530.is_present() and
                      (not rlc_cfg_v1530.is_present() or *rlc_cfg_v1530 == *other.rlc_cfg_v1530) and
                      rlc_bearer_cfg_secondary_r15.is_present() == other.rlc_bearer_cfg_secondary_r15.is_present() and
                      (not rlc_bearer_cfg_secondary_r15.is_present() or
                       *rlc_bearer_cfg_secondary_r15 == *other.rlc_bearer_cfg_secondary_r15) and
                      srb_id_v1530_present == other.srb_id_v1530_present and
                      (not srb_id_v1530_present or srb_id_v1530 == other.srb_id_v1530) and
                      rlc_cfg_v1560.is_present() == other.rlc_cfg_v1560.is_present() and
                      (not rlc_cfg_v1560.is_present() or *rlc_cfg_v1560 == *other.rlc_cfg_v1560) and
                      rlc_cfg_v1700.is_present() == other.rlc_cfg_v1700.is_present() and
                      (not rlc_cfg_v1700.is_present() or *rlc_cfg_v1700 == *other.rlc_cfg_v1700)));
}

void srb_to_add_mod_s::rlc_cfg_c_::set(types::options e)
{
  type_ = e;
}
rlc_cfg_c& srb_to_add_mod_s::rlc_cfg_c_::set_explicit_value()
{
  set(types::explicit_value);
  return c;
}
void srb_to_add_mod_s::rlc_cfg_c_::set_default_value()
{
  set(types::default_value);
}
void srb_to_add_mod_s::rlc_cfg_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::explicit_value:
      j.write_fieldname("explicitValue");
      c.to_json(j);
      break;
    case types::default_value:
      break;
    default:
      log_invalid_choice_id(type_, "srb_to_add_mod_s::rlc_cfg_c_");
  }
  j.end_obj();
}
SRSASN_CODE srb_to_add_mod_s::rlc_cfg_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::explicit_value:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::default_value:
      break;
    default:
      log_invalid_choice_id(type_, "srb_to_add_mod_s::rlc_cfg_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE srb_to_add_mod_s::rlc_cfg_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::explicit_value:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::default_value:
      break;
    default:
      log_invalid_choice_id(type_, "srb_to_add_mod_s::rlc_cfg_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool srb_to_add_mod_s::rlc_cfg_c_::operator==(const rlc_cfg_c_& other) const
{
  return type() == other.type() and c == other.c;
}

void srb_to_add_mod_s::lc_ch_cfg_c_::set(types::options e)
{
  type_ = e;
}
lc_ch_cfg_s& srb_to_add_mod_s::lc_ch_cfg_c_::set_explicit_value()
{
  set(types::explicit_value);
  return c;
}
void srb_to_add_mod_s::lc_ch_cfg_c_::set_default_value()
{
  set(types::default_value);
}
void srb_to_add_mod_s::lc_ch_cfg_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::explicit_value:
      j.write_fieldname("explicitValue");
      c.to_json(j);
      break;
    case types::default_value:
      break;
    default:
      log_invalid_choice_id(type_, "srb_to_add_mod_s::lc_ch_cfg_c_");
  }
  j.end_obj();
}
SRSASN_CODE srb_to_add_mod_s::lc_ch_cfg_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::explicit_value:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::default_value:
      break;
    default:
      log_invalid_choice_id(type_, "srb_to_add_mod_s::lc_ch_cfg_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE srb_to_add_mod_s::lc_ch_cfg_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::explicit_value:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::default_value:
      break;
    default:
      log_invalid_choice_id(type_, "srb_to_add_mod_s::lc_ch_cfg_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool srb_to_add_mod_s::lc_ch_cfg_c_::operator==(const lc_ch_cfg_c_& other) const
{
  return type() == other.type() and c == other.c;
}

// CRS-ChEstMPDCCH-ConfigDedicated-r16 ::= SEQUENCE
SRSASN_CODE crs_ch_est_mpdcch_cfg_ded_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(pwr_ratio_r16_present, 1));
  HANDLE_CODE(bref.pack(localized_map_type_r16_present, 1));

  if (pwr_ratio_r16_present) {
    HANDLE_CODE(pwr_ratio_r16.pack(bref));
  }
  if (localized_map_type_r16_present) {
    HANDLE_CODE(localized_map_type_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE crs_ch_est_mpdcch_cfg_ded_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(pwr_ratio_r16_present, 1));
  HANDLE_CODE(bref.unpack(localized_map_type_r16_present, 1));

  if (pwr_ratio_r16_present) {
    HANDLE_CODE(pwr_ratio_r16.unpack(bref));
  }
  if (localized_map_type_r16_present) {
    HANDLE_CODE(localized_map_type_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void crs_ch_est_mpdcch_cfg_ded_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (pwr_ratio_r16_present) {
    j.write_str("powerRatio-r16", pwr_ratio_r16.to_string());
  }
  if (localized_map_type_r16_present) {
    j.write_str("localizedMappingType-r16", localized_map_type_r16.to_string());
  }
  j.end_obj();
}

const char* crs_ch_est_mpdcch_cfg_ded_r16_s::pwr_ratio_r16_opts::to_string() const
{
  static const char* options[] = {"dB-4dot77", "dB-3", "dB-1dot77", "dB0", "dB1", "dB2", "dB3", "dB4dot77"};
  return convert_enum_idx(options, 8, value, "crs_ch_est_mpdcch_cfg_ded_r16_s::pwr_ratio_r16_e_");
}
float crs_ch_est_mpdcch_cfg_ded_r16_s::pwr_ratio_r16_opts::to_number() const
{
  static const float options[] = {-4.77, -3.0, -1.77, 0.0, 1.0, 2.0, 3.0, 4.77};
  return map_enum_number(options, 8, value, "crs_ch_est_mpdcch_cfg_ded_r16_s::pwr_ratio_r16_e_");
}
const char* crs_ch_est_mpdcch_cfg_ded_r16_s::pwr_ratio_r16_opts::to_number_string() const
{
  static const char* options[] = {"-4.77", "-3", "-1.77", "0", "1", "2", "3", "4.77"};
  return convert_enum_idx(options, 8, value, "crs_ch_est_mpdcch_cfg_ded_r16_s::pwr_ratio_r16_e_");
}

const char* crs_ch_est_mpdcch_cfg_ded_r16_s::localized_map_type_r16_opts::to_string() const
{
  static const char* options[] = {"predefined", "csi-Based", "reciprocityBased"};
  return convert_enum_idx(options, 3, value, "crs_ch_est_mpdcch_cfg_ded_r16_s::localized_map_type_r16_e_");
}

// MAC-MainConfig ::= SEQUENCE
SRSASN_CODE mac_main_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ul_sch_cfg_present, 1));
  HANDLE_CODE(bref.pack(drx_cfg_present, 1));
  HANDLE_CODE(bref.pack(phr_cfg_present, 1));

  if (ul_sch_cfg_present) {
    HANDLE_CODE(bref.pack(ul_sch_cfg.max_harq_tx_present, 1));
    HANDLE_CODE(bref.pack(ul_sch_cfg.periodic_bsr_timer_present, 1));
    if (ul_sch_cfg.max_harq_tx_present) {
      HANDLE_CODE(ul_sch_cfg.max_harq_tx.pack(bref));
    }
    if (ul_sch_cfg.periodic_bsr_timer_present) {
      HANDLE_CODE(ul_sch_cfg.periodic_bsr_timer.pack(bref));
    }
    HANDLE_CODE(ul_sch_cfg.retx_bsr_timer.pack(bref));
    HANDLE_CODE(bref.pack(ul_sch_cfg.tti_bundling, 1));
  }
  if (drx_cfg_present) {
    HANDLE_CODE(drx_cfg.pack(bref));
  }
  HANDLE_CODE(time_align_timer_ded.pack(bref));
  if (phr_cfg_present) {
    HANDLE_CODE(phr_cfg.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= sr_prohibit_timer_r9_present;
    group_flags[1] |= mac_main_cfg_v1020.is_present();
    group_flags[2] |= stag_to_release_list_r11.is_present();
    group_flags[2] |= stag_to_add_mod_list_r11.is_present();
    group_flags[2] |= drx_cfg_v1130.is_present();
    group_flags[3] |= e_harq_pattern_r12_present;
    group_flags[3] |= dual_connect_phr.is_present();
    group_flags[3] |= lc_ch_sr_cfg_r12.is_present();
    group_flags[4] |= drx_cfg_v1310.is_present();
    group_flags[4] |= extended_phr2_r13_present;
    group_flags[4] |= edrx_cfg_cycle_start_offset_r13.is_present();
    group_flags[5] |= drx_cfg_r13.is_present();
    group_flags[6] |= skip_ul_tx_r14.is_present();
    group_flags[6] |= data_inactivity_timer_cfg_r14.is_present();
    group_flags[7] |= rai_activation_r14_present;
    group_flags[8] |= short_tti_and_spt_r15.is_present();
    group_flags[8] |= mpdcch_ul_harq_ack_feedback_cfg_r15_present;
    group_flags[8] |= dormant_state_timers_r15.is_present();
    group_flags[9] |= ce_etws_cmas_rx_in_conn_r16_present;
    group_flags[10] |= offset_thres_ta_r17.is_present();
    group_flags[10] |= sr_prohibit_timer_offset_r17.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(sr_prohibit_timer_r9_present, 1));
      if (sr_prohibit_timer_r9_present) {
        HANDLE_CODE(pack_integer(bref, sr_prohibit_timer_r9, (uint8_t)0u, (uint8_t)7u));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(mac_main_cfg_v1020.is_present(), 1));
      if (mac_main_cfg_v1020.is_present()) {
        HANDLE_CODE(bref.pack(mac_main_cfg_v1020->scell_deactivation_timer_r10_present, 1));
        HANDLE_CODE(bref.pack(mac_main_cfg_v1020->extended_bsr_sizes_r10_present, 1));
        HANDLE_CODE(bref.pack(mac_main_cfg_v1020->extended_phr_r10_present, 1));
        if (mac_main_cfg_v1020->scell_deactivation_timer_r10_present) {
          HANDLE_CODE(mac_main_cfg_v1020->scell_deactivation_timer_r10.pack(bref));
        }
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(stag_to_release_list_r11.is_present(), 1));
      HANDLE_CODE(bref.pack(stag_to_add_mod_list_r11.is_present(), 1));
      HANDLE_CODE(bref.pack(drx_cfg_v1130.is_present(), 1));
      if (stag_to_release_list_r11.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *stag_to_release_list_r11, 1, 3, integer_packer<uint8_t>(1, 3)));
      }
      if (stag_to_add_mod_list_r11.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *stag_to_add_mod_list_r11, 1, 3));
      }
      if (drx_cfg_v1130.is_present()) {
        HANDLE_CODE(drx_cfg_v1130->pack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(e_harq_pattern_r12_present, 1));
      HANDLE_CODE(bref.pack(dual_connect_phr.is_present(), 1));
      HANDLE_CODE(bref.pack(lc_ch_sr_cfg_r12.is_present(), 1));
      if (e_harq_pattern_r12_present) {
        HANDLE_CODE(bref.pack(e_harq_pattern_r12, 1));
      }
      if (dual_connect_phr.is_present()) {
        HANDLE_CODE(dual_connect_phr->pack(bref));
      }
      if (lc_ch_sr_cfg_r12.is_present()) {
        HANDLE_CODE(lc_ch_sr_cfg_r12->pack(bref));
      }
    }
    if (group_flags[4]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(drx_cfg_v1310.is_present(), 1));
      HANDLE_CODE(bref.pack(extended_phr2_r13_present, 1));
      HANDLE_CODE(bref.pack(edrx_cfg_cycle_start_offset_r13.is_present(), 1));
      if (drx_cfg_v1310.is_present()) {
        HANDLE_CODE(drx_cfg_v1310->pack(bref));
      }
      if (extended_phr2_r13_present) {
        HANDLE_CODE(bref.pack(extended_phr2_r13, 1));
      }
      if (edrx_cfg_cycle_start_offset_r13.is_present()) {
        HANDLE_CODE(edrx_cfg_cycle_start_offset_r13->pack(bref));
      }
    }
    if (group_flags[5]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(drx_cfg_r13.is_present(), 1));
      if (drx_cfg_r13.is_present()) {
        HANDLE_CODE(drx_cfg_r13->pack(bref));
      }
    }
    if (group_flags[6]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(skip_ul_tx_r14.is_present(), 1));
      HANDLE_CODE(bref.pack(data_inactivity_timer_cfg_r14.is_present(), 1));
      if (skip_ul_tx_r14.is_present()) {
        HANDLE_CODE(skip_ul_tx_r14->pack(bref));
      }
      if (data_inactivity_timer_cfg_r14.is_present()) {
        HANDLE_CODE(data_inactivity_timer_cfg_r14->pack(bref));
      }
    }
    if (group_flags[7]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(rai_activation_r14_present, 1));
    }
    if (group_flags[8]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(short_tti_and_spt_r15.is_present(), 1));
      HANDLE_CODE(bref.pack(mpdcch_ul_harq_ack_feedback_cfg_r15_present, 1));
      HANDLE_CODE(bref.pack(dormant_state_timers_r15.is_present(), 1));
      if (short_tti_and_spt_r15.is_present()) {
        HANDLE_CODE(short_tti_and_spt_r15->pack(bref));
      }
      if (mpdcch_ul_harq_ack_feedback_cfg_r15_present) {
        HANDLE_CODE(bref.pack(mpdcch_ul_harq_ack_feedback_cfg_r15, 1));
      }
      if (dormant_state_timers_r15.is_present()) {
        HANDLE_CODE(dormant_state_timers_r15->pack(bref));
      }
    }
    if (group_flags[9]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ce_etws_cmas_rx_in_conn_r16_present, 1));
    }
    if (group_flags[10]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(offset_thres_ta_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(sr_prohibit_timer_offset_r17.is_present(), 1));
      if (offset_thres_ta_r17.is_present()) {
        HANDLE_CODE(offset_thres_ta_r17->pack(bref));
      }
      if (sr_prohibit_timer_offset_r17.is_present()) {
        HANDLE_CODE(sr_prohibit_timer_offset_r17->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mac_main_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ul_sch_cfg_present, 1));
  HANDLE_CODE(bref.unpack(drx_cfg_present, 1));
  HANDLE_CODE(bref.unpack(phr_cfg_present, 1));

  if (ul_sch_cfg_present) {
    HANDLE_CODE(bref.unpack(ul_sch_cfg.max_harq_tx_present, 1));
    HANDLE_CODE(bref.unpack(ul_sch_cfg.periodic_bsr_timer_present, 1));
    if (ul_sch_cfg.max_harq_tx_present) {
      HANDLE_CODE(ul_sch_cfg.max_harq_tx.unpack(bref));
    }
    if (ul_sch_cfg.periodic_bsr_timer_present) {
      HANDLE_CODE(ul_sch_cfg.periodic_bsr_timer.unpack(bref));
    }
    HANDLE_CODE(ul_sch_cfg.retx_bsr_timer.unpack(bref));
    HANDLE_CODE(bref.unpack(ul_sch_cfg.tti_bundling, 1));
  }
  if (drx_cfg_present) {
    HANDLE_CODE(drx_cfg.unpack(bref));
  }
  HANDLE_CODE(time_align_timer_ded.unpack(bref));
  if (phr_cfg_present) {
    HANDLE_CODE(phr_cfg.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(11);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(sr_prohibit_timer_r9_present, 1));
      if (sr_prohibit_timer_r9_present) {
        HANDLE_CODE(unpack_integer(sr_prohibit_timer_r9, bref, (uint8_t)0u, (uint8_t)7u));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool mac_main_cfg_v1020_present;
      HANDLE_CODE(bref.unpack(mac_main_cfg_v1020_present, 1));
      mac_main_cfg_v1020.set_present(mac_main_cfg_v1020_present);
      if (mac_main_cfg_v1020.is_present()) {
        HANDLE_CODE(bref.unpack(mac_main_cfg_v1020->scell_deactivation_timer_r10_present, 1));
        HANDLE_CODE(bref.unpack(mac_main_cfg_v1020->extended_bsr_sizes_r10_present, 1));
        HANDLE_CODE(bref.unpack(mac_main_cfg_v1020->extended_phr_r10_present, 1));
        if (mac_main_cfg_v1020->scell_deactivation_timer_r10_present) {
          HANDLE_CODE(mac_main_cfg_v1020->scell_deactivation_timer_r10.unpack(bref));
        }
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool stag_to_release_list_r11_present;
      HANDLE_CODE(bref.unpack(stag_to_release_list_r11_present, 1));
      stag_to_release_list_r11.set_present(stag_to_release_list_r11_present);
      bool stag_to_add_mod_list_r11_present;
      HANDLE_CODE(bref.unpack(stag_to_add_mod_list_r11_present, 1));
      stag_to_add_mod_list_r11.set_present(stag_to_add_mod_list_r11_present);
      bool drx_cfg_v1130_present;
      HANDLE_CODE(bref.unpack(drx_cfg_v1130_present, 1));
      drx_cfg_v1130.set_present(drx_cfg_v1130_present);
      if (stag_to_release_list_r11.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*stag_to_release_list_r11, bref, 1, 3, integer_packer<uint8_t>(1, 3)));
      }
      if (stag_to_add_mod_list_r11.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*stag_to_add_mod_list_r11, bref, 1, 3));
      }
      if (drx_cfg_v1130.is_present()) {
        HANDLE_CODE(drx_cfg_v1130->unpack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(e_harq_pattern_r12_present, 1));
      bool dual_connect_phr_present;
      HANDLE_CODE(bref.unpack(dual_connect_phr_present, 1));
      dual_connect_phr.set_present(dual_connect_phr_present);
      bool lc_ch_sr_cfg_r12_present;
      HANDLE_CODE(bref.unpack(lc_ch_sr_cfg_r12_present, 1));
      lc_ch_sr_cfg_r12.set_present(lc_ch_sr_cfg_r12_present);
      if (e_harq_pattern_r12_present) {
        HANDLE_CODE(bref.unpack(e_harq_pattern_r12, 1));
      }
      if (dual_connect_phr.is_present()) {
        HANDLE_CODE(dual_connect_phr->unpack(bref));
      }
      if (lc_ch_sr_cfg_r12.is_present()) {
        HANDLE_CODE(lc_ch_sr_cfg_r12->unpack(bref));
      }
    }
    if (group_flags[4]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool drx_cfg_v1310_present;
      HANDLE_CODE(bref.unpack(drx_cfg_v1310_present, 1));
      drx_cfg_v1310.set_present(drx_cfg_v1310_present);
      HANDLE_CODE(bref.unpack(extended_phr2_r13_present, 1));
      bool edrx_cfg_cycle_start_offset_r13_present;
      HANDLE_CODE(bref.unpack(edrx_cfg_cycle_start_offset_r13_present, 1));
      edrx_cfg_cycle_start_offset_r13.set_present(edrx_cfg_cycle_start_offset_r13_present);
      if (drx_cfg_v1310.is_present()) {
        HANDLE_CODE(drx_cfg_v1310->unpack(bref));
      }
      if (extended_phr2_r13_present) {
        HANDLE_CODE(bref.unpack(extended_phr2_r13, 1));
      }
      if (edrx_cfg_cycle_start_offset_r13.is_present()) {
        HANDLE_CODE(edrx_cfg_cycle_start_offset_r13->unpack(bref));
      }
    }
    if (group_flags[5]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool drx_cfg_r13_present;
      HANDLE_CODE(bref.unpack(drx_cfg_r13_present, 1));
      drx_cfg_r13.set_present(drx_cfg_r13_present);
      if (drx_cfg_r13.is_present()) {
        HANDLE_CODE(drx_cfg_r13->unpack(bref));
      }
    }
    if (group_flags[6]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool skip_ul_tx_r14_present;
      HANDLE_CODE(bref.unpack(skip_ul_tx_r14_present, 1));
      skip_ul_tx_r14.set_present(skip_ul_tx_r14_present);
      bool data_inactivity_timer_cfg_r14_present;
      HANDLE_CODE(bref.unpack(data_inactivity_timer_cfg_r14_present, 1));
      data_inactivity_timer_cfg_r14.set_present(data_inactivity_timer_cfg_r14_present);
      if (skip_ul_tx_r14.is_present()) {
        HANDLE_CODE(skip_ul_tx_r14->unpack(bref));
      }
      if (data_inactivity_timer_cfg_r14.is_present()) {
        HANDLE_CODE(data_inactivity_timer_cfg_r14->unpack(bref));
      }
    }
    if (group_flags[7]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(rai_activation_r14_present, 1));
    }
    if (group_flags[8]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool short_tti_and_spt_r15_present;
      HANDLE_CODE(bref.unpack(short_tti_and_spt_r15_present, 1));
      short_tti_and_spt_r15.set_present(short_tti_and_spt_r15_present);
      HANDLE_CODE(bref.unpack(mpdcch_ul_harq_ack_feedback_cfg_r15_present, 1));
      bool dormant_state_timers_r15_present;
      HANDLE_CODE(bref.unpack(dormant_state_timers_r15_present, 1));
      dormant_state_timers_r15.set_present(dormant_state_timers_r15_present);
      if (short_tti_and_spt_r15.is_present()) {
        HANDLE_CODE(short_tti_and_spt_r15->unpack(bref));
      }
      if (mpdcch_ul_harq_ack_feedback_cfg_r15_present) {
        HANDLE_CODE(bref.unpack(mpdcch_ul_harq_ack_feedback_cfg_r15, 1));
      }
      if (dormant_state_timers_r15.is_present()) {
        HANDLE_CODE(dormant_state_timers_r15->unpack(bref));
      }
    }
    if (group_flags[9]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(ce_etws_cmas_rx_in_conn_r16_present, 1));
    }
    if (group_flags[10]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool offset_thres_ta_r17_present;
      HANDLE_CODE(bref.unpack(offset_thres_ta_r17_present, 1));
      offset_thres_ta_r17.set_present(offset_thres_ta_r17_present);
      bool sr_prohibit_timer_offset_r17_present;
      HANDLE_CODE(bref.unpack(sr_prohibit_timer_offset_r17_present, 1));
      sr_prohibit_timer_offset_r17.set_present(sr_prohibit_timer_offset_r17_present);
      if (offset_thres_ta_r17.is_present()) {
        HANDLE_CODE(offset_thres_ta_r17->unpack(bref));
      }
      if (sr_prohibit_timer_offset_r17.is_present()) {
        HANDLE_CODE(sr_prohibit_timer_offset_r17->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void mac_main_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ul_sch_cfg_present) {
    j.write_fieldname("ul-SCH-Config");
    j.start_obj();
    if (ul_sch_cfg.max_harq_tx_present) {
      j.write_str("maxHARQ-Tx", ul_sch_cfg.max_harq_tx.to_string());
    }
    if (ul_sch_cfg.periodic_bsr_timer_present) {
      j.write_str("periodicBSR-Timer", ul_sch_cfg.periodic_bsr_timer.to_string());
    }
    j.write_str("retxBSR-Timer", ul_sch_cfg.retx_bsr_timer.to_string());
    j.write_bool("ttiBundling", ul_sch_cfg.tti_bundling);
    j.end_obj();
  }
  if (drx_cfg_present) {
    j.write_fieldname("drx-Config");
    drx_cfg.to_json(j);
  }
  j.write_str("timeAlignmentTimerDedicated", time_align_timer_ded.to_string());
  if (phr_cfg_present) {
    j.write_fieldname("phr-Config");
    phr_cfg.to_json(j);
  }
  if (ext) {
    if (sr_prohibit_timer_r9_present) {
      j.write_int("sr-ProhibitTimer-r9", sr_prohibit_timer_r9);
    }
    if (mac_main_cfg_v1020.is_present()) {
      j.write_fieldname("mac-MainConfig-v1020");
      j.start_obj();
      if (mac_main_cfg_v1020->scell_deactivation_timer_r10_present) {
        j.write_str("sCellDeactivationTimer-r10", mac_main_cfg_v1020->scell_deactivation_timer_r10.to_string());
      }
      if (mac_main_cfg_v1020->extended_bsr_sizes_r10_present) {
        j.write_str("extendedBSR-Sizes-r10", "setup");
      }
      if (mac_main_cfg_v1020->extended_phr_r10_present) {
        j.write_str("extendedPHR-r10", "setup");
      }
      j.end_obj();
    }
    if (stag_to_release_list_r11.is_present()) {
      j.start_array("stag-ToReleaseList-r11");
      for (const auto& e1 : *stag_to_release_list_r11) {
        j.write_int(e1);
      }
      j.end_array();
    }
    if (stag_to_add_mod_list_r11.is_present()) {
      j.start_array("stag-ToAddModList-r11");
      for (const auto& e1 : *stag_to_add_mod_list_r11) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (drx_cfg_v1130.is_present()) {
      j.write_fieldname("drx-Config-v1130");
      drx_cfg_v1130->to_json(j);
    }
    if (e_harq_pattern_r12_present) {
      j.write_bool("e-HARQ-Pattern-r12", e_harq_pattern_r12);
    }
    if (dual_connect_phr.is_present()) {
      j.write_fieldname("dualConnectivityPHR");
      dual_connect_phr->to_json(j);
    }
    if (lc_ch_sr_cfg_r12.is_present()) {
      j.write_fieldname("logicalChannelSR-Config-r12");
      lc_ch_sr_cfg_r12->to_json(j);
    }
    if (drx_cfg_v1310.is_present()) {
      j.write_fieldname("drx-Config-v1310");
      drx_cfg_v1310->to_json(j);
    }
    if (extended_phr2_r13_present) {
      j.write_bool("extendedPHR2-r13", extended_phr2_r13);
    }
    if (edrx_cfg_cycle_start_offset_r13.is_present()) {
      j.write_fieldname("eDRX-Config-CycleStartOffset-r13");
      edrx_cfg_cycle_start_offset_r13->to_json(j);
    }
    if (drx_cfg_r13.is_present()) {
      j.write_fieldname("drx-Config-r13");
      drx_cfg_r13->to_json(j);
    }
    if (skip_ul_tx_r14.is_present()) {
      j.write_fieldname("skipUplinkTx-r14");
      skip_ul_tx_r14->to_json(j);
    }
    if (data_inactivity_timer_cfg_r14.is_present()) {
      j.write_fieldname("dataInactivityTimerConfig-r14");
      data_inactivity_timer_cfg_r14->to_json(j);
    }
    if (rai_activation_r14_present) {
      j.write_str("rai-Activation-r14", "true");
    }
    if (short_tti_and_spt_r15.is_present()) {
      j.write_fieldname("shortTTI-AndSPT-r15");
      short_tti_and_spt_r15->to_json(j);
    }
    if (mpdcch_ul_harq_ack_feedback_cfg_r15_present) {
      j.write_bool("mpdcch-UL-HARQ-ACK-FeedbackConfig-r15", mpdcch_ul_harq_ack_feedback_cfg_r15);
    }
    if (dormant_state_timers_r15.is_present()) {
      j.write_fieldname("dormantStateTimers-r15");
      dormant_state_timers_r15->to_json(j);
    }
    if (ce_etws_cmas_rx_in_conn_r16_present) {
      j.write_str("ce-ETWS-CMAS-RxInConn-r16", "true");
    }
    if (offset_thres_ta_r17.is_present()) {
      j.write_fieldname("offsetThresholdTA-r17");
      offset_thres_ta_r17->to_json(j);
    }
    if (sr_prohibit_timer_offset_r17.is_present()) {
      j.write_fieldname("sr-ProhibitTimerOffset-r17");
      sr_prohibit_timer_offset_r17->to_json(j);
    }
  }
  j.end_obj();
}

const char* mac_main_cfg_s::ul_sch_cfg_s_::max_harq_tx_opts::to_string() const
{
  static const char* options[] = {
      "n1", "n2", "n3", "n4", "n5", "n6", "n7", "n8", "n10", "n12", "n16", "n20", "n24", "n28", "spare2", "spare1"};
  return convert_enum_idx(options, 16, value, "mac_main_cfg_s::ul_sch_cfg_s_::max_harq_tx_e_");
}
uint8_t mac_main_cfg_s::ul_sch_cfg_s_::max_harq_tx_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 5, 6, 7, 8, 10, 12, 16, 20, 24, 28};
  return map_enum_number(options, 14, value, "mac_main_cfg_s::ul_sch_cfg_s_::max_harq_tx_e_");
}

void mac_main_cfg_s::phr_cfg_c_::set(types::options e)
{
  type_ = e;
}
void mac_main_cfg_s::phr_cfg_c_::set_release()
{
  set(types::release);
}
mac_main_cfg_s::phr_cfg_c_::setup_s_& mac_main_cfg_s::phr_cfg_c_::set_setup()
{
  set(types::setup);
  return c;
}
void mac_main_cfg_s::phr_cfg_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_str("periodicPHR-Timer", c.periodic_phr_timer.to_string());
      j.write_str("prohibitPHR-Timer", c.prohibit_phr_timer.to_string());
      j.write_str("dl-PathlossChange", c.dl_pathloss_change.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "mac_main_cfg_s::phr_cfg_c_");
  }
  j.end_obj();
}
SRSASN_CODE mac_main_cfg_s::phr_cfg_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.periodic_phr_timer.pack(bref));
      HANDLE_CODE(c.prohibit_phr_timer.pack(bref));
      HANDLE_CODE(c.dl_pathloss_change.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "mac_main_cfg_s::phr_cfg_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mac_main_cfg_s::phr_cfg_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.periodic_phr_timer.unpack(bref));
      HANDLE_CODE(c.prohibit_phr_timer.unpack(bref));
      HANDLE_CODE(c.dl_pathloss_change.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "mac_main_cfg_s::phr_cfg_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* mac_main_cfg_s::phr_cfg_c_::setup_s_::periodic_phr_timer_opts::to_string() const
{
  static const char* options[] = {"sf10", "sf20", "sf50", "sf100", "sf200", "sf500", "sf1000", "infinity"};
  return convert_enum_idx(options, 8, value, "mac_main_cfg_s::phr_cfg_c_::setup_s_::periodic_phr_timer_e_");
}
int16_t mac_main_cfg_s::phr_cfg_c_::setup_s_::periodic_phr_timer_opts::to_number() const
{
  static const int16_t options[] = {10, 20, 50, 100, 200, 500, 1000, -1};
  return map_enum_number(options, 8, value, "mac_main_cfg_s::phr_cfg_c_::setup_s_::periodic_phr_timer_e_");
}

const char* mac_main_cfg_s::phr_cfg_c_::setup_s_::prohibit_phr_timer_opts::to_string() const
{
  static const char* options[] = {"sf0", "sf10", "sf20", "sf50", "sf100", "sf200", "sf500", "sf1000"};
  return convert_enum_idx(options, 8, value, "mac_main_cfg_s::phr_cfg_c_::setup_s_::prohibit_phr_timer_e_");
}
uint16_t mac_main_cfg_s::phr_cfg_c_::setup_s_::prohibit_phr_timer_opts::to_number() const
{
  static const uint16_t options[] = {0, 10, 20, 50, 100, 200, 500, 1000};
  return map_enum_number(options, 8, value, "mac_main_cfg_s::phr_cfg_c_::setup_s_::prohibit_phr_timer_e_");
}

const char* mac_main_cfg_s::phr_cfg_c_::setup_s_::dl_pathloss_change_opts::to_string() const
{
  static const char* options[] = {"dB1", "dB3", "dB6", "infinity"};
  return convert_enum_idx(options, 4, value, "mac_main_cfg_s::phr_cfg_c_::setup_s_::dl_pathloss_change_e_");
}
int8_t mac_main_cfg_s::phr_cfg_c_::setup_s_::dl_pathloss_change_opts::to_number() const
{
  static const int8_t options[] = {1, 3, 6, -1};
  return map_enum_number(options, 4, value, "mac_main_cfg_s::phr_cfg_c_::setup_s_::dl_pathloss_change_e_");
}

const char* mac_main_cfg_s::mac_main_cfg_v1020_s_::scell_deactivation_timer_r10_opts::to_string() const
{
  static const char* options[] = {"rf2", "rf4", "rf8", "rf16", "rf32", "rf64", "rf128", "spare"};
  return convert_enum_idx(options, 8, value, "mac_main_cfg_s::mac_main_cfg_v1020_s_::scell_deactivation_timer_r10_e_");
}
uint8_t mac_main_cfg_s::mac_main_cfg_v1020_s_::scell_deactivation_timer_r10_opts::to_number() const
{
  static const uint8_t options[] = {2, 4, 8, 16, 32, 64, 128};
  return map_enum_number(options, 7, value, "mac_main_cfg_s::mac_main_cfg_v1020_s_::scell_deactivation_timer_r10_e_");
}

void mac_main_cfg_s::dual_connect_phr_c_::set(types::options e)
{
  type_ = e;
}
void mac_main_cfg_s::dual_connect_phr_c_::set_release()
{
  set(types::release);
}
mac_main_cfg_s::dual_connect_phr_c_::setup_s_& mac_main_cfg_s::dual_connect_phr_c_::set_setup()
{
  set(types::setup);
  return c;
}
void mac_main_cfg_s::dual_connect_phr_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_str("phr-ModeOtherCG-r12", c.phr_mode_other_cg_r12.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "mac_main_cfg_s::dual_connect_phr_c_");
  }
  j.end_obj();
}
SRSASN_CODE mac_main_cfg_s::dual_connect_phr_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.phr_mode_other_cg_r12.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "mac_main_cfg_s::dual_connect_phr_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mac_main_cfg_s::dual_connect_phr_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.phr_mode_other_cg_r12.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "mac_main_cfg_s::dual_connect_phr_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* mac_main_cfg_s::dual_connect_phr_c_::setup_s_::phr_mode_other_cg_r12_opts::to_string() const
{
  static const char* options[] = {"real", "virtual"};
  return convert_enum_idx(options, 2, value, "mac_main_cfg_s::dual_connect_phr_c_::setup_s_::phr_mode_other_cg_r12_e_");
}

void mac_main_cfg_s::lc_ch_sr_cfg_r12_c_::set(types::options e)
{
  type_ = e;
}
void mac_main_cfg_s::lc_ch_sr_cfg_r12_c_::set_release()
{
  set(types::release);
}
mac_main_cfg_s::lc_ch_sr_cfg_r12_c_::setup_s_& mac_main_cfg_s::lc_ch_sr_cfg_r12_c_::set_setup()
{
  set(types::setup);
  return c;
}
void mac_main_cfg_s::lc_ch_sr_cfg_r12_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_str("logicalChannelSR-ProhibitTimer-r12", c.lc_ch_sr_prohibit_timer_r12.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "mac_main_cfg_s::lc_ch_sr_cfg_r12_c_");
  }
  j.end_obj();
}
SRSASN_CODE mac_main_cfg_s::lc_ch_sr_cfg_r12_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.lc_ch_sr_prohibit_timer_r12.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "mac_main_cfg_s::lc_ch_sr_cfg_r12_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mac_main_cfg_s::lc_ch_sr_cfg_r12_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.lc_ch_sr_prohibit_timer_r12.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "mac_main_cfg_s::lc_ch_sr_cfg_r12_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* mac_main_cfg_s::lc_ch_sr_cfg_r12_c_::setup_s_::lc_ch_sr_prohibit_timer_r12_opts::to_string() const
{
  static const char* options[] = {"sf20", "sf40", "sf64", "sf128", "sf512", "sf1024", "sf2560", "spare1"};
  return convert_enum_idx(
      options, 8, value, "mac_main_cfg_s::lc_ch_sr_cfg_r12_c_::setup_s_::lc_ch_sr_prohibit_timer_r12_e_");
}
uint16_t mac_main_cfg_s::lc_ch_sr_cfg_r12_c_::setup_s_::lc_ch_sr_prohibit_timer_r12_opts::to_number() const
{
  static const uint16_t options[] = {20, 40, 64, 128, 512, 1024, 2560};
  return map_enum_number(
      options, 7, value, "mac_main_cfg_s::lc_ch_sr_cfg_r12_c_::setup_s_::lc_ch_sr_prohibit_timer_r12_e_");
}

void mac_main_cfg_s::edrx_cfg_cycle_start_offset_r13_c_::set(types::options e)
{
  type_ = e;
}
void mac_main_cfg_s::edrx_cfg_cycle_start_offset_r13_c_::set_release()
{
  set(types::release);
}
mac_main_cfg_s::edrx_cfg_cycle_start_offset_r13_c_::setup_c_&
mac_main_cfg_s::edrx_cfg_cycle_start_offset_r13_c_::set_setup()
{
  set(types::setup);
  return c;
}
void mac_main_cfg_s::edrx_cfg_cycle_start_offset_r13_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      c.to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "mac_main_cfg_s::edrx_cfg_cycle_start_offset_r13_c_");
  }
  j.end_obj();
}
SRSASN_CODE mac_main_cfg_s::edrx_cfg_cycle_start_offset_r13_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "mac_main_cfg_s::edrx_cfg_cycle_start_offset_r13_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mac_main_cfg_s::edrx_cfg_cycle_start_offset_r13_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "mac_main_cfg_s::edrx_cfg_cycle_start_offset_r13_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void mac_main_cfg_s::edrx_cfg_cycle_start_offset_r13_c_::setup_c_::destroy_() {}
void mac_main_cfg_s::edrx_cfg_cycle_start_offset_r13_c_::setup_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
mac_main_cfg_s::edrx_cfg_cycle_start_offset_r13_c_::setup_c_::setup_c_(
    const mac_main_cfg_s::edrx_cfg_cycle_start_offset_r13_c_::setup_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sf5120:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf10240:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "mac_main_cfg_s::edrx_cfg_cycle_start_offset_r13_c_::setup_c_");
  }
}
mac_main_cfg_s::edrx_cfg_cycle_start_offset_r13_c_::setup_c_&
mac_main_cfg_s::edrx_cfg_cycle_start_offset_r13_c_::setup_c_::operator=(
    const mac_main_cfg_s::edrx_cfg_cycle_start_offset_r13_c_::setup_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sf5120:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf10240:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "mac_main_cfg_s::edrx_cfg_cycle_start_offset_r13_c_::setup_c_");
  }

  return *this;
}
uint8_t& mac_main_cfg_s::edrx_cfg_cycle_start_offset_r13_c_::setup_c_::set_sf5120()
{
  set(types::sf5120);
  return c.get<uint8_t>();
}
uint8_t& mac_main_cfg_s::edrx_cfg_cycle_start_offset_r13_c_::setup_c_::set_sf10240()
{
  set(types::sf10240);
  return c.get<uint8_t>();
}
void mac_main_cfg_s::edrx_cfg_cycle_start_offset_r13_c_::setup_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sf5120:
      j.write_int("sf5120", c.get<uint8_t>());
      break;
    case types::sf10240:
      j.write_int("sf10240", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "mac_main_cfg_s::edrx_cfg_cycle_start_offset_r13_c_::setup_c_");
  }
  j.end_obj();
}
SRSASN_CODE mac_main_cfg_s::edrx_cfg_cycle_start_offset_r13_c_::setup_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sf5120:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)1u));
      break;
    case types::sf10240:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)3u));
      break;
    default:
      log_invalid_choice_id(type_, "mac_main_cfg_s::edrx_cfg_cycle_start_offset_r13_c_::setup_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mac_main_cfg_s::edrx_cfg_cycle_start_offset_r13_c_::setup_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sf5120:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)1u));
      break;
    case types::sf10240:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)3u));
      break;
    default:
      log_invalid_choice_id(type_, "mac_main_cfg_s::edrx_cfg_cycle_start_offset_r13_c_::setup_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void mac_main_cfg_s::drx_cfg_r13_c_::set(types::options e)
{
  type_ = e;
}
void mac_main_cfg_s::drx_cfg_r13_c_::set_release()
{
  set(types::release);
}
drx_cfg_r13_s& mac_main_cfg_s::drx_cfg_r13_c_::set_setup()
{
  set(types::setup);
  return c;
}
void mac_main_cfg_s::drx_cfg_r13_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      c.to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "mac_main_cfg_s::drx_cfg_r13_c_");
  }
  j.end_obj();
}
SRSASN_CODE mac_main_cfg_s::drx_cfg_r13_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "mac_main_cfg_s::drx_cfg_r13_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mac_main_cfg_s::drx_cfg_r13_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "mac_main_cfg_s::drx_cfg_r13_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void mac_main_cfg_s::skip_ul_tx_r14_c_::set(types::options e)
{
  type_ = e;
}
void mac_main_cfg_s::skip_ul_tx_r14_c_::set_release()
{
  set(types::release);
}
mac_main_cfg_s::skip_ul_tx_r14_c_::setup_s_& mac_main_cfg_s::skip_ul_tx_r14_c_::set_setup()
{
  set(types::setup);
  return c;
}
void mac_main_cfg_s::skip_ul_tx_r14_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      if (c.skip_ul_tx_sps_r14_present) {
        j.write_str("skipUplinkTxSPS-r14", "true");
      }
      if (c.skip_ul_tx_dynamic_r14_present) {
        j.write_str("skipUplinkTxDynamic-r14", "true");
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "mac_main_cfg_s::skip_ul_tx_r14_c_");
  }
  j.end_obj();
}
SRSASN_CODE mac_main_cfg_s::skip_ul_tx_r14_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.skip_ul_tx_sps_r14_present, 1));
      HANDLE_CODE(bref.pack(c.skip_ul_tx_dynamic_r14_present, 1));
      break;
    default:
      log_invalid_choice_id(type_, "mac_main_cfg_s::skip_ul_tx_r14_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mac_main_cfg_s::skip_ul_tx_r14_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.skip_ul_tx_sps_r14_present, 1));
      HANDLE_CODE(bref.unpack(c.skip_ul_tx_dynamic_r14_present, 1));
      break;
    default:
      log_invalid_choice_id(type_, "mac_main_cfg_s::skip_ul_tx_r14_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void mac_main_cfg_s::data_inactivity_timer_cfg_r14_c_::set(types::options e)
{
  type_ = e;
}
void mac_main_cfg_s::data_inactivity_timer_cfg_r14_c_::set_release()
{
  set(types::release);
}
mac_main_cfg_s::data_inactivity_timer_cfg_r14_c_::setup_s_&
mac_main_cfg_s::data_inactivity_timer_cfg_r14_c_::set_setup()
{
  set(types::setup);
  return c;
}
void mac_main_cfg_s::data_inactivity_timer_cfg_r14_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_str("dataInactivityTimer-r14", c.data_inactivity_timer_r14.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "mac_main_cfg_s::data_inactivity_timer_cfg_r14_c_");
  }
  j.end_obj();
}
SRSASN_CODE mac_main_cfg_s::data_inactivity_timer_cfg_r14_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.data_inactivity_timer_r14.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "mac_main_cfg_s::data_inactivity_timer_cfg_r14_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mac_main_cfg_s::data_inactivity_timer_cfg_r14_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.data_inactivity_timer_r14.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "mac_main_cfg_s::data_inactivity_timer_cfg_r14_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void mac_main_cfg_s::short_tti_and_spt_r15_c_::set(types::options e)
{
  type_ = e;
}
void mac_main_cfg_s::short_tti_and_spt_r15_c_::set_release()
{
  set(types::release);
}
mac_main_cfg_s::short_tti_and_spt_r15_c_::setup_s_& mac_main_cfg_s::short_tti_and_spt_r15_c_::set_setup()
{
  set(types::setup);
  return c;
}
void mac_main_cfg_s::short_tti_and_spt_r15_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      if (c.drx_cfg_r15_present) {
        j.write_fieldname("drx-Config-r15");
        c.drx_cfg_r15.to_json(j);
      }
      if (c.periodic_bsr_timer_r15_present) {
        j.write_str("periodicBSR-Timer-r15", c.periodic_bsr_timer_r15.to_string());
      }
      if (c.proc_timeline_r15_present) {
        j.write_str("proc-Timeline-r15", c.proc_timeline_r15.to_string());
      }
      if (c.ssr_prohibit_timer_r15_present) {
        j.write_int("ssr-ProhibitTimer-r15", c.ssr_prohibit_timer_r15);
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "mac_main_cfg_s::short_tti_and_spt_r15_c_");
  }
  j.end_obj();
}
SRSASN_CODE mac_main_cfg_s::short_tti_and_spt_r15_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.drx_cfg_r15_present, 1));
      HANDLE_CODE(bref.pack(c.periodic_bsr_timer_r15_present, 1));
      HANDLE_CODE(bref.pack(c.proc_timeline_r15_present, 1));
      HANDLE_CODE(bref.pack(c.ssr_prohibit_timer_r15_present, 1));
      if (c.drx_cfg_r15_present) {
        HANDLE_CODE(c.drx_cfg_r15.pack(bref));
      }
      if (c.periodic_bsr_timer_r15_present) {
        HANDLE_CODE(c.periodic_bsr_timer_r15.pack(bref));
      }
      if (c.proc_timeline_r15_present) {
        HANDLE_CODE(c.proc_timeline_r15.pack(bref));
      }
      if (c.ssr_prohibit_timer_r15_present) {
        HANDLE_CODE(pack_integer(bref, c.ssr_prohibit_timer_r15, (uint8_t)0u, (uint8_t)7u));
      }
      break;
    default:
      log_invalid_choice_id(type_, "mac_main_cfg_s::short_tti_and_spt_r15_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mac_main_cfg_s::short_tti_and_spt_r15_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.drx_cfg_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.periodic_bsr_timer_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.proc_timeline_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.ssr_prohibit_timer_r15_present, 1));
      if (c.drx_cfg_r15_present) {
        HANDLE_CODE(c.drx_cfg_r15.unpack(bref));
      }
      if (c.periodic_bsr_timer_r15_present) {
        HANDLE_CODE(c.periodic_bsr_timer_r15.unpack(bref));
      }
      if (c.proc_timeline_r15_present) {
        HANDLE_CODE(c.proc_timeline_r15.unpack(bref));
      }
      if (c.ssr_prohibit_timer_r15_present) {
        HANDLE_CODE(unpack_integer(c.ssr_prohibit_timer_r15, bref, (uint8_t)0u, (uint8_t)7u));
      }
      break;
    default:
      log_invalid_choice_id(type_, "mac_main_cfg_s::short_tti_and_spt_r15_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* mac_main_cfg_s::short_tti_and_spt_r15_c_::setup_s_::periodic_bsr_timer_r15_opts::to_string() const
{
  static const char* options[] = {"sf1",
                                  "sf5",
                                  "sf10",
                                  "sf16",
                                  "sf20",
                                  "sf32",
                                  "sf40",
                                  "sf64",
                                  "sf80",
                                  "sf128",
                                  "sf160",
                                  "sf320",
                                  "sf640",
                                  "sf1280",
                                  "sf2560",
                                  "infinity"};
  return convert_enum_idx(
      options, 16, value, "mac_main_cfg_s::short_tti_and_spt_r15_c_::setup_s_::periodic_bsr_timer_r15_e_");
}
int16_t mac_main_cfg_s::short_tti_and_spt_r15_c_::setup_s_::periodic_bsr_timer_r15_opts::to_number() const
{
  static const int16_t options[] = {1, 5, 10, 16, 20, 32, 40, 64, 80, 128, 160, 320, 640, 1280, 2560, -1};
  return map_enum_number(
      options, 16, value, "mac_main_cfg_s::short_tti_and_spt_r15_c_::setup_s_::periodic_bsr_timer_r15_e_");
}

const char* mac_main_cfg_s::short_tti_and_spt_r15_c_::setup_s_::proc_timeline_r15_opts::to_string() const
{
  static const char* options[] = {"nplus4set1", "nplus6set1", "nplus6set2", "nplus8set2"};
  return convert_enum_idx(
      options, 4, value, "mac_main_cfg_s::short_tti_and_spt_r15_c_::setup_s_::proc_timeline_r15_e_");
}

void mac_main_cfg_s::dormant_state_timers_r15_c_::set(types::options e)
{
  type_ = e;
}
void mac_main_cfg_s::dormant_state_timers_r15_c_::set_release()
{
  set(types::release);
}
mac_main_cfg_s::dormant_state_timers_r15_c_::setup_s_& mac_main_cfg_s::dormant_state_timers_r15_c_::set_setup()
{
  set(types::setup);
  return c;
}
void mac_main_cfg_s::dormant_state_timers_r15_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      if (c.scell_hibernation_timer_r15_present) {
        j.write_str("sCellHibernationTimer-r15", c.scell_hibernation_timer_r15.to_string());
      }
      if (c.dormant_scell_deactivation_timer_r15_present) {
        j.write_str("dormantSCellDeactivationTimer-r15", c.dormant_scell_deactivation_timer_r15.to_string());
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "mac_main_cfg_s::dormant_state_timers_r15_c_");
  }
  j.end_obj();
}
SRSASN_CODE mac_main_cfg_s::dormant_state_timers_r15_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.scell_hibernation_timer_r15_present, 1));
      HANDLE_CODE(bref.pack(c.dormant_scell_deactivation_timer_r15_present, 1));
      if (c.scell_hibernation_timer_r15_present) {
        HANDLE_CODE(c.scell_hibernation_timer_r15.pack(bref));
      }
      if (c.dormant_scell_deactivation_timer_r15_present) {
        HANDLE_CODE(c.dormant_scell_deactivation_timer_r15.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "mac_main_cfg_s::dormant_state_timers_r15_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mac_main_cfg_s::dormant_state_timers_r15_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.scell_hibernation_timer_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.dormant_scell_deactivation_timer_r15_present, 1));
      if (c.scell_hibernation_timer_r15_present) {
        HANDLE_CODE(c.scell_hibernation_timer_r15.unpack(bref));
      }
      if (c.dormant_scell_deactivation_timer_r15_present) {
        HANDLE_CODE(c.dormant_scell_deactivation_timer_r15.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "mac_main_cfg_s::dormant_state_timers_r15_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* mac_main_cfg_s::dormant_state_timers_r15_c_::setup_s_::scell_hibernation_timer_r15_opts::to_string() const
{
  static const char* options[] = {"rf2", "rf4", "rf8", "rf16", "rf32", "rf64", "rf128", "spare"};
  return convert_enum_idx(
      options, 8, value, "mac_main_cfg_s::dormant_state_timers_r15_c_::setup_s_::scell_hibernation_timer_r15_e_");
}
uint8_t mac_main_cfg_s::dormant_state_timers_r15_c_::setup_s_::scell_hibernation_timer_r15_opts::to_number() const
{
  static const uint8_t options[] = {2, 4, 8, 16, 32, 64, 128};
  return map_enum_number(
      options, 7, value, "mac_main_cfg_s::dormant_state_timers_r15_c_::setup_s_::scell_hibernation_timer_r15_e_");
}

const char*
mac_main_cfg_s::dormant_state_timers_r15_c_::setup_s_::dormant_scell_deactivation_timer_r15_opts::to_string() const
{
  static const char* options[] = {"rf2",
                                  "rf4",
                                  "rf8",
                                  "rf16",
                                  "rf32",
                                  "rf64",
                                  "rf128",
                                  "rf320",
                                  "rf640",
                                  "rf1280",
                                  "rf2560",
                                  "rf5120",
                                  "rf10240",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(
      options,
      16,
      value,
      "mac_main_cfg_s::dormant_state_timers_r15_c_::setup_s_::dormant_scell_deactivation_timer_r15_e_");
}
uint16_t
mac_main_cfg_s::dormant_state_timers_r15_c_::setup_s_::dormant_scell_deactivation_timer_r15_opts::to_number() const
{
  static const uint16_t options[] = {2, 4, 8, 16, 32, 64, 128, 320, 640, 1280, 2560, 5120, 10240};
  return map_enum_number(
      options,
      13,
      value,
      "mac_main_cfg_s::dormant_state_timers_r15_c_::setup_s_::dormant_scell_deactivation_timer_r15_e_");
}

// MeasSubframePatternPCell-r10 ::= CHOICE
void meas_sf_pattern_pcell_r10_c::set(types::options e)
{
  type_ = e;
}
void meas_sf_pattern_pcell_r10_c::set_release()
{
  set(types::release);
}
meas_sf_pattern_r10_c& meas_sf_pattern_pcell_r10_c::set_setup()
{
  set(types::setup);
  return c;
}
void meas_sf_pattern_pcell_r10_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      c.to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "meas_sf_pattern_pcell_r10_c");
  }
  j.end_obj();
}
SRSASN_CODE meas_sf_pattern_pcell_r10_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "meas_sf_pattern_pcell_r10_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_sf_pattern_pcell_r10_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "meas_sf_pattern_pcell_r10_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// NAICS-AssistanceInfo-r12 ::= CHOICE
void naics_assist_info_r12_c::set(types::options e)
{
  type_ = e;
}
void naics_assist_info_r12_c::set_release()
{
  set(types::release);
}
naics_assist_info_r12_c::setup_s_& naics_assist_info_r12_c::set_setup()
{
  set(types::setup);
  return c;
}
void naics_assist_info_r12_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      if (c.neigh_cells_to_release_list_r12_present) {
        j.start_array("neighCellsToReleaseList-r12");
        for (const auto& e1 : c.neigh_cells_to_release_list_r12) {
          j.write_int(e1);
        }
        j.end_array();
      }
      if (c.neigh_cells_to_add_mod_list_r12_present) {
        j.start_array("neighCellsToAddModList-r12");
        for (const auto& e1 : c.neigh_cells_to_add_mod_list_r12) {
          e1.to_json(j);
        }
        j.end_array();
      }
      if (c.serv_cellp_a_r12_present) {
        j.write_str("servCellp-a-r12", c.serv_cellp_a_r12.to_string());
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "naics_assist_info_r12_c");
  }
  j.end_obj();
}
SRSASN_CODE naics_assist_info_r12_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.neigh_cells_to_release_list_r12_present, 1));
      HANDLE_CODE(bref.pack(c.neigh_cells_to_add_mod_list_r12_present, 1));
      HANDLE_CODE(bref.pack(c.serv_cellp_a_r12_present, 1));
      if (c.neigh_cells_to_release_list_r12_present) {
        HANDLE_CODE(pack_dyn_seq_of(bref, c.neigh_cells_to_release_list_r12, 1, 8, integer_packer<uint16_t>(0, 503)));
      }
      if (c.neigh_cells_to_add_mod_list_r12_present) {
        HANDLE_CODE(pack_dyn_seq_of(bref, c.neigh_cells_to_add_mod_list_r12, 1, 8));
      }
      if (c.serv_cellp_a_r12_present) {
        HANDLE_CODE(c.serv_cellp_a_r12.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "naics_assist_info_r12_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE naics_assist_info_r12_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.neigh_cells_to_release_list_r12_present, 1));
      HANDLE_CODE(bref.unpack(c.neigh_cells_to_add_mod_list_r12_present, 1));
      HANDLE_CODE(bref.unpack(c.serv_cellp_a_r12_present, 1));
      if (c.neigh_cells_to_release_list_r12_present) {
        HANDLE_CODE(unpack_dyn_seq_of(c.neigh_cells_to_release_list_r12, bref, 1, 8, integer_packer<uint16_t>(0, 503)));
      }
      if (c.neigh_cells_to_add_mod_list_r12_present) {
        HANDLE_CODE(unpack_dyn_seq_of(c.neigh_cells_to_add_mod_list_r12, bref, 1, 8));
      }
      if (c.serv_cellp_a_r12_present) {
        HANDLE_CODE(c.serv_cellp_a_r12.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "naics_assist_info_r12_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool naics_assist_info_r12_c::operator==(const naics_assist_info_r12_c& other) const
{
  return type() == other.type() and
         c.neigh_cells_to_release_list_r12_present == other.c.neigh_cells_to_release_list_r12_present and
         (not c.neigh_cells_to_release_list_r12_present or
          c.neigh_cells_to_release_list_r12 == other.c.neigh_cells_to_release_list_r12) and
         c.neigh_cells_to_add_mod_list_r12_present == other.c.neigh_cells_to_add_mod_list_r12_present and
         (not c.neigh_cells_to_add_mod_list_r12_present or
          c.neigh_cells_to_add_mod_list_r12 == other.c.neigh_cells_to_add_mod_list_r12) and
         c.serv_cellp_a_r12_present == other.c.serv_cellp_a_r12_present and
         (not c.serv_cellp_a_r12_present or c.serv_cellp_a_r12 == other.c.serv_cellp_a_r12);
}

// NeighCellsCRS-Info-r11 ::= CHOICE
void neigh_cells_crs_info_r11_c::set(types::options e)
{
  type_ = e;
}
void neigh_cells_crs_info_r11_c::set_release()
{
  set(types::release);
}
crs_assist_info_list_r11_l& neigh_cells_crs_info_r11_c::set_setup()
{
  set(types::setup);
  return c;
}
void neigh_cells_crs_info_r11_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.start_array("setup");
      for (const auto& e1 : c) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "neigh_cells_crs_info_r11_c");
  }
  j.end_obj();
}
SRSASN_CODE neigh_cells_crs_info_r11_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(pack_dyn_seq_of(bref, c, 1, 8));
      break;
    default:
      log_invalid_choice_id(type_, "neigh_cells_crs_info_r11_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE neigh_cells_crs_info_r11_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(unpack_dyn_seq_of(c, bref, 1, 8));
      break;
    default:
      log_invalid_choice_id(type_, "neigh_cells_crs_info_r11_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// NeighCellsCRS-Info-r13 ::= CHOICE
void neigh_cells_crs_info_r13_c::set(types::options e)
{
  type_ = e;
}
void neigh_cells_crs_info_r13_c::set_release()
{
  set(types::release);
}
crs_assist_info_list_r13_l& neigh_cells_crs_info_r13_c::set_setup()
{
  set(types::setup);
  return c;
}
void neigh_cells_crs_info_r13_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.start_array("setup");
      for (const auto& e1 : c) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "neigh_cells_crs_info_r13_c");
  }
  j.end_obj();
}
SRSASN_CODE neigh_cells_crs_info_r13_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(pack_dyn_seq_of(bref, c, 1, 8));
      break;
    default:
      log_invalid_choice_id(type_, "neigh_cells_crs_info_r13_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE neigh_cells_crs_info_r13_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(unpack_dyn_seq_of(c, bref, 1, 8));
      break;
    default:
      log_invalid_choice_id(type_, "neigh_cells_crs_info_r13_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool neigh_cells_crs_info_r13_c::operator==(const neigh_cells_crs_info_r13_c& other) const
{
  return type() == other.type() and c == other.c;
}

// NeighCellsCRS-Info-r15 ::= CHOICE
void neigh_cells_crs_info_r15_c::set(types::options e)
{
  type_ = e;
}
void neigh_cells_crs_info_r15_c::set_release()
{
  set(types::release);
}
crs_assist_info_list_r15_l& neigh_cells_crs_info_r15_c::set_setup()
{
  set(types::setup);
  return c;
}
void neigh_cells_crs_info_r15_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.start_array("setup");
      for (const auto& e1 : c) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "neigh_cells_crs_info_r15_c");
  }
  j.end_obj();
}
SRSASN_CODE neigh_cells_crs_info_r15_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(pack_dyn_seq_of(bref, c, 1, 8));
      break;
    default:
      log_invalid_choice_id(type_, "neigh_cells_crs_info_r15_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE neigh_cells_crs_info_r15_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(unpack_dyn_seq_of(c, bref, 1, 8));
      break;
    default:
      log_invalid_choice_id(type_, "neigh_cells_crs_info_r15_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool neigh_cells_crs_info_r15_c::operator==(const neigh_cells_crs_info_r15_c& other) const
{
  return type() == other.type() and c == other.c;
}

// RLF-TimersAndConstants-r13 ::= CHOICE
void rlf_timers_and_consts_r13_c::set(types::options e)
{
  type_ = e;
}
void rlf_timers_and_consts_r13_c::set_release()
{
  set(types::release);
}
rlf_timers_and_consts_r13_c::setup_s_& rlf_timers_and_consts_r13_c::set_setup()
{
  set(types::setup);
  return c;
}
void rlf_timers_and_consts_r13_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      c.to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "rlf_timers_and_consts_r13_c");
  }
  j.end_obj();
}
SRSASN_CODE rlf_timers_and_consts_r13_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rlf_timers_and_consts_r13_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rlf_timers_and_consts_r13_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rlf_timers_and_consts_r13_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

SRSASN_CODE rlf_timers_and_consts_r13_c::setup_s_::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(t301_v1310.pack(bref));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= t310_v1330_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(t310_v1330_present, 1));
      if (t310_v1330_present) {
        HANDLE_CODE(t310_v1330.pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rlf_timers_and_consts_r13_c::setup_s_::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(t301_v1310.unpack(bref));

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(t310_v1330_present, 1));
      if (t310_v1330_present) {
        HANDLE_CODE(t310_v1330.unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void rlf_timers_and_consts_r13_c::setup_s_::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("t301-v1310", t301_v1310.to_string());
  if (ext) {
    if (t310_v1330_present) {
      j.write_str("t310-v1330", t310_v1330.to_string());
    }
  }
  j.end_obj();
}

const char* rlf_timers_and_consts_r13_c::setup_s_::t301_v1310_opts::to_string() const
{
  static const char* options[] = {"ms2500", "ms3000", "ms3500", "ms4000", "ms5000", "ms6000", "ms8000", "ms10000"};
  return convert_enum_idx(options, 8, value, "rlf_timers_and_consts_r13_c::setup_s_::t301_v1310_e_");
}
uint16_t rlf_timers_and_consts_r13_c::setup_s_::t301_v1310_opts::to_number() const
{
  static const uint16_t options[] = {2500, 3000, 3500, 4000, 5000, 6000, 8000, 10000};
  return map_enum_number(options, 8, value, "rlf_timers_and_consts_r13_c::setup_s_::t301_v1310_e_");
}

const char* rlf_timers_and_consts_r13_c::setup_s_::t310_v1330_opts::to_string() const
{
  static const char* options[] = {"ms4000", "ms6000"};
  return convert_enum_idx(options, 2, value, "rlf_timers_and_consts_r13_c::setup_s_::t310_v1330_e_");
}
uint16_t rlf_timers_and_consts_r13_c::setup_s_::t310_v1330_opts::to_number() const
{
  static const uint16_t options[] = {4000, 6000};
  return map_enum_number(options, 2, value, "rlf_timers_and_consts_r13_c::setup_s_::t310_v1330_e_");
}

// RLF-TimersAndConstants-r9 ::= CHOICE
void rlf_timers_and_consts_r9_c::set(types::options e)
{
  type_ = e;
}
void rlf_timers_and_consts_r9_c::set_release()
{
  set(types::release);
}
rlf_timers_and_consts_r9_c::setup_s_& rlf_timers_and_consts_r9_c::set_setup()
{
  set(types::setup);
  return c;
}
void rlf_timers_and_consts_r9_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_str("t301-r9", c.t301_r9.to_string());
      j.write_str("t310-r9", c.t310_r9.to_string());
      j.write_str("n310-r9", c.n310_r9.to_string());
      j.write_str("t311-r9", c.t311_r9.to_string());
      j.write_str("n311-r9", c.n311_r9.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "rlf_timers_and_consts_r9_c");
  }
  j.end_obj();
}
SRSASN_CODE rlf_timers_and_consts_r9_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      bref.pack(c.ext, 1);
      HANDLE_CODE(c.t301_r9.pack(bref));
      HANDLE_CODE(c.t310_r9.pack(bref));
      HANDLE_CODE(c.n310_r9.pack(bref));
      HANDLE_CODE(c.t311_r9.pack(bref));
      HANDLE_CODE(c.n311_r9.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rlf_timers_and_consts_r9_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rlf_timers_and_consts_r9_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      bref.unpack(c.ext, 1);
      HANDLE_CODE(c.t301_r9.unpack(bref));
      HANDLE_CODE(c.t310_r9.unpack(bref));
      HANDLE_CODE(c.n310_r9.unpack(bref));
      HANDLE_CODE(c.t311_r9.unpack(bref));
      HANDLE_CODE(c.n311_r9.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rlf_timers_and_consts_r9_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* rlf_timers_and_consts_r9_c::setup_s_::t301_r9_opts::to_string() const
{
  static const char* options[] = {"ms100", "ms200", "ms300", "ms400", "ms600", "ms1000", "ms1500", "ms2000"};
  return convert_enum_idx(options, 8, value, "rlf_timers_and_consts_r9_c::setup_s_::t301_r9_e_");
}
uint16_t rlf_timers_and_consts_r9_c::setup_s_::t301_r9_opts::to_number() const
{
  static const uint16_t options[] = {100, 200, 300, 400, 600, 1000, 1500, 2000};
  return map_enum_number(options, 8, value, "rlf_timers_and_consts_r9_c::setup_s_::t301_r9_e_");
}

const char* rlf_timers_and_consts_r9_c::setup_s_::t310_r9_opts::to_string() const
{
  static const char* options[] = {"ms0", "ms50", "ms100", "ms200", "ms500", "ms1000", "ms2000"};
  return convert_enum_idx(options, 7, value, "rlf_timers_and_consts_r9_c::setup_s_::t310_r9_e_");
}
uint16_t rlf_timers_and_consts_r9_c::setup_s_::t310_r9_opts::to_number() const
{
  static const uint16_t options[] = {0, 50, 100, 200, 500, 1000, 2000};
  return map_enum_number(options, 7, value, "rlf_timers_and_consts_r9_c::setup_s_::t310_r9_e_");
}

const char* rlf_timers_and_consts_r9_c::setup_s_::n310_r9_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n3", "n4", "n6", "n8", "n10", "n20"};
  return convert_enum_idx(options, 8, value, "rlf_timers_and_consts_r9_c::setup_s_::n310_r9_e_");
}
uint8_t rlf_timers_and_consts_r9_c::setup_s_::n310_r9_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 6, 8, 10, 20};
  return map_enum_number(options, 8, value, "rlf_timers_and_consts_r9_c::setup_s_::n310_r9_e_");
}

const char* rlf_timers_and_consts_r9_c::setup_s_::t311_r9_opts::to_string() const
{
  static const char* options[] = {"ms1000", "ms3000", "ms5000", "ms10000", "ms15000", "ms20000", "ms30000"};
  return convert_enum_idx(options, 7, value, "rlf_timers_and_consts_r9_c::setup_s_::t311_r9_e_");
}
uint16_t rlf_timers_and_consts_r9_c::setup_s_::t311_r9_opts::to_number() const
{
  static const uint16_t options[] = {1000, 3000, 5000, 10000, 15000, 20000, 30000};
  return map_enum_number(options, 7, value, "rlf_timers_and_consts_r9_c::setup_s_::t311_r9_e_");
}

const char* rlf_timers_and_consts_r9_c::setup_s_::n311_r9_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n3", "n4", "n5", "n6", "n8", "n10"};
  return convert_enum_idx(options, 8, value, "rlf_timers_and_consts_r9_c::setup_s_::n311_r9_e_");
}
uint8_t rlf_timers_and_consts_r9_c::setup_s_::n311_r9_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 5, 6, 8, 10};
  return map_enum_number(options, 8, value, "rlf_timers_and_consts_r9_c::setup_s_::n311_r9_e_");
}

// RLF-TimersAndConstantsMCG-Failure-r16 ::= CHOICE
void rlf_timers_and_consts_mcg_fail_r16_c::set(types::options e)
{
  type_ = e;
}
void rlf_timers_and_consts_mcg_fail_r16_c::set_release()
{
  set(types::release);
}
rlf_timers_and_consts_mcg_fail_r16_c::setup_s_& rlf_timers_and_consts_mcg_fail_r16_c::set_setup()
{
  set(types::setup);
  return c;
}
void rlf_timers_and_consts_mcg_fail_r16_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_str("t316-r16", c.t316_r16.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "rlf_timers_and_consts_mcg_fail_r16_c");
  }
  j.end_obj();
}
SRSASN_CODE rlf_timers_and_consts_mcg_fail_r16_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      bref.pack(c.ext, 1);
      HANDLE_CODE(c.t316_r16.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rlf_timers_and_consts_mcg_fail_r16_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rlf_timers_and_consts_mcg_fail_r16_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      bref.unpack(c.ext, 1);
      HANDLE_CODE(c.t316_r16.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rlf_timers_and_consts_mcg_fail_r16_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* rlf_timers_and_consts_mcg_fail_r16_c::setup_s_::t316_r16_opts::to_string() const
{
  static const char* options[] = {
      "ms50", "ms100", "ms200", "ms300", "ms400", "ms500", "ms600", "ms1000", "ms1500", "ms2000"};
  return convert_enum_idx(options, 10, value, "rlf_timers_and_consts_mcg_fail_r16_c::setup_s_::t316_r16_e_");
}
uint16_t rlf_timers_and_consts_mcg_fail_r16_c::setup_s_::t316_r16_opts::to_number() const
{
  static const uint16_t options[] = {50, 100, 200, 300, 400, 500, 600, 1000, 1500, 2000};
  return map_enum_number(options, 10, value, "rlf_timers_and_consts_mcg_fail_r16_c::setup_s_::t316_r16_e_");
}

// SPS-Config ::= SEQUENCE
SRSASN_CODE sps_cfg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(semi_persist_sched_c_rnti_present, 1));
  HANDLE_CODE(bref.pack(sps_cfg_dl_present, 1));
  HANDLE_CODE(bref.pack(sps_cfg_ul_present, 1));

  if (semi_persist_sched_c_rnti_present) {
    HANDLE_CODE(semi_persist_sched_c_rnti.pack(bref));
  }
  if (sps_cfg_dl_present) {
    HANDLE_CODE(sps_cfg_dl.pack(bref));
  }
  if (sps_cfg_ul_present) {
    HANDLE_CODE(sps_cfg_ul.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sps_cfg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(semi_persist_sched_c_rnti_present, 1));
  HANDLE_CODE(bref.unpack(sps_cfg_dl_present, 1));
  HANDLE_CODE(bref.unpack(sps_cfg_ul_present, 1));

  if (semi_persist_sched_c_rnti_present) {
    HANDLE_CODE(semi_persist_sched_c_rnti.unpack(bref));
  }
  if (sps_cfg_dl_present) {
    HANDLE_CODE(sps_cfg_dl.unpack(bref));
  }
  if (sps_cfg_ul_present) {
    HANDLE_CODE(sps_cfg_ul.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sps_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (semi_persist_sched_c_rnti_present) {
    j.write_str("semiPersistSchedC-RNTI", semi_persist_sched_c_rnti.to_string());
  }
  if (sps_cfg_dl_present) {
    j.write_fieldname("sps-ConfigDL");
    sps_cfg_dl.to_json(j);
  }
  if (sps_cfg_ul_present) {
    j.write_fieldname("sps-ConfigUL");
    sps_cfg_ul.to_json(j);
  }
  j.end_obj();
}

// SPS-Config-v1430 ::= SEQUENCE
SRSASN_CODE sps_cfg_v1430_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ul_sps_v_rnti_r14_present, 1));
  HANDLE_CODE(bref.pack(sl_sps_v_rnti_r14_present, 1));
  HANDLE_CODE(bref.pack(sps_cfg_ul_to_add_mod_list_r14_present, 1));
  HANDLE_CODE(bref.pack(sps_cfg_ul_to_release_list_r14_present, 1));
  HANDLE_CODE(bref.pack(sps_cfg_sl_to_add_mod_list_r14_present, 1));
  HANDLE_CODE(bref.pack(sps_cfg_sl_to_release_list_r14_present, 1));

  if (ul_sps_v_rnti_r14_present) {
    HANDLE_CODE(ul_sps_v_rnti_r14.pack(bref));
  }
  if (sl_sps_v_rnti_r14_present) {
    HANDLE_CODE(sl_sps_v_rnti_r14.pack(bref));
  }
  if (sps_cfg_ul_to_add_mod_list_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sps_cfg_ul_to_add_mod_list_r14, 1, 8));
  }
  if (sps_cfg_ul_to_release_list_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sps_cfg_ul_to_release_list_r14, 1, 8, integer_packer<uint8_t>(1, 8)));
  }
  if (sps_cfg_sl_to_add_mod_list_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sps_cfg_sl_to_add_mod_list_r14, 1, 8));
  }
  if (sps_cfg_sl_to_release_list_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sps_cfg_sl_to_release_list_r14, 1, 8, integer_packer<uint8_t>(1, 8)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sps_cfg_v1430_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ul_sps_v_rnti_r14_present, 1));
  HANDLE_CODE(bref.unpack(sl_sps_v_rnti_r14_present, 1));
  HANDLE_CODE(bref.unpack(sps_cfg_ul_to_add_mod_list_r14_present, 1));
  HANDLE_CODE(bref.unpack(sps_cfg_ul_to_release_list_r14_present, 1));
  HANDLE_CODE(bref.unpack(sps_cfg_sl_to_add_mod_list_r14_present, 1));
  HANDLE_CODE(bref.unpack(sps_cfg_sl_to_release_list_r14_present, 1));

  if (ul_sps_v_rnti_r14_present) {
    HANDLE_CODE(ul_sps_v_rnti_r14.unpack(bref));
  }
  if (sl_sps_v_rnti_r14_present) {
    HANDLE_CODE(sl_sps_v_rnti_r14.unpack(bref));
  }
  if (sps_cfg_ul_to_add_mod_list_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sps_cfg_ul_to_add_mod_list_r14, bref, 1, 8));
  }
  if (sps_cfg_ul_to_release_list_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sps_cfg_ul_to_release_list_r14, bref, 1, 8, integer_packer<uint8_t>(1, 8)));
  }
  if (sps_cfg_sl_to_add_mod_list_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sps_cfg_sl_to_add_mod_list_r14, bref, 1, 8));
  }
  if (sps_cfg_sl_to_release_list_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sps_cfg_sl_to_release_list_r14, bref, 1, 8, integer_packer<uint8_t>(1, 8)));
  }

  return SRSASN_SUCCESS;
}
void sps_cfg_v1430_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ul_sps_v_rnti_r14_present) {
    j.write_str("ul-SPS-V-RNTI-r14", ul_sps_v_rnti_r14.to_string());
  }
  if (sl_sps_v_rnti_r14_present) {
    j.write_str("sl-SPS-V-RNTI-r14", sl_sps_v_rnti_r14.to_string());
  }
  if (sps_cfg_ul_to_add_mod_list_r14_present) {
    j.start_array("sps-ConfigUL-ToAddModList-r14");
    for (const auto& e1 : sps_cfg_ul_to_add_mod_list_r14) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sps_cfg_ul_to_release_list_r14_present) {
    j.start_array("sps-ConfigUL-ToReleaseList-r14");
    for (const auto& e1 : sps_cfg_ul_to_release_list_r14) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (sps_cfg_sl_to_add_mod_list_r14_present) {
    j.start_array("sps-ConfigSL-ToAddModList-r14");
    for (const auto& e1 : sps_cfg_sl_to_add_mod_list_r14) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sps_cfg_sl_to_release_list_r14_present) {
    j.start_array("sps-ConfigSL-ToReleaseList-r14");
    for (const auto& e1 : sps_cfg_sl_to_release_list_r14) {
      j.write_int(e1);
    }
    j.end_array();
  }
  j.end_obj();
}

// SPS-Config-v1530 ::= SEQUENCE
SRSASN_CODE sps_cfg_v1530_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(semi_persist_sched_c_rnti_r15_present, 1));
  HANDLE_CODE(bref.pack(sps_cfg_dl_r15_present, 1));
  HANDLE_CODE(bref.pack(sps_cfg_ul_stti_to_add_mod_list_r15_present, 1));
  HANDLE_CODE(bref.pack(sps_cfg_ul_stti_to_release_list_r15_present, 1));
  HANDLE_CODE(bref.pack(sps_cfg_ul_to_add_mod_list_r15_present, 1));
  HANDLE_CODE(bref.pack(sps_cfg_ul_to_release_list_r15_present, 1));

  if (semi_persist_sched_c_rnti_r15_present) {
    HANDLE_CODE(semi_persist_sched_c_rnti_r15.pack(bref));
  }
  if (sps_cfg_dl_r15_present) {
    HANDLE_CODE(sps_cfg_dl_r15.pack(bref));
  }
  if (sps_cfg_ul_stti_to_add_mod_list_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sps_cfg_ul_stti_to_add_mod_list_r15, 1, 6));
  }
  if (sps_cfg_ul_stti_to_release_list_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sps_cfg_ul_stti_to_release_list_r15, 1, 6, integer_packer<uint8_t>(1, 6)));
  }
  if (sps_cfg_ul_to_add_mod_list_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sps_cfg_ul_to_add_mod_list_r15, 1, 6));
  }
  if (sps_cfg_ul_to_release_list_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sps_cfg_ul_to_release_list_r15, 1, 6, integer_packer<uint8_t>(1, 6)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sps_cfg_v1530_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(semi_persist_sched_c_rnti_r15_present, 1));
  HANDLE_CODE(bref.unpack(sps_cfg_dl_r15_present, 1));
  HANDLE_CODE(bref.unpack(sps_cfg_ul_stti_to_add_mod_list_r15_present, 1));
  HANDLE_CODE(bref.unpack(sps_cfg_ul_stti_to_release_list_r15_present, 1));
  HANDLE_CODE(bref.unpack(sps_cfg_ul_to_add_mod_list_r15_present, 1));
  HANDLE_CODE(bref.unpack(sps_cfg_ul_to_release_list_r15_present, 1));

  if (semi_persist_sched_c_rnti_r15_present) {
    HANDLE_CODE(semi_persist_sched_c_rnti_r15.unpack(bref));
  }
  if (sps_cfg_dl_r15_present) {
    HANDLE_CODE(sps_cfg_dl_r15.unpack(bref));
  }
  if (sps_cfg_ul_stti_to_add_mod_list_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sps_cfg_ul_stti_to_add_mod_list_r15, bref, 1, 6));
  }
  if (sps_cfg_ul_stti_to_release_list_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sps_cfg_ul_stti_to_release_list_r15, bref, 1, 6, integer_packer<uint8_t>(1, 6)));
  }
  if (sps_cfg_ul_to_add_mod_list_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sps_cfg_ul_to_add_mod_list_r15, bref, 1, 6));
  }
  if (sps_cfg_ul_to_release_list_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sps_cfg_ul_to_release_list_r15, bref, 1, 6, integer_packer<uint8_t>(1, 6)));
  }

  return SRSASN_SUCCESS;
}
void sps_cfg_v1530_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (semi_persist_sched_c_rnti_r15_present) {
    j.write_str("semiPersistSchedC-RNTI-r15", semi_persist_sched_c_rnti_r15.to_string());
  }
  if (sps_cfg_dl_r15_present) {
    j.write_fieldname("sps-ConfigDL-r15");
    sps_cfg_dl_r15.to_json(j);
  }
  if (sps_cfg_ul_stti_to_add_mod_list_r15_present) {
    j.start_array("sps-ConfigUL-STTI-ToAddModList-r15");
    for (const auto& e1 : sps_cfg_ul_stti_to_add_mod_list_r15) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sps_cfg_ul_stti_to_release_list_r15_present) {
    j.start_array("sps-ConfigUL-STTI-ToReleaseList-r15");
    for (const auto& e1 : sps_cfg_ul_stti_to_release_list_r15) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (sps_cfg_ul_to_add_mod_list_r15_present) {
    j.start_array("sps-ConfigUL-ToAddModList-r15");
    for (const auto& e1 : sps_cfg_ul_to_add_mod_list_r15) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sps_cfg_ul_to_release_list_r15_present) {
    j.start_array("sps-ConfigUL-ToReleaseList-r15");
    for (const auto& e1 : sps_cfg_ul_to_release_list_r15) {
      j.write_int(e1);
    }
    j.end_array();
  }
  j.end_obj();
}
bool sps_cfg_v1530_s::operator==(const sps_cfg_v1530_s& other) const
{
  return semi_persist_sched_c_rnti_r15_present == other.semi_persist_sched_c_rnti_r15_present and
         (not semi_persist_sched_c_rnti_r15_present or
          semi_persist_sched_c_rnti_r15 == other.semi_persist_sched_c_rnti_r15) and
         sps_cfg_dl_r15_present == other.sps_cfg_dl_r15_present and
         (not sps_cfg_dl_r15_present or sps_cfg_dl_r15 == other.sps_cfg_dl_r15) and
         sps_cfg_ul_stti_to_add_mod_list_r15_present == other.sps_cfg_ul_stti_to_add_mod_list_r15_present and
         (not sps_cfg_ul_stti_to_add_mod_list_r15_present or
          sps_cfg_ul_stti_to_add_mod_list_r15 == other.sps_cfg_ul_stti_to_add_mod_list_r15) and
         sps_cfg_ul_stti_to_release_list_r15_present == other.sps_cfg_ul_stti_to_release_list_r15_present and
         (not sps_cfg_ul_stti_to_release_list_r15_present or
          sps_cfg_ul_stti_to_release_list_r15 == other.sps_cfg_ul_stti_to_release_list_r15) and
         sps_cfg_ul_to_add_mod_list_r15_present == other.sps_cfg_ul_to_add_mod_list_r15_present and
         (not sps_cfg_ul_to_add_mod_list_r15_present or
          sps_cfg_ul_to_add_mod_list_r15 == other.sps_cfg_ul_to_add_mod_list_r15) and
         sps_cfg_ul_to_release_list_r15_present == other.sps_cfg_ul_to_release_list_r15_present and
         (not sps_cfg_ul_to_release_list_r15_present or
          sps_cfg_ul_to_release_list_r15 == other.sps_cfg_ul_to_release_list_r15);
}

// SPS-Config-v1540 ::= SEQUENCE
SRSASN_CODE sps_cfg_v1540_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sps_cfg_dl_stti_r15_present, 1));

  if (sps_cfg_dl_stti_r15_present) {
    HANDLE_CODE(sps_cfg_dl_stti_r15.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sps_cfg_v1540_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sps_cfg_dl_stti_r15_present, 1));

  if (sps_cfg_dl_stti_r15_present) {
    HANDLE_CODE(sps_cfg_dl_stti_r15.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sps_cfg_v1540_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sps_cfg_dl_stti_r15_present) {
    j.write_fieldname("sps-ConfigDL-STTI-r15");
    sps_cfg_dl_stti_r15.to_json(j);
  }
  j.end_obj();
}

// RadioResourceConfigDedicated ::= SEQUENCE
SRSASN_CODE rr_cfg_ded_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(srb_to_add_mod_list_present, 1));
  HANDLE_CODE(bref.pack(drb_to_add_mod_list_present, 1));
  HANDLE_CODE(bref.pack(drb_to_release_list_present, 1));
  HANDLE_CODE(bref.pack(mac_main_cfg_present, 1));
  HANDLE_CODE(bref.pack(sps_cfg_present, 1));
  HANDLE_CODE(bref.pack(phys_cfg_ded_present, 1));

  if (srb_to_add_mod_list_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, srb_to_add_mod_list, 1, 2));
  }
  if (drb_to_add_mod_list_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, drb_to_add_mod_list, 1, 11));
  }
  if (drb_to_release_list_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, drb_to_release_list, 1, 11, integer_packer<uint8_t>(1, 32)));
  }
  if (mac_main_cfg_present) {
    HANDLE_CODE(mac_main_cfg.pack(bref));
  }
  if (sps_cfg_present) {
    HANDLE_CODE(sps_cfg.pack(bref));
  }
  if (phys_cfg_ded_present) {
    HANDLE_CODE(phys_cfg_ded.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= rlf_timers_and_consts_r9.is_present();
    group_flags[1] |= meas_sf_pattern_pcell_r10.is_present();
    group_flags[2] |= neigh_cells_crs_info_r11.is_present();
    group_flags[3] |= naics_info_r12.is_present();
    group_flags[4] |= neigh_cells_crs_info_r13.is_present();
    group_flags[4] |= rlf_timers_and_consts_r13.is_present();
    group_flags[5] |= sps_cfg_v1430.is_present();
    group_flags[6] |= srb_to_add_mod_list_ext_r15.is_present();
    group_flags[6] |= srb_to_release_list_ext_r15_present;
    group_flags[6] |= sps_cfg_v1530.is_present();
    group_flags[6] |= crs_intf_mitig_cfg_r15.is_present();
    group_flags[6] |= neigh_cells_crs_info_r15.is_present();
    group_flags[6] |= drb_to_add_mod_list_r15.is_present();
    group_flags[6] |= drb_to_release_list_r15.is_present();
    group_flags[6] |= dummy.is_present();
    group_flags[7] |= sps_cfg_v1540.is_present();
    group_flags[8] |= rlf_timers_and_consts_mcg_fail_r16.is_present();
    group_flags[8] |= crs_ch_est_mpdcch_cfg_ded_r16.is_present();
    group_flags[8] |= new_ue_id_r16_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(rlf_timers_and_consts_r9.is_present(), 1));
      if (rlf_timers_and_consts_r9.is_present()) {
        HANDLE_CODE(rlf_timers_and_consts_r9->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(meas_sf_pattern_pcell_r10.is_present(), 1));
      if (meas_sf_pattern_pcell_r10.is_present()) {
        HANDLE_CODE(meas_sf_pattern_pcell_r10->pack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(neigh_cells_crs_info_r11.is_present(), 1));
      if (neigh_cells_crs_info_r11.is_present()) {
        HANDLE_CODE(neigh_cells_crs_info_r11->pack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(naics_info_r12.is_present(), 1));
      if (naics_info_r12.is_present()) {
        HANDLE_CODE(naics_info_r12->pack(bref));
      }
    }
    if (group_flags[4]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(neigh_cells_crs_info_r13.is_present(), 1));
      HANDLE_CODE(bref.pack(rlf_timers_and_consts_r13.is_present(), 1));
      if (neigh_cells_crs_info_r13.is_present()) {
        HANDLE_CODE(neigh_cells_crs_info_r13->pack(bref));
      }
      if (rlf_timers_and_consts_r13.is_present()) {
        HANDLE_CODE(rlf_timers_and_consts_r13->pack(bref));
      }
    }
    if (group_flags[5]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(sps_cfg_v1430.is_present(), 1));
      if (sps_cfg_v1430.is_present()) {
        HANDLE_CODE(sps_cfg_v1430->pack(bref));
      }
    }
    if (group_flags[6]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(srb_to_add_mod_list_ext_r15.is_present(), 1));
      HANDLE_CODE(bref.pack(srb_to_release_list_ext_r15_present, 1));
      HANDLE_CODE(bref.pack(sps_cfg_v1530.is_present(), 1));
      HANDLE_CODE(bref.pack(crs_intf_mitig_cfg_r15.is_present(), 1));
      HANDLE_CODE(bref.pack(neigh_cells_crs_info_r15.is_present(), 1));
      HANDLE_CODE(bref.pack(drb_to_add_mod_list_r15.is_present(), 1));
      HANDLE_CODE(bref.pack(drb_to_release_list_r15.is_present(), 1));
      HANDLE_CODE(bref.pack(dummy.is_present(), 1));
      if (srb_to_add_mod_list_ext_r15.is_present()) {
        HANDLE_CODE(pack_fixed_seq_of(bref, &(*srb_to_add_mod_list_ext_r15)[0], srb_to_add_mod_list_ext_r15->size()));
      }
      if (srb_to_release_list_ext_r15_present) {
        HANDLE_CODE(pack_integer(bref, srb_to_release_list_ext_r15, (uint8_t)4u, (uint8_t)4u));
      }
      if (sps_cfg_v1530.is_present()) {
        HANDLE_CODE(sps_cfg_v1530->pack(bref));
      }
      if (crs_intf_mitig_cfg_r15.is_present()) {
        HANDLE_CODE(crs_intf_mitig_cfg_r15->pack(bref));
      }
      if (neigh_cells_crs_info_r15.is_present()) {
        HANDLE_CODE(neigh_cells_crs_info_r15->pack(bref));
      }
      if (drb_to_add_mod_list_r15.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *drb_to_add_mod_list_r15, 1, 15));
      }
      if (drb_to_release_list_r15.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *drb_to_release_list_r15, 1, 15, integer_packer<uint8_t>(1, 32)));
      }
      if (dummy.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *dummy, 1, 2, integer_packer<uint8_t>(1, 2)));
      }
    }
    if (group_flags[7]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(sps_cfg_v1540.is_present(), 1));
      if (sps_cfg_v1540.is_present()) {
        HANDLE_CODE(sps_cfg_v1540->pack(bref));
      }
    }
    if (group_flags[8]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(rlf_timers_and_consts_mcg_fail_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(crs_ch_est_mpdcch_cfg_ded_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(new_ue_id_r16_present, 1));
      if (rlf_timers_and_consts_mcg_fail_r16.is_present()) {
        HANDLE_CODE(rlf_timers_and_consts_mcg_fail_r16->pack(bref));
      }
      if (crs_ch_est_mpdcch_cfg_ded_r16.is_present()) {
        HANDLE_CODE(crs_ch_est_mpdcch_cfg_ded_r16->pack(bref));
      }
      if (new_ue_id_r16_present) {
        HANDLE_CODE(new_ue_id_r16.pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rr_cfg_ded_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(srb_to_add_mod_list_present, 1));
  HANDLE_CODE(bref.unpack(drb_to_add_mod_list_present, 1));
  HANDLE_CODE(bref.unpack(drb_to_release_list_present, 1));
  HANDLE_CODE(bref.unpack(mac_main_cfg_present, 1));
  HANDLE_CODE(bref.unpack(sps_cfg_present, 1));
  HANDLE_CODE(bref.unpack(phys_cfg_ded_present, 1));

  if (srb_to_add_mod_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(srb_to_add_mod_list, bref, 1, 2));
  }
  if (drb_to_add_mod_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(drb_to_add_mod_list, bref, 1, 11));
  }
  if (drb_to_release_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(drb_to_release_list, bref, 1, 11, integer_packer<uint8_t>(1, 32)));
  }
  if (mac_main_cfg_present) {
    HANDLE_CODE(mac_main_cfg.unpack(bref));
  }
  if (sps_cfg_present) {
    HANDLE_CODE(sps_cfg.unpack(bref));
  }
  if (phys_cfg_ded_present) {
    HANDLE_CODE(phys_cfg_ded.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(9);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool rlf_timers_and_consts_r9_present;
      HANDLE_CODE(bref.unpack(rlf_timers_and_consts_r9_present, 1));
      rlf_timers_and_consts_r9.set_present(rlf_timers_and_consts_r9_present);
      if (rlf_timers_and_consts_r9.is_present()) {
        HANDLE_CODE(rlf_timers_and_consts_r9->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool meas_sf_pattern_pcell_r10_present;
      HANDLE_CODE(bref.unpack(meas_sf_pattern_pcell_r10_present, 1));
      meas_sf_pattern_pcell_r10.set_present(meas_sf_pattern_pcell_r10_present);
      if (meas_sf_pattern_pcell_r10.is_present()) {
        HANDLE_CODE(meas_sf_pattern_pcell_r10->unpack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool neigh_cells_crs_info_r11_present;
      HANDLE_CODE(bref.unpack(neigh_cells_crs_info_r11_present, 1));
      neigh_cells_crs_info_r11.set_present(neigh_cells_crs_info_r11_present);
      if (neigh_cells_crs_info_r11.is_present()) {
        HANDLE_CODE(neigh_cells_crs_info_r11->unpack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool naics_info_r12_present;
      HANDLE_CODE(bref.unpack(naics_info_r12_present, 1));
      naics_info_r12.set_present(naics_info_r12_present);
      if (naics_info_r12.is_present()) {
        HANDLE_CODE(naics_info_r12->unpack(bref));
      }
    }
    if (group_flags[4]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool neigh_cells_crs_info_r13_present;
      HANDLE_CODE(bref.unpack(neigh_cells_crs_info_r13_present, 1));
      neigh_cells_crs_info_r13.set_present(neigh_cells_crs_info_r13_present);
      bool rlf_timers_and_consts_r13_present;
      HANDLE_CODE(bref.unpack(rlf_timers_and_consts_r13_present, 1));
      rlf_timers_and_consts_r13.set_present(rlf_timers_and_consts_r13_present);
      if (neigh_cells_crs_info_r13.is_present()) {
        HANDLE_CODE(neigh_cells_crs_info_r13->unpack(bref));
      }
      if (rlf_timers_and_consts_r13.is_present()) {
        HANDLE_CODE(rlf_timers_and_consts_r13->unpack(bref));
      }
    }
    if (group_flags[5]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool sps_cfg_v1430_present;
      HANDLE_CODE(bref.unpack(sps_cfg_v1430_present, 1));
      sps_cfg_v1430.set_present(sps_cfg_v1430_present);
      if (sps_cfg_v1430.is_present()) {
        HANDLE_CODE(sps_cfg_v1430->unpack(bref));
      }
    }
    if (group_flags[6]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool srb_to_add_mod_list_ext_r15_present;
      HANDLE_CODE(bref.unpack(srb_to_add_mod_list_ext_r15_present, 1));
      srb_to_add_mod_list_ext_r15.set_present(srb_to_add_mod_list_ext_r15_present);
      HANDLE_CODE(bref.unpack(srb_to_release_list_ext_r15_present, 1));
      bool sps_cfg_v1530_present;
      HANDLE_CODE(bref.unpack(sps_cfg_v1530_present, 1));
      sps_cfg_v1530.set_present(sps_cfg_v1530_present);
      bool crs_intf_mitig_cfg_r15_present;
      HANDLE_CODE(bref.unpack(crs_intf_mitig_cfg_r15_present, 1));
      crs_intf_mitig_cfg_r15.set_present(crs_intf_mitig_cfg_r15_present);
      bool neigh_cells_crs_info_r15_present;
      HANDLE_CODE(bref.unpack(neigh_cells_crs_info_r15_present, 1));
      neigh_cells_crs_info_r15.set_present(neigh_cells_crs_info_r15_present);
      bool drb_to_add_mod_list_r15_present;
      HANDLE_CODE(bref.unpack(drb_to_add_mod_list_r15_present, 1));
      drb_to_add_mod_list_r15.set_present(drb_to_add_mod_list_r15_present);
      bool drb_to_release_list_r15_present;
      HANDLE_CODE(bref.unpack(drb_to_release_list_r15_present, 1));
      drb_to_release_list_r15.set_present(drb_to_release_list_r15_present);
      bool dummy_present;
      HANDLE_CODE(bref.unpack(dummy_present, 1));
      dummy.set_present(dummy_present);
      if (srb_to_add_mod_list_ext_r15.is_present()) {
        HANDLE_CODE(unpack_fixed_seq_of(&(*srb_to_add_mod_list_ext_r15)[0], bref, srb_to_add_mod_list_ext_r15->size()));
      }
      if (srb_to_release_list_ext_r15_present) {
        HANDLE_CODE(unpack_integer(srb_to_release_list_ext_r15, bref, (uint8_t)4u, (uint8_t)4u));
      }
      if (sps_cfg_v1530.is_present()) {
        HANDLE_CODE(sps_cfg_v1530->unpack(bref));
      }
      if (crs_intf_mitig_cfg_r15.is_present()) {
        HANDLE_CODE(crs_intf_mitig_cfg_r15->unpack(bref));
      }
      if (neigh_cells_crs_info_r15.is_present()) {
        HANDLE_CODE(neigh_cells_crs_info_r15->unpack(bref));
      }
      if (drb_to_add_mod_list_r15.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*drb_to_add_mod_list_r15, bref, 1, 15));
      }
      if (drb_to_release_list_r15.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*drb_to_release_list_r15, bref, 1, 15, integer_packer<uint8_t>(1, 32)));
      }
      if (dummy.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*dummy, bref, 1, 2, integer_packer<uint8_t>(1, 2)));
      }
    }
    if (group_flags[7]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool sps_cfg_v1540_present;
      HANDLE_CODE(bref.unpack(sps_cfg_v1540_present, 1));
      sps_cfg_v1540.set_present(sps_cfg_v1540_present);
      if (sps_cfg_v1540.is_present()) {
        HANDLE_CODE(sps_cfg_v1540->unpack(bref));
      }
    }
    if (group_flags[8]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool rlf_timers_and_consts_mcg_fail_r16_present;
      HANDLE_CODE(bref.unpack(rlf_timers_and_consts_mcg_fail_r16_present, 1));
      rlf_timers_and_consts_mcg_fail_r16.set_present(rlf_timers_and_consts_mcg_fail_r16_present);
      bool crs_ch_est_mpdcch_cfg_ded_r16_present;
      HANDLE_CODE(bref.unpack(crs_ch_est_mpdcch_cfg_ded_r16_present, 1));
      crs_ch_est_mpdcch_cfg_ded_r16.set_present(crs_ch_est_mpdcch_cfg_ded_r16_present);
      HANDLE_CODE(bref.unpack(new_ue_id_r16_present, 1));
      if (rlf_timers_and_consts_mcg_fail_r16.is_present()) {
        HANDLE_CODE(rlf_timers_and_consts_mcg_fail_r16->unpack(bref));
      }
      if (crs_ch_est_mpdcch_cfg_ded_r16.is_present()) {
        HANDLE_CODE(crs_ch_est_mpdcch_cfg_ded_r16->unpack(bref));
      }
      if (new_ue_id_r16_present) {
        HANDLE_CODE(new_ue_id_r16.unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void rr_cfg_ded_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (srb_to_add_mod_list_present) {
    j.start_array("srb-ToAddModList");
    for (const auto& e1 : srb_to_add_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (drb_to_add_mod_list_present) {
    j.start_array("drb-ToAddModList");
    for (const auto& e1 : drb_to_add_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (drb_to_release_list_present) {
    j.start_array("drb-ToReleaseList");
    for (const auto& e1 : drb_to_release_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (mac_main_cfg_present) {
    j.write_fieldname("mac-MainConfig");
    mac_main_cfg.to_json(j);
  }
  if (sps_cfg_present) {
    j.write_fieldname("sps-Config");
    sps_cfg.to_json(j);
  }
  if (phys_cfg_ded_present) {
    j.write_fieldname("physicalConfigDedicated");
    phys_cfg_ded.to_json(j);
  }
  if (ext) {
    if (rlf_timers_and_consts_r9.is_present()) {
      j.write_fieldname("rlf-TimersAndConstants-r9");
      rlf_timers_and_consts_r9->to_json(j);
    }
    if (meas_sf_pattern_pcell_r10.is_present()) {
      j.write_fieldname("measSubframePatternPCell-r10");
      meas_sf_pattern_pcell_r10->to_json(j);
    }
    if (neigh_cells_crs_info_r11.is_present()) {
      j.write_fieldname("neighCellsCRS-Info-r11");
      neigh_cells_crs_info_r11->to_json(j);
    }
    if (naics_info_r12.is_present()) {
      j.write_fieldname("naics-Info-r12");
      naics_info_r12->to_json(j);
    }
    if (neigh_cells_crs_info_r13.is_present()) {
      j.write_fieldname("neighCellsCRS-Info-r13");
      neigh_cells_crs_info_r13->to_json(j);
    }
    if (rlf_timers_and_consts_r13.is_present()) {
      j.write_fieldname("rlf-TimersAndConstants-r13");
      rlf_timers_and_consts_r13->to_json(j);
    }
    if (sps_cfg_v1430.is_present()) {
      j.write_fieldname("sps-Config-v1430");
      sps_cfg_v1430->to_json(j);
    }
    if (srb_to_add_mod_list_ext_r15.is_present()) {
      j.start_array("srb-ToAddModListExt-r15");
      for (const auto& e1 : *srb_to_add_mod_list_ext_r15) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (srb_to_release_list_ext_r15_present) {
      j.write_int("srb-ToReleaseListExt-r15", srb_to_release_list_ext_r15);
    }
    if (sps_cfg_v1530.is_present()) {
      j.write_fieldname("sps-Config-v1530");
      sps_cfg_v1530->to_json(j);
    }
    if (crs_intf_mitig_cfg_r15.is_present()) {
      j.write_fieldname("crs-IntfMitigConfig-r15");
      crs_intf_mitig_cfg_r15->to_json(j);
    }
    if (neigh_cells_crs_info_r15.is_present()) {
      j.write_fieldname("neighCellsCRS-Info-r15");
      neigh_cells_crs_info_r15->to_json(j);
    }
    if (drb_to_add_mod_list_r15.is_present()) {
      j.start_array("drb-ToAddModList-r15");
      for (const auto& e1 : *drb_to_add_mod_list_r15) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (drb_to_release_list_r15.is_present()) {
      j.start_array("drb-ToReleaseList-r15");
      for (const auto& e1 : *drb_to_release_list_r15) {
        j.write_int(e1);
      }
      j.end_array();
    }
    if (dummy.is_present()) {
      j.start_array("dummy");
      for (const auto& e1 : *dummy) {
        j.write_int(e1);
      }
      j.end_array();
    }
    if (sps_cfg_v1540.is_present()) {
      j.write_fieldname("sps-Config-v1540");
      sps_cfg_v1540->to_json(j);
    }
    if (rlf_timers_and_consts_mcg_fail_r16.is_present()) {
      j.write_fieldname("rlf-TimersAndConstantsMCG-Failure-r16");
      rlf_timers_and_consts_mcg_fail_r16->to_json(j);
    }
    if (crs_ch_est_mpdcch_cfg_ded_r16.is_present()) {
      j.write_fieldname("crs-ChEstMPDCCH-ConfigDedicated-r16");
      crs_ch_est_mpdcch_cfg_ded_r16->to_json(j);
    }
    if (new_ue_id_r16_present) {
      j.write_str("newUE-Identity-r16", new_ue_id_r16.to_string());
    }
  }
  j.end_obj();
}

void rr_cfg_ded_s::mac_main_cfg_c_::set(types::options e)
{
  type_ = e;
}
mac_main_cfg_s& rr_cfg_ded_s::mac_main_cfg_c_::set_explicit_value()
{
  set(types::explicit_value);
  return c;
}
void rr_cfg_ded_s::mac_main_cfg_c_::set_default_value()
{
  set(types::default_value);
}
void rr_cfg_ded_s::mac_main_cfg_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::explicit_value:
      j.write_fieldname("explicitValue");
      c.to_json(j);
      break;
    case types::default_value:
      break;
    default:
      log_invalid_choice_id(type_, "rr_cfg_ded_s::mac_main_cfg_c_");
  }
  j.end_obj();
}
SRSASN_CODE rr_cfg_ded_s::mac_main_cfg_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::explicit_value:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::default_value:
      break;
    default:
      log_invalid_choice_id(type_, "rr_cfg_ded_s::mac_main_cfg_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rr_cfg_ded_s::mac_main_cfg_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::explicit_value:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::default_value:
      break;
    default:
      log_invalid_choice_id(type_, "rr_cfg_ded_s::mac_main_cfg_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void rr_cfg_ded_s::crs_intf_mitig_cfg_r15_c_::set(types::options e)
{
  type_ = e;
}
void rr_cfg_ded_s::crs_intf_mitig_cfg_r15_c_::set_release()
{
  set(types::release);
}
rr_cfg_ded_s::crs_intf_mitig_cfg_r15_c_::setup_c_& rr_cfg_ded_s::crs_intf_mitig_cfg_r15_c_::set_setup()
{
  set(types::setup);
  return c;
}
void rr_cfg_ded_s::crs_intf_mitig_cfg_r15_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      c.to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "rr_cfg_ded_s::crs_intf_mitig_cfg_r15_c_");
  }
  j.end_obj();
}
SRSASN_CODE rr_cfg_ded_s::crs_intf_mitig_cfg_r15_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rr_cfg_ded_s::crs_intf_mitig_cfg_r15_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rr_cfg_ded_s::crs_intf_mitig_cfg_r15_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rr_cfg_ded_s::crs_intf_mitig_cfg_r15_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void rr_cfg_ded_s::crs_intf_mitig_cfg_r15_c_::setup_c_::set(types::options e)
{
  type_ = e;
}
void rr_cfg_ded_s::crs_intf_mitig_cfg_r15_c_::setup_c_::set_crs_intf_mitig_enabled()
{
  set(types::crs_intf_mitig_enabled);
}
rr_cfg_ded_s::crs_intf_mitig_cfg_r15_c_::setup_c_::crs_intf_mitig_num_prbs_e_&
rr_cfg_ded_s::crs_intf_mitig_cfg_r15_c_::setup_c_::set_crs_intf_mitig_num_prbs()
{
  set(types::crs_intf_mitig_num_prbs);
  return c;
}
void rr_cfg_ded_s::crs_intf_mitig_cfg_r15_c_::setup_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::crs_intf_mitig_enabled:
      break;
    case types::crs_intf_mitig_num_prbs:
      j.write_str("crs-IntfMitigNumPRBs", c.to_string());
      break;
    default:
      log_invalid_choice_id(type_, "rr_cfg_ded_s::crs_intf_mitig_cfg_r15_c_::setup_c_");
  }
  j.end_obj();
}
SRSASN_CODE rr_cfg_ded_s::crs_intf_mitig_cfg_r15_c_::setup_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::crs_intf_mitig_enabled:
      break;
    case types::crs_intf_mitig_num_prbs:
      HANDLE_CODE(c.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rr_cfg_ded_s::crs_intf_mitig_cfg_r15_c_::setup_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rr_cfg_ded_s::crs_intf_mitig_cfg_r15_c_::setup_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::crs_intf_mitig_enabled:
      break;
    case types::crs_intf_mitig_num_prbs:
      HANDLE_CODE(c.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rr_cfg_ded_s::crs_intf_mitig_cfg_r15_c_::setup_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* rr_cfg_ded_s::crs_intf_mitig_cfg_r15_c_::setup_c_::crs_intf_mitig_num_prbs_opts::to_string() const
{
  static const char* options[] = {"n6", "n24"};
  return convert_enum_idx(
      options, 2, value, "rr_cfg_ded_s::crs_intf_mitig_cfg_r15_c_::setup_c_::crs_intf_mitig_num_prbs_e_");
}
uint8_t rr_cfg_ded_s::crs_intf_mitig_cfg_r15_c_::setup_c_::crs_intf_mitig_num_prbs_opts::to_number() const
{
  static const uint8_t options[] = {6, 24};
  return map_enum_number(
      options, 2, value, "rr_cfg_ded_s::crs_intf_mitig_cfg_r15_c_::setup_c_::crs_intf_mitig_num_prbs_e_");
}

// PDCCH-CandidateReductionValue-r14 ::= ENUMERATED
const char* pdcch_candidate_reduction_value_r14_opts::to_string() const
{
  static const char* options[] = {"n0", "n50", "n100", "n150"};
  return convert_enum_idx(options, 4, value, "pdcch_candidate_reduction_value_r14_e");
}
uint8_t pdcch_candidate_reduction_value_r14_opts::to_number() const
{
  static const uint8_t options[] = {0, 50, 100, 150};
  return map_enum_number(options, 4, value, "pdcch_candidate_reduction_value_r14_e");
}

// PDCCH-CandidateReductionsLAA-UL-r14 ::= CHOICE
void pdcch_candidate_reductions_laa_ul_r14_c::set(types::options e)
{
  type_ = e;
}
void pdcch_candidate_reductions_laa_ul_r14_c::set_release()
{
  set(types::release);
}
pdcch_candidate_reductions_laa_ul_r14_c::setup_s_& pdcch_candidate_reductions_laa_ul_r14_c::set_setup()
{
  set(types::setup);
  return c;
}
void pdcch_candidate_reductions_laa_ul_r14_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_str("pdcch-candidateReductionAL1-r14", c.pdcch_candidate_reduction_al1_r14.to_string());
      j.write_str("pdcch-candidateReductionAL2-r14", c.pdcch_candidate_reduction_al2_r14.to_string());
      j.write_str("pdcch-candidateReductionAL3-r14", c.pdcch_candidate_reduction_al3_r14.to_string());
      j.write_str("pdcch-candidateReductionAL4-r14", c.pdcch_candidate_reduction_al4_r14.to_string());
      j.write_str("pdcch-candidateReductionAL5-r14", c.pdcch_candidate_reduction_al5_r14.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "pdcch_candidate_reductions_laa_ul_r14_c");
  }
  j.end_obj();
}
SRSASN_CODE pdcch_candidate_reductions_laa_ul_r14_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.pdcch_candidate_reduction_al1_r14.pack(bref));
      HANDLE_CODE(c.pdcch_candidate_reduction_al2_r14.pack(bref));
      HANDLE_CODE(c.pdcch_candidate_reduction_al3_r14.pack(bref));
      HANDLE_CODE(c.pdcch_candidate_reduction_al4_r14.pack(bref));
      HANDLE_CODE(c.pdcch_candidate_reduction_al5_r14.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "pdcch_candidate_reductions_laa_ul_r14_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pdcch_candidate_reductions_laa_ul_r14_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.pdcch_candidate_reduction_al1_r14.unpack(bref));
      HANDLE_CODE(c.pdcch_candidate_reduction_al2_r14.unpack(bref));
      HANDLE_CODE(c.pdcch_candidate_reduction_al3_r14.unpack(bref));
      HANDLE_CODE(c.pdcch_candidate_reduction_al4_r14.unpack(bref));
      HANDLE_CODE(c.pdcch_candidate_reduction_al5_r14.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "pdcch_candidate_reductions_laa_ul_r14_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool pdcch_candidate_reductions_laa_ul_r14_c::operator==(const pdcch_candidate_reductions_laa_ul_r14_c& other) const
{
  return type() == other.type() and c.pdcch_candidate_reduction_al1_r14 == other.c.pdcch_candidate_reduction_al1_r14 and
         c.pdcch_candidate_reduction_al2_r14 == other.c.pdcch_candidate_reduction_al2_r14 and
         c.pdcch_candidate_reduction_al3_r14 == other.c.pdcch_candidate_reduction_al3_r14 and
         c.pdcch_candidate_reduction_al4_r14 == other.c.pdcch_candidate_reduction_al4_r14 and
         c.pdcch_candidate_reduction_al5_r14 == other.c.pdcch_candidate_reduction_al5_r14;
}

// AUL-Config-r15 ::= CHOICE
void aul_cfg_r15_c::set(types::options e)
{
  type_ = e;
}
void aul_cfg_r15_c::set_release()
{
  set(types::release);
}
aul_cfg_r15_c::setup_s_& aul_cfg_r15_c::set_setup()
{
  set(types::setup);
  return c;
}
void aul_cfg_r15_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_str("aul-CRNTI-r15", c.aul_crnti_r15.to_string());
      j.write_str("aul-Subframes-r15", c.aul_sfs_r15.to_string());
      j.write_int("aul-HARQ-Processes-r15", c.aul_harq_processes_r15);
      j.write_str("transmissionModeUL-AUL-r15", c.tx_mode_ul_aul_r15.to_string());
      j.write_str("aul-StartingFullBW-InsideMCOT-r15", c.aul_start_full_bw_inside_mcot_r15.to_string());
      j.write_str("aul-StartingFullBW-OutsideMCOT-r15", c.aul_start_full_bw_outside_mcot_r15.to_string());
      j.write_str("aul-StartingPartialBW-InsideMCOT-r15", c.aul_start_partial_bw_inside_mcot_r15.to_string());
      j.write_str("aul-StartingPartialBW-OutsideMCOT-r15", c.aul_start_partial_bw_outside_mcot_r15.to_string());
      j.write_str("aul-RetransmissionTimer-r15", c.aul_retx_timer_r15.to_string());
      j.write_int("endingSymbolAUL-r15", c.ending_symbol_aul_r15);
      j.write_int("subframeOffsetCOT-Sharing-r15", c.sf_offset_cot_sharing_r15);
      j.write_str("contentionWindowSizeTimer-r15", c.contention_win_size_timer_r15.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "aul_cfg_r15_c");
  }
  j.end_obj();
}
SRSASN_CODE aul_cfg_r15_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.aul_crnti_r15.pack(bref));
      HANDLE_CODE(c.aul_sfs_r15.pack(bref));
      HANDLE_CODE(pack_integer(bref, c.aul_harq_processes_r15, (uint8_t)1u, (uint8_t)16u));
      HANDLE_CODE(c.tx_mode_ul_aul_r15.pack(bref));
      HANDLE_CODE(c.aul_start_full_bw_inside_mcot_r15.pack(bref));
      HANDLE_CODE(c.aul_start_full_bw_outside_mcot_r15.pack(bref));
      HANDLE_CODE(c.aul_start_partial_bw_inside_mcot_r15.pack(bref));
      HANDLE_CODE(c.aul_start_partial_bw_outside_mcot_r15.pack(bref));
      HANDLE_CODE(c.aul_retx_timer_r15.pack(bref));
      HANDLE_CODE(pack_integer(bref, c.ending_symbol_aul_r15, (uint8_t)12u, (uint8_t)13u));
      HANDLE_CODE(pack_integer(bref, c.sf_offset_cot_sharing_r15, (uint8_t)2u, (uint8_t)4u));
      HANDLE_CODE(c.contention_win_size_timer_r15.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "aul_cfg_r15_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE aul_cfg_r15_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.aul_crnti_r15.unpack(bref));
      HANDLE_CODE(c.aul_sfs_r15.unpack(bref));
      HANDLE_CODE(unpack_integer(c.aul_harq_processes_r15, bref, (uint8_t)1u, (uint8_t)16u));
      HANDLE_CODE(c.tx_mode_ul_aul_r15.unpack(bref));
      HANDLE_CODE(c.aul_start_full_bw_inside_mcot_r15.unpack(bref));
      HANDLE_CODE(c.aul_start_full_bw_outside_mcot_r15.unpack(bref));
      HANDLE_CODE(c.aul_start_partial_bw_inside_mcot_r15.unpack(bref));
      HANDLE_CODE(c.aul_start_partial_bw_outside_mcot_r15.unpack(bref));
      HANDLE_CODE(c.aul_retx_timer_r15.unpack(bref));
      HANDLE_CODE(unpack_integer(c.ending_symbol_aul_r15, bref, (uint8_t)12u, (uint8_t)13u));
      HANDLE_CODE(unpack_integer(c.sf_offset_cot_sharing_r15, bref, (uint8_t)2u, (uint8_t)4u));
      HANDLE_CODE(c.contention_win_size_timer_r15.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "aul_cfg_r15_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool aul_cfg_r15_c::operator==(const aul_cfg_r15_c& other) const
{
  return type() == other.type() and c.aul_crnti_r15 == other.c.aul_crnti_r15 and
         c.aul_sfs_r15 == other.c.aul_sfs_r15 and c.aul_harq_processes_r15 == other.c.aul_harq_processes_r15 and
         c.tx_mode_ul_aul_r15 == other.c.tx_mode_ul_aul_r15 and
         c.aul_start_full_bw_inside_mcot_r15 == other.c.aul_start_full_bw_inside_mcot_r15 and
         c.aul_start_full_bw_outside_mcot_r15 == other.c.aul_start_full_bw_outside_mcot_r15 and
         c.aul_start_partial_bw_inside_mcot_r15 == other.c.aul_start_partial_bw_inside_mcot_r15 and
         c.aul_start_partial_bw_outside_mcot_r15 == other.c.aul_start_partial_bw_outside_mcot_r15 and
         c.aul_retx_timer_r15 == other.c.aul_retx_timer_r15 and
         c.ending_symbol_aul_r15 == other.c.ending_symbol_aul_r15 and
         c.sf_offset_cot_sharing_r15 == other.c.sf_offset_cot_sharing_r15 and
         c.contention_win_size_timer_r15 == other.c.contention_win_size_timer_r15;
}

const char* aul_cfg_r15_c::setup_s_::tx_mode_ul_aul_r15_opts::to_string() const
{
  static const char* options[] = {"tm1", "tm2"};
  return convert_enum_idx(options, 2, value, "aul_cfg_r15_c::setup_s_::tx_mode_ul_aul_r15_e_");
}
uint8_t aul_cfg_r15_c::setup_s_::tx_mode_ul_aul_r15_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(options, 2, value, "aul_cfg_r15_c::setup_s_::tx_mode_ul_aul_r15_e_");
}

const char* aul_cfg_r15_c::setup_s_::aul_start_partial_bw_inside_mcot_r15_opts::to_string() const
{
  static const char* options[] = {"o34", "o43", "o52", "o61", "oOS1"};
  return convert_enum_idx(options, 5, value, "aul_cfg_r15_c::setup_s_::aul_start_partial_bw_inside_mcot_r15_e_");
}
uint8_t aul_cfg_r15_c::setup_s_::aul_start_partial_bw_inside_mcot_r15_opts::to_number() const
{
  static const uint8_t options[] = {34, 43, 52, 61, 1};
  return map_enum_number(options, 5, value, "aul_cfg_r15_c::setup_s_::aul_start_partial_bw_inside_mcot_r15_e_");
}

const char* aul_cfg_r15_c::setup_s_::aul_start_partial_bw_outside_mcot_r15_opts::to_string() const
{
  static const char* options[] = {"o16", "o25", "o34", "o43", "o52", "o61", "oOS1"};
  return convert_enum_idx(options, 7, value, "aul_cfg_r15_c::setup_s_::aul_start_partial_bw_outside_mcot_r15_e_");
}
uint8_t aul_cfg_r15_c::setup_s_::aul_start_partial_bw_outside_mcot_r15_opts::to_number() const
{
  static const uint8_t options[] = {16, 25, 34, 43, 52, 61, 1};
  return map_enum_number(options, 7, value, "aul_cfg_r15_c::setup_s_::aul_start_partial_bw_outside_mcot_r15_e_");
}

const char* aul_cfg_r15_c::setup_s_::aul_retx_timer_r15_opts::to_string() const
{
  static const char* options[] = {"psf4",
                                  "psf5",
                                  "psf6",
                                  "psf8",
                                  "psf10",
                                  "psf12",
                                  "psf20",
                                  "psf28",
                                  "psf37",
                                  "psf44",
                                  "psf68",
                                  "psf84",
                                  "psf100",
                                  "psf116",
                                  "psf132",
                                  "psf164",
                                  "psf324"};
  return convert_enum_idx(options, 17, value, "aul_cfg_r15_c::setup_s_::aul_retx_timer_r15_e_");
}
uint16_t aul_cfg_r15_c::setup_s_::aul_retx_timer_r15_opts::to_number() const
{
  static const uint16_t options[] = {4, 5, 6, 8, 10, 12, 20, 28, 37, 44, 68, 84, 100, 116, 132, 164, 324};
  return map_enum_number(options, 17, value, "aul_cfg_r15_c::setup_s_::aul_retx_timer_r15_e_");
}

const char* aul_cfg_r15_c::setup_s_::contention_win_size_timer_r15_opts::to_string() const
{
  static const char* options[] = {"n0", "n5", "n10"};
  return convert_enum_idx(options, 3, value, "aul_cfg_r15_c::setup_s_::contention_win_size_timer_r15_e_");
}
uint8_t aul_cfg_r15_c::setup_s_::contention_win_size_timer_r15_opts::to_number() const
{
  static const uint8_t options[] = {0, 5, 10};
  return map_enum_number(options, 3, value, "aul_cfg_r15_c::setup_s_::contention_win_size_timer_r15_e_");
}

// CQI-ReportPeriodicSCell-r15 ::= CHOICE
void cqi_report_periodic_scell_r15_c::set(types::options e)
{
  type_ = e;
}
void cqi_report_periodic_scell_r15_c::set_release()
{
  set(types::release);
}
cqi_report_periodic_scell_r15_c::setup_s_& cqi_report_periodic_scell_r15_c::set_setup()
{
  set(types::setup);
  return c;
}
void cqi_report_periodic_scell_r15_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_int("cqi-pmi-ConfigIndexDormant-r15", c.cqi_pmi_cfg_idx_dormant_r15);
      if (c.ri_cfg_idx_dormant_r15_present) {
        j.write_int("ri-ConfigIndexDormant-r15", c.ri_cfg_idx_dormant_r15);
      }
      if (c.csi_sf_pattern_dormant_r15_present) {
        j.write_fieldname("csi-SubframePatternDormant-r15");
        c.csi_sf_pattern_dormant_r15.to_json(j);
      }
      if (c.cqi_format_ind_dormant_r15_present) {
        j.write_fieldname("cqi-FormatIndicatorDormant-r15");
        c.cqi_format_ind_dormant_r15.to_json(j);
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_periodic_scell_r15_c");
  }
  j.end_obj();
}
SRSASN_CODE cqi_report_periodic_scell_r15_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.ri_cfg_idx_dormant_r15_present, 1));
      HANDLE_CODE(bref.pack(c.csi_sf_pattern_dormant_r15_present, 1));
      HANDLE_CODE(bref.pack(c.cqi_format_ind_dormant_r15_present, 1));
      HANDLE_CODE(pack_integer(bref, c.cqi_pmi_cfg_idx_dormant_r15, (uint16_t)0u, (uint16_t)1023u));
      if (c.ri_cfg_idx_dormant_r15_present) {
        HANDLE_CODE(pack_integer(bref, c.ri_cfg_idx_dormant_r15, (uint16_t)0u, (uint16_t)1023u));
      }
      if (c.csi_sf_pattern_dormant_r15_present) {
        HANDLE_CODE(c.csi_sf_pattern_dormant_r15.pack(bref));
      }
      if (c.cqi_format_ind_dormant_r15_present) {
        HANDLE_CODE(c.cqi_format_ind_dormant_r15.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_periodic_scell_r15_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cqi_report_periodic_scell_r15_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.ri_cfg_idx_dormant_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.csi_sf_pattern_dormant_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.cqi_format_ind_dormant_r15_present, 1));
      HANDLE_CODE(unpack_integer(c.cqi_pmi_cfg_idx_dormant_r15, bref, (uint16_t)0u, (uint16_t)1023u));
      if (c.ri_cfg_idx_dormant_r15_present) {
        HANDLE_CODE(unpack_integer(c.ri_cfg_idx_dormant_r15, bref, (uint16_t)0u, (uint16_t)1023u));
      }
      if (c.csi_sf_pattern_dormant_r15_present) {
        HANDLE_CODE(c.csi_sf_pattern_dormant_r15.unpack(bref));
      }
      if (c.cqi_format_ind_dormant_r15_present) {
        HANDLE_CODE(c.cqi_format_ind_dormant_r15.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_periodic_scell_r15_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool cqi_report_periodic_scell_r15_c::operator==(const cqi_report_periodic_scell_r15_c& other) const
{
  return type() == other.type() and c.cqi_pmi_cfg_idx_dormant_r15 == other.c.cqi_pmi_cfg_idx_dormant_r15 and
         c.ri_cfg_idx_dormant_r15_present == other.c.ri_cfg_idx_dormant_r15_present and
         (not c.ri_cfg_idx_dormant_r15_present or c.ri_cfg_idx_dormant_r15 == other.c.ri_cfg_idx_dormant_r15) and
         c.csi_sf_pattern_dormant_r15_present == other.c.csi_sf_pattern_dormant_r15_present and
         (not c.csi_sf_pattern_dormant_r15_present or
          c.csi_sf_pattern_dormant_r15 == other.c.csi_sf_pattern_dormant_r15) and
         c.cqi_format_ind_dormant_r15_present == other.c.cqi_format_ind_dormant_r15_present and
         (not c.cqi_format_ind_dormant_r15_present or
          c.cqi_format_ind_dormant_r15 == other.c.cqi_format_ind_dormant_r15);
}

void cqi_report_periodic_scell_r15_c::setup_s_::csi_sf_pattern_dormant_r15_c_::set(types::options e)
{
  type_ = e;
}
void cqi_report_periodic_scell_r15_c::setup_s_::csi_sf_pattern_dormant_r15_c_::set_release()
{
  set(types::release);
}
cqi_report_periodic_scell_r15_c::setup_s_::csi_sf_pattern_dormant_r15_c_::setup_s_&
cqi_report_periodic_scell_r15_c::setup_s_::csi_sf_pattern_dormant_r15_c_::set_setup()
{
  set(types::setup);
  return c;
}
void cqi_report_periodic_scell_r15_c::setup_s_::csi_sf_pattern_dormant_r15_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_fieldname("csi-MeasSubframeSet1-r15");
      c.csi_meas_sf_set1_r15.to_json(j);
      j.write_fieldname("csi-MeasSubframeSet2-r15");
      c.csi_meas_sf_set2_r15.to_json(j);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_periodic_scell_r15_c::setup_s_::csi_sf_pattern_dormant_r15_c_");
  }
  j.end_obj();
}
SRSASN_CODE cqi_report_periodic_scell_r15_c::setup_s_::csi_sf_pattern_dormant_r15_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.csi_meas_sf_set1_r15.pack(bref));
      HANDLE_CODE(c.csi_meas_sf_set2_r15.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_periodic_scell_r15_c::setup_s_::csi_sf_pattern_dormant_r15_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cqi_report_periodic_scell_r15_c::setup_s_::csi_sf_pattern_dormant_r15_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.csi_meas_sf_set1_r15.unpack(bref));
      HANDLE_CODE(c.csi_meas_sf_set2_r15.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_periodic_scell_r15_c::setup_s_::csi_sf_pattern_dormant_r15_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool cqi_report_periodic_scell_r15_c::setup_s_::csi_sf_pattern_dormant_r15_c_::operator==(
    const csi_sf_pattern_dormant_r15_c_& other) const
{
  return type() == other.type() and c.csi_meas_sf_set1_r15 == other.c.csi_meas_sf_set1_r15 and
         c.csi_meas_sf_set2_r15 == other.c.csi_meas_sf_set2_r15;
}

void cqi_report_periodic_scell_r15_c::setup_s_::cqi_format_ind_dormant_r15_c_::destroy_()
{
  switch (type_) {
    case types::wideband_cqi_r15:
      c.destroy<wideband_cqi_r15_s_>();
      break;
    case types::subband_cqi_r15:
      c.destroy<subband_cqi_r15_s_>();
      break;
    default:
      break;
  }
}
void cqi_report_periodic_scell_r15_c::setup_s_::cqi_format_ind_dormant_r15_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::wideband_cqi_r15:
      c.init<wideband_cqi_r15_s_>();
      break;
    case types::subband_cqi_r15:
      c.init<subband_cqi_r15_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_periodic_scell_r15_c::setup_s_::cqi_format_ind_dormant_r15_c_");
  }
}
cqi_report_periodic_scell_r15_c::setup_s_::cqi_format_ind_dormant_r15_c_::cqi_format_ind_dormant_r15_c_(
    const cqi_report_periodic_scell_r15_c::setup_s_::cqi_format_ind_dormant_r15_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::wideband_cqi_r15:
      c.init(other.c.get<wideband_cqi_r15_s_>());
      break;
    case types::subband_cqi_r15:
      c.init(other.c.get<subband_cqi_r15_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_periodic_scell_r15_c::setup_s_::cqi_format_ind_dormant_r15_c_");
  }
}
cqi_report_periodic_scell_r15_c::setup_s_::cqi_format_ind_dormant_r15_c_&
cqi_report_periodic_scell_r15_c::setup_s_::cqi_format_ind_dormant_r15_c_::operator=(
    const cqi_report_periodic_scell_r15_c::setup_s_::cqi_format_ind_dormant_r15_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::wideband_cqi_r15:
      c.set(other.c.get<wideband_cqi_r15_s_>());
      break;
    case types::subband_cqi_r15:
      c.set(other.c.get<subband_cqi_r15_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_periodic_scell_r15_c::setup_s_::cqi_format_ind_dormant_r15_c_");
  }

  return *this;
}
cqi_report_periodic_scell_r15_c::setup_s_::cqi_format_ind_dormant_r15_c_::wideband_cqi_r15_s_&
cqi_report_periodic_scell_r15_c::setup_s_::cqi_format_ind_dormant_r15_c_::set_wideband_cqi_r15()
{
  set(types::wideband_cqi_r15);
  return c.get<wideband_cqi_r15_s_>();
}
cqi_report_periodic_scell_r15_c::setup_s_::cqi_format_ind_dormant_r15_c_::subband_cqi_r15_s_&
cqi_report_periodic_scell_r15_c::setup_s_::cqi_format_ind_dormant_r15_c_::set_subband_cqi_r15()
{
  set(types::subband_cqi_r15);
  return c.get<subband_cqi_r15_s_>();
}
void cqi_report_periodic_scell_r15_c::setup_s_::cqi_format_ind_dormant_r15_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::wideband_cqi_r15:
      j.write_fieldname("widebandCQI-r15");
      j.start_obj();
      if (c.get<wideband_cqi_r15_s_>().csi_report_mode_r15_present) {
        j.write_str("csi-ReportMode-r15", c.get<wideband_cqi_r15_s_>().csi_report_mode_r15.to_string());
      }
      j.end_obj();
      break;
    case types::subband_cqi_r15:
      j.write_fieldname("subbandCQI-r15");
      j.start_obj();
      j.write_int("k-r15", c.get<subband_cqi_r15_s_>().k_r15);
      j.write_str("periodicityFactor-r15", c.get<subband_cqi_r15_s_>().periodicity_factor_r15.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_periodic_scell_r15_c::setup_s_::cqi_format_ind_dormant_r15_c_");
  }
  j.end_obj();
}
SRSASN_CODE cqi_report_periodic_scell_r15_c::setup_s_::cqi_format_ind_dormant_r15_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::wideband_cqi_r15:
      HANDLE_CODE(bref.pack(c.get<wideband_cqi_r15_s_>().csi_report_mode_r15_present, 1));
      if (c.get<wideband_cqi_r15_s_>().csi_report_mode_r15_present) {
        HANDLE_CODE(c.get<wideband_cqi_r15_s_>().csi_report_mode_r15.pack(bref));
      }
      break;
    case types::subband_cqi_r15:
      HANDLE_CODE(pack_integer(bref, c.get<subband_cqi_r15_s_>().k_r15, (uint8_t)1u, (uint8_t)4u));
      HANDLE_CODE(c.get<subband_cqi_r15_s_>().periodicity_factor_r15.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_periodic_scell_r15_c::setup_s_::cqi_format_ind_dormant_r15_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cqi_report_periodic_scell_r15_c::setup_s_::cqi_format_ind_dormant_r15_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::wideband_cqi_r15:
      HANDLE_CODE(bref.unpack(c.get<wideband_cqi_r15_s_>().csi_report_mode_r15_present, 1));
      if (c.get<wideband_cqi_r15_s_>().csi_report_mode_r15_present) {
        HANDLE_CODE(c.get<wideband_cqi_r15_s_>().csi_report_mode_r15.unpack(bref));
      }
      break;
    case types::subband_cqi_r15:
      HANDLE_CODE(unpack_integer(c.get<subband_cqi_r15_s_>().k_r15, bref, (uint8_t)1u, (uint8_t)4u));
      HANDLE_CODE(c.get<subband_cqi_r15_s_>().periodicity_factor_r15.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "cqi_report_periodic_scell_r15_c::setup_s_::cqi_format_ind_dormant_r15_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool cqi_report_periodic_scell_r15_c::setup_s_::cqi_format_ind_dormant_r15_c_::operator==(
    const cqi_format_ind_dormant_r15_c_& other) const
{
  if (type_ != other.type_) {
    return false;
  }
  switch (type_) {
    case types::wideband_cqi_r15:
      return c.get<wideband_cqi_r15_s_>().csi_report_mode_r15_present ==
                 other.c.get<wideband_cqi_r15_s_>().csi_report_mode_r15_present and
             (not c.get<wideband_cqi_r15_s_>().csi_report_mode_r15_present or
              c.get<wideband_cqi_r15_s_>().csi_report_mode_r15 ==
                  other.c.get<wideband_cqi_r15_s_>().csi_report_mode_r15);
    case types::subband_cqi_r15:
      return c.get<subband_cqi_r15_s_>().k_r15 == other.c.get<subband_cqi_r15_s_>().k_r15 and
             c.get<subband_cqi_r15_s_>().periodicity_factor_r15 ==
                 other.c.get<subband_cqi_r15_s_>().periodicity_factor_r15;
    default:
      return true;
  }
  return true;
}

const char* cqi_report_periodic_scell_r15_c::setup_s_::cqi_format_ind_dormant_r15_c_::wideband_cqi_r15_s_::
    csi_report_mode_r15_opts::to_string() const
{
  static const char* options[] = {"submode1", "submode2"};
  return convert_enum_idx(options,
                          2,
                          value,
                          "cqi_report_periodic_scell_r15_c::setup_s_::cqi_format_ind_dormant_r15_c_::wideband_cqi_r15_"
                          "s_::csi_report_mode_r15_e_");
}
uint8_t cqi_report_periodic_scell_r15_c::setup_s_::cqi_format_ind_dormant_r15_c_::wideband_cqi_r15_s_::
    csi_report_mode_r15_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(options,
                         2,
                         value,
                         "cqi_report_periodic_scell_r15_c::setup_s_::cqi_format_ind_dormant_r15_c_::wideband_cqi_r15_s_"
                         "::csi_report_mode_r15_e_");
}

const char* cqi_report_periodic_scell_r15_c::setup_s_::cqi_format_ind_dormant_r15_c_::subband_cqi_r15_s_::
    periodicity_factor_r15_opts::to_string() const
{
  static const char* options[] = {"n2", "n4"};
  return convert_enum_idx(options,
                          2,
                          value,
                          "cqi_report_periodic_scell_r15_c::setup_s_::cqi_format_ind_dormant_r15_c_::subband_cqi_r15_s_"
                          "::periodicity_factor_r15_e_");
}
uint8_t cqi_report_periodic_scell_r15_c::setup_s_::cqi_format_ind_dormant_r15_c_::subband_cqi_r15_s_::
    periodicity_factor_r15_opts::to_number() const
{
  static const uint8_t options[] = {2, 4};
  return map_enum_number(options,
                         2,
                         value,
                         "cqi_report_periodic_scell_r15_c::setup_s_::cqi_format_ind_dormant_r15_c_::subband_cqi_r15_s_:"
                         ":periodicity_factor_r15_e_");
}

// CrossCarrierSchedulingConfigLAA-UL-r14 ::= SEQUENCE
SRSASN_CODE cross_carrier_sched_cfg_laa_ul_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, sched_cell_id_r14, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(pack_integer(bref, cif_in_sched_cell_r14, (uint8_t)1u, (uint8_t)7u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cross_carrier_sched_cfg_laa_ul_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(sched_cell_id_r14, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(unpack_integer(cif_in_sched_cell_r14, bref, (uint8_t)1u, (uint8_t)7u));

  return SRSASN_SUCCESS;
}
void cross_carrier_sched_cfg_laa_ul_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("schedulingCellId-r14", sched_cell_id_r14);
  j.write_int("cif-InSchedulingCell-r14", cif_in_sched_cell_r14);
  j.end_obj();
}
bool cross_carrier_sched_cfg_laa_ul_r14_s::operator==(const cross_carrier_sched_cfg_laa_ul_r14_s& other) const
{
  return sched_cell_id_r14 == other.sched_cell_id_r14 and cif_in_sched_cell_r14 == other.cif_in_sched_cell_r14;
}

// LBT-Config-r14 ::= CHOICE
void lbt_cfg_r14_c::destroy_() {}
void lbt_cfg_r14_c::set(types::options e)
{
  destroy_();
  type_ = e;
}
lbt_cfg_r14_c::lbt_cfg_r14_c(const lbt_cfg_r14_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::max_energy_detection_thres_r14:
      c.init(other.c.get<int8_t>());
      break;
    case types::energy_detection_thres_offset_r14:
      c.init(other.c.get<int8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "lbt_cfg_r14_c");
  }
}
lbt_cfg_r14_c& lbt_cfg_r14_c::operator=(const lbt_cfg_r14_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::max_energy_detection_thres_r14:
      c.set(other.c.get<int8_t>());
      break;
    case types::energy_detection_thres_offset_r14:
      c.set(other.c.get<int8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "lbt_cfg_r14_c");
  }

  return *this;
}
int8_t& lbt_cfg_r14_c::set_max_energy_detection_thres_r14()
{
  set(types::max_energy_detection_thres_r14);
  return c.get<int8_t>();
}
int8_t& lbt_cfg_r14_c::set_energy_detection_thres_offset_r14()
{
  set(types::energy_detection_thres_offset_r14);
  return c.get<int8_t>();
}
void lbt_cfg_r14_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::max_energy_detection_thres_r14:
      j.write_int("maxEnergyDetectionThreshold-r14", c.get<int8_t>());
      break;
    case types::energy_detection_thres_offset_r14:
      j.write_int("energyDetectionThresholdOffset-r14", c.get<int8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "lbt_cfg_r14_c");
  }
  j.end_obj();
}
SRSASN_CODE lbt_cfg_r14_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::max_energy_detection_thres_r14:
      HANDLE_CODE(pack_integer(bref, c.get<int8_t>(), (int8_t)-85, (int8_t)-52));
      break;
    case types::energy_detection_thres_offset_r14:
      HANDLE_CODE(pack_integer(bref, c.get<int8_t>(), (int8_t)-13, (int8_t)20));
      break;
    default:
      log_invalid_choice_id(type_, "lbt_cfg_r14_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE lbt_cfg_r14_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::max_energy_detection_thres_r14:
      HANDLE_CODE(unpack_integer(c.get<int8_t>(), bref, (int8_t)-85, (int8_t)-52));
      break;
    case types::energy_detection_thres_offset_r14:
      HANDLE_CODE(unpack_integer(c.get<int8_t>(), bref, (int8_t)-13, (int8_t)20));
      break;
    default:
      log_invalid_choice_id(type_, "lbt_cfg_r14_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool lbt_cfg_r14_c::operator==(const lbt_cfg_r14_c& other) const
{
  if (type_ != other.type_) {
    return false;
  }
  switch (type_) {
    case types::max_energy_detection_thres_r14:
      return c.get<int8_t>() == other.c.get<int8_t>();
    case types::energy_detection_thres_offset_r14:
      return c.get<int8_t>() == other.c.get<int8_t>();
    default:
      return true;
  }
  return true;
}

// PDCCH-ConfigLAA-r14 ::= SEQUENCE
SRSASN_CODE pdcch_cfg_laa_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(max_nof_sched_sfs_format0_b_r14_present, 1));
  HANDLE_CODE(bref.pack(max_nof_sched_sfs_format4_b_r14_present, 1));
  HANDLE_CODE(bref.pack(skip_monitoring_dci_format0_a_r14_present, 1));
  HANDLE_CODE(bref.pack(skip_monitoring_dci_format4_a_r14_present, 1));
  HANDLE_CODE(bref.pack(pdcch_candidate_reductions_format0_a_r14_present, 1));
  HANDLE_CODE(bref.pack(pdcch_candidate_reductions_format4_a_r14_present, 1));
  HANDLE_CODE(bref.pack(pdcch_candidate_reductions_format0_b_r14_present, 1));
  HANDLE_CODE(bref.pack(pdcch_candidate_reductions_format4_b_r14_present, 1));

  if (max_nof_sched_sfs_format0_b_r14_present) {
    HANDLE_CODE(max_nof_sched_sfs_format0_b_r14.pack(bref));
  }
  if (max_nof_sched_sfs_format4_b_r14_present) {
    HANDLE_CODE(max_nof_sched_sfs_format4_b_r14.pack(bref));
  }
  if (pdcch_candidate_reductions_format0_a_r14_present) {
    HANDLE_CODE(pdcch_candidate_reductions_format0_a_r14.pack(bref));
  }
  if (pdcch_candidate_reductions_format4_a_r14_present) {
    HANDLE_CODE(pdcch_candidate_reductions_format4_a_r14.pack(bref));
  }
  if (pdcch_candidate_reductions_format0_b_r14_present) {
    HANDLE_CODE(pdcch_candidate_reductions_format0_b_r14.pack(bref));
  }
  if (pdcch_candidate_reductions_format4_b_r14_present) {
    HANDLE_CODE(pdcch_candidate_reductions_format4_b_r14.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdcch_cfg_laa_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(max_nof_sched_sfs_format0_b_r14_present, 1));
  HANDLE_CODE(bref.unpack(max_nof_sched_sfs_format4_b_r14_present, 1));
  HANDLE_CODE(bref.unpack(skip_monitoring_dci_format0_a_r14_present, 1));
  HANDLE_CODE(bref.unpack(skip_monitoring_dci_format4_a_r14_present, 1));
  HANDLE_CODE(bref.unpack(pdcch_candidate_reductions_format0_a_r14_present, 1));
  HANDLE_CODE(bref.unpack(pdcch_candidate_reductions_format4_a_r14_present, 1));
  HANDLE_CODE(bref.unpack(pdcch_candidate_reductions_format0_b_r14_present, 1));
  HANDLE_CODE(bref.unpack(pdcch_candidate_reductions_format4_b_r14_present, 1));

  if (max_nof_sched_sfs_format0_b_r14_present) {
    HANDLE_CODE(max_nof_sched_sfs_format0_b_r14.unpack(bref));
  }
  if (max_nof_sched_sfs_format4_b_r14_present) {
    HANDLE_CODE(max_nof_sched_sfs_format4_b_r14.unpack(bref));
  }
  if (pdcch_candidate_reductions_format0_a_r14_present) {
    HANDLE_CODE(pdcch_candidate_reductions_format0_a_r14.unpack(bref));
  }
  if (pdcch_candidate_reductions_format4_a_r14_present) {
    HANDLE_CODE(pdcch_candidate_reductions_format4_a_r14.unpack(bref));
  }
  if (pdcch_candidate_reductions_format0_b_r14_present) {
    HANDLE_CODE(pdcch_candidate_reductions_format0_b_r14.unpack(bref));
  }
  if (pdcch_candidate_reductions_format4_b_r14_present) {
    HANDLE_CODE(pdcch_candidate_reductions_format4_b_r14.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pdcch_cfg_laa_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (max_nof_sched_sfs_format0_b_r14_present) {
    j.write_str("maxNumberOfSchedSubframes-Format0B-r14", max_nof_sched_sfs_format0_b_r14.to_string());
  }
  if (max_nof_sched_sfs_format4_b_r14_present) {
    j.write_str("maxNumberOfSchedSubframes-Format4B-r14", max_nof_sched_sfs_format4_b_r14.to_string());
  }
  if (skip_monitoring_dci_format0_a_r14_present) {
    j.write_str("skipMonitoringDCI-Format0A-r14", "true");
  }
  if (skip_monitoring_dci_format4_a_r14_present) {
    j.write_str("skipMonitoringDCI-Format4A-r14", "true");
  }
  if (pdcch_candidate_reductions_format0_a_r14_present) {
    j.write_fieldname("pdcch-CandidateReductions-Format0A-r14");
    pdcch_candidate_reductions_format0_a_r14.to_json(j);
  }
  if (pdcch_candidate_reductions_format4_a_r14_present) {
    j.write_fieldname("pdcch-CandidateReductions-Format4A-r14");
    pdcch_candidate_reductions_format4_a_r14.to_json(j);
  }
  if (pdcch_candidate_reductions_format0_b_r14_present) {
    j.write_fieldname("pdcch-CandidateReductions-Format0B-r14");
    pdcch_candidate_reductions_format0_b_r14.to_json(j);
  }
  if (pdcch_candidate_reductions_format4_b_r14_present) {
    j.write_fieldname("pdcch-CandidateReductions-Format4B-r14");
    pdcch_candidate_reductions_format4_b_r14.to_json(j);
  }
  j.end_obj();
}
bool pdcch_cfg_laa_r14_s::operator==(const pdcch_cfg_laa_r14_s& other) const
{
  return max_nof_sched_sfs_format0_b_r14_present == other.max_nof_sched_sfs_format0_b_r14_present and
         (not max_nof_sched_sfs_format0_b_r14_present or
          max_nof_sched_sfs_format0_b_r14 == other.max_nof_sched_sfs_format0_b_r14) and
         max_nof_sched_sfs_format4_b_r14_present == other.max_nof_sched_sfs_format4_b_r14_present and
         (not max_nof_sched_sfs_format4_b_r14_present or
          max_nof_sched_sfs_format4_b_r14 == other.max_nof_sched_sfs_format4_b_r14) and
         skip_monitoring_dci_format0_a_r14_present == other.skip_monitoring_dci_format0_a_r14_present and
         skip_monitoring_dci_format4_a_r14_present == other.skip_monitoring_dci_format4_a_r14_present and
         pdcch_candidate_reductions_format0_a_r14_present == other.pdcch_candidate_reductions_format0_a_r14_present and
         (not pdcch_candidate_reductions_format0_a_r14_present or
          pdcch_candidate_reductions_format0_a_r14 == other.pdcch_candidate_reductions_format0_a_r14) and
         pdcch_candidate_reductions_format4_a_r14_present == other.pdcch_candidate_reductions_format4_a_r14_present and
         (not pdcch_candidate_reductions_format4_a_r14_present or
          pdcch_candidate_reductions_format4_a_r14 == other.pdcch_candidate_reductions_format4_a_r14) and
         pdcch_candidate_reductions_format0_b_r14_present == other.pdcch_candidate_reductions_format0_b_r14_present and
         (not pdcch_candidate_reductions_format0_b_r14_present or
          pdcch_candidate_reductions_format0_b_r14 == other.pdcch_candidate_reductions_format0_b_r14) and
         pdcch_candidate_reductions_format4_b_r14_present == other.pdcch_candidate_reductions_format4_b_r14_present and
         (not pdcch_candidate_reductions_format4_b_r14_present or
          pdcch_candidate_reductions_format4_b_r14 == other.pdcch_candidate_reductions_format4_b_r14);
}

const char* pdcch_cfg_laa_r14_s::max_nof_sched_sfs_format0_b_r14_opts::to_string() const
{
  static const char* options[] = {"sf2", "sf3", "sf4"};
  return convert_enum_idx(options, 3, value, "pdcch_cfg_laa_r14_s::max_nof_sched_sfs_format0_b_r14_e_");
}
uint8_t pdcch_cfg_laa_r14_s::max_nof_sched_sfs_format0_b_r14_opts::to_number() const
{
  static const uint8_t options[] = {2, 3, 4};
  return map_enum_number(options, 3, value, "pdcch_cfg_laa_r14_s::max_nof_sched_sfs_format0_b_r14_e_");
}

const char* pdcch_cfg_laa_r14_s::max_nof_sched_sfs_format4_b_r14_opts::to_string() const
{
  static const char* options[] = {"sf2", "sf3", "sf4"};
  return convert_enum_idx(options, 3, value, "pdcch_cfg_laa_r14_s::max_nof_sched_sfs_format4_b_r14_e_");
}
uint8_t pdcch_cfg_laa_r14_s::max_nof_sched_sfs_format4_b_r14_opts::to_number() const
{
  static const uint8_t options[] = {2, 3, 4};
  return map_enum_number(options, 3, value, "pdcch_cfg_laa_r14_s::max_nof_sched_sfs_format4_b_r14_e_");
}

// PUSCH-ModeConfigLAA-r15 ::= SEQUENCE
SRSASN_CODE pusch_mode_cfg_laa_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(laa_pusch_mode1, 1));
  HANDLE_CODE(bref.pack(laa_pusch_mode2, 1));
  HANDLE_CODE(bref.pack(laa_pusch_mode3, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pusch_mode_cfg_laa_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(laa_pusch_mode1, 1));
  HANDLE_CODE(bref.unpack(laa_pusch_mode2, 1));
  HANDLE_CODE(bref.unpack(laa_pusch_mode3, 1));

  return SRSASN_SUCCESS;
}
void pusch_mode_cfg_laa_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_bool("laa-PUSCH-Mode1", laa_pusch_mode1);
  j.write_bool("laa-PUSCH-Mode2", laa_pusch_mode2);
  j.write_bool("laa-PUSCH-Mode3", laa_pusch_mode3);
  j.end_obj();
}
bool pusch_mode_cfg_laa_r15_s::operator==(const pusch_mode_cfg_laa_r15_s& other) const
{
  return laa_pusch_mode1 == other.laa_pusch_mode1 and laa_pusch_mode2 == other.laa_pusch_mode2 and
         laa_pusch_mode3 == other.laa_pusch_mode3;
}

// SoundingRS-UL-ConfigDedicatedAperiodic-v1430 ::= CHOICE
void srs_ul_cfg_ded_aperiodic_v1430_c::set(types::options e)
{
  type_ = e;
}
void srs_ul_cfg_ded_aperiodic_v1430_c::set_release()
{
  set(types::release);
}
srs_ul_cfg_ded_aperiodic_v1430_c::setup_s_& srs_ul_cfg_ded_aperiodic_v1430_c::set_setup()
{
  set(types::setup);
  return c;
}
void srs_ul_cfg_ded_aperiodic_v1430_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      if (c.srs_sf_ind_r14_present) {
        j.write_int("srs-SubframeIndication-r14", c.srs_sf_ind_r14);
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "srs_ul_cfg_ded_aperiodic_v1430_c");
  }
  j.end_obj();
}
SRSASN_CODE srs_ul_cfg_ded_aperiodic_v1430_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.srs_sf_ind_r14_present, 1));
      if (c.srs_sf_ind_r14_present) {
        HANDLE_CODE(pack_integer(bref, c.srs_sf_ind_r14, (uint8_t)1u, (uint8_t)4u));
      }
      break;
    default:
      log_invalid_choice_id(type_, "srs_ul_cfg_ded_aperiodic_v1430_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_ul_cfg_ded_aperiodic_v1430_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.srs_sf_ind_r14_present, 1));
      if (c.srs_sf_ind_r14_present) {
        HANDLE_CODE(unpack_integer(c.srs_sf_ind_r14, bref, (uint8_t)1u, (uint8_t)4u));
      }
      break;
    default:
      log_invalid_choice_id(type_, "srs_ul_cfg_ded_aperiodic_v1430_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool srs_ul_cfg_ded_aperiodic_v1430_c::operator==(const srs_ul_cfg_ded_aperiodic_v1430_c& other) const
{
  return type() == other.type() and c.srs_sf_ind_r14_present == other.c.srs_sf_ind_r14_present and
         (not c.srs_sf_ind_r14_present or c.srs_sf_ind_r14 == other.c.srs_sf_ind_r14);
}

// CQI-ReportConfigSCell-r10 ::= SEQUENCE
SRSASN_CODE cqi_report_cfg_scell_r10_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cqi_report_mode_aperiodic_r10_present, 1));
  HANDLE_CODE(bref.pack(cqi_report_periodic_scell_r10_present, 1));
  HANDLE_CODE(bref.pack(pmi_ri_report_r10_present, 1));

  if (cqi_report_mode_aperiodic_r10_present) {
    HANDLE_CODE(cqi_report_mode_aperiodic_r10.pack(bref));
  }
  HANDLE_CODE(pack_integer(bref, nom_pdsch_rs_epre_offset_r10, (int8_t)-1, (int8_t)6));
  if (cqi_report_periodic_scell_r10_present) {
    HANDLE_CODE(cqi_report_periodic_scell_r10.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cqi_report_cfg_scell_r10_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cqi_report_mode_aperiodic_r10_present, 1));
  HANDLE_CODE(bref.unpack(cqi_report_periodic_scell_r10_present, 1));
  HANDLE_CODE(bref.unpack(pmi_ri_report_r10_present, 1));

  if (cqi_report_mode_aperiodic_r10_present) {
    HANDLE_CODE(cqi_report_mode_aperiodic_r10.unpack(bref));
  }
  HANDLE_CODE(unpack_integer(nom_pdsch_rs_epre_offset_r10, bref, (int8_t)-1, (int8_t)6));
  if (cqi_report_periodic_scell_r10_present) {
    HANDLE_CODE(cqi_report_periodic_scell_r10.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cqi_report_cfg_scell_r10_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (cqi_report_mode_aperiodic_r10_present) {
    j.write_str("cqi-ReportModeAperiodic-r10", cqi_report_mode_aperiodic_r10.to_string());
  }
  j.write_int("nomPDSCH-RS-EPRE-Offset-r10", nom_pdsch_rs_epre_offset_r10);
  if (cqi_report_periodic_scell_r10_present) {
    j.write_fieldname("cqi-ReportPeriodicSCell-r10");
    cqi_report_periodic_scell_r10.to_json(j);
  }
  if (pmi_ri_report_r10_present) {
    j.write_str("pmi-RI-Report-r10", "setup");
  }
  j.end_obj();
}
bool cqi_report_cfg_scell_r10_s::operator==(const cqi_report_cfg_scell_r10_s& other) const
{
  return cqi_report_mode_aperiodic_r10_present == other.cqi_report_mode_aperiodic_r10_present and
         (not cqi_report_mode_aperiodic_r10_present or
          cqi_report_mode_aperiodic_r10 == other.cqi_report_mode_aperiodic_r10) and
         nom_pdsch_rs_epre_offset_r10 == other.nom_pdsch_rs_epre_offset_r10 and
         cqi_report_periodic_scell_r10_present == other.cqi_report_periodic_scell_r10_present and
         (not cqi_report_periodic_scell_r10_present or
          cqi_report_periodic_scell_r10 == other.cqi_report_periodic_scell_r10) and
         pmi_ri_report_r10_present == other.pmi_ri_report_r10_present;
}

// CQI-ReportConfigSCell-r15 ::= SEQUENCE
SRSASN_CODE cqi_report_cfg_scell_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cqi_report_periodic_scell_r15_present, 1));
  HANDLE_CODE(bref.pack(alt_cqi_table_minus1024_qam_r15_present, 1));

  if (cqi_report_periodic_scell_r15_present) {
    HANDLE_CODE(cqi_report_periodic_scell_r15.pack(bref));
  }
  if (alt_cqi_table_minus1024_qam_r15_present) {
    HANDLE_CODE(alt_cqi_table_minus1024_qam_r15.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cqi_report_cfg_scell_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cqi_report_periodic_scell_r15_present, 1));
  HANDLE_CODE(bref.unpack(alt_cqi_table_minus1024_qam_r15_present, 1));

  if (cqi_report_periodic_scell_r15_present) {
    HANDLE_CODE(cqi_report_periodic_scell_r15.unpack(bref));
  }
  if (alt_cqi_table_minus1024_qam_r15_present) {
    HANDLE_CODE(alt_cqi_table_minus1024_qam_r15.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cqi_report_cfg_scell_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (cqi_report_periodic_scell_r15_present) {
    j.write_fieldname("cqi-ReportPeriodicSCell-r15");
    cqi_report_periodic_scell_r15.to_json(j);
  }
  if (alt_cqi_table_minus1024_qam_r15_present) {
    j.write_str("altCQI-Table-1024QAM-r15", alt_cqi_table_minus1024_qam_r15.to_string());
  }
  j.end_obj();
}
bool cqi_report_cfg_scell_r15_s::operator==(const cqi_report_cfg_scell_r15_s& other) const
{
  return cqi_report_periodic_scell_r15_present == other.cqi_report_periodic_scell_r15_present and
         (not cqi_report_periodic_scell_r15_present or
          cqi_report_periodic_scell_r15 == other.cqi_report_periodic_scell_r15) and
         alt_cqi_table_minus1024_qam_r15_present == other.alt_cqi_table_minus1024_qam_r15_present and
         (not alt_cqi_table_minus1024_qam_r15_present or
          alt_cqi_table_minus1024_qam_r15 == other.alt_cqi_table_minus1024_qam_r15);
}

const char* cqi_report_cfg_scell_r15_s::alt_cqi_table_minus1024_qam_r15_opts::to_string() const
{
  static const char* options[] = {"allSubframes", "csi-SubframeSet1", "csi-SubframeSet2", "spare1"};
  return convert_enum_idx(options, 4, value, "cqi_report_cfg_scell_r15_s::alt_cqi_table_minus1024_qam_r15_e_");
}
uint8_t cqi_report_cfg_scell_r15_s::alt_cqi_table_minus1024_qam_r15_opts::to_number() const
{
  switch (value) {
    case csi_sf_set1:
      return 1;
    case csi_sf_set2:
      return 2;
    default:
      invalid_enum_number(value, "cqi_report_cfg_scell_r15_s::alt_cqi_table_minus1024_qam_r15_e_");
  }
  return 0;
}

// CQI-ReportPeriodicSCell-v1730 ::= SEQUENCE
SRSASN_CODE cqi_report_periodic_scell_v1730_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ri_cfg_idx2_dormant_r17_present, 1));

  HANDLE_CODE(pack_integer(bref, cqi_pmi_cfg_idx2_dormant_r17, (uint16_t)0u, (uint16_t)1023u));
  if (ri_cfg_idx2_dormant_r17_present) {
    HANDLE_CODE(pack_integer(bref, ri_cfg_idx2_dormant_r17, (uint16_t)0u, (uint16_t)1023u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cqi_report_periodic_scell_v1730_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ri_cfg_idx2_dormant_r17_present, 1));

  HANDLE_CODE(unpack_integer(cqi_pmi_cfg_idx2_dormant_r17, bref, (uint16_t)0u, (uint16_t)1023u));
  if (ri_cfg_idx2_dormant_r17_present) {
    HANDLE_CODE(unpack_integer(ri_cfg_idx2_dormant_r17, bref, (uint16_t)0u, (uint16_t)1023u));
  }

  return SRSASN_SUCCESS;
}
void cqi_report_periodic_scell_v1730_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("cqi-pmi-ConfigIndex2Dormant-r17", cqi_pmi_cfg_idx2_dormant_r17);
  if (ri_cfg_idx2_dormant_r17_present) {
    j.write_int("ri-ConfigIndex2Dormant-r17", ri_cfg_idx2_dormant_r17);
  }
  j.end_obj();
}
bool cqi_report_periodic_scell_v1730_s::operator==(const cqi_report_periodic_scell_v1730_s& other) const
{
  return cqi_pmi_cfg_idx2_dormant_r17 == other.cqi_pmi_cfg_idx2_dormant_r17 and
         ri_cfg_idx2_dormant_r17_present == other.ri_cfg_idx2_dormant_r17_present and
         (not ri_cfg_idx2_dormant_r17_present or ri_cfg_idx2_dormant_r17 == other.ri_cfg_idx2_dormant_r17);
}

// CQI-ShortConfigSCell-r15 ::= CHOICE
void cqi_short_cfg_scell_r15_c::set(types::options e)
{
  type_ = e;
}
void cqi_short_cfg_scell_r15_c::set_release()
{
  set(types::release);
}
cqi_short_cfg_scell_r15_c::setup_s_& cqi_short_cfg_scell_r15_c::set_setup()
{
  set(types::setup);
  return c;
}
void cqi_short_cfg_scell_r15_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_int("cqi-pmi-ConfigIndexShort-r15", c.cqi_pmi_cfg_idx_short_r15);
      if (c.ri_cfg_idx_short_r15_present) {
        j.write_int("ri-ConfigIndexShort-r15", c.ri_cfg_idx_short_r15);
      }
      if (c.cqi_format_ind_short_r15_present) {
        j.write_fieldname("cqi-FormatIndicatorShort-r15");
        c.cqi_format_ind_short_r15.to_json(j);
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "cqi_short_cfg_scell_r15_c");
  }
  j.end_obj();
}
SRSASN_CODE cqi_short_cfg_scell_r15_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.ri_cfg_idx_short_r15_present, 1));
      HANDLE_CODE(bref.pack(c.cqi_format_ind_short_r15_present, 1));
      HANDLE_CODE(pack_integer(bref, c.cqi_pmi_cfg_idx_short_r15, (uint16_t)0u, (uint16_t)1023u));
      if (c.ri_cfg_idx_short_r15_present) {
        HANDLE_CODE(pack_integer(bref, c.ri_cfg_idx_short_r15, (uint16_t)0u, (uint16_t)1023u));
      }
      if (c.cqi_format_ind_short_r15_present) {
        HANDLE_CODE(c.cqi_format_ind_short_r15.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "cqi_short_cfg_scell_r15_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cqi_short_cfg_scell_r15_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.ri_cfg_idx_short_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.cqi_format_ind_short_r15_present, 1));
      HANDLE_CODE(unpack_integer(c.cqi_pmi_cfg_idx_short_r15, bref, (uint16_t)0u, (uint16_t)1023u));
      if (c.ri_cfg_idx_short_r15_present) {
        HANDLE_CODE(unpack_integer(c.ri_cfg_idx_short_r15, bref, (uint16_t)0u, (uint16_t)1023u));
      }
      if (c.cqi_format_ind_short_r15_present) {
        HANDLE_CODE(c.cqi_format_ind_short_r15.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "cqi_short_cfg_scell_r15_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool cqi_short_cfg_scell_r15_c::operator==(const cqi_short_cfg_scell_r15_c& other) const
{
  return type() == other.type() and c.cqi_pmi_cfg_idx_short_r15 == other.c.cqi_pmi_cfg_idx_short_r15 and
         c.ri_cfg_idx_short_r15_present == other.c.ri_cfg_idx_short_r15_present and
         (not c.ri_cfg_idx_short_r15_present or c.ri_cfg_idx_short_r15 == other.c.ri_cfg_idx_short_r15) and
         c.cqi_format_ind_short_r15_present == other.c.cqi_format_ind_short_r15_present and
         (not c.cqi_format_ind_short_r15_present or c.cqi_format_ind_short_r15 == other.c.cqi_format_ind_short_r15);
}

void cqi_short_cfg_scell_r15_c::setup_s_::cqi_format_ind_short_r15_c_::destroy_()
{
  switch (type_) {
    case types::wideband_cqi_short_r15:
      c.destroy<wideband_cqi_short_r15_s_>();
      break;
    case types::subband_cqi_short_r15:
      c.destroy<subband_cqi_short_r15_s_>();
      break;
    default:
      break;
  }
}
void cqi_short_cfg_scell_r15_c::setup_s_::cqi_format_ind_short_r15_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::wideband_cqi_short_r15:
      c.init<wideband_cqi_short_r15_s_>();
      break;
    case types::subband_cqi_short_r15:
      c.init<subband_cqi_short_r15_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cqi_short_cfg_scell_r15_c::setup_s_::cqi_format_ind_short_r15_c_");
  }
}
cqi_short_cfg_scell_r15_c::setup_s_::cqi_format_ind_short_r15_c_::cqi_format_ind_short_r15_c_(
    const cqi_short_cfg_scell_r15_c::setup_s_::cqi_format_ind_short_r15_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::wideband_cqi_short_r15:
      c.init(other.c.get<wideband_cqi_short_r15_s_>());
      break;
    case types::subband_cqi_short_r15:
      c.init(other.c.get<subband_cqi_short_r15_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cqi_short_cfg_scell_r15_c::setup_s_::cqi_format_ind_short_r15_c_");
  }
}
cqi_short_cfg_scell_r15_c::setup_s_::cqi_format_ind_short_r15_c_&
cqi_short_cfg_scell_r15_c::setup_s_::cqi_format_ind_short_r15_c_::operator=(
    const cqi_short_cfg_scell_r15_c::setup_s_::cqi_format_ind_short_r15_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::wideband_cqi_short_r15:
      c.set(other.c.get<wideband_cqi_short_r15_s_>());
      break;
    case types::subband_cqi_short_r15:
      c.set(other.c.get<subband_cqi_short_r15_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cqi_short_cfg_scell_r15_c::setup_s_::cqi_format_ind_short_r15_c_");
  }

  return *this;
}
cqi_short_cfg_scell_r15_c::setup_s_::cqi_format_ind_short_r15_c_::wideband_cqi_short_r15_s_&
cqi_short_cfg_scell_r15_c::setup_s_::cqi_format_ind_short_r15_c_::set_wideband_cqi_short_r15()
{
  set(types::wideband_cqi_short_r15);
  return c.get<wideband_cqi_short_r15_s_>();
}
cqi_short_cfg_scell_r15_c::setup_s_::cqi_format_ind_short_r15_c_::subband_cqi_short_r15_s_&
cqi_short_cfg_scell_r15_c::setup_s_::cqi_format_ind_short_r15_c_::set_subband_cqi_short_r15()
{
  set(types::subband_cqi_short_r15);
  return c.get<subband_cqi_short_r15_s_>();
}
void cqi_short_cfg_scell_r15_c::setup_s_::cqi_format_ind_short_r15_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::wideband_cqi_short_r15:
      j.write_fieldname("widebandCQI-Short-r15");
      j.start_obj();
      if (c.get<wideband_cqi_short_r15_s_>().csi_report_mode_short_r15_present) {
        j.write_str("csi-ReportModeShort-r15",
                    c.get<wideband_cqi_short_r15_s_>().csi_report_mode_short_r15.to_string());
      }
      j.end_obj();
      break;
    case types::subband_cqi_short_r15:
      j.write_fieldname("subbandCQI-Short-r15");
      j.start_obj();
      j.write_int("k-r15", c.get<subband_cqi_short_r15_s_>().k_r15);
      j.write_str("periodicityFactor-r15", c.get<subband_cqi_short_r15_s_>().periodicity_factor_r15.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "cqi_short_cfg_scell_r15_c::setup_s_::cqi_format_ind_short_r15_c_");
  }
  j.end_obj();
}
SRSASN_CODE cqi_short_cfg_scell_r15_c::setup_s_::cqi_format_ind_short_r15_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::wideband_cqi_short_r15:
      HANDLE_CODE(bref.pack(c.get<wideband_cqi_short_r15_s_>().csi_report_mode_short_r15_present, 1));
      if (c.get<wideband_cqi_short_r15_s_>().csi_report_mode_short_r15_present) {
        HANDLE_CODE(c.get<wideband_cqi_short_r15_s_>().csi_report_mode_short_r15.pack(bref));
      }
      break;
    case types::subband_cqi_short_r15:
      HANDLE_CODE(pack_integer(bref, c.get<subband_cqi_short_r15_s_>().k_r15, (uint8_t)1u, (uint8_t)4u));
      HANDLE_CODE(c.get<subband_cqi_short_r15_s_>().periodicity_factor_r15.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "cqi_short_cfg_scell_r15_c::setup_s_::cqi_format_ind_short_r15_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cqi_short_cfg_scell_r15_c::setup_s_::cqi_format_ind_short_r15_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::wideband_cqi_short_r15:
      HANDLE_CODE(bref.unpack(c.get<wideband_cqi_short_r15_s_>().csi_report_mode_short_r15_present, 1));
      if (c.get<wideband_cqi_short_r15_s_>().csi_report_mode_short_r15_present) {
        HANDLE_CODE(c.get<wideband_cqi_short_r15_s_>().csi_report_mode_short_r15.unpack(bref));
      }
      break;
    case types::subband_cqi_short_r15:
      HANDLE_CODE(unpack_integer(c.get<subband_cqi_short_r15_s_>().k_r15, bref, (uint8_t)1u, (uint8_t)4u));
      HANDLE_CODE(c.get<subband_cqi_short_r15_s_>().periodicity_factor_r15.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "cqi_short_cfg_scell_r15_c::setup_s_::cqi_format_ind_short_r15_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool cqi_short_cfg_scell_r15_c::setup_s_::cqi_format_ind_short_r15_c_::operator==(
    const cqi_format_ind_short_r15_c_& other) const
{
  if (type_ != other.type_) {
    return false;
  }
  switch (type_) {
    case types::wideband_cqi_short_r15:
      return c.get<wideband_cqi_short_r15_s_>().csi_report_mode_short_r15_present ==
                 other.c.get<wideband_cqi_short_r15_s_>().csi_report_mode_short_r15_present and
             (not c.get<wideband_cqi_short_r15_s_>().csi_report_mode_short_r15_present or
              c.get<wideband_cqi_short_r15_s_>().csi_report_mode_short_r15 ==
                  other.c.get<wideband_cqi_short_r15_s_>().csi_report_mode_short_r15);
    case types::subband_cqi_short_r15:
      return c.get<subband_cqi_short_r15_s_>().k_r15 == other.c.get<subband_cqi_short_r15_s_>().k_r15 and
             c.get<subband_cqi_short_r15_s_>().periodicity_factor_r15 ==
                 other.c.get<subband_cqi_short_r15_s_>().periodicity_factor_r15;
    default:
      return true;
  }
  return true;
}

const char* cqi_short_cfg_scell_r15_c::setup_s_::cqi_format_ind_short_r15_c_::wideband_cqi_short_r15_s_::
    csi_report_mode_short_r15_opts::to_string() const
{
  static const char* options[] = {"submode1", "submode2"};
  return convert_enum_idx(options,
                          2,
                          value,
                          "cqi_short_cfg_scell_r15_c::setup_s_::cqi_format_ind_short_r15_c_::wideband_cqi_short_r15_s_:"
                          ":csi_report_mode_short_r15_e_");
}
uint8_t cqi_short_cfg_scell_r15_c::setup_s_::cqi_format_ind_short_r15_c_::wideband_cqi_short_r15_s_::
    csi_report_mode_short_r15_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(options,
                         2,
                         value,
                         "cqi_short_cfg_scell_r15_c::setup_s_::cqi_format_ind_short_r15_c_::wideband_cqi_short_r15_s_::"
                         "csi_report_mode_short_r15_e_");
}

const char* cqi_short_cfg_scell_r15_c::setup_s_::cqi_format_ind_short_r15_c_::subband_cqi_short_r15_s_::
    periodicity_factor_r15_opts::to_string() const
{
  static const char* options[] = {"n2", "n4"};
  return convert_enum_idx(options,
                          2,
                          value,
                          "cqi_short_cfg_scell_r15_c::setup_s_::cqi_format_ind_short_r15_c_::subband_cqi_short_r15_s_::"
                          "periodicity_factor_r15_e_");
}
uint8_t cqi_short_cfg_scell_r15_c::setup_s_::cqi_format_ind_short_r15_c_::subband_cqi_short_r15_s_::
    periodicity_factor_r15_opts::to_number() const
{
  static const uint8_t options[] = {2, 4};
  return map_enum_number(options,
                         2,
                         value,
                         "cqi_short_cfg_scell_r15_c::setup_s_::cqi_format_ind_short_r15_c_::subband_cqi_short_r15_s_::"
                         "periodicity_factor_r15_e_");
}

// CrossCarrierSchedulingConfig-r10 ::= SEQUENCE
SRSASN_CODE cross_carrier_sched_cfg_r10_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(sched_cell_info_r10.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cross_carrier_sched_cfg_r10_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(sched_cell_info_r10.unpack(bref));

  return SRSASN_SUCCESS;
}
void cross_carrier_sched_cfg_r10_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("schedulingCellInfo-r10");
  sched_cell_info_r10.to_json(j);
  j.end_obj();
}
bool cross_carrier_sched_cfg_r10_s::operator==(const cross_carrier_sched_cfg_r10_s& other) const
{
  return sched_cell_info_r10 == other.sched_cell_info_r10;
}

void cross_carrier_sched_cfg_r10_s::sched_cell_info_r10_c_::destroy_()
{
  switch (type_) {
    case types::own_r10:
      c.destroy<own_r10_s_>();
      break;
    case types::other_r10:
      c.destroy<other_r10_s_>();
      break;
    default:
      break;
  }
}
void cross_carrier_sched_cfg_r10_s::sched_cell_info_r10_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::own_r10:
      c.init<own_r10_s_>();
      break;
    case types::other_r10:
      c.init<other_r10_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cross_carrier_sched_cfg_r10_s::sched_cell_info_r10_c_");
  }
}
cross_carrier_sched_cfg_r10_s::sched_cell_info_r10_c_::sched_cell_info_r10_c_(
    const cross_carrier_sched_cfg_r10_s::sched_cell_info_r10_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::own_r10:
      c.init(other.c.get<own_r10_s_>());
      break;
    case types::other_r10:
      c.init(other.c.get<other_r10_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cross_carrier_sched_cfg_r10_s::sched_cell_info_r10_c_");
  }
}
cross_carrier_sched_cfg_r10_s::sched_cell_info_r10_c_& cross_carrier_sched_cfg_r10_s::sched_cell_info_r10_c_::operator=(
    const cross_carrier_sched_cfg_r10_s::sched_cell_info_r10_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::own_r10:
      c.set(other.c.get<own_r10_s_>());
      break;
    case types::other_r10:
      c.set(other.c.get<other_r10_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cross_carrier_sched_cfg_r10_s::sched_cell_info_r10_c_");
  }

  return *this;
}
cross_carrier_sched_cfg_r10_s::sched_cell_info_r10_c_::own_r10_s_&
cross_carrier_sched_cfg_r10_s::sched_cell_info_r10_c_::set_own_r10()
{
  set(types::own_r10);
  return c.get<own_r10_s_>();
}
cross_carrier_sched_cfg_r10_s::sched_cell_info_r10_c_::other_r10_s_&
cross_carrier_sched_cfg_r10_s::sched_cell_info_r10_c_::set_other_r10()
{
  set(types::other_r10);
  return c.get<other_r10_s_>();
}
void cross_carrier_sched_cfg_r10_s::sched_cell_info_r10_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::own_r10:
      j.write_fieldname("own-r10");
      j.start_obj();
      j.write_bool("cif-Presence-r10", c.get<own_r10_s_>().cif_presence_r10);
      j.end_obj();
      break;
    case types::other_r10:
      j.write_fieldname("other-r10");
      j.start_obj();
      j.write_int("schedulingCellId-r10", c.get<other_r10_s_>().sched_cell_id_r10);
      j.write_int("pdsch-Start-r10", c.get<other_r10_s_>().pdsch_start_r10);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "cross_carrier_sched_cfg_r10_s::sched_cell_info_r10_c_");
  }
  j.end_obj();
}
SRSASN_CODE cross_carrier_sched_cfg_r10_s::sched_cell_info_r10_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::own_r10:
      HANDLE_CODE(bref.pack(c.get<own_r10_s_>().cif_presence_r10, 1));
      break;
    case types::other_r10:
      HANDLE_CODE(pack_integer(bref, c.get<other_r10_s_>().sched_cell_id_r10, (uint8_t)0u, (uint8_t)7u));
      HANDLE_CODE(pack_integer(bref, c.get<other_r10_s_>().pdsch_start_r10, (uint8_t)1u, (uint8_t)4u));
      break;
    default:
      log_invalid_choice_id(type_, "cross_carrier_sched_cfg_r10_s::sched_cell_info_r10_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cross_carrier_sched_cfg_r10_s::sched_cell_info_r10_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::own_r10:
      HANDLE_CODE(bref.unpack(c.get<own_r10_s_>().cif_presence_r10, 1));
      break;
    case types::other_r10:
      HANDLE_CODE(unpack_integer(c.get<other_r10_s_>().sched_cell_id_r10, bref, (uint8_t)0u, (uint8_t)7u));
      HANDLE_CODE(unpack_integer(c.get<other_r10_s_>().pdsch_start_r10, bref, (uint8_t)1u, (uint8_t)4u));
      break;
    default:
      log_invalid_choice_id(type_, "cross_carrier_sched_cfg_r10_s::sched_cell_info_r10_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool cross_carrier_sched_cfg_r10_s::sched_cell_info_r10_c_::operator==(const sched_cell_info_r10_c_& other) const
{
  if (type_ != other.type_) {
    return false;
  }
  switch (type_) {
    case types::own_r10:
      return c.get<own_r10_s_>().cif_presence_r10 == other.c.get<own_r10_s_>().cif_presence_r10;
    case types::other_r10:
      return c.get<other_r10_s_>().sched_cell_id_r10 == other.c.get<other_r10_s_>().sched_cell_id_r10 and
             c.get<other_r10_s_>().pdsch_start_r10 == other.c.get<other_r10_s_>().pdsch_start_r10;
    default:
      return true;
  }
  return true;
}

// CrossCarrierSchedulingConfig-r13 ::= SEQUENCE
SRSASN_CODE cross_carrier_sched_cfg_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(sched_cell_info_r13.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cross_carrier_sched_cfg_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(sched_cell_info_r13.unpack(bref));

  return SRSASN_SUCCESS;
}
void cross_carrier_sched_cfg_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("schedulingCellInfo-r13");
  sched_cell_info_r13.to_json(j);
  j.end_obj();
}
bool cross_carrier_sched_cfg_r13_s::operator==(const cross_carrier_sched_cfg_r13_s& other) const
{
  return sched_cell_info_r13 == other.sched_cell_info_r13;
}

void cross_carrier_sched_cfg_r13_s::sched_cell_info_r13_c_::destroy_()
{
  switch (type_) {
    case types::own_r13:
      c.destroy<own_r13_s_>();
      break;
    case types::other_r13:
      c.destroy<other_r13_s_>();
      break;
    default:
      break;
  }
}
void cross_carrier_sched_cfg_r13_s::sched_cell_info_r13_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::own_r13:
      c.init<own_r13_s_>();
      break;
    case types::other_r13:
      c.init<other_r13_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cross_carrier_sched_cfg_r13_s::sched_cell_info_r13_c_");
  }
}
cross_carrier_sched_cfg_r13_s::sched_cell_info_r13_c_::sched_cell_info_r13_c_(
    const cross_carrier_sched_cfg_r13_s::sched_cell_info_r13_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::own_r13:
      c.init(other.c.get<own_r13_s_>());
      break;
    case types::other_r13:
      c.init(other.c.get<other_r13_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cross_carrier_sched_cfg_r13_s::sched_cell_info_r13_c_");
  }
}
cross_carrier_sched_cfg_r13_s::sched_cell_info_r13_c_& cross_carrier_sched_cfg_r13_s::sched_cell_info_r13_c_::operator=(
    const cross_carrier_sched_cfg_r13_s::sched_cell_info_r13_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::own_r13:
      c.set(other.c.get<own_r13_s_>());
      break;
    case types::other_r13:
      c.set(other.c.get<other_r13_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cross_carrier_sched_cfg_r13_s::sched_cell_info_r13_c_");
  }

  return *this;
}
cross_carrier_sched_cfg_r13_s::sched_cell_info_r13_c_::own_r13_s_&
cross_carrier_sched_cfg_r13_s::sched_cell_info_r13_c_::set_own_r13()
{
  set(types::own_r13);
  return c.get<own_r13_s_>();
}
cross_carrier_sched_cfg_r13_s::sched_cell_info_r13_c_::other_r13_s_&
cross_carrier_sched_cfg_r13_s::sched_cell_info_r13_c_::set_other_r13()
{
  set(types::other_r13);
  return c.get<other_r13_s_>();
}
void cross_carrier_sched_cfg_r13_s::sched_cell_info_r13_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::own_r13:
      j.write_fieldname("own-r13");
      j.start_obj();
      j.write_bool("cif-Presence-r13", c.get<own_r13_s_>().cif_presence_r13);
      j.end_obj();
      break;
    case types::other_r13:
      j.write_fieldname("other-r13");
      j.start_obj();
      j.write_int("schedulingCellId-r13", c.get<other_r13_s_>().sched_cell_id_r13);
      j.write_int("pdsch-Start-r13", c.get<other_r13_s_>().pdsch_start_r13);
      j.write_int("cif-InSchedulingCell-r13", c.get<other_r13_s_>().cif_in_sched_cell_r13);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "cross_carrier_sched_cfg_r13_s::sched_cell_info_r13_c_");
  }
  j.end_obj();
}
SRSASN_CODE cross_carrier_sched_cfg_r13_s::sched_cell_info_r13_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::own_r13:
      HANDLE_CODE(bref.pack(c.get<own_r13_s_>().cif_presence_r13, 1));
      break;
    case types::other_r13:
      HANDLE_CODE(pack_integer(bref, c.get<other_r13_s_>().sched_cell_id_r13, (uint8_t)0u, (uint8_t)31u));
      HANDLE_CODE(pack_integer(bref, c.get<other_r13_s_>().pdsch_start_r13, (uint8_t)1u, (uint8_t)4u));
      HANDLE_CODE(pack_integer(bref, c.get<other_r13_s_>().cif_in_sched_cell_r13, (uint8_t)1u, (uint8_t)7u));
      break;
    default:
      log_invalid_choice_id(type_, "cross_carrier_sched_cfg_r13_s::sched_cell_info_r13_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cross_carrier_sched_cfg_r13_s::sched_cell_info_r13_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::own_r13:
      HANDLE_CODE(bref.unpack(c.get<own_r13_s_>().cif_presence_r13, 1));
      break;
    case types::other_r13:
      HANDLE_CODE(unpack_integer(c.get<other_r13_s_>().sched_cell_id_r13, bref, (uint8_t)0u, (uint8_t)31u));
      HANDLE_CODE(unpack_integer(c.get<other_r13_s_>().pdsch_start_r13, bref, (uint8_t)1u, (uint8_t)4u));
      HANDLE_CODE(unpack_integer(c.get<other_r13_s_>().cif_in_sched_cell_r13, bref, (uint8_t)1u, (uint8_t)7u));
      break;
    default:
      log_invalid_choice_id(type_, "cross_carrier_sched_cfg_r13_s::sched_cell_info_r13_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool cross_carrier_sched_cfg_r13_s::sched_cell_info_r13_c_::operator==(const sched_cell_info_r13_c_& other) const
{
  if (type_ != other.type_) {
    return false;
  }
  switch (type_) {
    case types::own_r13:
      return c.get<own_r13_s_>().cif_presence_r13 == other.c.get<own_r13_s_>().cif_presence_r13;
    case types::other_r13:
      return c.get<other_r13_s_>().sched_cell_id_r13 == other.c.get<other_r13_s_>().sched_cell_id_r13 and
             c.get<other_r13_s_>().pdsch_start_r13 == other.c.get<other_r13_s_>().pdsch_start_r13 and
             c.get<other_r13_s_>().cif_in_sched_cell_r13 == other.c.get<other_r13_s_>().cif_in_sched_cell_r13;
    default:
      return true;
  }
  return true;
}

// LAA-SCellConfiguration-r13 ::= SEQUENCE
SRSASN_CODE laa_scell_cfg_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(sf_start_position_r13.pack(bref));
  HANDLE_CODE(laa_scell_sf_cfg_r13.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE laa_scell_cfg_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(sf_start_position_r13.unpack(bref));
  HANDLE_CODE(laa_scell_sf_cfg_r13.unpack(bref));

  return SRSASN_SUCCESS;
}
void laa_scell_cfg_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("subframeStartPosition-r13", sf_start_position_r13.to_string());
  j.write_str("laa-SCellSubframeConfig-r13", laa_scell_sf_cfg_r13.to_string());
  j.end_obj();
}
bool laa_scell_cfg_r13_s::operator==(const laa_scell_cfg_r13_s& other) const
{
  return sf_start_position_r13 == other.sf_start_position_r13 and laa_scell_sf_cfg_r13 == other.laa_scell_sf_cfg_r13;
}

const char* laa_scell_cfg_r13_s::sf_start_position_r13_opts::to_string() const
{
  static const char* options[] = {"s0", "s07"};
  return convert_enum_idx(options, 2, value, "laa_scell_cfg_r13_s::sf_start_position_r13_e_");
}
float laa_scell_cfg_r13_s::sf_start_position_r13_opts::to_number() const
{
  static const float options[] = {0.0, 0.7};
  return map_enum_number(options, 2, value, "laa_scell_cfg_r13_s::sf_start_position_r13_e_");
}
const char* laa_scell_cfg_r13_s::sf_start_position_r13_opts::to_number_string() const
{
  static const char* options[] = {"0", "0.7"};
  return convert_enum_idx(options, 2, value, "laa_scell_cfg_r13_s::sf_start_position_r13_e_");
}

// LAA-SCellConfiguration-v1430 ::= SEQUENCE
SRSASN_CODE laa_scell_cfg_v1430_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cross_carrier_sched_cfg_ul_r14_present, 1));
  HANDLE_CODE(bref.pack(lbt_cfg_r14_present, 1));
  HANDLE_CODE(bref.pack(pdcch_cfg_laa_r14_present, 1));
  HANDLE_CODE(bref.pack(absence_of_any_other_technology_r14_present, 1));
  HANDLE_CODE(bref.pack(srs_ul_cfg_ded_aperiodic_v1430_present, 1));

  if (cross_carrier_sched_cfg_ul_r14_present) {
    HANDLE_CODE(cross_carrier_sched_cfg_ul_r14.pack(bref));
  }
  if (lbt_cfg_r14_present) {
    HANDLE_CODE(lbt_cfg_r14.pack(bref));
  }
  if (pdcch_cfg_laa_r14_present) {
    HANDLE_CODE(pdcch_cfg_laa_r14.pack(bref));
  }
  if (srs_ul_cfg_ded_aperiodic_v1430_present) {
    HANDLE_CODE(srs_ul_cfg_ded_aperiodic_v1430.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE laa_scell_cfg_v1430_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cross_carrier_sched_cfg_ul_r14_present, 1));
  HANDLE_CODE(bref.unpack(lbt_cfg_r14_present, 1));
  HANDLE_CODE(bref.unpack(pdcch_cfg_laa_r14_present, 1));
  HANDLE_CODE(bref.unpack(absence_of_any_other_technology_r14_present, 1));
  HANDLE_CODE(bref.unpack(srs_ul_cfg_ded_aperiodic_v1430_present, 1));

  if (cross_carrier_sched_cfg_ul_r14_present) {
    HANDLE_CODE(cross_carrier_sched_cfg_ul_r14.unpack(bref));
  }
  if (lbt_cfg_r14_present) {
    HANDLE_CODE(lbt_cfg_r14.unpack(bref));
  }
  if (pdcch_cfg_laa_r14_present) {
    HANDLE_CODE(pdcch_cfg_laa_r14.unpack(bref));
  }
  if (srs_ul_cfg_ded_aperiodic_v1430_present) {
    HANDLE_CODE(srs_ul_cfg_ded_aperiodic_v1430.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void laa_scell_cfg_v1430_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (cross_carrier_sched_cfg_ul_r14_present) {
    j.write_fieldname("crossCarrierSchedulingConfig-UL-r14");
    cross_carrier_sched_cfg_ul_r14.to_json(j);
  }
  if (lbt_cfg_r14_present) {
    j.write_fieldname("lbt-Config-r14");
    lbt_cfg_r14.to_json(j);
  }
  if (pdcch_cfg_laa_r14_present) {
    j.write_fieldname("pdcch-ConfigLAA-r14");
    pdcch_cfg_laa_r14.to_json(j);
  }
  if (absence_of_any_other_technology_r14_present) {
    j.write_str("absenceOfAnyOtherTechnology-r14", "true");
  }
  if (srs_ul_cfg_ded_aperiodic_v1430_present) {
    j.write_fieldname("soundingRS-UL-ConfigDedicatedAperiodic-v1430");
    srs_ul_cfg_ded_aperiodic_v1430.to_json(j);
  }
  j.end_obj();
}
bool laa_scell_cfg_v1430_s::operator==(const laa_scell_cfg_v1430_s& other) const
{
  return cross_carrier_sched_cfg_ul_r14_present == other.cross_carrier_sched_cfg_ul_r14_present and
         (not cross_carrier_sched_cfg_ul_r14_present or
          cross_carrier_sched_cfg_ul_r14 == other.cross_carrier_sched_cfg_ul_r14) and
         lbt_cfg_r14_present == other.lbt_cfg_r14_present and
         (not lbt_cfg_r14_present or lbt_cfg_r14 == other.lbt_cfg_r14) and
         pdcch_cfg_laa_r14_present == other.pdcch_cfg_laa_r14_present and
         (not pdcch_cfg_laa_r14_present or pdcch_cfg_laa_r14 == other.pdcch_cfg_laa_r14) and
         absence_of_any_other_technology_r14_present == other.absence_of_any_other_technology_r14_present and
         srs_ul_cfg_ded_aperiodic_v1430_present == other.srs_ul_cfg_ded_aperiodic_v1430_present and
         (not srs_ul_cfg_ded_aperiodic_v1430_present or
          srs_ul_cfg_ded_aperiodic_v1430 == other.srs_ul_cfg_ded_aperiodic_v1430);
}

void laa_scell_cfg_v1430_s::cross_carrier_sched_cfg_ul_r14_c_::set(types::options e)
{
  type_ = e;
}
void laa_scell_cfg_v1430_s::cross_carrier_sched_cfg_ul_r14_c_::set_release()
{
  set(types::release);
}
laa_scell_cfg_v1430_s::cross_carrier_sched_cfg_ul_r14_c_::setup_s_&
laa_scell_cfg_v1430_s::cross_carrier_sched_cfg_ul_r14_c_::set_setup()
{
  set(types::setup);
  return c;
}
void laa_scell_cfg_v1430_s::cross_carrier_sched_cfg_ul_r14_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_fieldname("crossCarrierSchedulingConfigLAA-UL-r14");
      c.cross_carrier_sched_cfg_laa_ul_r14.to_json(j);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "laa_scell_cfg_v1430_s::cross_carrier_sched_cfg_ul_r14_c_");
  }
  j.end_obj();
}
SRSASN_CODE laa_scell_cfg_v1430_s::cross_carrier_sched_cfg_ul_r14_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.cross_carrier_sched_cfg_laa_ul_r14.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "laa_scell_cfg_v1430_s::cross_carrier_sched_cfg_ul_r14_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE laa_scell_cfg_v1430_s::cross_carrier_sched_cfg_ul_r14_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.cross_carrier_sched_cfg_laa_ul_r14.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "laa_scell_cfg_v1430_s::cross_carrier_sched_cfg_ul_r14_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool laa_scell_cfg_v1430_s::cross_carrier_sched_cfg_ul_r14_c_::operator==(
    const cross_carrier_sched_cfg_ul_r14_c_& other) const
{
  return type() == other.type() and c.cross_carrier_sched_cfg_laa_ul_r14 == other.c.cross_carrier_sched_cfg_laa_ul_r14;
}

// LAA-SCellConfiguration-v1530 ::= SEQUENCE
SRSASN_CODE laa_scell_cfg_v1530_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(aul_cfg_r15_present, 1));
  HANDLE_CODE(bref.pack(pusch_mode_cfg_laa_r15_present, 1));

  if (aul_cfg_r15_present) {
    HANDLE_CODE(aul_cfg_r15.pack(bref));
  }
  if (pusch_mode_cfg_laa_r15_present) {
    HANDLE_CODE(pusch_mode_cfg_laa_r15.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE laa_scell_cfg_v1530_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(aul_cfg_r15_present, 1));
  HANDLE_CODE(bref.unpack(pusch_mode_cfg_laa_r15_present, 1));

  if (aul_cfg_r15_present) {
    HANDLE_CODE(aul_cfg_r15.unpack(bref));
  }
  if (pusch_mode_cfg_laa_r15_present) {
    HANDLE_CODE(pusch_mode_cfg_laa_r15.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void laa_scell_cfg_v1530_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (aul_cfg_r15_present) {
    j.write_fieldname("aul-Config-r15");
    aul_cfg_r15.to_json(j);
  }
  if (pusch_mode_cfg_laa_r15_present) {
    j.write_fieldname("pusch-ModeConfigLAA-r15");
    pusch_mode_cfg_laa_r15.to_json(j);
  }
  j.end_obj();
}
bool laa_scell_cfg_v1530_s::operator==(const laa_scell_cfg_v1530_s& other) const
{
  return aul_cfg_r15_present == other.aul_cfg_r15_present and
         (not aul_cfg_r15_present or aul_cfg_r15 == other.aul_cfg_r15) and
         pusch_mode_cfg_laa_r15_present == other.pusch_mode_cfg_laa_r15_present and
         (not pusch_mode_cfg_laa_r15_present or pusch_mode_cfg_laa_r15 == other.pusch_mode_cfg_laa_r15);
}

// PDCCH-ConfigSCell-r13 ::= SEQUENCE
SRSASN_CODE pdcch_cfg_scell_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(skip_monitoring_dci_format0_minus1_a_r13_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdcch_cfg_scell_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(skip_monitoring_dci_format0_minus1_a_r13_present, 1));

  return SRSASN_SUCCESS;
}
void pdcch_cfg_scell_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (skip_monitoring_dci_format0_minus1_a_r13_present) {
    j.write_str("skipMonitoringDCI-format0-1A-r13", "true");
  }
  j.end_obj();
}
bool pdcch_cfg_scell_r13_s::operator==(const pdcch_cfg_scell_r13_s& other) const
{
  return skip_monitoring_dci_format0_minus1_a_r13_present == other.skip_monitoring_dci_format0_minus1_a_r13_present;
}

// PDSCH-ConfigDedicatedSCell-v1430 ::= SEQUENCE
SRSASN_CODE pdsch_cfg_ded_scell_v1430_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(tbs_idx_alt2_r14_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdsch_cfg_ded_scell_v1430_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(tbs_idx_alt2_r14_present, 1));

  return SRSASN_SUCCESS;
}
void pdsch_cfg_ded_scell_v1430_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (tbs_idx_alt2_r14_present) {
    j.write_str("tbsIndexAlt2-r14", "b33");
  }
  j.end_obj();
}
bool pdsch_cfg_ded_scell_v1430_s::operator==(const pdsch_cfg_ded_scell_v1430_s& other) const
{
  return tbs_idx_alt2_r14_present == other.tbs_idx_alt2_r14_present;
}

// PUCCH-ConfigDedicated-v1370 ::= SEQUENCE
SRSASN_CODE pucch_cfg_ded_v1370_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pucch_format_v1370.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pucch_cfg_ded_v1370_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(pucch_format_v1370.unpack(bref));

  return SRSASN_SUCCESS;
}
void pucch_cfg_ded_v1370_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("pucch-Format-v1370");
  pucch_format_v1370.to_json(j);
  j.end_obj();
}
bool pucch_cfg_ded_v1370_s::operator==(const pucch_cfg_ded_v1370_s& other) const
{
  return pucch_format_v1370 == other.pucch_format_v1370;
}

void pucch_cfg_ded_v1370_s::pucch_format_v1370_c_::set(types::options e)
{
  type_ = e;
}
void pucch_cfg_ded_v1370_s::pucch_format_v1370_c_::set_release()
{
  set(types::release);
}
pucch_format3_conf_r13_s& pucch_cfg_ded_v1370_s::pucch_format_v1370_c_::set_setup()
{
  set(types::setup);
  return c;
}
void pucch_cfg_ded_v1370_s::pucch_format_v1370_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      c.to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_v1370_s::pucch_format_v1370_c_");
  }
  j.end_obj();
}
SRSASN_CODE pucch_cfg_ded_v1370_s::pucch_format_v1370_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_v1370_s::pucch_format_v1370_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pucch_cfg_ded_v1370_s::pucch_format_v1370_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_v1370_s::pucch_format_v1370_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool pucch_cfg_ded_v1370_s::pucch_format_v1370_c_::operator==(const pucch_format_v1370_c_& other) const
{
  return type() == other.type() and c == other.c;
}

// PUSCH-ConfigDedicatedSCell-r10 ::= SEQUENCE
SRSASN_CODE pusch_cfg_ded_scell_r10_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(group_hop_disabled_r10_present, 1));
  HANDLE_CODE(bref.pack(dmrs_with_occ_activ_r10_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pusch_cfg_ded_scell_r10_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(group_hop_disabled_r10_present, 1));
  HANDLE_CODE(bref.unpack(dmrs_with_occ_activ_r10_present, 1));

  return SRSASN_SUCCESS;
}
void pusch_cfg_ded_scell_r10_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (group_hop_disabled_r10_present) {
    j.write_str("groupHoppingDisabled-r10", "true");
  }
  if (dmrs_with_occ_activ_r10_present) {
    j.write_str("dmrs-WithOCC-Activated-r10", "true");
  }
  j.end_obj();
}
bool pusch_cfg_ded_scell_r10_s::operator==(const pusch_cfg_ded_scell_r10_s& other) const
{
  return group_hop_disabled_r10_present == other.group_hop_disabled_r10_present and
         dmrs_with_occ_activ_r10_present == other.dmrs_with_occ_activ_r10_present;
}

// PUSCH-ConfigDedicatedSCell-v1430 ::= SEQUENCE
SRSASN_CODE pusch_cfg_ded_scell_v1430_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(enable256_qam_r14_present, 1));

  if (enable256_qam_r14_present) {
    HANDLE_CODE(enable256_qam_r14.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pusch_cfg_ded_scell_v1430_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(enable256_qam_r14_present, 1));

  if (enable256_qam_r14_present) {
    HANDLE_CODE(enable256_qam_r14.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pusch_cfg_ded_scell_v1430_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (enable256_qam_r14_present) {
    j.write_fieldname("enable256QAM-r14");
    enable256_qam_r14.to_json(j);
  }
  j.end_obj();
}
bool pusch_cfg_ded_scell_v1430_s::operator==(const pusch_cfg_ded_scell_v1430_s& other) const
{
  return enable256_qam_r14_present == other.enable256_qam_r14_present and
         (not enable256_qam_r14_present or enable256_qam_r14 == other.enable256_qam_r14);
}

// PUSCH-ConfigDedicatedScell-v1530 ::= SEQUENCE
SRSASN_CODE pusch_cfg_ded_scell_v1530_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(uci_on_pusch_r15.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pusch_cfg_ded_scell_v1530_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(uci_on_pusch_r15.unpack(bref));

  return SRSASN_SUCCESS;
}
void pusch_cfg_ded_scell_v1530_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("uci-OnPUSCH-r15");
  uci_on_pusch_r15.to_json(j);
  j.end_obj();
}
bool pusch_cfg_ded_scell_v1530_s::operator==(const pusch_cfg_ded_scell_v1530_s& other) const
{
  return uci_on_pusch_r15 == other.uci_on_pusch_r15;
}

void pusch_cfg_ded_scell_v1530_s::uci_on_pusch_r15_c_::set(types::options e)
{
  type_ = e;
}
void pusch_cfg_ded_scell_v1530_s::uci_on_pusch_r15_c_::set_release()
{
  set(types::release);
}
pusch_cfg_ded_scell_v1530_s::uci_on_pusch_r15_c_::setup_s_&
pusch_cfg_ded_scell_v1530_s::uci_on_pusch_r15_c_::set_setup()
{
  set(types::setup);
  return c;
}
void pusch_cfg_ded_scell_v1530_s::uci_on_pusch_r15_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_int("betaOffsetAUL-r15", c.beta_offset_aul_r15);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "pusch_cfg_ded_scell_v1530_s::uci_on_pusch_r15_c_");
  }
  j.end_obj();
}
SRSASN_CODE pusch_cfg_ded_scell_v1530_s::uci_on_pusch_r15_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(pack_integer(bref, c.beta_offset_aul_r15, (uint8_t)0u, (uint8_t)15u));
      break;
    default:
      log_invalid_choice_id(type_, "pusch_cfg_ded_scell_v1530_s::uci_on_pusch_r15_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pusch_cfg_ded_scell_v1530_s::uci_on_pusch_r15_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(unpack_integer(c.beta_offset_aul_r15, bref, (uint8_t)0u, (uint8_t)15u));
      break;
    default:
      log_invalid_choice_id(type_, "pusch_cfg_ded_scell_v1530_s::uci_on_pusch_r15_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool pusch_cfg_ded_scell_v1530_s::uci_on_pusch_r15_c_::operator==(const uci_on_pusch_r15_c_& other) const
{
  return type() == other.type() and c.beta_offset_aul_r15 == other.c.beta_offset_aul_r15;
}

// SchedulingRequestConfigSCell-r13 ::= CHOICE
void sched_request_cfg_scell_r13_c::set(types::options e)
{
  type_ = e;
}
void sched_request_cfg_scell_r13_c::set_release()
{
  set(types::release);
}
sched_request_cfg_scell_r13_c::setup_s_& sched_request_cfg_scell_r13_c::set_setup()
{
  set(types::setup);
  return c;
}
void sched_request_cfg_scell_r13_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_int("sr-PUCCH-ResourceIndex-r13", c.sr_pucch_res_idx_r13);
      if (c.sr_pucch_res_idx_p1_r13_present) {
        j.write_int("sr-PUCCH-ResourceIndexP1-r13", c.sr_pucch_res_idx_p1_r13);
      }
      j.write_int("sr-ConfigIndex-r13", c.sr_cfg_idx_r13);
      j.write_str("dsr-TransMax-r13", c.dsr_trans_max_r13.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "sched_request_cfg_scell_r13_c");
  }
  j.end_obj();
}
SRSASN_CODE sched_request_cfg_scell_r13_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.sr_pucch_res_idx_p1_r13_present, 1));
      HANDLE_CODE(pack_integer(bref, c.sr_pucch_res_idx_r13, (uint16_t)0u, (uint16_t)2047u));
      if (c.sr_pucch_res_idx_p1_r13_present) {
        HANDLE_CODE(pack_integer(bref, c.sr_pucch_res_idx_p1_r13, (uint16_t)0u, (uint16_t)2047u));
      }
      HANDLE_CODE(pack_integer(bref, c.sr_cfg_idx_r13, (uint8_t)0u, (uint8_t)157u));
      HANDLE_CODE(c.dsr_trans_max_r13.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sched_request_cfg_scell_r13_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sched_request_cfg_scell_r13_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.sr_pucch_res_idx_p1_r13_present, 1));
      HANDLE_CODE(unpack_integer(c.sr_pucch_res_idx_r13, bref, (uint16_t)0u, (uint16_t)2047u));
      if (c.sr_pucch_res_idx_p1_r13_present) {
        HANDLE_CODE(unpack_integer(c.sr_pucch_res_idx_p1_r13, bref, (uint16_t)0u, (uint16_t)2047u));
      }
      HANDLE_CODE(unpack_integer(c.sr_cfg_idx_r13, bref, (uint8_t)0u, (uint8_t)157u));
      HANDLE_CODE(c.dsr_trans_max_r13.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sched_request_cfg_scell_r13_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool sched_request_cfg_scell_r13_c::operator==(const sched_request_cfg_scell_r13_c& other) const
{
  return type() == other.type() and c.sr_pucch_res_idx_r13 == other.c.sr_pucch_res_idx_r13 and
         c.sr_pucch_res_idx_p1_r13_present == other.c.sr_pucch_res_idx_p1_r13_present and
         (not c.sr_pucch_res_idx_p1_r13_present or c.sr_pucch_res_idx_p1_r13 == other.c.sr_pucch_res_idx_p1_r13) and
         c.sr_cfg_idx_r13 == other.c.sr_cfg_idx_r13 and c.dsr_trans_max_r13 == other.c.dsr_trans_max_r13;
}

const char* sched_request_cfg_scell_r13_c::setup_s_::dsr_trans_max_r13_opts::to_string() const
{
  static const char* options[] = {"n4", "n8", "n16", "n32", "n64", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "sched_request_cfg_scell_r13_c::setup_s_::dsr_trans_max_r13_e_");
}
uint8_t sched_request_cfg_scell_r13_c::setup_s_::dsr_trans_max_r13_opts::to_number() const
{
  static const uint8_t options[] = {4, 8, 16, 32, 64};
  return map_enum_number(options, 5, value, "sched_request_cfg_scell_r13_c::setup_s_::dsr_trans_max_r13_e_");
}

// SoundingRS-AperiodicSet-r14 ::= SEQUENCE
SRSASN_CODE srs_aperiodic_set_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(srs_cc_set_idx_list_r14_present, 1));

  if (srs_cc_set_idx_list_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, srs_cc_set_idx_list_r14, 1, 4));
  }
  HANDLE_CODE(srs_ul_cfg_ded_aperiodic_r14.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_aperiodic_set_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(srs_cc_set_idx_list_r14_present, 1));

  if (srs_cc_set_idx_list_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(srs_cc_set_idx_list_r14, bref, 1, 4));
  }
  HANDLE_CODE(srs_ul_cfg_ded_aperiodic_r14.unpack(bref));

  return SRSASN_SUCCESS;
}
void srs_aperiodic_set_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (srs_cc_set_idx_list_r14_present) {
    j.start_array("srs-CC-SetIndexList-r14");
    for (const auto& e1 : srs_cc_set_idx_list_r14) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.write_fieldname("soundingRS-UL-ConfigDedicatedAperiodic-r14");
  srs_ul_cfg_ded_aperiodic_r14.to_json(j);
  j.end_obj();
}
bool srs_aperiodic_set_r14_s::operator==(const srs_aperiodic_set_r14_s& other) const
{
  return srs_cc_set_idx_list_r14_present == other.srs_cc_set_idx_list_r14_present and
         (not srs_cc_set_idx_list_r14_present or srs_cc_set_idx_list_r14 == other.srs_cc_set_idx_list_r14) and
         srs_ul_cfg_ded_aperiodic_r14 == other.srs_ul_cfg_ded_aperiodic_r14;
}

// SoundingRS-AperiodicSetUpPTsExt-r14 ::= SEQUENCE
SRSASN_CODE srs_aperiodic_set_up_pts_ext_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(srs_cc_set_idx_list_r14_present, 1));

  if (srs_cc_set_idx_list_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, srs_cc_set_idx_list_r14, 1, 4));
  }
  HANDLE_CODE(srs_ul_cfg_ded_aperiodic_up_pts_ext_r14.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_aperiodic_set_up_pts_ext_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(srs_cc_set_idx_list_r14_present, 1));

  if (srs_cc_set_idx_list_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(srs_cc_set_idx_list_r14, bref, 1, 4));
  }
  HANDLE_CODE(srs_ul_cfg_ded_aperiodic_up_pts_ext_r14.unpack(bref));

  return SRSASN_SUCCESS;
}
void srs_aperiodic_set_up_pts_ext_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (srs_cc_set_idx_list_r14_present) {
    j.start_array("srs-CC-SetIndexList-r14");
    for (const auto& e1 : srs_cc_set_idx_list_r14) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.write_fieldname("soundingRS-UL-ConfigDedicatedAperiodicUpPTsExt-r14");
  srs_ul_cfg_ded_aperiodic_up_pts_ext_r14.to_json(j);
  j.end_obj();
}
bool srs_aperiodic_set_up_pts_ext_r14_s::operator==(const srs_aperiodic_set_up_pts_ext_r14_s& other) const
{
  return srs_cc_set_idx_list_r14_present == other.srs_cc_set_idx_list_r14_present and
         (not srs_cc_set_idx_list_r14_present or srs_cc_set_idx_list_r14 == other.srs_cc_set_idx_list_r14) and
         srs_ul_cfg_ded_aperiodic_up_pts_ext_r14 == other.srs_ul_cfg_ded_aperiodic_up_pts_ext_r14;
}

// TPC-PDCCH-ConfigSCell-r13 ::= CHOICE
void tpc_pdcch_cfg_scell_r13_c::set(types::options e)
{
  type_ = e;
}
void tpc_pdcch_cfg_scell_r13_c::set_release()
{
  set(types::release);
}
tpc_pdcch_cfg_scell_r13_c::setup_s_& tpc_pdcch_cfg_scell_r13_c::set_setup()
{
  set(types::setup);
  return c;
}
void tpc_pdcch_cfg_scell_r13_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_fieldname("tpc-Index-PUCCH-SCell-r13");
      c.tpc_idx_pucch_scell_r13.to_json(j);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "tpc_pdcch_cfg_scell_r13_c");
  }
  j.end_obj();
}
SRSASN_CODE tpc_pdcch_cfg_scell_r13_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.tpc_idx_pucch_scell_r13.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "tpc_pdcch_cfg_scell_r13_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE tpc_pdcch_cfg_scell_r13_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.tpc_idx_pucch_scell_r13.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "tpc_pdcch_cfg_scell_r13_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool tpc_pdcch_cfg_scell_r13_c::operator==(const tpc_pdcch_cfg_scell_r13_c& other) const
{
  return type() == other.type() and c.tpc_idx_pucch_scell_r13 == other.c.tpc_idx_pucch_scell_r13;
}

// UplinkPUSCH-LessPowerControlDedicated-v1430 ::= SEQUENCE
SRSASN_CODE ul_pusch_less_pwr_ctrl_ded_v1430_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(p0_ue_periodic_srs_r14_present, 1));
  HANDLE_CODE(bref.pack(p0_ue_aperiodic_srs_r14_present, 1));

  if (p0_ue_periodic_srs_r14_present) {
    HANDLE_CODE(pack_integer(bref, p0_ue_periodic_srs_r14, (int8_t)-8, (int8_t)7));
  }
  if (p0_ue_aperiodic_srs_r14_present) {
    HANDLE_CODE(pack_integer(bref, p0_ue_aperiodic_srs_r14, (int8_t)-8, (int8_t)7));
  }
  HANDLE_CODE(bref.pack(accumulation_enabled_r14, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_pusch_less_pwr_ctrl_ded_v1430_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(p0_ue_periodic_srs_r14_present, 1));
  HANDLE_CODE(bref.unpack(p0_ue_aperiodic_srs_r14_present, 1));

  if (p0_ue_periodic_srs_r14_present) {
    HANDLE_CODE(unpack_integer(p0_ue_periodic_srs_r14, bref, (int8_t)-8, (int8_t)7));
  }
  if (p0_ue_aperiodic_srs_r14_present) {
    HANDLE_CODE(unpack_integer(p0_ue_aperiodic_srs_r14, bref, (int8_t)-8, (int8_t)7));
  }
  HANDLE_CODE(bref.unpack(accumulation_enabled_r14, 1));

  return SRSASN_SUCCESS;
}
void ul_pusch_less_pwr_ctrl_ded_v1430_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (p0_ue_periodic_srs_r14_present) {
    j.write_int("p0-UE-PeriodicSRS-r14", p0_ue_periodic_srs_r14);
  }
  if (p0_ue_aperiodic_srs_r14_present) {
    j.write_int("p0-UE-AperiodicSRS-r14", p0_ue_aperiodic_srs_r14);
  }
  j.write_bool("accumulationEnabled-r14", accumulation_enabled_r14);
  j.end_obj();
}
bool ul_pusch_less_pwr_ctrl_ded_v1430_s::operator==(const ul_pusch_less_pwr_ctrl_ded_v1430_s& other) const
{
  return p0_ue_periodic_srs_r14_present == other.p0_ue_periodic_srs_r14_present and
         (not p0_ue_periodic_srs_r14_present or p0_ue_periodic_srs_r14 == other.p0_ue_periodic_srs_r14) and
         p0_ue_aperiodic_srs_r14_present == other.p0_ue_aperiodic_srs_r14_present and
         (not p0_ue_aperiodic_srs_r14_present or p0_ue_aperiodic_srs_r14 == other.p0_ue_aperiodic_srs_r14) and
         accumulation_enabled_r14 == other.accumulation_enabled_r14;
}

// UplinkPowerControlDedicatedSCell-r10 ::= SEQUENCE
SRSASN_CODE ul_pwr_ctrl_ded_scell_r10_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(psrs_offset_ap_r10_present, 1));
  HANDLE_CODE(bref.pack(filt_coef_r10_present, 1));

  HANDLE_CODE(pack_integer(bref, p0_ue_pusch_r10, (int8_t)-8, (int8_t)7));
  HANDLE_CODE(delta_mcs_enabled_r10.pack(bref));
  HANDLE_CODE(bref.pack(accumulation_enabled_r10, 1));
  HANDLE_CODE(pack_integer(bref, psrs_offset_r10, (uint8_t)0u, (uint8_t)15u));
  if (psrs_offset_ap_r10_present) {
    HANDLE_CODE(pack_integer(bref, psrs_offset_ap_r10, (uint8_t)0u, (uint8_t)15u));
  }
  if (filt_coef_r10_present) {
    HANDLE_CODE(filt_coef_r10.pack(bref));
  }
  HANDLE_CODE(pathloss_ref_linking_r10.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_pwr_ctrl_ded_scell_r10_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(psrs_offset_ap_r10_present, 1));
  HANDLE_CODE(bref.unpack(filt_coef_r10_present, 1));

  HANDLE_CODE(unpack_integer(p0_ue_pusch_r10, bref, (int8_t)-8, (int8_t)7));
  HANDLE_CODE(delta_mcs_enabled_r10.unpack(bref));
  HANDLE_CODE(bref.unpack(accumulation_enabled_r10, 1));
  HANDLE_CODE(unpack_integer(psrs_offset_r10, bref, (uint8_t)0u, (uint8_t)15u));
  if (psrs_offset_ap_r10_present) {
    HANDLE_CODE(unpack_integer(psrs_offset_ap_r10, bref, (uint8_t)0u, (uint8_t)15u));
  }
  if (filt_coef_r10_present) {
    HANDLE_CODE(filt_coef_r10.unpack(bref));
  }
  HANDLE_CODE(pathloss_ref_linking_r10.unpack(bref));

  return SRSASN_SUCCESS;
}
void ul_pwr_ctrl_ded_scell_r10_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("p0-UE-PUSCH-r10", p0_ue_pusch_r10);
  j.write_str("deltaMCS-Enabled-r10", delta_mcs_enabled_r10.to_string());
  j.write_bool("accumulationEnabled-r10", accumulation_enabled_r10);
  j.write_int("pSRS-Offset-r10", psrs_offset_r10);
  if (psrs_offset_ap_r10_present) {
    j.write_int("pSRS-OffsetAp-r10", psrs_offset_ap_r10);
  }
  if (filt_coef_r10_present) {
    j.write_str("filterCoefficient-r10", filt_coef_r10.to_string());
  }
  j.write_str("pathlossReferenceLinking-r10", pathloss_ref_linking_r10.to_string());
  j.end_obj();
}
bool ul_pwr_ctrl_ded_scell_r10_s::operator==(const ul_pwr_ctrl_ded_scell_r10_s& other) const
{
  return p0_ue_pusch_r10 == other.p0_ue_pusch_r10 and delta_mcs_enabled_r10 == other.delta_mcs_enabled_r10 and
         accumulation_enabled_r10 == other.accumulation_enabled_r10 and psrs_offset_r10 == other.psrs_offset_r10 and
         psrs_offset_ap_r10_present == other.psrs_offset_ap_r10_present and
         (not psrs_offset_ap_r10_present or psrs_offset_ap_r10 == other.psrs_offset_ap_r10) and
         filt_coef_r10_present == other.filt_coef_r10_present and
         (not filt_coef_r10_present or filt_coef_r10 == other.filt_coef_r10) and
         pathloss_ref_linking_r10 == other.pathloss_ref_linking_r10;
}

const char* ul_pwr_ctrl_ded_scell_r10_s::delta_mcs_enabled_r10_opts::to_string() const
{
  static const char* options[] = {"en0", "en1"};
  return convert_enum_idx(options, 2, value, "ul_pwr_ctrl_ded_scell_r10_s::delta_mcs_enabled_r10_e_");
}
uint8_t ul_pwr_ctrl_ded_scell_r10_s::delta_mcs_enabled_r10_opts::to_number() const
{
  static const uint8_t options[] = {0, 1};
  return map_enum_number(options, 2, value, "ul_pwr_ctrl_ded_scell_r10_s::delta_mcs_enabled_r10_e_");
}

const char* ul_pwr_ctrl_ded_scell_r10_s::pathloss_ref_linking_r10_opts::to_string() const
{
  static const char* options[] = {"pCell", "sCell"};
  return convert_enum_idx(options, 2, value, "ul_pwr_ctrl_ded_scell_r10_s::pathloss_ref_linking_r10_e_");
}

// UplinkPowerControlDedicatedSCell-v1310 ::= SEQUENCE
SRSASN_CODE ul_pwr_ctrl_ded_scell_v1310_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(delta_tx_d_offset_list_pucch_r10_present, 1));

  HANDLE_CODE(pack_integer(bref, p0_ue_pucch, (int8_t)-8, (int8_t)7));
  if (delta_tx_d_offset_list_pucch_r10_present) {
    HANDLE_CODE(delta_tx_d_offset_list_pucch_r10.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_pwr_ctrl_ded_scell_v1310_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(delta_tx_d_offset_list_pucch_r10_present, 1));

  HANDLE_CODE(unpack_integer(p0_ue_pucch, bref, (int8_t)-8, (int8_t)7));
  if (delta_tx_d_offset_list_pucch_r10_present) {
    HANDLE_CODE(delta_tx_d_offset_list_pucch_r10.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ul_pwr_ctrl_ded_scell_v1310_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("p0-UE-PUCCH", p0_ue_pucch);
  if (delta_tx_d_offset_list_pucch_r10_present) {
    j.write_fieldname("deltaTxD-OffsetListPUCCH-r10");
    delta_tx_d_offset_list_pucch_r10.to_json(j);
  }
  j.end_obj();
}
bool ul_pwr_ctrl_ded_scell_v1310_s::operator==(const ul_pwr_ctrl_ded_scell_v1310_s& other) const
{
  return p0_ue_pucch == other.p0_ue_pucch and
         delta_tx_d_offset_list_pucch_r10_present == other.delta_tx_d_offset_list_pucch_r10_present and
         (not delta_tx_d_offset_list_pucch_r10_present or
          delta_tx_d_offset_list_pucch_r10 == other.delta_tx_d_offset_list_pucch_r10);
}

// MAC-MainConfigSCell-r11 ::= SEQUENCE
SRSASN_CODE mac_main_cfg_scell_r11_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(stag_id_r11_present, 1));

  if (stag_id_r11_present) {
    HANDLE_CODE(pack_integer(bref, stag_id_r11, (uint8_t)1u, (uint8_t)3u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mac_main_cfg_scell_r11_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(stag_id_r11_present, 1));

  if (stag_id_r11_present) {
    HANDLE_CODE(unpack_integer(stag_id_r11, bref, (uint8_t)1u, (uint8_t)3u));
  }

  return SRSASN_SUCCESS;
}
void mac_main_cfg_scell_r11_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (stag_id_r11_present) {
    j.write_int("stag-Id-r11", stag_id_r11);
  }
  j.end_obj();
}
bool mac_main_cfg_scell_r11_s::operator==(const mac_main_cfg_scell_r11_s& other) const
{
  return ext == other.ext and stag_id_r11_present == other.stag_id_r11_present and
         (not stag_id_r11_present or stag_id_r11 == other.stag_id_r11);
}

// PhysicalConfigDedicatedSCell-r10 ::= SEQUENCE
SRSASN_CODE phys_cfg_ded_scell_r10_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(non_ul_cfg_r10_present, 1));
  HANDLE_CODE(bref.pack(ul_cfg_r10_present, 1));

  if (non_ul_cfg_r10_present) {
    HANDLE_CODE(bref.pack(non_ul_cfg_r10.ant_info_r10_present, 1));
    HANDLE_CODE(bref.pack(non_ul_cfg_r10.cross_carrier_sched_cfg_r10_present, 1));
    HANDLE_CODE(bref.pack(non_ul_cfg_r10.csi_rs_cfg_r10_present, 1));
    HANDLE_CODE(bref.pack(non_ul_cfg_r10.pdsch_cfg_ded_r10_present, 1));
    if (non_ul_cfg_r10.ant_info_r10_present) {
      HANDLE_CODE(non_ul_cfg_r10.ant_info_r10.pack(bref));
    }
    if (non_ul_cfg_r10.cross_carrier_sched_cfg_r10_present) {
      HANDLE_CODE(non_ul_cfg_r10.cross_carrier_sched_cfg_r10.pack(bref));
    }
    if (non_ul_cfg_r10.csi_rs_cfg_r10_present) {
      HANDLE_CODE(non_ul_cfg_r10.csi_rs_cfg_r10.pack(bref));
    }
    if (non_ul_cfg_r10.pdsch_cfg_ded_r10_present) {
      HANDLE_CODE(non_ul_cfg_r10.pdsch_cfg_ded_r10.pack(bref));
    }
  }
  if (ul_cfg_r10_present) {
    HANDLE_CODE(bref.pack(ul_cfg_r10.ant_info_ul_r10_present, 1));
    HANDLE_CODE(bref.pack(ul_cfg_r10.pusch_cfg_ded_scell_r10_present, 1));
    HANDLE_CODE(bref.pack(ul_cfg_r10.ul_pwr_ctrl_ded_scell_r10_present, 1));
    HANDLE_CODE(bref.pack(ul_cfg_r10.cqi_report_cfg_scell_r10_present, 1));
    HANDLE_CODE(bref.pack(ul_cfg_r10.srs_ul_cfg_ded_r10_present, 1));
    HANDLE_CODE(bref.pack(ul_cfg_r10.srs_ul_cfg_ded_v1020_present, 1));
    HANDLE_CODE(bref.pack(ul_cfg_r10.srs_ul_cfg_ded_aperiodic_r10_present, 1));
    if (ul_cfg_r10.ant_info_ul_r10_present) {
      HANDLE_CODE(ul_cfg_r10.ant_info_ul_r10.pack(bref));
    }
    if (ul_cfg_r10.pusch_cfg_ded_scell_r10_present) {
      HANDLE_CODE(ul_cfg_r10.pusch_cfg_ded_scell_r10.pack(bref));
    }
    if (ul_cfg_r10.ul_pwr_ctrl_ded_scell_r10_present) {
      HANDLE_CODE(ul_cfg_r10.ul_pwr_ctrl_ded_scell_r10.pack(bref));
    }
    if (ul_cfg_r10.cqi_report_cfg_scell_r10_present) {
      HANDLE_CODE(ul_cfg_r10.cqi_report_cfg_scell_r10.pack(bref));
    }
    if (ul_cfg_r10.srs_ul_cfg_ded_r10_present) {
      HANDLE_CODE(ul_cfg_r10.srs_ul_cfg_ded_r10.pack(bref));
    }
    if (ul_cfg_r10.srs_ul_cfg_ded_v1020_present) {
      HANDLE_CODE(ul_cfg_r10.srs_ul_cfg_ded_v1020.pack(bref));
    }
    if (ul_cfg_r10.srs_ul_cfg_ded_aperiodic_r10_present) {
      HANDLE_CODE(ul_cfg_r10.srs_ul_cfg_ded_aperiodic_r10.pack(bref));
    }
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= csi_rs_cfg_nzp_to_release_list_r11.is_present();
    group_flags[0] |= csi_rs_cfg_nzp_to_add_mod_list_r11.is_present();
    group_flags[0] |= csi_rs_cfg_zp_to_release_list_r11.is_present();
    group_flags[0] |= csi_rs_cfg_zp_to_add_mod_list_r11.is_present();
    group_flags[0] |= epdcch_cfg_r11.is_present();
    group_flags[0] |= pdsch_cfg_ded_v1130.is_present();
    group_flags[0] |= cqi_report_cfg_v1130.is_present();
    group_flags[0] |= pusch_cfg_ded_v1130.is_present();
    group_flags[0] |= ul_pwr_ctrl_ded_scell_v1130.is_present();
    group_flags[1] |= ant_info_v1250.is_present();
    group_flags[1] |= eimta_main_cfg_scell_r12.is_present();
    group_flags[1] |= cqi_report_cfg_scell_v1250.is_present();
    group_flags[1] |= ul_pwr_ctrl_ded_scell_v1250.is_present();
    group_flags[1] |= csi_rs_cfg_v1250.is_present();
    group_flags[2] |= pdsch_cfg_ded_v1280.is_present();
    group_flags[3] |= pucch_cell_r13_present;
    group_flags[3] |= pucch_scell.is_present();
    group_flags[3] |= cross_carrier_sched_cfg_r13.is_present();
    group_flags[3] |= pdcch_cfg_scell_r13.is_present();
    group_flags[3] |= cqi_report_cfg_v1310.is_present();
    group_flags[3] |= pdsch_cfg_ded_v1310.is_present();
    group_flags[3] |= srs_ul_cfg_ded_v1310.is_present();
    group_flags[3] |= srs_ul_cfg_ded_up_pts_ext_r13.is_present();
    group_flags[3] |= srs_ul_cfg_ded_aperiodic_v1310.is_present();
    group_flags[3] |= srs_ul_cfg_ded_aperiodic_up_pts_ext_r13.is_present();
    group_flags[3] |= csi_rs_cfg_v1310.is_present();
    group_flags[3] |= laa_scell_cfg_r13.is_present();
    group_flags[3] |= csi_rs_cfg_nzp_to_add_mod_list_ext_r13.is_present();
    group_flags[3] |= csi_rs_cfg_nzp_to_release_list_ext_r13.is_present();
    group_flags[4] |= cqi_report_cfg_v1320.is_present();
    group_flags[5] |= laa_scell_cfg_v1430.is_present();
    group_flags[5] |= type_b_srs_tpc_pdcch_cfg_r14.is_present();
    group_flags[5] |= ul_pusch_less_pwr_ctrl_ded_v1430.is_present();
    group_flags[5] |= srs_ul_periodic_cfg_ded_list_r14.is_present();
    group_flags[5] |= srs_ul_periodic_cfg_ded_up_pts_ext_list_r14.is_present();
    group_flags[5] |= srs_ul_aperiodic_cfg_ded_list_r14.is_present();
    group_flags[5] |= srs_ul_cfg_ded_ap_up_pts_ext_list_r14.is_present();
    group_flags[5] |= must_cfg_r14.is_present();
    group_flags[5] |= pusch_cfg_ded_v1430.is_present();
    group_flags[5] |= csi_rs_cfg_v1430.is_present();
    group_flags[5] |= csi_rs_cfg_zp_ap_list_r14.is_present();
    group_flags[5] |= cqi_report_cfg_v1430.is_present();
    group_flags[5] |= semi_open_loop_r14_present;
    group_flags[5] |= pdsch_cfg_ded_scell_v1430.is_present();
    group_flags[6] |= csi_rs_cfg_v1480.is_present();
    group_flags[7] |= phys_cfg_ded_stti_r15.is_present();
    group_flags[7] |= pdsch_cfg_ded_v1530.is_present();
    group_flags[7] |= dummy.is_present();
    group_flags[7] |= cqi_report_cfg_scell_r15.is_present();
    group_flags[7] |= cqi_short_cfg_scell_r15.is_present();
    group_flags[7] |= csi_rs_cfg_v1530.is_present();
    group_flags[7] |= ul_pwr_ctrl_ded_scell_v1530.is_present();
    group_flags[7] |= laa_scell_cfg_v1530.is_present();
    group_flags[7] |= pusch_cfg_ded_v1530.is_present();
    group_flags[7] |= semi_static_cfi_cfg_r15.is_present();
    group_flags[7] |= blind_pdsch_repeat_cfg_r15.is_present();
    group_flags[8] |= spucch_cfg_v1550.is_present();
    group_flags[9] |= srs_ul_cfg_ded_add_r16.is_present();
    group_flags[9] |= ul_pwr_ctrl_add_srs_r16.is_present();
    group_flags[9] |= srs_virtual_cell_id_r16.is_present();
    group_flags[9] |= wideband_prg_r16.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(csi_rs_cfg_nzp_to_release_list_r11.is_present(), 1));
      HANDLE_CODE(bref.pack(csi_rs_cfg_nzp_to_add_mod_list_r11.is_present(), 1));
      HANDLE_CODE(bref.pack(csi_rs_cfg_zp_to_release_list_r11.is_present(), 1));
      HANDLE_CODE(bref.pack(csi_rs_cfg_zp_to_add_mod_list_r11.is_present(), 1));
      HANDLE_CODE(bref.pack(epdcch_cfg_r11.is_present(), 1));
      HANDLE_CODE(bref.pack(pdsch_cfg_ded_v1130.is_present(), 1));
      HANDLE_CODE(bref.pack(cqi_report_cfg_v1130.is_present(), 1));
      HANDLE_CODE(bref.pack(pusch_cfg_ded_v1130.is_present(), 1));
      HANDLE_CODE(bref.pack(ul_pwr_ctrl_ded_scell_v1130.is_present(), 1));
      if (csi_rs_cfg_nzp_to_release_list_r11.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *csi_rs_cfg_nzp_to_release_list_r11, 1, 3, integer_packer<uint8_t>(1, 3)));
      }
      if (csi_rs_cfg_nzp_to_add_mod_list_r11.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *csi_rs_cfg_nzp_to_add_mod_list_r11, 1, 3));
      }
      if (csi_rs_cfg_zp_to_release_list_r11.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *csi_rs_cfg_zp_to_release_list_r11, 1, 4, integer_packer<uint8_t>(1, 4)));
      }
      if (csi_rs_cfg_zp_to_add_mod_list_r11.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *csi_rs_cfg_zp_to_add_mod_list_r11, 1, 4));
      }
      if (epdcch_cfg_r11.is_present()) {
        HANDLE_CODE(epdcch_cfg_r11->pack(bref));
      }
      if (pdsch_cfg_ded_v1130.is_present()) {
        HANDLE_CODE(pdsch_cfg_ded_v1130->pack(bref));
      }
      if (cqi_report_cfg_v1130.is_present()) {
        HANDLE_CODE(cqi_report_cfg_v1130->pack(bref));
      }
      if (pusch_cfg_ded_v1130.is_present()) {
        HANDLE_CODE(pusch_cfg_ded_v1130->pack(bref));
      }
      if (ul_pwr_ctrl_ded_scell_v1130.is_present()) {
        HANDLE_CODE(ul_pwr_ctrl_ded_scell_v1130->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ant_info_v1250.is_present(), 1));
      HANDLE_CODE(bref.pack(eimta_main_cfg_scell_r12.is_present(), 1));
      HANDLE_CODE(bref.pack(cqi_report_cfg_scell_v1250.is_present(), 1));
      HANDLE_CODE(bref.pack(ul_pwr_ctrl_ded_scell_v1250.is_present(), 1));
      HANDLE_CODE(bref.pack(csi_rs_cfg_v1250.is_present(), 1));
      if (ant_info_v1250.is_present()) {
        HANDLE_CODE(ant_info_v1250->pack(bref));
      }
      if (eimta_main_cfg_scell_r12.is_present()) {
        HANDLE_CODE(eimta_main_cfg_scell_r12->pack(bref));
      }
      if (cqi_report_cfg_scell_v1250.is_present()) {
        HANDLE_CODE(cqi_report_cfg_scell_v1250->pack(bref));
      }
      if (ul_pwr_ctrl_ded_scell_v1250.is_present()) {
        HANDLE_CODE(ul_pwr_ctrl_ded_scell_v1250->pack(bref));
      }
      if (csi_rs_cfg_v1250.is_present()) {
        HANDLE_CODE(csi_rs_cfg_v1250->pack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(pdsch_cfg_ded_v1280.is_present(), 1));
      if (pdsch_cfg_ded_v1280.is_present()) {
        HANDLE_CODE(pdsch_cfg_ded_v1280->pack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(pucch_cell_r13_present, 1));
      HANDLE_CODE(bref.pack(pucch_scell.is_present(), 1));
      HANDLE_CODE(bref.pack(cross_carrier_sched_cfg_r13.is_present(), 1));
      HANDLE_CODE(bref.pack(pdcch_cfg_scell_r13.is_present(), 1));
      HANDLE_CODE(bref.pack(cqi_report_cfg_v1310.is_present(), 1));
      HANDLE_CODE(bref.pack(pdsch_cfg_ded_v1310.is_present(), 1));
      HANDLE_CODE(bref.pack(srs_ul_cfg_ded_v1310.is_present(), 1));
      HANDLE_CODE(bref.pack(srs_ul_cfg_ded_up_pts_ext_r13.is_present(), 1));
      HANDLE_CODE(bref.pack(srs_ul_cfg_ded_aperiodic_v1310.is_present(), 1));
      HANDLE_CODE(bref.pack(srs_ul_cfg_ded_aperiodic_up_pts_ext_r13.is_present(), 1));
      HANDLE_CODE(bref.pack(csi_rs_cfg_v1310.is_present(), 1));
      HANDLE_CODE(bref.pack(laa_scell_cfg_r13.is_present(), 1));
      HANDLE_CODE(bref.pack(csi_rs_cfg_nzp_to_add_mod_list_ext_r13.is_present(), 1));
      HANDLE_CODE(bref.pack(csi_rs_cfg_nzp_to_release_list_ext_r13.is_present(), 1));
      if (pucch_scell.is_present()) {
        HANDLE_CODE(pucch_scell->pack(bref));
      }
      if (cross_carrier_sched_cfg_r13.is_present()) {
        HANDLE_CODE(cross_carrier_sched_cfg_r13->pack(bref));
      }
      if (pdcch_cfg_scell_r13.is_present()) {
        HANDLE_CODE(pdcch_cfg_scell_r13->pack(bref));
      }
      if (cqi_report_cfg_v1310.is_present()) {
        HANDLE_CODE(cqi_report_cfg_v1310->pack(bref));
      }
      if (pdsch_cfg_ded_v1310.is_present()) {
        HANDLE_CODE(pdsch_cfg_ded_v1310->pack(bref));
      }
      if (srs_ul_cfg_ded_v1310.is_present()) {
        HANDLE_CODE(srs_ul_cfg_ded_v1310->pack(bref));
      }
      if (srs_ul_cfg_ded_up_pts_ext_r13.is_present()) {
        HANDLE_CODE(srs_ul_cfg_ded_up_pts_ext_r13->pack(bref));
      }
      if (srs_ul_cfg_ded_aperiodic_v1310.is_present()) {
        HANDLE_CODE(srs_ul_cfg_ded_aperiodic_v1310->pack(bref));
      }
      if (srs_ul_cfg_ded_aperiodic_up_pts_ext_r13.is_present()) {
        HANDLE_CODE(srs_ul_cfg_ded_aperiodic_up_pts_ext_r13->pack(bref));
      }
      if (csi_rs_cfg_v1310.is_present()) {
        HANDLE_CODE(csi_rs_cfg_v1310->pack(bref));
      }
      if (laa_scell_cfg_r13.is_present()) {
        HANDLE_CODE(laa_scell_cfg_r13->pack(bref));
      }
      if (csi_rs_cfg_nzp_to_add_mod_list_ext_r13.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *csi_rs_cfg_nzp_to_add_mod_list_ext_r13, 1, 21));
      }
      if (csi_rs_cfg_nzp_to_release_list_ext_r13.is_present()) {
        HANDLE_CODE(
            pack_dyn_seq_of(bref, *csi_rs_cfg_nzp_to_release_list_ext_r13, 1, 21, integer_packer<uint8_t>(4, 24)));
      }
    }
    if (group_flags[4]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(cqi_report_cfg_v1320.is_present(), 1));
      if (cqi_report_cfg_v1320.is_present()) {
        HANDLE_CODE(cqi_report_cfg_v1320->pack(bref));
      }
    }
    if (group_flags[5]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(laa_scell_cfg_v1430.is_present(), 1));
      HANDLE_CODE(bref.pack(type_b_srs_tpc_pdcch_cfg_r14.is_present(), 1));
      HANDLE_CODE(bref.pack(ul_pusch_less_pwr_ctrl_ded_v1430.is_present(), 1));
      HANDLE_CODE(bref.pack(srs_ul_periodic_cfg_ded_list_r14.is_present(), 1));
      HANDLE_CODE(bref.pack(srs_ul_periodic_cfg_ded_up_pts_ext_list_r14.is_present(), 1));
      HANDLE_CODE(bref.pack(srs_ul_aperiodic_cfg_ded_list_r14.is_present(), 1));
      HANDLE_CODE(bref.pack(srs_ul_cfg_ded_ap_up_pts_ext_list_r14.is_present(), 1));
      HANDLE_CODE(bref.pack(must_cfg_r14.is_present(), 1));
      HANDLE_CODE(bref.pack(pusch_cfg_ded_v1430.is_present(), 1));
      HANDLE_CODE(bref.pack(csi_rs_cfg_v1430.is_present(), 1));
      HANDLE_CODE(bref.pack(csi_rs_cfg_zp_ap_list_r14.is_present(), 1));
      HANDLE_CODE(bref.pack(cqi_report_cfg_v1430.is_present(), 1));
      HANDLE_CODE(bref.pack(semi_open_loop_r14_present, 1));
      HANDLE_CODE(bref.pack(pdsch_cfg_ded_scell_v1430.is_present(), 1));
      if (laa_scell_cfg_v1430.is_present()) {
        HANDLE_CODE(laa_scell_cfg_v1430->pack(bref));
      }
      if (type_b_srs_tpc_pdcch_cfg_r14.is_present()) {
        HANDLE_CODE(type_b_srs_tpc_pdcch_cfg_r14->pack(bref));
      }
      if (ul_pusch_less_pwr_ctrl_ded_v1430.is_present()) {
        HANDLE_CODE(ul_pusch_less_pwr_ctrl_ded_v1430->pack(bref));
      }
      if (srs_ul_periodic_cfg_ded_list_r14.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *srs_ul_periodic_cfg_ded_list_r14, 1, 2));
      }
      if (srs_ul_periodic_cfg_ded_up_pts_ext_list_r14.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *srs_ul_periodic_cfg_ded_up_pts_ext_list_r14, 1, 4));
      }
      if (srs_ul_aperiodic_cfg_ded_list_r14.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *srs_ul_aperiodic_cfg_ded_list_r14, 1, 2));
      }
      if (srs_ul_cfg_ded_ap_up_pts_ext_list_r14.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *srs_ul_cfg_ded_ap_up_pts_ext_list_r14, 1, 4));
      }
      if (must_cfg_r14.is_present()) {
        HANDLE_CODE(must_cfg_r14->pack(bref));
      }
      if (pusch_cfg_ded_v1430.is_present()) {
        HANDLE_CODE(pusch_cfg_ded_v1430->pack(bref));
      }
      if (csi_rs_cfg_v1430.is_present()) {
        HANDLE_CODE(csi_rs_cfg_v1430->pack(bref));
      }
      if (csi_rs_cfg_zp_ap_list_r14.is_present()) {
        HANDLE_CODE(csi_rs_cfg_zp_ap_list_r14->pack(bref));
      }
      if (cqi_report_cfg_v1430.is_present()) {
        HANDLE_CODE(cqi_report_cfg_v1430->pack(bref));
      }
      if (semi_open_loop_r14_present) {
        HANDLE_CODE(bref.pack(semi_open_loop_r14, 1));
      }
      if (pdsch_cfg_ded_scell_v1430.is_present()) {
        HANDLE_CODE(pdsch_cfg_ded_scell_v1430->pack(bref));
      }
    }
    if (group_flags[6]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(csi_rs_cfg_v1480.is_present(), 1));
      if (csi_rs_cfg_v1480.is_present()) {
        HANDLE_CODE(csi_rs_cfg_v1480->pack(bref));
      }
    }
    if (group_flags[7]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(phys_cfg_ded_stti_r15.is_present(), 1));
      HANDLE_CODE(bref.pack(pdsch_cfg_ded_v1530.is_present(), 1));
      HANDLE_CODE(bref.pack(dummy.is_present(), 1));
      HANDLE_CODE(bref.pack(cqi_report_cfg_scell_r15.is_present(), 1));
      HANDLE_CODE(bref.pack(cqi_short_cfg_scell_r15.is_present(), 1));
      HANDLE_CODE(bref.pack(csi_rs_cfg_v1530.is_present(), 1));
      HANDLE_CODE(bref.pack(ul_pwr_ctrl_ded_scell_v1530.is_present(), 1));
      HANDLE_CODE(bref.pack(laa_scell_cfg_v1530.is_present(), 1));
      HANDLE_CODE(bref.pack(pusch_cfg_ded_v1530.is_present(), 1));
      HANDLE_CODE(bref.pack(semi_static_cfi_cfg_r15.is_present(), 1));
      HANDLE_CODE(bref.pack(blind_pdsch_repeat_cfg_r15.is_present(), 1));
      if (phys_cfg_ded_stti_r15.is_present()) {
        HANDLE_CODE(phys_cfg_ded_stti_r15->pack(bref));
      }
      if (pdsch_cfg_ded_v1530.is_present()) {
        HANDLE_CODE(pdsch_cfg_ded_v1530->pack(bref));
      }
      if (dummy.is_present()) {
        HANDLE_CODE(dummy->pack(bref));
      }
      if (cqi_report_cfg_scell_r15.is_present()) {
        HANDLE_CODE(cqi_report_cfg_scell_r15->pack(bref));
      }
      if (cqi_short_cfg_scell_r15.is_present()) {
        HANDLE_CODE(cqi_short_cfg_scell_r15->pack(bref));
      }
      if (csi_rs_cfg_v1530.is_present()) {
        HANDLE_CODE(csi_rs_cfg_v1530->pack(bref));
      }
      if (ul_pwr_ctrl_ded_scell_v1530.is_present()) {
        HANDLE_CODE(ul_pwr_ctrl_ded_scell_v1530->pack(bref));
      }
      if (laa_scell_cfg_v1530.is_present()) {
        HANDLE_CODE(laa_scell_cfg_v1530->pack(bref));
      }
      if (pusch_cfg_ded_v1530.is_present()) {
        HANDLE_CODE(pusch_cfg_ded_v1530->pack(bref));
      }
      if (semi_static_cfi_cfg_r15.is_present()) {
        HANDLE_CODE(semi_static_cfi_cfg_r15->pack(bref));
      }
      if (blind_pdsch_repeat_cfg_r15.is_present()) {
        HANDLE_CODE(blind_pdsch_repeat_cfg_r15->pack(bref));
      }
    }
    if (group_flags[8]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(spucch_cfg_v1550.is_present(), 1));
      if (spucch_cfg_v1550.is_present()) {
        HANDLE_CODE(spucch_cfg_v1550->pack(bref));
      }
    }
    if (group_flags[9]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(srs_ul_cfg_ded_add_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(ul_pwr_ctrl_add_srs_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(srs_virtual_cell_id_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(wideband_prg_r16.is_present(), 1));
      if (srs_ul_cfg_ded_add_r16.is_present()) {
        HANDLE_CODE(srs_ul_cfg_ded_add_r16->pack(bref));
      }
      if (ul_pwr_ctrl_add_srs_r16.is_present()) {
        HANDLE_CODE(ul_pwr_ctrl_add_srs_r16->pack(bref));
      }
      if (srs_virtual_cell_id_r16.is_present()) {
        HANDLE_CODE(srs_virtual_cell_id_r16->pack(bref));
      }
      if (wideband_prg_r16.is_present()) {
        HANDLE_CODE(wideband_prg_r16->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE phys_cfg_ded_scell_r10_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(non_ul_cfg_r10_present, 1));
  HANDLE_CODE(bref.unpack(ul_cfg_r10_present, 1));

  if (non_ul_cfg_r10_present) {
    HANDLE_CODE(bref.unpack(non_ul_cfg_r10.ant_info_r10_present, 1));
    HANDLE_CODE(bref.unpack(non_ul_cfg_r10.cross_carrier_sched_cfg_r10_present, 1));
    HANDLE_CODE(bref.unpack(non_ul_cfg_r10.csi_rs_cfg_r10_present, 1));
    HANDLE_CODE(bref.unpack(non_ul_cfg_r10.pdsch_cfg_ded_r10_present, 1));
    if (non_ul_cfg_r10.ant_info_r10_present) {
      HANDLE_CODE(non_ul_cfg_r10.ant_info_r10.unpack(bref));
    }
    if (non_ul_cfg_r10.cross_carrier_sched_cfg_r10_present) {
      HANDLE_CODE(non_ul_cfg_r10.cross_carrier_sched_cfg_r10.unpack(bref));
    }
    if (non_ul_cfg_r10.csi_rs_cfg_r10_present) {
      HANDLE_CODE(non_ul_cfg_r10.csi_rs_cfg_r10.unpack(bref));
    }
    if (non_ul_cfg_r10.pdsch_cfg_ded_r10_present) {
      HANDLE_CODE(non_ul_cfg_r10.pdsch_cfg_ded_r10.unpack(bref));
    }
  }
  if (ul_cfg_r10_present) {
    HANDLE_CODE(bref.unpack(ul_cfg_r10.ant_info_ul_r10_present, 1));
    HANDLE_CODE(bref.unpack(ul_cfg_r10.pusch_cfg_ded_scell_r10_present, 1));
    HANDLE_CODE(bref.unpack(ul_cfg_r10.ul_pwr_ctrl_ded_scell_r10_present, 1));
    HANDLE_CODE(bref.unpack(ul_cfg_r10.cqi_report_cfg_scell_r10_present, 1));
    HANDLE_CODE(bref.unpack(ul_cfg_r10.srs_ul_cfg_ded_r10_present, 1));
    HANDLE_CODE(bref.unpack(ul_cfg_r10.srs_ul_cfg_ded_v1020_present, 1));
    HANDLE_CODE(bref.unpack(ul_cfg_r10.srs_ul_cfg_ded_aperiodic_r10_present, 1));
    if (ul_cfg_r10.ant_info_ul_r10_present) {
      HANDLE_CODE(ul_cfg_r10.ant_info_ul_r10.unpack(bref));
    }
    if (ul_cfg_r10.pusch_cfg_ded_scell_r10_present) {
      HANDLE_CODE(ul_cfg_r10.pusch_cfg_ded_scell_r10.unpack(bref));
    }
    if (ul_cfg_r10.ul_pwr_ctrl_ded_scell_r10_present) {
      HANDLE_CODE(ul_cfg_r10.ul_pwr_ctrl_ded_scell_r10.unpack(bref));
    }
    if (ul_cfg_r10.cqi_report_cfg_scell_r10_present) {
      HANDLE_CODE(ul_cfg_r10.cqi_report_cfg_scell_r10.unpack(bref));
    }
    if (ul_cfg_r10.srs_ul_cfg_ded_r10_present) {
      HANDLE_CODE(ul_cfg_r10.srs_ul_cfg_ded_r10.unpack(bref));
    }
    if (ul_cfg_r10.srs_ul_cfg_ded_v1020_present) {
      HANDLE_CODE(ul_cfg_r10.srs_ul_cfg_ded_v1020.unpack(bref));
    }
    if (ul_cfg_r10.srs_ul_cfg_ded_aperiodic_r10_present) {
      HANDLE_CODE(ul_cfg_r10.srs_ul_cfg_ded_aperiodic_r10.unpack(bref));
    }
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(10);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool csi_rs_cfg_nzp_to_release_list_r11_present;
      HANDLE_CODE(bref.unpack(csi_rs_cfg_nzp_to_release_list_r11_present, 1));
      csi_rs_cfg_nzp_to_release_list_r11.set_present(csi_rs_cfg_nzp_to_release_list_r11_present);
      bool csi_rs_cfg_nzp_to_add_mod_list_r11_present;
      HANDLE_CODE(bref.unpack(csi_rs_cfg_nzp_to_add_mod_list_r11_present, 1));
      csi_rs_cfg_nzp_to_add_mod_list_r11.set_present(csi_rs_cfg_nzp_to_add_mod_list_r11_present);
      bool csi_rs_cfg_zp_to_release_list_r11_present;
      HANDLE_CODE(bref.unpack(csi_rs_cfg_zp_to_release_list_r11_present, 1));
      csi_rs_cfg_zp_to_release_list_r11.set_present(csi_rs_cfg_zp_to_release_list_r11_present);
      bool csi_rs_cfg_zp_to_add_mod_list_r11_present;
      HANDLE_CODE(bref.unpack(csi_rs_cfg_zp_to_add_mod_list_r11_present, 1));
      csi_rs_cfg_zp_to_add_mod_list_r11.set_present(csi_rs_cfg_zp_to_add_mod_list_r11_present);
      bool epdcch_cfg_r11_present;
      HANDLE_CODE(bref.unpack(epdcch_cfg_r11_present, 1));
      epdcch_cfg_r11.set_present(epdcch_cfg_r11_present);
      bool pdsch_cfg_ded_v1130_present;
      HANDLE_CODE(bref.unpack(pdsch_cfg_ded_v1130_present, 1));
      pdsch_cfg_ded_v1130.set_present(pdsch_cfg_ded_v1130_present);
      bool cqi_report_cfg_v1130_present;
      HANDLE_CODE(bref.unpack(cqi_report_cfg_v1130_present, 1));
      cqi_report_cfg_v1130.set_present(cqi_report_cfg_v1130_present);
      bool pusch_cfg_ded_v1130_present;
      HANDLE_CODE(bref.unpack(pusch_cfg_ded_v1130_present, 1));
      pusch_cfg_ded_v1130.set_present(pusch_cfg_ded_v1130_present);
      bool ul_pwr_ctrl_ded_scell_v1130_present;
      HANDLE_CODE(bref.unpack(ul_pwr_ctrl_ded_scell_v1130_present, 1));
      ul_pwr_ctrl_ded_scell_v1130.set_present(ul_pwr_ctrl_ded_scell_v1130_present);
      if (csi_rs_cfg_nzp_to_release_list_r11.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*csi_rs_cfg_nzp_to_release_list_r11, bref, 1, 3, integer_packer<uint8_t>(1, 3)));
      }
      if (csi_rs_cfg_nzp_to_add_mod_list_r11.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*csi_rs_cfg_nzp_to_add_mod_list_r11, bref, 1, 3));
      }
      if (csi_rs_cfg_zp_to_release_list_r11.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*csi_rs_cfg_zp_to_release_list_r11, bref, 1, 4, integer_packer<uint8_t>(1, 4)));
      }
      if (csi_rs_cfg_zp_to_add_mod_list_r11.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*csi_rs_cfg_zp_to_add_mod_list_r11, bref, 1, 4));
      }
      if (epdcch_cfg_r11.is_present()) {
        HANDLE_CODE(epdcch_cfg_r11->unpack(bref));
      }
      if (pdsch_cfg_ded_v1130.is_present()) {
        HANDLE_CODE(pdsch_cfg_ded_v1130->unpack(bref));
      }
      if (cqi_report_cfg_v1130.is_present()) {
        HANDLE_CODE(cqi_report_cfg_v1130->unpack(bref));
      }
      if (pusch_cfg_ded_v1130.is_present()) {
        HANDLE_CODE(pusch_cfg_ded_v1130->unpack(bref));
      }
      if (ul_pwr_ctrl_ded_scell_v1130.is_present()) {
        HANDLE_CODE(ul_pwr_ctrl_ded_scell_v1130->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool ant_info_v1250_present;
      HANDLE_CODE(bref.unpack(ant_info_v1250_present, 1));
      ant_info_v1250.set_present(ant_info_v1250_present);
      bool eimta_main_cfg_scell_r12_present;
      HANDLE_CODE(bref.unpack(eimta_main_cfg_scell_r12_present, 1));
      eimta_main_cfg_scell_r12.set_present(eimta_main_cfg_scell_r12_present);
      bool cqi_report_cfg_scell_v1250_present;
      HANDLE_CODE(bref.unpack(cqi_report_cfg_scell_v1250_present, 1));
      cqi_report_cfg_scell_v1250.set_present(cqi_report_cfg_scell_v1250_present);
      bool ul_pwr_ctrl_ded_scell_v1250_present;
      HANDLE_CODE(bref.unpack(ul_pwr_ctrl_ded_scell_v1250_present, 1));
      ul_pwr_ctrl_ded_scell_v1250.set_present(ul_pwr_ctrl_ded_scell_v1250_present);
      bool csi_rs_cfg_v1250_present;
      HANDLE_CODE(bref.unpack(csi_rs_cfg_v1250_present, 1));
      csi_rs_cfg_v1250.set_present(csi_rs_cfg_v1250_present);
      if (ant_info_v1250.is_present()) {
        HANDLE_CODE(ant_info_v1250->unpack(bref));
      }
      if (eimta_main_cfg_scell_r12.is_present()) {
        HANDLE_CODE(eimta_main_cfg_scell_r12->unpack(bref));
      }
      if (cqi_report_cfg_scell_v1250.is_present()) {
        HANDLE_CODE(cqi_report_cfg_scell_v1250->unpack(bref));
      }
      if (ul_pwr_ctrl_ded_scell_v1250.is_present()) {
        HANDLE_CODE(ul_pwr_ctrl_ded_scell_v1250->unpack(bref));
      }
      if (csi_rs_cfg_v1250.is_present()) {
        HANDLE_CODE(csi_rs_cfg_v1250->unpack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool pdsch_cfg_ded_v1280_present;
      HANDLE_CODE(bref.unpack(pdsch_cfg_ded_v1280_present, 1));
      pdsch_cfg_ded_v1280.set_present(pdsch_cfg_ded_v1280_present);
      if (pdsch_cfg_ded_v1280.is_present()) {
        HANDLE_CODE(pdsch_cfg_ded_v1280->unpack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(pucch_cell_r13_present, 1));
      bool pucch_scell_present;
      HANDLE_CODE(bref.unpack(pucch_scell_present, 1));
      pucch_scell.set_present(pucch_scell_present);
      bool cross_carrier_sched_cfg_r13_present;
      HANDLE_CODE(bref.unpack(cross_carrier_sched_cfg_r13_present, 1));
      cross_carrier_sched_cfg_r13.set_present(cross_carrier_sched_cfg_r13_present);
      bool pdcch_cfg_scell_r13_present;
      HANDLE_CODE(bref.unpack(pdcch_cfg_scell_r13_present, 1));
      pdcch_cfg_scell_r13.set_present(pdcch_cfg_scell_r13_present);
      bool cqi_report_cfg_v1310_present;
      HANDLE_CODE(bref.unpack(cqi_report_cfg_v1310_present, 1));
      cqi_report_cfg_v1310.set_present(cqi_report_cfg_v1310_present);
      bool pdsch_cfg_ded_v1310_present;
      HANDLE_CODE(bref.unpack(pdsch_cfg_ded_v1310_present, 1));
      pdsch_cfg_ded_v1310.set_present(pdsch_cfg_ded_v1310_present);
      bool srs_ul_cfg_ded_v1310_present;
      HANDLE_CODE(bref.unpack(srs_ul_cfg_ded_v1310_present, 1));
      srs_ul_cfg_ded_v1310.set_present(srs_ul_cfg_ded_v1310_present);
      bool srs_ul_cfg_ded_up_pts_ext_r13_present;
      HANDLE_CODE(bref.unpack(srs_ul_cfg_ded_up_pts_ext_r13_present, 1));
      srs_ul_cfg_ded_up_pts_ext_r13.set_present(srs_ul_cfg_ded_up_pts_ext_r13_present);
      bool srs_ul_cfg_ded_aperiodic_v1310_present;
      HANDLE_CODE(bref.unpack(srs_ul_cfg_ded_aperiodic_v1310_present, 1));
      srs_ul_cfg_ded_aperiodic_v1310.set_present(srs_ul_cfg_ded_aperiodic_v1310_present);
      bool srs_ul_cfg_ded_aperiodic_up_pts_ext_r13_present;
      HANDLE_CODE(bref.unpack(srs_ul_cfg_ded_aperiodic_up_pts_ext_r13_present, 1));
      srs_ul_cfg_ded_aperiodic_up_pts_ext_r13.set_present(srs_ul_cfg_ded_aperiodic_up_pts_ext_r13_present);
      bool csi_rs_cfg_v1310_present;
      HANDLE_CODE(bref.unpack(csi_rs_cfg_v1310_present, 1));
      csi_rs_cfg_v1310.set_present(csi_rs_cfg_v1310_present);
      bool laa_scell_cfg_r13_present;
      HANDLE_CODE(bref.unpack(laa_scell_cfg_r13_present, 1));
      laa_scell_cfg_r13.set_present(laa_scell_cfg_r13_present);
      bool csi_rs_cfg_nzp_to_add_mod_list_ext_r13_present;
      HANDLE_CODE(bref.unpack(csi_rs_cfg_nzp_to_add_mod_list_ext_r13_present, 1));
      csi_rs_cfg_nzp_to_add_mod_list_ext_r13.set_present(csi_rs_cfg_nzp_to_add_mod_list_ext_r13_present);
      bool csi_rs_cfg_nzp_to_release_list_ext_r13_present;
      HANDLE_CODE(bref.unpack(csi_rs_cfg_nzp_to_release_list_ext_r13_present, 1));
      csi_rs_cfg_nzp_to_release_list_ext_r13.set_present(csi_rs_cfg_nzp_to_release_list_ext_r13_present);
      if (pucch_scell.is_present()) {
        HANDLE_CODE(pucch_scell->unpack(bref));
      }
      if (cross_carrier_sched_cfg_r13.is_present()) {
        HANDLE_CODE(cross_carrier_sched_cfg_r13->unpack(bref));
      }
      if (pdcch_cfg_scell_r13.is_present()) {
        HANDLE_CODE(pdcch_cfg_scell_r13->unpack(bref));
      }
      if (cqi_report_cfg_v1310.is_present()) {
        HANDLE_CODE(cqi_report_cfg_v1310->unpack(bref));
      }
      if (pdsch_cfg_ded_v1310.is_present()) {
        HANDLE_CODE(pdsch_cfg_ded_v1310->unpack(bref));
      }
      if (srs_ul_cfg_ded_v1310.is_present()) {
        HANDLE_CODE(srs_ul_cfg_ded_v1310->unpack(bref));
      }
      if (srs_ul_cfg_ded_up_pts_ext_r13.is_present()) {
        HANDLE_CODE(srs_ul_cfg_ded_up_pts_ext_r13->unpack(bref));
      }
      if (srs_ul_cfg_ded_aperiodic_v1310.is_present()) {
        HANDLE_CODE(srs_ul_cfg_ded_aperiodic_v1310->unpack(bref));
      }
      if (srs_ul_cfg_ded_aperiodic_up_pts_ext_r13.is_present()) {
        HANDLE_CODE(srs_ul_cfg_ded_aperiodic_up_pts_ext_r13->unpack(bref));
      }
      if (csi_rs_cfg_v1310.is_present()) {
        HANDLE_CODE(csi_rs_cfg_v1310->unpack(bref));
      }
      if (laa_scell_cfg_r13.is_present()) {
        HANDLE_CODE(laa_scell_cfg_r13->unpack(bref));
      }
      if (csi_rs_cfg_nzp_to_add_mod_list_ext_r13.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*csi_rs_cfg_nzp_to_add_mod_list_ext_r13, bref, 1, 21));
      }
      if (csi_rs_cfg_nzp_to_release_list_ext_r13.is_present()) {
        HANDLE_CODE(
            unpack_dyn_seq_of(*csi_rs_cfg_nzp_to_release_list_ext_r13, bref, 1, 21, integer_packer<uint8_t>(4, 24)));
      }
    }
    if (group_flags[4]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool cqi_report_cfg_v1320_present;
      HANDLE_CODE(bref.unpack(cqi_report_cfg_v1320_present, 1));
      cqi_report_cfg_v1320.set_present(cqi_report_cfg_v1320_present);
      if (cqi_report_cfg_v1320.is_present()) {
        HANDLE_CODE(cqi_report_cfg_v1320->unpack(bref));
      }
    }
    if (group_flags[5]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool laa_scell_cfg_v1430_present;
      HANDLE_CODE(bref.unpack(laa_scell_cfg_v1430_present, 1));
      laa_scell_cfg_v1430.set_present(laa_scell_cfg_v1430_present);
      bool type_b_srs_tpc_pdcch_cfg_r14_present;
      HANDLE_CODE(bref.unpack(type_b_srs_tpc_pdcch_cfg_r14_present, 1));
      type_b_srs_tpc_pdcch_cfg_r14.set_present(type_b_srs_tpc_pdcch_cfg_r14_present);
      bool ul_pusch_less_pwr_ctrl_ded_v1430_present;
      HANDLE_CODE(bref.unpack(ul_pusch_less_pwr_ctrl_ded_v1430_present, 1));
      ul_pusch_less_pwr_ctrl_ded_v1430.set_present(ul_pusch_less_pwr_ctrl_ded_v1430_present);
      bool srs_ul_periodic_cfg_ded_list_r14_present;
      HANDLE_CODE(bref.unpack(srs_ul_periodic_cfg_ded_list_r14_present, 1));
      srs_ul_periodic_cfg_ded_list_r14.set_present(srs_ul_periodic_cfg_ded_list_r14_present);
      bool srs_ul_periodic_cfg_ded_up_pts_ext_list_r14_present;
      HANDLE_CODE(bref.unpack(srs_ul_periodic_cfg_ded_up_pts_ext_list_r14_present, 1));
      srs_ul_periodic_cfg_ded_up_pts_ext_list_r14.set_present(srs_ul_periodic_cfg_ded_up_pts_ext_list_r14_present);
      bool srs_ul_aperiodic_cfg_ded_list_r14_present;
      HANDLE_CODE(bref.unpack(srs_ul_aperiodic_cfg_ded_list_r14_present, 1));
      srs_ul_aperiodic_cfg_ded_list_r14.set_present(srs_ul_aperiodic_cfg_ded_list_r14_present);
      bool srs_ul_cfg_ded_ap_up_pts_ext_list_r14_present;
      HANDLE_CODE(bref.unpack(srs_ul_cfg_ded_ap_up_pts_ext_list_r14_present, 1));
      srs_ul_cfg_ded_ap_up_pts_ext_list_r14.set_present(srs_ul_cfg_ded_ap_up_pts_ext_list_r14_present);
      bool must_cfg_r14_present;
      HANDLE_CODE(bref.unpack(must_cfg_r14_present, 1));
      must_cfg_r14.set_present(must_cfg_r14_present);
      bool pusch_cfg_ded_v1430_present;
      HANDLE_CODE(bref.unpack(pusch_cfg_ded_v1430_present, 1));
      pusch_cfg_ded_v1430.set_present(pusch_cfg_ded_v1430_present);
      bool csi_rs_cfg_v1430_present;
      HANDLE_CODE(bref.unpack(csi_rs_cfg_v1430_present, 1));
      csi_rs_cfg_v1430.set_present(csi_rs_cfg_v1430_present);
      bool csi_rs_cfg_zp_ap_list_r14_present;
      HANDLE_CODE(bref.unpack(csi_rs_cfg_zp_ap_list_r14_present, 1));
      csi_rs_cfg_zp_ap_list_r14.set_present(csi_rs_cfg_zp_ap_list_r14_present);
      bool cqi_report_cfg_v1430_present;
      HANDLE_CODE(bref.unpack(cqi_report_cfg_v1430_present, 1));
      cqi_report_cfg_v1430.set_present(cqi_report_cfg_v1430_present);
      HANDLE_CODE(bref.unpack(semi_open_loop_r14_present, 1));
      bool pdsch_cfg_ded_scell_v1430_present;
      HANDLE_CODE(bref.unpack(pdsch_cfg_ded_scell_v1430_present, 1));
      pdsch_cfg_ded_scell_v1430.set_present(pdsch_cfg_ded_scell_v1430_present);
      if (laa_scell_cfg_v1430.is_present()) {
        HANDLE_CODE(laa_scell_cfg_v1430->unpack(bref));
      }
      if (type_b_srs_tpc_pdcch_cfg_r14.is_present()) {
        HANDLE_CODE(type_b_srs_tpc_pdcch_cfg_r14->unpack(bref));
      }
      if (ul_pusch_less_pwr_ctrl_ded_v1430.is_present()) {
        HANDLE_CODE(ul_pusch_less_pwr_ctrl_ded_v1430->unpack(bref));
      }
      if (srs_ul_periodic_cfg_ded_list_r14.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*srs_ul_periodic_cfg_ded_list_r14, bref, 1, 2));
      }
      if (srs_ul_periodic_cfg_ded_up_pts_ext_list_r14.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*srs_ul_periodic_cfg_ded_up_pts_ext_list_r14, bref, 1, 4));
      }
      if (srs_ul_aperiodic_cfg_ded_list_r14.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*srs_ul_aperiodic_cfg_ded_list_r14, bref, 1, 2));
      }
      if (srs_ul_cfg_ded_ap_up_pts_ext_list_r14.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*srs_ul_cfg_ded_ap_up_pts_ext_list_r14, bref, 1, 4));
      }
      if (must_cfg_r14.is_present()) {
        HANDLE_CODE(must_cfg_r14->unpack(bref));
      }
      if (pusch_cfg_ded_v1430.is_present()) {
        HANDLE_CODE(pusch_cfg_ded_v1430->unpack(bref));
      }
      if (csi_rs_cfg_v1430.is_present()) {
        HANDLE_CODE(csi_rs_cfg_v1430->unpack(bref));
      }
      if (csi_rs_cfg_zp_ap_list_r14.is_present()) {
        HANDLE_CODE(csi_rs_cfg_zp_ap_list_r14->unpack(bref));
      }
      if (cqi_report_cfg_v1430.is_present()) {
        HANDLE_CODE(cqi_report_cfg_v1430->unpack(bref));
      }
      if (semi_open_loop_r14_present) {
        HANDLE_CODE(bref.unpack(semi_open_loop_r14, 1));
      }
      if (pdsch_cfg_ded_scell_v1430.is_present()) {
        HANDLE_CODE(pdsch_cfg_ded_scell_v1430->unpack(bref));
      }
    }
    if (group_flags[6]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool csi_rs_cfg_v1480_present;
      HANDLE_CODE(bref.unpack(csi_rs_cfg_v1480_present, 1));
      csi_rs_cfg_v1480.set_present(csi_rs_cfg_v1480_present);
      if (csi_rs_cfg_v1480.is_present()) {
        HANDLE_CODE(csi_rs_cfg_v1480->unpack(bref));
      }
    }
    if (group_flags[7]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool phys_cfg_ded_stti_r15_present;
      HANDLE_CODE(bref.unpack(phys_cfg_ded_stti_r15_present, 1));
      phys_cfg_ded_stti_r15.set_present(phys_cfg_ded_stti_r15_present);
      bool pdsch_cfg_ded_v1530_present;
      HANDLE_CODE(bref.unpack(pdsch_cfg_ded_v1530_present, 1));
      pdsch_cfg_ded_v1530.set_present(pdsch_cfg_ded_v1530_present);
      bool dummy_present;
      HANDLE_CODE(bref.unpack(dummy_present, 1));
      dummy.set_present(dummy_present);
      bool cqi_report_cfg_scell_r15_present;
      HANDLE_CODE(bref.unpack(cqi_report_cfg_scell_r15_present, 1));
      cqi_report_cfg_scell_r15.set_present(cqi_report_cfg_scell_r15_present);
      bool cqi_short_cfg_scell_r15_present;
      HANDLE_CODE(bref.unpack(cqi_short_cfg_scell_r15_present, 1));
      cqi_short_cfg_scell_r15.set_present(cqi_short_cfg_scell_r15_present);
      bool csi_rs_cfg_v1530_present;
      HANDLE_CODE(bref.unpack(csi_rs_cfg_v1530_present, 1));
      csi_rs_cfg_v1530.set_present(csi_rs_cfg_v1530_present);
      bool ul_pwr_ctrl_ded_scell_v1530_present;
      HANDLE_CODE(bref.unpack(ul_pwr_ctrl_ded_scell_v1530_present, 1));
      ul_pwr_ctrl_ded_scell_v1530.set_present(ul_pwr_ctrl_ded_scell_v1530_present);
      bool laa_scell_cfg_v1530_present;
      HANDLE_CODE(bref.unpack(laa_scell_cfg_v1530_present, 1));
      laa_scell_cfg_v1530.set_present(laa_scell_cfg_v1530_present);
      bool pusch_cfg_ded_v1530_present;
      HANDLE_CODE(bref.unpack(pusch_cfg_ded_v1530_present, 1));
      pusch_cfg_ded_v1530.set_present(pusch_cfg_ded_v1530_present);
      bool semi_static_cfi_cfg_r15_present;
      HANDLE_CODE(bref.unpack(semi_static_cfi_cfg_r15_present, 1));
      semi_static_cfi_cfg_r15.set_present(semi_static_cfi_cfg_r15_present);
      bool blind_pdsch_repeat_cfg_r15_present;
      HANDLE_CODE(bref.unpack(blind_pdsch_repeat_cfg_r15_present, 1));
      blind_pdsch_repeat_cfg_r15.set_present(blind_pdsch_repeat_cfg_r15_present);
      if (phys_cfg_ded_stti_r15.is_present()) {
        HANDLE_CODE(phys_cfg_ded_stti_r15->unpack(bref));
      }
      if (pdsch_cfg_ded_v1530.is_present()) {
        HANDLE_CODE(pdsch_cfg_ded_v1530->unpack(bref));
      }
      if (dummy.is_present()) {
        HANDLE_CODE(dummy->unpack(bref));
      }
      if (cqi_report_cfg_scell_r15.is_present()) {
        HANDLE_CODE(cqi_report_cfg_scell_r15->unpack(bref));
      }
      if (cqi_short_cfg_scell_r15.is_present()) {
        HANDLE_CODE(cqi_short_cfg_scell_r15->unpack(bref));
      }
      if (csi_rs_cfg_v1530.is_present()) {
        HANDLE_CODE(csi_rs_cfg_v1530->unpack(bref));
      }
      if (ul_pwr_ctrl_ded_scell_v1530.is_present()) {
        HANDLE_CODE(ul_pwr_ctrl_ded_scell_v1530->unpack(bref));
      }
      if (laa_scell_cfg_v1530.is_present()) {
        HANDLE_CODE(laa_scell_cfg_v1530->unpack(bref));
      }
      if (pusch_cfg_ded_v1530.is_present()) {
        HANDLE_CODE(pusch_cfg_ded_v1530->unpack(bref));
      }
      if (semi_static_cfi_cfg_r15.is_present()) {
        HANDLE_CODE(semi_static_cfi_cfg_r15->unpack(bref));
      }
      if (blind_pdsch_repeat_cfg_r15.is_present()) {
        HANDLE_CODE(blind_pdsch_repeat_cfg_r15->unpack(bref));
      }
    }
    if (group_flags[8]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool spucch_cfg_v1550_present;
      HANDLE_CODE(bref.unpack(spucch_cfg_v1550_present, 1));
      spucch_cfg_v1550.set_present(spucch_cfg_v1550_present);
      if (spucch_cfg_v1550.is_present()) {
        HANDLE_CODE(spucch_cfg_v1550->unpack(bref));
      }
    }
    if (group_flags[9]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool srs_ul_cfg_ded_add_r16_present;
      HANDLE_CODE(bref.unpack(srs_ul_cfg_ded_add_r16_present, 1));
      srs_ul_cfg_ded_add_r16.set_present(srs_ul_cfg_ded_add_r16_present);
      bool ul_pwr_ctrl_add_srs_r16_present;
      HANDLE_CODE(bref.unpack(ul_pwr_ctrl_add_srs_r16_present, 1));
      ul_pwr_ctrl_add_srs_r16.set_present(ul_pwr_ctrl_add_srs_r16_present);
      bool srs_virtual_cell_id_r16_present;
      HANDLE_CODE(bref.unpack(srs_virtual_cell_id_r16_present, 1));
      srs_virtual_cell_id_r16.set_present(srs_virtual_cell_id_r16_present);
      bool wideband_prg_r16_present;
      HANDLE_CODE(bref.unpack(wideband_prg_r16_present, 1));
      wideband_prg_r16.set_present(wideband_prg_r16_present);
      if (srs_ul_cfg_ded_add_r16.is_present()) {
        HANDLE_CODE(srs_ul_cfg_ded_add_r16->unpack(bref));
      }
      if (ul_pwr_ctrl_add_srs_r16.is_present()) {
        HANDLE_CODE(ul_pwr_ctrl_add_srs_r16->unpack(bref));
      }
      if (srs_virtual_cell_id_r16.is_present()) {
        HANDLE_CODE(srs_virtual_cell_id_r16->unpack(bref));
      }
      if (wideband_prg_r16.is_present()) {
        HANDLE_CODE(wideband_prg_r16->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void phys_cfg_ded_scell_r10_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (non_ul_cfg_r10_present) {
    j.write_fieldname("nonUL-Configuration-r10");
    j.start_obj();
    if (non_ul_cfg_r10.ant_info_r10_present) {
      j.write_fieldname("antennaInfo-r10");
      non_ul_cfg_r10.ant_info_r10.to_json(j);
    }
    if (non_ul_cfg_r10.cross_carrier_sched_cfg_r10_present) {
      j.write_fieldname("crossCarrierSchedulingConfig-r10");
      non_ul_cfg_r10.cross_carrier_sched_cfg_r10.to_json(j);
    }
    if (non_ul_cfg_r10.csi_rs_cfg_r10_present) {
      j.write_fieldname("csi-RS-Config-r10");
      non_ul_cfg_r10.csi_rs_cfg_r10.to_json(j);
    }
    if (non_ul_cfg_r10.pdsch_cfg_ded_r10_present) {
      j.write_fieldname("pdsch-ConfigDedicated-r10");
      non_ul_cfg_r10.pdsch_cfg_ded_r10.to_json(j);
    }
    j.end_obj();
  }
  if (ul_cfg_r10_present) {
    j.write_fieldname("ul-Configuration-r10");
    j.start_obj();
    if (ul_cfg_r10.ant_info_ul_r10_present) {
      j.write_fieldname("antennaInfoUL-r10");
      ul_cfg_r10.ant_info_ul_r10.to_json(j);
    }
    if (ul_cfg_r10.pusch_cfg_ded_scell_r10_present) {
      j.write_fieldname("pusch-ConfigDedicatedSCell-r10");
      ul_cfg_r10.pusch_cfg_ded_scell_r10.to_json(j);
    }
    if (ul_cfg_r10.ul_pwr_ctrl_ded_scell_r10_present) {
      j.write_fieldname("uplinkPowerControlDedicatedSCell-r10");
      ul_cfg_r10.ul_pwr_ctrl_ded_scell_r10.to_json(j);
    }
    if (ul_cfg_r10.cqi_report_cfg_scell_r10_present) {
      j.write_fieldname("cqi-ReportConfigSCell-r10");
      ul_cfg_r10.cqi_report_cfg_scell_r10.to_json(j);
    }
    if (ul_cfg_r10.srs_ul_cfg_ded_r10_present) {
      j.write_fieldname("soundingRS-UL-ConfigDedicated-r10");
      ul_cfg_r10.srs_ul_cfg_ded_r10.to_json(j);
    }
    if (ul_cfg_r10.srs_ul_cfg_ded_v1020_present) {
      j.write_fieldname("soundingRS-UL-ConfigDedicated-v1020");
      ul_cfg_r10.srs_ul_cfg_ded_v1020.to_json(j);
    }
    if (ul_cfg_r10.srs_ul_cfg_ded_aperiodic_r10_present) {
      j.write_fieldname("soundingRS-UL-ConfigDedicatedAperiodic-r10");
      ul_cfg_r10.srs_ul_cfg_ded_aperiodic_r10.to_json(j);
    }
    j.end_obj();
  }
  if (ext) {
    if (csi_rs_cfg_nzp_to_release_list_r11.is_present()) {
      j.start_array("csi-RS-ConfigNZPToReleaseList-r11");
      for (const auto& e1 : *csi_rs_cfg_nzp_to_release_list_r11) {
        j.write_int(e1);
      }
      j.end_array();
    }
    if (csi_rs_cfg_nzp_to_add_mod_list_r11.is_present()) {
      j.start_array("csi-RS-ConfigNZPToAddModList-r11");
      for (const auto& e1 : *csi_rs_cfg_nzp_to_add_mod_list_r11) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (csi_rs_cfg_zp_to_release_list_r11.is_present()) {
      j.start_array("csi-RS-ConfigZPToReleaseList-r11");
      for (const auto& e1 : *csi_rs_cfg_zp_to_release_list_r11) {
        j.write_int(e1);
      }
      j.end_array();
    }
    if (csi_rs_cfg_zp_to_add_mod_list_r11.is_present()) {
      j.start_array("csi-RS-ConfigZPToAddModList-r11");
      for (const auto& e1 : *csi_rs_cfg_zp_to_add_mod_list_r11) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (epdcch_cfg_r11.is_present()) {
      j.write_fieldname("epdcch-Config-r11");
      epdcch_cfg_r11->to_json(j);
    }
    if (pdsch_cfg_ded_v1130.is_present()) {
      j.write_fieldname("pdsch-ConfigDedicated-v1130");
      pdsch_cfg_ded_v1130->to_json(j);
    }
    if (cqi_report_cfg_v1130.is_present()) {
      j.write_fieldname("cqi-ReportConfig-v1130");
      cqi_report_cfg_v1130->to_json(j);
    }
    if (pusch_cfg_ded_v1130.is_present()) {
      j.write_fieldname("pusch-ConfigDedicated-v1130");
      pusch_cfg_ded_v1130->to_json(j);
    }
    if (ul_pwr_ctrl_ded_scell_v1130.is_present()) {
      j.write_fieldname("uplinkPowerControlDedicatedSCell-v1130");
      ul_pwr_ctrl_ded_scell_v1130->to_json(j);
    }
    if (ant_info_v1250.is_present()) {
      j.write_fieldname("antennaInfo-v1250");
      ant_info_v1250->to_json(j);
    }
    if (eimta_main_cfg_scell_r12.is_present()) {
      j.write_fieldname("eimta-MainConfigSCell-r12");
      eimta_main_cfg_scell_r12->to_json(j);
    }
    if (cqi_report_cfg_scell_v1250.is_present()) {
      j.write_fieldname("cqi-ReportConfigSCell-v1250");
      cqi_report_cfg_scell_v1250->to_json(j);
    }
    if (ul_pwr_ctrl_ded_scell_v1250.is_present()) {
      j.write_fieldname("uplinkPowerControlDedicatedSCell-v1250");
      ul_pwr_ctrl_ded_scell_v1250->to_json(j);
    }
    if (csi_rs_cfg_v1250.is_present()) {
      j.write_fieldname("csi-RS-Config-v1250");
      csi_rs_cfg_v1250->to_json(j);
    }
    if (pdsch_cfg_ded_v1280.is_present()) {
      j.write_fieldname("pdsch-ConfigDedicated-v1280");
      pdsch_cfg_ded_v1280->to_json(j);
    }
    if (pucch_cell_r13_present) {
      j.write_str("pucch-Cell-r13", "true");
    }
    if (pucch_scell.is_present()) {
      j.write_fieldname("pucch-SCell");
      pucch_scell->to_json(j);
    }
    if (cross_carrier_sched_cfg_r13.is_present()) {
      j.write_fieldname("crossCarrierSchedulingConfig-r13");
      cross_carrier_sched_cfg_r13->to_json(j);
    }
    if (pdcch_cfg_scell_r13.is_present()) {
      j.write_fieldname("pdcch-ConfigSCell-r13");
      pdcch_cfg_scell_r13->to_json(j);
    }
    if (cqi_report_cfg_v1310.is_present()) {
      j.write_fieldname("cqi-ReportConfig-v1310");
      cqi_report_cfg_v1310->to_json(j);
    }
    if (pdsch_cfg_ded_v1310.is_present()) {
      j.write_fieldname("pdsch-ConfigDedicated-v1310");
      pdsch_cfg_ded_v1310->to_json(j);
    }
    if (srs_ul_cfg_ded_v1310.is_present()) {
      j.write_fieldname("soundingRS-UL-ConfigDedicated-v1310");
      srs_ul_cfg_ded_v1310->to_json(j);
    }
    if (srs_ul_cfg_ded_up_pts_ext_r13.is_present()) {
      j.write_fieldname("soundingRS-UL-ConfigDedicatedUpPTsExt-r13");
      srs_ul_cfg_ded_up_pts_ext_r13->to_json(j);
    }
    if (srs_ul_cfg_ded_aperiodic_v1310.is_present()) {
      j.write_fieldname("soundingRS-UL-ConfigDedicatedAperiodic-v1310");
      srs_ul_cfg_ded_aperiodic_v1310->to_json(j);
    }
    if (srs_ul_cfg_ded_aperiodic_up_pts_ext_r13.is_present()) {
      j.write_fieldname("soundingRS-UL-ConfigDedicatedAperiodicUpPTsExt-r13");
      srs_ul_cfg_ded_aperiodic_up_pts_ext_r13->to_json(j);
    }
    if (csi_rs_cfg_v1310.is_present()) {
      j.write_fieldname("csi-RS-Config-v1310");
      csi_rs_cfg_v1310->to_json(j);
    }
    if (laa_scell_cfg_r13.is_present()) {
      j.write_fieldname("laa-SCellConfiguration-r13");
      laa_scell_cfg_r13->to_json(j);
    }
    if (csi_rs_cfg_nzp_to_add_mod_list_ext_r13.is_present()) {
      j.start_array("csi-RS-ConfigNZPToAddModListExt-r13");
      for (const auto& e1 : *csi_rs_cfg_nzp_to_add_mod_list_ext_r13) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (csi_rs_cfg_nzp_to_release_list_ext_r13.is_present()) {
      j.start_array("csi-RS-ConfigNZPToReleaseListExt-r13");
      for (const auto& e1 : *csi_rs_cfg_nzp_to_release_list_ext_r13) {
        j.write_int(e1);
      }
      j.end_array();
    }
    if (cqi_report_cfg_v1320.is_present()) {
      j.write_fieldname("cqi-ReportConfig-v1320");
      cqi_report_cfg_v1320->to_json(j);
    }
    if (laa_scell_cfg_v1430.is_present()) {
      j.write_fieldname("laa-SCellConfiguration-v1430");
      laa_scell_cfg_v1430->to_json(j);
    }
    if (type_b_srs_tpc_pdcch_cfg_r14.is_present()) {
      j.write_fieldname("typeB-SRS-TPC-PDCCH-Config-r14");
      type_b_srs_tpc_pdcch_cfg_r14->to_json(j);
    }
    if (ul_pusch_less_pwr_ctrl_ded_v1430.is_present()) {
      j.write_fieldname("uplinkPUSCH-LessPowerControlDedicated-v1430");
      ul_pusch_less_pwr_ctrl_ded_v1430->to_json(j);
    }
    if (srs_ul_periodic_cfg_ded_list_r14.is_present()) {
      j.start_array("soundingRS-UL-PeriodicConfigDedicatedList-r14");
      for (const auto& e1 : *srs_ul_periodic_cfg_ded_list_r14) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (srs_ul_periodic_cfg_ded_up_pts_ext_list_r14.is_present()) {
      j.start_array("soundingRS-UL-PeriodicConfigDedicatedUpPTsExtList-r14");
      for (const auto& e1 : *srs_ul_periodic_cfg_ded_up_pts_ext_list_r14) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (srs_ul_aperiodic_cfg_ded_list_r14.is_present()) {
      j.start_array("soundingRS-UL-AperiodicConfigDedicatedList-r14");
      for (const auto& e1 : *srs_ul_aperiodic_cfg_ded_list_r14) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (srs_ul_cfg_ded_ap_up_pts_ext_list_r14.is_present()) {
      j.start_array("soundingRS-UL-ConfigDedicatedApUpPTsExtList-r14");
      for (const auto& e1 : *srs_ul_cfg_ded_ap_up_pts_ext_list_r14) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (must_cfg_r14.is_present()) {
      j.write_fieldname("must-Config-r14");
      must_cfg_r14->to_json(j);
    }
    if (pusch_cfg_ded_v1430.is_present()) {
      j.write_fieldname("pusch-ConfigDedicated-v1430");
      pusch_cfg_ded_v1430->to_json(j);
    }
    if (csi_rs_cfg_v1430.is_present()) {
      j.write_fieldname("csi-RS-Config-v1430");
      csi_rs_cfg_v1430->to_json(j);
    }
    if (csi_rs_cfg_zp_ap_list_r14.is_present()) {
      j.write_fieldname("csi-RS-ConfigZP-ApList-r14");
      csi_rs_cfg_zp_ap_list_r14->to_json(j);
    }
    if (cqi_report_cfg_v1430.is_present()) {
      j.write_fieldname("cqi-ReportConfig-v1430");
      cqi_report_cfg_v1430->to_json(j);
    }
    if (semi_open_loop_r14_present) {
      j.write_bool("semiOpenLoop-r14", semi_open_loop_r14);
    }
    if (pdsch_cfg_ded_scell_v1430.is_present()) {
      j.write_fieldname("pdsch-ConfigDedicatedSCell-v1430");
      pdsch_cfg_ded_scell_v1430->to_json(j);
    }
    if (csi_rs_cfg_v1480.is_present()) {
      j.write_fieldname("csi-RS-Config-v1480");
      csi_rs_cfg_v1480->to_json(j);
    }
    if (phys_cfg_ded_stti_r15.is_present()) {
      j.write_fieldname("physicalConfigDedicatedSTTI-r15");
      phys_cfg_ded_stti_r15->to_json(j);
    }
    if (pdsch_cfg_ded_v1530.is_present()) {
      j.write_fieldname("pdsch-ConfigDedicated-v1530");
      pdsch_cfg_ded_v1530->to_json(j);
    }
    if (dummy.is_present()) {
      j.write_fieldname("dummy");
      dummy->to_json(j);
    }
    if (cqi_report_cfg_scell_r15.is_present()) {
      j.write_fieldname("cqi-ReportConfigSCell-r15");
      cqi_report_cfg_scell_r15->to_json(j);
    }
    if (cqi_short_cfg_scell_r15.is_present()) {
      j.write_fieldname("cqi-ShortConfigSCell-r15");
      cqi_short_cfg_scell_r15->to_json(j);
    }
    if (csi_rs_cfg_v1530.is_present()) {
      j.write_fieldname("csi-RS-Config-v1530");
      csi_rs_cfg_v1530->to_json(j);
    }
    if (ul_pwr_ctrl_ded_scell_v1530.is_present()) {
      j.write_fieldname("uplinkPowerControlDedicatedSCell-v1530");
      ul_pwr_ctrl_ded_scell_v1530->to_json(j);
    }
    if (laa_scell_cfg_v1530.is_present()) {
      j.write_fieldname("laa-SCellConfiguration-v1530");
      laa_scell_cfg_v1530->to_json(j);
    }
    if (pusch_cfg_ded_v1530.is_present()) {
      j.write_fieldname("pusch-ConfigDedicated-v1530");
      pusch_cfg_ded_v1530->to_json(j);
    }
    if (semi_static_cfi_cfg_r15.is_present()) {
      j.write_fieldname("semiStaticCFI-Config-r15");
      semi_static_cfi_cfg_r15->to_json(j);
    }
    if (blind_pdsch_repeat_cfg_r15.is_present()) {
      j.write_fieldname("blindPDSCH-Repetition-Config-r15");
      blind_pdsch_repeat_cfg_r15->to_json(j);
    }
    if (spucch_cfg_v1550.is_present()) {
      j.write_fieldname("spucch-Config-v1550");
      spucch_cfg_v1550->to_json(j);
    }
    if (srs_ul_cfg_ded_add_r16.is_present()) {
      j.write_fieldname("soundingRS-UL-ConfigDedicatedAdd-r16");
      srs_ul_cfg_ded_add_r16->to_json(j);
    }
    if (ul_pwr_ctrl_add_srs_r16.is_present()) {
      j.write_fieldname("uplinkPowerControlAddSRS-r16");
      ul_pwr_ctrl_add_srs_r16->to_json(j);
    }
    if (srs_virtual_cell_id_r16.is_present()) {
      j.write_fieldname("soundingRS-VirtualCellID-r16");
      srs_virtual_cell_id_r16->to_json(j);
    }
    if (wideband_prg_r16.is_present()) {
      j.write_fieldname("widebandPRG-r16");
      wideband_prg_r16->to_json(j);
    }
  }
  j.end_obj();
}
bool phys_cfg_ded_scell_r10_s::operator==(const phys_cfg_ded_scell_r10_s& other) const
{
  return ext == other.ext and non_ul_cfg_r10.ant_info_r10_present == other.non_ul_cfg_r10.ant_info_r10_present and
         (not non_ul_cfg_r10.ant_info_r10_present or
          non_ul_cfg_r10.ant_info_r10 == other.non_ul_cfg_r10.ant_info_r10) and
         non_ul_cfg_r10.cross_carrier_sched_cfg_r10_present ==
             other.non_ul_cfg_r10.cross_carrier_sched_cfg_r10_present and
         (not non_ul_cfg_r10.cross_carrier_sched_cfg_r10_present or
          non_ul_cfg_r10.cross_carrier_sched_cfg_r10 == other.non_ul_cfg_r10.cross_carrier_sched_cfg_r10) and
         non_ul_cfg_r10.csi_rs_cfg_r10_present == other.non_ul_cfg_r10.csi_rs_cfg_r10_present and
         (not non_ul_cfg_r10.csi_rs_cfg_r10_present or
          non_ul_cfg_r10.csi_rs_cfg_r10 == other.non_ul_cfg_r10.csi_rs_cfg_r10) and
         non_ul_cfg_r10.pdsch_cfg_ded_r10_present == other.non_ul_cfg_r10.pdsch_cfg_ded_r10_present and
         (not non_ul_cfg_r10.pdsch_cfg_ded_r10_present or
          non_ul_cfg_r10.pdsch_cfg_ded_r10 == other.non_ul_cfg_r10.pdsch_cfg_ded_r10) and
         ul_cfg_r10.ant_info_ul_r10_present == other.ul_cfg_r10.ant_info_ul_r10_present and
         (not ul_cfg_r10.ant_info_ul_r10_present or ul_cfg_r10.ant_info_ul_r10 == other.ul_cfg_r10.ant_info_ul_r10) and
         ul_cfg_r10.pusch_cfg_ded_scell_r10_present == other.ul_cfg_r10.pusch_cfg_ded_scell_r10_present and
         (not ul_cfg_r10.pusch_cfg_ded_scell_r10_present or
          ul_cfg_r10.pusch_cfg_ded_scell_r10 == other.ul_cfg_r10.pusch_cfg_ded_scell_r10) and
         ul_cfg_r10.ul_pwr_ctrl_ded_scell_r10_present == other.ul_cfg_r10.ul_pwr_ctrl_ded_scell_r10_present and
         (not ul_cfg_r10.ul_pwr_ctrl_ded_scell_r10_present or
          ul_cfg_r10.ul_pwr_ctrl_ded_scell_r10 == other.ul_cfg_r10.ul_pwr_ctrl_ded_scell_r10) and
         ul_cfg_r10.cqi_report_cfg_scell_r10_present == other.ul_cfg_r10.cqi_report_cfg_scell_r10_present and
         (not ul_cfg_r10.cqi_report_cfg_scell_r10_present or
          ul_cfg_r10.cqi_report_cfg_scell_r10 == other.ul_cfg_r10.cqi_report_cfg_scell_r10) and
         ul_cfg_r10.srs_ul_cfg_ded_r10_present == other.ul_cfg_r10.srs_ul_cfg_ded_r10_present and
         (not ul_cfg_r10.srs_ul_cfg_ded_r10_present or
          ul_cfg_r10.srs_ul_cfg_ded_r10 == other.ul_cfg_r10.srs_ul_cfg_ded_r10) and
         ul_cfg_r10.srs_ul_cfg_ded_v1020_present == other.ul_cfg_r10.srs_ul_cfg_ded_v1020_present and
         (not ul_cfg_r10.srs_ul_cfg_ded_v1020_present or
          ul_cfg_r10.srs_ul_cfg_ded_v1020 == other.ul_cfg_r10.srs_ul_cfg_ded_v1020) and
         ul_cfg_r10.srs_ul_cfg_ded_aperiodic_r10_present == other.ul_cfg_r10.srs_ul_cfg_ded_aperiodic_r10_present and
         (not ul_cfg_r10.srs_ul_cfg_ded_aperiodic_r10_present or
          ul_cfg_r10.srs_ul_cfg_ded_aperiodic_r10 == other.ul_cfg_r10.srs_ul_cfg_ded_aperiodic_r10) and
         (not ext or
          (csi_rs_cfg_nzp_to_release_list_r11.is_present() == other.csi_rs_cfg_nzp_to_release_list_r11.is_present() and
           (not csi_rs_cfg_nzp_to_release_list_r11.is_present() or
            *csi_rs_cfg_nzp_to_release_list_r11 == *other.csi_rs_cfg_nzp_to_release_list_r11) and
           csi_rs_cfg_nzp_to_add_mod_list_r11.is_present() == other.csi_rs_cfg_nzp_to_add_mod_list_r11.is_present() and
           (not csi_rs_cfg_nzp_to_add_mod_list_r11.is_present() or
            *csi_rs_cfg_nzp_to_add_mod_list_r11 == *other.csi_rs_cfg_nzp_to_add_mod_list_r11) and
           csi_rs_cfg_zp_to_release_list_r11.is_present() == other.csi_rs_cfg_zp_to_release_list_r11.is_present() and
           (not csi_rs_cfg_zp_to_release_list_r11.is_present() or
            *csi_rs_cfg_zp_to_release_list_r11 == *other.csi_rs_cfg_zp_to_release_list_r11) and
           csi_rs_cfg_zp_to_add_mod_list_r11.is_present() == other.csi_rs_cfg_zp_to_add_mod_list_r11.is_present() and
           (not csi_rs_cfg_zp_to_add_mod_list_r11.is_present() or
            *csi_rs_cfg_zp_to_add_mod_list_r11 == *other.csi_rs_cfg_zp_to_add_mod_list_r11) and
           epdcch_cfg_r11.is_present() == other.epdcch_cfg_r11.is_present() and
           (not epdcch_cfg_r11.is_present() or *epdcch_cfg_r11 == *other.epdcch_cfg_r11) and
           pdsch_cfg_ded_v1130.is_present() == other.pdsch_cfg_ded_v1130.is_present() and
           (not pdsch_cfg_ded_v1130.is_present() or *pdsch_cfg_ded_v1130 == *other.pdsch_cfg_ded_v1130) and
           cqi_report_cfg_v1130.is_present() == other.cqi_report_cfg_v1130.is_present() and
           (not cqi_report_cfg_v1130.is_present() or *cqi_report_cfg_v1130 == *other.cqi_report_cfg_v1130) and
           pusch_cfg_ded_v1130.is_present() == other.pusch_cfg_ded_v1130.is_present() and
           (not pusch_cfg_ded_v1130.is_present() or *pusch_cfg_ded_v1130 == *other.pusch_cfg_ded_v1130) and
           ul_pwr_ctrl_ded_scell_v1130.is_present() == other.ul_pwr_ctrl_ded_scell_v1130.is_present() and
           (not ul_pwr_ctrl_ded_scell_v1130.is_present() or
            *ul_pwr_ctrl_ded_scell_v1130 == *other.ul_pwr_ctrl_ded_scell_v1130) and
           ant_info_v1250.is_present() == other.ant_info_v1250.is_present() and
           (not ant_info_v1250.is_present() or *ant_info_v1250 == *other.ant_info_v1250) and
           eimta_main_cfg_scell_r12.is_present() == other.eimta_main_cfg_scell_r12.is_present() and
           (not eimta_main_cfg_scell_r12.is_present() or
            *eimta_main_cfg_scell_r12 == *other.eimta_main_cfg_scell_r12) and
           cqi_report_cfg_scell_v1250.is_present() == other.cqi_report_cfg_scell_v1250.is_present() and
           (not cqi_report_cfg_scell_v1250.is_present() or
            *cqi_report_cfg_scell_v1250 == *other.cqi_report_cfg_scell_v1250) and
           ul_pwr_ctrl_ded_scell_v1250.is_present() == other.ul_pwr_ctrl_ded_scell_v1250.is_present() and
           (not ul_pwr_ctrl_ded_scell_v1250.is_present() or
            *ul_pwr_ctrl_ded_scell_v1250 == *other.ul_pwr_ctrl_ded_scell_v1250) and
           csi_rs_cfg_v1250.is_present() == other.csi_rs_cfg_v1250.is_present() and
           (not csi_rs_cfg_v1250.is_present() or *csi_rs_cfg_v1250 == *other.csi_rs_cfg_v1250) and
           pdsch_cfg_ded_v1280.is_present() == other.pdsch_cfg_ded_v1280.is_present() and
           (not pdsch_cfg_ded_v1280.is_present() or *pdsch_cfg_ded_v1280 == *other.pdsch_cfg_ded_v1280) and
           pucch_cell_r13_present == other.pucch_cell_r13_present and
           pucch_scell.is_present() == other.pucch_scell.is_present() and
           (not pucch_scell.is_present() or *pucch_scell == *other.pucch_scell) and
           cross_carrier_sched_cfg_r13.is_present() == other.cross_carrier_sched_cfg_r13.is_present() and
           (not cross_carrier_sched_cfg_r13.is_present() or
            *cross_carrier_sched_cfg_r13 == *other.cross_carrier_sched_cfg_r13) and
           pdcch_cfg_scell_r13.is_present() == other.pdcch_cfg_scell_r13.is_present() and
           (not pdcch_cfg_scell_r13.is_present() or *pdcch_cfg_scell_r13 == *other.pdcch_cfg_scell_r13) and
           cqi_report_cfg_v1310.is_present() == other.cqi_report_cfg_v1310.is_present() and
           (not cqi_report_cfg_v1310.is_present() or *cqi_report_cfg_v1310 == *other.cqi_report_cfg_v1310) and
           pdsch_cfg_ded_v1310.is_present() == other.pdsch_cfg_ded_v1310.is_present() and
           (not pdsch_cfg_ded_v1310.is_present() or *pdsch_cfg_ded_v1310 == *other.pdsch_cfg_ded_v1310) and
           srs_ul_cfg_ded_v1310.is_present() == other.srs_ul_cfg_ded_v1310.is_present() and
           (not srs_ul_cfg_ded_v1310.is_present() or *srs_ul_cfg_ded_v1310 == *other.srs_ul_cfg_ded_v1310) and
           srs_ul_cfg_ded_up_pts_ext_r13.is_present() == other.srs_ul_cfg_ded_up_pts_ext_r13.is_present() and
           (not srs_ul_cfg_ded_up_pts_ext_r13.is_present() or
            *srs_ul_cfg_ded_up_pts_ext_r13 == *other.srs_ul_cfg_ded_up_pts_ext_r13) and
           srs_ul_cfg_ded_aperiodic_v1310.is_present() == other.srs_ul_cfg_ded_aperiodic_v1310.is_present() and
           (not srs_ul_cfg_ded_aperiodic_v1310.is_present() or
            *srs_ul_cfg_ded_aperiodic_v1310 == *other.srs_ul_cfg_ded_aperiodic_v1310) and
           srs_ul_cfg_ded_aperiodic_up_pts_ext_r13.is_present() ==
               other.srs_ul_cfg_ded_aperiodic_up_pts_ext_r13.is_present() and
           (not srs_ul_cfg_ded_aperiodic_up_pts_ext_r13.is_present() or
            *srs_ul_cfg_ded_aperiodic_up_pts_ext_r13 == *other.srs_ul_cfg_ded_aperiodic_up_pts_ext_r13) and
           csi_rs_cfg_v1310.is_present() == other.csi_rs_cfg_v1310.is_present() and
           (not csi_rs_cfg_v1310.is_present() or *csi_rs_cfg_v1310 == *other.csi_rs_cfg_v1310) and
           laa_scell_cfg_r13.is_present() == other.laa_scell_cfg_r13.is_present() and
           (not laa_scell_cfg_r13.is_present() or *laa_scell_cfg_r13 == *other.laa_scell_cfg_r13) and
           csi_rs_cfg_nzp_to_add_mod_list_ext_r13.is_present() ==
               other.csi_rs_cfg_nzp_to_add_mod_list_ext_r13.is_present() and
           (not csi_rs_cfg_nzp_to_add_mod_list_ext_r13.is_present() or
            *csi_rs_cfg_nzp_to_add_mod_list_ext_r13 == *other.csi_rs_cfg_nzp_to_add_mod_list_ext_r13) and
           csi_rs_cfg_nzp_to_release_list_ext_r13.is_present() ==
               other.csi_rs_cfg_nzp_to_release_list_ext_r13.is_present() and
           (not csi_rs_cfg_nzp_to_release_list_ext_r13.is_present() or
            *csi_rs_cfg_nzp_to_release_list_ext_r13 == *other.csi_rs_cfg_nzp_to_release_list_ext_r13) and
           cqi_report_cfg_v1320.is_present() == other.cqi_report_cfg_v1320.is_present() and
           (not cqi_report_cfg_v1320.is_present() or *cqi_report_cfg_v1320 == *other.cqi_report_cfg_v1320) and
           laa_scell_cfg_v1430.is_present() == other.laa_scell_cfg_v1430.is_present() and
           (not laa_scell_cfg_v1430.is_present() or *laa_scell_cfg_v1430 == *other.laa_scell_cfg_v1430) and
           type_b_srs_tpc_pdcch_cfg_r14.is_present() == other.type_b_srs_tpc_pdcch_cfg_r14.is_present() and
           (not type_b_srs_tpc_pdcch_cfg_r14.is_present() or
            *type_b_srs_tpc_pdcch_cfg_r14 == *other.type_b_srs_tpc_pdcch_cfg_r14) and
           ul_pusch_less_pwr_ctrl_ded_v1430.is_present() == other.ul_pusch_less_pwr_ctrl_ded_v1430.is_present() and
           (not ul_pusch_less_pwr_ctrl_ded_v1430.is_present() or
            *ul_pusch_less_pwr_ctrl_ded_v1430 == *other.ul_pusch_less_pwr_ctrl_ded_v1430) and
           srs_ul_periodic_cfg_ded_list_r14.is_present() == other.srs_ul_periodic_cfg_ded_list_r14.is_present() and
           (not srs_ul_periodic_cfg_ded_list_r14.is_present() or
            *srs_ul_periodic_cfg_ded_list_r14 == *other.srs_ul_periodic_cfg_ded_list_r14) and
           srs_ul_periodic_cfg_ded_up_pts_ext_list_r14.is_present() ==
               other.srs_ul_periodic_cfg_ded_up_pts_ext_list_r14.is_present() and
           (not srs_ul_periodic_cfg_ded_up_pts_ext_list_r14.is_present() or
            *srs_ul_periodic_cfg_ded_up_pts_ext_list_r14 == *other.srs_ul_periodic_cfg_ded_up_pts_ext_list_r14) and
           srs_ul_aperiodic_cfg_ded_list_r14.is_present() == other.srs_ul_aperiodic_cfg_ded_list_r14.is_present() and
           (not srs_ul_aperiodic_cfg_ded_list_r14.is_present() or
            *srs_ul_aperiodic_cfg_ded_list_r14 == *other.srs_ul_aperiodic_cfg_ded_list_r14) and
           srs_ul_cfg_ded_ap_up_pts_ext_list_r14.is_present() ==
               other.srs_ul_cfg_ded_ap_up_pts_ext_list_r14.is_present() and
           (not srs_ul_cfg_ded_ap_up_pts_ext_list_r14.is_present() or
            *srs_ul_cfg_ded_ap_up_pts_ext_list_r14 == *other.srs_ul_cfg_ded_ap_up_pts_ext_list_r14) and
           must_cfg_r14.is_present() == other.must_cfg_r14.is_present() and
           (not must_cfg_r14.is_present() or *must_cfg_r14 == *other.must_cfg_r14) and
           pusch_cfg_ded_v1430.is_present() == other.pusch_cfg_ded_v1430.is_present() and
           (not pusch_cfg_ded_v1430.is_present() or *pusch_cfg_ded_v1430 == *other.pusch_cfg_ded_v1430) and
           csi_rs_cfg_v1430.is_present() == other.csi_rs_cfg_v1430.is_present() and
           (not csi_rs_cfg_v1430.is_present() or *csi_rs_cfg_v1430 == *other.csi_rs_cfg_v1430) and
           csi_rs_cfg_zp_ap_list_r14.is_present() == other.csi_rs_cfg_zp_ap_list_r14.is_present() and
           (not csi_rs_cfg_zp_ap_list_r14.is_present() or
            *csi_rs_cfg_zp_ap_list_r14 == *other.csi_rs_cfg_zp_ap_list_r14) and
           cqi_report_cfg_v1430.is_present() == other.cqi_report_cfg_v1430.is_present() and
           (not cqi_report_cfg_v1430.is_present() or *cqi_report_cfg_v1430 == *other.cqi_report_cfg_v1430) and
           semi_open_loop_r14_present == other.semi_open_loop_r14_present and
           (not semi_open_loop_r14_present or semi_open_loop_r14 == other.semi_open_loop_r14) and
           pdsch_cfg_ded_scell_v1430.is_present() == other.pdsch_cfg_ded_scell_v1430.is_present() and
           (not pdsch_cfg_ded_scell_v1430.is_present() or
            *pdsch_cfg_ded_scell_v1430 == *other.pdsch_cfg_ded_scell_v1430) and
           csi_rs_cfg_v1480.is_present() == other.csi_rs_cfg_v1480.is_present() and
           (not csi_rs_cfg_v1480.is_present() or *csi_rs_cfg_v1480 == *other.csi_rs_cfg_v1480) and
           phys_cfg_ded_stti_r15.is_present() == other.phys_cfg_ded_stti_r15.is_present() and
           (not phys_cfg_ded_stti_r15.is_present() or *phys_cfg_ded_stti_r15 == *other.phys_cfg_ded_stti_r15) and
           pdsch_cfg_ded_v1530.is_present() == other.pdsch_cfg_ded_v1530.is_present() and
           (not pdsch_cfg_ded_v1530.is_present() or *pdsch_cfg_ded_v1530 == *other.pdsch_cfg_ded_v1530) and
           dummy.is_present() == other.dummy.is_present() and (not dummy.is_present() or *dummy == *other.dummy) and
           cqi_report_cfg_scell_r15.is_present() == other.cqi_report_cfg_scell_r15.is_present() and
           (not cqi_report_cfg_scell_r15.is_present() or
            *cqi_report_cfg_scell_r15 == *other.cqi_report_cfg_scell_r15) and
           cqi_short_cfg_scell_r15.is_present() == other.cqi_short_cfg_scell_r15.is_present() and
           (not cqi_short_cfg_scell_r15.is_present() or *cqi_short_cfg_scell_r15 == *other.cqi_short_cfg_scell_r15) and
           csi_rs_cfg_v1530.is_present() == other.csi_rs_cfg_v1530.is_present() and
           (not csi_rs_cfg_v1530.is_present() or *csi_rs_cfg_v1530 == *other.csi_rs_cfg_v1530) and
           ul_pwr_ctrl_ded_scell_v1530.is_present() == other.ul_pwr_ctrl_ded_scell_v1530.is_present() and
           (not ul_pwr_ctrl_ded_scell_v1530.is_present() or
            *ul_pwr_ctrl_ded_scell_v1530 == *other.ul_pwr_ctrl_ded_scell_v1530) and
           laa_scell_cfg_v1530.is_present() == other.laa_scell_cfg_v1530.is_present() and
           (not laa_scell_cfg_v1530.is_present() or *laa_scell_cfg_v1530 == *other.laa_scell_cfg_v1530) and
           pusch_cfg_ded_v1530.is_present() == other.pusch_cfg_ded_v1530.is_present() and
           (not pusch_cfg_ded_v1530.is_present() or *pusch_cfg_ded_v1530 == *other.pusch_cfg_ded_v1530) and
           semi_static_cfi_cfg_r15.is_present() == other.semi_static_cfi_cfg_r15.is_present() and
           (not semi_static_cfi_cfg_r15.is_present() or *semi_static_cfi_cfg_r15 == *other.semi_static_cfi_cfg_r15) and
           blind_pdsch_repeat_cfg_r15.is_present() == other.blind_pdsch_repeat_cfg_r15.is_present() and
           (not blind_pdsch_repeat_cfg_r15.is_present() or
            *blind_pdsch_repeat_cfg_r15 == *other.blind_pdsch_repeat_cfg_r15) and
           spucch_cfg_v1550.is_present() == other.spucch_cfg_v1550.is_present() and
           (not spucch_cfg_v1550.is_present() or *spucch_cfg_v1550 == *other.spucch_cfg_v1550) and
           srs_ul_cfg_ded_add_r16.is_present() == other.srs_ul_cfg_ded_add_r16.is_present() and
           (not srs_ul_cfg_ded_add_r16.is_present() or *srs_ul_cfg_ded_add_r16 == *other.srs_ul_cfg_ded_add_r16) and
           ul_pwr_ctrl_add_srs_r16.is_present() == other.ul_pwr_ctrl_add_srs_r16.is_present() and
           (not ul_pwr_ctrl_add_srs_r16.is_present() or *ul_pwr_ctrl_add_srs_r16 == *other.ul_pwr_ctrl_add_srs_r16) and
           srs_virtual_cell_id_r16.is_present() == other.srs_virtual_cell_id_r16.is_present() and
           (not srs_virtual_cell_id_r16.is_present() or *srs_virtual_cell_id_r16 == *other.srs_virtual_cell_id_r16) and
           wideband_prg_r16.is_present() == other.wideband_prg_r16.is_present() and
           (not wideband_prg_r16.is_present() or *wideband_prg_r16 == *other.wideband_prg_r16)));
}

void phys_cfg_ded_scell_r10_s::pucch_scell_c_::set(types::options e)
{
  type_ = e;
}
void phys_cfg_ded_scell_r10_s::pucch_scell_c_::set_release()
{
  set(types::release);
}
phys_cfg_ded_scell_r10_s::pucch_scell_c_::setup_s_& phys_cfg_ded_scell_r10_s::pucch_scell_c_::set_setup()
{
  set(types::setup);
  return c;
}
void phys_cfg_ded_scell_r10_s::pucch_scell_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      if (c.pucch_cfg_ded_r13_present) {
        j.write_fieldname("pucch-ConfigDedicated-r13");
        c.pucch_cfg_ded_r13.to_json(j);
      }
      if (c.sched_request_cfg_r13_present) {
        j.write_fieldname("schedulingRequestConfig-r13");
        c.sched_request_cfg_r13.to_json(j);
      }
      if (c.tpc_pdcch_cfg_pucch_scell_r13_present) {
        j.write_fieldname("tpc-PDCCH-ConfigPUCCH-SCell-r13");
        c.tpc_pdcch_cfg_pucch_scell_r13.to_json(j);
      }
      if (c.pusch_cfg_ded_r13_present) {
        j.write_fieldname("pusch-ConfigDedicated-r13");
        c.pusch_cfg_ded_r13.to_json(j);
      }
      if (c.ul_pwr_ctrl_ded_r13_present) {
        j.write_fieldname("uplinkPowerControlDedicated-r13");
        c.ul_pwr_ctrl_ded_r13.to_json(j);
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_scell_r10_s::pucch_scell_c_");
  }
  j.end_obj();
}
SRSASN_CODE phys_cfg_ded_scell_r10_s::pucch_scell_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.pucch_cfg_ded_r13_present, 1));
      HANDLE_CODE(bref.pack(c.sched_request_cfg_r13_present, 1));
      HANDLE_CODE(bref.pack(c.tpc_pdcch_cfg_pucch_scell_r13_present, 1));
      HANDLE_CODE(bref.pack(c.pusch_cfg_ded_r13_present, 1));
      HANDLE_CODE(bref.pack(c.ul_pwr_ctrl_ded_r13_present, 1));
      if (c.pucch_cfg_ded_r13_present) {
        HANDLE_CODE(c.pucch_cfg_ded_r13.pack(bref));
      }
      if (c.sched_request_cfg_r13_present) {
        HANDLE_CODE(c.sched_request_cfg_r13.pack(bref));
      }
      if (c.tpc_pdcch_cfg_pucch_scell_r13_present) {
        HANDLE_CODE(c.tpc_pdcch_cfg_pucch_scell_r13.pack(bref));
      }
      if (c.pusch_cfg_ded_r13_present) {
        HANDLE_CODE(c.pusch_cfg_ded_r13.pack(bref));
      }
      if (c.ul_pwr_ctrl_ded_r13_present) {
        HANDLE_CODE(c.ul_pwr_ctrl_ded_r13.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_scell_r10_s::pucch_scell_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE phys_cfg_ded_scell_r10_s::pucch_scell_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.pucch_cfg_ded_r13_present, 1));
      HANDLE_CODE(bref.unpack(c.sched_request_cfg_r13_present, 1));
      HANDLE_CODE(bref.unpack(c.tpc_pdcch_cfg_pucch_scell_r13_present, 1));
      HANDLE_CODE(bref.unpack(c.pusch_cfg_ded_r13_present, 1));
      HANDLE_CODE(bref.unpack(c.ul_pwr_ctrl_ded_r13_present, 1));
      if (c.pucch_cfg_ded_r13_present) {
        HANDLE_CODE(c.pucch_cfg_ded_r13.unpack(bref));
      }
      if (c.sched_request_cfg_r13_present) {
        HANDLE_CODE(c.sched_request_cfg_r13.unpack(bref));
      }
      if (c.tpc_pdcch_cfg_pucch_scell_r13_present) {
        HANDLE_CODE(c.tpc_pdcch_cfg_pucch_scell_r13.unpack(bref));
      }
      if (c.pusch_cfg_ded_r13_present) {
        HANDLE_CODE(c.pusch_cfg_ded_r13.unpack(bref));
      }
      if (c.ul_pwr_ctrl_ded_r13_present) {
        HANDLE_CODE(c.ul_pwr_ctrl_ded_r13.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_scell_r10_s::pucch_scell_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool phys_cfg_ded_scell_r10_s::pucch_scell_c_::operator==(const pucch_scell_c_& other) const
{
  return type() == other.type() and c.pucch_cfg_ded_r13_present == other.c.pucch_cfg_ded_r13_present and
         (not c.pucch_cfg_ded_r13_present or c.pucch_cfg_ded_r13 == other.c.pucch_cfg_ded_r13) and
         c.sched_request_cfg_r13_present == other.c.sched_request_cfg_r13_present and
         (not c.sched_request_cfg_r13_present or c.sched_request_cfg_r13 == other.c.sched_request_cfg_r13) and
         c.tpc_pdcch_cfg_pucch_scell_r13_present == other.c.tpc_pdcch_cfg_pucch_scell_r13_present and
         (not c.tpc_pdcch_cfg_pucch_scell_r13_present or
          c.tpc_pdcch_cfg_pucch_scell_r13 == other.c.tpc_pdcch_cfg_pucch_scell_r13) and
         c.pusch_cfg_ded_r13_present == other.c.pusch_cfg_ded_r13_present and
         (not c.pusch_cfg_ded_r13_present or c.pusch_cfg_ded_r13 == other.c.pusch_cfg_ded_r13) and
         c.ul_pwr_ctrl_ded_r13_present == other.c.ul_pwr_ctrl_ded_r13_present and
         (not c.ul_pwr_ctrl_ded_r13_present or c.ul_pwr_ctrl_ded_r13 == other.c.ul_pwr_ctrl_ded_r13);
}

void phys_cfg_ded_scell_r10_s::must_cfg_r14_c_::set(types::options e)
{
  type_ = e;
}
void phys_cfg_ded_scell_r10_s::must_cfg_r14_c_::set_release()
{
  set(types::release);
}
phys_cfg_ded_scell_r10_s::must_cfg_r14_c_::setup_s_& phys_cfg_ded_scell_r10_s::must_cfg_r14_c_::set_setup()
{
  set(types::setup);
  return c;
}
void phys_cfg_ded_scell_r10_s::must_cfg_r14_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_str("k-max-r14", c.k_max_r14.to_string());
      if (c.p_a_must_r14_present) {
        j.write_str("p-a-must-r14", c.p_a_must_r14.to_string());
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_scell_r10_s::must_cfg_r14_c_");
  }
  j.end_obj();
}
SRSASN_CODE phys_cfg_ded_scell_r10_s::must_cfg_r14_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.p_a_must_r14_present, 1));
      HANDLE_CODE(c.k_max_r14.pack(bref));
      if (c.p_a_must_r14_present) {
        HANDLE_CODE(c.p_a_must_r14.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_scell_r10_s::must_cfg_r14_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE phys_cfg_ded_scell_r10_s::must_cfg_r14_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.p_a_must_r14_present, 1));
      HANDLE_CODE(c.k_max_r14.unpack(bref));
      if (c.p_a_must_r14_present) {
        HANDLE_CODE(c.p_a_must_r14.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_scell_r10_s::must_cfg_r14_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool phys_cfg_ded_scell_r10_s::must_cfg_r14_c_::operator==(const must_cfg_r14_c_& other) const
{
  return type() == other.type() and c.k_max_r14 == other.c.k_max_r14 and
         c.p_a_must_r14_present == other.c.p_a_must_r14_present and
         (not c.p_a_must_r14_present or c.p_a_must_r14 == other.c.p_a_must_r14);
}

const char* phys_cfg_ded_scell_r10_s::must_cfg_r14_c_::setup_s_::k_max_r14_opts::to_string() const
{
  static const char* options[] = {"l1", "l3"};
  return convert_enum_idx(options, 2, value, "phys_cfg_ded_scell_r10_s::must_cfg_r14_c_::setup_s_::k_max_r14_e_");
}
uint8_t phys_cfg_ded_scell_r10_s::must_cfg_r14_c_::setup_s_::k_max_r14_opts::to_number() const
{
  static const uint8_t options[] = {1, 3};
  return map_enum_number(options, 2, value, "phys_cfg_ded_scell_r10_s::must_cfg_r14_c_::setup_s_::k_max_r14_e_");
}

const char* phys_cfg_ded_scell_r10_s::must_cfg_r14_c_::setup_s_::p_a_must_r14_opts::to_string() const
{
  static const char* options[] = {"dB-6", "dB-4dot77", "dB-3", "dB-1dot77", "dB0", "dB1", "dB2", "dB3"};
  return convert_enum_idx(options, 8, value, "phys_cfg_ded_scell_r10_s::must_cfg_r14_c_::setup_s_::p_a_must_r14_e_");
}
float phys_cfg_ded_scell_r10_s::must_cfg_r14_c_::setup_s_::p_a_must_r14_opts::to_number() const
{
  static const float options[] = {-6.0, -4.77, -3.0, -1.77, 0.0, 1.0, 2.0, 3.0};
  return map_enum_number(options, 8, value, "phys_cfg_ded_scell_r10_s::must_cfg_r14_c_::setup_s_::p_a_must_r14_e_");
}
const char* phys_cfg_ded_scell_r10_s::must_cfg_r14_c_::setup_s_::p_a_must_r14_opts::to_number_string() const
{
  static const char* options[] = {"-6", "-4.77", "-3", "-1.77", "0", "1", "2", "3"};
  return convert_enum_idx(options, 8, value, "phys_cfg_ded_scell_r10_s::must_cfg_r14_c_::setup_s_::p_a_must_r14_e_");
}

void phys_cfg_ded_scell_r10_s::semi_static_cfi_cfg_r15_c_::set(types::options e)
{
  type_ = e;
}
void phys_cfg_ded_scell_r10_s::semi_static_cfi_cfg_r15_c_::set_release()
{
  set(types::release);
}
phys_cfg_ded_scell_r10_s::semi_static_cfi_cfg_r15_c_::setup_c_&
phys_cfg_ded_scell_r10_s::semi_static_cfi_cfg_r15_c_::set_setup()
{
  set(types::setup);
  return c;
}
void phys_cfg_ded_scell_r10_s::semi_static_cfi_cfg_r15_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      c.to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_scell_r10_s::semi_static_cfi_cfg_r15_c_");
  }
  j.end_obj();
}
SRSASN_CODE phys_cfg_ded_scell_r10_s::semi_static_cfi_cfg_r15_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_scell_r10_s::semi_static_cfi_cfg_r15_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE phys_cfg_ded_scell_r10_s::semi_static_cfi_cfg_r15_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_scell_r10_s::semi_static_cfi_cfg_r15_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool phys_cfg_ded_scell_r10_s::semi_static_cfi_cfg_r15_c_::operator==(const semi_static_cfi_cfg_r15_c_& other) const
{
  return type() == other.type() and c == other.c;
}

void phys_cfg_ded_scell_r10_s::semi_static_cfi_cfg_r15_c_::setup_c_::destroy_()
{
  switch (type_) {
    case types::cfi_cfg_r15:
      c.destroy<cfi_cfg_r15_s>();
      break;
    case types::cfi_pattern_cfg_r15:
      c.destroy<cfi_pattern_cfg_r15_s>();
      break;
    default:
      break;
  }
}
void phys_cfg_ded_scell_r10_s::semi_static_cfi_cfg_r15_c_::setup_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::cfi_cfg_r15:
      c.init<cfi_cfg_r15_s>();
      break;
    case types::cfi_pattern_cfg_r15:
      c.init<cfi_pattern_cfg_r15_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_scell_r10_s::semi_static_cfi_cfg_r15_c_::setup_c_");
  }
}
phys_cfg_ded_scell_r10_s::semi_static_cfi_cfg_r15_c_::setup_c_::setup_c_(
    const phys_cfg_ded_scell_r10_s::semi_static_cfi_cfg_r15_c_::setup_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::cfi_cfg_r15:
      c.init(other.c.get<cfi_cfg_r15_s>());
      break;
    case types::cfi_pattern_cfg_r15:
      c.init(other.c.get<cfi_pattern_cfg_r15_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_scell_r10_s::semi_static_cfi_cfg_r15_c_::setup_c_");
  }
}
phys_cfg_ded_scell_r10_s::semi_static_cfi_cfg_r15_c_::setup_c_&
phys_cfg_ded_scell_r10_s::semi_static_cfi_cfg_r15_c_::setup_c_::operator=(
    const phys_cfg_ded_scell_r10_s::semi_static_cfi_cfg_r15_c_::setup_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::cfi_cfg_r15:
      c.set(other.c.get<cfi_cfg_r15_s>());
      break;
    case types::cfi_pattern_cfg_r15:
      c.set(other.c.get<cfi_pattern_cfg_r15_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_scell_r10_s::semi_static_cfi_cfg_r15_c_::setup_c_");
  }

  return *this;
}
cfi_cfg_r15_s& phys_cfg_ded_scell_r10_s::semi_static_cfi_cfg_r15_c_::setup_c_::set_cfi_cfg_r15()
{
  set(types::cfi_cfg_r15);
  return c.get<cfi_cfg_r15_s>();
}
cfi_pattern_cfg_r15_s& phys_cfg_ded_scell_r10_s::semi_static_cfi_cfg_r15_c_::setup_c_::set_cfi_pattern_cfg_r15()
{
  set(types::cfi_pattern_cfg_r15);
  return c.get<cfi_pattern_cfg_r15_s>();
}
void phys_cfg_ded_scell_r10_s::semi_static_cfi_cfg_r15_c_::setup_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::cfi_cfg_r15:
      j.write_fieldname("cfi-Config-r15");
      c.get<cfi_cfg_r15_s>().to_json(j);
      break;
    case types::cfi_pattern_cfg_r15:
      j.write_fieldname("cfi-PatternConfig-r15");
      c.get<cfi_pattern_cfg_r15_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_scell_r10_s::semi_static_cfi_cfg_r15_c_::setup_c_");
  }
  j.end_obj();
}
SRSASN_CODE phys_cfg_ded_scell_r10_s::semi_static_cfi_cfg_r15_c_::setup_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::cfi_cfg_r15:
      HANDLE_CODE(c.get<cfi_cfg_r15_s>().pack(bref));
      break;
    case types::cfi_pattern_cfg_r15:
      HANDLE_CODE(c.get<cfi_pattern_cfg_r15_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_scell_r10_s::semi_static_cfi_cfg_r15_c_::setup_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE phys_cfg_ded_scell_r10_s::semi_static_cfi_cfg_r15_c_::setup_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::cfi_cfg_r15:
      HANDLE_CODE(c.get<cfi_cfg_r15_s>().unpack(bref));
      break;
    case types::cfi_pattern_cfg_r15:
      HANDLE_CODE(c.get<cfi_pattern_cfg_r15_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_scell_r10_s::semi_static_cfi_cfg_r15_c_::setup_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool phys_cfg_ded_scell_r10_s::semi_static_cfi_cfg_r15_c_::setup_c_::operator==(const setup_c_& other) const
{
  if (type_ != other.type_) {
    return false;
  }
  switch (type_) {
    case types::cfi_cfg_r15:
      return c.get<cfi_cfg_r15_s>() == other.c.get<cfi_cfg_r15_s>();
    case types::cfi_pattern_cfg_r15:
      return c.get<cfi_pattern_cfg_r15_s>() == other.c.get<cfi_pattern_cfg_r15_s>();
    default:
      return true;
  }
  return true;
}

void phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::set(types::options e)
{
  type_ = e;
}
void phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::set_release()
{
  set(types::release);
}
phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_&
phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::set_setup()
{
  set(types::setup);
  return c;
}
void phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_bool("blindSubframePDSCH-Repetitions-r15", c.blind_sf_pdsch_repeats_r15);
      j.write_bool("blindSlotSubslotPDSCH-Repetitions-r15", c.blind_slot_subslot_pdsch_repeats_r15);
      if (c.max_num_sf_pdsch_repeats_r15_present) {
        j.write_str("maxNumber-SubframePDSCH-Repetitions-r15", c.max_num_sf_pdsch_repeats_r15.to_string());
      }
      if (c.max_num_slot_subslot_pdsch_repeats_r15_present) {
        j.write_str("maxNumber-SlotSubslotPDSCH-Repetitions-r15", c.max_num_slot_subslot_pdsch_repeats_r15.to_string());
      }
      if (c.rv_sf_pdsch_repeats_r15_present) {
        j.write_str("rv-SubframePDSCH-Repetitions-r15", c.rv_sf_pdsch_repeats_r15.to_string());
      }
      if (c.rv_slotsublot_pdsch_repeats_r15_present) {
        j.write_str("rv-SlotsublotPDSCH-Repetitions-r15", c.rv_slotsublot_pdsch_repeats_r15.to_string());
      }
      if (c.nof_processes_sf_pdsch_repeats_r15_present) {
        j.write_int("numberOfProcesses-SubframePDSCH-Repetitions-r15", c.nof_processes_sf_pdsch_repeats_r15);
      }
      if (c.nof_processes_slot_subslot_pdsch_repeats_r15_present) {
        j.write_int("numberOfProcesses-SlotSubslotPDSCH-Repetitions-r15",
                    c.nof_processes_slot_subslot_pdsch_repeats_r15);
      }
      if (c.mcs_restrict_sf_pdsch_repeats_r15_present) {
        j.write_str("mcs-restrictionSubframePDSCH-Repetitions-r15", c.mcs_restrict_sf_pdsch_repeats_r15.to_string());
      }
      if (c.mcs_restrict_slot_subslot_pdsch_repeats_r15_present) {
        j.write_str("mcs-restrictionSlotSubslotPDSCH-Repetitions-r15",
                    c.mcs_restrict_slot_subslot_pdsch_repeats_r15.to_string());
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_");
  }
  j.end_obj();
}
SRSASN_CODE phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.max_num_sf_pdsch_repeats_r15_present, 1));
      HANDLE_CODE(bref.pack(c.max_num_slot_subslot_pdsch_repeats_r15_present, 1));
      HANDLE_CODE(bref.pack(c.rv_sf_pdsch_repeats_r15_present, 1));
      HANDLE_CODE(bref.pack(c.rv_slotsublot_pdsch_repeats_r15_present, 1));
      HANDLE_CODE(bref.pack(c.nof_processes_sf_pdsch_repeats_r15_present, 1));
      HANDLE_CODE(bref.pack(c.nof_processes_slot_subslot_pdsch_repeats_r15_present, 1));
      HANDLE_CODE(bref.pack(c.mcs_restrict_sf_pdsch_repeats_r15_present, 1));
      HANDLE_CODE(bref.pack(c.mcs_restrict_slot_subslot_pdsch_repeats_r15_present, 1));
      HANDLE_CODE(bref.pack(c.blind_sf_pdsch_repeats_r15, 1));
      HANDLE_CODE(bref.pack(c.blind_slot_subslot_pdsch_repeats_r15, 1));
      if (c.max_num_sf_pdsch_repeats_r15_present) {
        HANDLE_CODE(c.max_num_sf_pdsch_repeats_r15.pack(bref));
      }
      if (c.max_num_slot_subslot_pdsch_repeats_r15_present) {
        HANDLE_CODE(c.max_num_slot_subslot_pdsch_repeats_r15.pack(bref));
      }
      if (c.rv_sf_pdsch_repeats_r15_present) {
        HANDLE_CODE(c.rv_sf_pdsch_repeats_r15.pack(bref));
      }
      if (c.rv_slotsublot_pdsch_repeats_r15_present) {
        HANDLE_CODE(c.rv_slotsublot_pdsch_repeats_r15.pack(bref));
      }
      if (c.nof_processes_sf_pdsch_repeats_r15_present) {
        HANDLE_CODE(pack_integer(bref, c.nof_processes_sf_pdsch_repeats_r15, (uint8_t)1u, (uint8_t)16u));
      }
      if (c.nof_processes_slot_subslot_pdsch_repeats_r15_present) {
        HANDLE_CODE(pack_integer(bref, c.nof_processes_slot_subslot_pdsch_repeats_r15, (uint8_t)1u, (uint8_t)16u));
      }
      if (c.mcs_restrict_sf_pdsch_repeats_r15_present) {
        HANDLE_CODE(c.mcs_restrict_sf_pdsch_repeats_r15.pack(bref));
      }
      if (c.mcs_restrict_slot_subslot_pdsch_repeats_r15_present) {
        HANDLE_CODE(c.mcs_restrict_slot_subslot_pdsch_repeats_r15.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.max_num_sf_pdsch_repeats_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.max_num_slot_subslot_pdsch_repeats_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.rv_sf_pdsch_repeats_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.rv_slotsublot_pdsch_repeats_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.nof_processes_sf_pdsch_repeats_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.nof_processes_slot_subslot_pdsch_repeats_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.mcs_restrict_sf_pdsch_repeats_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.mcs_restrict_slot_subslot_pdsch_repeats_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.blind_sf_pdsch_repeats_r15, 1));
      HANDLE_CODE(bref.unpack(c.blind_slot_subslot_pdsch_repeats_r15, 1));
      if (c.max_num_sf_pdsch_repeats_r15_present) {
        HANDLE_CODE(c.max_num_sf_pdsch_repeats_r15.unpack(bref));
      }
      if (c.max_num_slot_subslot_pdsch_repeats_r15_present) {
        HANDLE_CODE(c.max_num_slot_subslot_pdsch_repeats_r15.unpack(bref));
      }
      if (c.rv_sf_pdsch_repeats_r15_present) {
        HANDLE_CODE(c.rv_sf_pdsch_repeats_r15.unpack(bref));
      }
      if (c.rv_slotsublot_pdsch_repeats_r15_present) {
        HANDLE_CODE(c.rv_slotsublot_pdsch_repeats_r15.unpack(bref));
      }
      if (c.nof_processes_sf_pdsch_repeats_r15_present) {
        HANDLE_CODE(unpack_integer(c.nof_processes_sf_pdsch_repeats_r15, bref, (uint8_t)1u, (uint8_t)16u));
      }
      if (c.nof_processes_slot_subslot_pdsch_repeats_r15_present) {
        HANDLE_CODE(unpack_integer(c.nof_processes_slot_subslot_pdsch_repeats_r15, bref, (uint8_t)1u, (uint8_t)16u));
      }
      if (c.mcs_restrict_sf_pdsch_repeats_r15_present) {
        HANDLE_CODE(c.mcs_restrict_sf_pdsch_repeats_r15.unpack(bref));
      }
      if (c.mcs_restrict_slot_subslot_pdsch_repeats_r15_present) {
        HANDLE_CODE(c.mcs_restrict_slot_subslot_pdsch_repeats_r15.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::operator==(
    const blind_pdsch_repeat_cfg_r15_c_& other) const
{
  return type() == other.type() and c.blind_sf_pdsch_repeats_r15 == other.c.blind_sf_pdsch_repeats_r15 and
         c.blind_slot_subslot_pdsch_repeats_r15 == other.c.blind_slot_subslot_pdsch_repeats_r15 and
         c.max_num_sf_pdsch_repeats_r15_present == other.c.max_num_sf_pdsch_repeats_r15_present and
         (not c.max_num_sf_pdsch_repeats_r15_present or
          c.max_num_sf_pdsch_repeats_r15 == other.c.max_num_sf_pdsch_repeats_r15) and
         c.max_num_slot_subslot_pdsch_repeats_r15_present == other.c.max_num_slot_subslot_pdsch_repeats_r15_present and
         (not c.max_num_slot_subslot_pdsch_repeats_r15_present or
          c.max_num_slot_subslot_pdsch_repeats_r15 == other.c.max_num_slot_subslot_pdsch_repeats_r15) and
         c.rv_sf_pdsch_repeats_r15_present == other.c.rv_sf_pdsch_repeats_r15_present and
         (not c.rv_sf_pdsch_repeats_r15_present or c.rv_sf_pdsch_repeats_r15 == other.c.rv_sf_pdsch_repeats_r15) and
         c.rv_slotsublot_pdsch_repeats_r15_present == other.c.rv_slotsublot_pdsch_repeats_r15_present and
         (not c.rv_slotsublot_pdsch_repeats_r15_present or
          c.rv_slotsublot_pdsch_repeats_r15 == other.c.rv_slotsublot_pdsch_repeats_r15) and
         c.nof_processes_sf_pdsch_repeats_r15_present == other.c.nof_processes_sf_pdsch_repeats_r15_present and
         (not c.nof_processes_sf_pdsch_repeats_r15_present or
          c.nof_processes_sf_pdsch_repeats_r15 == other.c.nof_processes_sf_pdsch_repeats_r15) and
         c.nof_processes_slot_subslot_pdsch_repeats_r15_present ==
             other.c.nof_processes_slot_subslot_pdsch_repeats_r15_present and
         (not c.nof_processes_slot_subslot_pdsch_repeats_r15_present or
          c.nof_processes_slot_subslot_pdsch_repeats_r15 == other.c.nof_processes_slot_subslot_pdsch_repeats_r15) and
         c.mcs_restrict_sf_pdsch_repeats_r15_present == other.c.mcs_restrict_sf_pdsch_repeats_r15_present and
         (not c.mcs_restrict_sf_pdsch_repeats_r15_present or
          c.mcs_restrict_sf_pdsch_repeats_r15 == other.c.mcs_restrict_sf_pdsch_repeats_r15) and
         c.mcs_restrict_slot_subslot_pdsch_repeats_r15_present ==
             other.c.mcs_restrict_slot_subslot_pdsch_repeats_r15_present and
         (not c.mcs_restrict_slot_subslot_pdsch_repeats_r15_present or
          c.mcs_restrict_slot_subslot_pdsch_repeats_r15 == other.c.mcs_restrict_slot_subslot_pdsch_repeats_r15);
}

const char*
phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::max_num_sf_pdsch_repeats_r15_opts::to_string() const
{
  static const char* options[] = {"n4", "n6"};
  return convert_enum_idx(
      options,
      2,
      value,
      "phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::max_num_sf_pdsch_repeats_r15_e_");
}
uint8_t
phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::max_num_sf_pdsch_repeats_r15_opts::to_number() const
{
  static const uint8_t options[] = {4, 6};
  return map_enum_number(
      options,
      2,
      value,
      "phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::max_num_sf_pdsch_repeats_r15_e_");
}

const char* phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::
    max_num_slot_subslot_pdsch_repeats_r15_opts::to_string() const
{
  static const char* options[] = {"n4", "n6"};
  return convert_enum_idx(
      options,
      2,
      value,
      "phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::max_num_slot_subslot_pdsch_repeats_r15_e_");
}
uint8_t phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::max_num_slot_subslot_pdsch_repeats_r15_opts::
    to_number() const
{
  static const uint8_t options[] = {4, 6};
  return map_enum_number(
      options,
      2,
      value,
      "phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::max_num_slot_subslot_pdsch_repeats_r15_e_");
}

const char*
phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::rv_sf_pdsch_repeats_r15_opts::to_string() const
{
  static const char* options[] = {"dlrvseq1", "dlrvseq2"};
  return convert_enum_idx(
      options,
      2,
      value,
      "phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::rv_sf_pdsch_repeats_r15_e_");
}
uint8_t
phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::rv_sf_pdsch_repeats_r15_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(
      options,
      2,
      value,
      "phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::rv_sf_pdsch_repeats_r15_e_");
}

const char*
phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::rv_slotsublot_pdsch_repeats_r15_opts::to_string()
    const
{
  static const char* options[] = {"dlrvseq1", "dlrvseq2"};
  return convert_enum_idx(
      options,
      2,
      value,
      "phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::rv_slotsublot_pdsch_repeats_r15_e_");
}
uint8_t
phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::rv_slotsublot_pdsch_repeats_r15_opts::to_number()
    const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(
      options,
      2,
      value,
      "phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::rv_slotsublot_pdsch_repeats_r15_e_");
}

const char*
phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::mcs_restrict_sf_pdsch_repeats_r15_opts::to_string()
    const
{
  static const char* options[] = {"n0", "n1"};
  return convert_enum_idx(
      options,
      2,
      value,
      "phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::mcs_restrict_sf_pdsch_repeats_r15_e_");
}
uint8_t
phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::mcs_restrict_sf_pdsch_repeats_r15_opts::to_number()
    const
{
  static const uint8_t options[] = {0, 1};
  return map_enum_number(
      options,
      2,
      value,
      "phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::mcs_restrict_sf_pdsch_repeats_r15_e_");
}

const char* phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::
    mcs_restrict_slot_subslot_pdsch_repeats_r15_opts::to_string() const
{
  static const char* options[] = {"n0", "n1"};
  return convert_enum_idx(options,
                          2,
                          value,
                          "phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::mcs_restrict_slot_"
                          "subslot_pdsch_repeats_r15_e_");
}
uint8_t phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::
    mcs_restrict_slot_subslot_pdsch_repeats_r15_opts::to_number() const
{
  static const uint8_t options[] = {0, 1};
  return map_enum_number(options,
                         2,
                         value,
                         "phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::mcs_restrict_slot_subslot_"
                         "pdsch_repeats_r15_e_");
}

// PhysicalConfigDedicatedSCell-v1370 ::= SEQUENCE
SRSASN_CODE phys_cfg_ded_scell_v1370_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pucch_scell_v1370.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE phys_cfg_ded_scell_v1370_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(pucch_scell_v1370.unpack(bref));

  return SRSASN_SUCCESS;
}
void phys_cfg_ded_scell_v1370_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("pucch-SCell-v1370");
  pucch_scell_v1370.to_json(j);
  j.end_obj();
}
bool phys_cfg_ded_scell_v1370_s::operator==(const phys_cfg_ded_scell_v1370_s& other) const
{
  return pucch_scell_v1370 == other.pucch_scell_v1370;
}

void phys_cfg_ded_scell_v1370_s::pucch_scell_v1370_c_::set(types::options e)
{
  type_ = e;
}
void phys_cfg_ded_scell_v1370_s::pucch_scell_v1370_c_::set_release()
{
  set(types::release);
}
phys_cfg_ded_scell_v1370_s::pucch_scell_v1370_c_::setup_s_&
phys_cfg_ded_scell_v1370_s::pucch_scell_v1370_c_::set_setup()
{
  set(types::setup);
  return c;
}
void phys_cfg_ded_scell_v1370_s::pucch_scell_v1370_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      if (c.pucch_cfg_ded_v1370_present) {
        j.write_fieldname("pucch-ConfigDedicated-v1370");
        c.pucch_cfg_ded_v1370.to_json(j);
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_scell_v1370_s::pucch_scell_v1370_c_");
  }
  j.end_obj();
}
SRSASN_CODE phys_cfg_ded_scell_v1370_s::pucch_scell_v1370_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.pucch_cfg_ded_v1370_present, 1));
      if (c.pucch_cfg_ded_v1370_present) {
        HANDLE_CODE(c.pucch_cfg_ded_v1370.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_scell_v1370_s::pucch_scell_v1370_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE phys_cfg_ded_scell_v1370_s::pucch_scell_v1370_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.pucch_cfg_ded_v1370_present, 1));
      if (c.pucch_cfg_ded_v1370_present) {
        HANDLE_CODE(c.pucch_cfg_ded_v1370.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_scell_v1370_s::pucch_scell_v1370_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool phys_cfg_ded_scell_v1370_s::pucch_scell_v1370_c_::operator==(const pucch_scell_v1370_c_& other) const
{
  return type() == other.type() and c.pucch_cfg_ded_v1370_present == other.c.pucch_cfg_ded_v1370_present and
         (not c.pucch_cfg_ded_v1370_present or c.pucch_cfg_ded_v1370 == other.c.pucch_cfg_ded_v1370);
}

// PhysicalConfigDedicatedSCell-v1730 ::= SEQUENCE
SRSASN_CODE phys_cfg_ded_scell_v1730_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(cqi_report_periodic_scell_v1730.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE phys_cfg_ded_scell_v1730_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(cqi_report_periodic_scell_v1730.unpack(bref));

  return SRSASN_SUCCESS;
}
void phys_cfg_ded_scell_v1730_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("cqi-ReportPeriodicSCell-v1730");
  cqi_report_periodic_scell_v1730.to_json(j);
  j.end_obj();
}
bool phys_cfg_ded_scell_v1730_s::operator==(const phys_cfg_ded_scell_v1730_s& other) const
{
  return cqi_report_periodic_scell_v1730 == other.cqi_report_periodic_scell_v1730;
}

// AntennaInfoDedicated-v10i0 ::= SEQUENCE
SRSASN_CODE ant_info_ded_v10i0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(max_layers_mimo_r10_present, 1));

  if (max_layers_mimo_r10_present) {
    HANDLE_CODE(max_layers_mimo_r10.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ant_info_ded_v10i0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(max_layers_mimo_r10_present, 1));

  if (max_layers_mimo_r10_present) {
    HANDLE_CODE(max_layers_mimo_r10.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ant_info_ded_v10i0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (max_layers_mimo_r10_present) {
    j.write_str("maxLayersMIMO-r10", max_layers_mimo_r10.to_string());
  }
  j.end_obj();
}
bool ant_info_ded_v10i0_s::operator==(const ant_info_ded_v10i0_s& other) const
{
  return max_layers_mimo_r10_present == other.max_layers_mimo_r10_present and
         (not max_layers_mimo_r10_present or max_layers_mimo_r10 == other.max_layers_mimo_r10);
}

const char* ant_info_ded_v10i0_s::max_layers_mimo_r10_opts::to_string() const
{
  static const char* options[] = {"twoLayers", "fourLayers", "eightLayers"};
  return convert_enum_idx(options, 3, value, "ant_info_ded_v10i0_s::max_layers_mimo_r10_e_");
}
uint8_t ant_info_ded_v10i0_s::max_layers_mimo_r10_opts::to_number() const
{
  static const uint8_t options[] = {2, 4, 8};
  return map_enum_number(options, 3, value, "ant_info_ded_v10i0_s::max_layers_mimo_r10_e_");
}

// RadioResourceConfigDedicatedSCell-r10 ::= SEQUENCE
SRSASN_CODE rr_cfg_ded_scell_r10_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(phys_cfg_ded_scell_r10_present, 1));

  if (phys_cfg_ded_scell_r10_present) {
    HANDLE_CODE(phys_cfg_ded_scell_r10.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= mac_main_cfg_scell_r11.is_present();
    group_flags[1] |= naics_info_r12.is_present();
    group_flags[2] |= neigh_cells_crs_info_scell_r13.is_present();
    group_flags[3] |= phys_cfg_ded_scell_v1370.is_present();
    group_flags[4] |= crs_intf_mitig_enabled_r15_present;
    group_flags[4] |= neigh_cells_crs_info_r15.is_present();
    group_flags[4] |= sps_cfg_v1530.is_present();
    group_flags[5] |= phys_cfg_ded_scell_v1730.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(mac_main_cfg_scell_r11.is_present(), 1));
      if (mac_main_cfg_scell_r11.is_present()) {
        HANDLE_CODE(mac_main_cfg_scell_r11->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(naics_info_r12.is_present(), 1));
      if (naics_info_r12.is_present()) {
        HANDLE_CODE(naics_info_r12->pack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(neigh_cells_crs_info_scell_r13.is_present(), 1));
      if (neigh_cells_crs_info_scell_r13.is_present()) {
        HANDLE_CODE(neigh_cells_crs_info_scell_r13->pack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(phys_cfg_ded_scell_v1370.is_present(), 1));
      if (phys_cfg_ded_scell_v1370.is_present()) {
        HANDLE_CODE(phys_cfg_ded_scell_v1370->pack(bref));
      }
    }
    if (group_flags[4]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(crs_intf_mitig_enabled_r15_present, 1));
      HANDLE_CODE(bref.pack(neigh_cells_crs_info_r15.is_present(), 1));
      HANDLE_CODE(bref.pack(sps_cfg_v1530.is_present(), 1));
      if (crs_intf_mitig_enabled_r15_present) {
        HANDLE_CODE(bref.pack(crs_intf_mitig_enabled_r15, 1));
      }
      if (neigh_cells_crs_info_r15.is_present()) {
        HANDLE_CODE(neigh_cells_crs_info_r15->pack(bref));
      }
      if (sps_cfg_v1530.is_present()) {
        HANDLE_CODE(sps_cfg_v1530->pack(bref));
      }
    }
    if (group_flags[5]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(phys_cfg_ded_scell_v1730.is_present(), 1));
      if (phys_cfg_ded_scell_v1730.is_present()) {
        HANDLE_CODE(phys_cfg_ded_scell_v1730->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rr_cfg_ded_scell_r10_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(phys_cfg_ded_scell_r10_present, 1));

  if (phys_cfg_ded_scell_r10_present) {
    HANDLE_CODE(phys_cfg_ded_scell_r10.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(6);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool mac_main_cfg_scell_r11_present;
      HANDLE_CODE(bref.unpack(mac_main_cfg_scell_r11_present, 1));
      mac_main_cfg_scell_r11.set_present(mac_main_cfg_scell_r11_present);
      if (mac_main_cfg_scell_r11.is_present()) {
        HANDLE_CODE(mac_main_cfg_scell_r11->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool naics_info_r12_present;
      HANDLE_CODE(bref.unpack(naics_info_r12_present, 1));
      naics_info_r12.set_present(naics_info_r12_present);
      if (naics_info_r12.is_present()) {
        HANDLE_CODE(naics_info_r12->unpack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool neigh_cells_crs_info_scell_r13_present;
      HANDLE_CODE(bref.unpack(neigh_cells_crs_info_scell_r13_present, 1));
      neigh_cells_crs_info_scell_r13.set_present(neigh_cells_crs_info_scell_r13_present);
      if (neigh_cells_crs_info_scell_r13.is_present()) {
        HANDLE_CODE(neigh_cells_crs_info_scell_r13->unpack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool phys_cfg_ded_scell_v1370_present;
      HANDLE_CODE(bref.unpack(phys_cfg_ded_scell_v1370_present, 1));
      phys_cfg_ded_scell_v1370.set_present(phys_cfg_ded_scell_v1370_present);
      if (phys_cfg_ded_scell_v1370.is_present()) {
        HANDLE_CODE(phys_cfg_ded_scell_v1370->unpack(bref));
      }
    }
    if (group_flags[4]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(crs_intf_mitig_enabled_r15_present, 1));
      bool neigh_cells_crs_info_r15_present;
      HANDLE_CODE(bref.unpack(neigh_cells_crs_info_r15_present, 1));
      neigh_cells_crs_info_r15.set_present(neigh_cells_crs_info_r15_present);
      bool sps_cfg_v1530_present;
      HANDLE_CODE(bref.unpack(sps_cfg_v1530_present, 1));
      sps_cfg_v1530.set_present(sps_cfg_v1530_present);
      if (crs_intf_mitig_enabled_r15_present) {
        HANDLE_CODE(bref.unpack(crs_intf_mitig_enabled_r15, 1));
      }
      if (neigh_cells_crs_info_r15.is_present()) {
        HANDLE_CODE(neigh_cells_crs_info_r15->unpack(bref));
      }
      if (sps_cfg_v1530.is_present()) {
        HANDLE_CODE(sps_cfg_v1530->unpack(bref));
      }
    }
    if (group_flags[5]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool phys_cfg_ded_scell_v1730_present;
      HANDLE_CODE(bref.unpack(phys_cfg_ded_scell_v1730_present, 1));
      phys_cfg_ded_scell_v1730.set_present(phys_cfg_ded_scell_v1730_present);
      if (phys_cfg_ded_scell_v1730.is_present()) {
        HANDLE_CODE(phys_cfg_ded_scell_v1730->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void rr_cfg_ded_scell_r10_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (phys_cfg_ded_scell_r10_present) {
    j.write_fieldname("physicalConfigDedicatedSCell-r10");
    phys_cfg_ded_scell_r10.to_json(j);
  }
  if (ext) {
    if (mac_main_cfg_scell_r11.is_present()) {
      j.write_fieldname("mac-MainConfigSCell-r11");
      mac_main_cfg_scell_r11->to_json(j);
    }
    if (naics_info_r12.is_present()) {
      j.write_fieldname("naics-Info-r12");
      naics_info_r12->to_json(j);
    }
    if (neigh_cells_crs_info_scell_r13.is_present()) {
      j.write_fieldname("neighCellsCRS-InfoSCell-r13");
      neigh_cells_crs_info_scell_r13->to_json(j);
    }
    if (phys_cfg_ded_scell_v1370.is_present()) {
      j.write_fieldname("physicalConfigDedicatedSCell-v1370");
      phys_cfg_ded_scell_v1370->to_json(j);
    }
    if (crs_intf_mitig_enabled_r15_present) {
      j.write_bool("crs-IntfMitigEnabled-r15", crs_intf_mitig_enabled_r15);
    }
    if (neigh_cells_crs_info_r15.is_present()) {
      j.write_fieldname("neighCellsCRS-Info-r15");
      neigh_cells_crs_info_r15->to_json(j);
    }
    if (sps_cfg_v1530.is_present()) {
      j.write_fieldname("sps-Config-v1530");
      sps_cfg_v1530->to_json(j);
    }
    if (phys_cfg_ded_scell_v1730.is_present()) {
      j.write_fieldname("physicalConfigDedicatedSCell-v1730");
      phys_cfg_ded_scell_v1730->to_json(j);
    }
  }
  j.end_obj();
}
bool rr_cfg_ded_scell_r10_s::operator==(const rr_cfg_ded_scell_r10_s& other) const
{
  return ext == other.ext and phys_cfg_ded_scell_r10_present == other.phys_cfg_ded_scell_r10_present and
         (not phys_cfg_ded_scell_r10_present or phys_cfg_ded_scell_r10 == other.phys_cfg_ded_scell_r10) and
         (not ext or
          (mac_main_cfg_scell_r11.is_present() == other.mac_main_cfg_scell_r11.is_present() and
           (not mac_main_cfg_scell_r11.is_present() or *mac_main_cfg_scell_r11 == *other.mac_main_cfg_scell_r11) and
           naics_info_r12.is_present() == other.naics_info_r12.is_present() and
           (not naics_info_r12.is_present() or *naics_info_r12 == *other.naics_info_r12) and
           neigh_cells_crs_info_scell_r13.is_present() == other.neigh_cells_crs_info_scell_r13.is_present() and
           (not neigh_cells_crs_info_scell_r13.is_present() or
            *neigh_cells_crs_info_scell_r13 == *other.neigh_cells_crs_info_scell_r13) and
           phys_cfg_ded_scell_v1370.is_present() == other.phys_cfg_ded_scell_v1370.is_present() and
           (not phys_cfg_ded_scell_v1370.is_present() or
            *phys_cfg_ded_scell_v1370 == *other.phys_cfg_ded_scell_v1370) and
           crs_intf_mitig_enabled_r15_present == other.crs_intf_mitig_enabled_r15_present and
           (not crs_intf_mitig_enabled_r15_present or
            crs_intf_mitig_enabled_r15 == other.crs_intf_mitig_enabled_r15) and
           neigh_cells_crs_info_r15.is_present() == other.neigh_cells_crs_info_r15.is_present() and
           (not neigh_cells_crs_info_r15.is_present() or
            *neigh_cells_crs_info_r15 == *other.neigh_cells_crs_info_r15) and
           sps_cfg_v1530.is_present() == other.sps_cfg_v1530.is_present() and
           (not sps_cfg_v1530.is_present() or *sps_cfg_v1530 == *other.sps_cfg_v1530) and
           phys_cfg_ded_scell_v1730.is_present() == other.phys_cfg_ded_scell_v1730.is_present() and
           (not phys_cfg_ded_scell_v1730.is_present() or
            *phys_cfg_ded_scell_v1730 == *other.phys_cfg_ded_scell_v1730)));
}

// SCellToAddModExt-r13 ::= SEQUENCE
SRSASN_CODE scell_to_add_mod_ext_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cell_identif_r13_present, 1));
  HANDLE_CODE(bref.pack(rr_cfg_common_scell_r13_present, 1));
  HANDLE_CODE(bref.pack(rr_cfg_ded_scell_r13_present, 1));
  HANDLE_CODE(bref.pack(ant_info_ded_scell_r13_present, 1));

  HANDLE_CODE(pack_integer(bref, scell_idx_r13, (uint8_t)1u, (uint8_t)31u));
  if (cell_identif_r13_present) {
    HANDLE_CODE(pack_integer(bref, cell_identif_r13.pci_r13, (uint16_t)0u, (uint16_t)503u));
    HANDLE_CODE(pack_integer(bref, cell_identif_r13.dl_carrier_freq_r13, (uint32_t)0u, (uint32_t)262143u));
  }
  if (rr_cfg_common_scell_r13_present) {
    HANDLE_CODE(rr_cfg_common_scell_r13.pack(bref));
  }
  if (rr_cfg_ded_scell_r13_present) {
    HANDLE_CODE(rr_cfg_ded_scell_r13.pack(bref));
  }
  if (ant_info_ded_scell_r13_present) {
    HANDLE_CODE(ant_info_ded_scell_r13.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE scell_to_add_mod_ext_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cell_identif_r13_present, 1));
  HANDLE_CODE(bref.unpack(rr_cfg_common_scell_r13_present, 1));
  HANDLE_CODE(bref.unpack(rr_cfg_ded_scell_r13_present, 1));
  HANDLE_CODE(bref.unpack(ant_info_ded_scell_r13_present, 1));

  HANDLE_CODE(unpack_integer(scell_idx_r13, bref, (uint8_t)1u, (uint8_t)31u));
  if (cell_identif_r13_present) {
    HANDLE_CODE(unpack_integer(cell_identif_r13.pci_r13, bref, (uint16_t)0u, (uint16_t)503u));
    HANDLE_CODE(unpack_integer(cell_identif_r13.dl_carrier_freq_r13, bref, (uint32_t)0u, (uint32_t)262143u));
  }
  if (rr_cfg_common_scell_r13_present) {
    HANDLE_CODE(rr_cfg_common_scell_r13.unpack(bref));
  }
  if (rr_cfg_ded_scell_r13_present) {
    HANDLE_CODE(rr_cfg_ded_scell_r13.unpack(bref));
  }
  if (ant_info_ded_scell_r13_present) {
    HANDLE_CODE(ant_info_ded_scell_r13.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void scell_to_add_mod_ext_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("sCellIndex-r13", scell_idx_r13);
  if (cell_identif_r13_present) {
    j.write_fieldname("cellIdentification-r13");
    j.start_obj();
    j.write_int("physCellId-r13", cell_identif_r13.pci_r13);
    j.write_int("dl-CarrierFreq-r13", cell_identif_r13.dl_carrier_freq_r13);
    j.end_obj();
  }
  if (rr_cfg_common_scell_r13_present) {
    j.write_fieldname("radioResourceConfigCommonSCell-r13");
    rr_cfg_common_scell_r13.to_json(j);
  }
  if (rr_cfg_ded_scell_r13_present) {
    j.write_fieldname("radioResourceConfigDedicatedSCell-r13");
    rr_cfg_ded_scell_r13.to_json(j);
  }
  if (ant_info_ded_scell_r13_present) {
    j.write_fieldname("antennaInfoDedicatedSCell-r13");
    ant_info_ded_scell_r13.to_json(j);
  }
  j.end_obj();
}

// PUCCH-ConfigDedicated-v13c0 ::= SEQUENCE
SRSASN_CODE pucch_cfg_ded_v13c0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(ch_sel_v13c0.n1_pucch_an_cs_v13c0.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pucch_cfg_ded_v13c0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(ch_sel_v13c0.n1_pucch_an_cs_v13c0.unpack(bref));

  return SRSASN_SUCCESS;
}
void pucch_cfg_ded_v13c0_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("channelSelection-v13c0");
  j.start_obj();
  j.write_fieldname("n1PUCCH-AN-CS-v13c0");
  ch_sel_v13c0.n1_pucch_an_cs_v13c0.to_json(j);
  j.end_obj();
  j.end_obj();
}

void pucch_cfg_ded_v13c0_s::ch_sel_v13c0_s_::n1_pucch_an_cs_v13c0_c_::set(types::options e)
{
  type_ = e;
}
void pucch_cfg_ded_v13c0_s::ch_sel_v13c0_s_::n1_pucch_an_cs_v13c0_c_::set_release()
{
  set(types::release);
}
pucch_cfg_ded_v13c0_s::ch_sel_v13c0_s_::n1_pucch_an_cs_v13c0_c_::setup_s_&
pucch_cfg_ded_v13c0_s::ch_sel_v13c0_s_::n1_pucch_an_cs_v13c0_c_::set_setup()
{
  set(types::setup);
  return c;
}
void pucch_cfg_ded_v13c0_s::ch_sel_v13c0_s_::n1_pucch_an_cs_v13c0_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.start_array("n1PUCCH-AN-CS-ListP1-v13c0");
      for (const auto& e1 : c.n1_pucch_an_cs_list_p1_v13c0) {
        j.write_int(e1);
      }
      j.end_array();
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_v13c0_s::ch_sel_v13c0_s_::n1_pucch_an_cs_v13c0_c_");
  }
  j.end_obj();
}
SRSASN_CODE pucch_cfg_ded_v13c0_s::ch_sel_v13c0_s_::n1_pucch_an_cs_v13c0_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.n1_pucch_an_cs_list_p1_v13c0, 2, 4, integer_packer<uint16_t>(0, 2047)));
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_v13c0_s::ch_sel_v13c0_s_::n1_pucch_an_cs_v13c0_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pucch_cfg_ded_v13c0_s::ch_sel_v13c0_s_::n1_pucch_an_cs_v13c0_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(unpack_dyn_seq_of(c.n1_pucch_an_cs_list_p1_v13c0, bref, 2, 4, integer_packer<uint16_t>(0, 2047)));
      break;
    default:
      log_invalid_choice_id(type_, "pucch_cfg_ded_v13c0_s::ch_sel_v13c0_s_::n1_pucch_an_cs_v13c0_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// DRB-ToAddModSCG-r12 ::= SEQUENCE
SRSASN_CODE drb_to_add_mod_scg_r12_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(drb_type_r12_present, 1));
  HANDLE_CODE(bref.pack(rlc_cfg_scg_r12_present, 1));
  HANDLE_CODE(bref.pack(rlc_cfg_v1250_present, 1));
  HANDLE_CODE(bref.pack(lc_ch_id_scg_r12_present, 1));
  HANDLE_CODE(bref.pack(lc_ch_cfg_scg_r12_present, 1));

  HANDLE_CODE(pack_integer(bref, drb_id_r12, (uint8_t)1u, (uint8_t)32u));
  if (drb_type_r12_present) {
    HANDLE_CODE(drb_type_r12.pack(bref));
  }
  if (rlc_cfg_scg_r12_present) {
    HANDLE_CODE(rlc_cfg_scg_r12.pack(bref));
  }
  if (rlc_cfg_v1250_present) {
    HANDLE_CODE(rlc_cfg_v1250.pack(bref));
  }
  if (lc_ch_id_scg_r12_present) {
    HANDLE_CODE(pack_integer(bref, lc_ch_id_scg_r12, (uint8_t)3u, (uint8_t)10u));
  }
  if (lc_ch_cfg_scg_r12_present) {
    HANDLE_CODE(lc_ch_cfg_scg_r12.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= rlc_cfg_v1430.is_present();
    group_flags[1] |= lc_ch_id_scg_r15_present;
    group_flags[1] |= rlc_cfg_v1530.is_present();
    group_flags[1] |= rlc_bearer_cfg_secondary_r15.is_present();
    group_flags[2] |= rlc_cfg_v1560.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(rlc_cfg_v1430.is_present(), 1));
      if (rlc_cfg_v1430.is_present()) {
        HANDLE_CODE(rlc_cfg_v1430->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(lc_ch_id_scg_r15_present, 1));
      HANDLE_CODE(bref.pack(rlc_cfg_v1530.is_present(), 1));
      HANDLE_CODE(bref.pack(rlc_bearer_cfg_secondary_r15.is_present(), 1));
      if (lc_ch_id_scg_r15_present) {
        HANDLE_CODE(pack_integer(bref, lc_ch_id_scg_r15, (uint8_t)32u, (uint8_t)38u));
      }
      if (rlc_cfg_v1530.is_present()) {
        HANDLE_CODE(rlc_cfg_v1530->pack(bref));
      }
      if (rlc_bearer_cfg_secondary_r15.is_present()) {
        HANDLE_CODE(rlc_bearer_cfg_secondary_r15->pack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(rlc_cfg_v1560.is_present(), 1));
      if (rlc_cfg_v1560.is_present()) {
        HANDLE_CODE(rlc_cfg_v1560->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_to_add_mod_scg_r12_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(drb_type_r12_present, 1));
  HANDLE_CODE(bref.unpack(rlc_cfg_scg_r12_present, 1));
  HANDLE_CODE(bref.unpack(rlc_cfg_v1250_present, 1));
  HANDLE_CODE(bref.unpack(lc_ch_id_scg_r12_present, 1));
  HANDLE_CODE(bref.unpack(lc_ch_cfg_scg_r12_present, 1));

  HANDLE_CODE(unpack_integer(drb_id_r12, bref, (uint8_t)1u, (uint8_t)32u));
  if (drb_type_r12_present) {
    HANDLE_CODE(drb_type_r12.unpack(bref));
  }
  if (rlc_cfg_scg_r12_present) {
    HANDLE_CODE(rlc_cfg_scg_r12.unpack(bref));
  }
  if (rlc_cfg_v1250_present) {
    HANDLE_CODE(rlc_cfg_v1250.unpack(bref));
  }
  if (lc_ch_id_scg_r12_present) {
    HANDLE_CODE(unpack_integer(lc_ch_id_scg_r12, bref, (uint8_t)3u, (uint8_t)10u));
  }
  if (lc_ch_cfg_scg_r12_present) {
    HANDLE_CODE(lc_ch_cfg_scg_r12.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(3);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool rlc_cfg_v1430_present;
      HANDLE_CODE(bref.unpack(rlc_cfg_v1430_present, 1));
      rlc_cfg_v1430.set_present(rlc_cfg_v1430_present);
      if (rlc_cfg_v1430.is_present()) {
        HANDLE_CODE(rlc_cfg_v1430->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(lc_ch_id_scg_r15_present, 1));
      bool rlc_cfg_v1530_present;
      HANDLE_CODE(bref.unpack(rlc_cfg_v1530_present, 1));
      rlc_cfg_v1530.set_present(rlc_cfg_v1530_present);
      bool rlc_bearer_cfg_secondary_r15_present;
      HANDLE_CODE(bref.unpack(rlc_bearer_cfg_secondary_r15_present, 1));
      rlc_bearer_cfg_secondary_r15.set_present(rlc_bearer_cfg_secondary_r15_present);
      if (lc_ch_id_scg_r15_present) {
        HANDLE_CODE(unpack_integer(lc_ch_id_scg_r15, bref, (uint8_t)32u, (uint8_t)38u));
      }
      if (rlc_cfg_v1530.is_present()) {
        HANDLE_CODE(rlc_cfg_v1530->unpack(bref));
      }
      if (rlc_bearer_cfg_secondary_r15.is_present()) {
        HANDLE_CODE(rlc_bearer_cfg_secondary_r15->unpack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool rlc_cfg_v1560_present;
      HANDLE_CODE(bref.unpack(rlc_cfg_v1560_present, 1));
      rlc_cfg_v1560.set_present(rlc_cfg_v1560_present);
      if (rlc_cfg_v1560.is_present()) {
        HANDLE_CODE(rlc_cfg_v1560->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void drb_to_add_mod_scg_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("drb-Identity-r12", drb_id_r12);
  if (drb_type_r12_present) {
    j.write_fieldname("drb-Type-r12");
    drb_type_r12.to_json(j);
  }
  if (rlc_cfg_scg_r12_present) {
    j.write_fieldname("rlc-ConfigSCG-r12");
    rlc_cfg_scg_r12.to_json(j);
  }
  if (rlc_cfg_v1250_present) {
    j.write_fieldname("rlc-Config-v1250");
    rlc_cfg_v1250.to_json(j);
  }
  if (lc_ch_id_scg_r12_present) {
    j.write_int("logicalChannelIdentitySCG-r12", lc_ch_id_scg_r12);
  }
  if (lc_ch_cfg_scg_r12_present) {
    j.write_fieldname("logicalChannelConfigSCG-r12");
    lc_ch_cfg_scg_r12.to_json(j);
  }
  if (ext) {
    if (rlc_cfg_v1430.is_present()) {
      j.write_fieldname("rlc-Config-v1430");
      rlc_cfg_v1430->to_json(j);
    }
    if (lc_ch_id_scg_r15_present) {
      j.write_int("logicalChannelIdentitySCG-r15", lc_ch_id_scg_r15);
    }
    if (rlc_cfg_v1530.is_present()) {
      j.write_fieldname("rlc-Config-v1530");
      rlc_cfg_v1530->to_json(j);
    }
    if (rlc_bearer_cfg_secondary_r15.is_present()) {
      j.write_fieldname("rlc-BearerConfigSecondary-r15");
      rlc_bearer_cfg_secondary_r15->to_json(j);
    }
    if (rlc_cfg_v1560.is_present()) {
      j.write_fieldname("rlc-Config-v1560");
      rlc_cfg_v1560->to_json(j);
    }
  }
  j.end_obj();
}

void drb_to_add_mod_scg_r12_s::drb_type_r12_c_::set(types::options e)
{
  type_ = e;
}
void drb_to_add_mod_scg_r12_s::drb_type_r12_c_::set_split_r12()
{
  set(types::split_r12);
}
drb_to_add_mod_scg_r12_s::drb_type_r12_c_::scg_r12_s_& drb_to_add_mod_scg_r12_s::drb_type_r12_c_::set_scg_r12()
{
  set(types::scg_r12);
  return c;
}
void drb_to_add_mod_scg_r12_s::drb_type_r12_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::split_r12:
      break;
    case types::scg_r12:
      j.write_fieldname("scg-r12");
      j.start_obj();
      if (c.eps_bearer_id_r12_present) {
        j.write_int("eps-BearerIdentity-r12", c.eps_bearer_id_r12);
      }
      if (c.pdcp_cfg_r12_present) {
        j.write_fieldname("pdcp-Config-r12");
        c.pdcp_cfg_r12.to_json(j);
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "drb_to_add_mod_scg_r12_s::drb_type_r12_c_");
  }
  j.end_obj();
}
SRSASN_CODE drb_to_add_mod_scg_r12_s::drb_type_r12_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::split_r12:
      break;
    case types::scg_r12:
      HANDLE_CODE(bref.pack(c.eps_bearer_id_r12_present, 1));
      HANDLE_CODE(bref.pack(c.pdcp_cfg_r12_present, 1));
      if (c.eps_bearer_id_r12_present) {
        HANDLE_CODE(pack_integer(bref, c.eps_bearer_id_r12, (uint8_t)0u, (uint8_t)15u));
      }
      if (c.pdcp_cfg_r12_present) {
        HANDLE_CODE(c.pdcp_cfg_r12.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "drb_to_add_mod_scg_r12_s::drb_type_r12_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_to_add_mod_scg_r12_s::drb_type_r12_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::split_r12:
      break;
    case types::scg_r12:
      HANDLE_CODE(bref.unpack(c.eps_bearer_id_r12_present, 1));
      HANDLE_CODE(bref.unpack(c.pdcp_cfg_r12_present, 1));
      if (c.eps_bearer_id_r12_present) {
        HANDLE_CODE(unpack_integer(c.eps_bearer_id_r12, bref, (uint8_t)0u, (uint8_t)15u));
      }
      if (c.pdcp_cfg_r12_present) {
        HANDLE_CODE(c.pdcp_cfg_r12.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "drb_to_add_mod_scg_r12_s::drb_type_r12_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// PhysicalConfigDedicated-v13c0 ::= SEQUENCE
SRSASN_CODE phys_cfg_ded_v13c0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pucch_cfg_ded_v13c0.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE phys_cfg_ded_v13c0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(pucch_cfg_ded_v13c0.unpack(bref));

  return SRSASN_SUCCESS;
}
void phys_cfg_ded_v13c0_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("pucch-ConfigDedicated-v13c0");
  pucch_cfg_ded_v13c0.to_json(j);
  j.end_obj();
}

// RadioResourceConfigCommonSCell-v10l0 ::= SEQUENCE
SRSASN_CODE rr_cfg_common_scell_v10l0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, ul_cfg_v10l0.add_spec_emission_scell_v10l0, (uint16_t)33u, (uint16_t)288u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rr_cfg_common_scell_v10l0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(ul_cfg_v10l0.add_spec_emission_scell_v10l0, bref, (uint16_t)33u, (uint16_t)288u));

  return SRSASN_SUCCESS;
}
void rr_cfg_common_scell_v10l0_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("ul-Configuration-v10l0");
  j.start_obj();
  j.write_int("additionalSpectrumEmissionSCell-v10l0", ul_cfg_v10l0.add_spec_emission_scell_v10l0);
  j.end_obj();
  j.end_obj();
}

// RLF-TimersAndConstantsSCG-r12 ::= CHOICE
void rlf_timers_and_consts_scg_r12_c::set(types::options e)
{
  type_ = e;
}
void rlf_timers_and_consts_scg_r12_c::set_release()
{
  set(types::release);
}
rlf_timers_and_consts_scg_r12_c::setup_s_& rlf_timers_and_consts_scg_r12_c::set_setup()
{
  set(types::setup);
  return c;
}
void rlf_timers_and_consts_scg_r12_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_str("t313-r12", c.t313_r12.to_string());
      j.write_str("n313-r12", c.n313_r12.to_string());
      j.write_str("n314-r12", c.n314_r12.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "rlf_timers_and_consts_scg_r12_c");
  }
  j.end_obj();
}
SRSASN_CODE rlf_timers_and_consts_scg_r12_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      bref.pack(c.ext, 1);
      HANDLE_CODE(c.t313_r12.pack(bref));
      HANDLE_CODE(c.n313_r12.pack(bref));
      HANDLE_CODE(c.n314_r12.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rlf_timers_and_consts_scg_r12_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rlf_timers_and_consts_scg_r12_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      bref.unpack(c.ext, 1);
      HANDLE_CODE(c.t313_r12.unpack(bref));
      HANDLE_CODE(c.n313_r12.unpack(bref));
      HANDLE_CODE(c.n314_r12.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rlf_timers_and_consts_scg_r12_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* rlf_timers_and_consts_scg_r12_c::setup_s_::t313_r12_opts::to_string() const
{
  static const char* options[] = {"ms0", "ms50", "ms100", "ms200", "ms500", "ms1000", "ms2000"};
  return convert_enum_idx(options, 7, value, "rlf_timers_and_consts_scg_r12_c::setup_s_::t313_r12_e_");
}
uint16_t rlf_timers_and_consts_scg_r12_c::setup_s_::t313_r12_opts::to_number() const
{
  static const uint16_t options[] = {0, 50, 100, 200, 500, 1000, 2000};
  return map_enum_number(options, 7, value, "rlf_timers_and_consts_scg_r12_c::setup_s_::t313_r12_e_");
}

const char* rlf_timers_and_consts_scg_r12_c::setup_s_::n313_r12_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n3", "n4", "n6", "n8", "n10", "n20"};
  return convert_enum_idx(options, 8, value, "rlf_timers_and_consts_scg_r12_c::setup_s_::n313_r12_e_");
}
uint8_t rlf_timers_and_consts_scg_r12_c::setup_s_::n313_r12_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 6, 8, 10, 20};
  return map_enum_number(options, 8, value, "rlf_timers_and_consts_scg_r12_c::setup_s_::n313_r12_e_");
}

const char* rlf_timers_and_consts_scg_r12_c::setup_s_::n314_r12_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n3", "n4", "n5", "n6", "n8", "n10"};
  return convert_enum_idx(options, 8, value, "rlf_timers_and_consts_scg_r12_c::setup_s_::n314_r12_e_");
}
uint8_t rlf_timers_and_consts_scg_r12_c::setup_s_::n314_r12_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 5, 6, 8, 10};
  return map_enum_number(options, 8, value, "rlf_timers_and_consts_scg_r12_c::setup_s_::n314_r12_e_");
}

// SCellToAddMod-r10 ::= SEQUENCE
SRSASN_CODE scell_to_add_mod_r10_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(cell_identif_r10_present, 1));
  HANDLE_CODE(bref.pack(rr_cfg_common_scell_r10_present, 1));
  HANDLE_CODE(bref.pack(rr_cfg_ded_scell_r10_present, 1));

  HANDLE_CODE(pack_integer(bref, scell_idx_r10, (uint8_t)1u, (uint8_t)7u));
  if (cell_identif_r10_present) {
    HANDLE_CODE(pack_integer(bref, cell_identif_r10.pci_r10, (uint16_t)0u, (uint16_t)503u));
    HANDLE_CODE(pack_integer(bref, cell_identif_r10.dl_carrier_freq_r10, (uint32_t)0u, (uint32_t)65535u));
  }
  if (rr_cfg_common_scell_r10_present) {
    HANDLE_CODE(rr_cfg_common_scell_r10.pack(bref));
  }
  if (rr_cfg_ded_scell_r10_present) {
    HANDLE_CODE(rr_cfg_ded_scell_r10.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= dl_carrier_freq_v1090_present;
    group_flags[1] |= ant_info_ded_scell_v10i0.is_present();
    group_flags[2] |= srs_switch_from_serv_cell_idx_r14_present;
    group_flags[3] |= scell_state_r15_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(dl_carrier_freq_v1090_present, 1));
      if (dl_carrier_freq_v1090_present) {
        HANDLE_CODE(pack_integer(bref, dl_carrier_freq_v1090, (uint32_t)65536u, (uint32_t)262143u));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ant_info_ded_scell_v10i0.is_present(), 1));
      if (ant_info_ded_scell_v10i0.is_present()) {
        HANDLE_CODE(ant_info_ded_scell_v10i0->pack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(srs_switch_from_serv_cell_idx_r14_present, 1));
      if (srs_switch_from_serv_cell_idx_r14_present) {
        HANDLE_CODE(pack_integer(bref, srs_switch_from_serv_cell_idx_r14, (uint8_t)0u, (uint8_t)31u));
      }
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(scell_state_r15_present, 1));
      if (scell_state_r15_present) {
        HANDLE_CODE(scell_state_r15.pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE scell_to_add_mod_r10_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(cell_identif_r10_present, 1));
  HANDLE_CODE(bref.unpack(rr_cfg_common_scell_r10_present, 1));
  HANDLE_CODE(bref.unpack(rr_cfg_ded_scell_r10_present, 1));

  HANDLE_CODE(unpack_integer(scell_idx_r10, bref, (uint8_t)1u, (uint8_t)7u));
  if (cell_identif_r10_present) {
    HANDLE_CODE(unpack_integer(cell_identif_r10.pci_r10, bref, (uint16_t)0u, (uint16_t)503u));
    HANDLE_CODE(unpack_integer(cell_identif_r10.dl_carrier_freq_r10, bref, (uint32_t)0u, (uint32_t)65535u));
  }
  if (rr_cfg_common_scell_r10_present) {
    HANDLE_CODE(rr_cfg_common_scell_r10.unpack(bref));
  }
  if (rr_cfg_ded_scell_r10_present) {
    HANDLE_CODE(rr_cfg_ded_scell_r10.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(4);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(dl_carrier_freq_v1090_present, 1));
      if (dl_carrier_freq_v1090_present) {
        HANDLE_CODE(unpack_integer(dl_carrier_freq_v1090, bref, (uint32_t)65536u, (uint32_t)262143u));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool ant_info_ded_scell_v10i0_present;
      HANDLE_CODE(bref.unpack(ant_info_ded_scell_v10i0_present, 1));
      ant_info_ded_scell_v10i0.set_present(ant_info_ded_scell_v10i0_present);
      if (ant_info_ded_scell_v10i0.is_present()) {
        HANDLE_CODE(ant_info_ded_scell_v10i0->unpack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(srs_switch_from_serv_cell_idx_r14_present, 1));
      if (srs_switch_from_serv_cell_idx_r14_present) {
        HANDLE_CODE(unpack_integer(srs_switch_from_serv_cell_idx_r14, bref, (uint8_t)0u, (uint8_t)31u));
      }
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(scell_state_r15_present, 1));
      if (scell_state_r15_present) {
        HANDLE_CODE(scell_state_r15.unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void scell_to_add_mod_r10_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("sCellIndex-r10", scell_idx_r10);
  if (cell_identif_r10_present) {
    j.write_fieldname("cellIdentification-r10");
    j.start_obj();
    j.write_int("physCellId-r10", cell_identif_r10.pci_r10);
    j.write_int("dl-CarrierFreq-r10", cell_identif_r10.dl_carrier_freq_r10);
    j.end_obj();
  }
  if (rr_cfg_common_scell_r10_present) {
    j.write_fieldname("radioResourceConfigCommonSCell-r10");
    rr_cfg_common_scell_r10.to_json(j);
  }
  if (rr_cfg_ded_scell_r10_present) {
    j.write_fieldname("radioResourceConfigDedicatedSCell-r10");
    rr_cfg_ded_scell_r10.to_json(j);
  }
  if (ext) {
    if (dl_carrier_freq_v1090_present) {
      j.write_int("dl-CarrierFreq-v1090", dl_carrier_freq_v1090);
    }
    if (ant_info_ded_scell_v10i0.is_present()) {
      j.write_fieldname("antennaInfoDedicatedSCell-v10i0");
      ant_info_ded_scell_v10i0->to_json(j);
    }
    if (srs_switch_from_serv_cell_idx_r14_present) {
      j.write_int("srs-SwitchFromServCellIndex-r14", srs_switch_from_serv_cell_idx_r14);
    }
    if (scell_state_r15_present) {
      j.write_str("sCellState-r15", scell_state_r15.to_string());
    }
  }
  j.end_obj();
}
bool scell_to_add_mod_r10_s::operator==(const scell_to_add_mod_r10_s& other) const
{
  return ext == other.ext and scell_idx_r10 == other.scell_idx_r10 and
         cell_identif_r10.pci_r10 == other.cell_identif_r10.pci_r10 and
         cell_identif_r10.dl_carrier_freq_r10 == other.cell_identif_r10.dl_carrier_freq_r10 and
         rr_cfg_common_scell_r10_present == other.rr_cfg_common_scell_r10_present and
         (not rr_cfg_common_scell_r10_present or rr_cfg_common_scell_r10 == other.rr_cfg_common_scell_r10) and
         rr_cfg_ded_scell_r10_present == other.rr_cfg_ded_scell_r10_present and
         (not rr_cfg_ded_scell_r10_present or rr_cfg_ded_scell_r10 == other.rr_cfg_ded_scell_r10) and
         (not ext or (dl_carrier_freq_v1090_present == other.dl_carrier_freq_v1090_present and
                      (not dl_carrier_freq_v1090_present or dl_carrier_freq_v1090 == other.dl_carrier_freq_v1090) and
                      ant_info_ded_scell_v10i0.is_present() == other.ant_info_ded_scell_v10i0.is_present() and
                      (not ant_info_ded_scell_v10i0.is_present() or
                       *ant_info_ded_scell_v10i0 == *other.ant_info_ded_scell_v10i0) and
                      srs_switch_from_serv_cell_idx_r14_present == other.srs_switch_from_serv_cell_idx_r14_present and
                      (not srs_switch_from_serv_cell_idx_r14_present or
                       srs_switch_from_serv_cell_idx_r14 == other.srs_switch_from_serv_cell_idx_r14) and
                      scell_state_r15_present == other.scell_state_r15_present and
                      (not scell_state_r15_present or scell_state_r15 == other.scell_state_r15)));
}

const char* scell_to_add_mod_r10_s::scell_state_r15_opts::to_string() const
{
  static const char* options[] = {"activated", "dormant"};
  return convert_enum_idx(options, 2, value, "scell_to_add_mod_r10_s::scell_state_r15_e_");
}

// SCellToAddModExt-v1370 ::= SEQUENCE
SRSASN_CODE scell_to_add_mod_ext_v1370_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rr_cfg_common_scell_v1370_present, 1));

  if (rr_cfg_common_scell_v1370_present) {
    HANDLE_CODE(rr_cfg_common_scell_v1370.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE scell_to_add_mod_ext_v1370_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rr_cfg_common_scell_v1370_present, 1));

  if (rr_cfg_common_scell_v1370_present) {
    HANDLE_CODE(rr_cfg_common_scell_v1370.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void scell_to_add_mod_ext_v1370_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rr_cfg_common_scell_v1370_present) {
    j.write_fieldname("radioResourceConfigCommonSCell-v1370");
    rr_cfg_common_scell_v1370.to_json(j);
  }
  j.end_obj();
}

// SCellToAddModExt-v1430 ::= SEQUENCE
SRSASN_CODE scell_to_add_mod_ext_v1430_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(srs_switch_from_serv_cell_idx_r14_present, 1));

  if (srs_switch_from_serv_cell_idx_r14_present) {
    HANDLE_CODE(pack_integer(bref, srs_switch_from_serv_cell_idx_r14, (uint8_t)0u, (uint8_t)31u));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= scell_state_r15_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(scell_state_r15_present, 1));
      if (scell_state_r15_present) {
        HANDLE_CODE(scell_state_r15.pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE scell_to_add_mod_ext_v1430_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(srs_switch_from_serv_cell_idx_r14_present, 1));

  if (srs_switch_from_serv_cell_idx_r14_present) {
    HANDLE_CODE(unpack_integer(srs_switch_from_serv_cell_idx_r14, bref, (uint8_t)0u, (uint8_t)31u));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(scell_state_r15_present, 1));
      if (scell_state_r15_present) {
        HANDLE_CODE(scell_state_r15.unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void scell_to_add_mod_ext_v1430_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (srs_switch_from_serv_cell_idx_r14_present) {
    j.write_int("srs-SwitchFromServCellIndex-r14", srs_switch_from_serv_cell_idx_r14);
  }
  if (ext) {
    if (scell_state_r15_present) {
      j.write_str("sCellState-r15", scell_state_r15.to_string());
    }
  }
  j.end_obj();
}

const char* scell_to_add_mod_ext_v1430_s::scell_state_r15_opts::to_string() const
{
  static const char* options[] = {"activated", "dormant"};
  return convert_enum_idx(options, 2, value, "scell_to_add_mod_ext_v1430_s::scell_state_r15_e_");
}

// RadioResourceConfigDedicatedSCG-r12 ::= SEQUENCE
SRSASN_CODE rr_cfg_ded_scg_r12_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(drb_to_add_mod_list_scg_r12_present, 1));
  HANDLE_CODE(bref.pack(mac_main_cfg_scg_r12_present, 1));
  HANDLE_CODE(bref.pack(rlf_timers_and_consts_scg_r12_present, 1));

  if (drb_to_add_mod_list_scg_r12_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, drb_to_add_mod_list_scg_r12, 1, 11));
  }
  if (mac_main_cfg_scg_r12_present) {
    HANDLE_CODE(mac_main_cfg_scg_r12.pack(bref));
  }
  if (rlf_timers_and_consts_scg_r12_present) {
    HANDLE_CODE(rlf_timers_and_consts_scg_r12.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= drb_to_add_mod_list_scg_r15.is_present();
    group_flags[1] |= srb_to_add_mod_list_scg_r15.is_present();
    group_flags[1] |= srb_to_release_list_scg_r15.is_present();
    group_flags[2] |= drb_to_release_list_scg_r15.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(drb_to_add_mod_list_scg_r15.is_present(), 1));
      if (drb_to_add_mod_list_scg_r15.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *drb_to_add_mod_list_scg_r15, 1, 15));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(srb_to_add_mod_list_scg_r15.is_present(), 1));
      HANDLE_CODE(bref.pack(srb_to_release_list_scg_r15.is_present(), 1));
      if (srb_to_add_mod_list_scg_r15.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *srb_to_add_mod_list_scg_r15, 1, 2));
      }
      if (srb_to_release_list_scg_r15.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *srb_to_release_list_scg_r15, 1, 2, integer_packer<uint8_t>(1, 2)));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(drb_to_release_list_scg_r15.is_present(), 1));
      if (drb_to_release_list_scg_r15.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *drb_to_release_list_scg_r15, 1, 15, integer_packer<uint8_t>(1, 32)));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rr_cfg_ded_scg_r12_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(drb_to_add_mod_list_scg_r12_present, 1));
  HANDLE_CODE(bref.unpack(mac_main_cfg_scg_r12_present, 1));
  HANDLE_CODE(bref.unpack(rlf_timers_and_consts_scg_r12_present, 1));

  if (drb_to_add_mod_list_scg_r12_present) {
    HANDLE_CODE(unpack_dyn_seq_of(drb_to_add_mod_list_scg_r12, bref, 1, 11));
  }
  if (mac_main_cfg_scg_r12_present) {
    HANDLE_CODE(mac_main_cfg_scg_r12.unpack(bref));
  }
  if (rlf_timers_and_consts_scg_r12_present) {
    HANDLE_CODE(rlf_timers_and_consts_scg_r12.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(3);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool drb_to_add_mod_list_scg_r15_present;
      HANDLE_CODE(bref.unpack(drb_to_add_mod_list_scg_r15_present, 1));
      drb_to_add_mod_list_scg_r15.set_present(drb_to_add_mod_list_scg_r15_present);
      if (drb_to_add_mod_list_scg_r15.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*drb_to_add_mod_list_scg_r15, bref, 1, 15));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool srb_to_add_mod_list_scg_r15_present;
      HANDLE_CODE(bref.unpack(srb_to_add_mod_list_scg_r15_present, 1));
      srb_to_add_mod_list_scg_r15.set_present(srb_to_add_mod_list_scg_r15_present);
      bool srb_to_release_list_scg_r15_present;
      HANDLE_CODE(bref.unpack(srb_to_release_list_scg_r15_present, 1));
      srb_to_release_list_scg_r15.set_present(srb_to_release_list_scg_r15_present);
      if (srb_to_add_mod_list_scg_r15.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*srb_to_add_mod_list_scg_r15, bref, 1, 2));
      }
      if (srb_to_release_list_scg_r15.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*srb_to_release_list_scg_r15, bref, 1, 2, integer_packer<uint8_t>(1, 2)));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool drb_to_release_list_scg_r15_present;
      HANDLE_CODE(bref.unpack(drb_to_release_list_scg_r15_present, 1));
      drb_to_release_list_scg_r15.set_present(drb_to_release_list_scg_r15_present);
      if (drb_to_release_list_scg_r15.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*drb_to_release_list_scg_r15, bref, 1, 15, integer_packer<uint8_t>(1, 32)));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void rr_cfg_ded_scg_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (drb_to_add_mod_list_scg_r12_present) {
    j.start_array("drb-ToAddModListSCG-r12");
    for (const auto& e1 : drb_to_add_mod_list_scg_r12) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (mac_main_cfg_scg_r12_present) {
    j.write_fieldname("mac-MainConfigSCG-r12");
    mac_main_cfg_scg_r12.to_json(j);
  }
  if (rlf_timers_and_consts_scg_r12_present) {
    j.write_fieldname("rlf-TimersAndConstantsSCG-r12");
    rlf_timers_and_consts_scg_r12.to_json(j);
  }
  if (ext) {
    if (drb_to_add_mod_list_scg_r15.is_present()) {
      j.start_array("drb-ToAddModListSCG-r15");
      for (const auto& e1 : *drb_to_add_mod_list_scg_r15) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (srb_to_add_mod_list_scg_r15.is_present()) {
      j.start_array("srb-ToAddModListSCG-r15");
      for (const auto& e1 : *srb_to_add_mod_list_scg_r15) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (srb_to_release_list_scg_r15.is_present()) {
      j.start_array("srb-ToReleaseListSCG-r15");
      for (const auto& e1 : *srb_to_release_list_scg_r15) {
        j.write_int(e1);
      }
      j.end_array();
    }
    if (drb_to_release_list_scg_r15.is_present()) {
      j.start_array("drb-ToReleaseListSCG-r15");
      for (const auto& e1 : *drb_to_release_list_scg_r15) {
        j.write_int(e1);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

// PhysicalConfigDedicatedSCell-v13c0 ::= SEQUENCE
SRSASN_CODE phys_cfg_ded_scell_v13c0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pucch_scell_v13c0.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE phys_cfg_ded_scell_v13c0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(pucch_scell_v13c0.unpack(bref));

  return SRSASN_SUCCESS;
}
void phys_cfg_ded_scell_v13c0_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("pucch-SCell-v13c0");
  pucch_scell_v13c0.to_json(j);
  j.end_obj();
}

void phys_cfg_ded_scell_v13c0_s::pucch_scell_v13c0_c_::set(types::options e)
{
  type_ = e;
}
void phys_cfg_ded_scell_v13c0_s::pucch_scell_v13c0_c_::set_release()
{
  set(types::release);
}
phys_cfg_ded_scell_v13c0_s::pucch_scell_v13c0_c_::setup_s_&
phys_cfg_ded_scell_v13c0_s::pucch_scell_v13c0_c_::set_setup()
{
  set(types::setup);
  return c;
}
void phys_cfg_ded_scell_v13c0_s::pucch_scell_v13c0_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_fieldname("pucch-ConfigDedicated-v13c0");
      c.pucch_cfg_ded_v13c0.to_json(j);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_scell_v13c0_s::pucch_scell_v13c0_c_");
  }
  j.end_obj();
}
SRSASN_CODE phys_cfg_ded_scell_v13c0_s::pucch_scell_v13c0_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.pucch_cfg_ded_v13c0.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_scell_v13c0_s::pucch_scell_v13c0_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE phys_cfg_ded_scell_v13c0_s::pucch_scell_v13c0_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(c.pucch_cfg_ded_v13c0.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "phys_cfg_ded_scell_v13c0_s::pucch_scell_v13c0_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// RadioResourceConfigDedicatedSCell-v13c0 ::= SEQUENCE
SRSASN_CODE rr_cfg_ded_scell_v13c0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(phys_cfg_ded_scell_v13c0.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rr_cfg_ded_scell_v13c0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(phys_cfg_ded_scell_v13c0.unpack(bref));

  return SRSASN_SUCCESS;
}
void rr_cfg_ded_scell_v13c0_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("physicalConfigDedicatedSCell-v13c0");
  phys_cfg_ded_scell_v13c0.to_json(j);
  j.end_obj();
}

// SCellToAddMod-v13c0 ::= SEQUENCE
SRSASN_CODE scell_to_add_mod_v13c0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rr_cfg_ded_scell_v13c0_present, 1));

  if (rr_cfg_ded_scell_v13c0_present) {
    HANDLE_CODE(rr_cfg_ded_scell_v13c0.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE scell_to_add_mod_v13c0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rr_cfg_ded_scell_v13c0_present, 1));

  if (rr_cfg_ded_scell_v13c0_present) {
    HANDLE_CODE(rr_cfg_ded_scell_v13c0.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void scell_to_add_mod_v13c0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rr_cfg_ded_scell_v13c0_present) {
    j.write_fieldname("radioResourceConfigDedicatedSCell-v13c0");
    rr_cfg_ded_scell_v13c0.to_json(j);
  }
  j.end_obj();
}

// SCellToAddMod-v10l0 ::= SEQUENCE
SRSASN_CODE scell_to_add_mod_v10l0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rr_cfg_common_scell_v10l0_present, 1));

  if (rr_cfg_common_scell_v10l0_present) {
    HANDLE_CODE(rr_cfg_common_scell_v10l0.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE scell_to_add_mod_v10l0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rr_cfg_common_scell_v10l0_present, 1));

  if (rr_cfg_common_scell_v10l0_present) {
    HANDLE_CODE(rr_cfg_common_scell_v10l0.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void scell_to_add_mod_v10l0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rr_cfg_common_scell_v10l0_present) {
    j.write_fieldname("radioResourceConfigCommonSCell-v10l0");
    rr_cfg_common_scell_v10l0.to_json(j);
  }
  j.end_obj();
}

// RadioResourceConfigDedicated-v13c0 ::= SEQUENCE
SRSASN_CODE rr_cfg_ded_v13c0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(phys_cfg_ded_v13c0.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rr_cfg_ded_v13c0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(phys_cfg_ded_v13c0.unpack(bref));

  return SRSASN_SUCCESS;
}
void rr_cfg_ded_v13c0_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("physicalConfigDedicated-v13c0");
  phys_cfg_ded_v13c0.to_json(j);
  j.end_obj();
}

// Cell-ToAddMod-r12 ::= SEQUENCE
SRSASN_CODE cell_to_add_mod_r12_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(cell_identif_r12_present, 1));
  HANDLE_CODE(bref.pack(meas_result_cell_to_add_r12_present, 1));

  HANDLE_CODE(pack_integer(bref, scell_idx_r12, (uint8_t)1u, (uint8_t)7u));
  if (cell_identif_r12_present) {
    HANDLE_CODE(pack_integer(bref, cell_identif_r12.pci_r12, (uint16_t)0u, (uint16_t)503u));
    HANDLE_CODE(pack_integer(bref, cell_identif_r12.dl_carrier_freq_r12, (uint32_t)0u, (uint32_t)262143u));
  }
  if (meas_result_cell_to_add_r12_present) {
    HANDLE_CODE(pack_integer(bref, meas_result_cell_to_add_r12.rsrp_result_r12, (uint8_t)0u, (uint8_t)97u));
    HANDLE_CODE(pack_integer(bref, meas_result_cell_to_add_r12.rsrq_result_r12, (uint8_t)0u, (uint8_t)34u));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= scell_idx_r13_present;
    group_flags[0] |= meas_result_cell_to_add_v1310.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(scell_idx_r13_present, 1));
      HANDLE_CODE(bref.pack(meas_result_cell_to_add_v1310.is_present(), 1));
      if (scell_idx_r13_present) {
        HANDLE_CODE(pack_integer(bref, scell_idx_r13, (uint8_t)1u, (uint8_t)31u));
      }
      if (meas_result_cell_to_add_v1310.is_present()) {
        HANDLE_CODE(pack_integer(bref, meas_result_cell_to_add_v1310->rs_sinr_result_r13, (uint8_t)0u, (uint8_t)127u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_to_add_mod_r12_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(cell_identif_r12_present, 1));
  HANDLE_CODE(bref.unpack(meas_result_cell_to_add_r12_present, 1));

  HANDLE_CODE(unpack_integer(scell_idx_r12, bref, (uint8_t)1u, (uint8_t)7u));
  if (cell_identif_r12_present) {
    HANDLE_CODE(unpack_integer(cell_identif_r12.pci_r12, bref, (uint16_t)0u, (uint16_t)503u));
    HANDLE_CODE(unpack_integer(cell_identif_r12.dl_carrier_freq_r12, bref, (uint32_t)0u, (uint32_t)262143u));
  }
  if (meas_result_cell_to_add_r12_present) {
    HANDLE_CODE(unpack_integer(meas_result_cell_to_add_r12.rsrp_result_r12, bref, (uint8_t)0u, (uint8_t)97u));
    HANDLE_CODE(unpack_integer(meas_result_cell_to_add_r12.rsrq_result_r12, bref, (uint8_t)0u, (uint8_t)34u));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(scell_idx_r13_present, 1));
      bool meas_result_cell_to_add_v1310_present;
      HANDLE_CODE(bref.unpack(meas_result_cell_to_add_v1310_present, 1));
      meas_result_cell_to_add_v1310.set_present(meas_result_cell_to_add_v1310_present);
      if (scell_idx_r13_present) {
        HANDLE_CODE(unpack_integer(scell_idx_r13, bref, (uint8_t)1u, (uint8_t)31u));
      }
      if (meas_result_cell_to_add_v1310.is_present()) {
        HANDLE_CODE(
            unpack_integer(meas_result_cell_to_add_v1310->rs_sinr_result_r13, bref, (uint8_t)0u, (uint8_t)127u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void cell_to_add_mod_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("sCellIndex-r12", scell_idx_r12);
  if (cell_identif_r12_present) {
    j.write_fieldname("cellIdentification-r12");
    j.start_obj();
    j.write_int("physCellId-r12", cell_identif_r12.pci_r12);
    j.write_int("dl-CarrierFreq-r12", cell_identif_r12.dl_carrier_freq_r12);
    j.end_obj();
  }
  if (meas_result_cell_to_add_r12_present) {
    j.write_fieldname("measResultCellToAdd-r12");
    j.start_obj();
    j.write_int("rsrpResult-r12", meas_result_cell_to_add_r12.rsrp_result_r12);
    j.write_int("rsrqResult-r12", meas_result_cell_to_add_r12.rsrq_result_r12);
    j.end_obj();
  }
  if (ext) {
    if (scell_idx_r13_present) {
      j.write_int("sCellIndex-r13", scell_idx_r13);
    }
    if (meas_result_cell_to_add_v1310.is_present()) {
      j.write_fieldname("measResultCellToAdd-v1310");
      j.start_obj();
      j.write_int("rs-sinr-Result-r13", meas_result_cell_to_add_v1310->rs_sinr_result_r13);
      j.end_obj();
    }
  }
  j.end_obj();
}
