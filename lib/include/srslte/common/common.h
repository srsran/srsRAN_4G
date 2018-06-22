/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
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

#include <stdint.h>
#include <string.h>

/*******************************************************************************
                              DEFINES
*******************************************************************************/

#define SRSLTE_UE_CATEGORY     4

#define SRSLTE_N_SRB           3
#define SRSLTE_N_DRB           8
#define SRSLTE_N_RADIO_BEARERS 11

#define SRSLTE_N_MCH_LCIDS     32

#define HARQ_DELAY_MS   4
#define MSG3_DELAY_MS   2 // Delay added to HARQ_DELAY_MS
#define TTI_RX(tti)     (tti>HARQ_DELAY_MS?((tti-HARQ_DELAY_MS)%10240):(10240+tti-HARQ_DELAY_MS))
#define TTI_TX(tti)     ((tti+HARQ_DELAY_MS)%10240)
#define TTI_RX_ACK(tti) ((tti+(2*HARQ_DELAY_MS))%10240)

#define UL_PIDOF(tti)   (tti%(2*HARQ_DELAY_MS))

#define TTIMOD_SZ       (((2*HARQ_DELAY_MS) < 10)?10:20)
#define TTIMOD(tti)     (tti%TTIMOD_SZ)

#define ASYNC_DL_SCHED  (HARQ_DELAY_MS <= 4)

// Cat 3 UE - Max number of DL-SCH transport block bits received within a TTI
// 3GPP 36.306 Table 4.1.1
#define SRSLTE_MAX_BUFFER_SIZE_BITS  102048
#define SRSLTE_MAX_BUFFER_SIZE_BYTES 12756
#define SRSLTE_BUFFER_HEADER_OFFSET  1020

#define SRSLTE_BUFFER_POOL_LOG_ENABLED

#ifdef SRSLTE_BUFFER_POOL_LOG_ENABLED
#define pool_allocate (pool->allocate(__PRETTY_FUNCTION__))
#define SRSLTE_BUFFER_POOL_LOG_NAME_LEN 128
#else
#define pool_allocate (pool->allocate())
#endif

#define ZERO_OBJECT(x) memset(&(x), 0x0, sizeof((x)))

#include "srslte/srslte.h"

/*******************************************************************************
                              TYPEDEFS
*******************************************************************************/

namespace srslte {

typedef enum{
  ERROR_NONE = 0,
  ERROR_INVALID_PARAMS,
  ERROR_INVALID_COMMAND,
  ERROR_OUT_OF_BOUNDS,
  ERROR_CANT_START,
  ERROR_ALREADY_STARTED,
  ERROR_N_ITEMS,
}error_t;
static const char error_text[ERROR_N_ITEMS][20] = { "None",
                                                    "Invalid parameters",
                                                    "Invalid command",
                                                    "Out of bounds",
                                                    "Can't start",
                                                    "Already started"};

//#define ENABLE_TIMESTAMP

/******************************************************************************
 * Byte and Bit buffers
 *
 * Generic buffers with headroom to accommodate packet headers and custom
 * copy constructors & assignment operators for quick copying. Byte buffer
 * holds a next pointer to support linked lists.
 *****************************************************************************/
class byte_buffer_t{
public:
    uint32_t    N_bytes;
    uint8_t     buffer[SRSLTE_MAX_BUFFER_SIZE_BYTES];
    uint8_t    *msg;
#ifdef SRSLTE_BUFFER_POOL_LOG_ENABLED
    char        debug_name[SRSLTE_BUFFER_POOL_LOG_NAME_LEN];
#endif

    byte_buffer_t():N_bytes(0)
    {
      bzero(buffer, SRSLTE_MAX_BUFFER_SIZE_BYTES);
      timestamp_is_set = false;
      msg  = &buffer[SRSLTE_BUFFER_HEADER_OFFSET];
      next = NULL; 
#ifdef SRSLTE_BUFFER_POOL_LOG_ENABLED
      bzero(debug_name, SRSLTE_BUFFER_POOL_LOG_NAME_LEN);
#endif
    }
    byte_buffer_t(const byte_buffer_t& buf)
    {
      bzero(buffer, SRSLTE_MAX_BUFFER_SIZE_BYTES);
      N_bytes = buf.N_bytes;
      memcpy(msg, buf.msg, N_bytes);
    }
    byte_buffer_t & operator= (const byte_buffer_t & buf)
    {
      // avoid self assignment
      if (&buf == this)
        return *this;
      bzero(buffer, SRSLTE_MAX_BUFFER_SIZE_BYTES);
      N_bytes = buf.N_bytes;
      memcpy(msg, buf.msg, N_bytes);
      return *this;
    }
    void reset()
    {
      msg       = &buffer[SRSLTE_BUFFER_HEADER_OFFSET];
      N_bytes   = 0;
      timestamp_is_set = false; 
    }
    uint32_t get_headroom()
    {
      return msg-buffer;
    }
    // Returns the remaining space from what is reported to be the length of msg
    uint32_t get_tailroom()
    {
      return (sizeof(buffer) - (msg-buffer) - N_bytes);
    }
    long get_latency_us()
    {
#ifdef ENABLE_TIMESTAMP
      if(!timestamp_is_set)
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

private:

    struct timeval timestamp[3];
    bool           timestamp_is_set; 
    byte_buffer_t *next;
};

struct bit_buffer_t{
    uint32_t    N_bits;
    uint8_t     buffer[SRSLTE_MAX_BUFFER_SIZE_BITS];
    uint8_t    *msg;
#ifdef SRSLTE_BUFFER_POOL_LOG_ENABLED
    char        debug_name[128];
#endif

    bit_buffer_t():N_bits(0)
    {
      msg = &buffer[SRSLTE_BUFFER_HEADER_OFFSET];
    }
    bit_buffer_t(const bit_buffer_t& buf){
      N_bits = buf.N_bits;
      memcpy(msg, buf.msg, N_bits);
    }
    bit_buffer_t & operator= (const bit_buffer_t & buf){
      // avoid self assignment
      if (&buf == this)
        return *this;
      N_bits = buf.N_bits;
      memcpy(msg, buf.msg, N_bits);
      return *this;
    }
    void reset()
    {
      msg       = &buffer[SRSLTE_BUFFER_HEADER_OFFSET];
      N_bits    = 0;
      timestamp_is_set = false; 
    }
    uint32_t get_headroom()
    {
      return msg-buffer;
    }
    long get_latency_us()
    {
#ifdef ENABLE_TIMESTAMP
      if(!timestamp_is_set)
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
    struct timeval timestamp[3];
    bool           timestamp_is_set; 

};

} // namespace srslte

#endif // SRSLTE_COMMON_H
