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

/******************************************************************************
 *  File:         tti_synch.h
 *  Description:  Interface used for PHY-MAC synchronization
 *                (producer-consumer model). The consumer waits while its
 *                counter is lower than the producer counter.
 *                The PHY is the consumer. The MAC is the producer.
 *  Reference:
 *****************************************************************************/

#ifndef SRSLTE_TTI_SYNC_H
#define SRSLTE_TTI_SYNC_H

#include <stdint.h>

namespace srslte {

class tti_sync
{
public:
  tti_sync(uint32_t modulus_)
  {
    modulus   = modulus_;
    increment = 1;
    init_counters(0);
  }
  virtual void     increase()                  = 0;
  virtual void     increase(uint32_t cnt)      = 0;
  virtual void     resync()                    = 0;
  virtual uint32_t wait()                      = 0;
  virtual void     set_producer_cntr(uint32_t) = 0;
  uint32_t         get_producer_cntr() { return producer_cntr; }
  uint32_t         get_consumer_cntr() { return consumer_cntr; }
  void             set_increment(uint32_t increment_) { increment = increment_; }

protected:
  void increase_producer() { producer_cntr = (producer_cntr + increment) % modulus; }
  void increase_producer(uint32_t cnt) { producer_cntr = cnt % modulus; }
  void increase_consumer() { consumer_cntr = (consumer_cntr + increment) % modulus; }
  bool wait_condition() { return producer_cntr == consumer_cntr; }
  void init_counters(uint32_t val)
  {
    consumer_cntr = val;
    producer_cntr = val;
  }
  uint32_t increment;
  uint32_t modulus;
  uint32_t producer_cntr;
  uint32_t consumer_cntr;
};

} // namespace srslte

#endif // SRSLTE_TTI_SYNC_H
