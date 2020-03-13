/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSLTE_COMMON_H
#define SRSLTE_COMMON_H

/*******************************************************************************
                              INCLUDES
*******************************************************************************/

#include <memory>
#include <stdint.h>
#include <string.h>

/*******************************************************************************
                              DEFINES
*******************************************************************************/

#define SRSLTE_UE_CATEGORY 4

#define SRSLTE_N_SRB 3
#define SRSLTE_N_DRB 8
#define SRSLTE_N_RADIO_BEARERS 11

#define SRSLTE_N_MCH_LCIDS 32

#define FDD_HARQ_DELAY_DL_MS 4
#define FDD_HARQ_DELAY_UL_MS 4
#define MSG3_DELAY_MS 2 // Delay added to FDD_HARQ_DELAY_DL_MS

#define TTI_SUB(a, b) ((((a) + 10240) - (b)) % 10240)
#define TTI_ADD(a, b) (((a) + (b)) % 10240)

#define TTI_TX(tti) TTI_ADD(tti, FDD_HARQ_DELAY_DL_MS)

#define TTI_RX(tti) (TTI_SUB(tti, FDD_HARQ_DELAY_UL_MS))
#define TTI_RX_ACK(tti) (TTI_ADD(tti, FDD_HARQ_DELAY_UL_MS + FDD_HARQ_DELAY_DL_MS))

#define TTIMOD_SZ 20
#define TTIMOD(tti) (tti % TTIMOD_SZ)

#define PHICH_MAX_SF 6 // Maximum PHICH in a subframe (1 in FDD, > 1 in TDD, see table 9.1.2-1 36.213)

#define ASYNC_DL_SCHED (FDD_HARQ_DELAY_UL_MS <= 4)

// Cat 4 UE - Max number of DL-SCH transport block bits received within a TTI
// 3GPP 36.306 v15.4.0 Table 4.1.1 for Category 11 with 2 layers and 256QAM
#define SRSLTE_MAX_TBSIZE_BITS 97896
#define SRSLTE_BUFFER_HEADER_OFFSET 1020
#define SRSLTE_MAX_BUFFER_SIZE_BITS (SRSLTE_MAX_TBSIZE_BITS + SRSLTE_BUFFER_HEADER_OFFSET)
#define SRSLTE_MAX_BUFFER_SIZE_BYTES (SRSLTE_MAX_TBSIZE_BITS / 8 + SRSLTE_BUFFER_HEADER_OFFSET)

//#define SRSLTE_BUFFER_POOL_LOG_ENABLED

#ifdef SRSLTE_BUFFER_POOL_LOG_ENABLED
#define pool_allocate (srslte::allocate_unique_buffer(*pool, __PRETTY_FUNCTION__))
#define pool_allocate_blocking (srslte::allocate_unique_buffer(*pool, __PRETTY_FUNCTION__, true))
#define SRSLTE_BUFFER_POOL_LOG_NAME_LEN 128
#else
#define pool_allocate (srslte::allocate_unique_buffer(*pool))
#define pool_allocate_blocking (srslte::allocate_unique_buffer(*pool, true))
#endif

#include "srslte/srslte.h"

/*******************************************************************************
                              TYPEDEFS
*******************************************************************************/

namespace srslte {

//#define ENABLE_TIMESTAMP

/******************************************************************************
 * Byte and Bit buffers
 *
 * Generic buffers with headroom to accommodate packet headers and custom
 * copy constructors & assignment operators for quick copying. Byte buffer
 * holds a next pointer to support linked lists.
 *****************************************************************************/
class byte_buffer_t
{
public:
  uint32_t N_bytes;
  uint8_t  buffer[SRSLTE_MAX_BUFFER_SIZE_BYTES];
  uint8_t* msg;
#ifdef SRSLTE_BUFFER_POOL_LOG_ENABLED
  char debug_name[SRSLTE_BUFFER_POOL_LOG_NAME_LEN];
#endif

  byte_buffer_t() : N_bytes(0)
  {
    bzero(buffer, SRSLTE_MAX_BUFFER_SIZE_BYTES);
#ifdef ENABLE_TIMESTAMP
    timestamp_is_set = false;
#endif
    msg  = &buffer[SRSLTE_BUFFER_HEADER_OFFSET];
    next = NULL;
#ifdef SRSLTE_BUFFER_POOL_LOG_ENABLED
    bzero(debug_name, SRSLTE_BUFFER_POOL_LOG_NAME_LEN);
#endif
  }
  byte_buffer_t(const byte_buffer_t& buf)
  {
    bzero(buffer, SRSLTE_MAX_BUFFER_SIZE_BYTES);
    msg  = &buffer[SRSLTE_BUFFER_HEADER_OFFSET];
    next = NULL;
    // copy actual contents
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
    next    = NULL;
    N_bytes = buf.N_bytes;
    memcpy(msg, buf.msg, N_bytes);
    return *this;
  }
  void clear()
  {
    msg     = &buffer[SRSLTE_BUFFER_HEADER_OFFSET];
    N_bytes = 0;
#ifdef ENABLE_TIMESTAMP
    timestamp_is_set = false;
#endif
  }
  uint32_t get_headroom() { return msg - buffer; }
  // Returns the remaining space from what is reported to be the length of msg
  uint32_t get_tailroom() { return (sizeof(buffer) - (msg - buffer) - N_bytes); }
  long     get_latency_us()
  {
#ifdef ENABLE_TIMESTAMP
    if (!timestamp_is_set)
      return 0;
    gettimeofday(&timestamp[2], NULL);
    get_time_interval(timestamp);
    return timestamp[0].tv_usec;
#else
    return 0;
#endif
  }

  void set_timestamp()
  {
#ifdef ENABLE_TIMESTAMP
    gettimeofday(&timestamp[1], NULL);
    timestamp_is_set = true;
#endif
  }

  void append_bytes(uint8_t* buf, uint32_t size)
  {
    memcpy(&msg[N_bytes], buf, size);
    N_bytes += size;
  }

private:
#ifdef ENABLE_TIMESTAMP
  struct timeval timestamp[3];
  bool           timestamp_is_set;
#endif
  byte_buffer_t* next;
};

struct bit_buffer_t {
  uint32_t N_bits;
  uint8_t  buffer[SRSLTE_MAX_BUFFER_SIZE_BITS];
  uint8_t* msg;
#ifdef SRSLTE_BUFFER_POOL_LOG_ENABLED
  char debug_name[128];
#endif

  bit_buffer_t() : N_bits(0)
  {
    msg = &buffer[SRSLTE_BUFFER_HEADER_OFFSET];
#ifdef ENABLE_TIMESTAMP
    timestamp_is_set = false;
#endif
  }
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
#ifdef ENABLE_TIMESTAMP
    timestamp_is_set = false;
#endif
  }
  uint32_t get_headroom() { return msg - buffer; }
  long     get_latency_us()
  {
#ifdef ENABLE_TIMESTAMP
    if (!timestamp_is_set)
      return 0;
    gettimeofday(&timestamp[2], NULL);
    return timestamp[0].tv_usec;
#else
    return 0;
#endif
  }
  void set_timestamp()
  {
#ifdef ENABLE_TIMESTAMP
    gettimeofday(&timestamp[1], NULL);
    timestamp_is_set = true;
#endif
  }

private:
#ifdef ENABLE_TIMESTAMP
  struct timeval timestamp[3];
  bool           timestamp_is_set;
#endif
};

// Create a Managed Life-Time Byte Buffer
class byte_buffer_pool;
class byte_buffer_deleter
{
public:
  explicit byte_buffer_deleter(byte_buffer_pool* pool_ = nullptr) : pool(pool_) {}
  void              operator()(byte_buffer_t* buf) const;
  byte_buffer_pool* pool;
};

typedef std::unique_ptr<byte_buffer_t, byte_buffer_deleter> unique_byte_buffer_t;

} // namespace srslte

#endif // SRSLTE_COMMON_H
