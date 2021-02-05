/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSLTE_BYTE_BUFFER_H
#define SRSLTE_BYTE_BUFFER_H

#include "common.h"
#include <chrono>
#include <cstdint>

//#define SRSLTE_BUFFER_POOL_LOG_ENABLED

#ifdef SRSLTE_BUFFER_POOL_LOG_ENABLED
#define pool_allocate (srslte::allocate_unique_buffer(*pool, __PRETTY_FUNCTION__))
#define pool_allocate_blocking (srslte::allocate_unique_buffer(*pool, __PRETTY_FUNCTION__, true))
#define SRSLTE_BUFFER_POOL_LOG_NAME_LEN 128
#else
#define pool_allocate (srslte::allocate_unique_buffer(*pool))
#define pool_allocate_blocking (srslte::allocate_unique_buffer(*pool, true))
#endif

namespace srslte {

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

  uint32_t N_bytes;
  uint8_t  buffer[SRSLTE_MAX_BUFFER_SIZE_BYTES];
  uint8_t* msg;
#ifdef SRSLTE_BUFFER_POOL_LOG_ENABLED
  char debug_name[SRSLTE_BUFFER_POOL_LOG_NAME_LEN];
#endif

  struct buffer_metadata_t {
    uint32_t pdcp_sn = 0;
  } md;

  byte_buffer_t() : N_bytes(0)
  {
    bzero(buffer, SRSLTE_MAX_BUFFER_SIZE_BYTES);
    msg = &buffer[SRSLTE_BUFFER_HEADER_OFFSET];
#ifdef SRSLTE_BUFFER_POOL_LOG_ENABLED
    bzero(debug_name, SRSLTE_BUFFER_POOL_LOG_NAME_LEN);
#endif
  }
  byte_buffer_t(const byte_buffer_t& buf)
  {
    bzero(buffer, SRSLTE_MAX_BUFFER_SIZE_BYTES);
    msg = &buffer[SRSLTE_BUFFER_HEADER_OFFSET];
    // copy actual contents
    md      = buf.md;
    N_bytes = buf.N_bytes;
    memcpy(msg, buf.msg, N_bytes);
  }
  byte_buffer_t& operator=(const byte_buffer_t& buf)
  {
    // avoid self assignment
    if (&buf == this)
      return *this;
    bzero(buffer, SRSLTE_MAX_BUFFER_SIZE_BYTES);
    msg     = &buffer[SRSLTE_BUFFER_HEADER_OFFSET];
    N_bytes = buf.N_bytes;
    md      = buf.md;
    memcpy(msg, buf.msg, N_bytes);
    return *this;
  }
  void clear()
  {
    msg     = &buffer[SRSLTE_BUFFER_HEADER_OFFSET];
    N_bytes = 0;
    md      = {};
    tp.clear();
  }
  uint32_t get_headroom() { return msg - buffer; }
  // Returns the remaining space from what is reported to be the length of msg
  uint32_t                  get_tailroom() { return (sizeof(buffer) - (msg - buffer) - N_bytes); }
  std::chrono::microseconds get_latency_us() const { return tp.get_latency_us(); }

  std::chrono::high_resolution_clock::time_point get_timestamp() const { return tp.get_timestamp(); }

  void set_timestamp() { tp.set_timestamp(); }

  void set_timestamp(std::chrono::high_resolution_clock::time_point tp_) { tp.set_timestamp(tp_); }

  void append_bytes(uint8_t* buf, uint32_t size)
  {
    memcpy(&msg[N_bytes], buf, size);
    N_bytes += size;
  }

  uint8_t*       data() { return msg; }
  const uint8_t* data() const { return msg; }
  uint32_t       size() const { return N_bytes; }
  iterator       begin() { return msg; }
  const iterator begin() const { return msg; }
  iterator       end() { return msg + N_bytes; }
  const_iterator end() const { return msg + N_bytes; }

  void* operator new(size_t sz);
  void* operator new(size_t sz, const std::nothrow_t& nothrow_value) noexcept;
  void* operator new(size_t sz, void* ptr) noexcept { return ptr; }
  void* operator new[](size_t sz) = delete;
  void  operator delete(void* ptr);
  void  operator delete[](void* ptr) = delete;

private:
  buffer_latency_calc tp;
};

struct bit_buffer_t {
  uint32_t N_bits;
  uint8_t  buffer[SRSLTE_MAX_BUFFER_SIZE_BITS];
  uint8_t* msg;
#ifdef SRSLTE_BUFFER_POOL_LOG_ENABLED
  char debug_name[128];
#endif

  bit_buffer_t() : N_bits(0) { msg = &buffer[SRSLTE_BUFFER_HEADER_OFFSET]; }
  bit_buffer_t(const bit_buffer_t& buf)
  {
    msg    = &buffer[SRSLTE_BUFFER_HEADER_OFFSET];
    N_bits = buf.N_bits;
    memcpy(msg, buf.msg, N_bits);
  }
  bit_buffer_t& operator=(const bit_buffer_t& buf)
  {
    // avoid self assignment
    if (&buf == this) {
      return *this;
    }
    msg    = &buffer[SRSLTE_BUFFER_HEADER_OFFSET];
    N_bits = buf.N_bits;
    memcpy(msg, buf.msg, N_bits);
    return *this;
  }
  void clear()
  {
    msg    = &buffer[SRSLTE_BUFFER_HEADER_OFFSET];
    N_bits = 0;
  }
  uint32_t get_headroom() { return msg - buffer; }
};

// Create a Managed Life-Time Byte Buffer
class byte_buffer_pool;

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

} // namespace srslte

#endif // SRSLTE_BYTE_BUFFER_H
