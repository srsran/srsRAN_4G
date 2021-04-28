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

#ifndef SRSRAN_BYTE_BUFFER_H
#define SRSRAN_BYTE_BUFFER_H

#include "common.h"
#include "srsran/adt/span.h"
#include <chrono>
#include <cstdint>

//#define SRSRAN_BUFFER_POOL_LOG_ENABLED

namespace srsran {

#define ENABLE_TIMESTAMP

struct buffer_latency_calc {
  void clear()
  {
#ifdef ENABLE_TIMESTAMP
    timestamp_is_set = false;
#endif
  }

  std::chrono::microseconds get_latency_us() const
  {
#ifdef ENABLE_TIMESTAMP
    if (!timestamp_is_set) {
      return std::chrono::microseconds{0};
    }
    return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - tp);
#else
    return std::chrono::microseconds{0};
#endif
  }

  std::chrono::high_resolution_clock::time_point get_timestamp() const { return tp; }

  void set_timestamp()
  {
#ifdef ENABLE_TIMESTAMP
    tp               = std::chrono::high_resolution_clock::now();
    timestamp_is_set = true;
#endif
  }

  void set_timestamp(std::chrono::high_resolution_clock::time_point tp_)
  {
#ifdef ENABLE_TIMESTAMP
    tp               = tp_;
    timestamp_is_set = true;
#endif
  }

private:
#ifdef ENABLE_TIMESTAMP
  std::chrono::high_resolution_clock::time_point tp;
  bool                                           timestamp_is_set = false;
#endif
};

/******************************************************************************
 * Byte buffer
 *
 * Generic byte buffer with headroom to accommodate packet headers and custom
 * copy constructors & assignment operators for quick copying. Byte buffer
 * holds a next pointer to support linked lists.
 *****************************************************************************/
class byte_buffer_t
{
public:
  using iterator       = uint8_t*;
  using const_iterator = const uint8_t*;

  uint32_t N_bytes = 0;
  uint8_t  buffer[SRSRAN_MAX_BUFFER_SIZE_BYTES];
  uint8_t* msg = nullptr;
#ifdef SRSRAN_BUFFER_POOL_LOG_ENABLED
  char debug_name[SRSRAN_BUFFER_POOL_LOG_NAME_LEN];
#endif

  struct buffer_metadata_t {
    uint32_t            pdcp_sn = 0;
    buffer_latency_calc tp;
  } md;

  byte_buffer_t() : msg(&buffer[SRSRAN_BUFFER_HEADER_OFFSET])
  {
#ifdef SRSRAN_BUFFER_POOL_LOG_ENABLED
    bzero(debug_name, SRSRAN_BUFFER_POOL_LOG_NAME_LEN);
#endif
  }
  explicit byte_buffer_t(uint32_t size) : msg(&buffer[SRSRAN_BUFFER_HEADER_OFFSET]), N_bytes(size)
  {
#ifdef SRSRAN_BUFFER_POOL_LOG_ENABLED
    bzero(debug_name, SRSRAN_BUFFER_POOL_LOG_NAME_LEN);
#endif
  }
  byte_buffer_t(uint32_t size, uint8_t val) : byte_buffer_t(size) { std::fill(msg, msg + N_bytes, val); }
  byte_buffer_t(const byte_buffer_t& buf) : msg(&buffer[SRSRAN_BUFFER_HEADER_OFFSET]), md(buf.md), N_bytes(buf.N_bytes)
  {
    // copy actual contents
    memcpy(msg, buf.msg, N_bytes);
  }

  byte_buffer_t& operator=(const byte_buffer_t& buf)
  {
    // avoid self assignment
    if (&buf == this)
      return *this;
    msg     = &buffer[SRSRAN_BUFFER_HEADER_OFFSET];
    N_bytes = buf.N_bytes;
    md      = buf.md;
    memcpy(msg, buf.msg, N_bytes);
    return *this;
  }

  void clear()
  {
    msg     = &buffer[SRSRAN_BUFFER_HEADER_OFFSET];
    N_bytes = 0;
    md      = {};
  }
  uint32_t get_headroom() { return msg - buffer; }
  // Returns the remaining space from what is reported to be the length of msg
  uint32_t                  get_tailroom() const { return (sizeof(buffer) - (msg - buffer) - N_bytes); }
  std::chrono::microseconds get_latency_us() const { return md.tp.get_latency_us(); }

  std::chrono::high_resolution_clock::time_point get_timestamp() const { return md.tp.get_timestamp(); }

  void set_timestamp() { md.tp.set_timestamp(); }

  void set_timestamp(std::chrono::high_resolution_clock::time_point tp_) { md.tp.set_timestamp(tp_); }

  void append_bytes(uint8_t* buf, uint32_t size)
  {
    memcpy(&msg[N_bytes], buf, size);
    N_bytes += size;
  }

  // vector-like interface
  void           resize(size_t size) { N_bytes = size; }
  size_t         capacity() const { return get_tailroom(); }
  uint8_t*       data() { return msg; }
  const uint8_t* data() const { return msg; }
  uint32_t       size() const { return N_bytes; }
  iterator       begin() { return msg; }
  const_iterator begin() const { return msg; }
  iterator       end() { return msg + N_bytes; }
  const_iterator end() const { return msg + N_bytes; }

  void* operator new(size_t sz);
  void* operator new(size_t sz, const std::nothrow_t& nothrow_value) noexcept;
  void* operator new[](size_t sz) = delete;
  void  operator delete(void* ptr);
  void  operator delete[](void* ptr) = delete;
};

struct bit_buffer_t {
  uint32_t N_bits = 0;
  uint8_t  buffer[SRSRAN_MAX_BUFFER_SIZE_BITS];
  uint8_t* msg = nullptr;
#ifdef SRSRAN_BUFFER_POOL_LOG_ENABLED
  char debug_name[128];
#endif

  bit_buffer_t() : msg(&buffer[SRSRAN_BUFFER_HEADER_OFFSET]) {}
  bit_buffer_t(const bit_buffer_t& buf) : msg(&buffer[SRSRAN_BUFFER_HEADER_OFFSET]), N_bits(buf.N_bits)
  {
    memcpy(msg, buf.msg, N_bits);
  }
  bit_buffer_t& operator=(const bit_buffer_t& buf)
  {
    // avoid self assignment
    if (&buf == this) {
      return *this;
    }
    msg    = &buffer[SRSRAN_BUFFER_HEADER_OFFSET];
    N_bits = buf.N_bits;
    memcpy(msg, buf.msg, N_bits);
    return *this;
  }
  void clear()
  {
    msg    = &buffer[SRSRAN_BUFFER_HEADER_OFFSET];
    N_bits = 0;
  }
  uint32_t get_headroom() { return msg - buffer; }
};

using unique_byte_buffer_t = std::unique_ptr<byte_buffer_t>;

///
/// Utilities to create a span out of a byte_buffer.
///

using byte_span       = span<uint8_t>;
using const_byte_span = span<const uint8_t>;

inline byte_span make_span(byte_buffer_t& b)
{
  return byte_span{b.msg, b.N_bytes};
}

inline const_byte_span make_span(const byte_buffer_t& b)
{
  return const_byte_span{b.msg, b.N_bytes};
}

inline byte_span make_span(unique_byte_buffer_t& b)
{
  return byte_span{b->msg, b->N_bytes};
}

inline const_byte_span make_span(const unique_byte_buffer_t& b)
{
  return const_byte_span{b->msg, b->N_bytes};
}

} // namespace srsran

#endif // SRSRAN_BYTE_BUFFER_H
