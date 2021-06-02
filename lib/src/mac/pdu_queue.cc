/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/mac/pdu_queue.h"
#include "srsran/common/log_helper.h"
#include "srsran/phy/utils/debug.h"

namespace srsran {

void pdu_queue::init(process_callback* callback_)
{
  callback = callback_;
}

uint8_t* pdu_queue::request(uint32_t len)
{
  if (len > MAX_PDU_LEN) {
    ERROR("Error request buffer of invalid size %d. Max bytes %d", len, MAX_PDU_LEN);
    return NULL;
  }
  // This function must be non-blocking. In case we run out of buffers, it shall handle the error properly
  pdu_t* pdu = pool.allocate("pdu_queue::request", false);
  if (pdu) {
    if ((void*)pdu->ptr != (void*)pdu) {
      ERROR("Fatal error in memory alignment in struct pdu_queue::pdu_t");
      exit(-1);
    }
    return pdu->ptr;
  } else {
    logger.error("Not enough buffers for MAC PDU");
    ERROR("Not enough buffers for MAC PDU");
    return nullptr;
  }
}

void pdu_queue::deallocate(const uint8_t* pdu)
{
  if (!pool.deallocate((pdu_t*)pdu)) {
    logger.warning("Error deallocating from buffer pool in deallocate(): buffer not created in this pool.");
  }
}

/* Demultiplexing of logical channels and dissassemble of MAC CE
 * This function enqueues the packet and returns quicly because ACK
 * deadline is important here.
 */
void pdu_queue::push(const uint8_t* ptr, uint32_t len, channel_t channel, int grant_nof_prbs)
{
  if (ptr) {
    pdu_t* pdu          = (pdu_t*)ptr;
    pdu->len            = len;
    pdu->channel        = channel;
    pdu->grant_nof_prbs = grant_nof_prbs;
    if (!pdu_q.try_push(pdu)) {
      logger.warning("Error pushing pdu: queue is full");
    }
  } else {
    logger.warning("Error pushing pdu: ptr is empty");
  }
}

bool pdu_queue::process_pdus()
{
  bool     have_data = false;
  uint32_t cnt       = 0;
  pdu_t*   pdu;
  while (pdu_q.try_pop(pdu)) {
    if (callback) {
      callback->process_pdu(pdu->ptr, pdu->len, pdu->channel, pdu->grant_nof_prbs);
    }
    cnt++;
    have_data = true;
  }
  if (cnt > 20) {
    logger.warning("PDU queue dispatched %d packets", cnt);
    printf("Warning PDU queue dispatched %d packets\n", cnt);
  }
  return have_data;
}

void pdu_queue::reset()
{
  pdu_t* pdu;
  while (pdu_q.try_pop(pdu)) {
    // nop
  }
}

} // namespace srsran
