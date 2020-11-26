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

/*! \brief Base class for implementing socket handlers using
 * the netsource object.
 *
 */

#ifndef SRSLTE_NETSOURE_HANDLER_H
#define SRSLTE_NETSOURE_HANDLER_H

#include "srslte/common/log.h"
#include "srslte/common/threads.h"
#include "srslte/phy/io/netsource.h"
#include <array>
#include <iostream>
#include <memory>

class netsource_handler : public srslte::thread
{
public:
  netsource_handler(const std::string name_) : thread(name_) { rx_buf = unique_byte_array_t(new byte_array_t); }

  ~netsource_handler(){};

  void stop()
  {
    run_enable = false;
    int cnt    = 0;
    while (running && cnt < 100) {
      usleep(10000);
      cnt++;
    }
    if (running) {
      thread_cancel();
    }
    wait_thread_finish();
  }

  virtual void run_thread() = 0;

  bool run_enable  = true;
  bool running     = false;
  bool initialized = false;

  std::string net_ip   = "0.0.0.0";
  uint32_t    net_port = 0;

  const static uint32_t                    RX_BUF_SIZE = 1024 * 1024;
  typedef std::array<uint8_t, RX_BUF_SIZE> byte_array_t;
  typedef std::unique_ptr<byte_array_t>    unique_byte_array_t;
  unique_byte_array_t                      rx_buf;

  srslte_netsource_t net_source;

  srslte::log* log = nullptr;
};

#endif // SRSLTE_NETSOURE_HANDLER_H
