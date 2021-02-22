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

#ifndef SRSUE_MUX_NR_H
#define SRSUE_MUX_NR_H

#include "srslte/common/byte_buffer.h"
#include "srslte/common/common.h"
#include "srslte/srslog/srslog.h"
#include "srslte/srslte.h"

namespace srsue {
class mux_nr
{
public:
  explicit mux_nr(srslog::basic_logger& logger);
  ~mux_nr(){};
  void reset();
  void init();

  void step();

  void msg3_flush();
  void msg3_prepare();
  void msg3_transmitted();
  bool msg3_is_transmitted();
  bool msg3_is_pending();
  bool msg3_is_empty();

private:
  srslog::basic_logger&        logger;
  srslte::unique_byte_buffer_t msg3_buff = nullptr;
  typedef enum { none, pending, transmitted } msg3_state_t;
  msg3_state_t msg3_state = none;
};
} // namespace srsue

#endif // SRSUE_MUX_NR_H
