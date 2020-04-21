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

#ifndef SRSLTE_PDU_QUEUE_H
#define SRSLTE_PDU_QUEUE_H

#include "srslte/common/block_queue.h"
#include "srslte/common/buffer_pool.h"
#include "srslte/common/log.h"
#include "srslte/common/timers.h"
#include "srslte/mac/pdu.h"

/* Logical Channel Demultiplexing and MAC CE dissassemble */

namespace srslte {

class pdu_queue
{
public:
  typedef enum { DCH, BCH, MCH } channel_t;
  class process_callback
  {
  public:
    virtual void process_pdu(uint8_t* buff, uint32_t len, channel_t channel) = 0;
  };

  pdu_queue(uint32_t pool_size = DEFAULT_POOL_SIZE) : pool(pool_size), callback(NULL) {}
  void init(process_callback* callback, log_ref log_h_);

  uint8_t* request(uint32_t len);
  void     deallocate(uint8_t* pdu);
  void     push(uint8_t* ptr, uint32_t len, channel_t channel = DCH);

  bool process_pdus();

private:
  const static int DEFAULT_POOL_SIZE = 64;             // Number of PDU buffers in total
  const static int MAX_PDU_LEN       = 150 * 1024 / 8; // ~ 150 Mbps

  typedef struct {
    uint8_t   ptr[MAX_PDU_LEN];
    uint32_t  len;
    channel_t channel;
#ifdef SRSLTE_BUFFER_POOL_LOG_ENABLED
    char debug_name[128];
#endif

  } pdu_t;

  block_queue<pdu_t*> pdu_q;
  buffer_pool<pdu_t>  pool;

  process_callback* callback;
  log_ref           log_h;
};

} // namespace srslte

#endif // SRSLTE_PDU_QUEUE_H
