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

#ifndef SRSRAN_RLC_AM_LTE_PACKING_H
#define SRSRAN_RLC_AM_LTE_PACKING_H

#include "srsran/common/string_helpers.h"
#include "srsran/rlc/rlc_am_base.h"
#include "srsran/rlc/rlc_am_data_structs.h" // required for rlc_am_pdu_segment
#include <list>

namespace srsran {

struct rlc_sn_info_t {
  uint32_t sn;
  bool     is_acked;
};

struct rlc_amd_rx_pdu {
  rlc_amd_pdu_header_t header;
  unique_byte_buffer_t buf;
  uint32_t             rlc_sn = 0;

  rlc_amd_rx_pdu() = default;
  explicit rlc_amd_rx_pdu(uint32_t rlc_sn_) : rlc_sn(rlc_sn_) {}
};

struct rlc_amd_rx_pdu_segments_t {
  std::list<rlc_amd_rx_pdu> segments;
};

/****************************************************************************
 * Header pack/unpack helper functions
 * Ref: 3GPP TS 36.322 v10.0.0 Section 6.2.1
 ***************************************************************************/
void rlc_am_read_data_pdu_header(byte_buffer_t* pdu, rlc_amd_pdu_header_t* header);
void rlc_am_read_data_pdu_header(uint8_t** payload, uint32_t* nof_bytes, rlc_amd_pdu_header_t* header);
void rlc_am_write_data_pdu_header(rlc_amd_pdu_header_t* header, byte_buffer_t* pdu);
void rlc_am_write_data_pdu_header(rlc_amd_pdu_header_t* header, uint8_t** payload);
void rlc_am_read_status_pdu(byte_buffer_t* pdu, rlc_status_pdu_t* status);
void rlc_am_read_status_pdu(uint8_t* payload, uint32_t nof_bytes, rlc_status_pdu_t* status);
void rlc_am_write_status_pdu(rlc_status_pdu_t* status, byte_buffer_t* pdu);
int  rlc_am_write_status_pdu(rlc_status_pdu_t* status, uint8_t* payload);

uint32_t rlc_am_packed_length(rlc_amd_pdu_header_t* header);
uint32_t rlc_am_packed_length(rlc_status_pdu_t* status);
uint32_t rlc_am_packed_length(rlc_amd_retx_lte_t retx);
bool     rlc_am_is_pdu_segment(uint8_t* payload);
bool     rlc_am_is_valid_status_pdu(const rlc_status_pdu_t& status, uint32_t rx_win_min = 0);
bool     rlc_am_start_aligned(const uint8_t fi);
bool     rlc_am_end_aligned(const uint8_t fi);
bool     rlc_am_is_unaligned(const uint8_t fi);
bool     rlc_am_not_start_aligned(const uint8_t fi);
std::string
rlc_am_undelivered_sdu_info_to_string(const std::map<uint32_t, pdcp_pdu_info<rlc_amd_pdu_header_t> >& info_queue);

template <typename... Args>
void log_rlc_amd_pdu_header_to_string(srslog::log_channel&        log_ch,
                                      const std::string&          rb_name,
                                      const char*                 fmt_str,
                                      const rlc_amd_pdu_header_t& header,
                                      Args&&... args)
{
  if (not log_ch.enabled()) {
    return;
  }

  fmt::memory_buffer buffer;
  fmt::format_to(buffer,
                 "{}: [{}, RF={}, P={}, FI={}, SN={}, LSF={}, SO={}, N_li={}",
                 rb_name,
                 rlc_dc_field_text[header.dc],
                 (header.rf ? "1" : "0"),
                 (header.p ? "1" : "0"),
                 (header.fi ? "1" : "0"),
                 header.sn,
                 (header.lsf ? "1" : "0"),
                 header.so,
                 header.N_li);
  if (header.N_li > 0) {
    fmt::format_to(buffer, " ({}", header.li[0]);
    for (uint32_t i = 1; i < header.N_li; ++i) {
      fmt::format_to(buffer, ", {}", header.li[i]);
    }
    fmt::format_to(buffer, ")");
  }
  fmt::format_to(buffer, "]");

  log_ch(fmt_str, std::forward<Args>(args)..., to_c_str(buffer));
}

template <typename... Args>
void log_rlc_am_status_pdu_to_string(srslog::log_channel& log_ch,
                                     const std::string&   rb_name,
                                     const char*          fmt_str,
                                     rlc_status_pdu_t*    status,
                                     Args&&... args)
{
  if (not log_ch.enabled()) {
    return;
  }
  fmt::memory_buffer buffer;
  fmt::format_to(buffer, "{}: ACK_SN = {}, N_nack = {}", rb_name, status->ack_sn, status->N_nack);
  if (status->N_nack > 0) {
    fmt::format_to(buffer, ", NACK_SN = ");
    for (uint32_t i = 0; i < status->N_nack; ++i) {
      if (status->nacks[i].has_so) {
        fmt::format_to(
            buffer, "[{} {}:{}]", status->nacks[i].nack_sn, status->nacks[i].so_start, status->nacks[i].so_end);
      } else {
        fmt::format_to(buffer, "[{}]", status->nacks[i].nack_sn);
      }
    }
  }
  log_ch(fmt_str, std::forward<Args>(args)..., to_c_str(buffer));
}
} // namespace srsran

#endif // SRSRAN_RLC_AM_LTE_PACKING_H
