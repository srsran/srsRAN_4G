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


#define Error(fmt, ...)   log_h->error(fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) log_h->warning(fmt, ##__VA_ARGS__)
#define Info(fmt, ...)    log_h->info(fmt, ##__VA_ARGS__)
#define Debug(fmt, ...)   log_h->debug(fmt, ##__VA_ARGS__)

#include "srslte/common/pdu_queue.h"


namespace srslte {
    

void pdu_queue::init(process_callback *callback_, log* log_h_)
{
  callback  = callback_;
  log_h     = log_h_;   
}

uint8_t* pdu_queue::request(uint32_t len)
{  
  if (len > MAX_PDU_LEN) {
    fprintf(stderr, "Error request buffer of invalid size %d. Max bytes %d\n", len, MAX_PDU_LEN);
    return NULL; 
  }
  pdu_t *pdu = pool.allocate();  
  if (!pdu) {
    if (log_h) {
      log_h->error("Not enough buffers for MAC PDU\n");      
    }
    fprintf(stderr, "Not enough buffers for MAC PDU\n");
  }
  if ((void*) pdu->ptr != (void*) pdu) {
    fprintf(stderr, "Fatal error in memory alignment in struct pdu_queue::pdu_t\n");
    exit(-1);
  }
  
  return pdu->ptr; 
}

void pdu_queue::deallocate(uint8_t* pdu)
{
  if (!pool.deallocate((pdu_t*) pdu)) {
    log_h->warning("Error deallocating from buffer pool in deallocate(): buffer not created in this pool.\n");
  }
}

/* Demultiplexing of logical channels and dissassemble of MAC CE 
 * This function enqueues the packet and returns quicly because ACK 
 * deadline is important here. 
 */ 
void pdu_queue::push(uint8_t *ptr, uint32_t len, uint32_t tstamp)
{
  if (ptr) {
    pdu_t *pdu  = (pdu_t*) ptr;
    pdu->len    = len;
    pdu->tstamp = tstamp;
    pdu_q.push(pdu);
  } else {
    log_h->warning("Error pushing pdu: ptr is empty\n");
  }
}

bool pdu_queue::process_pdus()
{
  bool have_data = false; 
  uint32_t cnt  = 0; 
  pdu_t *pdu; 
  while(pdu_q.try_pop(&pdu)) {
    if (callback) {
      callback->process_pdu(pdu->ptr, pdu->len, pdu->tstamp);
    }
    if (!pool.deallocate(pdu)) {
      log_h->warning("Error deallocating from buffer pool in process_pdus(): buffer not created in this pool.\n");
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

}
