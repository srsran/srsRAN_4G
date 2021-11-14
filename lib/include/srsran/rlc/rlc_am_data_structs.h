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

#ifndef SRSRAN_RLC_AM_DATA_STRUCTS_H
#define SRSRAN_RLC_AM_DATA_STRUCTS_H

#include "srsran/adt/circular_buffer.h"
#include "srsran/adt/circular_map.h"
#include "srsran/adt/intrusive_list.h"
#include <array>
#include <vector>

namespace srsran {

class rlc_amd_tx_pdu;
class pdcp_pdu_info;

/// Pool that manages the allocation of RLC AM PDU Segments to RLC PDUs and tracking of segments ACK state
struct rlc_am_pdu_segment_pool {
  const static size_t MAX_POOL_SIZE = 16384;
  using rlc_list_tag                = default_intrusive_tag;
  struct free_list_tag {};

  /// RLC AM PDU Segment, containing the PDCP SN and RLC SN it has been assigned to, and its current ACK state
  struct segment_resource : public intrusive_forward_list_element<rlc_list_tag>,
                            public intrusive_forward_list_element<free_list_tag>,
                            public intrusive_double_linked_list_element<> {
    const static uint32_t invalid_rlc_sn  = std::numeric_limits<uint32_t>::max();
    const static uint32_t invalid_pdcp_sn = std::numeric_limits<uint32_t>::max() - 1; // -1 for Status Report

    int      id() const;
    void     release_pdcp_sn();
    void     release_rlc_sn();
    uint32_t rlc_sn() const { return rlc_sn_; }
    uint32_t pdcp_sn() const { return pdcp_sn_; }
    bool     empty() const { return rlc_sn_ == invalid_rlc_sn and pdcp_sn_ == invalid_pdcp_sn; }

  private:
    friend struct rlc_am_pdu_segment_pool;
    uint32_t                 rlc_sn_     = invalid_rlc_sn;
    uint32_t                 pdcp_sn_    = invalid_pdcp_sn;
    rlc_am_pdu_segment_pool* parent_pool = nullptr;
  };

  rlc_am_pdu_segment_pool();
  rlc_am_pdu_segment_pool(const rlc_am_pdu_segment_pool&) = delete;
  rlc_am_pdu_segment_pool(rlc_am_pdu_segment_pool&&)      = delete;
  rlc_am_pdu_segment_pool& operator=(const rlc_am_pdu_segment_pool&) = delete;
  rlc_am_pdu_segment_pool& operator=(rlc_am_pdu_segment_pool&&) = delete;
  bool                     has_segments() const { return not free_list.empty(); }
  bool                     make_segment(rlc_amd_tx_pdu& rlc_list, pdcp_pdu_info& pdcp_info);

private:
  intrusive_forward_list<segment_resource, free_list_tag> free_list;
  std::array<segment_resource, MAX_POOL_SIZE>             segments;
};

/// RLC AM PDU Segment, containing the PDCP SN and RLC SN it has been assigned to, and its current ACK state
using rlc_am_pdu_segment = rlc_am_pdu_segment_pool::segment_resource;

/// Class that contains the parameters and state (e.g. unACKed segments) of a PDCP PDU
class pdcp_pdu_info
{
  using list_type = intrusive_double_linked_list<rlc_am_pdu_segment>;

  list_type list; // List of unACKed RLC PDUs that contain segments that belong to the PDCP PDU.

public:
  const static uint32_t status_report_sn = std::numeric_limits<uint32_t>::max();
  const static uint32_t invalid_pdcp_sn  = std::numeric_limits<uint32_t>::max() - 1;

  using iterator       = typename list_type::iterator;
  using const_iterator = typename list_type::const_iterator;

  // Copy is forbidden to avoid multiple PDCP SN references to the same segment
  pdcp_pdu_info()                              = default;
  pdcp_pdu_info(pdcp_pdu_info&&) noexcept      = default;
  pdcp_pdu_info(const pdcp_pdu_info&) noexcept = delete;
  pdcp_pdu_info& operator=(const pdcp_pdu_info&) noexcept = delete;
  pdcp_pdu_info& operator=(pdcp_pdu_info&&) noexcept = default;
  ~pdcp_pdu_info() { clear(); }

  uint32_t sn         = invalid_pdcp_sn;
  bool     fully_txed = false; // Boolean indicating if the SDU is fully transmitted.

  bool fully_acked() const { return fully_txed and list.empty(); }
  bool valid() const { return sn != invalid_pdcp_sn; }

  // Interface for list of unACKed RLC segments of the PDCP PDU
  void add_segment(rlc_am_pdu_segment& segment) { list.push_front(&segment); }
  void ack_segment(rlc_am_pdu_segment& segment);
  void clear()
  {
    sn         = invalid_pdcp_sn;
    fully_txed = false;
    while (not list.empty()) {
      ack_segment(list.front());
    }
  }
  const_iterator begin() const { return list.begin(); }
  const_iterator end() const { return list.end(); }
};

template <class T, std::size_t WINDOW_SIZE>
struct rlc_ringbuffer_t {
  T& add_pdu(size_t sn)
  {
    srsran_expect(not has_sn(sn), "The same SN=%zd should not be added twice", sn);
    window.overwrite(sn, T(sn));
    return window[sn];
  }
  void remove_pdu(size_t sn)
  {
    srsran_expect(has_sn(sn), "The removed SN=%zd is not in the window", sn);
    window.erase(sn);
  }
  T&     operator[](size_t sn) { return window[sn]; }
  size_t size() const { return window.size(); }
  bool   empty() const { return window.empty(); }
  void   clear() { window.clear(); }

  bool has_sn(uint32_t sn) const { return window.contains(sn); }

  // Return the sum data bytes of all active PDUs (check PDU is non-null)
  uint32_t get_buffered_bytes()
  {
    uint32_t buff_size = 0;
    for (const auto& pdu : window) {
      if (pdu.second.buf != nullptr) {
        buff_size += pdu.second.buf->N_bytes;
      }
    }
    return buff_size;
  }

private:
  srsran::static_circular_map<uint32_t, T, WINDOW_SIZE> window;
};

struct buffered_pdcp_pdu_list {
public:
  explicit buffered_pdcp_pdu_list() : buffered_pdus(buffered_pdcp_pdu_list::buffer_size) { clear(); }

  void clear()
  {
    count = 0;
    for (pdcp_pdu_info& b : buffered_pdus) {
      b.clear();
    }
  }

  void add_pdcp_sdu(uint32_t sn)
  {
    srsran_expect(sn <= max_pdcp_sn or sn == status_report_sn, "Invalid PDCP SN=%d", sn);
    srsran_assert(not has_pdcp_sn(sn), "Cannot re-add same PDCP SN twice");
    pdcp_pdu_info& pdu = get_pdu_(sn);
    if (pdu.valid()) {
      pdu.clear();
      count--;
    }
    pdu.sn = sn;
    count++;
  }
  void clear_pdcp_sdu(uint32_t sn)
  {
    pdcp_pdu_info& pdu = get_pdu_(sn);
    if (not pdu.valid()) {
      return;
    }
    pdu.clear();
    count--;
  }

  pdcp_pdu_info& operator[](uint32_t sn)
  {
    srsran_expect(has_pdcp_sn(sn), "Invalid access to non-existent PDCP SN=%d", sn);
    return get_pdu_(sn);
  }
  bool has_pdcp_sn(uint32_t pdcp_sn) const
  {
    srsran_expect(pdcp_sn <= max_pdcp_sn or pdcp_sn == status_report_sn, "Invalid PDCP SN=%d", pdcp_sn);
    return get_pdu_(pdcp_sn).sn == pdcp_sn;
  }
  uint32_t nof_sdus() const { return count; }

private:
  const static size_t   max_pdcp_sn      = 262143u;
  const static size_t   buffer_size      = 4096u;
  const static uint32_t status_report_sn = pdcp_pdu_info::status_report_sn;

  pdcp_pdu_info& get_pdu_(uint32_t sn)
  {
    return (sn == status_report_sn) ? status_report_pdu : buffered_pdus[static_cast<size_t>(sn % buffer_size)];
  }
  const pdcp_pdu_info& get_pdu_(uint32_t sn) const
  {
    return (sn == status_report_sn) ? status_report_pdu : buffered_pdus[static_cast<size_t>(sn % buffer_size)];
  }

  // size equal to buffer_size
  std::vector<pdcp_pdu_info> buffered_pdus;
  pdcp_pdu_info              status_report_pdu;
  uint32_t                   count = 0;
};

} // namespace srsran

#endif // SRSRAN_RLC_AM_DATA_STRUCTS_H
