/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#ifndef SRSRAN_RLC_AM_NR_PACKING_H
#define SRSRAN_RLC_AM_NR_PACKING_H

#include "srsran/common/string_helpers.h"
#include "srsran/rlc/rlc_am_base.h"
#include <set>

namespace srsran {

const uint32_t INVALID_RLC_SN         = 0xFFFFFFFF;
const uint32_t RETX_COUNT_NOT_STARTED = 0xFFFFFFFF;

///< AM NR PDU header
struct rlc_am_nr_pdu_header_t {
  rlc_am_nr_pdu_header_t()                                = default;
  rlc_am_nr_pdu_header_t(const rlc_am_nr_pdu_header_t& h) = default;
  rlc_am_nr_pdu_header_t& operator=(const rlc_am_nr_pdu_header_t&) = default;
  rlc_am_nr_pdu_header_t(rlc_am_nr_pdu_header_t&& h)               = default;
  ~rlc_am_nr_pdu_header_t()                                        = default;

  rlc_am_nr_pdu_header_t& operator=(rlc_am_nr_pdu_header_t&& h) = default;

  rlc_dc_field_t      dc      = {}; ///< Data/Control (D/C) field
  uint8_t             p       = {}; ///< Polling bit
  rlc_nr_si_field_t   si      = {}; ///< Segmentation info
  rlc_am_nr_sn_size_t sn_size = {}; ///< Sequence number size (12 or 18 bits)
  uint32_t            sn      = {}; ///< Sequence number
  uint16_t            so      = {}; ///< Sequence offset
};

struct rlc_amd_pdu_nr_t {
  rlc_am_nr_pdu_header_t header;
  unique_byte_buffer_t   buf;
};

struct rlc_amd_rx_pdu_nr {
  rlc_am_nr_pdu_header_t header = {};
  unique_byte_buffer_t   buf    = nullptr;
  uint32_t               rlc_sn = {};

  rlc_amd_rx_pdu_nr() = default;
  explicit rlc_amd_rx_pdu_nr(uint32_t rlc_sn_) : rlc_sn(rlc_sn_) {}
};

struct rlc_amd_rx_pdu_nr_cmp {
  bool operator()(const rlc_amd_rx_pdu_nr& a, const rlc_amd_rx_pdu_nr& b) const { return a.header.so < b.header.so; }
};

struct rlc_amd_rx_sdu_nr_t {
  uint32_t             rlc_sn         = 0;
  bool                 fully_received = false;
  bool                 has_gap        = false;
  unique_byte_buffer_t buf;
  using segment_list_t = std::set<rlc_amd_rx_pdu_nr, rlc_amd_rx_pdu_nr_cmp>;
  segment_list_t segments;

  rlc_amd_rx_sdu_nr_t() = default;
  explicit rlc_amd_rx_sdu_nr_t(uint32_t rlc_sn_) : rlc_sn(rlc_sn_) {}
};

struct rlc_amd_tx_sdu_nr_t {
  uint32_t             rlc_sn = INVALID_RLC_SN;
  unique_byte_buffer_t buf;

  rlc_amd_tx_sdu_nr_t() = default;
  explicit rlc_amd_tx_sdu_nr_t(uint32_t rlc_sn_) : rlc_sn(rlc_sn_) {}
};

constexpr uint32_t rlc_am_nr_status_pdu_sizeof_header_ack_sn        = 3; ///< header fixed part and ACK SN
constexpr uint32_t rlc_am_nr_status_pdu_sizeof_nack_sn_ext_12bit_sn = 2; ///< NACK SN and extension fields (12 bit SN)
constexpr uint32_t rlc_am_nr_status_pdu_sizeof_nack_sn_ext_18bit_sn = 3; ///< NACK SN and extension fields (18 bit SN)
constexpr uint32_t rlc_am_nr_status_pdu_sizeof_nack_so              = 4; ///< NACK segment offsets (start and end)
constexpr uint32_t rlc_am_nr_status_pdu_sizeof_nack_range           = 1; ///< NACK range (nof consecutively lost SDUs)

/// AM NR Status PDU header
class rlc_am_nr_status_pdu_t
{
private:
  /// Stored SN size required to compute the packed size
  rlc_am_nr_sn_size_t sn_size = rlc_am_nr_sn_size_t::nulltype;
  /// Stored modulus to determine continuous sequences across SN overflows
  uint32_t mod_nr = cardinality(rlc_am_nr_sn_size_t::nulltype);
  /// Internal NACK container; keep in sync with packed_size_
  std::vector<rlc_status_nack_t> nacks_ = {};
  /// Stores the current packed size; sync on each change of nacks_
  uint32_t packed_size_ = rlc_am_nr_status_pdu_sizeof_header_ack_sn;

  void     refresh_packed_size();
  uint32_t nack_size(const rlc_status_nack_t& nack) const;

public:
  /// CPT header
  rlc_am_nr_control_pdu_type_t cpt = rlc_am_nr_control_pdu_type_t::status_pdu;
  /// SN of the next not received RLC Data PDU
  uint32_t ack_sn = INVALID_RLC_SN;
  /// Read-only reference to NACKs
  const std::vector<rlc_status_nack_t>& nacks = nacks_;
  /// Read-only reference to packed size
  const uint32_t& packed_size = packed_size_;

  rlc_am_nr_status_pdu_t(rlc_am_nr_sn_size_t sn_size);
  void reset();
  bool is_continuous_sequence(const rlc_status_nack_t& left, const rlc_status_nack_t& right) const;
  void push_nack(const rlc_status_nack_t& nack);
  const std::vector<rlc_status_nack_t>& get_nacks() const { return nacks_; }
  uint32_t                              get_packed_size() const { return packed_size; }
  bool                                  trim(uint32_t max_packed_size);
};

/****************************************************************************
 * Header pack/unpack helper functions for NR
 * Ref: 3GPP TS 38.322 v15.3.0 Section 6.2.2.3
 ***************************************************************************/
uint32_t rlc_am_nr_read_data_pdu_header(const byte_buffer_t*      pdu,
                                        const rlc_am_nr_sn_size_t sn_size,
                                        rlc_am_nr_pdu_header_t*   header);

uint32_t rlc_am_nr_read_data_pdu_header(const uint8_t*            payload,
                                        const uint32_t            nof_bytes,
                                        const rlc_am_nr_sn_size_t sn_size,
                                        rlc_am_nr_pdu_header_t*   header);

uint32_t rlc_am_nr_write_data_pdu_header(const rlc_am_nr_pdu_header_t& header, uint8_t* payload);
uint32_t rlc_am_nr_write_data_pdu_header(const rlc_am_nr_pdu_header_t& header, byte_buffer_t* pdu);

uint32_t rlc_am_nr_packed_length(const rlc_am_nr_pdu_header_t& header);

/****************************************************************************
 * Status PDU pack/unpack helper functions for NR
 * Ref: 3GPP TS 38.322 v16.2.0 Section 6.2.2.5
 ***************************************************************************/
uint32_t
rlc_am_nr_read_status_pdu(const byte_buffer_t* pdu, const rlc_am_nr_sn_size_t sn_size, rlc_am_nr_status_pdu_t* status);

uint32_t rlc_am_nr_read_status_pdu(const uint8_t*            payload,
                                   const uint32_t            nof_bytes,
                                   const rlc_am_nr_sn_size_t sn_size,
                                   rlc_am_nr_status_pdu_t*   status);
uint32_t
rlc_am_nr_read_status_pdu_12bit_sn(const uint8_t* payload, const uint32_t nof_bytes, rlc_am_nr_status_pdu_t* status);
uint32_t
rlc_am_nr_read_status_pdu_18bit_sn(const uint8_t* payload, const uint32_t nof_bytes, rlc_am_nr_status_pdu_t* status);

int32_t rlc_am_nr_write_status_pdu(const rlc_am_nr_status_pdu_t& status_pdu,
                                   const rlc_am_nr_sn_size_t     sn_size,
                                   byte_buffer_t*                pdu);
int32_t rlc_am_nr_write_status_pdu_12bit_sn(const rlc_am_nr_status_pdu_t& status_pdu, byte_buffer_t* pdu);
int32_t rlc_am_nr_write_status_pdu_18bit_sn(const rlc_am_nr_status_pdu_t& status_pdu, byte_buffer_t* pdu);

/**
 * Logs Status PDU into provided log channel, using fmt_str as format string
 */
template <typename... Args>
void log_rlc_am_nr_status_pdu_to_string(srslog::log_channel&    log_ch,
                                        const char*             fmt_str,
                                        rlc_am_nr_status_pdu_t* status,
                                        const std::string&      rb_name,
                                        Args&&... args)
{
  if (not log_ch.enabled()) {
    return;
  }
  fmt::memory_buffer buffer;
  fmt::format_to(buffer, "ACK_SN = {}, N_nack = {}", status->ack_sn, status->nacks.size());
  if (status->nacks.size() > 0) {
    fmt::format_to(buffer, ", NACK_SN = ");
    for (uint32_t i = 0; i < status->nacks.size(); ++i) {
      if (status->nacks[i].has_nack_range) {
        if (status->nacks[i].has_so) {
          fmt::format_to(buffer,
                         "[{} {}:{} r{}]",
                         status->nacks[i].nack_sn,
                         status->nacks[i].so_start,
                         status->nacks[i].so_end,
                         status->nacks[i].nack_range);
        } else {
          fmt::format_to(buffer, "[{} r{}]", status->nacks[i].nack_sn, status->nacks[i].nack_range);
        }
      } else {
        if (status->nacks[i].has_so) {
          fmt::format_to(
              buffer, "[{} {}:{}]", status->nacks[i].nack_sn, status->nacks[i].so_start, status->nacks[i].so_end);
        } else {
          fmt::format_to(buffer, "[{}]", status->nacks[i].nack_sn);
        }
      }
    }
  }
  log_ch(fmt_str, std::forward<Args>(args)..., to_c_str(buffer));
}

/*
 * Log NR AMD PDUs
 */
inline void log_rlc_am_nr_pdu_header_to_string(srslog::log_channel&          log_ch,
                                               const rlc_am_nr_pdu_header_t& header,
                                               const std::string&            rb_name)
{
  if (not log_ch.enabled()) {
    return;
  }
  fmt::memory_buffer buffer;
  fmt::format_to(buffer,
                 "{}: [{}, P={}, SI={}, SN_SIZE={}, SN={}, SO={}",
                 rb_name,
                 rlc_dc_field_text[header.dc],
                 (header.p ? "1" : "0"),
                 to_string_short(header.si),
                 to_string(header.sn_size),
                 header.sn,
                 header.so);
  fmt::format_to(buffer, "]");

  log_ch("%s", to_c_str(buffer));
}
} // namespace srsran

#endif // SRSRAN_RLC_AM_NR_PACKING_H
