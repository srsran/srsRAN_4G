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

#ifndef SRSRAN_RLC_AM_DATA_STRUCTS_H
#define SRSRAN_RLC_AM_DATA_STRUCTS_H

#include "srsran/adt/circular_buffer.h"
#include "srsran/adt/circular_map.h"
#include "srsran/adt/intrusive_list.h"
#include "srsran/common/buffer_pool.h"
#include <array>
#include <list>
#include <vector>

namespace srsran {

template <typename HeaderType>
class rlc_amd_tx_pdu;
template <typename HeaderType>
class pdcp_pdu_info;

/// Pool that manages the allocation of RLC AM PDU Segments to RLC PDUs and tracking of segments ACK state
template <typename HeaderType>
struct rlc_am_pdu_segment_pool {
  const static size_t MAX_POOL_SIZE = 16384;

  /// RLC AM PDU Segment, containing the PDCP SN and RLC SN it has been assigned to, and its current ACK state
  using rlc_list_tag = default_intrusive_tag;
  struct free_list_tag {};
  struct segment_resource : public intrusive_forward_list_element<rlc_list_tag>,
                            public intrusive_forward_list_element<free_list_tag>,
                            public intrusive_double_linked_list_element<> {
    const static uint32_t invalid_rlc_sn  = std::numeric_limits<uint32_t>::max();
    const static uint32_t invalid_pdcp_sn = std::numeric_limits<uint32_t>::max() - 1; // -1 for Status Report

    int id() const { return std::distance(parent_pool->segments.cbegin(), this); }

    void release_pdcp_sn()
    {
      pdcp_sn_ = invalid_pdcp_sn;
      if (empty()) {
        parent_pool->free_list.push_front(this);
      }
    }

    void release_rlc_sn()
    {
      rlc_sn_ = invalid_rlc_sn;
      if (empty()) {
        parent_pool->free_list.push_front(this);
      }
    }

    uint32_t rlc_sn() const { return rlc_sn_; }
    uint32_t pdcp_sn() const { return pdcp_sn_; }
    bool     empty() const { return rlc_sn_ == invalid_rlc_sn and pdcp_sn_ == invalid_pdcp_sn; }

  private:
    friend struct rlc_am_pdu_segment_pool<HeaderType>;
    uint32_t                             rlc_sn_     = invalid_rlc_sn;
    uint32_t                             pdcp_sn_    = invalid_pdcp_sn;
    rlc_am_pdu_segment_pool<HeaderType>* parent_pool = nullptr;
  };

  rlc_am_pdu_segment_pool()
  {
    for (segment_resource& s : segments) {
      s.parent_pool = this;
      free_list.push_front(&s);
    }
  }
  rlc_am_pdu_segment_pool(const rlc_am_pdu_segment_pool&) = delete;
  rlc_am_pdu_segment_pool(rlc_am_pdu_segment_pool&&)      = delete;
  rlc_am_pdu_segment_pool& operator=(const rlc_am_pdu_segment_pool&) = delete;
  rlc_am_pdu_segment_pool& operator=(rlc_am_pdu_segment_pool&&) = delete;

  bool has_segments() const { return not free_list.empty(); }
  bool make_segment(rlc_amd_tx_pdu<HeaderType>& rlc_list, pdcp_pdu_info<HeaderType>& pdcp_list)
  {
    if (not has_segments()) {
      return false;
    }
    segment_resource* segment = free_list.pop_front();
    segment->rlc_sn_          = rlc_list.rlc_sn;
    segment->pdcp_sn_         = pdcp_list.sn;
    rlc_list.add_segment(*segment);
    pdcp_list.add_segment(*segment);
    return true;
  }

private:
  intrusive_forward_list<rlc_am_pdu_segment_pool<HeaderType>::segment_resource, free_list_tag> free_list;
  std::array<rlc_am_pdu_segment_pool<HeaderType>::segment_resource, MAX_POOL_SIZE>             segments;
};

/// Class that contains the parameters and state (e.g. segments) of a RLC PDU
template <typename HeaderType>
class rlc_amd_tx_pdu
{
  using rlc_am_pdu_segment             = typename rlc_am_pdu_segment_pool<HeaderType>::segment_resource;
  using list_type                      = intrusive_forward_list<rlc_am_pdu_segment>;
  const static uint32_t invalid_rlc_sn = std::numeric_limits<uint32_t>::max();

  list_type list;

public:
  using iterator       = typename list_type::iterator;
  using const_iterator = typename list_type::const_iterator;

  const uint32_t       rlc_sn     = invalid_rlc_sn;
  uint32_t             retx_count = 0;
  HeaderType           header     = {};
  unique_byte_buffer_t buf        = nullptr;

  explicit rlc_amd_tx_pdu(uint32_t rlc_sn_) : rlc_sn(rlc_sn_) {}
  rlc_amd_tx_pdu(const rlc_amd_tx_pdu&)           = delete;
  rlc_amd_tx_pdu(rlc_amd_tx_pdu&& other) noexcept = default;
  rlc_amd_tx_pdu& operator=(const rlc_amd_tx_pdu& other) = delete;
  rlc_amd_tx_pdu& operator=(rlc_amd_tx_pdu&& other) = delete;
  ~rlc_amd_tx_pdu()
  {
    while (not list.empty()) {
      // remove from list
      rlc_am_pdu_segment* segment = list.pop_front();
      // deallocate if also removed from PDCP
      segment->release_rlc_sn();
    }
  }

  // Segment List Interface
  void           add_segment(rlc_am_pdu_segment& segment) { list.push_front(&segment); }
  const_iterator begin() const { return list.begin(); }
  const_iterator end() const { return list.end(); }
  iterator       begin() { return list.begin(); }
  iterator       end() { return list.end(); }
};

/// Class that contains the parameters and state (e.g. unACKed segments) of a PDCP PDU
template <typename HeaderType>
class pdcp_pdu_info
{
  using rlc_am_pdu_segment = typename rlc_am_pdu_segment_pool<HeaderType>::segment_resource;
  using list_type          = intrusive_double_linked_list<rlc_am_pdu_segment>;

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
  void ack_segment(rlc_am_pdu_segment& segment)
  {
    // remove from list
    list.pop(&segment);
    // signal pool that the pdcp handle is released
    segment.release_pdcp_sn();
  }
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

template <class T>
struct rlc_ringbuffer_base {
  virtual ~rlc_ringbuffer_base()           = default;
  virtual T&     add_pdu(size_t sn)        = 0;
  virtual void   remove_pdu(size_t sn)     = 0;
  virtual T&     operator[](size_t sn)     = 0;
  virtual size_t size() const              = 0;
  virtual bool   empty() const             = 0;
  virtual bool   full() const              = 0;
  virtual void   clear()                   = 0;
  virtual bool   has_sn(uint32_t sn) const = 0;
};

template <class T, std::size_t WINDOW_SIZE>
struct rlc_ringbuffer_t : public rlc_ringbuffer_base<T> {
  ~rlc_ringbuffer_t() = default;

  T& add_pdu(size_t sn) override
  {
    srsran_expect(not has_sn(sn), "The same SN=%zd should not be added twice", sn);
    window.overwrite(sn, T(sn));
    return window[sn];
  }
  void remove_pdu(size_t sn) override
  {
    srsran_expect(has_sn(sn), "The removed SN=%zd is not in the window", sn);
    window.erase(sn);
  }
  T&     operator[](size_t sn) override { return window[sn]; }
  size_t size() const override { return window.size(); }
  bool   full() const override { return window.full(); }
  bool   empty() const override { return window.empty(); }
  void   clear() override { window.clear(); }

  bool has_sn(uint32_t sn) const override { return window.contains(sn); }

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

template <typename HeaderType>
struct buffered_pdcp_pdu_list {
public:
  explicit buffered_pdcp_pdu_list() : buffered_pdus(buffered_pdcp_pdu_list::buffer_size) { clear(); }

  void clear()
  {
    count = 0;
    for (pdcp_pdu_info<HeaderType>& b : buffered_pdus) {
      b.clear();
    }
  }

  void add_pdcp_sdu(uint32_t sn)
  {
    srsran_expect(sn <= max_pdcp_sn or sn == status_report_sn, "Invalid PDCP SN=%d", sn);
    srsran_assert(not has_pdcp_sn(sn), "Cannot re-add same PDCP SN twice");
    pdcp_pdu_info<HeaderType>& pdu = get_pdu_(sn);
    if (pdu.valid()) {
      pdu.clear();
      count--;
    }
    pdu.sn = sn;
    count++;
  }

  void clear_pdcp_sdu(uint32_t sn)
  {
    pdcp_pdu_info<HeaderType>& pdu = get_pdu_(sn);
    if (not pdu.valid()) {
      return;
    }
    pdu.clear();
    count--;
  }

  pdcp_pdu_info<HeaderType>& operator[](uint32_t sn)
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
  const static uint32_t status_report_sn = pdcp_pdu_info<HeaderType>::status_report_sn;

  pdcp_pdu_info<HeaderType>& get_pdu_(uint32_t sn)
  {
    return (sn == status_report_sn) ? status_report_pdu : buffered_pdus[static_cast<size_t>(sn % buffer_size)];
  }
  const pdcp_pdu_info<HeaderType>& get_pdu_(uint32_t sn) const
  {
    return (sn == status_report_sn) ? status_report_pdu : buffered_pdus[static_cast<size_t>(sn % buffer_size)];
  }

  // size equal to buffer_size
  std::vector<pdcp_pdu_info<HeaderType> > buffered_pdus;
  pdcp_pdu_info<HeaderType>               status_report_pdu;
  uint32_t                                count = 0;
};

struct rlc_amd_retx_base_t {
  const static uint32_t invalid_rlc_sn = std::numeric_limits<uint32_t>::max();

  uint32_t sn;         ///< sequence number
  bool     is_segment; ///< flag whether this is a segment or not
  uint32_t so_start;   ///< offset to first byte of this segment
  // so_end or segment_length are different for LTE and NR, hence are defined in subclasses
  uint32_t current_so; ///< stores progressing SO during segmentation of this object

  rlc_amd_retx_base_t() : sn(invalid_rlc_sn), is_segment(false), so_start(0), current_so(0) {}
  virtual ~rlc_amd_retx_base_t() = default;

  /**
   * @brief overlaps implements a check whether the range of this retransmission object includes
   * the given segment offset
   * @param so the segment offset to check
   * @return true if the segment offset is covered by the retransmission object. Otherwise false
   */
  virtual bool overlaps(uint32_t so) const = 0;
};

struct rlc_amd_retx_lte_t : public rlc_amd_retx_base_t {
  uint32_t so_end; ///< offset to first byte beyond the end of this segment

  rlc_amd_retx_lte_t() : rlc_amd_retx_base_t(), so_end(0) {}
  bool overlaps(uint32_t segment_offset) const override
  {
    return (segment_offset >= so_start) && (segment_offset < so_end);
  }
};

struct rlc_amd_retx_nr_t : public rlc_amd_retx_base_t {
  uint32_t segment_length; ///< number of bytes contained in this segment

  rlc_amd_retx_nr_t() : rlc_amd_retx_base_t(), segment_length(0) {}
  bool overlaps(uint32_t segment_offset) const override
  {
    return (segment_offset >= so_start) && (segment_offset < current_so + segment_length);
  }
};

template <class T>
class pdu_retx_queue_base
{
public:
  virtual ~pdu_retx_queue_base()           = default;
  virtual T&     push()                    = 0;
  virtual void   pop()                     = 0;
  virtual T&     front()                   = 0;
  virtual void   clear()                   = 0;
  virtual size_t size() const              = 0;
  virtual bool   empty() const             = 0;
  virtual bool   full() const              = 0;

  virtual T&       operator[](size_t idx)       = 0;
  virtual const T& operator[](size_t idx) const = 0;

  virtual bool has_sn(uint32_t sn) const              = 0;
  virtual bool has_sn(uint32_t sn, uint32_t so) const = 0;
};

template <class T, std::size_t WINDOW_SIZE>
class pdu_retx_queue : public pdu_retx_queue_base<T>
{
public:
  ~pdu_retx_queue() = default;

  T& push() override
  {
    assert(not full());
    T& p = buffer[wpos];
    wpos = (wpos + 1) % WINDOW_SIZE;
    return p;
  }

  void pop() override { rpos = (rpos + 1) % WINDOW_SIZE; }

  T& front() override
  {
    assert(not empty());
    return buffer[rpos];
  }

  T& operator[](size_t idx) override
  {
    srsran_assert(idx < size(), "Out-of-bounds access to element idx=%zd", idx);
    return buffer[(rpos + idx) % WINDOW_SIZE];
  }

  const T& operator[](size_t idx) const override
  {
    srsran_assert(idx < size(), "Out-of-bounds access to element idx=%zd", idx);
    return buffer[(rpos + idx) % WINDOW_SIZE];
  }

  void clear() override
  {
    wpos = 0;
    rpos = 0;
  }

  bool has_sn(uint32_t sn) const override
  {
    for (size_t i = rpos; i != wpos; i = (i + 1) % WINDOW_SIZE) {
      if (buffer[i].sn == sn) {
        return true;
      }
    }
    return false;
  }

  bool has_sn(uint32_t sn, uint32_t so) const override
  {
    for (size_t i = rpos; i != wpos; i = (i + 1) % WINDOW_SIZE) {
      if (buffer[i].sn == sn) {
        if (buffer[i].overlaps(so)) {
          return true;
        }
      }
    }
    return false;
  }

  size_t size() const override { return (wpos >= rpos) ? wpos - rpos : WINDOW_SIZE + wpos - rpos; }
  bool   empty() const override { return wpos == rpos; }
  bool   full() const override { return size() == WINDOW_SIZE - 1; }

private:
  std::array<T, WINDOW_SIZE> buffer;
  size_t                     wpos = 0;
  size_t                     rpos = 0;
};

template <class T>
class pdu_retx_queue_list
{
  std::list<T> queue;

public:
  ~pdu_retx_queue_list() = default;
  T& push()
  {
    queue.emplace_back();
    return queue.back();
  }

  void pop()
  {
    if (not queue.empty()) {
      queue.pop_front();
    }
  }

  T& front()
  {
    assert(not queue.empty());
    return queue.front();
  }

  const std::list<T>& get_inner_queue() const { return queue; }

  void   clear() { queue.clear(); }
  size_t size() const { return queue.size(); }
  bool   empty() const { return queue.empty(); }

  bool has_sn(uint32_t sn) const
  {
    if (queue.empty()) {
      return false;
    }
    for (auto elem : queue) {
      if (elem.sn == sn) {
        return true;
      }
    }
    return false;
  };

  bool has_sn(uint32_t sn, uint32_t so) const
  {
    if (queue.empty()) {
      return false;
    }
    for (auto elem : queue) {
      if (elem.sn == sn) {
        if (elem.overlaps(so)) {
          return true;
        }
      }
    }
    return false;
  };

  /**
   * @brief remove_sn removes SN from queue and returns after first match
   * @param sn sequence number to be removed from queue
   * @return true if one element was removed, false if no element to remove was found
   */
  bool remove_sn(uint32_t sn)
  {
    if (queue.empty()) {
      return false;
    }
    auto iter = queue.begin();
    while (iter != queue.end()) {
      if (iter->sn == sn) {
        iter = queue.erase(iter);
        return true;
      } else {
        ++iter;
      }
    }
    return false;
  }
};

} // namespace srsran

#endif // SRSRAN_RLC_AM_DATA_STRUCTS_H
