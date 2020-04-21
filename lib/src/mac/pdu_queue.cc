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

#include "srslte/mac/pdu_queue.h"
#include "srslte/common/log_helper.h"

namespace srslte {

void pdu_queue::init(process_callback* callback_, log_ref log_h_)
{
  callback = callback_;
  log_h    = log_h_;
}

uint8_t* pdu_queue::request(uint32_t len)
{
  if (len > MAX_PDU_LEN) {
    ERROR("Error request buffer of invalid size %d. Max bytes %d\n", len, MAX_PDU_LEN);
    return NULL;
  }
  pdu_t* pdu = pool.allocate("pdu_queue::request", true);
  if (!pdu) {
    if (log_h) {
      log_h->error("Not enough buffers for MAC PDU\n");
    }
    ERROR("Not enough buffers for MAC PDU\n");
  }
  if ((void*)pdu->ptr != (void*)pdu) {
    ERROR("Fatal error in memory alignment in struct pdu_queue::pdu_t\n");
    exit(-1);
  }

  return pdu->ptr;
}

void pdu_queue::deallocate(uint8_t* pdu)
{
  if (!pool.deallocate((pdu_t*)pdu)) {
    log_h->warning("Error deallocating from buffer pool in deallocate(): buffer not created in this pool.\n");
  }
}

/* Demultiplexing of logical channels and dissassemble of MAC CE
 * This function enqueues the packet and returns quicly because ACK
 * deadline is important here.
 */
void pdu_queue::push(uint8_t* ptr, uint32_t len, channel_t channel)
{
  if (ptr) {
    pdu_t* pdu   = (pdu_t*)ptr;
    pdu->len     = len;
    pdu->channel = channel;
    pdu_q.push(pdu);
  } else {
    log_h->warning("Error pushing pdu: ptr is empty\n");
  }
}

bool pdu_queue::process_pdus()
{
  bool     have_data = false;
  uint32_t cnt       = 0;
  pdu_t*   pdu;
  while (pdu_q.try_pop(&pdu)) {
    if (callback) {
      callback->process_pdu(pdu->ptr, pdu->len, pdu->channel);
    }
    cnt++;
    have_data = true;
  }
  if (cnt > 20) {
    if (log_h) {
      log_h->warning("PDU queue dispatched %d packets\n", cnt);
    }
    printf("Warning PDU queue dispatched %d packets\n", cnt);
  }
  return have_data;
}

} // namespace srslte
